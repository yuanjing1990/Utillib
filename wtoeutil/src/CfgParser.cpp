
/** @file
 *  @brief          配置解析实现文件
 *  @author         huangjun
 *  @date           2013-6-20
 *  @version        1.00
 *  @note           
 *
 *  配置解析实现文件
 */
#include <boost/shared_ptr.hpp>
#include <fstream>
#include "CfgParser.hpp"

namespace wtoeutil{



/**
 *  @brief          构造函数
 *  @author         huangjun
 *  @return         void
 *  @note           
 *
 *  构造函数
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
CCfgParser::CCfgParser() : m_spCallBack()
                            , m_parserStatus(EPARSERSTATUS_LINE_START)
                            , m_groupName("")
                            , m_bNeedCallGroupBegin( true )
                            , m_keyName("")
                            , m_keyNameSpace("")
                            , m_keyValue("")
                            , m_keyValueSpace("")
                            , m_errInfo("")
                            , m_line(1)
                            , m_col(1)
{
    m_statusHandlers[EPARSERSTATUS_LINE_START] = lineStartHandler;
    m_statusHandlers[EPARSERSTATUS_GOT_LEFT_SQUARE] = gotLeftSquareHandler;
    m_statusHandlers[EPARSERSTATUS_GETTING_GROUP_NAME] = gettingGroupNameHandler;
    m_statusHandlers[EPARSERSTATUS_GOT_GROUP_NAME] = gotGroupNameHandler;
    m_statusHandlers[EPARSERSTATUS_EXPECT_LINEEND] = expectLinefeedHandler;
    m_statusHandlers[EPARSERSTATUS_COMMENT] = commentHandler;
    m_statusHandlers[EPARSERSTATUS_GETTING_KEYNAME] = gettingKeyNameHandler;
    m_statusHandlers[EPARSERSTATUS_GOT_KEYNAME] = gotKeyNameHandler;
    m_statusHandlers[EPARSERSTATUS_GOT_EQUAL] = gotEqualHandler;
    m_statusHandlers[EPARSERSTATUS_GETTING_KEYVALUE] = gettingKeyValueHandler;
    m_statusHandlers[EPARSERSTATUS_ERROR] = errorHandler;
}

/**
 *  @brief          析构函数
 *  @author         huangjun
 *  @return         void
 *  @note           
 *
 *  析构函数
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
CCfgParser::~CCfgParser()
{
}

/**
 *  @brief          设置回调接口
 *  @author         huangjun
 *  @param[in]      spCallBack      回调接口
 *  @return         void
 *  @note           
 *
 *  设置回调接口
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
void CCfgParser::setCallBack(CSpICfgParserCallBack spCallBack)
{
    m_spCallBack = spCallBack;
}

/**
 *  @brief          解析文件
 *  @author         huangjun
 *  @param[in]      strCfgFilePath      文件路径
 *  @return         bool                是否成功
 *  @note           
 *
 *  解析文件
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::parse(const std::string & strCfgFilePath)
{
    if ( !m_spCallBack )
    {
        m_errInfo = "No call back!";
        return false;
    }

    std::ifstream   ifs;

    try
    {
        ifs.open(strCfgFilePath.c_str());
        if ( ifs.fail() )
        {
            m_errInfo = "Open file failed!" + strCfgFilePath;
            return false;
        }
    }
    catch ( ... )
    {
        m_errInfo = "Open file failed!" + strCfgFilePath;
        return false;
    }

    char            c = '\0';
    bool            bRet = true;

    // 初始化解析器
    initParse();

    while ( !ifs.eof() )
    {
        ifs.get( c );
        if ( ifs.fail() )
        {
            // ifstream 在读到最后一个字符后，并不会 eof()
            // 而是在读下一个字符时失败，此时要做判断
            break;
        }

        bRet = m_statusHandlers[m_parserStatus](this, c);
        if ( !bRet )
        {
            break;
        }

        if ( '\n' == c )
        {
            // 换行符，增加行号
            ++ m_line;
            m_col = 1;
        }
        else
        {
            // 增加列号
            ++ m_col;
        }
    }

    ifs.close();

    if ( !bRet )
    {
        return false;
    }

    // 最后添加一个结束符，结束处理
    if ( !m_statusHandlers[m_parserStatus](this, '\0') )
    {
        return false;
    }

    // 结束掉最后一个组
    return groupEnd();
}

/**
 *  @brief          获取错误信息
 *  @author         huangjun
 *  @return         std::string     错误信息
 *  @note           
 *
 *  获取错误信息
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
const std::string & CCfgParser::getErrInfo()
{
    return m_errInfo;
}

/**
 *  @brief          获取错误所在行
 *  @author         huangjun
 *  @return         uint32_t        错误所在行号
 *  @note           
 *
 *  获取错误所在行
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
uint32_t CCfgParser::getErrLine()
{
    return m_line;
}

/**
 *  @brief          获取错误所在列
 *  @author         huangjun
 *  @return         uint32_t        错误所在列号
 *  @note           
 *
 *  获取错误所在列
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
uint32_t CCfgParser::getErrCol()
{
    return m_col;
}


/**
 *  @brief          初始化解析
 *  @author         huangjun
 *  @return         void
 *  @note           
 *
 *  初始化解析
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
void CCfgParser::initParse()
{
    // 初始化解析状态
    m_parserStatus = EPARSERSTATUS_LINE_START;

    // 初始化组名
    initGroupName();

    // 遇到无名组中的键值需调用一次groupBegin
    m_bNeedCallGroupBegin = true;   

    // 初始化键名
    initKeyName();

    // 初始化键值
    initKeyValue();

    // 初始化错误信息
    m_errInfo = "";

    // 初始化行号
    m_line = 1;

    // 初始化列号
    m_col = 1;
}

/**
 *  @brief          组解析开始
 *  @author         huangjun
 *  @return         bool        是否成功
 *  @note           
 *
 *  组解析开始
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::groupBegin()
{
    bool bRet = m_spCallBack->groupBegin( m_groupName );

    if ( !bRet )
    {
        m_errInfo = "Call spCallBack->groupBegin( \"" + m_groupName + "\" ) failed!";
        return false;
    }

    // 清除无名组标记
    m_bNeedCallGroupBegin = false;

    return true;
}

/**
 *  @brief          获取到了键名字对
 *  @author         huangjun
 *  @return         bool            是否成功
 *  @note           
 *
 *  获取到了键名字对
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::key()
{
    if ( m_bNeedCallGroupBegin )
    {
        // 该键值为无名组中的第一个键
        // 需要调用一次 groupBegin
        if ( !groupBegin() )
        {
            return false;
        }
    }

    bool bRet = m_spCallBack->key(m_groupName, m_keyName, m_keyValue);
    
    if ( !bRet )
    {
        m_errInfo = "Call spCallBack->key( \""
                        + m_groupName + "\", \""
                        + m_keyName + "\", \""
                        + m_keyValue + "\" ) failed!";
        return false;
    }

    return true;
}

/**
 *  @brief          组解析结束
 *  @author         huangjun
 *  @return         bool        是否成功
 *  @note           
 *
 *  组解析结束
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::groupEnd()
{
    // 不再判断组名是否存在
#if 0
    if ( m_groupName.empty() )
    {
        return true;
    }
#endif

    if ( m_bNeedCallGroupBegin )
    {
        // 还没有调用过 groupBegin，此时不用调用 groupEnd
        return true;
    }

    bool bRet = m_spCallBack->groupEnd( m_groupName );

    if ( !bRet )
    {
        m_errInfo = "Call spCallBack->groupEnd( \"" + m_groupName + "\" ) failed!";
        return false;
    }

    return true;
}

/**
 *  @brief          初始化组名缓冲区
 *  @author         huangjun
 *  @return         void
 *  @note           
 *
 *  初始化组名缓冲区
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
void CCfgParser::initGroupName()
{
    m_groupName = "";
}

/**
 *  @brief          将字符添加到组名缓冲区
 *  @author         huangjun
 *  @param[in]      c           要输出的字符
 *  @return         void
 *  @note           
 *
 *  将字符添加到组名缓冲区
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
void CCfgParser::putGroupName(char c)
{
    m_groupName += c;
}

/**
 *  @brief          初始化键名缓冲区
 *  @author         huangjun
 *  @return         void
 *  @note           
 *
 *  初始化键名缓冲区
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
void CCfgParser::initKeyName()
{
    m_keyName = "";
    m_keyNameSpace = "";
}

/**
 *  @brief          将字符添加键名缓冲区
 *  @author         huangjun
 *  @param[in]      c           要输出的字符
 *  @return         void
 *  @note           
 *
 *  将字符添加键名缓冲区
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
void CCfgParser::putKeyName(char c)
{
    if ( isspace( c ) )
    {
        // 输出到空格缓冲区
        m_keyNameSpace += c;
    }
    else
    {
        // 将空格缓冲区的内容添加到 name 尾部
        m_keyName += m_keyNameSpace;

        // 清空空格缓冲区
        m_keyNameSpace = "";

        // 添加字符
        m_keyName += c;
    }
}

/**
 *  @brief          初始化键值缓冲区
 *  @author         huangjun
 *  @return         void
 *  @note           
 *
 *  初始化键值缓冲区
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
void CCfgParser::initKeyValue()
{
    m_keyValue = "";
    m_keyValueSpace = "";
}

/**
 *  @brief          将字符添加键值缓冲区
 *  @author         huangjun
 *  @param[in]      c           要输出的字符
 *  @return         void
 *  @note           
 *
 *  将字符添加键值缓冲区
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
void CCfgParser::putKeyValue(char c)
{
    if ( isspace( c ) )
    {
        // 输出到空格缓冲区
        m_keyValueSpace += c;
    }
    else
    {
        // 将空格缓冲区的内容添加到 value 尾部
        m_keyValue += m_keyValueSpace;

        // 清空空格缓冲区
        m_keyValueSpace = "";

        // 添加字符
        m_keyValue += c;
    }
}



/**
 *  @brief          是否为空格
 *  @author         huangjun
 *  @param[in]      c           被判断的字符
 *  @return         bool        是否为字符
 *  @note           
 *
 *  是否为空格
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::isspace(char c)
{
    if ( ' ' == c
        || '\t' == c
        || '\r' == c )
    {
        return true;
    }

    return false;
}




/**
 *  @brief          EPARSERSTATUS_LINE_START 处理函数
 *  @author         huangjun
 *  @param[in]      pThis       解析器指针
 *  @param[in]      c           要解析的字符
 *  @return         bool        是否成功
 *  @note           
 *
 *  EPARSERSTATUS_LINE_START 处理函数
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::lineStartHandler( CCfgParser * pThis, char c )
{
    // 行开始
    //      '\n'    ->  
    //      '\0'    ->  
    //      isspace ->  
    //      '#'     ->  EPARSERSTATUS_COMMENT
    //      '['     ->  EPARSERSTATUS_GOT_LEFT_SQUARE
    //      '='     ->  EPARSERSTATUS_ERROR
    //      *       ->  EPARSERSTATUS_GETTING_KEYNAME

    if ( '\n' == c
        || '\0' == c
        || isspace( c ) )
    {
        // 不改变状态
        return true;
    }

    if ( '#' == c )
    {
        // 跳到获取注释状态
        pThis->m_parserStatus = EPARSERSTATUS_COMMENT;
        return true;
    }

    if ( '[' == c )
    {
        // 将上一个组结束掉
        if ( !pThis->groupEnd() )
        {
            // 跳转到错误状态
            pThis->m_parserStatus = EPARSERSTATUS_ERROR;
            return false;
        }

        // 跳到已获取组名左括号状态
        pThis->m_parserStatus = EPARSERSTATUS_GOT_LEFT_SQUARE;
        return true;
    }

    if ( '=' == c )
    {
        pThis->m_parserStatus = EPARSERSTATUS_ERROR;
        pThis->m_errInfo = "No item name!";
        return false;
    }

    // 不再判断键值是否在组中
#if 0
    if ( pThis->m_groupName.empty() )
    {
        // 此时没有组
        pThis->m_parserStatus = EPARSERSTATUS_ERROR;
        pThis->m_errInfo = "Found key not in group!";
        return false;
    }
#endif

    // 获取到键名的第一个字符
    pThis->initKeyName();
    pThis->initKeyValue();
    pThis->putKeyName( c );

    // 跳转到获取键名状态
    pThis->m_parserStatus = EPARSERSTATUS_GETTING_KEYNAME;
    return true;
}

/**
 *  @brief          EPARSERSTATUS_GOT_LEFT_SQUARE 处理函数
 *  @author         huangjun
 *  @param[in]      pThis       解析器指针
 *  @param[in]      c           要解析的字符
 *  @return         bool        是否成功
 *  @note           
 *
 *  EPARSERSTATUS_GOT_LEFT_SQUARE 处理函数
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::gotLeftSquareHandler( CCfgParser * pThis, char c )
{
    // 获取到了 '['
    //      '\n'    ->  EPARSERSTATUS_ERROR
    //      '\0'    ->  EPARSERSTATUS_ERROR
    //      ']'     ->  EPARSERSTATUS_ERROR
    //      isspace ->  
    //      *       ->  EPARSERSTATUS_GETTING_GROUP_NAME

    if ( '\n' == c
        || '\0' == c
        || ']' == c)
    {
        // 跳转到错误状态
        pThis->m_parserStatus = EPARSERSTATUS_ERROR;
        pThis->m_errInfo = "Expect group name!";
        return false;
    }

    if ( isspace( c ) )
    {
        // 继续在该状态
        // 等待组名的第一个字符
        return true;
    }

    // 获取到组名的第一个字符
    pThis->initGroupName();
    pThis->putGroupName( c );

    // 跳转到获取组名状态
    pThis->m_parserStatus = EPARSERSTATUS_GETTING_GROUP_NAME;
    return true;
}

/**
 *  @brief          EPARSERSTATUS_GETTING_GROUP_NAME 处理函数
 *  @author         huangjun
 *  @param[in]      pThis       解析器指针
 *  @param[in]      c           要解析的字符
 *  @return         bool        是否成功
 *  @note           
 *
 *  EPARSERSTATUS_GETTING_GROUP_NAME 处理函数
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::gettingGroupNameHandler( CCfgParser * pThis, char c )
{
    // 正在读取组名
    //      '\n'    ->  EPARSERSTATUS_ERROR
    //      '\0'    ->  EPARSERSTATUS_ERROR
    //      isspace ->  EPARSERSTATUS_GOT_GROUP_NAME
    //      "]"     ->  EPARSERSTATUS_EXPECT_LINEEND
    //      *       ->  
    if ( '\n' == c
        || '\0' == c )
    {
        // 跳转到错误状态
        pThis->m_parserStatus = EPARSERSTATUS_ERROR;
        pThis->m_errInfo = "Expect ']' !";
        return false;
    }

    if ( isspace( c ) )
    {
        // 跳转到已获取组名状态
        // 此处不调用 groupBegin，等到读取到 ']' 号后再调用
        pThis->m_parserStatus = EPARSERSTATUS_GOT_GROUP_NAME;
        return true;
    }

    if ( ']' == c )
    {
        // 组名结束，调用 groupBegin
        if ( !pThis->groupBegin() )
        {
            pThis->m_parserStatus = EPARSERSTATUS_ERROR;
            return false;
        }

        // 跳转到期待换行符状态
        pThis->m_parserStatus = EPARSERSTATUS_EXPECT_LINEEND;
        return true;
    }

    // 添加字符到组名缓冲区
    pThis->putGroupName( c );

    // 继续在该状态
    return true;
}

/**
 *  @brief          EPARSERSTATUS_GOT_GROUP_NAME 处理函数
 *  @author         huangjun
 *  @param[in]      pThis       解析器指针
 *  @param[in]      c           要解析的字符
 *  @return         bool        是否成功
 *  @note           
 *
 *  EPARSERSTATUS_GOT_GROUP_NAME 处理函数
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::gotGroupNameHandler( CCfgParser * pThis, char c )
{
    // 已获得组名，期待 ']'
    //      "]"     ->  EPARSERSTATUS_EXPECT_LINEEND
    //      isspace ->  
    //      '\n'    ->  EPARSERSTATUS_ERROR
    //      '\0'    ->  EPARSERSTATUS_ERROR
    //      *       ->  EPARSERSTATUS_ERROR

    if ( ']' == c )
    {
        // 组名结束，调用 groupBegin
        if ( !pThis->groupBegin() )
        {
            pThis->m_parserStatus = EPARSERSTATUS_ERROR;
            return false;
        }

        // 跳转到期待换行符状态
        pThis->m_parserStatus = EPARSERSTATUS_EXPECT_LINEEND;
        return true;
    }

    if ( isspace( c ) )
    {
        // 继续在该状态
        return true;
    }

    // 跳转到错误状态
    pThis->m_parserStatus = EPARSERSTATUS_ERROR;
    pThis->m_errInfo = "Expect ']' !";
    return false;
}

/**
 *  @brief          EPARSERSTATUS_EXPECT_LINEEND 处理函数
 *  @author         huangjun
 *  @param[in]      pThis       解析器指针
 *  @param[in]      c           要解析的字符
 *  @return         bool        是否成功
 *  @note           
 *
 *  EPARSERSTATUS_EXPECT_LINEEND 处理函数
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::expectLinefeedHandler( CCfgParser * pThis, char c )
{
    // 期待获取行结尾
    //      '\n'    ->  EPARSERSTATUS_LINE_START
    //      '\0'    ->  EPARSERSTATUS_LINE_START
    //      isspace ->  
    //      *       ->  EPARSERSTATUS_ERROR

    if ( '\n' == c
        || '\0' == c )
    {
        // 跳转到行起始状态
        pThis->m_parserStatus = EPARSERSTATUS_LINE_START;
        return true;
    }

    if ( isspace( c ) )
    {
        // 继续在该状态
        return true;
    }

    // 跳转到错误状态
    pThis->m_parserStatus = EPARSERSTATUS_ERROR;
    pThis->m_errInfo = "Expect '\\n' !";
    return false;
}

/**
 *  @brief          EPARSERSTATUS_COMMENT 处理函数
 *  @author         huangjun
 *  @param[in]      pThis       解析器指针
 *  @param[in]      c           要解析的字符
 *  @return         bool        是否成功
 *  @note           
 *
 *  EPARSERSTATUS_COMMENT 处理函数
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::commentHandler( CCfgParser * pThis, char c )
{
    // 注释行
    //      '\n'    ->  EPARSERSTATUS_LINE_START
    //      '\0'    ->  EPARSERSTATUS_LINE_START
    //      *       ->  

    if ( '\n' == c
        || '\0' == c )
    {
        // 跳转到行起始状态
        pThis->m_parserStatus = EPARSERSTATUS_LINE_START;
        return true;
    }

    // 继续在该状态
    return true;
}

/**
 *  @brief          EPARSERSTATUS_GETTING_KEYNAME 处理函数
 *  @author         huangjun
 *  @param[in]      pThis       解析器指针
 *  @param[in]      c           要解析的字符
 *  @return         bool        是否成功
 *  @note           
 *
 *  EPARSERSTATUS_GETTING_KEYNAME 处理函数
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::gettingKeyNameHandler( CCfgParser * pThis, char c )
{
    // 正在获取键名
    //      '='     ->  EPARSERSTATUS_GOT_EQUAL
    //      '\n'    ->  EPARSERSTATUS_ERROR
    //      '\0'    ->  EPARSERSTATUS_ERROR
    //      *       ->  

    if ( '=' == c )
    {
        // 跳转到已获取等号状态
        pThis->m_parserStatus = EPARSERSTATUS_GOT_EQUAL;
        return true;
    }

    if ( '\n' == c
        || '\0' == c )
    {
        // 跳转到错误状态
        pThis->m_parserStatus = EPARSERSTATUS_ERROR;
        pThis->m_errInfo = "Expect '=' !";
        return false;
    }

    // 将当前字符添加到键名缓冲区中
    pThis->putKeyName( c );

    // 继续在该状态
    return true;
}

/**
 *  @brief          EPARSERSTATUS_GOT_KEYNAME 处理函数
 *  @author         huangjun
 *  @param[in]      pThis       解析器指针
 *  @param[in]      c           要解析的字符
 *  @return         bool        是否成功
 *  @note           
 *
 *  EPARSERSTATUS_GOT_KEYNAME 处理函数
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::gotKeyNameHandler( CCfgParser * pThis, char c )
{
    // 已经获取到键名
    //      '='     ->  EPARSERSTATUS_GOT_EQUAL
    //      isspace ->  
    //      '\n'    ->  EPARSERSTATUS_ERROR
    //      '\0'    ->  EPARSERSTATUS_ERROR
    //      *       ->  EPARSERSTATUS_ERROR

    if ( '=' == c )
    {
        // 跳转到已获取等号状态
        pThis->m_parserStatus = EPARSERSTATUS_GOT_EQUAL;
        return true;
    }

    if ( isspace( c ) )
    {
        // 继续在该状态
        return true;
    }

    // 跳转到错误状态
    pThis->m_parserStatus = EPARSERSTATUS_ERROR;
    pThis->m_errInfo = "Expect '=' !";
    return false;
}

/**
 *  @brief          EPARSERSTATUS_GOT_EQUAL 处理函数
 *  @author         huangjun
 *  @param[in]      pThis       解析器指针
 *  @param[in]      c           要解析的字符
 *  @return         bool        是否成功
 *  @note           
 *
 *  EPARSERSTATUS_GOT_EQUAL 处理函数
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::gotEqualHandler( CCfgParser * pThis, char c )
{
    // 已经获得名值分隔符
    //      '\n'    ->  EPARSERSTATUS_LINE_START
    //      '\0'    ->  EPARSERSTATUS_LINE_START
    //      isspace ->  
    //      *       ->  EPARSERSTATUS_GETTING_KEYVALUE

    if ( '\n' == c
        || '\0' == c )
    {
        // 输出键
        if ( !pThis->key() )
        {
            // 跳转到错误状态
            pThis->m_parserStatus = EPARSERSTATUS_ERROR;
            return false;
        }
    
        // 跳转到行起始状态
        pThis->m_parserStatus = EPARSERSTATUS_LINE_START;
        return true;
    }

    if ( isspace( c ) )
    {
        // 继续在该状态
        return true;
    }

    // 获取到第一个键值字符
    pThis->putKeyValue( c );

    // 跳转到获取键值状态
    pThis->m_parserStatus = EPARSERSTATUS_GETTING_KEYVALUE;
    return true;
}

/**
 *  @brief          EPARSERSTATUS_GETTING_KEYVALUE 处理函数
 *  @author         huangjun
 *  @param[in]      pThis       解析器指针
 *  @param[in]      c           要解析的字符
 *  @return         bool        是否成功
 *  @note           
 *
 *  EPARSERSTATUS_GETTING_KEYVALUE 处理函数
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::gettingKeyValueHandler( CCfgParser * pThis, char c )
{
    // 正在获取键值
    //      '\n'    ->  EPARSERSTATUS_LINE_START
    //      '\0'    ->  EPARSERSTATUS_LINE_START
    //      *       ->  

    if ( '\n' == c
        || '\0' == c )
    {
        // 输出键
        if ( !pThis->key() )
        {
            // 跳转到错误状态
            pThis->m_parserStatus = EPARSERSTATUS_ERROR;
            return false;
        }
    
        // 跳转到行起始状态
        pThis->m_parserStatus = EPARSERSTATUS_LINE_START;
        return true;
    }

    // 获取到第一个键值字符
    pThis->putKeyValue( c );

    // 继续在该状态
    return true;
}

/**
 *  @brief          EPARSERSTATUS_ERROR 处理函数
 *  @author         huangjun
 *  @param[in]      pThis       解析器指针
 *  @param[in]      c           要解析的字符
 *  @return         bool        是否成功
 *  @note           
 *
 *  EPARSERSTATUS_ERROR 处理函数
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::errorHandler( CCfgParser * pThis, char c )
{
    // 解析状态错误
    return false;
}



}


