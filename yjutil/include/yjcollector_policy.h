#ifndef YJCOLLECTOR_POLICY_H_
#define YJCOLLECTOR_POLICY_H_

#include "yjutil.h"
#include <memory>

namespace yjutil {

class CopyPolicyImpl;
class FilterPolicyImpl;
class RenamePolicyImpl;
typedef std::shared_ptr<CopyPolicyImpl> CopyPolicyImplSp;
typedef std::shared_ptr<FilterPolicyImpl> FilterPolicyImplSp;
typedef std::shared_ptr<RenamePolicyImpl> RenamePolicyImplSp;

class RenamePolicy {
  private:
    RenamePolicyImplSp m_impl;

  public:
    RenamePolicy(RenamePolicyImpl *renamePolicy);
    std::string rename(const std::string &src);
};

class CopyPolicy {
  private:
    CopyPolicyImplSp m_impl;
    RenamePolicy m_renamePolicy;
    std::string m_srcDir;
    std::string m_destDir;

  public:
    CopyPolicy(CopyPolicyImpl *impl, RenamePolicyImpl *renamePolicy,
               const std::string &srcDir, const std::string &destDir);
    ~CopyPolicy();
    bool copy(const std::string &src);
};

class FilterPolicy {
  private:
    FilterPolicyImplSp m_impl;

  public:
    FilterPolicy(FilterPolicyImpl *impl);
    ~FilterPolicy();
    bool filter(const std::string &file);
};

class CopyPolicyImpl {
  public:
    CopyPolicyImpl();
    virtual ~CopyPolicyImpl();
    virtual bool copy(const std::string &src, const std::string &srcDir,
                      const std::string &destDir,
                      RenamePolicy renamePolicy);
};

class FilterPolicyImpl {
  public:
    FilterPolicyImpl();
    virtual ~FilterPolicyImpl();
    virtual bool filter(const std::string &file);
};

class RenamePolicyImpl {
  public:
    RenamePolicyImpl();
    virtual ~RenamePolicyImpl();
    virtual std::string rename(const std::string &src);
};

} // namespace yjutil

#endif