#include <string>
class DtlogdLogCrypt {
  public:
    DtlogdLogCrypt(std::string keyfile);
    ~DtlogdLogCrypt(void);

    bool encryptFile(std::string srcFileName, std::string destFileName);

    bool decryptFile(std::string srcFileName, std::string destFileName);

  private:
    bool _cryptFile(std::string srcFileName, std::string destFileName, bool isEncrypt);
};