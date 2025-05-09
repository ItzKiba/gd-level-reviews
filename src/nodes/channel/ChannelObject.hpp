#pragma once

struct ChannelObject {
    int m_ownerAccountID;
    std::string m_username;
    bool m_isOpen;
    int m_modLevel;
    int m_iconID;
    int m_color1;
    int m_color2;
    int m_color3;
    int m_maxRequests;
    int m_requestsCount;
};