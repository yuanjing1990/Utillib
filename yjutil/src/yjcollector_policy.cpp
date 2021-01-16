#include "yjcollector_policy.h"

#include <boost/filesystem.hpp>
#include <fstream>
#include <stdlib.h>
#include <string>

namespace yjutil {
CopyPolicy::CopyPolicy(CopyPolicyImpl *impl, RenamePolicyImpl *renamePolicy,
                       const std::string &srcDir, const std::string &destDir)
    : m_impl(impl), m_renamePolicy(renamePolicy), m_srcDir(srcDir),
      m_destDir(destDir) {}

CopyPolicy::~CopyPolicy() {}

bool CopyPolicy::copy(const std::string &src) {
    return m_impl->copy(src, m_srcDir, m_destDir, m_renamePolicy);
}

FilterPolicy::FilterPolicy(FilterPolicyImpl *impl) : m_impl(impl) {}

FilterPolicy::~FilterPolicy() {}

bool FilterPolicy::filter(const std::string &file) {
    if (!m_impl) {
        return false;
    }
    return m_impl->filter(file);
}

RenamePolicy::RenamePolicy(RenamePolicyImpl *renamePolicy) : m_impl(renamePolicy) {}

std::string RenamePolicy::rename(const std::string &src) {
    if (!m_impl) {
        return src;
    }
    return m_impl->rename(src);
}

CopyPolicyImpl::CopyPolicyImpl() {}

CopyPolicyImpl::~CopyPolicyImpl() {}

bool CopyPolicyImpl::copy(const std::string &src, const std::string &srcDir,
                          const std::string &destDir,
                          RenamePolicy renamePolicy) {
    if (src.find(srcDir) != 0 || !boost::filesystem::exists(destDir)) {
        return false;
    }
    std::string relativePath = src.substr(srcDir.size(), src.size());
    std::string rename_relativePath = renamePolicy.rename(relativePath);
    if (!rename_relativePath.empty()) {
        relativePath = rename_relativePath;
    }

    std::string destFile = destDir + relativePath;
    boost::filesystem::path path(destFile.substr(0, destFile.find_last_of("/")));
    boost::filesystem::create_directories(path);

    DEBUG_PRINT("copy: %s -> %s", src.c_str(), destFile.c_str());

    std::ifstream ifs(src, std::ios::binary);
    std::ofstream ofs(destFile, std::ios::binary);
    if (!ifs || !ofs) {
        return false;
    }
    ofs << ifs.rdbuf();
    return true;
}

FilterPolicyImpl::FilterPolicyImpl() {}

FilterPolicyImpl::~FilterPolicyImpl() {}

bool FilterPolicyImpl::filter(const std::string &file) { return false; }

RenamePolicyImpl::RenamePolicyImpl() {}

RenamePolicyImpl::~RenamePolicyImpl() {}

std::string RenamePolicyImpl::rename(const std::string &src) {
    return src;
}

} // namespace yjutil
