#include "yjcollector_mgr.h"
#include "yjcollector.h"
#include "yjutil.h"
#include <algorithm>

namespace yjutil {
CollectorMgr::CollectorMgr() {}

CollectorMgr::~CollectorMgr() {}

void CollectorMgr::add(CollectorSp collector) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_collectorSet.insert(collector);
}

void CollectorMgr::del(CollectorSp collector) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_collectorSet.erase(collector);
}

void CollectorMgr::collect() {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto e : m_collectorSet) {
        e->doCollect();
    }
}

} // namespace yjutil
