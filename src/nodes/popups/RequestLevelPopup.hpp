#pragma once

#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/TextInput.hpp>
#include "../../managers/Server.hpp"
#include "../../managers/RequestCache.hpp"

using namespace geode::prelude;

class RequestLevelPopup : public geode::Popup<int, int, const char*, const char*> {
protected:
    bool setup(int, int, const char*, const char*);
    void onSubmitClick(CCObject* sender);

    LoadingSpinner* m_loadingCircle;
    int m_ownerAccountID;
    int m_levelID;
    int m_submitterAccountID;
    const char* m_submitterUsername;

    EventListener<web::WebTask> m_listener;

public:
    static RequestLevelPopup* create(int, int, const char*, const char*);
};