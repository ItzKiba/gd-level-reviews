#pragma once

#include <Geode/Geode.hpp>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include "ReviewObject.hpp"
#include "../popups/FullReviewPopup.hpp"

using namespace geode::prelude;
using namespace std::chrono;

class ReviewCell : public CCNode {
public:
    static ReviewCell* create(ReviewObject object, int index);
    ReviewObject m_reviewObject;

protected:
    static constexpr float CELL_HEIGHT = 40.f;
    static constexpr float CELL_WIDTH = 344.f;
    bool init(ReviewObject object, int index);
    std::string convertToLocalTime(std::string date);
};