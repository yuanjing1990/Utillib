#ifndef YJCOLLECTOR_H_
#define YJCOLLECTOR_H_

#include "yjcollector_policy.h"
#include <string>
#include <vector>

namespace yjutil {
class CopyPolicy;
class CopyPolicyImpl;
class FilterPolicy;
class FilterPolicyImpl;
class RenamePolicy;

class Collector {
  public:
    virtual ~Collector() {}
    virtual bool doCollect() = 0;
};

class FileCollector : public Collector {
  public:
    FileCollector(const std::string &strSrcPath, const std::string &strDestPath,
                  CopyPolicyImpl *copyImpl = new CopyPolicyImpl(),
                  FilterPolicyImpl *filterImpl = new FilterPolicyImpl(),
                  RenamePolicyImpl *renameImpl = new RenamePolicyImpl());
    ~FileCollector();

  public:
    bool doCollect();

  private:
    bool isDirExist(const std::string &dir);
    bool getFileVec(const std::string &dir, std::vector<std::string> &fileVec);
    bool getFileVecInner(const std::string &dirRoot, const std::string &curDir,
                         std::vector<std::string> &fileVec);
    bool collectFile(std::string file);

  private:
    std::string m_srcDir;
    std::string m_destDir;
    CopyPolicy m_copyPolicy;
    FilterPolicy m_filterPolicy;
};

} // namespace yjutil

#endif // YJCOLLECTOR_H_