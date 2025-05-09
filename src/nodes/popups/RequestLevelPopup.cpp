#include "RequestLevelPopup.hpp"

RequestLevelPopup* RequestLevelPopup::create(int ownerAccountID, int levelID, const char* ownerUsername, const char* levelName) {
    auto ret = new RequestLevelPopup();
    if (ret->initAnchored(320.f, 180.f, ownerAccountID, levelID, ownerUsername, levelName, "GJ_square02.png")) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool RequestLevelPopup::setup(int ownerAccountID, int levelID, const char* ownerUsername, const char* levelName) {
    this->setTitle(fmt::format("Request level to {}", ownerUsername).c_str());
    this->setKeypadEnabled(true);
    auto size = m_mainLayer->getScaledContentSize();

    m_ownerAccountID = ownerAccountID;
    m_levelID = levelID;
    m_submitterUsername = GJAccountManager::sharedState()->m_username.c_str();
    m_submitterAccountID = GJAccountManager::sharedState()->m_accountID;

    m_loadingCircle = LoadingSpinner::create(50.f);
    m_loadingCircle->setVisible(false);
    m_loadingCircle->setPosition(size / 2);
    m_mainLayer->addChild(m_loadingCircle, 10);

    this->m_closeBtn->setVisible(false);
    
    auto label = MDTextArea::create(fmt::format("Would you like to request your level <cg>{}</c> to <cj>{}</c>'s Request Channel?\n\n<cr>Note: You can only submit one level to any given Request Channel at a time.</c>", levelName, ownerUsername).c_str(), {280, 80});
    label->setPosition({size.width / 2, size.height / 2 + 10.f});
    m_mainLayer->addChild(label, 10);

    auto menu = CCMenu::create();
    menu->setPosition({size.width / 2, 30.f});
    menu->setLayout(RowLayout::create()->setGap(20.f));

    auto submitBtn = CCMenuItemSpriteExtra::create(ButtonSprite::create("Submit"), this, menu_selector(RequestLevelPopup::onSubmitClick));

    auto cancelBtn = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Cancel"), [this](CCObject*) {
        onClose(this);
    });

    menu->addChild(cancelBtn);
    menu->addChild(submitBtn);
    menu->updateLayout();
    m_mainLayer->addChild(menu);

    return true;
}

void RequestLevelPopup::onSubmitClick(CCObject* sender) {
    this->setKeypadEnabled(false);
    m_closeBtn->setVisible(false);
    m_loadingCircle->setVisible(true);

    m_listener.bind([this] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            if (res->code() != 200) {
                auto json = res->json().unwrap();
                if (json.contains("message")) {
                    Notification::create(fmt::format("Error {}: {}", res->code(), json["message"].asString().unwrapOr("")), NotificationIcon::Error)->show();
                }
            } else {
                Notification::create("Your level has been submitted!", NotificationIcon::Success)->show();
            }
            onClose(this);
        } else if (e->isCancelled()) {
            onClose(this);
        }
    });

    std::string token = Server::TOKEN;

    auto req = web::WebRequest();
    auto obj = matjson::makeObject({
        {"owner_account_id", m_ownerAccountID},
        {"level_id", m_levelID},
        {"submitter_account_id", m_submitterAccountID},
        {"submitter_username", m_submitterUsername},
        {"token", token}
    });
    req.bodyJSON(obj);
    m_listener.setFilter(req.post(fmt::format("{}/channel/add-level", Server::SERVER_URL)));
}