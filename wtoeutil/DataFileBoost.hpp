
#ifndef DATAFILEBOOST_HPP_
#define DATAFILEBOOST_HPP_

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/set.hpp>

#include "FileSystem.hpp"

namespace wtoeutil
{

/**
* 将一个结构写入文件.
* 使用boost的序列化.
* 如果文件不存在,会创建文件.
std::ofstream ofs( file.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );
if( !ofs.is_open() )
return false;
boost::archive::text_oarchive oa( ofs );
oa << info;
ofs.close();
return true;
*/
template< typename ST >
bool struct2File_boost( const ST &st, const std::string &filePath )
{
#if defined( WIN32 ) && defined( _MSC_VER )
    boost::filesystem::path bp( filePath );
    std::ofstream f( bp.BOOST_FILESYSTEM_C_STR, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc, _SH_DENYRW );
    if( f.is_open() == false )
    {
        if( EACCES == errno )
        {
            _set_errno( 0 );
            std::cout << " struct2File open failed filePath = " << filePath << " = file access deny!\n";
        }
        else
        {
            std::cout << " struct2File open failed filePath = " << filePath << "\n";
        }
        return false;
    }

#elif defined( __GNUC__ )
    SFileTryLock tl( filePath, O_CREAT|O_WRONLY, LOCK_EX|LOCK_NB ); // O_CREAT|O_RDWR
    if( tl.result() == -1 )
    {
        std::cout << " struct2File open failed filePath = " << filePath << "\n";
        return false;
    }
    else if( tl.result() == -2 )
    {
        std::cout << " struct2File open failed filePath = " << filePath << " = file access deny!\n";
        return false;
    }
    else if( tl.result() == 0 )
    {
        // ok;
    }
    else
    {
        std::cout << " struct2File open failed filePath = " << filePath << " = unknown!\n";
        return false;
    }

    boost::filesystem::path bp( filePath );
    std::ofstream f( bp.BOOST_FILESYSTEM_C_STR, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );
    if( f.is_open() == false )
    {
        std::cout << " struct2File open failed filePath = " << filePath << "\n";
        return false;
    }

#endif

    bool retValue = false;
    try
    {
        boost::archive::text_oarchive oa( f );
        oa << st;
        retValue = true;
    }
    catch( const boost::archive::archive_exception &ae )
    {
        std::cout << " struct2File catch " << ae.what() << "\n";
    }
    catch( ... )
    {
        std::cout << " struct2File catch unknown\n";
    }

    f.close();
    return retValue;
}

/**
* 将一个文件中的内容还原为一个结构.
* 如果文件不存在,则返回失败.
std::ifstream ifs( file.c_str(), std::ios_base::in | std::ios_base::binary );
if( !ifs.is_open() )
{
if( !isExistRegularFile( file ) )
{
info.clear();
return true; // 如果不存在文件,则以空返回.
}
return false;
}
info.clear();
boost::archive::text_iarchive ia( ifs );
ia >> info;
ifs.close();
return true;
*/
template< typename ST >
bool file2Struct_boost( const std::string &filePath, ST &st )
{
#if defined( WIN32 ) && defined( _MSC_VER )
    boost::filesystem::path bp( filePath );
    std::ifstream f( bp.BOOST_FILESYSTEM_C_STR, std::ios_base::in | std::ios_base::binary, _SH_DENYWR );
    if( f.is_open() == false )
    {
        if( EACCES == errno )
        {
            _set_errno( 0 );
            std::cout << " file2Struct open failed filePath = " << filePath << " = file access deny!\n";
        }
        else
        {
            std::cout << " file2Struct open failed filePath = " << filePath << "\n";
        }
        return false;
    }

#elif defined( __GNUC__ )
    SFileTryLock tl( filePath, O_RDONLY, LOCK_SH|LOCK_NB );
    if( tl.result() == -1 )
    {
        std::cout << " file2Struct open failed filePath = " << filePath << "\n";
        return false;
    }
    else if( tl.result() == -2 )
    {
        std::cout << " file2Struct open failed filePath = " << filePath << " = file access deny!\n";
        return false;
    }
    else if( tl.result() == 0 )
    {
        // ok;
    }
    else
    {
        std::cout << " file2Struct open failed filePath = " << filePath << " = unknown!\n";
        return false;
    }

    boost::filesystem::path bp( filePath );
    std::ifstream f( bp.BOOST_FILESYSTEM_C_STR, std::ios_base::in | std::ios_base::binary );
    if( f.is_open() == false )
    {
        std::cout << " file2Struct open failed filePath = " << filePath << "\n";
        return false;
    }
#endif

    bool retValue = false;
    try
    {
        boost::archive::text_iarchive ia( f );
        ia >> st;
        retValue = true;
    }
    catch( const boost::archive::archive_exception &ae )
    {
        std::cout << " file2Struct catch " << ae.what() << "\n";
    }
    catch( ... )
    {
        std::cout << " file2Struct catch unknown\n";
    }

    f.close();
    return retValue;
}

}

#endif

/*
在序列化结构时，需要手写一些boost序列化函数。举例如下：

struct SMediaResBase
{
boost::uuids::uuid  id;
uint32_t            addr;
uint8_t             channelIndex;
};

struct SMediaRes
{
SMediaResBase               res;
std::string                 user;
boost::array<uint8_t,16>    pwd;
};

namespace boost
{
namespace serialization
{

template<class Archive>
void serialize( Archive & ar, boost::uuids::uuid & g, const unsigned int version )
{
ar & g.data;
}
template<class Archive>
void serialize( Archive & ar, SMediaResBase & g, const unsigned int version )
{
ar & g.id;
ar & g.addr;
ar & g.channelIndex;
}
template<class Archive>
void serialize( Archive & ar, SMediaRes & g, const unsigned int version )
{
ar & g.res;
ar & g.user;
ar & g.pwd;
}

}
}

*/
