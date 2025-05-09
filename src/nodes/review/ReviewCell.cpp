#include "ReviewCell.hpp"

ReviewCell* ReviewCell::create(ReviewObject object, int index) {
    auto ret = new ReviewCell();
    if (ret->init(object, index)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ReviewCell::init(ReviewObject object, int index) {
    if (!CCNode::init()) return false;

    // general menu
    auto menu = CCMenu::create();
    this->setContentSize({CELL_WIDTH, CELL_HEIGHT});
    menu->setContentSize({CELL_WIDTH, CELL_HEIGHT});
    menu->setPosition({0, 0});
    this->addChild(menu);

    // level name label
    auto levelLabel = CCLabelBMFont::create(fmt::format("{} ({})", object.m_levelName, object.m_levelID).c_str(), "goldFont.fnt");
    levelLabel->setScale(0.4f);
    auto levelButton = CCMenuItemExt::createSpriteExtra(levelLabel, [object](CCObject*) {
        auto search = GJSearchObject::create(SearchType::Type19, std::to_string(object.m_levelID));
        auto scene = LevelBrowserLayer::scene(search);
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
    });
    levelButton->m_scaleMultiplier = 1.1f;
    levelButton->setPosition({levelLabel->getScaledContentWidth() / 2 + 15.f, 12.f});
    menu->addChild(levelButton);

    // review preview label
    auto str = object.m_reviewString;
    if (str.length() > 28) {
        str = str.substr(0, 28);
        str.append("...");
    }
    auto previewLabel = CCLabelBMFont::create(str.c_str(), "bigFont.fnt");
    previewLabel->setScale(0.35f);
    previewLabel->setAnchorPoint({0, 0.5});
    previewLabel->setPosition({15.f, 28.f});
    this->addChild(previewLabel);

    // name label
    auto nameLabel = CCLabelBMFont::create(object.m_ownerUsername.c_str(), "goldFont.fnt");
    nameLabel->setScale(0.25f);
    nameLabel->setAnchorPoint({0, 0.5});
    auto nameButton = CCMenuItemExt::createSpriteExtra(nameLabel, [object](CCObject*) {
        ProfilePage::create(object.m_ownerAccountID, false)->show();
    });
    nameButton->m_scaleMultiplier = 1.1f;
    nameButton->setPosition({previewLabel->getPositionX() + previewLabel->getScaledContentWidth() + 20.f, previewLabel->getPositionY()});
    menu->addChild(nameButton);

    // date label
    std::string date_string = convertToLocalTime(object.m_date);
    auto dateLabel = CCLabelBMFont::create(date_string.c_str(), "chatFont.fnt");
    dateLabel->setScale(0.45f);
    dateLabel->setAnchorPoint({1, 0.5});
    dateLabel->setColor({50, 50, 50});
    dateLabel->setPosition({CELL_WIDTH - 4.f, 4.f});
    this->addChild(dateLabel);

    // view btn
    auto viewButtonSprite = ButtonSprite::create("View");
    viewButtonSprite->setScale(0.6f);
    auto viewButton = CCMenuItemExt::createSpriteExtra(viewButtonSprite, [object](CCObject*) {
       FullReviewPopup::create(object.m_reviewString.c_str())->show();
    });
    viewButton->m_scaleMultiplier = 1.1f;
    viewButton->setPosition({CELL_WIDTH - viewButtonSprite->getScaledContentWidth() / 2 - 15.f, CELL_HEIGHT / 2});
    menu->addChild(viewButton);
    
    return true;
}

// thanks google and stack overflow
std::string ReviewCell::convertToLocalTime(std::string date) {
    auto dot_pos = date.find('.');
    if (dot_pos != std::string::npos) {
        date = date.substr(0, dot_pos) + "Z";
    }

    std::tm tm = {};
    std::istringstream ss(date);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    if (ss.fail()) {
        return "Unknown";
    }

    time_t utc_time = _mkgmtime(&tm); 
    sys_seconds utc_tp = sys_seconds{seconds{utc_time}};

    zoned_time local_zoned_time{current_zone(), utc_tp}; 

    std::string local_time_str = std::format("{:%F %I:%M:%S %p %Z}", local_zoned_time);

    return local_time_str;
}