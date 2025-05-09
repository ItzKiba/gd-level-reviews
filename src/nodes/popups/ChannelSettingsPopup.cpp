#include "ChannelSettingsPopup.hpp"

ChannelSettingsPopup* ChannelSettingsPopup::create() {
    auto ret = new ChannelSettingsPopup();
    if (ret->initAnchored(320.f, 180.f, "GJ_square02.png")) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ChannelSettingsPopup::setup() {
    this->setTitle("Channel Settings");
    this->setKeypadEnabled(true);
    auto size = m_mainLayer->getScaledContentSize();

    m_loadingCircle = LoadingSpinner::create(50.f);
    m_loadingCircle->setVisible(false);
    m_loadingCircle->setPosition(size / 2);
    m_mainLayer->addChild(m_loadingCircle, 10);

    auto requestsLabel = CCLabelBMFont::create("Max Requests", "bigFont.fnt");
    requestsLabel->setPosition(size / 2 + CCPoint(-50, 25));
    requestsLabel->setScale(0.75f);
    m_mainLayer->addChild(requestsLabel);

    m_textInput = TextInput::create(60, "");
    m_textInput->setFilter("0123456789");
    m_textInput->setTextAlign(TextInputAlign::Center);
    m_textInput->setCallback([this](const std::string& str) {
        m_maxRequestsInput = std::atoi(str.c_str());
    });
    std::string setInput = std::to_string(RequestCache::currentMaxRequests);
    m_textInput->setString(setInput, true);
    m_textInput->setPosition(size / 2 + CCPoint(90, 25));
    m_mainLayer->addChild(m_textInput);

    auto openLabel = CCLabelBMFont::create("Channel Open", "bigFont.fnt");
    openLabel->setPosition(size / 2 + CCPoint(-50, -15));
    openLabel->setScale(0.75f);
    m_mainLayer->addChild(openLabel);
    
    m_isOpenInput = RequestCache::currentIsOpen;
    auto openSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    auto closedSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto useSprite = closedSprite;
    if (m_isOpenInput) useSprite = openSprite;
    m_statusButton = CCMenuItemSpriteExtra::create(useSprite, this, menu_selector(ChannelSettingsPopup::onCheckboxClick));

    auto menu = CCMenu::create();
    menu->addChild(m_statusButton);
    menu->setPosition(size / 2 + CCPoint(90, -15));
    m_mainLayer->addChild(menu);

    auto submitBtn = CCMenuItemSpriteExtra::create(ButtonSprite::create("Update"), this, menu_selector(ChannelSettingsPopup::onSubmitClick));
    auto menu2 = CCMenu::create();
    menu2->addChild(submitBtn);
    menu2->setPosition({size.width / 2, 30.f});
    m_mainLayer->addChild(menu2);

    return true;
}

void ChannelSettingsPopup::onCheckboxClick(CCObject* sender) {
    auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
        if (m_isOpenInput) {
            btn->setSprite(CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"));
            m_isOpenInput = false;
        } else {
            btn->setSprite(CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"));
            m_isOpenInput = true;
        }
}

void ChannelSettingsPopup::onSubmitClick(CCObject* sender) {
    this->setKeypadEnabled(false);
    m_closeBtn->setVisible(false);
    m_loadingCircle->setVisible(true);

    m_listener.bind([this] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            if (res->code() != 200) {
                Notification::create("Failed to create/update your Request Channel.", NotificationIcon::Error)->show();
            } else {
                Notification::create("Successfully created/updated your Request Channel", NotificationIcon::Success)->show();
            }
            onClose(this);
        } else if (e->isCancelled()) {
            onClose(this);
        }
    });

    auto am = GJAccountManager::sharedState();
    auto gm = GameManager::sharedState();
    int accountID = am->m_accountID;
    int iconID = gm->getPlayerFrame();
    int color1 = gm->getPlayerColor();
    int color2 = gm->getPlayerColor2();
    int color3 = gm->getPlayerGlowColor();
    int mod_level = 0; // temporary
    std::string username = gm->m_playerName;
    int maxRequests = m_maxRequestsInput;
    bool isOpen = m_isOpenInput;
    std::string token = Server::TOKEN;

    auto req = web::WebRequest();
    auto obj = matjson::makeObject({
        {"owner_account_id", accountID},
        {"icon_id", iconID},
        {"color1", color1},
        {"color2", color2},
        {"color3", color3},
        {"mod_level", mod_level},
        {"username", username},
        {"max_requests", maxRequests},
        {"is_open", isOpen},
        {"token", token}
    });
    req.bodyJSON(obj);
    m_listener.setFilter(req.post(fmt::format("{}/channel/upsert", Server::SERVER_URL)));
}