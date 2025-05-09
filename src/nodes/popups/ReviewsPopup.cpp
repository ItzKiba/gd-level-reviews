#include "ReviewsPopup.hpp"

ReviewsPopup* ReviewsPopup::create(int id, const char* name, bool type) {
    auto ret = new ReviewsPopup();
    if (ret->initAnchored(380.f, 280.f, id, name, type, "GJ_square01.png")) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ReviewsPopup::setup(int id, const char* name, bool type) {
    this->setTitle(fmt::format("Level Reviews {} {}", type ? "of" : "by", name));
    this->setKeypadEnabled(true);
    auto size = m_mainLayer->getScaledContentSize();

    m_loadingCircle = LoadingSpinner::create(50.f);
    m_loadingCircle->setVisible(true);
    m_loadingCircle->setPosition(size / 2);
    m_mainLayer->addChild(m_loadingCircle, 10);

    auto dummyListView = ListView::create(CCArray::create(), 40.f, 344.f, 220.f);
    m_dummyListLayer = GJCommentListLayer::create(dummyListView, "", {0, 0, 0}, 341.25f, 220.f, false);
    m_dummyListLayer->setPosition({20, 20});
    m_mainLayer->addChild(m_dummyListLayer);

    // get reviews
    m_listener.bind([this, size] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            if (res->code() != 200) {
                log::error("Error (getting reviews)", res->code());
                Notification::create("Failed to pull reviews.", NotificationIcon::Error)->show();
            } else {
                auto json = res->json().unwrap();
                log::info("{}", json.dump());
                
                auto dataArray = json["data"].asArray();
                if (dataArray.isErr()) {
                    //log::error("Error (get-channels-page) [2]", res->code());
                    Notification::create("Failed to pull reviews.", NotificationIcon::Error)->show();
                    return;
                }
                
                auto total = json["total"].asInt();
                if (!total.isErr()) {
                    auto totalLabel = CCLabelBMFont::create(fmt::format("Total: {}", total.unwrapOr(-1)).c_str(), "goldFont.fnt");
                    totalLabel->setScale(0.5f);
                    totalLabel->setPosition({size.width / 2, 13.5});
                    m_mainLayer->addChild(totalLabel, 50);
                }

                m_listItems = CCArray::create();
                int i = 0;
                for (auto review : dataArray.unwrap()) {
                    int owner_account_id = review["owner_account_id"].asInt().unwrapOr(-1);
                    std::string owner_username = review["owner_username"].asString().unwrapOr("Unknown");
                    int requester_account_id = review["requester_account_id"].asInt().unwrapOr(-1);
                    std::string requester_username = review["requester_username"].asString().unwrapOr("Unknown");
                    int level_id = review["level_id"].asInt().unwrapOr(-1);
                    std::string level_name = review["level_name"].asString().unwrapOr("Unknown");
                    std::string review_string = review["review_string"].asString().unwrapOr("Unknown");
                    std::string date_string = review["date"].asString().unwrapOr("Unknown");

                    ReviewObject obj = {owner_account_id, owner_username.c_str(), requester_account_id, requester_username.c_str(), level_id, level_name.c_str(), review_string.c_str(), date_string.c_str()};
                    auto cell = ReviewCell::create(obj, i);
                    m_listItems->addObject(cell);
                    i++;
                }

                auto winSize = CCDirector::sharedDirector()->getWinSize();
                auto newView = ListView::create(m_listItems, 40.f, 344.f, 220.f);
                m_reviewListLayer = GJCommentListLayer::create(newView, "", {0, 0, 0}, 341.25f, 220.f, false);
                m_reviewListLayer->setPosition({20, 20});
                m_mainLayer->addChild(m_reviewListLayer);
                m_dummyListLayer->removeFromParent();

                m_loadingCircle->setVisible(false);

            }
        } else if (e->isCancelled()) {
            // nothing
        }
    });

    if (!type) {
        auto req = web::WebRequest();
        auto obj = matjson::makeObject({{"page", 1}, {"account_id", id}}); // just page 1 for now
        req.bodyJSON(obj);
        m_listener.setFilter(req.post(fmt::format("{}/review/get-reviews-written-by-user", Server::SERVER_URL)));
    } else {
        auto req = web::WebRequest();
        auto obj = matjson::makeObject({{"page", 1}, {"level_id", id}}); // just page 1 for now
        req.bodyJSON(obj);
        m_listener.setFilter(req.post(fmt::format("{}/review/get-reviews-of-level", Server::SERVER_URL)));
    }
    

    return true;
}