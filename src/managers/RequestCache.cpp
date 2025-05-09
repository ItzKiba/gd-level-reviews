#include "RequestCache.hpp"

std::vector<RequestObject> RequestCache::requestCache;
int RequestCache::currentMaxRequests = 0;
bool RequestCache::currentIsOpen = false;

void RequestCache::clear() {
    requestCache.clear();
}

void RequestCache::add(RequestObject obj) {
    requestCache.push_back(obj);
}

void RequestCache::removeByID(int levelID) {
    for (int i = 0; i < requestCache.size(); i++) {
        if (requestCache.at(i).m_levelID == levelID) {
            requestCache.erase(requestCache.begin() + i);
            break;
        }
    } 
}

GJSearchObject* RequestCache::generateSearchObj() {
    std::stringstream download;
    bool first = true;

    for (auto obj : requestCache) {
        if (!first) {
            download << ",";
        }
        download << std::to_string(obj.m_levelID);
        first = false;
    }

    log::info("{}", download.str());
    GJSearchObject* searchObj = GJSearchObject::create(SearchType::Type19, download.str());
    return searchObj;
}

RequestObject RequestCache::getObjFromID(int id) {
    RequestObject ret = {-1, -1, "Unknown"};
    for (auto obj : requestCache) {
        if (obj.m_levelID == id) {
            ret = obj;
            break;
        }
    }
    return ret;
}