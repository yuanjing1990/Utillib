#include "wtoelib.h"
#include <iostream>
int main(int argc,char* argv[])
{
    wtoeutil::MD5 md5("12345",6);
    std::cout << "seed 12345===>";
    std::cout << md5.toString() << std::endl;

    md5.update("54321",6);
    std::cout << "seed 54321===>";
    std::cout << md5.toString() << std::endl;

    md5.update("12345",6);
    std::cout << "seed 12345===>";
    std::cout << md5.toString() << std::endl;
}
