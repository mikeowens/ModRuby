#ifndef MODRUBY_APACHE_SERVER_DECL
#define MODRUBY_APACHE_SERVER_DECL

#include <httpd.h>
#include <http_config.h>

#include "process.h"

namespace apache
{

class Server
{
    Server();

  private:

    const server_rec* _sr;
    Server* _next;

  public:

    Server(const server_rec* s);
    ~Server();

    void operator=(const server_rec* _sr);

    process_rec* process() { return _sr->process; }

    Server* next() { return _next; }
    const char* defn_name() { return _sr->defn_name; }
    unsigned defn_line_number() { return _sr->defn_line_number; }

    char* server_admin() { return _sr->server_admin; }
    char* server_hostname() { return _sr->server_hostname; }
    apr_port_t port() { return _sr->port; }

    char* error_fname() { return _sr->error_fname; }
    apr_file_t* error_log() { return _sr->error_log; }

    int loglevel() {
#if AP_SERVER_MINORVERSION_NUMBER >= 4
        return _sr->log.level;
#else
        return _sr->loglevel;
#endif
    }

    int is_virtual() { return _sr->is_virtual; }

    ap_conf_vector_t* lookup_defaults() { return _sr->lookup_defaults; }
    server_addr_rec* addrs() { return _sr->addrs; }

    apr_interval_time_t timeout() { return _sr->timeout; }
    apr_interval_time_t keep_alive_timeout() { return _sr->keep_alive_timeout; }
    int keep_alive_max() { return _sr->keep_alive_max; }
    int keep_alive() { return _sr->keep_alive; }

    const char* path() { return _sr->path; }
    int pathlen() { return _sr->pathlen; }

    apr_array_header_t* names() { return _sr->names; }
    apr_array_header_t* wild_names() { return _sr->wild_names; }

    int limit_req_line() { return _sr->limit_req_line; }
    int limit_req_fieldsize() { return _sr->limit_req_fieldsize; }
    int limit_req_fields() { return _sr->limit_req_fields; }

    const server_rec* get_server_rec() { return _sr; }

    void* get_module_config(module* m);
};

} // end namespace apache

#endif
