#ifndef MD5_HPP_
#define MD5_HPP_
#include <fstream>

#include "utildef.hpp"

namespace wtoeutil {
/* Type define */
typedef unsigned char byte;
typedef unsigned int uint32;

using std::ifstream;
using std::string;

/* MD5 declaration. */
class UTIL_EXPORT MD5 {
  public:
    /*
     * Default Constructor
     */
    MD5();

    /*
     * Constructor with length and value
     */
    MD5(const void *input, size_t length);

    /*
     * Constructor with string
     */
    MD5(const string &str);

    /*
     * Constructor with input stream
     */
    MD5(ifstream &in);

    /*
     * Update the value with input
     */
    string update(const void *input, size_t length);

    /*
     * Update the value with string
     */
    string update(const string &str);

    /*
     * Update the value with input stream
     */
    string update(ifstream &in);

    /*
     * Compute the value and return byte's MD5
     */
    const byte *digest();

    /*
     * Return MD5 with string
     */
    string toString();

    /*
     * Reset the MD5
     */
    void reset();

  private:
    void update(const byte *input, size_t length);
    void final();
    void transform(const byte block[64]);
    void encode(const uint32 *input, byte *output, size_t length);
    void decode(const byte *input, uint32 *output, size_t length);
    string bytesToHexString(const byte *input, size_t length);

    /* class uncopyable */
    MD5(const MD5 &);
    MD5 &operator=(const MD5 &);

  private:
    uint32 _state[4]; /* state (ABCD) */
    uint32 _count[2]; /* number of bits, modulo 2^64 (low-order word first) */
    byte _buffer[64]; /* input buffer */
    byte _digest[16]; /* message digest */
    bool _finished;   /* calculate finished ? */

    static const byte PADDING[64]; /* padding for calculate */
    static const char HEX[16];
    enum {
        BUFFER_SIZE = 1024
    };
};

} // namespace wtoeutil
#endif /*MD5_H*/
