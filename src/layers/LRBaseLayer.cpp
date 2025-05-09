#include "LRBaseLayer.hpp"

LRBaseLayer* LRBaseLayer::create() {
    auto ret = new LRBaseLayer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool LRBaseLayer::init() {
    if (!CCLayer::init()) return false;
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    setKeypadEnabled(true);

    m_background = CCSprite::create("game_bg_05_001.png");
    m_background->setScale(winSize.width / m_background->getContentWidth() + 0.05f);
    m_background->setColor({100, 50, 255});
    m_background->setPosition(winSize / 2);
    addChild(m_background, -2);

    auto backBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"),
        this,
        menu_selector(LRBaseLayer::onBack)
    );
    backBtn->setSizeMult(1.2f);
    auto menuBack = CCMenu::create();
    menuBack->addChild(backBtn, 5);
    menuBack->setPosition({24, winSize.height - 23});
    addChild(menuBack, 5);

    addSideArt(this, geode::SideArt::All);

    return true;
}

CCScene* LRBaseLayer::scene() {
    auto layer = LRBaseLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}

void LRBaseLayer::onBack(CCObject* sender) {
    keyBackClicked();
}

void LRBaseLayer::keyBackClicked() {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}