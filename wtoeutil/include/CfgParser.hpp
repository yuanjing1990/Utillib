
/** @file
 *  @brief          配置解析实现文件
 *  @author         huangjun
 *  @date           2013-6-20
 *  @version        1.00
 *  @note           
 *
 *  配置解析实现文件
 */

#ifndef CFGPARSER_HPP_
#define CFGPARSER_HPP_

#include "utildef.hpp"
#include <memory>
#include <string>

namespace wtoeutil {

/**
 *  @class          mvd.ICfgParserCallBack
 *  @brief          解析器回调接口
 *  @author         huangjun
 *  @note           
 *
 *  解析器回调接口
 */
interface UTIL_EXPORT ICfgParserCallBack {

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
    virtual ~ICfgParserCallBack() {
    }

    /** @brief      组开始
     */
    virtual bool groupBegin(const std::string &strGroupName) = 0;

    /** @brief      值
     */
    virtual bool key(const std::string &strGroupName,
                     const std::string &strKeyName,
                     const std::string &strKeyValue) = 0;

    /** @brief      组结束
     */
    virtual bool groupEnd(const std::string &strGroupName) = 0;
};

typedef std::shared_ptr<ICfgParserCallBack> CSpICfgParserCallBack;

/**
 *  @class          mvd.CCfgParser
 *  @brief          配置解析器
 *  @author         huangjun
 *  @note           
 *
 *  配置解析器
 */
class UTIL_EXPORT CCfgParser {

  public:
    /** @brief      构造函数
     */
    CCfgParser();

    /** @brief      析构函数
     */
    virtual ~CCfgParser();

    /** @brief      设置回调接口
     */
    void setCallBack(CSpICfgParserCallBack spCallBack);

    /** @brief      解析文件
     */
    bool parse(const std::string &strCfgFilePath);

    /** @brief      获取错误信息
     */
    const std::string &getErrInfo();

    /** @brief      获取错误所在行
     */
    uint32_t getErrLine();

    /** @brief      获取错误所在列
     */
    uint32_t getErrCol();

  private:
    /** @brief      解析状态
     */
    enum EParserStatus {
        EPARSERSTATUS_LINE_START,         ///< 行开始
                                          //      '\n'    ->
                                          //      '\0'    ->
                                          //      isspace ->
                                          //      '#'     ->  EPARSERSTATUS_COMMENT
                                          //      '['     ->  EPARSERSTATUS_GOT_LEFT_SQUARE
                                          //      '='     ->  EPARSERSTATUS_ERROR
                                          //      *       ->  EPARSERSTATUS_GETTING_KEYNAME
        EPARSERSTATUS_GOT_LEFT_SQUARE,    ///< 获取到了 '['
                                          //      '\n'    ->  EPARSERSTATUS_ERROR
                                          //      '\0'    ->  EPARSERSTATUS_ERROR
                                          //      ']'     ->  EPARSERSTATUS_ERROR
                                          //      isspace ->
                                          //      *       ->  EPARSERSTATUS_GETTING_GROUP_NAME
        EPARSERSTATUS_GETTING_GROUP_NAME, ///< 正在读取组名
                                          //      '\n'    ->  EPARSERSTATUS_ERROR
                                          //      '\0'    ->  EPARSERSTATUS_ERROR
                                          //      isspace ->  EPARSERSTATUS_GOT_GROUP_NAME
                                          //      "]"     ->  EPARSERSTATUS_EXPECT_LINEEND
                                          //      *       ->
        EPARSERSTATUS_GOT_GROUP_NAME,     ///< 已获得组名，期待 ']'
                                          //      "]"     ->  EPARSERSTATUS_EXPECT_LINEEND
                                          //      isspace ->
                                          //      '\n'    ->  EPARSERSTATUS_ERROR
                                          //      '\0'    ->  EPARSERSTATUS_ERROR
                                          //      *       ->  EPARSERSTATUS_ERROR
        EPARSERSTATUS_EXPECT_LINEEND,     ///< 期待获取行结尾
                                          //      '\n'    ->  EPARSERSTATUS_LINE_START
                                          //      '\0'    ->  EPARSERSTATUS_LINE_START
                                          //      isspace ->
                                          //      *       ->  EPARSERSTATUS_ERROR

        EPARSERSTATUS_COMMENT, ///< 注释行
                               //      '\n'    ->  EPARSERSTATUS_LINE_START
                               //      '\0'    ->  EPARSERSTATUS_LINE_START
                               //      *       ->

        EPARSERSTATUS_GETTING_KEYNAME, ///< 正在获取键名
                                       //      '='     ->  EPARSERSTATUS_GOT_EQUAL
                                       //      '\n'    ->  EPARSERSTATUS_ERROR
                                       //      '\0'    ->  EPARSERSTATUS_ERROR
                                       //      *       ->

        EPARSERSTATUS_GOT_KEYNAME, ///< 已经获取到键名
                                   //      isspace ->
                                   //      '='     ->  EPARSERSTATUS_GOT_EQUAL
                                   //      '\n'    ->  EPARSERSTATUS_ERROR
                                   //      '\0'    ->  EPARSERSTATUS_ERROR
                                   //      *       ->  EPARSERSTATUS_ERROR

        EPARSERSTATUS_GOT_EQUAL, ///< 已经获得名值分隔符
                                 //      '\n'    ->  EPARSERSTATUS_LINE_START
                                 //      '\0'    ->  EPARSERSTATUS_LINE_START
                                 //      isspace ->
                                 //      *       ->  EPARSERSTATUS_GETTING_KEYVALUE

        EPARSERSTATUS_GETTING_KEYVALUE, ///< 正在获取键值
                                        //      '\n'    ->  EPARSERSTATUS_LINE_START
                                        //      '\0'    ->  EPARSERSTATUS_LINE_START
                                        //      *       ->

        EPARSERSTATUS_ERROR, ///< 解析状态错误

        EPARSERSTATUS_NUMBER
    };

    typedef bool (*StatusHandler)(CCfgParser *pThis, char c);

    /** @brief      解析回调接口
     */
    CSpICfgParserCallBack m_spCallBack;

    /** @brief      解析状态
     */
    EParserStatus m_parserStatus;

    /** @brief      组名
     */
    std::string m_groupName;

    /** @brief      标记是否调用了groupBegin
     */
    bool m_bNeedCallGroupBegin;

    /** @brief      键名
     */
    std::string m_keyName;

    /** @brief      当前读取的键名尾部的空格
     *              只有在读取到非空格时，才将这里的内容添加到 m_keyName 中
     */
    std::string m_keyNameSpace;

    /** @brief      键值
     */
    std::string m_keyValue;

    /** @brief      当前读取的键值尾部的空格
     *              只有在读取到非空格时，才将这里的内容添加到 m_keyValue 中
     */
    std::string m_keyValueSpace;

    /** @brief      错误信息
     */
    std::string m_errInfo;

    /** @brief      当前解析的行号
     */
    uint32_t m_line;

    /** @brief      当前解析的列号
     */
    uint32_t m_col;

    /** @brief      状态处理器
     */
    StatusHandler m_statusHandlers[EPARSERSTATUS_NUMBER];

  private:
    /** @brief      初始化解析
     */
    void initParse();

    /** @brief      获取到了组名
     */
    bool groupBegin();

    /** @brief      获取到了键名字对
     */
    bool key();

    /** @brief      获取到了组名
     */
    bool groupEnd();

    /** @brief      初始化组名缓冲区
     */
    void initGroupName();

    /** @brief      将字符添加到组名缓冲区
     */
    void putGroupName(char c);

    /** @brief      初始化键名缓冲区
     */
    void initKeyName();

    /** @brief      将字符添加键名缓冲区
     */
    void putKeyName(char c);

    /** @brief      初始化键值缓冲区
     */
    void initKeyValue();

    /** @brief      将字符添加键值缓冲区
     */
    void putKeyValue(char c);

    /** @brief      是否为空格
     */
    static bool isspace(char c);

  private:
    /************************************************************************/
    /* 状态处理函数 */
    /************************************************************************/

    /** @brief      EPARSERSTATUS_LINE_START 处理函数
     */
    static bool lineStartHandler(CCfgParser *pThis, char c);

    /** @brief      EPARSERSTATUS_GOT_LEFT_SQUARE 处理函数
     */
    static bool gotLeftSquareHandler(CCfgParser *pThis, char c);

    /** @brief      EPARSERSTATUS_GETTING_GROUP_NAME 处理函数
     */
    static bool gettingGroupNameHandler(CCfgParser *pThis, char c);

    /** @brief      EPARSERSTATUS_GOT_GROUP_NAME 处理函数
     */
    static bool gotGroupNameHandler(CCfgParser *pThis, char c);

    /** @brief      EPARSERSTATUS_EXPECT_LINEEND 处理函数
     */
    static bool expectLinefeedHandler(CCfgParser *pThis, char c);

    /** @brief      EPARSERSTATUS_COMMENT 处理函数
     */
    static bool commentHandler(CCfgParser *pThis, char c);

    /** @brief      EPARSERSTATUS_GETTING_KEYNAME 处理函数
     */
    static bool gettingKeyNameHandler(CCfgParser *pThis, char c);

    /** @brief      EPARSERSTATUS_GOT_KEYNAME 处理函数
     */
    static bool gotKeyNameHandler(CCfgParser *pThis, char c);

    /** @brief      EPARSERSTATUS_GOT_EQUAL 处理函数
     */
    static bool gotEqualHandler(CCfgParser *pThis, char c);

    /** @brief      EPARSERSTATUS_GETTING_KEYVALUE 处理函数
     */
    static bool gettingKeyValueHandler(CCfgParser *pThis, char c);

    /** @brief      EPARSERSTATUS_ERROR 处理函数
     */
    static bool errorHandler(CCfgParser *pThis, char c);
};

} // namespace wtoeutil

#endif
