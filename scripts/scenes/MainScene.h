#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <iostream>
#include "../model/GameController.h"

USING_NS_CC;

class MainScene : public Scene
{
public:
    static Scene* createScene();

    virtual bool init();
    
    void onButtonPressedStart(Ref* pSender);
    void onCellPressed(Ref* pSender);
    void onGameFinishButtonOk(Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(MainScene);

private:
    std::shared_ptr<GameController> _gameController;
    ui::TextField* _textFieldHeight;
    ui::TextField* _textFieldWidth;
    ui::TextField* _textFieldColors;
    DrawNode* _gameFieldBackground;
    LayerGradient* _gameLayer;
    LayerColor* _gameFinishLayer;
    MenuItemLabel* _labelScore;
    Menu* _gameField;
    Menu* _gameTopMenu;
    float _cellSize;

    const int DEFAULT_COUNT_ROWS = 10;
    const int DEFAULT_COUNT_COLS = 16;
    const int DEFAULT_COUNT_COLORS = 3;
    const int MAX_COUNT_ROWS = 32;
    const int MAX_COUNT_COLS = 32;
    const int MAX_COUNT_COLORS = 9;
    const int RANDOM_GENERATE_COLORS = false;
    const float MAX_GAME_FIELD_SIZE_PERC = 0.6f;
    const int TEXT_FIELD_WIDTH = 100;
    const int TEXT_FIELD_HEIGHT = 50;
    const int BUTTON_START_WIDTH = 100;
    const int BUTTON_START_HEIGHT = 50;
    const int GAME_FINISH_BUTTON_OK_WIDTH = 300;
    const int GAME_FINISH_BUTTON_OK_HEIGHT = 100;

    void initUI();
    void drawGameField();

    int getRowsFromField();
    int getColsFromField();
    int getColorsFromField();

    void gameFinish();

    void createTextField(
        int width,
        int height,
        int maxTextLength,
        const std::string& placeholderText, 
        MenuItemSprite*& menuItem, 
        ui::TextField*& textField);
};

#endif // __MAIN_SCENE_H__
