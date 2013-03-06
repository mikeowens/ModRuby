#ifndef MODRUBY_APR_TABLE_DECLARE
#define MODRUBY_APR_TABLE_DECLARE

#include "apr.h"
#include "common.h"

namespace modruby
{

namespace apr
{

/** This class is a thin C++ wrapper around the APR table structure and
 * associated methods. It simply provides a C++ interface for the main Apache
 * C++ classes to use for convenience.
 */
class table
{
  protected:
    
    // All memory management methods are not allowed. This class does not do
    // memory management of tables or pools. It is a simple wrapper to make
    // operating on tables easier.

  public:

    apr_table_t* handle;

    class iterator
    {
        iterator();

        void clear();

    public:

        mutable i32 _i;
        const apr_array_header_t* _h;

        iterator(const table& t);
        iterator(const iterator& i);
        ~iterator();

        const iterator& operator=(const iterator& i);
        const iterator& operator=(const apr_array_header_t* h);

        i32  index();
        bool seek(i32 idx);
        bool valid();

        bool rewind() const;
        bool first() const;
        bool last() const;
        bool next() const;
        bool prev() const;
        
        const char* key() const;
        const char* data() const;
        
        int remove(modruby::i32 del);

        void rebind(const table& t);
    };

    table(apr_table_t* t);
    table(apr_pool_t* p);
    ~table();

    const table& operator=(apr_table_t* t);
    const table& operator=(const table& t);
   
    const apr_array_header_t* begin();

    QString join(const char* sep);
    QString& join(const char* sep, QString& b);

    u32 size() const;
    void clear();
    iterator operator[](i32 idx) const;
    const char* operator[](const char* key) const;
    const char* get(const char* key) const;
    void set(const char* key, const char* val);
    void setn(const char* key, const char* val);
    void add(const char* key, const char* val);
    void addn(const char* key, const char* val);
    void merge(const char* key, const char* val);
    void mergen(const char* key, const char* val);
    void unset(const char* key);

};

} // end namespace apr
} // end namespace modruby

#endif
