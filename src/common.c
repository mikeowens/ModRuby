#include <httpd.h>
#include <http_config.h>
#include <http_protocol.h>
#include <ap_config.h>

#include "module.h"

ruby_config* ruby_server_config(ap_conf_vector_t* module_config)
{
    return ap_get_module_config(module_config, &ruby_module);
}

ruby_dir_config* ruby_directory_config(request_rec* r)
{
    return ap_get_module_config(r->per_dir_config, &ruby_module);
}

apr_hash_t* ruby_handler_config(request_rec* r, const char* name)
{
    ruby_config* cfg = ruby_server_config(r->server->module_config);

    apr_hash_t* h_config;
    h_config = (apr_hash_t*)apr_hash_get( cfg->handlers,
                                          name,
                                          APR_HASH_KEY_STRING );

    return h_config;
}
