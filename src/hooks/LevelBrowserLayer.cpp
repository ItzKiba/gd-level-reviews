#include <Geode/modify/LevelBrowserLayer.hpp>
#include "../layers/LRYourLevelsLayer.hpp"

class $modify(LRYourLevelsLayerHook, LevelBrowserLayer) {
    void loadPage(GJSearchObject* searchObj) {
        if (auto find = geode::cast::typeinfo_cast<LRYourLevelsLayer*>(this)) {
            auto gm = GameManager::sharedState();
            auto userID = gm->m_playerUserID;
            gm->m_playerUserID = 0;

            LevelBrowserLayer::loadPage(searchObj);

            gm->m_playerUserID = userID;
        } else {
            LevelBrowserLayer::loadPage(searchObj);
        }
    }
};