#pragma once

#include <Geode/loader/Event.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/TextInput.hpp>
#include "../../managers/Server.hpp"

using namespace geode::prelude;

class AddReviewPopup : public geode::Popup<int, const char*, int, const char*> {
protected:
    bool setup(int, const char*, int, const char*);
    void onSubmitClick(CCObject* sender);

    int m_requesterAccountID;
    const char* m_requesterUsername;
    int m_levelID;
    const char* m_levelName;

    CCTextInputNode* m_textInput;
    EventListener<web::WebTask> m_listener;

public:
    static AddReviewPopup* create(int, const char*, int, const char*);
};