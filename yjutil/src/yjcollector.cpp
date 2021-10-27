#include <algorithm>
#include <dirent.h>
#include <filesystem>
#include <functional>
#include <stdio.h>
#include <string.h>

#include "yjcollector.h"
#include "yjcollector_policy.h"
#include "yjfile.h"
#include "yjutil.h"

namespace yjutil {

FileCollector::FileCollector(const std::string &strSrcPath,
                             const std::string &strDestPath,
                             CopyPolicyImpl *copyImpl,
                             FilterPolicyImpl *filterImpl,
                             RenamePolicyImpl *renameImpl)
    : m_srcDir(strSrcPath), m_destDir(strDestPath),
      m_copyPolicy(copyImpl, renameImpl, strSrcPath, strDestPath),
      m_filterPolicy(filterImpl){};

FileCollector::~FileCollector() {}

bool FileCollector::isDirExist(const std::string &dir) {
    if (isDirectory(dir)) {
        return true;
    }
    return false;
}

bool FileCollector::getFileVec(const std::string &dir,
                               std::vector<std::string> &fileVec) {
    if (!isDirExist(dir)) {
        return false;
    }
    return getFileVecInner(dir, dir, fileVec);
}

bool FileCollector::getFileVecInner(const std::string &dirRoot,
                                    const std::string &curDir,
                                    std::vector<std::string> &fileVec) {
    std::filesystem::directory_iterator cur(curDir);
    std::filesystem::directory_iterator end;

    while (cur != end) {
        std::filesystem::path path(*cur);
        if (std::filesystem::is_directory(path)) {
            getFileVecInner(dirRoot, path.string(), fileVec);
        } else if (std::filesystem::is_regular_file(path)) {
            fileVec.push_back(path.string());
        }
        ++cur;
    }
    sort(fileVec.begin(), fileVec.end());
    return true;
}

bool FileCollector::doCollect() {
    std::vector<std::string> srcFileVec;
    if (!getFileVec(m_srcDir, srcFileVec) || !isDirExist(m_destDir)) {
        return false;
    }

    std::for_each(srcFileVec.begin(), srcFileVec.end(),
                  std::bind1st(std::mem_fun(&FileCollector::collectFile), this));
    return true;
}

bool FileCollector::collectFile(std::string file) {
    if (m_filterPolicy.filter(file)) {
        return false;
    }
    m_copyPolicy.copy(file);
    return true;
}

} // namespace yjutil