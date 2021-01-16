#include "yjfile.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

namespace yjutil {

std::string getFileName(const std::string& filePath) {
    return filePath.substr(filePath.find_last_of("/") + 1, filePath.length());
}

std::string getFileDirectory(const std::string& filePath) {
    return filePath.substr(0, filePath.find_last_of("/") + 1);
}

bool isDirectory(const std::string& filePath) {
    struct stat s_buf;
    if (0 != stat(filePath.c_str(), &s_buf)) {
        // 目录路径不存在
        return false;
    }
    return S_ISDIR(s_buf.st_mode);
}

bool isFile(const std::string& filePath) {
    struct stat s_buf;
    if (0 != stat(filePath.c_str(), &s_buf)) {
        // 文件路径不存在
        return false;
    }
    return S_ISREG(s_buf.st_mode);
}

bool isFileExist(const std::string& filePath) {
    struct stat s_buf;
    return 0 == stat(filePath.c_str(), &s_buf);
}

bool createFile(const std::string& filePath) {
    int ret = open(filePath.c_str(), O_CREAT);
    if (ret != -1) {
        close(ret);
    }
    return -1 != ret;
}

bool removeFile(const std::string& filePath) {
    unlink(filePath.c_str());
    return true;
}

bool createDir(const std::string& dirPath, bool bCreateParent) {
    int ret = true;
    if (bCreateParent) {
        if (isFileExist(getFileDirectory(dirPath))) {
            ret = ret && createDir(dirPath, false);
        } else {
            ret = ret && createDir(getFileDirectory(dirPath), true);
        }
    } else {
        ret = ret && (0 == mkdir(dirPath.c_str(), S_IRWXU));
    }
    return ret;
}

bool removeDir(const std::string& dirPath, bool bRecursion) {
    int ret = true;
    if (bRecursion) {
        DIR* dir = opendir(dirPath.c_str());
        struct dirent* file;
        while(! (file = readdir(dir))) {
            if (file->d_type == DT_REG) {
                removeFile(file->d_name);
            } else if (file->d_type == DT_DIR) {
                removeDir(file->d_name, true);
                rmdir(file->d_name);
            }
        }
        closedir(dir);
    } else {
        ret = ret && (0 == rmdir(dirPath.c_str()));
    }
    return ret;
}

}