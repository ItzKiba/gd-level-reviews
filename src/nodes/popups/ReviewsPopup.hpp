#pragma once

#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/TextInput.hpp>
#include "../../managers/Server.hpp"
#include "../../managers/RequestCache.hpp"
#include "../review/ReviewObject.hpp"
#include "../review/ReviewCell.hpp"

using namespace geode::prelude;

class ReviewsPopup : public geode::Popup<int, const char*, bool> {
protected:
    bool setup(int, const char*, bool);
    
    LoadingSpinner* m_loadingCircle;
    EventListener<web::WebTask> m_listener;
    GJCommentListLayer* m_reviewListLayer;
    GJCommentListLayer* m_dummyListLayer;
    CCArray* m_listItems;

public:
    static ReviewsPopup* create(int, const char*, bool);
};