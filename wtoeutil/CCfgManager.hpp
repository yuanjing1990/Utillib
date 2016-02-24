#ifndef CFGMANAGER_HPP_
#define CFGMANAGER_HPP_

#include <boost/enable_shared_from_this.hpp>
#include <string>
#include <set>
#include <vector>
#include <map>
#include "CfgParser.hpp"
#include "utildef.h"
    
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
    /** @brief      ���캯��
     */
    CCfgManager();

    /** @brief      ��������
     */
    virtual ~CCfgManager();

public:
    /************************************************************************/
    /* ICfgParserCallBack �ӿ�*/
    /************************************************************************/

    /** @brief      �������ʼ
     */
    virtual bool groupBegin(const std::string & strGroupName);

    /** @brief      ֵ����
     */
    virtual bool key(const std::string & strGroupName,
                        const std::string & strKeyName,
                        const std::string & strKeyValue);

    /** @brief      ���������
     */
    virtual bool groupEnd(const std::string & strGroupName);

public:
    /** @brief      ���������ļ���·��
     */
    void setCfgFileBasePath(const std::string & BasePath);

    /** @brief      ���ö�ȡ�ļ��б�
     */
    void setConfigFiles( std::set<std::string> & filenames );


    /** @brief      ��ȡ���ò���
     */
    void getConfigParas( std::vector<SParaKeyValue> & keyValue );

    /** @brief      ��ȡ���������ļ�����
     */
    void loadCfg(std::map< std::string, std::vector<SParaKeyValue > > &configs);

private:
    std::string m_basepath;                         //�ļ�����·��
    CCfgParser  m_cfgParser;                        //�����ļ�������
    std::set<std::string> m_filename;               //���������ļ�����
    std::vector<SParaKeyValue> m_keyValue;          //�����ļ���ֵ��
};


}
#endif
