#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class LRBaseLayer : public CCLayer {
protected:
    virtual bool init();
    virtual void keyBackClicked();
    void onBack(CCObject*);

    cocos2d::CCSprite* m_background;

public:
    static LRBaseLayer* create();
    static cocos2d::CCScene* scene();
};