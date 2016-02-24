
/** @file
 *  @brief          ���ý���ʵ���ļ�
 *  @author         huangjun
 *  @date           2013-6-20
 *  @version        1.00
 *  @note           
 *
 *  ���ý���ʵ���ļ�
 */

#ifndef CFGPARSER_HPP_
#define CFGPARSER_HPP_

#include "utildef.h"

namespace wtoeutil{


/**
 *  @class          mvd.ICfgParserCallBack
 *  @brief          �������ص��ӿ�
 *  @author         huangjun
 *  @note           
 *
 *  �������ص��ӿ�
 */
interface UTIL_EXPORT ICfgParserCallBack {

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
    virtual ~ICfgParserCallBack()
    {
    }

    /** @brief      �鿪ʼ
     */
    virtual bool groupBegin(const std::string & strGroupName) = 0;

    /** @brief      ֵ
     */
    virtual bool key(const std::string & strGroupName,
                        const std::string & strKeyName,
                        const std::string & strKeyValue) = 0;

    /** @brief      �����
     */
    virtual bool groupEnd(const std::string & strGroupName) = 0;
};

typedef boost::shared_ptr<ICfgParserCallBack>   CSpICfgParserCallBack;


/**
 *  @class          mvd.CCfgParser
 *  @brief          ���ý�����
 *  @author         huangjun
 *  @note           
 *
 *  ���ý�����
 */
class UTIL_EXPORT CCfgParser {

public:
    
    /** @brief      ���캯��
     */
    CCfgParser();

    /** @brief      ��������
     */
    virtual ~CCfgParser();

    /** @brief      ���ûص��ӿ�
     */
    void setCallBack(CSpICfgParserCallBack spCallBack);

    /** @brief      �����ļ�
     */
    bool parse(const std::string & strCfgFilePath);

    /** @brief      ��ȡ������Ϣ
     */
    const std::string & getErrInfo();

    /** @brief      ��ȡ����������
     */
    uint32_t getErrLine();

    /** @brief      ��ȡ����������
     */
    uint32_t getErrCol();

private:
    
    /** @brief      ����״̬
     */
    enum EParserStatus
    {
        EPARSERSTATUS_LINE_START,           ///< �п�ʼ
                                            //      '\n'    ->  
                                            //      '\0'    ->  
                                            //      isspace ->  
                                            //      '#'     ->  EPARSERSTATUS_COMMENT
                                            //      '['     ->  EPARSERSTATUS_GOT_LEFT_SQUARE
                                            //      '='     ->  EPARSERSTATUS_ERROR
                                            //      *       ->  EPARSERSTATUS_GETTING_KEYNAME
        EPARSERSTATUS_GOT_LEFT_SQUARE,      ///< ��ȡ���� '['
                                            //      '\n'    ->  EPARSERSTATUS_ERROR
                                            //      '\0'    ->  EPARSERSTATUS_ERROR
                                            //      ']'     ->  EPARSERSTATUS_ERROR
                                            //      isspace ->  
                                            //      *       ->  EPARSERSTATUS_GETTING_GROUP_NAME
        EPARSERSTATUS_GETTING_GROUP_NAME,   ///< ���ڶ�ȡ����
                                            //      '\n'    ->  EPARSERSTATUS_ERROR
                                            //      '\0'    ->  EPARSERSTATUS_ERROR
                                            //      isspace ->  EPARSERSTATUS_GOT_GROUP_NAME
                                            //      "]"     ->  EPARSERSTATUS_EXPECT_LINEEND
                                            //      *       ->  
        EPARSERSTATUS_GOT_GROUP_NAME,       ///< �ѻ���������ڴ� ']'
                                            //      "]"     ->  EPARSERSTATUS_EXPECT_LINEEND
                                            //      isspace ->  
                                            //      '\n'    ->  EPARSERSTATUS_ERROR
                                            //      '\0'    ->  EPARSERSTATUS_ERROR
                                            //      *       ->  EPARSERSTATUS_ERROR
        EPARSERSTATUS_EXPECT_LINEEND,       ///< �ڴ���ȡ�н�β
                                            //      '\n'    ->  EPARSERSTATUS_LINE_START
                                            //      '\0'    ->  EPARSERSTATUS_LINE_START
                                            //      isspace ->  
                                            //      *       ->  EPARSERSTATUS_ERROR

        EPARSERSTATUS_COMMENT,              ///< ע����
                                            //      '\n'    ->  EPARSERSTATUS_LINE_START
                                            //      '\0'    ->  EPARSERSTATUS_LINE_START
                                            //      *       ->  

        EPARSERSTATUS_GETTING_KEYNAME,      ///< ���ڻ�ȡ����
                                            //      '='     ->  EPARSERSTATUS_GOT_EQUAL
                                            //      '\n'    ->  EPARSERSTATUS_ERROR
                                            //      '\0'    ->  EPARSERSTATUS_ERROR
                                            //      *       ->  

        EPARSERSTATUS_GOT_KEYNAME,          ///< �Ѿ���ȡ������
                                            //      isspace ->  
                                            //      '='     ->  EPARSERSTATUS_GOT_EQUAL
                                            //      '\n'    ->  EPARSERSTATUS_ERROR
                                            //      '\0'    ->  EPARSERSTATUS_ERROR
                                            //      *       ->  EPARSERSTATUS_ERROR

        EPARSERSTATUS_GOT_EQUAL,            ///< �Ѿ������ֵ�ָ���
                                            //      '\n'    ->  EPARSERSTATUS_LINE_START
                                            //      '\0'    ->  EPARSERSTATUS_LINE_START
                                            //      isspace ->  
                                            //      *       ->  EPARSERSTATUS_GETTING_KEYVALUE

        EPARSERSTATUS_GETTING_KEYVALUE,     ///< ���ڻ�ȡ��ֵ
                                            //      '\n'    ->  EPARSERSTATUS_LINE_START
                                            //      '\0'    ->  EPARSERSTATUS_LINE_START
                                            //      *       ->  

        EPARSERSTATUS_ERROR,                ///< ����״̬����

        EPARSERSTATUS_NUMBER
    };

    typedef bool (*StatusHandler)( CCfgParser * pThis, char c );

    /** @brief      �����ص��ӿ�
     */
    CSpICfgParserCallBack   m_spCallBack;

    /** @brief      ����״̬
     */
    EParserStatus           m_parserStatus;

    /** @brief      ����
     */
    std::string             m_groupName;

    /** @brief      ����Ƿ������groupBegin
     */
    bool                    m_bNeedCallGroupBegin;

    /** @brief      ����
     */
    std::string             m_keyName;

    /** @brief      ��ǰ��ȡ�ļ���β���Ŀո�
     *              ֻ���ڶ�ȡ���ǿո�ʱ���Ž������������ӵ� m_keyName ��
     */
    std::string             m_keyNameSpace;

    /** @brief      ��ֵ
     */
    std::string             m_keyValue;

    /** @brief      ��ǰ��ȡ�ļ�ֵβ���Ŀո�
     *              ֻ���ڶ�ȡ���ǿո�ʱ���Ž������������ӵ� m_keyValue ��
     */
    std::string             m_keyValueSpace;

    /** @brief      ������Ϣ
     */
    std::string             m_errInfo;

    /** @brief      ��ǰ�������к�
     */
    uint32_t                m_line;

    /** @brief      ��ǰ�������к�
     */
    uint32_t                m_col;

    /** @brief      ״̬������
     */
    StatusHandler           m_statusHandlers[EPARSERSTATUS_NUMBER];

private:
    
    /** @brief      ��ʼ������
     */
    void initParse();

    /** @brief      ��ȡ��������
     */
    bool groupBegin();

    /** @brief      ��ȡ���˼����ֶ�
     */
    bool key();

    /** @brief      ��ȡ��������
     */
    bool groupEnd();

    /** @brief      ��ʼ������������
     */
    void initGroupName();

    /** @brief      ���ַ���ӵ�����������
     */
    void putGroupName(char c);

    /** @brief      ��ʼ������������
     */
    void initKeyName();

    /** @brief      ���ַ���Ӽ���������
     */
    void putKeyName(char c);

    /** @brief      ��ʼ����ֵ������
     */
    void initKeyValue();

    /** @brief      ���ַ���Ӽ�ֵ������
     */
    void putKeyValue(char c);

    /** @brief      �Ƿ�Ϊ�ո�
     */
    static bool isspace(char c);

private:

    /************************************************************************/
    /* ״̬������ */
    /************************************************************************/

    /** @brief      EPARSERSTATUS_LINE_START ������
     */
    static bool lineStartHandler( CCfgParser * pThis, char c );

    /** @brief      EPARSERSTATUS_GOT_LEFT_SQUARE ������
     */
    static bool gotLeftSquareHandler( CCfgParser * pThis, char c );

    /** @brief      EPARSERSTATUS_GETTING_GROUP_NAME ������
     */
    static bool gettingGroupNameHandler( CCfgParser * pThis, char c );

    /** @brief      EPARSERSTATUS_GOT_GROUP_NAME ������
     */
    static bool gotGroupNameHandler( CCfgParser * pThis, char c );

    /** @brief      EPARSERSTATUS_EXPECT_LINEEND ������
     */
    static bool expectLinefeedHandler( CCfgParser * pThis, char c );

    /** @brief      EPARSERSTATUS_COMMENT ������
     */
    static bool commentHandler( CCfgParser * pThis, char c );

    /** @brief      EPARSERSTATUS_GETTING_KEYNAME ������
     */
    static bool gettingKeyNameHandler( CCfgParser * pThis, char c );

    /** @brief      EPARSERSTATUS_GOT_KEYNAME ������
     */
    static bool gotKeyNameHandler( CCfgParser * pThis, char c );

    /** @brief      EPARSERSTATUS_GOT_EQUAL ������
     */
    static bool gotEqualHandler( CCfgParser * pThis, char c );

    /** @brief      EPARSERSTATUS_GETTING_KEYVALUE ������
     */
    static bool gettingKeyValueHandler( CCfgParser * pThis, char c );

    /** @brief      EPARSERSTATUS_ERROR ������
     */
    static bool errorHandler( CCfgParser * pThis, char c );
};

}

#endif

