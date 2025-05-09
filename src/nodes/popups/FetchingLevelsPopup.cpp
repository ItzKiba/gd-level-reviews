#include "FetchingLevelsPopup.hpp"

FetchingLevelsPopup* FetchingLevelsPopup::create() {
    auto ret = new FetchingLevelsPopup();
    if (ret->initAnchored(240.f, 160.f, "GJ_square05.png")) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool FetchingLevelsPopup::setup() {
    this->setTitle("Fetching levels...");

    auto am = GJAccountManager::sharedState();
    int accountID = am->m_accountID;

    //test
    //accountID = 1;

    auto spinner = LoadingSpinner::create(100.f);
    spinner->setPosition(m_mainLayer->getScaledContentSize() / 2);
    m_mainLayer->addChild(spinner);

    this->m_closeBtn->setVisible(false);

    this->setKeypadEnabled(false);
    // get-channel-levels
    m_listener.bind([this] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            if (res->code() != 200) {
                log::error("Error (get-channel)", res->code());
                onClose(this);
                Notification::create("Failed to pull your requests.", NotificationIcon::Error)->show();
            } else {
                auto json = res->json().unwrap();
                log::info("{}", json.dump());
                
                if (json["data"].contains("is_open")) {
                    log::info("found is open");
                    RequestCache::currentIsOpen = json["data"]["is_open"].asBool().unwrapOr(false);
                }
                if (json["data"].contains("max_requests")) {
                    log::info("found max requests");
                    RequestCache::currentMaxRequests = json["data"]["max_requests"].asInt().unwrapOr(0);
                }

                // user has no requests
                if (!json["data"].contains("levels")) {
                    onClose(this);
                    Notification::create("You currently don't have any requests.", NotificationIcon::Info)->show();
                    RequestCache::clear();
                } else {
                    auto jsonArray = json["data"]["levels"].asArray();
                    if (jsonArray.isErr()) return;

                    RequestCache::clear();
                    for (auto level : jsonArray.unwrap()) {
                        log::info("{}, {}, {}", level["level_id"].asInt().unwrapOr(-1), level["submitter_username"].asString().unwrapOr("-1"), level["submitter_account_id"].asInt().unwrapOr(-1));
                        int level_id = (int)(level["level_id"].asInt().unwrapOr(-1));
                        int submitter_account_id = (int)(level["submitter_account_id"].asInt().unwrapOr(-1));
                        std::string submitter_username = level["submitter_username"].asString().unwrapOr("-1");

                        if (level_id == -1 || submitter_account_id == -1 || submitter_username == "-1") continue;
                        RequestCache::add({level_id, submitter_account_id, submitter_username});
                    }
                    log::info("size of jsonArray: {}", jsonArray.unwrap().size());

                    onClose(this);
                    Notification::create("Successfully pulled your requests!", NotificationIcon::Success)->show();
                }
            }
        } else if (e->isCancelled()) {
            onClose(this);
        }
    });

    auto req = web::WebRequest();
    auto obj = matjson::makeObject({{"owner_account_id", accountID}});
    req.bodyJSON(obj);
    m_listener.setFilter(req.post(fmt::format("{}/channel/get-channel", Server::SERVER_URL)));

    return true;
}

void FetchingLevelsPopup::onClose(CCObject* sender) {
    Popup::onClose(sender);
    auto search = RequestCache::generateSearchObj();
    auto scene = LRRequestsLayer::scene(search);
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
}