#pragma once

#include <Geode/ui/TextInput.hpp>
#include "../../managers/Server.hpp"
#include "../../managers/RequestCache.hpp"

using namespace geode::prelude;

class FullReviewPopup : public geode::Popup<const char*> {
protected:
    bool setup(const char*);

public:
    static FullReviewPopup* create(const char*);
};