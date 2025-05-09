#pragma once

#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/web.hpp>
#include "../../managers/Server.hpp"
#include "../../layers/LRRequestsLayer.hpp"
#include "../../managers/RequestCache.hpp"

using namespace geode::prelude;

class FetchingLevelsPopup : public geode::Popup<> {
protected:
    bool setup();
    void onClose(CCObject*);
    EventListener<web::WebTask> m_listener;
public:
    static FetchingLevelsPopup* create();
};