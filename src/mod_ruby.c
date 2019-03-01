#include <httpd.h>
#include <http_config.h>
#include <http_protocol.h>
#include <http_log.h>
#include <http_request.h>
#include <ap_config.h>

#include "common.h"
#include "module.h"

module AP_MODULE_DECLARE_DATA ruby_module;

/* Module Interface */

/* Child shutdown handler */
static apr_status_t ruby_child_shutdown(void* data)
{
    ruby_shutdown_module();

    return APR_SUCCESS;
}

/* Child init hook. This will be called exactly once. */
static void ruby_child_init_hook(apr_pool_t* child_pool, server_rec* s)
{
    ruby_init_module(child_pool, s);

    /* Have the ruby_child_shutdown called when child_pool is destroyed. Thus,
    ** when the process shuts down, we shut down Ruby as well. 
    */
    apr_pool_cleanup_register( child_pool, NULL, ruby_child_shutdown, 
                               apr_pool_cleanup_null );
}

/* Ruby handler */
static int ruby_handler(request_rec *r)
{
    return ruby_request_handler(r);
}

/* RHTML handler */
static int ruby_rhtml_handler(request_rec *r)
{
    return ruby_request_rhtml_handler(r);
}

/* Script handler */
static int ruby_script_handler(request_rec *r)
{
    return ruby_request_script_handler(r);
}

/* Ruby access handler */
static int ruby_access_handler(request_rec *r)
{
    return ruby_request_access_handler(r);
}

static apr_status_t input_filter( ap_filter_t *f,
                                  apr_bucket_brigade* bb,
                                  ap_input_mode_t mode,
                                  apr_read_type_e block,
                                  apr_off_t read_bytes )
{
    return ap_get_brigade(f->next, bb, mode, block, read_bytes);
}

static void register_hooks(apr_pool_t *p)
{
    /* We use this to initialize Ruby. Ruby VM is process-scope, so we have to
    ** be careful about how we initialize it from this module. Apache modules
    ** are loaded in multiple phases, and if we initialize Ruby more than once,
    ** we will get warning about constants already being defined, which just
    ** clutters up the log files. By using child_init, we can delay Ruby
    ** initialization until after the main process has forked, thereby ensuring
    ** only one initialization takes place.
    */
    ap_hook_child_init(ruby_child_init_hook, NULL, NULL, APR_HOOK_MIDDLE);

    /* The generic handler */
    ap_hook_handler(ruby_handler, NULL, NULL, APR_HOOK_MIDDLE);

    /* The RHTML handler */
    ap_hook_handler(ruby_rhtml_handler, NULL, NULL, APR_HOOK_MIDDLE);

    /* The ruby script handler */
    ap_hook_handler(ruby_script_handler, NULL, NULL, APR_HOOK_MIDDLE);
    
    /* check access handler */
    ap_hook_check_access_ex(ruby_access_handler, NULL, NULL, APR_HOOK_FIRST, AP_AUTH_INTERNAL_PER_URI);

    ap_register_input_filter( "ruby_input_filter", input_filter, NULL,
                               AP_FTYPE_RESOURCE);
}

static const char* 
set_ruby_default_handler_module( cmd_parms *parms, 
                                void* config, 
                                const char* arg )
{
    if(arg == NULL)
    {
        return NULL;
    }
    
    // If this is in a server config (outside directory)
    if(ap_check_cmd_context(parms, NOT_IN_DIR_LOC_FILE) == NULL)
    {
        ruby_config* cfg = ap_get_module_config( parms->server->module_config,
                                                &ruby_module );
        
        cfg->default_handler_module = (char*)arg;
        
        return NULL;
    }

    ruby_dir_config* dir_config = (ruby_dir_config*)config;
    apr_table_set(dir_config->options, "RubyHandlerModule", (char*)arg);
    
    /* Success */
    return NULL;
}

static const char* 
set_ruby_default_handler_class( cmd_parms *parms, 
                               void* config, 
                               const char* arg )
{
    if(arg == NULL)
    {
        return NULL;
    }
    
    // If this is in a server config (outside directory)
    if(ap_check_cmd_context(parms, NOT_IN_DIR_LOC_FILE) == NULL)
    {
        ruby_config* cfg = ap_get_module_config( parms->server->module_config,
                                                &ruby_module );
        
        cfg->default_handler_class = (char*)arg;
        
        return NULL;
    }

    ruby_dir_config* dir_config = (ruby_dir_config*)config;
    apr_table_set(dir_config->options, "RubyHandlerClass", (char*)arg);
    
    /* Success */
    return NULL;
}

static const char* 
set_ruby_default_handler_method( cmd_parms *parms, 
                                void* config, 
                                const char* arg )
{
    if(arg == NULL)
    {
        return NULL;
    }
    
    // If this is in a server config (outside directory)
    if(ap_check_cmd_context(parms, NOT_IN_DIR_LOC_FILE) == NULL)
    {
        ruby_config* cfg = ap_get_module_config( parms->server->module_config,
                                                &ruby_module );
        
        cfg->default_handler_method = (char*)arg;
        
        return NULL;
    }
    
    ruby_dir_config* dir_config = (ruby_dir_config*)config;
    apr_table_set(dir_config->options, "RubyHandlerMethod", (char*)arg);
    
    /* Success */
    return NULL;
}

static const char* 
set_ruby_config_var( cmd_parms *parms, 
                    void* config, 
                    const char* arg1,
                    const char* arg2 )
{
    if(arg1 == NULL)
    {
        return NULL;
    }
    
    // If this is in a server config (outside directory)
    if(ap_check_cmd_context(parms, NOT_IN_DIR_LOC_FILE) == NULL)
    {
        ruby_config* cfg = ap_get_module_config( parms->server->module_config,
                                                &ruby_module );

        apr_table_set(cfg->options, (char*)arg1, (char*)arg2);
        return NULL;
    }
    
    ruby_dir_config* dir_config = (ruby_dir_config*)config;

    apr_table_set(dir_config->options, (char*)arg1, (char*)arg2);
    
    /* Success */
    return NULL;
}

static const char* 
set_ruby_env_var( cmd_parms* parms, 
                 void* config, 
                 const char* arg1,
                 const char* arg2 )
{
    if(arg1 == NULL)
    {
        return NULL;
    }

    if(arg2 == NULL)
    {
        return NULL;
    }
    
    setenv(arg1, arg2, 1);
        
    /* Success */
    return NULL;
}

/* ---------------------------------------------------------------------------- */
/* Custom Handlers */
/* ---------------------------------------------------------------------------- */

/* Remember that the void* config parameter in the set_xxx commands always
** points to a dir_config. If you need the server_config, you have to use
** ap_get_module_config. 
**
** Furthermore, NEVER use apr_pstrdup() when assigning values from *arg to a
** server_config struct. As a rule, avoid apr_pstrdup() altogther in
** configuration phase.
*/

static const char* 
set_ruby_handler(cmd_parms *parms, void* config, const char* arg)
{
    if(arg == NULL)
    {
        return NULL;
    }
    
    // If this is in a server config (outside directory). This can only be in
    // either global environment or <VirtualHost>.
    if(ap_check_cmd_context(parms, NOT_IN_DIR_LOC_FILE) == NULL)
    {
        ruby_config* cfg = ap_get_module_config( parms->server->module_config,
                                                &ruby_module );
        
        /* DO NOT use apr_pstrdup(parms->pool, arg) here to make a copy of the
        ** string before assignbment. It will cause a segfault.
        */
        cfg->handler = arg;
        
        return NULL;
    }
    
    ruby_dir_config* dir_config = (ruby_dir_config*)config;
    apr_table_set(dir_config->options, "RubyHandler", (char*)arg);

    /* Success */
    return NULL;
}

static const char* 
set_ruby_access_handler(cmd_parms *parms, void* config, const char* arg)
{
    if(arg == NULL)
    {
        return NULL;
    }
    
    // If this is in a server config (outside directory). This can only be in
    // either global environment or <VirtualHost>.
    if(ap_check_cmd_context(parms, NOT_IN_DIR_LOC_FILE) == NULL)
    {
        ruby_config* cfg = ap_get_module_config( parms->server->module_config,
                                                &ruby_module );
        
        /* DO NOT use apr_pstrdup(parms->pool, arg) here to make a copy of the
        ** string before assignbment. It will cause a segfault.
        */
        cfg->handler = arg;
        
        return NULL;
    }
    
    ruby_dir_config* dir_config = (ruby_dir_config*)config;
    apr_table_set(dir_config->options, "RubyAccessHandler", (char*)arg);

    /* Success */
    return NULL;
}

static const char* 
set_ruby_handler_declare(cmd_parms *parms, void* config, const char* arg)
{
    if(arg == NULL)
    {
        return NULL;
    }
    
    const char* errmsg = ap_check_cmd_context(parms, NOT_IN_DIR_LOC_FILE);
  
    // If this is outside of a directory
    if(errmsg == NULL)
    {
        ruby_config* cfg = ap_get_module_config( parms->server->module_config,
                                                &ruby_module );

        // Check to see if this handler has been registered
        apr_hash_t* handler = apr_hash_get( cfg->handlers, (char*)arg, 
                                            APR_HASH_KEY_STRING);

        // If it doesn't exist
        if(handler == NULL)
        {            
            // Create it
            apr_hash_t* h_config = apr_hash_make(parms->pool);
            apr_hash_set( cfg->handlers,
                          (char*)arg, APR_HASH_KEY_STRING, (void*)h_config);
        }
    }
        
    return errmsg;
}


static const char* 
ruby_handler_set_params( cmd_parms *parms, const char* key,
                        const char* handler, const char* value )
{
    if((handler == NULL) || (value == NULL)) 
    {
        return apr_psprintf( parms->temp_pool, 
                             "Handler %s %s arguments not provided",
                             handler, value );
    }

    const char* errmsg = ap_check_cmd_context(parms, NOT_IN_DIR_LOC_FILE);

    // If this is outside of a directory
    if(errmsg != NULL)
    {
        return errmsg;
    }

    ruby_config* cfg = ap_get_module_config( parms->server->module_config,
                                            &ruby_module );
        
    // Check to see if this handler has been registered
    apr_hash_t* h_config = apr_hash_get( cfg->handlers, (char*)handler, 
                                         APR_HASH_KEY_STRING);

    // If it doesn't exist
    if(h_config == NULL)
    {
        return apr_psprintf( parms->temp_pool, 
                             "Handler %s is not registered",
                             handler);
    }
   
    // Set parameters
    apr_hash_set( h_config, (char*)key, APR_HASH_KEY_STRING, value);

    return NULL;
}

static const char* 
set_ruby_handler_config( cmd_parms *parms, void* config, 
                        const char* handler, 
                        const char* key, const char* value )
{
    return ruby_handler_set_params( parms, key, handler, value );
}

static const char* 
set_ruby_handler_module( cmd_parms *parms, void* config, 
                        const char* arg1, const char* arg2 )
{
    return ruby_handler_set_params( parms, "RubyHandlerModule",
                                   arg1, arg2 );
}

static const char* 
set_ruby_handler_class( cmd_parms *parms, void* config, 
                              const char* arg1, const char* arg2 )
{
    return ruby_handler_set_params( parms, "RubyHandlerClass",
                                   arg1, arg2 );
}

static const char* 
set_ruby_handler_method( cmd_parms *parms, void* config,
                               const char* arg1, const char* arg2 )
{
    return ruby_handler_set_params( parms, "RubyHandlerMethod",
                                   arg1, arg2 );
}

static const command_rec mod_ruby_cmds[] =
{
    AP_INIT_TAKE1(
        "RubyDefaultHandlerModule",
        set_ruby_default_handler_module,
        NULL,
        RSRC_CONF | OR_ALL,
        "RubyHandlerModule <string> "
        "-- set default Ruby module for Apache handler."
    ),

    AP_INIT_TAKE1(
        "RubyDefaultHandlerClass",
        set_ruby_default_handler_class,
        NULL,
        RSRC_CONF | OR_ALL,
        "RubyHandlerClass <string> "
        "-- set default Ruby class for Apache handler."
    ),

    AP_INIT_TAKE1(
        "RubyDefaultHandlerMethod",
        set_ruby_default_handler_method,
        NULL,
        RSRC_CONF | OR_ALL,
        "RubyHandlerMethod <string> "
        "-- set default Ruby method for Apache handler."
    ),

    AP_INIT_TAKE1(
        "RubyHandler",
        set_ruby_handler,
        NULL,
        RSRC_CONF | OR_ALL,
        "RubyHandler {name} "
        "-- set a Ruby handler."
    ),

    AP_INIT_TAKE1(
        "RubyAccessHandler",
        set_ruby_access_handler,
        NULL,
        RSRC_CONF | OR_ALL,
        "RubyAccessHandler {name} "
        "-- set a Ruby check_access handler."
    ),
    
    AP_INIT_TAKE2(
        "RubyConfig",
        set_ruby_config_var,
        NULL,
        RSRC_CONF | OR_ALL,
        "RubyConfig {key} {value} "
        "-- set server/per-directory variable for Apache handler."
    ),

    AP_INIT_TAKE2(
        "RubyEnv",
        set_ruby_env_var,
        NULL,
        RSRC_CONF | OR_ALL,
        "RubyEnv {key} {value} "
        "-- set ENV variable for Apache handler."
    ),

    AP_INIT_TAKE1(
        "RubyHandlerDeclare",
        set_ruby_handler_declare,
        NULL,
        RSRC_CONF,
        "RubyHandlerDeclare {name} "
        "-- declare a custom Ruby handler."
    ),

    AP_INIT_TAKE2(
        "RubyHandlerModule",
        set_ruby_handler_module,
        NULL,
        RSRC_CONF,
        "RubyHandlerModule {handler} {module} "
        "-- set Ruby module for custom handler."
    ),

    AP_INIT_TAKE2(
        "RubyHandlerClass",
        set_ruby_handler_class,
        NULL,
        RSRC_CONF,
        "RubyHandlerClass {handler} {class} "
        "-- set Ruby class for custom handler."
    ),

    AP_INIT_TAKE2(
        "RubyHandlerMethod",
        set_ruby_handler_method,
        NULL,
        RSRC_CONF,
        "RubyHandlerMethod {handler} {method} "
        "-- set Ruby method for custom handler."
    ),

    AP_INIT_TAKE3(
        "RubyHandlerConfig",
        set_ruby_handler_config,
        NULL,
        RSRC_CONF,
        "RubyHandlerConfig {handler} {key} {value}"
        " -- define key/value config setting for handler."
    ),

    {NULL}
};

static void* create_config(apr_pool_t* p, server_rec *s)
{
    ruby_config* cfg;

    /* allocate space for the configuration structure from the provided pool p. */
    cfg = (ruby_config*)apr_pcalloc(p, sizeof(ruby_config));

    cfg->handler                = NULL;
    cfg->default_handler_module = NULL;
    cfg->default_handler_class  = NULL; 
    cfg->default_handler_method = NULL;

    cfg->options    = apr_table_make(p, 3);
    cfg->handlers   = apr_hash_make(p);

    /* return the new server configuration structure. */
    return (void*)cfg;
}

static void* create_dir_conf(apr_pool_t* p, char* dir)
{
    ruby_dir_config* cfg = apr_pcalloc(p, sizeof(ruby_dir_config));

    cfg->options = apr_table_make(p, 3);
    cfg->dir     = dir;

    /* Debugging
    ap_log_perror( APLOG_MARK, APLOG_WARNING, 
                   0, p, "create_dir_conf %x->%s", 
                   cfg, dir);
    */

    return cfg;
}

static void* merge_dir_conf(apr_pool_t* pool, void* current_config, void* new_config)
{
    ruby_dir_config* dir_current = (ruby_dir_config*)current_config;
    ruby_dir_config* dir_new     = (ruby_dir_config*)new_config;
    ruby_dir_config* dir_merged  = apr_palloc(pool, sizeof(ruby_dir_config));

    dir_merged->options = apr_table_overlay( pool, 
                                             dir_current->options, 
                                             dir_new->options );

    apr_table_compress(dir_merged->options, APR_OVERLAP_TABLES_SET);

    /* Debugging
    const char* handler = apr_table_get(dir_merged->options, "RubyHandler");

    ap_log_perror(APLOG_MARK, APLOG_WARNING, 
                  0, pool, "Dir %x %s->%s Handler: %s ", 
                  dir_merged, dir_current->dir, dir_new->dir, handler);
    */

    /* This will cause a segfault (by using strdup)
       dir_merged->dir = apr_pstrdup(pool, dir_new->dir);

       > gdb --args /usr/sbin/apache2 -f 
       > /etc/apache2/httpd.conf -DONE_PROCESS -DNO_DETACH 
    */

    // This will not
    dir_merged->dir = dir_new->dir;

    return dir_merged;
}

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA ruby_module = {
    STANDARD20_MODULE_STUFF, 
    create_dir_conf,   /* create per-dir config    */
    merge_dir_conf,    /* merge per-dir config     */
    create_config,     /* create per-server config */
    NULL,              /* merge per-server config  */
    mod_ruby_cmds,     /* config file cmds         */
    register_hooks     /* register hooks           */
};

module AP_MODULE_DECLARE_DATA _module = {
    STANDARD20_MODULE_STUFF, 
    create_dir_conf,   /* create per-dir config    */
    merge_dir_conf,    /* merge per-dir config     */
    create_config,     /* create per-server config */
    NULL,              /* merge per-server config  */
    mod_ruby_cmds,     /* config file cmds         */
    register_hooks     /* register hooks           */
};
