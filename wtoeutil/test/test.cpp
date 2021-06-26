#include "wtoelib.hpp"
#include <iostream>
void print(std::string str) {
    std::cout << str << std::endl;
}
int main(int argc, char *argv[]) {
    std::cout << "MD5 test-------------------\n";
    wtoeutil::MD5 md5("12345", 6);
    std::cout << "seed 12345===>";
    std::cout << md5.toString() << std::endl;

    md5.update("54321", 6);
    std::cout << "seed 54321===>";
    std::cout << md5.toString() << std::endl;

    md5.update("12345", 6);
    std::cout << "seed 12345===>";
    std::cout << md5.toString() << std::endl;
    std::cout << "MD5 test-------------------\n";

    std::cout << "FileSystem test-------------------\n";
    std::string dir(".");
    std::string parentDir;
    wtoeutil::getCurrentPath(dir);
    std::cout << "getCurrentPath=" << dir << std::endl;
    wtoeutil::getParentPath(dir, parentDir);
    std::cout << "getParentPath=" << parentDir << std::endl;
    if (wtoeutil::getParentPath(dir, parentDir, 0))
        std::cout << "getParentPath lv0=" << parentDir << std::endl;
    if (wtoeutil::getParentPath(dir, parentDir, 1))
        std::cout << "getParentPath lv1=" << parentDir << std::endl;
    if (wtoeutil::getParentPath(dir, parentDir, 2))
        std::cout << "getParentPath lv2=" << parentDir << std::endl;
    if (wtoeutil::getParentPath(dir, parentDir, 3))
        std::cout << "getParentPath lv3=" << parentDir << std::endl;
    if (wtoeutil::getParentPath(dir, parentDir, 4))
        std::cout << "getParentPath lv4=" << parentDir << std::endl;
    std::set<std::string> fileset;
    wtoeutil::getDirFileEntry(fileset, dir);
    std::for_each(fileset.begin(), fileset.end(), print);
    std::cout << "FileSystem test-------------------\n";

    std::cout << "CfgParser test-------------------\n";
    boost::shared_ptr<wtoeutil::CCfgManager> mgr = boost::make_shared<wtoeutil::CCfgManager>();
    mgr->setCfgFileBasePath("./");
    std::set<std::string> files;
    files.insert("test.cfg");
    mgr->setConfigFiles(files);
    std::map<std::string, std::vector<SParaKeyValue>> cfgMap;
    mgr->loadCfg(cfgMap);
    for (std::map<std::string, std::vector<SParaKeyValue>>::iterator it = cfgMap.begin(); it != cfgMap.end(); ++it) {
        std::vector<SParaKeyValue> vec = it->second;
        std::cout << "[" << it->first << "]\n";
        for (int i = 0; i < vec.size(); ++i) {
            SParaKeyValue &val = vec.at(i);
            std::cout << val.strGroupName << "," << val.strkeyName << "," << val.strkeyValue << std::endl;
        }
    }
    std::cout << "CfgParser test-------------------\n";
}
