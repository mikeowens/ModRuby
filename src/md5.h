#ifndef MD5_DECLARE
#define MD5_DECLARE

#include "common.h"

#include <string>

namespace modruby
{

/*
 * If compiled on a machine that doesn't have a 32-bit integer,
 * you just set "uint32" to the appropriate datatype for an
 * unsigned 32-bit integer.  For example:
 *
 *       cc -Duint32='unsigned long' md5.c
 *
 */
#ifndef uint32
#  define uint32 unsigned int
#endif

struct Context
{
  uint32 buf[4];
  uint32 bits[2];
  unsigned char in[64];
};

typedef char MD5Context[88];

/* The four core functions - F1 is optimized somewhat */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f, w, x, y, z, data, s) \
        ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

/** This class encapsulates an MD5 hash */
class md5
{
    MD5Context context;
    unsigned char _hash[16];
    char _hash_str[33];

  public:

    md5();
    md5(const char* path);
    ~md5();

    const unsigned char* hash() const
    {
        return _hash;
    }

    const char* result() const
    {
        return _hash_str;
    }

    /** Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
     * initialization constants.
     */
    void init();

    /** Update context to reflect the concatenation of another buffer full
     * of bytes.
     */
    void update(const char *buf, unsigned int len);

    /** Generates intermediate values between updates for debugging. */
    std::string debug();

    /** Final wrapup - pad to 64-byte boundary with the bit pattern 1 0* (64-bit
     * count of bits processed, MSB-first)
     */
    void final();

    /** The core of the MD5 algorithm, this alters an existing MD5 hash to
     * reflect the addition of 16 longwords of new data.  MD5Update blocks the
     * data * and converts bytes into longwords for this routine.
     */
    static void transform(uint32 buf[4], const uint32 in[16]);

    /** Convert a digest into base-16.  digest should be declared as "unsigned
     * char digest[16]" in the calling function.  The MD5 digest is stored in
     * the first 16 bytes.  zBuf should be "char zBuf[33]".
     */
    void DigestToBase16(unsigned char *digest, char *zBuf);

    static void byteReverse (unsigned char *buf, unsigned longs);
};

} // end namespace

#endif
