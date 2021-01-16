#include "crypt.h"
#include <algorithm>
#include <fstream>
#include <math.h>
#include <openssl/aes.h>
#include <vector>

#define YJ_AES_256 256
#define YJ_CRYPT_BLOCK_SIZE (YJ_AES_256 * 1024)

using namespace std;
std::vector<char> key(32);
std::vector<char> iv(16);
DtlogdLogCrypt::DtlogdLogCrypt(std::string keyfile) {
    ifstream ifs(keyfile.c_str(), ios::in | ios::binary);
    if (ifs.is_open()) {
        ifs.read(&key[0], 32);
        ifs.read(&iv[0], 16);
    }
}

DtlogdLogCrypt::~DtlogdLogCrypt(void) {
}

bool DtlogdLogCrypt::encryptFile(std::string srcFileName, std::string destFileName) {
    return _cryptFile(srcFileName, destFileName, true);
}

bool DtlogdLogCrypt::decryptFile(std::string srcFileName, std::string destFileName) {
    return _cryptFile(srcFileName, destFileName, false);
}

bool DtlogdLogCrypt::_cryptFile(std::string srcFileName, std::string destFileName, bool isEncrypt) {
    ifstream ifs(srcFileName.c_str(), ios::in | ios::binary);
    ofstream ofs(destFileName.c_str(), ios::out | ios::trunc | ios::binary);

    std::vector<char> inBuf(YJ_CRYPT_BLOCK_SIZE);
    std::vector<char> outBuf(YJ_CRYPT_BLOCK_SIZE);
    while (ifs.is_open() && !ifs.eof()) {
        ifs.read(&inBuf[0], YJ_CRYPT_BLOCK_SIZE);
        size_t len = ifs.gcount();
        len = ceil((double)len / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;

        AES_KEY aes_key;
        if (isEncrypt) {
            AES_set_encrypt_key((unsigned char *)&key[0], YJ_AES_256, &aes_key);
        } else {
            AES_set_decrypt_key((unsigned char *)&key[0], YJ_AES_256, &aes_key);
        }
        std::vector<char> aes_iv(iv);

        AES_cbc_encrypt((unsigned char *)&inBuf[0], (unsigned char *)&outBuf[0], len, &aes_key, (unsigned char *)&aes_iv[0], isEncrypt ? AES_ENCRYPT : AES_DECRYPT);
        ofs.write(&outBuf[0], len);
    }
    ifs.close();
    ofs.close();
    return true;
}
