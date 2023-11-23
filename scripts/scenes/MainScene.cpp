#include "MainScene.h"
#include "SimpleAudioEngine.h"

Scene* MainScene::createScene() {
    return MainScene::create();
}

bool MainScene::init() {
    _gameController = std::make_shared<GameController>();

    if ( !Scene::init() )
    {
        return false;
    }

    {
        _gameLayer = LayerGradient::create(Color4B::ORANGE, Color4B::GREEN);
        this->addChild(_gameLayer, -2);
    }

    initUI();

    drawGameField();

    return true;
}

void MainScene::drawGameField() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    int rows = getRowsFromField();
    int cols = getColsFromField();
    int countColors = getColorsFromField();

    if (_gameField != nullptr) {
        _gameField->removeAllChildrenWithCleanup(true);
        _gameField->removeFromParentAndCleanup(true);
    }
    if (_gameFieldBackground != nullptr) {
        _gameFieldBackground->removeFromParentAndCleanup(true);
    }

    _gameController->generateGrid(cols, rows, countColors, RANDOM_GENERATE_COLORS);

    float widthGameField;
    float heightGameField;
    if (rows > cols) {
        heightGameField = visibleSize.height * MAX_GAME_FIELD_SIZE_PERC;
        _cellSize = heightGameField / rows;
        widthGameField = _cellSize * cols;
    }
    else {
        widthGameField = visibleSize.width * MAX_GAME_FIELD_SIZE_PERC;
        _cellSize = widthGameField / cols;
        heightGameField = _cellSize * rows;
    }

    _gameField = Menu::create();

    {
        _gameFieldBackground = DrawNode::create();
        _gameFieldBackground->drawSolidRect(
            Vec2(0, 0),
            Vec2(widthGameField, heightGameField),
            Color4F::BLACK);
        _gameFieldBackground->setPosition(
            Vec2(visibleSize.width / 2 - widthGameField / 2, visibleSize.height / 2 - heightGameField / 2));
        _gameLayer->addChild(_gameFieldBackground);
    }

    _gameField->setPosition(
        Vec2(visibleSize.width / 2 - widthGameField / 2, visibleSize.height / 2 - heightGameField / 2));
    _gameLayer->addChild(_gameField);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            Cell cell = _gameController->getCell(j, i);
            auto sprite = Sprite::create("primitives/square.png", Rect(0, 0, _cellSize, _cellSize));
            sprite->setColor(_gameController->colorAt(cell.colorId));
            auto menuItem = MenuItemSprite::create(sprite, sprite, CC_CALLBACK_1(MainScene::onCellPressed, this));
            menuItem->setPosition(Vec2((j + 0.5f) * _cellSize, (i + 0.5f) * _cellSize));
            cell.node = menuItem;
            _gameController->setCell(j, i, cell);
            _gameField->addChild(menuItem, 1);
        }
    }
}

void MainScene::initUI() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    LayerColor* topBackgroundLayer;
    {
        topBackgroundLayer = LayerColor::create(Color4B::GRAY);
        topBackgroundLayer->setContentSize(Size(visibleSize.width, visibleSize.height * 0.1f));
        topBackgroundLayer->setPosition(0, visibleSize.height - topBackgroundLayer->getContentSize().height);
        this->addChild(topBackgroundLayer);
    }

    MenuItemSprite* textFieldHeightMenuItem;
    createTextField(TEXT_FIELD_WIDTH, TEXT_FIELD_HEIGHT, 2, std::to_string(DEFAULT_COUNT_ROWS), textFieldHeightMenuItem, _textFieldHeight);

    MenuItemSprite* textFieldWidthMenuItem;
    createTextField(TEXT_FIELD_WIDTH, TEXT_FIELD_HEIGHT, 2, std::to_string(DEFAULT_COUNT_COLS), textFieldWidthMenuItem, _textFieldWidth);

    MenuItemSprite* textFieldColorsMenuItem;
    createTextField(TEXT_FIELD_WIDTH, TEXT_FIELD_HEIGHT, 1, std::to_string(DEFAULT_COUNT_COLORS), textFieldColorsMenuItem, _textFieldColors);

    auto labelWidth = MenuItemLabel::create(Label::createWithTTF(u8"Ширина", "fonts/arial.ttf", 24));
    auto labelHeight = MenuItemLabel::create(Label::createWithTTF(u8"Высота", "fonts/arial.ttf", 24));
    auto labelColors = MenuItemLabel::create(Label::createWithTTF(u8"Цвета", "fonts/arial.ttf", 24));

    MenuItemSprite* buttonStart;
    {
        auto sprite = Sprite::create("primitives/square.png", Rect(0, 0, BUTTON_START_WIDTH, BUTTON_START_HEIGHT));

        buttonStart = MenuItemSprite::create(sprite, sprite, CC_CALLBACK_1(MainScene::onButtonPressedStart, this));
        
        auto label = Label::createWithTTF(u8"Старт", "fonts/arial.ttf", 24);
        label->setTextColor(Color4B::BLACK);
        label->setPosition(buttonStart->getContentSize() / 2);
        buttonStart->addChild(label);
    }

    _gameTopMenu = Menu::create(
        labelHeight,
        textFieldHeightMenuItem,
        labelWidth,
        textFieldWidthMenuItem,
        labelColors,
        textFieldColorsMenuItem,
        buttonStart, NULL);
    _gameTopMenu->setAnchorPoint(Vec2(0, 0));
    _gameTopMenu->setPosition(visibleSize.width / 2, visibleSize.height - topBackgroundLayer->getContentSize().height / 2);
    _gameTopMenu->alignItemsHorizontallyWithPadding(15);
    this->addChild(_gameTopMenu, 1);
}

void MainScene::createTextField(
        int width, int height, int maxTextLength,
        const std::string& placeholderText,
        MenuItemSprite*& menuItem,
        ui::TextField*& textField) {
    auto sprite = Sprite::create("primitives/square.png", Rect(0, 0, width, height));
    menuItem = MenuItemSprite::create(sprite, sprite);

    textField = ui::TextField::create(placeholderText, "fonts/arial.ttf", 24);
    textField->setTextHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    textField->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    textField->setCursorEnabled(true);
    textField->setMaxLength(maxTextLength);
    textField->setMaxLengthEnabled(true);
    textField->setTextColor(Color4B::BLACK);
    textField->setTouchAreaEnabled(true);
    textField->setTouchSize(Size(width, height));
    textField->setPosition(Vec2(
        menuItem->getContentSize().width / 2,
        menuItem->getContentSize().height / 2));

    menuItem->addChild(textField);
}

int MainScene::getRowsFromField() {
    int rows = DEFAULT_COUNT_ROWS;
    if (_textFieldHeight != nullptr) {
        std::string str = _textFieldHeight->getString();
        try {
            rows = atoi(str.c_str());
        } catch (...) {
            rows = 0;
        }
        if (rows <= 0) {
            rows = DEFAULT_COUNT_ROWS;
            _textFieldHeight->setText(std::to_string(rows));
        } else if (rows > MAX_COUNT_ROWS) {
            rows = MAX_COUNT_ROWS;
            _textFieldHeight->setText(std::to_string(rows));
        }
    }
    return rows;
}

int MainScene::getColsFromField() {
    int cols = DEFAULT_COUNT_COLS;
    if (_textFieldWidth != nullptr) {
        std::string str = _textFieldWidth->getString();
        try {
            cols = atoi(str.c_str());
        } catch (...) {
            cols = 0;
        }
        if (cols <= 0) {
            cols = DEFAULT_COUNT_COLS;
            _textFieldWidth->setText(std::to_string(cols));
        } else if (cols > MAX_COUNT_COLS) {
            cols = MAX_COUNT_COLS;
            _textFieldWidth->setText(std::to_string(cols));
        }
    }
    return cols;
}

int MainScene::getColorsFromField() {
    int colors = DEFAULT_COUNT_COLORS;
    if (_textFieldColors != nullptr) {
        std::string str = _textFieldColors->getString();
        try {
            colors = atoi(str.c_str());
        } catch (...) {
            colors = 0;
        }
        if (colors <= 0) {
            colors = DEFAULT_COUNT_COLORS;
            _textFieldColors->setText(std::to_string(colors));
        } else if (colors > MAX_COUNT_COLORS) {
            colors = MAX_COUNT_COLORS;
            _textFieldColors->setText(std::to_string(colors));
        }
    }
    return colors;
}

void MainScene::gameFinish() {
    if (_gameFinishLayer == nullptr) {
        auto visibleSize = Director::getInstance()->getVisibleSize();
        _gameFinishLayer = LayerColor::create(Color4B::GRAY);

        _labelScore = MenuItemLabel::create(Label::createWithTTF(u8"Баллы", "fonts/arial.ttf", 24));

        MenuItemSprite* buttonOk;
        {
            auto sprite = Sprite::create(
                "primitives/square.png", Rect(0, 0, GAME_FINISH_BUTTON_OK_WIDTH, GAME_FINISH_BUTTON_OK_HEIGHT));

            buttonOk = MenuItemSprite::create(sprite, sprite, CC_CALLBACK_1(MainScene::onGameFinishButtonOk, this));

            auto label = Label::createWithTTF(u8"Ok", "fonts/arial.ttf", 36);
            label->setTextColor(Color4B::BLACK);
            label->setPosition(buttonOk->getContentSize() / 2);
            buttonOk->addChild(label);
        }

        auto menu = Menu::create(_labelScore, buttonOk, NULL);
        menu->setAnchorPoint(Vec2(0, 0));
        menu->setPosition(visibleSize.width / 2, visibleSize.height / 2);
        menu->alignItemsVerticallyWithPadding(30);
        _gameFinishLayer->addChild(menu);
        this->addChild(_gameFinishLayer, 3);
    } else {
        _gameFinishLayer->setVisible(true);
    }
    _labelScore->setString(u8"Баллы: " + std::to_string(_gameController->getPlayerScore()));
    _gameLayer->setVisible(false);
    _gameTopMenu->setVisible(false);
}

void MainScene::onCellPressed(Ref* pSender) {
    MenuItemSprite* item = (MenuItemSprite*)pSender;
    int x = item->getPosition().x / _cellSize;
    int y = item->getPosition().y / _cellSize;
    _gameController->deleteCell(x, y, _cellSize);
    if (_gameController->gameIsFinish()) {
        gameFinish();
    }
}

void MainScene::onGameFinishButtonOk(Ref* pSender) {
    _gameFinishLayer->setVisible(false);
    _gameLayer->setVisible(true);
    _gameTopMenu->setVisible(true);
    drawGameField();
}

void MainScene::onButtonPressedStart(Ref* pSender) {
    drawGameField();
}
