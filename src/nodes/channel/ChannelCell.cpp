#include "ChannelCell.hpp"

bool ChannelCell::init(ChannelObject object, int index) {
    if (!CCNode::init()) return false;

    auto gm = GameManager::sharedState();
    auto yourUserID = gm->m_playerUserID.value();
    auto yourAccountID = GJAccountManager::sharedState()->m_accountID;
    
    // general menu
    auto menu = CCMenu::create();
    menu->setContentSize({CELL_WIDTH, CELL_HEIGHT});
    menu->setPosition({0, 0});
    this->addChild(menu);

    // name label
    auto nameLabel = CCLabelBMFont::create(object.m_username.c_str(), "bigFont.fnt");
    nameLabel->setScale(0.6f);
    auto nameButton = CCMenuItemExt::createSpriteExtra(nameLabel, [object](CCObject*) {
        ProfilePage::create(object.m_ownerAccountID, false)->show();
    });
    nameButton->m_scaleMultiplier = 1.1f;
    nameButton->setPosition({CELL_HEIGHT + nameLabel->getScaledContentWidth() / 2 + 2.f, CELL_HEIGHT / 2 + 1.f});
    menu->addChild(nameButton);

    // player icon
    auto simplePlayer = SimplePlayer::create(0);
    simplePlayer->updatePlayerFrame(object.m_iconID, IconType::Cube);
    simplePlayer->setColors(gm->colorForIdx(object.m_color1), gm->colorForIdx(object.m_color2));
    if (object.m_color3 == -1) {
        simplePlayer->disableGlowOutline();
    } else {
        simplePlayer->setGlowOutline(gm->colorForIdx(object.m_color3));
    }
    simplePlayer->setPosition({CELL_HEIGHT / 2 + 2.f, CELL_HEIGHT / 2});
    simplePlayer->setScale(0.75f);
    this->addChild(simplePlayer);

    // badge
    if (object.m_modLevel > 0) {
        CCSprite* badge = nullptr;
        if (object.m_modLevel == 1) badge = CCSprite::createWithSpriteFrameName("modBadge_01_001.png");
        if (object.m_modLevel == 2) badge = CCSprite::createWithSpriteFrameName("modBadge_02_001.png");

        if (badge != nullptr) {
            badge->setScale(0.85f);
            badge->setPosition({nameButton->getPositionX() + nameButton->getScaledContentWidth() / 2 + 17.f, CELL_HEIGHT / 2});
            this->addChild(badge);
        }
    }

    // submit btn
    auto submitButtonSprite = ButtonSprite::create("Submit");
    submitButtonSprite->setScale(0.6f);
    auto submitButton = CCMenuItemExt::createSpriteExtra(submitButtonSprite, [object, yourAccountID, yourUserID](CCObject*) {
        auto searchObj = GJSearchObject::create(SearchType::UsersLevels, std::to_string(yourUserID).c_str());

        auto gm = GameManager::sharedState();
        gm->m_playerUserID = 0;
        auto scene = LRYourLevelsLayer::scene(searchObj, yourAccountID, object.m_username.c_str());
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
        gm->m_playerUserID = yourUserID;
    });
    submitButton->m_scaleMultiplier = 1.1f;
    submitButton->setPosition({CELL_WIDTH - submitButtonSprite->getScaledContentWidth() / 2 - 10.f, CELL_HEIGHT / 2 + 2.5f});
    if (!object.m_isOpen || object.m_requestsCount >= object.m_maxRequests) {
        submitButton->setEnabled(false);
        submitButtonSprite->setColor({100, 100, 100});
    }
    menu->addChild(submitButton);

    // review btn
    auto reviewSpr = CircleButtonSprite::create(CCSprite::createWithSpriteFrameName("scroll.png"_spr), CircleBaseColor::Cyan, CircleBaseSize::MediumAlt);
    reviewSpr->setScale(0.5f);
    auto reviewsBtn = CCMenuItemExt::createSpriteExtra(
        reviewSpr,
        [this, object](CCObject*) {
            auto popup = ReviewsPopup::create(object.m_ownerAccountID, object.m_username.c_str(), false);
            popup->m_noElasticity = true;
            popup->show();
        }
    );
    reviewsBtn->setPosition({CELL_WIDTH - submitButtonSprite->getScaledContentWidth() - 30.f, CELL_HEIGHT / 2});
    reviewsBtn->m_scaleMultiplier = 1.1f;
    menu->addChild(reviewsBtn);

    // number of requests label
    auto requestLabelText = fmt::format("{}/{}", object.m_requestsCount, object.m_maxRequests);
    auto requestLabel = CCLabelBMFont::create(requestLabelText.c_str(), "goldFont.fnt");
    requestLabel->setScale(0.5f);
    requestLabel->setPosition({CELL_WIDTH - submitButtonSprite->getScaledContentWidth() - 50.f, CELL_HEIGHT / 2});
    requestLabel->setAnchorPoint({1.0, 0.5});
    this->addChild(requestLabel);

    this->setAnchorPoint({0, 0.5});
    this->setContentSize({CELL_WIDTH, CELL_HEIGHT});

    // status label
    std::string statusLabelText = "Unknown";
    ccColor3B statusColor = {255, 255, 255};
    if (object.m_isOpen && object.m_requestsCount < object.m_maxRequests) {
        statusLabelText = "Open";
        statusColor = {0, 255, 0};
    } else {
        statusLabelText = "Closed";
        statusColor = {255, 50, 50};
    }
    auto statusLabel = CCLabelBMFont::create(statusLabelText.c_str(), "bigFont.fnt");
    statusLabel->setScale(0.3f);
    statusLabel->setColor(statusColor);
    statusLabel->setPosition(submitButton->getPosition() - CCPoint({0, 15.f}));
    this->addChild(statusLabel);

    return true;
}

ChannelCell* ChannelCell::create(ChannelObject object, int index) {
    auto ret = new ChannelCell();
    if (ret->init(object, index)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}