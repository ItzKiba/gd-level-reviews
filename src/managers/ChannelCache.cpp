#include "ChannelCache.hpp"

std::vector<ChannelObject> ChannelCache::channelCache;

void ChannelCache::clear() {
    channelCache.clear();
}

void ChannelCache::add(ChannelObject obj) {
    channelCache.push_back(obj);
}

std::vector<ChannelObject> ChannelCache::getAllChannels() {
    return channelCache;
}