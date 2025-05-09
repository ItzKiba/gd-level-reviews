#pragma once

#include "../nodes/channel/ChannelObject.hpp"

using namespace geode::prelude;

class ChannelCache {
protected:
    static std::vector<ChannelObject> channelCache;
public:
    static void clear();
    static void add(ChannelObject obj);
    static std::vector<ChannelObject> getAllChannels();
};