#ifndef MODRUBY_APACHE_PROCESS_DECL
#define MODRUBY_APACHE_PROCESS_DECL

#include <apache2/httpd.h>
#include <apache2/http_request.h>
#include <apache2/http_config.h>
#include <apache2/http_core.h>
#include <apache2/http_protocol.h>

namespace apache
{

class Process
{
    Process();

  protected:
    
    process_rec* _r;

  public:

    Process(process_rec* r);
    ~Process();

    inline apr_pool_t* pool() { return _r->pool; }
    inline apr_pool_t* pconf() { return _r->pconf; }
    inline int argc() { return _r->argc; }
    inline const char* const* argv() { return _r->argv; }
    inline const char* short_name() { return _r->short_name; }

};

} // end namespace apache

#endif
