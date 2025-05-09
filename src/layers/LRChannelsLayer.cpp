#include "LRChannelsLayer.hpp"

LRChannelsLayer* LRChannelsLayer::create() {
    auto ret = new LRChannelsLayer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool LRChannelsLayer::init() {
    if (!LRBaseLayer::init()) return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto dummyListView = ListView::create(CCArray::create(), 40.f, 358.f);
    auto dummyListLayer = GJListLayer::create(dummyListView, "Request Channels", {0, 0, 0}, 358.f, 220.f, false);
    dummyListLayer->setColor({191, 114, 62});
    dummyListLayer->setOpacity(255);
    if (auto bottom = dummyListLayer->getChildByID("bottom-border")) bottom->setPositionY(bottom->getPositionY() - 5.f);
    dummyListLayer->setPosition((winSize / 2) - (dummyListLayer->getContentSize() / 2));
    m_dummyListLayer = dummyListLayer;
    addChild(m_dummyListLayer);

    // get-channels-page
    m_listener.bind([this] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            if (res->code() != 200) {
                log::error("Error (get-channels-page)", res->code());
                Notification::create("Failed to pull request channels.", NotificationIcon::Error)->show();
            } else {
                auto json = res->json().unwrap();
                log::info("{}", json.dump());
                
                auto dataArray = json["data"].asArray();
                if (dataArray.isErr()) {
                    log::error("Error (get-channels-page) [2]", res->code());
                    Notification::create("Failed to pull request channels.", NotificationIcon::Error)->show();
                    return;
                }

                ChannelCache::clear();

                for (auto channel : dataArray.unwrap()) {
                    int owner_account_id = channel["owner_account_id"].asInt().unwrapOr(-1);
                    std::string username = channel["username"].asString().unwrapOr("Unknown");
                    bool is_open = channel["is_open"].asBool().unwrapOr(false);
                    int mod_level = channel["mod_level"].asInt().unwrapOr(-1);
                    int icon_id = channel["icon_id"].asInt().unwrapOr(0);
                    int color1 = channel["color1"].asInt().unwrapOr(0);
                    int color2 = channel["color2"].asInt().unwrapOr(0);
                    int color3 = channel["color3"].asInt().unwrapOr(-1);
                    int max_requests = channel["max_requests"].asInt().unwrapOr(0);
                    int level_count = channel["level_count"].asInt().unwrapOr(0);

                    ChannelCache::add({owner_account_id, username, is_open, mod_level, icon_id, color1, color2, color3, max_requests, level_count});
                }

                auto channels = ChannelCache::getAllChannels();
                m_listItems = CCArray::create();
                for (int i = 0; i < channels.size(); i++) {
                    auto cell = ChannelCell::create(channels.at(i), i);
                    m_listItems->addObject(cell);
                }
                auto winSize = CCDirector::sharedDirector()->getWinSize();
                m_channelListView = ListView::create(m_listItems, 40.f, 358.f);
                m_channelListLayer = GJListLayer::create(m_channelListView, "Request Channels", {0, 0, 0}, 358.f, 220.f, false);
                m_channelListLayer->setColor({191, 114, 62});
                m_channelListLayer->setOpacity(255);
                if (auto bottom = m_channelListLayer->getChildByID("bottom-border")) bottom->setPositionY(bottom->getPositionY() - 5.f);
                m_channelListLayer->setPosition((winSize / 2) - (m_channelListLayer->getContentSize() / 2));
                addChild(m_channelListLayer);
                m_dummyListLayer->removeFromParent();
            }
        } else if (e->isCancelled()) {
            // nothing
        }
    });

    auto req = web::WebRequest();
    auto obj = matjson::makeObject({{"page", 1}}); // just page 1 for now
    req.bodyJSON(obj);
    m_listener.setFilter(req.post(fmt::format("{}/channel/get-channels-page", Server::SERVER_URL)));

    return true;
}

CCScene* LRChannelsLayer::scene() {
    auto layer = LRChannelsLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}