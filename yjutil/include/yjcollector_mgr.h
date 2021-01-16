#ifndef YJCOLLECTOR_MGR_H
#define YJCOLLECTOR_MGR_H

#include <boost/shared_ptr.hpp>
#include <mutex>
#include <set>

using boost::shared_ptr;

namespace yjutil {
class CollectorMgr;
class Collector;
typedef shared_ptr<Collector> CollectorSp;

class CollectorMgr {
  public:
    CollectorMgr();
    ~CollectorMgr();
    void add(CollectorSp collector);
    void del(CollectorSp collector);
    void collect();

  private:
    std::set<CollectorSp> m_collectorSet;
    std::mutex m_mutex;
}; // class CollectorMgr
} // namespace yjutil

#endif // YJCOLLECTOR_MGR_H