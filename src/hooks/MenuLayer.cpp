#include <Geode/modify/MenuLayer.hpp>
#include "../layers/LRMainLayer.hpp"

using namespace geode::prelude;

class $modify(LR_MenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto channelBtn = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::create(CCSprite::createWithSpriteFrameName("scroll.png"_spr), CircleBaseColor::Pink, CircleBaseSize::MediumAlt),
            this,
            menu_selector(LR_MenuLayer::onChannelBtn)
        );

        auto menu = this->getChildByID("bottom-menu");
        menu->addChild(channelBtn);
        menu->updateLayout();

        return true;
    }

    void onChannelBtn(CCObject* sender) {
        auto scene = LRMainLayer::scene();
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
    }
};