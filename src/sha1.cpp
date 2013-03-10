#include "sha1.h"

namespace modruby
{

sha1::sha1()
{

}

bool sha1::hashdata(const char* data, unsigned int len)
{
    init();
    update(data, len);
    final();

    return true;
}

bool sha1::hashfile(const char* path)
{
    init();

    FILE *in;
    char zBuf[10240];

    if(access(path, R_OK) != 0)
    {
        //string msg = (string)"sha1() : No such file: " << path;

        // TODO
        //throw std::exception(msg.string());
        return false;
    }

    in = fopen(path,"rb");

    if(in == 0)
    {
        printf("sha1_test : cannot open file %s", path);
    }
    
    for(;;)
    {
        int n;
        
        n = fread(zBuf, 1, sizeof(zBuf), in);
        
        if(n <= 0)
        {
            break;
        }
        
        update(zBuf, (unsigned)n);
    }
    
    fclose(in);
    final();

    return true;
}

sha1::~sha1()
{

}

void sha1::init()
{
    apr_sha1_init(&_context);
}

void sha1::update(const char *buf, unsigned int len)
{
    apr_sha1_update_binary(&_context, (const unsigned char*)buf, len);
}

void sha1::final()
{
    apr_sha1_final(&_sha1[0], &_context);

    // Compute the hex value
    i32 i, j;
    static char const zEncode[] = "0123456789abcdef";

    j = 0;
    for(i = 0; i < 20; i++)
    {
        i32 a = (unsigned char)_sha1[i];
        _hash[j++] = zEncode[(a>>4)&0xf];
        _hash[j++] = zEncode[a & 0xf];
    }

    _hash[40] = '\0';
}

} // end namespace
