#include <Geode/modify/LevelCell.hpp>
#include "../managers/RequestCache.hpp"
#include "../layers/LRRequestsLayer.hpp"
#include "../layers/LRYourLevelsLayer.hpp"
#include "../nodes/popups/RemoveLevelPopup.hpp"
#include "../nodes/popups/RequestLevelPopup.hpp"
#include "../nodes/popups/AddReviewPopup.hpp"

using namespace geode::prelude;

class $modify(LR_LevelCell, LevelCell) {

    struct Fields {
        int m_submitter_account_id;
        std::string m_submitter_username;
    };

    void loadCustomLevelCell() {
        LevelCell::loadCustomLevelCell();
        this->runAction(CCSequence::createWithTwoActions(
            CCDelayTime::create(0.0f),
            CCCallFunc::create(this, callfunc_selector(LR_LevelCell::modifyCellRequestsLayer))
        ));
        this->runAction(CCSequence::createWithTwoActions(
            CCDelayTime::create(0.0f),
            CCCallFunc::create(this, callfunc_selector(LR_LevelCell::modifyCellYourLevelsLayer))
        ));
    }

    void modifyCellYourLevelsLayer() {
        // check for the correct scene
        bool found = false;
        auto parent = this->getParent();
        LRYourLevelsLayer* parentLayer;
        while (parent) {
            if (auto find = geode::cast::typeinfo_cast<LRYourLevelsLayer*>(parent)) {
                parentLayer = find;
                found = true;
                break;
            }
            parent = parent->getParent();
        }
        if (!found) return;

        auto mainmenu = m_mainMenu;
        auto viewBtn = mainmenu->getChildByIDRecursive("view-button");
        viewBtn->setVisible(false);

        auto reqSpr = ButtonSprite::create("Request");
        reqSpr->setScale(0.75f);
        auto reqBtn = CCMenuItemExt::createSpriteExtra(
            reqSpr,
            [this, parentLayer](CCObject*) {
                RequestLevelPopup::create(parentLayer->m_ownerAccountID, this->m_level->m_levelID, parentLayer->m_username, this->m_level->m_levelName.c_str())->show();
            }
        );

        reqBtn->setPosition(viewBtn->getPosition() + CCPoint(-20, 0));
        reqBtn->m_scaleMultiplier = 1.1f;
        mainmenu->addChild(reqBtn);
        
        auto percentage = m_mainLayer->getChildByIDRecursive("percentage-label");
        if (percentage) {
            percentage->setVisible(false);
        }

        CCMenuItemSpriteExtra* accBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(mainmenu->getChildByIDRecursive("creator-name"));
        if (accBtn) {
            accBtn->setEnabled(false);
        }
    }

    void modifyCellRequestsLayer() {
        // check for the correct scene
        bool found = false;
        auto parent = this->getParent();
        LRRequestsLayer* parentLayer;
        while (parent) {
            if (auto find = geode::cast::typeinfo_cast<LRRequestsLayer*>(parent)) {
                parentLayer = find;
                found = true;
                break;
            }
            parent = parent->getParent();
        }
        if (!found) return;

        auto obj = RequestCache::getObjFromID(this->m_level->m_levelID);
        m_fields->m_submitter_username = obj.m_submitter_username;
        m_fields->m_submitter_account_id = obj.m_submitter_account_id;

        auto label = CCLabelBMFont::create(fmt::format("Submitted by {}", m_fields->m_submitter_username).c_str(), "bigFont.fnt");
        label->setColor({0, 255, 0});
        label->setScale(0.2f);
        auto labelMenu = CCMenu::create();
        labelMenu->setPosition({m_width - label->getScaledContentWidth() / 2 - 5.f, 5.f});
        auto nameButton = CCMenuItemExt::createSpriteExtra(label, [this](CCObject*) {
            ProfilePage::create(m_fields->m_submitter_account_id, false)->show();
        });
        nameButton->m_scaleMultiplier = 1.1f;
        labelMenu->addChild(nameButton);
        m_mainLayer->addChild(labelMenu);

        auto mainmenu = m_mainMenu;
        auto viewBtn = mainmenu->getChildByIDRecursive("view-button");

        // adding review button
        auto reviewSpr = CCSprite::createWithSpriteFrameName("btn_chatHistory_001.png");
        reviewSpr->setScale(0.66f);
        auto reviewBtn = CCMenuItemExt::createSpriteExtra(
            reviewSpr,
            [this, parentLayer](CCObject*) {
                AddReviewPopup::create(m_fields->m_submitter_account_id, m_fields->m_submitter_username.c_str(), this->m_level->m_levelID, this->m_level->m_levelName.c_str())->show();
            }
        );
        reviewBtn->setPosition(viewBtn->getPosition() + CCPoint(-45, 0));
        mainmenu->addChild(reviewBtn);

        // adding remove button
        auto removeSpr = CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png");
        removeSpr->setScale(0.44f);
        auto removeBtn = CCMenuItemExt::createSpriteExtra(
            removeSpr,
            [this, parentLayer](CCObject*) {
                RemoveLevelPopup::create(this->m_level->m_levelID, parentLayer)->show();
            }
        );
        removeBtn->setPosition(viewBtn->getPosition() + CCPoint(-80, 0));
        mainmenu->addChild(removeBtn);

        if (m_compactView) {
            removeBtn->setPosition(removeBtn->getPosition() + CCPoint(10, 0));
        }

        auto percentage = m_mainLayer->getChildByIDRecursive("percentage-label");
        if (percentage) {
            percentage->setVisible(false);
        }

        auto completed = m_mainLayer->getChildByIDRecursive("completed-icon");
        if (completed) {
            completed->setVisible(false);
        }


    }
};