#include "FullReviewPopup.hpp"

FullReviewPopup* FullReviewPopup::create(const char* reviewStr) {
    auto ret = new FullReviewPopup();
    if (ret->initAnchored(320.f, 180.f, reviewStr, "GJ_square02.png")) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool FullReviewPopup::setup(const char* reviewStr) {
    this->setTitle("Full Review");
    this->setKeypadEnabled(true);
    auto size = m_mainLayer->getScaledContentSize();

    auto label = MDTextArea::create(reviewStr, {280, 110});
    label->setPosition({size.width / 2, size.height / 2});
    m_mainLayer->addChild(label, 10);

    return true;
}