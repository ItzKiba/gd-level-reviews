#include "RemoveLevelPopup.hpp"

RemoveLevelPopup* RemoveLevelPopup::create(int levelID, LRRequestsLayer* parentLayer) {
    auto ret = new RemoveLevelPopup();
    if (ret->initAnchored(240.f, 160.f, levelID, parentLayer, "GJ_square02.png")) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool RemoveLevelPopup::setup(int levelID, LRRequestsLayer* parentLayer) {
    this->setTitle("Removing level...");

    auto am = GJAccountManager::sharedState();
    int accountID = am->m_accountID;

    //test
    //accountID = 1;

    auto spinner = LoadingSpinner::create(100.f);
    spinner->setPosition(m_mainLayer->getScaledContentSize() / 2);
    m_mainLayer->addChild(spinner);

    this->m_closeBtn->setVisible(false);

    this->setKeypadEnabled(false);
    // remove-level
    m_listener.bind([this, levelID, parentLayer] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            if (res->code() != 200) {
                log::error("Error (remove-level)", res->code());
                onClose(this);
                Notification::create("Failed to remove level.", NotificationIcon::Error)->show();
            } else {
                auto layer = parentLayer;
                RequestCache::removeByID(levelID);
                auto searchObj = RequestCache::generateSearchObj();
                Notification::create("Removed level.", NotificationIcon::Success)->show();
                layer->loadPage(searchObj);
                onClose(this);
            }
        } else if (e->isCancelled()) {
            onClose(this);
        }
    });

    auto req = web::WebRequest();
    auto obj = matjson::makeObject({
        {"owner_account_id", accountID},
        {"level_id", levelID},
        {"token", Server::TOKEN}
    });
    req.bodyJSON(obj);
    m_listener.setFilter(req.post(fmt::format("{}/channel/remove-level", Server::SERVER_URL)));

    return true;
}

void RemoveLevelPopup::onClose(CCObject* sender) {
    Popup::onClose(sender);
}