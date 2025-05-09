#include "LRYourLevelsLayer.hpp"

LRYourLevelsLayer* LRYourLevelsLayer::create(GJSearchObject* searchObj, int ownerAccountID, const char* username) {
    auto ret = new LRYourLevelsLayer();
    if (ret->init(searchObj, ownerAccountID, username)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool LRYourLevelsLayer::init(GJSearchObject* searchObj, int ownerAccountID, const char* username) {
    if (!LevelBrowserLayer::init(searchObj)) return false;

    m_ownerAccountID = ownerAccountID;
    m_username = username;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    m_background = CCSprite::create("game_bg_05_001.png");
    m_background->setScale(winSize.width / m_background->getContentWidth() + 0.05f);
    m_background->setColor({100, 50, 255});
    m_background->setPosition(winSize / 2);
    addChild(m_background, -1);

    auto requestToLabel = CCLabelBMFont::create(fmt::format("Request level to {}", username).c_str(), "goldFont.fnt");
    requestToLabel->setPosition({winSize.width / 2, -30.f});
    addChild(requestToLabel, 10);

    return true;
}

CCScene* LRYourLevelsLayer::scene(GJSearchObject* searchObj, int ownerAccountID, const char* username) {
    auto layer = LRYourLevelsLayer::create(searchObj, ownerAccountID, username);
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}