#include "LRMainLayer.hpp"
#include "LRChannelsLayer.hpp"
#include "LRRequestsLayer.hpp"
#include "../nodes/popups/FetchingLevelsPopup.hpp"

LRMainLayer* LRMainLayer::create() {
    auto ret = new LRMainLayer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool LRMainLayer::init() {
    if (!LRBaseLayer::init()) return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto title = CCSprite::createWithSpriteFrameName("title.png"_spr);
    title->setPosition({winSize.width / 2, winSize.height - 60.f});
    title->setScale(1.2f);
    addChild(title);

    auto channelsBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("channels_btn.png"_spr),
        this,
        menu_selector(LRMainLayer::onChannelsBtn)
    );
    auto requestsBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("requests_btn.png"_spr),
        this,
        menu_selector(LRMainLayer::onRequestsBtn)
    );

    auto menu = CCMenu::create();
    menu->setPosition(winSize / 2);
    menu->addChild(channelsBtn);
    menu->addChild(requestsBtn);
    menu->setLayout(
        RowLayout::create()
            ->setGap(20.f)
            ->setAxisAlignment(AxisAlignment::Center)
    );
    menu->updateLayout();
    addChild(menu);

    return true;
}

CCScene* LRMainLayer::scene() {
    auto layer = LRMainLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}

void LRMainLayer::onChannelsBtn(CCObject* sender) {
    auto scene = LRChannelsLayer::scene();
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
}

void LRMainLayer::onRequestsBtn(CCObject* sender) {
    FetchingLevelsPopup::create()->show();
}