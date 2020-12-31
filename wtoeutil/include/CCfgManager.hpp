#ifndef CFGMANAGER_HPP_
#define CFGMANAGER_HPP_

#include <boost/enable_shared_from_this.hpp>
#include <string>
#include <set>
#include <vector>
#include <map>
#include "CfgParser.hpp"
#include "utildef.hpp"
    
struct SParaKeyValue{
    std::string strGroupName;
    std::string strkeyName;
    std::string strkeyValue;
    SParaKeyValue():
        strGroupName(""),strkeyName(""),strkeyValue("")
    {}
    bool operator == (const SParaKeyValue& para) const
    {
        if(strGroupName.compare(para.strGroupName) == 0 &&
           strkeyName.compare(para.strkeyName) == 0 )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};
    
namespace wtoeutil{
 
class UTIL_EXPORT CCfgManager : public ICfgParserCallBack
                                        , public boost::enable_shared_from_this<CCfgManager>
{
public:
    /** @brief      构造函数
     */
    CCfgManager();

    /** @brief      析构函数
     */
    virtual ~CCfgManager();

public:
    /************************************************************************/
    /* ICfgParserCallBack 接口*/
    /************************************************************************/

    /** @brief      组解析开始
     */
    virtual bool groupBegin(const std::string & strGroupName);

    /** @brief      值解析
     */
    virtual bool key(const std::string & strGroupName,
                        const std::string & strKeyName,
                        const std::string & strKeyValue);

    /** @brief      组解析结束
     */
    virtual bool groupEnd(const std::string & strGroupName);

public:
    /** @brief      设置配置文件的路径
     */
    void setCfgFileBasePath(const std::string & BasePath);

    /** @brief      设置读取文件列表
     */
    void setConfigFiles( std::set<std::string> & filenames );


    /** @brief      获取配置参数
     */
    void getConfigParas( std::vector<SParaKeyValue> & keyValue );

    /** @brief      获取所有配置文件参数
     */
    void loadCfg(std::map< std::string, std::vector<SParaKeyValue > > &configs);

private:
    std::string m_basepath;                         //文件所在路径
    CCfgParser  m_cfgParser;                        //配置文件解析器
    std::set<std::string> m_filename;               //所有配置文件名称
    std::vector<SParaKeyValue> m_keyValue;          //配置文件键值对
};


}
#endif
