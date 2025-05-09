#include "LRRequestsLayer.hpp"

LRRequestsLayer* LRRequestsLayer::create(GJSearchObject* searchObj) {
    auto ret = new LRRequestsLayer();
    if (ret->init(searchObj)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool LRRequestsLayer::init(GJSearchObject* searchObj) {
    if (!LevelBrowserLayer::init(searchObj)) return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    m_background = CCSprite::create("game_bg_05_001.png");
    m_background->setScale(winSize.width / m_background->getContentWidth() + 0.05f);
    m_background->setColor({100, 50, 255});
    m_background->setPosition(winSize / 2);
    addChild(m_background, -1);

    auto listLayer = this->getChildByID("GJListLayer");
    if (listLayer) {
        auto settingsBtn = CCMenuItemExt::createSpriteExtra(CCSprite::createWithSpriteFrameName("accountBtn_settings_001.png"), [this](CCObject*) {
            ChannelSettingsPopup::create()->show();
        });
        auto settingsMenu = CCMenu::create();
        settingsMenu->setPosition({winSize.width / 2, 30.f});
        settingsMenu->addChild(settingsBtn);
        addChild(settingsMenu, 20);
    }

    return true;
}

CCScene* LRRequestsLayer::scene(GJSearchObject* searchObj) {
    auto layer = LRRequestsLayer::create(searchObj);
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}