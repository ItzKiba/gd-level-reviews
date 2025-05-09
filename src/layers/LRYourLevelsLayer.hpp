#pragma once

#include "LRBaseLayer.hpp"

using namespace geode::prelude;
class LRYourLevelsLayer : public LevelBrowserLayer {
protected:
    virtual bool init(GJSearchObject*, int, const char*);
    CCSprite* m_background;
public:
    static LRYourLevelsLayer* create(GJSearchObject*, int, const char*);
    static cocos2d::CCScene* scene(GJSearchObject*, int, const char*);
    int m_ownerAccountID;
    const char* m_username;
};