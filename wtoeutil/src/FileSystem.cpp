
#include <stdio.h>
#include <iostream>

#include "FileSystem.hpp"

namespace wtoeutil
{

bool getDirFileEntry( std::set< std::string > &fileEntrys, const std::string &dir )
{
    boost::filesystem::path p( dir );
    std::vector< boost::filesystem::directory_entry > v;
    fileEntrys.clear();
    try
    {
        /*
         * Throws: filesystem_error;
         * or
         * boost::system::error_code ec;
         * boost::filesystem::is_directory( p, ec );
         */
        if( !boost::filesystem::is_directory( p ) )
        {
            return false;
        }
        std::copy( boost::filesystem::directory_iterator( p ), boost::filesystem::directory_iterator(), std::back_inserter( v ) );
        std::vector< boost::filesystem::directory_entry >::const_iterator it( v.begin() );
        std::vector< boost::filesystem::directory_entry >::const_iterator end( v.end() );
        for( ; it != end; ++it)
        {
            if( !boost::filesystem::is_regular_file( it->path() ) )
                continue;
            /*
            tmpPath.filename() << std::endl; // 文件名全部,如[abc.txt]
            tmpPath.extension() << std::endl; // 文件名中的后缀,如[.txt]
            tmpPath.stem() << std::endl << std::endl; // 文件前缀,如[abc]
            定义的文件名有.号,所以只能用全名.(00.19.AD.00.00.AB.192.168.1.231)
            */
            std::string str = it->path().filename().string();
            fileEntrys.insert( str );
        }
        return true;
    }
    catch( const boost::filesystem::filesystem_error& fe )
    {
        std::cout << "*getDirFileEntry catch " << fe.what() << "\n";
    }
    catch( ... )
    {
        std::cout << "*getDirFileEntry catch unknown\n";
    }

    return false;
}

bool removeRegularFile( const std::string &filePathName )
{
    boost::filesystem::path p( filePathName );
    try
    {
        if( !boost::filesystem::is_regular_file( p ) ) // 判断是文件
        {
            return false;
        }
        if( !boost::filesystem::remove( p ) )
        {
            return false;
        }
        return true;
    }
    catch( const boost::filesystem::filesystem_error& fe )
    {
        std::cout << "*removeRegularFile catch " << fe.what() << "\n";
    }
    catch( ... )
    {
        std::cout << "*removeRegularFile catch unknown\n";
    }
    return false;
}

bool createRegularFile( const std::string &filePathName )
{
    FILE *fp = fopen( filePathName.c_str(), "w" );
    if( fp == 0 )
        return false;
    fclose( fp );
    return true;
}

bool createDirectory( const std::string &dirPathName )
{
    boost::filesystem::path p( dirPathName );
    try
    {
        if( !boost::filesystem::create_directory( p ) )
        {
            return false;
        }
        return true;
    }
    catch( const boost::filesystem::filesystem_error& fe )
    {
        std::cout << "*createDirectory catch " << fe.what() << "\n";
    }
    catch( ... )
    {
        std::cout << "*createDirectory catch unknown\n";
    }
    return false;
}

bool createDirectories( const std::string &dirPathNames )
{
    boost::filesystem::path p( dirPathNames );
    try
    {
        if( !boost::filesystem::create_directories( p ) )
        {
            return false;
        }
        return true;
    }
    catch( const boost::filesystem::filesystem_error& fe )
    {
        std::cout << "*createDirectories catch " << fe.what() << "\n";
    }
    catch( ... )
    {
        std::cout << "*createDirectories catch unknown\n";
    }
    return false;
}

bool removeDirectory( const std::string &dirPathName )
{
    boost::filesystem::path p( dirPathName );
    try
    {
        if( !boost::filesystem::is_directory( p ) ) // 判断是目录,否则都可以删除
        {
            return false;
        }
        if( !boost::filesystem::remove( p ) )
        {
            return false;
        }
        return true;
    }
    catch( const boost::filesystem::filesystem_error& fe )
    {
        std::cout << "*removeDirectory catch " << fe.what() << "\n";
    }
    catch( ... )
    {
        std::cout << "*removeDirectory catch unknown\n";
    }
    return false;
}

bool removeDirectoryAll( const std::string &dirPathName )
{
    boost::filesystem::path p( dirPathName );
    try
    {
        if( !boost::filesystem::is_directory( p ) ) // 判断是目录,否则都可以删除
        {
            return false;
        }
        // The number of files removed
        boost::uintmax_t t = boost::filesystem::remove_all( p );
        if( t == 0 )
        {
            return false;
        }
        return true;
    }
    catch( const boost::filesystem::filesystem_error& fe )
    {
        std::cout << "*removeDirectoryAll catch " << fe.what() << "\n";
    }
    catch( ... )
    {
        std::cout << "*removeDirectoryAll catch unknown\n";
    }
    return false;
}

bool isExistRegularFile( const std::string &filePathName )
{
    boost::filesystem::path p( filePathName );
    try
    {
        if( !boost::filesystem::is_regular_file( p ) ) // 判断是文件
        {
            return false;
        }
        return true;
    }
    catch( const boost::filesystem::filesystem_error& fe )
    {
        std::cout << "*isExistRegularFile catch " << fe.what() << "\n";
    }
    catch( ... )
    {
        std::cout << "*isExistRegularFile catch unknown\n";
    }
    return false;
}

bool isExistDirectory( const std::string &path )
{
    boost::filesystem::path p( path );
    try
    {
        if( !boost::filesystem::is_directory( p ) ) // Throws: filesystem_error;//boost::system::error_code ec;boost::filesystem::is_directory( bp, ec );
        {
            return false;
        }
        return true;
    }
    catch( const boost::filesystem::filesystem_error& fe )
    {
        std::cout << "*isExistDirectory catch " << fe.what() << "\n";
    }
    catch( ... )
    {
        std::cout << "*isExistDirectory catch unknown\n";
    }
    return false;
}

bool isExistPath( const std::string &path )
{
    boost::filesystem::path p( path );
    try
    {
        if( !boost::filesystem::exists( p ) ) // Throws: filesystem_error
        {
            return false;
        }
        return true;
    }
    catch( const boost::filesystem::filesystem_error& fe )
    {
        std::cout << "*isExistPath catch " << fe.what() << "\n";
    }
    catch( ... )
    {
        std::cout << "*isExistPath catch unknown\n";
    }
    return false;
}

bool getParentPath( const std::string &oldPath, std::string &newPath )
{
    boost::filesystem::path p( oldPath );
    if( p.has_parent_path() )
    {
        p = p.parent_path();
        newPath = p.string();
        return true;
    }
    return false;
}

bool getParentPath( const std::string &oldPath, std::string &newPath, const uint32_t parentLevel )
{
    if( parentLevel == 0 )
    {
        newPath = oldPath;
        return true;
    }
        
    boost::filesystem::path p( oldPath );
    uint32_t count = parentLevel;
    while( p.has_parent_path() )
    {
        p = p.parent_path(); // 如果path已经为空,则parent_path总是返回空.
        --count;
        if( count == 0 )
        {
            newPath = p.string();
            return true;
        }
    }

    return false;
}

bool getCurrentPath( std::string &currentPath )
{
    boost::filesystem::path p;
    try
    {
        p = boost::filesystem::current_path();
        currentPath = p.string();
        return true;
    }
    catch( const boost::filesystem::filesystem_error& fe )
    {
        std::cout << "*getCurrentPath catch " << fe.what() << "\n";
    }
    catch( ... )
    {
        std::cout << "*getCurrentPath catch unknown\n";
    }
    return false;
}

}

#include "DataFileBoost.hpp"
