#ifndef FILESYSTEM_HPP_
#define FILESYSTEM_HPP_

#include <fstream>
#include <inttypes.h>
#include <iostream>
#include <memory>
#include <set>
#include <string>

#if defined(WIN32) && defined(_MSC_VER)
#elif defined(__GNUC__)
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#include <filesystem>
#include <fstream>

#include "utildef.hpp"

namespace wtoeutil {

/**
    * 获取一个目录下,所有的文件名列表.
    */
bool UTIL_EXPORT getDirFileEntry(std::set<std::string> &fileEntrys, const std::string &dir);

/**
    * 删除一个文件.
    * 如果此文件不存在,失败.
    * 如果是目录,失败.
    */
bool UTIL_EXPORT removeRegularFile(const std::string &filePathName);

/**
    * 创建一个常规文件.
    * 注意:如果创建文件时路径是 c:/abc/ 则创建失败.因为这是个目录路径.
    */
bool UTIL_EXPORT createRegularFile(const std::string &filePathName);

/**
    * 创建一个目录,
    * 要求路径中的最后一个目录才是不存在的,它的父目录要全部存在.
    */
bool UTIL_EXPORT createDirectory(const std::string &dirPathName);
/**
    * 创建目录层次
    * 不存在的目录会全部创建.
    */
bool UTIL_EXPORT createDirectories(const std::string &dirPathNames);

/**
    * 删除一个目录,
    * 要求此目录为空.
    */
bool UTIL_EXPORT removeDirectory(const std::string &filePathName);
/**
    * 删除一个目录,
    * 并删除其下所有内容
    */
bool UTIL_EXPORT removeDirectoryAll(const std::string &filePathName);

/**
    * 常规文件是否存在.
    */
bool UTIL_EXPORT isExistRegularFile(const std::string &filePathName);

/**
    * 目录是否存在.
    */
bool UTIL_EXPORT isExistDirectory(const std::string &dirPathName);

/**
    * 判断本路径存在.
    * 注意:存在的路径,有可能是文件,也有可能是目录.
    */
bool UTIL_EXPORT isExistPath(const std::string &path);

/**
    * 使路径向上一层.
    * 注意:这里只是在字符串上动作,不考虑此路径是否存在.
    * @note
    * @verbatim 
    * - 如./abc/def/,向上一级为变成./abc/def[注意这个情况.要小心它]
    * - 如./abc/def ,向上一级为变成./abc
    * - 如./        ,向上一级为变成.
    * - 如.         ,向上一级为变成[空串]
    * @endverbatim
    * 传入的oldPath,应该是一个路径.
    */
bool UTIL_EXPORT getParentPath(const std::string &oldPath, std::string &newPath);

/**
    * 与上一个重载的getParentPath不同之处在于它会向上parentLevel次.
    * 如果parentLevel==1,则与上一个相同.
    * 如果parentLevel==0,则不变返回.
    */
bool UTIL_EXPORT getParentPath(const std::string &oldPath, std::string &newPath, const uint32_t parentLevel);

/**
    * 当前目录路径.
    * 得到的是绝对路径.
    * 注意:路径最后是不带斜线的.
    * 如  c:\abc\def
    */
bool UTIL_EXPORT getCurrentPath(std::string &currentPath);

#if defined(WIN32) && defined(_MSC_VER)
#elif defined(__GNUC__)
struct SFileTryLock // 对文件进行一次上锁操作.
{
  public:
    SFileTryLock(const std::string &filePath, int openflag, int lockflag) {
        m_fd = -1;
        m_ret = -1;
        m_fd = open(filePath.c_str(), openflag); // -1失败,其它成功
        if (m_fd == -1)                          // 没有成功打开
            return;
        m_ret = -2;
        int l = flock(m_fd, lockflag); // 0成功,-1失败
        if (l == -1)                   // 没有成功的得到锁
            return;
        m_ret = 0;
    }
    ~SFileTryLock() {
        if (m_fd != -1) {
            close(m_fd); //可以用LOCK_UN释放锁,也可以用关闭文件描述符的方式去释放锁.
        }
    }
    int result() const {
        return m_ret; // 0成功,-1打开失败,-2加锁失败.
    }

    // 对文件操作位置进行定位.
    off_t lseek(off_t offset, int whence) {
        return ::lseek(m_fd, offset, whence);
    }

    ssize_t write(const void *buf, size_t count) {
        return ::write(m_fd, buf, count);
    }
    ssize_t read(void *buf, size_t count) {
        return ::read(m_fd, buf, count);
    }

  private:
    int m_ret;
    int m_fd;
};

#endif

} // namespace wtoeutil

#endif

/*
由于我们的实现是fstream,而Linux下的fstream没有此参数,所以以下解决方案仅在windows下的效.

文件打开时,open函数的最后一个参数,默认是ios_base::_Openprot,它等于_SH_DENYNO,即允许多个使用者同时打开.
_SH_COMPAT
Sets Compatibility mode for 16-bit applications. 16位程序兼容,即兼容的共享方式

_SH_DENYNO
Permits read and write access. 允许其它使用者读和写.

_SH_DENYRD
Denies read access to the file. 拒绝其它使用者读,但其它使用者可以写.

_SH_DENYRW
Denies read and write access to the file.拒绝其它使用者读,且拒绝其它使用者写

_SH_DENYWR
Denies write access to the file.拒绝其它使用者写

我们要用的是：读时拒绝写；写时拒绝读/写。
std::ofstream f( filePath.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc, _SH_DENYRW );
std::ifstream f( filePath.c_str(), std::ios_base::in  | std::ios_base::binary                       , _SH_DENYWR );

而在Linux下,要用flock函数.
它需要:
先用open打开一个文件,得到文件描述符,
再用flock对这个文件描述符去加锁,
如果加成功,即表示没有人使用.

*/
