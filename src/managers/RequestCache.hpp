#pragma once

#include "../nodes/request/RequestObject.hpp"
#include <algorithm>

using namespace geode::prelude;

class RequestCache {
protected:
    static std::vector<RequestObject> requestCache;
public:
    static void clear();
    static void add(RequestObject obj);
    static void removeByID(int levelID);
    static GJSearchObject* generateSearchObj();
    static RequestObject getObjFromID(int id);

    static int currentMaxRequests;
    static bool currentIsOpen;
};