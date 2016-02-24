
/** @file
 *  @brief          ���ý���ʵ���ļ�
 *  @author         huangjun
 *  @date           2013-6-20
 *  @version        1.00
 *  @note           
 *
 *  ���ý���ʵ���ļ�
 */
#include <boost/shared_ptr.hpp>
#include <fstream>
#include "CfgParser.hpp"

namespace wtoeutil{



/**
 *  @brief          ���캯��
 *  @author         huangjun
 *  @return         void
 *  @note           
 *
 *  ���캯��
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
 *  @brief          ��������
 *  @author         huangjun
 *  @return         void
 *  @note           
 *
 *  ��������
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
CCfgParser::~CCfgParser()
{
}

/**
 *  @brief          ���ûص��ӿ�
 *  @author         huangjun
 *  @param[in]      spCallBack      �ص��ӿ�
 *  @return         void
 *  @note           
 *
 *  ���ûص��ӿ�
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
 *  @brief          �����ļ�
 *  @author         huangjun
 *  @param[in]      strCfgFilePath      �ļ�·��
 *  @return         bool                �Ƿ�ɹ�
 *  @note           
 *
 *  �����ļ�
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

    // ��ʼ��������
    initParse();

    while ( !ifs.eof() )
    {
        ifs.get( c );
        if ( ifs.fail() )
        {
            // ifstream �ڶ������һ���ַ��󣬲����� eof()
            // �����ڶ���һ���ַ�ʱʧ�ܣ���ʱҪ���ж�
            break;
        }

        bRet = m_statusHandlers[m_parserStatus](this, c);
        if ( !bRet )
        {
            break;
        }

        if ( '\n' == c )
        {
            // ���з��������к�
            ++ m_line;
            m_col = 1;
        }
        else
        {
            // �����к�
            ++ m_col;
        }
    }

    ifs.close();

    if ( !bRet )
    {
        return false;
    }

    // ������һ������������������
    if ( !m_statusHandlers[m_parserStatus](this, '\0') )
    {
        return false;
    }

    // ���������һ����
    return groupEnd();
}

/**
 *  @brief          ��ȡ������Ϣ
 *  @author         huangjun
 *  @return         std::string     ������Ϣ
 *  @note           
 *
 *  ��ȡ������Ϣ
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
 *  @brief          ��ȡ����������
 *  @author         huangjun
 *  @return         uint32_t        ���������к�
 *  @note           
 *
 *  ��ȡ����������
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
 *  @brief          ��ȡ����������
 *  @author         huangjun
 *  @return         uint32_t        ���������к�
 *  @note           
 *
 *  ��ȡ����������
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
 *  @brief          ��ʼ������
 *  @author         huangjun
 *  @return         void
 *  @note           
 *
 *  ��ʼ������
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
void CCfgParser::initParse()
{
    // ��ʼ������״̬
    m_parserStatus = EPARSERSTATUS_LINE_START;

    // ��ʼ������
    initGroupName();

    // �����������еļ�ֵ�����һ��groupBegin
    m_bNeedCallGroupBegin = true;   

    // ��ʼ������
    initKeyName();

    // ��ʼ����ֵ
    initKeyValue();

    // ��ʼ��������Ϣ
    m_errInfo = "";

    // ��ʼ���к�
    m_line = 1;

    // ��ʼ���к�
    m_col = 1;
}

/**
 *  @brief          �������ʼ
 *  @author         huangjun
 *  @return         bool        �Ƿ�ɹ�
 *  @note           
 *
 *  �������ʼ
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

    // �����������
    m_bNeedCallGroupBegin = false;

    return true;
}

/**
 *  @brief          ��ȡ���˼����ֶ�
 *  @author         huangjun
 *  @return         bool            �Ƿ�ɹ�
 *  @note           
 *
 *  ��ȡ���˼����ֶ�
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::key()
{
    if ( m_bNeedCallGroupBegin )
    {
        // �ü�ֵΪ�������еĵ�һ����
        // ��Ҫ����һ�� groupBegin
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
 *  @brief          ���������
 *  @author         huangjun
 *  @return         bool        �Ƿ�ɹ�
 *  @note           
 *
 *  ���������
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::groupEnd()
{
    // �����ж������Ƿ����
#if 0
    if ( m_groupName.empty() )
    {
        return true;
    }
#endif

    if ( m_bNeedCallGroupBegin )
    {
        // ��û�е��ù� groupBegin����ʱ���õ��� groupEnd
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
 *  @brief          ��ʼ������������
 *  @author         huangjun
 *  @return         void
 *  @note           
 *
 *  ��ʼ������������
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
 *  @brief          ���ַ���ӵ�����������
 *  @author         huangjun
 *  @param[in]      c           Ҫ������ַ�
 *  @return         void
 *  @note           
 *
 *  ���ַ���ӵ�����������
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
 *  @brief          ��ʼ������������
 *  @author         huangjun
 *  @return         void
 *  @note           
 *
 *  ��ʼ������������
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
 *  @brief          ���ַ���Ӽ���������
 *  @author         huangjun
 *  @param[in]      c           Ҫ������ַ�
 *  @return         void
 *  @note           
 *
 *  ���ַ���Ӽ���������
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
void CCfgParser::putKeyName(char c)
{
    if ( isspace( c ) )
    {
        // ������ո񻺳���
        m_keyNameSpace += c;
    }
    else
    {
        // ���ո񻺳�����������ӵ� name β��
        m_keyName += m_keyNameSpace;

        // ��տո񻺳���
        m_keyNameSpace = "";

        // ����ַ�
        m_keyName += c;
    }
}

/**
 *  @brief          ��ʼ����ֵ������
 *  @author         huangjun
 *  @return         void
 *  @note           
 *
 *  ��ʼ����ֵ������
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
 *  @brief          ���ַ���Ӽ�ֵ������
 *  @author         huangjun
 *  @param[in]      c           Ҫ������ַ�
 *  @return         void
 *  @note           
 *
 *  ���ַ���Ӽ�ֵ������
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
void CCfgParser::putKeyValue(char c)
{
    if ( isspace( c ) )
    {
        // ������ո񻺳���
        m_keyValueSpace += c;
    }
    else
    {
        // ���ո񻺳�����������ӵ� value β��
        m_keyValue += m_keyValueSpace;

        // ��տո񻺳���
        m_keyValueSpace = "";

        // ����ַ�
        m_keyValue += c;
    }
}



/**
 *  @brief          �Ƿ�Ϊ�ո�
 *  @author         huangjun
 *  @param[in]      c           ���жϵ��ַ�
 *  @return         bool        �Ƿ�Ϊ�ַ�
 *  @note           
 *
 *  �Ƿ�Ϊ�ո�
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
 *  @brief          EPARSERSTATUS_LINE_START ������
 *  @author         huangjun
 *  @param[in]      pThis       ������ָ��
 *  @param[in]      c           Ҫ�������ַ�
 *  @return         bool        �Ƿ�ɹ�
 *  @note           
 *
 *  EPARSERSTATUS_LINE_START ������
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::lineStartHandler( CCfgParser * pThis, char c )
{
    // �п�ʼ
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
        // ���ı�״̬
        return true;
    }

    if ( '#' == c )
    {
        // ������ȡע��״̬
        pThis->m_parserStatus = EPARSERSTATUS_COMMENT;
        return true;
    }

    if ( '[' == c )
    {
        // ����һ���������
        if ( !pThis->groupEnd() )
        {
            // ��ת������״̬
            pThis->m_parserStatus = EPARSERSTATUS_ERROR;
            return false;
        }

        // �����ѻ�ȡ����������״̬
        pThis->m_parserStatus = EPARSERSTATUS_GOT_LEFT_SQUARE;
        return true;
    }

    if ( '=' == c )
    {
        pThis->m_parserStatus = EPARSERSTATUS_ERROR;
        pThis->m_errInfo = "No item name!";
        return false;
    }

    // �����жϼ�ֵ�Ƿ�������
#if 0
    if ( pThis->m_groupName.empty() )
    {
        // ��ʱû����
        pThis->m_parserStatus = EPARSERSTATUS_ERROR;
        pThis->m_errInfo = "Found key not in group!";
        return false;
    }
#endif

    // ��ȡ�������ĵ�һ���ַ�
    pThis->initKeyName();
    pThis->initKeyValue();
    pThis->putKeyName( c );

    // ��ת����ȡ����״̬
    pThis->m_parserStatus = EPARSERSTATUS_GETTING_KEYNAME;
    return true;
}

/**
 *  @brief          EPARSERSTATUS_GOT_LEFT_SQUARE ������
 *  @author         huangjun
 *  @param[in]      pThis       ������ָ��
 *  @param[in]      c           Ҫ�������ַ�
 *  @return         bool        �Ƿ�ɹ�
 *  @note           
 *
 *  EPARSERSTATUS_GOT_LEFT_SQUARE ������
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::gotLeftSquareHandler( CCfgParser * pThis, char c )
{
    // ��ȡ���� '['
    //      '\n'    ->  EPARSERSTATUS_ERROR
    //      '\0'    ->  EPARSERSTATUS_ERROR
    //      ']'     ->  EPARSERSTATUS_ERROR
    //      isspace ->  
    //      *       ->  EPARSERSTATUS_GETTING_GROUP_NAME

    if ( '\n' == c
        || '\0' == c
        || ']' == c)
    {
        // ��ת������״̬
        pThis->m_parserStatus = EPARSERSTATUS_ERROR;
        pThis->m_errInfo = "Expect group name!";
        return false;
    }

    if ( isspace( c ) )
    {
        // �����ڸ�״̬
        // �ȴ������ĵ�һ���ַ�
        return true;
    }

    // ��ȡ�������ĵ�һ���ַ�
    pThis->initGroupName();
    pThis->putGroupName( c );

    // ��ת����ȡ����״̬
    pThis->m_parserStatus = EPARSERSTATUS_GETTING_GROUP_NAME;
    return true;
}

/**
 *  @brief          EPARSERSTATUS_GETTING_GROUP_NAME ������
 *  @author         huangjun
 *  @param[in]      pThis       ������ָ��
 *  @param[in]      c           Ҫ�������ַ�
 *  @return         bool        �Ƿ�ɹ�
 *  @note           
 *
 *  EPARSERSTATUS_GETTING_GROUP_NAME ������
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::gettingGroupNameHandler( CCfgParser * pThis, char c )
{
    // ���ڶ�ȡ����
    //      '\n'    ->  EPARSERSTATUS_ERROR
    //      '\0'    ->  EPARSERSTATUS_ERROR
    //      isspace ->  EPARSERSTATUS_GOT_GROUP_NAME
    //      "]"     ->  EPARSERSTATUS_EXPECT_LINEEND
    //      *       ->  
    if ( '\n' == c
        || '\0' == c )
    {
        // ��ת������״̬
        pThis->m_parserStatus = EPARSERSTATUS_ERROR;
        pThis->m_errInfo = "Expect ']' !";
        return false;
    }

    if ( isspace( c ) )
    {
        // ��ת���ѻ�ȡ����״̬
        // �˴������� groupBegin���ȵ���ȡ�� ']' �ź��ٵ���
        pThis->m_parserStatus = EPARSERSTATUS_GOT_GROUP_NAME;
        return true;
    }

    if ( ']' == c )
    {
        // �������������� groupBegin
        if ( !pThis->groupBegin() )
        {
            pThis->m_parserStatus = EPARSERSTATUS_ERROR;
            return false;
        }

        // ��ת���ڴ����з�״̬
        pThis->m_parserStatus = EPARSERSTATUS_EXPECT_LINEEND;
        return true;
    }

    // ����ַ�������������
    pThis->putGroupName( c );

    // �����ڸ�״̬
    return true;
}

/**
 *  @brief          EPARSERSTATUS_GOT_GROUP_NAME ������
 *  @author         huangjun
 *  @param[in]      pThis       ������ָ��
 *  @param[in]      c           Ҫ�������ַ�
 *  @return         bool        �Ƿ�ɹ�
 *  @note           
 *
 *  EPARSERSTATUS_GOT_GROUP_NAME ������
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::gotGroupNameHandler( CCfgParser * pThis, char c )
{
    // �ѻ���������ڴ� ']'
    //      "]"     ->  EPARSERSTATUS_EXPECT_LINEEND
    //      isspace ->  
    //      '\n'    ->  EPARSERSTATUS_ERROR
    //      '\0'    ->  EPARSERSTATUS_ERROR
    //      *       ->  EPARSERSTATUS_ERROR

    if ( ']' == c )
    {
        // �������������� groupBegin
        if ( !pThis->groupBegin() )
        {
            pThis->m_parserStatus = EPARSERSTATUS_ERROR;
            return false;
        }

        // ��ת���ڴ����з�״̬
        pThis->m_parserStatus = EPARSERSTATUS_EXPECT_LINEEND;
        return true;
    }

    if ( isspace( c ) )
    {
        // �����ڸ�״̬
        return true;
    }

    // ��ת������״̬
    pThis->m_parserStatus = EPARSERSTATUS_ERROR;
    pThis->m_errInfo = "Expect ']' !";
    return false;
}

/**
 *  @brief          EPARSERSTATUS_EXPECT_LINEEND ������
 *  @author         huangjun
 *  @param[in]      pThis       ������ָ��
 *  @param[in]      c           Ҫ�������ַ�
 *  @return         bool        �Ƿ�ɹ�
 *  @note           
 *
 *  EPARSERSTATUS_EXPECT_LINEEND ������
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::expectLinefeedHandler( CCfgParser * pThis, char c )
{
    // �ڴ���ȡ�н�β
    //      '\n'    ->  EPARSERSTATUS_LINE_START
    //      '\0'    ->  EPARSERSTATUS_LINE_START
    //      isspace ->  
    //      *       ->  EPARSERSTATUS_ERROR

    if ( '\n' == c
        || '\0' == c )
    {
        // ��ת������ʼ״̬
        pThis->m_parserStatus = EPARSERSTATUS_LINE_START;
        return true;
    }

    if ( isspace( c ) )
    {
        // �����ڸ�״̬
        return true;
    }

    // ��ת������״̬
    pThis->m_parserStatus = EPARSERSTATUS_ERROR;
    pThis->m_errInfo = "Expect '\\n' !";
    return false;
}

/**
 *  @brief          EPARSERSTATUS_COMMENT ������
 *  @author         huangjun
 *  @param[in]      pThis       ������ָ��
 *  @param[in]      c           Ҫ�������ַ�
 *  @return         bool        �Ƿ�ɹ�
 *  @note           
 *
 *  EPARSERSTATUS_COMMENT ������
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::commentHandler( CCfgParser * pThis, char c )
{
    // ע����
    //      '\n'    ->  EPARSERSTATUS_LINE_START
    //      '\0'    ->  EPARSERSTATUS_LINE_START
    //      *       ->  

    if ( '\n' == c
        || '\0' == c )
    {
        // ��ת������ʼ״̬
        pThis->m_parserStatus = EPARSERSTATUS_LINE_START;
        return true;
    }

    // �����ڸ�״̬
    return true;
}

/**
 *  @brief          EPARSERSTATUS_GETTING_KEYNAME ������
 *  @author         huangjun
 *  @param[in]      pThis       ������ָ��
 *  @param[in]      c           Ҫ�������ַ�
 *  @return         bool        �Ƿ�ɹ�
 *  @note           
 *
 *  EPARSERSTATUS_GETTING_KEYNAME ������
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::gettingKeyNameHandler( CCfgParser * pThis, char c )
{
    // ���ڻ�ȡ����
    //      '='     ->  EPARSERSTATUS_GOT_EQUAL
    //      '\n'    ->  EPARSERSTATUS_ERROR
    //      '\0'    ->  EPARSERSTATUS_ERROR
    //      *       ->  

    if ( '=' == c )
    {
        // ��ת���ѻ�ȡ�Ⱥ�״̬
        pThis->m_parserStatus = EPARSERSTATUS_GOT_EQUAL;
        return true;
    }

    if ( '\n' == c
        || '\0' == c )
    {
        // ��ת������״̬
        pThis->m_parserStatus = EPARSERSTATUS_ERROR;
        pThis->m_errInfo = "Expect '=' !";
        return false;
    }

    // ����ǰ�ַ���ӵ�������������
    pThis->putKeyName( c );

    // �����ڸ�״̬
    return true;
}

/**
 *  @brief          EPARSERSTATUS_GOT_KEYNAME ������
 *  @author         huangjun
 *  @param[in]      pThis       ������ָ��
 *  @param[in]      c           Ҫ�������ַ�
 *  @return         bool        �Ƿ�ɹ�
 *  @note           
 *
 *  EPARSERSTATUS_GOT_KEYNAME ������
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::gotKeyNameHandler( CCfgParser * pThis, char c )
{
    // �Ѿ���ȡ������
    //      '='     ->  EPARSERSTATUS_GOT_EQUAL
    //      isspace ->  
    //      '\n'    ->  EPARSERSTATUS_ERROR
    //      '\0'    ->  EPARSERSTATUS_ERROR
    //      *       ->  EPARSERSTATUS_ERROR

    if ( '=' == c )
    {
        // ��ת���ѻ�ȡ�Ⱥ�״̬
        pThis->m_parserStatus = EPARSERSTATUS_GOT_EQUAL;
        return true;
    }

    if ( isspace( c ) )
    {
        // �����ڸ�״̬
        return true;
    }

    // ��ת������״̬
    pThis->m_parserStatus = EPARSERSTATUS_ERROR;
    pThis->m_errInfo = "Expect '=' !";
    return false;
}

/**
 *  @brief          EPARSERSTATUS_GOT_EQUAL ������
 *  @author         huangjun
 *  @param[in]      pThis       ������ָ��
 *  @param[in]      c           Ҫ�������ַ�
 *  @return         bool        �Ƿ�ɹ�
 *  @note           
 *
 *  EPARSERSTATUS_GOT_EQUAL ������
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::gotEqualHandler( CCfgParser * pThis, char c )
{
    // �Ѿ������ֵ�ָ���
    //      '\n'    ->  EPARSERSTATUS_LINE_START
    //      '\0'    ->  EPARSERSTATUS_LINE_START
    //      isspace ->  
    //      *       ->  EPARSERSTATUS_GETTING_KEYVALUE

    if ( '\n' == c
        || '\0' == c )
    {
        // �����
        if ( !pThis->key() )
        {
            // ��ת������״̬
            pThis->m_parserStatus = EPARSERSTATUS_ERROR;
            return false;
        }
    
        // ��ת������ʼ״̬
        pThis->m_parserStatus = EPARSERSTATUS_LINE_START;
        return true;
    }

    if ( isspace( c ) )
    {
        // �����ڸ�״̬
        return true;
    }

    // ��ȡ����һ����ֵ�ַ�
    pThis->putKeyValue( c );

    // ��ת����ȡ��ֵ״̬
    pThis->m_parserStatus = EPARSERSTATUS_GETTING_KEYVALUE;
    return true;
}

/**
 *  @brief          EPARSERSTATUS_GETTING_KEYVALUE ������
 *  @author         huangjun
 *  @param[in]      pThis       ������ָ��
 *  @param[in]      c           Ҫ�������ַ�
 *  @return         bool        �Ƿ�ɹ�
 *  @note           
 *
 *  EPARSERSTATUS_GETTING_KEYVALUE ������
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::gettingKeyValueHandler( CCfgParser * pThis, char c )
{
    // ���ڻ�ȡ��ֵ
    //      '\n'    ->  EPARSERSTATUS_LINE_START
    //      '\0'    ->  EPARSERSTATUS_LINE_START
    //      *       ->  

    if ( '\n' == c
        || '\0' == c )
    {
        // �����
        if ( !pThis->key() )
        {
            // ��ת������״̬
            pThis->m_parserStatus = EPARSERSTATUS_ERROR;
            return false;
        }
    
        // ��ת������ʼ״̬
        pThis->m_parserStatus = EPARSERSTATUS_LINE_START;
        return true;
    }

    // ��ȡ����һ����ֵ�ַ�
    pThis->putKeyValue( c );

    // �����ڸ�״̬
    return true;
}

/**
 *  @brief          EPARSERSTATUS_ERROR ������
 *  @author         huangjun
 *  @param[in]      pThis       ������ָ��
 *  @param[in]      c           Ҫ�������ַ�
 *  @return         bool        �Ƿ�ɹ�
 *  @note           
 *
 *  EPARSERSTATUS_ERROR ������
 *
 *  @remarks        
 *  @since          2013-6-27
 *  @see            
 */
bool CCfgParser::errorHandler( CCfgParser * pThis, char c )
{
    // ����״̬����
    return false;
}



}


