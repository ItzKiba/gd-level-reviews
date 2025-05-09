#pragma once

#include "LRBaseLayer.hpp"

using namespace prelude;

class LRMainLayer : public LRBaseLayer {
protected:
    virtual bool init();

    void onChannelsBtn(CCObject*);
    void onRequestsBtn(CCObject*);

public:
    static LRMainLayer* create();
    static cocos2d::CCScene* scene();
};  