#ifndef MODRUBY_UTIL_DECL
#define MODRUBY_UTIL_DECL

#include <cstring>
#include <string>
#include <sstream>
#include <vector>

namespace modruby
{

/// Converts a string to lowercase
std::string to_lower(std::string x);

/// Converts a string to lowercase
std::string to_upper(std::string x);

/// URL encodes a string
std::string url_encode(const char* url);

/// Url decodes a string
/// \param is_query If this is set, decoder will translate '+' to to space.
std::string url_decode(const char* url, int is_query=true);

/// Replace on string with another
template<class T>
int inline replace(T& source, const char* find, const char* replace)
{
    int num=0;
    size_t fLen = strlen(find);
    size_t rLen = strlen(replace);
    for(typename T::size_type pos=0; (pos=source.find(find, pos))!=T::npos; pos+=rLen)
    {
        num++;
        source.replace(pos, fLen, replace);
    }

    return num;
}

std::vector<std::string>& 
split(const std::string &s, char delim, std::vector<std::string> &elems);

} // ! namespace modruby
 
#endif
