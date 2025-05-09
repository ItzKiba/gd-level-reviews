#include "AddReviewPopup.hpp"

AddReviewPopup* AddReviewPopup::create(int requesterID, const char* requesterUsername, int levelID, const char* levelName) {
    auto ret = new AddReviewPopup();
    if (ret->initAnchored(360.f, 240.f, requesterID, requesterUsername, levelID, levelName, "GJ_square02.png")) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool AddReviewPopup::setup(int requesterID, const char* requesterUsername, int levelID, const char* levelName) {
    this->setTitle("Add Review");
    this->setKeypadEnabled(true);
    auto size = m_mainLayer->getScaledContentSize();

    auto smallText = CCLabelBMFont::create("(Max 500 characters)", "chatFont.fnt");
    smallText->setOpacity(128);
    smallText->setScale(0.5f);
    smallText->setPosition(m_title->getPosition() + CCPoint(0, -12));
    m_mainLayer->addChild(smallText);

    auto backTexture = CCScale9Sprite::create("square02_001.png");
    backTexture->setContentSize({280.f, 140.f});
    backTexture->setPosition(size / 2 + CCPoint(0, 10));
    backTexture->setOpacity(100);
    m_mainLayer->addChild(backTexture);

    m_textInput = CCTextInputNode::create(320.f, 180.f, "Write your review...", 5, nullptr);
    m_textInput->setScaledContentSize({320.f, 180.f});
    m_textInput->setScale(0.75f);
    m_textInput->setAllowedChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()-_=+/?.>,<:;| ");
    auto textArea = TextArea::create("Write your review...", "chatFont.fnt", 1.f, 320.f, {0.5, 0.5}, 15.f, true);
    m_textInput->addTextArea(textArea);
    m_textInput->setLabelPlaceholderColor({165, 165, 165});
    m_textInput->setPosition({140, 110});
    m_buttonMenu->addChild(m_textInput, 50);

    auto submitBtn = CCMenuItemSpriteExtra::create(ButtonSprite::create("Submit"), this, menu_selector(AddReviewPopup::onSubmitClick));
    auto menu2 = CCMenu::create();
    menu2->addChild(submitBtn);
    menu2->setPosition({size.width / 2, 30.f});
    m_mainLayer->addChild(menu2);

    m_requesterAccountID = requesterID;
    m_requesterUsername = requesterUsername;
    m_levelID = levelID;
    m_levelName = levelName;

    return true;
}

void AddReviewPopup::onSubmitClick(CCObject* sender) {
    this->setKeypadEnabled(false);
    m_closeBtn->setVisible(false);

    m_listener.bind([this] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            if (res->code() != 200) {
                Notification::create(fmt::format("Error {}: Failed to submit your review", res->code()), NotificationIcon::Error)->show();
            } else {
                Notification::create(fmt::format("Submitted your level review for {}", this->m_levelName), NotificationIcon::Success)->show();
            }
            onClose(this);
        } else if (e->isCancelled()) {
            onClose(this);
        }
    });

    auto am = GJAccountManager::sharedState();
    auto gm = GameManager::sharedState();

    int accountID = am->m_accountID;
    std::string username = gm->m_playerName;

    std::string token = Server::TOKEN;

    auto req = web::WebRequest();
    auto obj = matjson::makeObject({
        {"owner_account_id", accountID},
        {"owner_username", username},
        {"requester_account_id", m_requesterAccountID},
        {"requester_username", m_requesterUsername},
        {"level_id", m_levelID},
        {"level_name", m_levelName},
        {"review_string", this->m_textInput->getString().c_str()},
        {"token", token}
    });
    req.bodyJSON(obj);
    m_listener.setFilter(req.post(fmt::format("{}/review/create-review", Server::SERVER_URL)));
}