#include <stdlib.h>

#include <string>
#include <sstream>

#include "apr_table.h"

using std::string;
using std::stringstream;

namespace modruby
{
namespace apr
{

// APR Table

table::table(apr_pool_t* p)
{
    handle = apr_table_make(p, 5);
}

table::table(apr_table_t* t) : handle(t)
{

}

table::~table()
{

}

const table& table::operator=(apr_table_t* t)
{
    handle = t;
    
    return *this;
}

const table& table::operator=(const table& t)
{
    if(this != &t)
    {
        handle = t.handle;
    }
    
    return *this;
}

table::iterator table::operator[](i32 idx) const
{
    // const apr_array_header_t* h = apr_table_elts(handle);

    iterator i(*this);

    i.seek(idx);

    return i;
}

const char* table::operator[](const char* key) const
{
    return apr_table_get(handle, key);
}

const apr_array_header_t* table::begin()
{
    return apr_table_elts(handle);
}

u32 table::size() const
{
    const apr_array_header_t* h = apr_table_elts(handle);

    return h->nelts;
}

void table::clear()
{
    apr_table_clear(handle);
}

const char* table::get(const char* key) const
{
    return apr_table_get(handle, key);
}

void table::set(const char* key, const char* val)
{
    apr_table_set(handle, key, val);
}

void table::setn(const char* key, const char* val)
{
    apr_table_setn(handle, key, val);
}

void table::add(const char* key, const char* val)
{
    apr_table_add(handle, key, val);
}

void table::addn(const char* key, const char* val)
{
    apr_table_addn(handle, key, val);
}

void table::merge(const char* key, const char* val)
{
    apr_table_merge(handle, key, val);
}

void table::mergen(const char* key, const char* val)
{
    apr_table_mergen(handle, key, val);
}

void table::unset(const char* key)
{
    apr_table_unset(handle, key);
}

string table::join(const char* sep)
{
    string x;

    return join(sep, x);
}

string& table::join(const char* sep, string& data)
{
    stringstream strm;

    apr::table::iterator i(*this);

    int len = size();

    while(i.next())
    {
        strm << i.key() << "=" << i.data();

        if(i.index() < len - 1)
        {
            strm << sep;
        }
    }

    data = strm.str();

    return data;
}

//------------------------------------------------------------------------------
// Iterator
//------------------------------------------------------------------------------

table::iterator::iterator(const table& t)
{
    const apr_array_header_t* h = apr_table_elts(t.handle);

    *this = h;
}

table::iterator::~iterator()
{
    
}

void table::iterator::rebind(const table& t)
{
    const apr_array_header_t* h = apr_table_elts(t.handle);

    *this = h;
}

i32 table::iterator::index()
{
    return _i;
}

bool table::iterator::seek(i32 idx)
{
    if(idx < _h->nelts)
    {
        _i = idx;

        return true;
    }
    
    return false;
}

const table::iterator& table::iterator::operator=(const apr_array_header_t* h)
{
    _h = h;
    _i = -1;

    return *this;
}

const table::iterator& table::iterator::operator=(const iterator& i)
{
    if(this != &i)
    {
        _h = i._h;
        _i = i._i;
    }

    return *this;
}

const char* table::iterator::key() const
{
    if(_i != -1)
    {
        return ((const apr_table_entry_t*)_h->elts)[_i].key;
    }

    return NULL;
}

const char* table::iterator::data() const
{
    if(_i != -1)
    {
        return ((const apr_table_entry_t*)_h->elts)[_i].val;
    }

    return NULL;
}

bool table::iterator::rewind() const
{
    _i = -1;

    return true;
}

bool table::iterator::first() const
{
    if(_h->nelts > 0)
    {
        _i = 0;
    }

    return true;
}

bool table::iterator::last() const
{
    if(_h->nelts > 0)
    {
        _i = _h->nelts-1;

        return true;
    }

    return false;
}

bool table::iterator::next() const
{
    // If i is not at the end
    if(_i == _h->nelts-1)
    {
        return false;
    }

    if(_h->nelts > 0)
    {
        _i++;

        return true;
    }
    
    return false;
}

bool table::iterator::prev() const
{
    // If _i is valid
    if(_i != -1)
    {
        // Decrement
        _i--;

        // If before beginning
        if(_i < 0)
        {
            // Not valid
            return false;
        }
        
        return true;
    }

    return false;
}

} // end namespace apr
} // end namespace modruby
