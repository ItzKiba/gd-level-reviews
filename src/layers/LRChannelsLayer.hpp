#pragma once

#include "LRBaseLayer.hpp"
#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/ListView.hpp>
#include "../managers/Server.hpp"
#include "../managers/ChannelCache.hpp"
#include "../nodes/channel/ChannelObject.hpp"
#include "../nodes/channel/ChannelCell.hpp"

using namespace geode::prelude;

class LRChannelsLayer : public LRBaseLayer {
protected:
    virtual bool init();
    CCArray* m_listItems;
    EventListener<web::WebTask> m_listener;
    int m_totalChannels;
    GJListLayer* m_channelListLayer;
    GJListLayer* m_dummyListLayer;
    ListView* m_channelListView;
public:
    static LRChannelsLayer* create();
    static cocos2d::CCScene* scene();
};  