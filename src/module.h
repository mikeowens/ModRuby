#ifndef MODRUBY_MODULE_DECL
#define MODRUBY_MODULE_DECL

#include <http_protocol.h>
#include <http_config.h>

#include "common.h"

#include <apr_hash.h>

BEGIN_DECL

/** 
This structure holds the configuration parameters in the Apache module.
*/
typedef struct ruby_config
{
    char* handler;                 ///< Current handler selected for given request
    char* default_handler_module;  ///< Default handler Ruby module
    char* default_handler_class;   ///< Default handler Ruby class
    char* default_handler_method;  ///< Default handler Ruby method (handler)

    apr_table_t* options;          ///< Apache handler user-defined options to pass in
    apr_hash_t* handlers;          ///< User-defined Apache handlers 

} ruby_config;

/** 
This structure holds directory configuration parameters in the RUBY Apache module.
*/
typedef struct ruby_dir_config
{
    char* dir;
    apr_table_t* options;
} ruby_dir_config;

extern module AP_MODULE_DECLARE_DATA ruby_module;

ruby_config* ruby_server_config(ap_conf_vector_t* module_config);
ruby_dir_config* ruby_directory_config(request_rec* r);
apr_hash_t* ruby_handler_config(request_rec* r, const char* name);

/* Main module functions */
int ruby_init_module(apr_pool_t* p, server_rec* s);
int ruby_shutdown_module();
int ruby_request_handler(request_rec* req);
int ruby_request_rhtml_handler(request_rec* req);
int ruby_request_script_handler(request_rec* req);
int ruby_request_init_configuration(request_rec* r);
int ruby_request_access_handler(request_rec* req);

/* Error logging */
int ruby_log_error(request_rec* r, int level, const char* msg);

/*  Unit testing */
int ruby_test_handler(request_rec *r);

END_DECL

#endif
