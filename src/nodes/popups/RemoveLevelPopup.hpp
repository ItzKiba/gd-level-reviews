#pragma once

#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/web.hpp>
#include "../../managers/Server.hpp"
#include "../../layers/LRRequestsLayer.hpp"
#include "../../managers/RequestCache.hpp"

using namespace geode::prelude;

class RemoveLevelPopup : public geode::Popup<int, LRRequestsLayer*> {
protected:
    bool setup(int, LRRequestsLayer*);
    void onClose(CCObject*);
    EventListener<web::WebTask> m_listener;
public:
    static RemoveLevelPopup* create(int, LRRequestsLayer*);
};