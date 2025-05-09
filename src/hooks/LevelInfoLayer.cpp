#include <Geode/modify/LevelInfoLayer.hpp>
#include "../nodes/popups/ReviewsPopup.hpp"

class $modify(LR_LevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        auto leftmenu = this->getChildByIDRecursive("left-side-menu");
        if (!leftmenu) return true;

        auto reviewsBtn = CCMenuItemExt::createSpriteExtra(
            CircleButtonSprite::create(CCSprite::createWithSpriteFrameName("scroll.png"_spr), CircleBaseColor::Cyan, CircleBaseSize::Medium),
            [this, level](CCObject*) {
                auto popup = ReviewsPopup::create(level->m_levelID, level->m_levelName.c_str(), true);
                popup->m_noElasticity = true;
                popup->show();
            }
        );
        leftmenu->addChild(reviewsBtn);
        leftmenu->updateLayout();

        return true;
    }
};