#pragma once

#include <Geode/Geode.hpp>
#include "ChannelObject.hpp"
#include "../../layers/LRYourLevelsLayer.hpp"
#include "../popups/ReviewsPopup.hpp"

using namespace geode::prelude;

class ChannelCell : public CCNode {
public:
    static ChannelCell* create(ChannelObject object, int index);

protected:
    static constexpr float CELL_HEIGHT = 40.f;
    static constexpr float CELL_WIDTH = 354.f;
    bool init(ChannelObject object, int index);
};