#include <stdio.h>

#include <algorithm>
#include <functional>
#include <locale>

#include "util.hpp"

using namespace std;

namespace modruby
{

string to_lower(string x)
{
    // locale utfFile("en_US.UTF-8");
    transform( x.begin(), x.end(), x.begin(),
               bind2nd( ptr_fun(&std::tolower<char>),
                        locale("") ) );

    return x;
}

string to_upper(string x)
{
    // locale utfFile("en_US.UTF-8");
    transform( x.begin(), x.end(), x.begin(),
               bind2nd( ptr_fun(&std::toupper<char>),
                        locale("") ) );

    return x;
}

std::vector<std::string>&
split(const std::string& s, char delim, std::vector<std::string>& elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }

    return elems;
}

// Next two functions hext2int() and buffer::url_decode() (taken from
// buffer_urldecode_internal()) are from lighttpd-1.4.15, Copyright (c) 2004,
// Jan Kneschke, incremental All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// - Neither the name of the 'incremental' nor the names of its contributors may
//   be used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

static char hex2int(unsigned char hex)
{
    hex = hex - '0';
    if (hex > 9)
    {
        hex = (hex + '0' - 1) | 0x20;
        hex = hex - 'a' + 11;
    }
    if (hex > 15)
    {
        hex = 0xFF;
    }

    return hex;
}

string url_decode(const char* url, int is_query)
{
    unsigned char high, low;

    if (!url)
    {
        return "";
    }

    char* str       = (char*)strdup(url);
    const char* src = (const char*)str;
    char* dst       = (char*)str;

    while ((*src) != '\0')
    {
        if (is_query && *src == '+')
        {
            *dst = ' ';
        }
        else if (*src == '%')
        {
            *dst = '%';

            high = hex2int(*(src + 1));

            if (high != 0xFF)
            {
                low = hex2int(*(src + 2));
                if (low != 0xFF)
                {
                    high = (high << 4) | low;

                    /* map control-characters out */
                    //if (high < 32 || high == 127) high = '_';

                    *dst = high;
                    src += 2;
                }
            }
        }
        else
        {
            *dst = *src;
        }

        dst++;
        src++;
    }

    *dst = '\0';

    string decoded = str;

    free((void*)str);

    return decoded;
}

// Adapted from urlencode.c from:
//
// http://www.mp3tunes.com/download/oboe_sdk.zip/apis/liboboe/src/
//
// Copyright (c) 2002-3, Andrew McNab, University of Manchester All rights
// reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     o Redistributions of source code must retain the above
//       copyright notice, this list of conditions and the following
//       disclaimer.
//     o Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials
//       provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

string url_encode(const char* url)
{
    // Get the srouce string
    const char* src = url;
    const char* str = (const char*)malloc(strlen(url) * 3);
    char* dst       = (char*)str;

    while (*src != '\0')
    {
        if (isalnum(*src) || (*src == '.') || (*src == '_') || (*src == '-'))
        {
            *dst = *src;
            dst++;
        }
        else if (*src == ' ')
        {
            *dst = '+';
            dst++;
        }
        else
        {
            sprintf(dst, "%%%2X", *src);
            dst = &dst[3];
        }

        ++src;
    }

    *dst = '\0';

    string encoded = str;

    free((void*)str);

    return encoded;
}

} // ! namespace modruby
