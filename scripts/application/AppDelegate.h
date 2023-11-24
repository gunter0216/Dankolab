#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"
#include "../scenes/MainScene.h"

USING_NS_CC;

class  AppDelegate : private Application {
public:
    const Size APP_SIZE = Size(1000, 800);
    const int FPS = 60;

    AppDelegate();
    virtual ~AppDelegate();

    virtual void initGLContextAttrs();

    /**
    @brief    Implement Director and Scene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  Called when the application moves to the background
    @param  the pointer of the application
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  Called when the application reenters the foreground
    @param  the pointer of the application
    */
    virtual void applicationWillEnterForeground();

    void openConsole();
};

#endif // _APP_DELEGATE_H_
