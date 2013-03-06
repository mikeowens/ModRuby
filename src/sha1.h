#ifndef MODRUBY_SHA1_DECLARE
#define MODRUBY_SHA1_DECLARE

#include "apr.h"
#include "common.h"

namespace modruby
{

/** This class encapsulates an SHA1 hash */
class sha1
{
    apr_sha1_ctx_t _context;
    unsigned char _sha1[APR_SHA1_DIGESTSIZE];
    unsigned char _hash[41];

  public:

    sha1();
    ~sha1();

    bool hashdata(const char* data, unsigned int len);
    bool hashfile(const char* path);

    const unsigned char* hash() const
    {
        return _sha1;
    }

    const char* result() const
    {
        return (const char*)_hash;
    }

    /** Start SHA1 accumulation.  Set bit count to 0 and buffer to mysterious
     * initialization constants.
     */
    void init();

    /** Update context to reflect the concatenation of another buffer full
     * of bytes.
     */
    void update(const char *buf, unsigned int len);

    /** Final wrapup - pad to 64-byte boundary with the bit pattern 1 0* (64-bit
     * count of bits processed, MSB-first)
     */
    void final();
};

} // end namespace

#endif
