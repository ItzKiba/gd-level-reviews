#pragma once

#include <string>

struct ReviewObject {
    int m_ownerAccountID;
    std::string m_ownerUsername;
    int m_requesterAccountID;
    std::string m_requesterUsername;
    int m_levelID;
    std::string m_levelName;
    std::string m_reviewString;
    std::string m_date;
};