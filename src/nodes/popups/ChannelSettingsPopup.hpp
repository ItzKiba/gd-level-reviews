#pragma once

#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/TextInput.hpp>
#include "../../managers/Server.hpp"
#include "../../managers/RequestCache.hpp"

using namespace geode::prelude;

class ChannelSettingsPopup : public geode::Popup<> {
protected:
    bool setup();
    void onCheckboxClick(CCObject* sender);
    void onSubmitClick(CCObject* sender);

    TextInput* m_textInput;
    CCMenuItemSpriteExtra* m_statusButton;
    int m_maxRequestsInput;
    bool m_isOpenInput;
    LoadingSpinner* m_loadingCircle;

    EventListener<web::WebTask> m_listener;

public:
    static ChannelSettingsPopup* create();
};