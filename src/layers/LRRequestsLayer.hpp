#pragma once

#include "LRBaseLayer.hpp"
#include "../nodes/popups/ChannelSettingsPopup.hpp"

using namespace geode::prelude;
class LRRequestsLayer : public LevelBrowserLayer {
protected:
    virtual bool init(GJSearchObject*);
    CCSprite* m_background;
    
public:
    static LRRequestsLayer* create(GJSearchObject*);
    static cocos2d::CCScene* scene(GJSearchObject*);
    
};