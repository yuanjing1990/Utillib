#include "CCfgManager.hpp"
#include <string>

namespace wtoeutil{

    CCfgManager::CCfgManager()
    {

    }

    CCfgManager::~CCfgManager()
    {

    }

    bool CCfgManager::groupBegin(const std::string & strGroupName)
    {
        return true;
    }

    bool CCfgManager::key(const std::string & strGroupName, const std::string & strKeyName, const std::string & strKeyValue)
    {
        if (strGroupName.empty() || strKeyName.empty() || strKeyValue.empty())
        {
            return false;
        }

        SParaKeyValue paraKeyValue;
        paraKeyValue.strGroupName = strGroupName;
        paraKeyValue.strkeyName = strKeyName;
        paraKeyValue.strkeyValue = strKeyValue;

        m_keyValue.push_back(paraKeyValue);
        
        return true;
    }

    bool CCfgManager::groupEnd(const std::string & strGroupName)
    {
        return true;
    }

    void CCfgManager::setCfgFileBasePath(const std::string & Path)
    {
        m_basepath.clear();
        m_basepath = Path;
    }

    void CCfgManager::setConfigFiles( std::set<std::string> & filenames )
    {
        m_filename = filenames;
    }

    void CCfgManager::getConfigParas( std::vector<SParaKeyValue> & keyValue )
    {
        m_keyValue = keyValue;
    }

    void CCfgManager::loadCfg(std::map< std::string, std::vector<SParaKeyValue> > &configs)
    {
        if (m_filename.empty())
        {
            return;
        }

        //设置回调函数
        m_cfgParser.setCallBack( boost::dynamic_pointer_cast<ICfgParserCallBack>( shared_from_this() ) );

        std::set<std::string>::iterator it = m_filename.begin();
        for (; it!=m_filename.end(); it++)
        {
            m_keyValue.clear();
            if(m_cfgParser.parse( m_basepath + *it ))
            {
                configs.insert(std::make_pair(*it, m_keyValue));
            }
        }
    }
}
