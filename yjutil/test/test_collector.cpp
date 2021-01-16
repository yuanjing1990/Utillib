#include <algorithm>
#include <boost/filesystem.hpp>
#include <fstream>
#include <gtest/gtest.h>
#include <queue>
#include <stack>
#include <string>

#include "yjcollector.h"
#include "yjcollector_mgr.h"
#include "yjcollector_policy.h"

using namespace yjutil;
using namespace boost::filesystem;

static std::string sSrcPath = "/tmp/testyjutil/";
static std::string sDestPath = "/tmp/testyjutil/copy_to/";
class TestCollector : public ::testing::Test {
    public:
        TestCollector(){}
        virtual ~TestCollector(){}

        virtual void SetUp() {
            DEBUG_PRINT("SetUp ... ");
            boost::filesystem::create_directories(sSrcPath);
            std::ofstream ofs(sSrcPath + "testfile.txt");
            ofs << "test";
            ofs.close();
        }

        virtual void TearDown() {
            DEBUG_PRINT("TearDown ... ");
            boost::filesystem::remove_all(sSrcPath);
        }
};

TEST_F(TestCollector, test_rename_policy) {
    std::string anypath("/path/to/anything");

    RenamePolicy renamePolicy(nullptr);
    ASSERT_TRUE(0 == anypath.compare(renamePolicy.rename(anypath)));

    RenamePolicyImplSp renamePolicyImpl(new RenamePolicyImpl());
    ASSERT_TRUE(0 == anypath.compare(renamePolicyImpl->rename(anypath)));

    RenamePolicy renamePolicyWithImpl(new RenamePolicyImpl());
    ASSERT_TRUE(0 == anypath.compare(renamePolicyWithImpl.rename(anypath)));
}

TEST_F(TestCollector, test_filter_policy) {
    std::string anypath("/path/to/anything");

    FilterPolicy filterPolicy(nullptr);
    ASSERT_FALSE(filterPolicy.filter(anypath));

    FilterPolicyImplSp filterPolicyImpl(new FilterPolicyImpl());
    ASSERT_FALSE(filterPolicyImpl->filter(anypath));

    FilterPolicy filterPolicyWithImpl(new FilterPolicyImpl());
    ASSERT_FALSE(filterPolicyWithImpl.filter(anypath));
}

TEST_F(TestCollector, test_copy_policy) {
    std::string srcFile("test_copy_policy");
    std::string sDestPath(sSrcPath + "test_copy_to/");

    std::string srcFilePath = sSrcPath + srcFile;
    std::ofstream ofs(srcFilePath);
    ofs << "test";
    ofs.close();
    ASSERT_TRUE(exists(srcFilePath));
    ASSERT_FALSE(exists(sDestPath + srcFile));

    CopyPolicyImpl copyPolicyImpl;
    ASSERT_FALSE(copyPolicyImpl.copy(srcFilePath, "/not_src_path/", sDestPath, RenamePolicy(nullptr)));
    ASSERT_FALSE(exists(sDestPath + srcFile));

    ASSERT_FALSE(copyPolicyImpl.copy(srcFilePath, sSrcPath, "/dest_not_exist/", RenamePolicy(nullptr)));

    create_directories(sDestPath);
    ASSERT_TRUE(copyPolicyImpl.copy(srcFilePath, sSrcPath, sDestPath, RenamePolicy(nullptr)));
    ASSERT_TRUE(exists(sDestPath + srcFile));
    remove_all(sDestPath);
}

TEST_F(TestCollector, test_filecollector_docollect) {
    boost::shared_ptr<Collector> collector(new FileCollector(sSrcPath, sDestPath, new CopyPolicyImpl(), new FilterPolicyImpl(), new RenamePolicyImpl()));
    ASSERT_FALSE(collector->doCollect());

    create_directories(sDestPath);
    ASSERT_TRUE(collector->doCollect());
    remove_all(sDestPath);
}

// ===================================================================================
/*
输入第一行包含一个字符串s，代表压缩后的字符串。
S的长度<=1000;
S仅包含大写字母、[、]、|;
解压后的字符串长度不超过100000;
压缩递归层数不超过10层;
*/
void test() {
    std::string inStr;
    std::cin >> inStr;

    std::string lastMod;
    std::string unpackStr;
    std::stack<std::string> unpackStack;
    int pos = 0;
    for (int i = 0; i < inStr.length(); ++i) {
        if ((inStr.at(i) == '[' || inStr.at(i) == '|')) {
            // std::cout << inStr.substr(pos, i - pos) << std::endl;
            unpackStack.push(inStr.substr(pos, i - pos));
            pos = i + 1;
        } else if (inStr.at(i) == ']') {
            std::string mod = inStr.substr(pos, i - pos);
            if (mod.empty()) {
                lastMod = unpackStack.top();
                unpackStack.pop();
            } else {
                lastMod = mod;
            }

            int num = 0;

            while ((num = atoi(unpackStack.top().c_str())) == 0) {
                lastMod = unpackStack.top() + lastMod;
                unpackStack.pop();
            }
            while (num = atoi(unpackStack.top().c_str())) {
                unpackStack.pop();
                std::string buf;
                for (int j = 0; j < num; ++j) {
                    buf.append(lastMod);
                }
                lastMod = buf;
                // std::cout << "lastMod:" << lastMod << std::endl;
            }
            lastMod = unpackStack.top() + lastMod;
            // std::cout << "out lastMod:" << lastMod << std::endl;
            unpackStack.pop();
            unpackStack.push(lastMod);
            pos = i + 1;
        }
    }
    unpackStack.push(inStr.substr(pos, inStr.length() - pos));
    while (!unpackStack.empty()) {
        unpackStr = unpackStack.top() + unpackStr;
        unpackStack.pop();
    }

    std::cout << unpackStr << std::endl;
}

int find_max_from_begin(std::vector<int> &vec, int b, int e) {
    int max = vec.at(b);
    size_t i, index = b;
    for (i = b + 1; i <= e; i++) {
        if (vec.at(i) > max) {
            max = vec.at(i);
            index = i;
        }
    }
    return index;
}

int find_max_from_end(std::vector<int> &vec, int b, int e) {
    int max = vec.at(e);
    size_t i, index = e;
    for (i = e - 1; i >= b; i++) {
        if (vec.at(i) > max) {
            max = vec.at(i);
            index = i;
        }
    }
    return index;
}

int get_result(std::vector<int> &vec, std::vector<int> &result) {
    for (size_t i = 0; i < vec.size(); i++) {
        int r = vec.size() - 1;
        int cnt_after = 0;
        do {
            if (i + 1 >= vec.size() - 1)
                break;
            r = find_max_from_begin(vec, i, r) - 1;
            ++cnt_after;
        } while (r != i + 1);

        std::cout << "i=" << i << ":cnt_after=" << cnt_after << std::endl;

        int cnt_before = 0;
        r = 0;
        do {
            if (i <= 0)
                break;
            r = find_max_from_end(vec, r, i) + 1;
            ++cnt_before;
        } while (r != i - 1);
        std::cout << "i=" << i << ":cnt_before=" << cnt_before << std::endl;

        result.push_back(cnt_after + cnt_before + 1);
        break;
    }
}

void test_mytest2() {

    std::vector<int> vec;
    int n = 0;
    std::cin >> n;
    int high = 0;
    for (int i = 0; i < n; ++i) {
        std::cin >> high;
        vec.push_back(high);
    }

    std::copy(vec.begin(), vec.end(), std::ostream_iterator<int>(std::cout, ","));
    std::cout << std::endl;

    std::vector<int> result;
    std::cout << find_max_from_begin(vec, 0, vec.size() - 1) << std::endl;
    get_result(vec, result);

    std::copy(result.begin(), result.end(), std::ostream_iterator<int>(std::cout, ","));
}