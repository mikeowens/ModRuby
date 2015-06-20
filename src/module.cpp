#include <unistd.h>
#include <httpd.h>
#include <http_log.h>

#include <map>
#include <string>
#include <sstream>

#include "ruby.hpp"
#include "request.h"
#include "server.h"
#include "ruby_apr.h"
#include "ruby_apr_array.h"
#include "ruby_apr_pool.h"
#include "ruby_apr_file_info.h"
#include "ruby_apr_table.h"
#include "ruby_request.h"
#include "ruby_server.h"
#include "ruby_connection.h"
#include "ruby_process.h"
#include "rhtml.h"
#include "ruby_rhtml.h"
#include "config.h"

#include "module.hpp"

#if AP_SERVER_MINORVERSION_NUMBER >= 4
APLOG_USE_MODULE();
#endif

typedef VALUE (*fn)(...);

using namespace std;

// Generic Ruby object to hold the the handler instance. A handler is a Ruby
// class which works as the persistent Ruby server environment that handles all
// requests. All Apache requests are sent in to it, and it can set up the
// respective VHost environments for all sites, loads the config files, and
// handle the respective requests.

static map<string, ruby::Object*> handlers;
static ruby::Object* ruby_handler;

using namespace modruby;

void log_message(apr_pool_t* pool, int level, const char* message)
{
    stringstream strm;

    strm << "mod_ruby[" << getpid() << "]: "
         << message ;

    ap_log_perror(APLOG_MARK, level, 0, pool, strm.str().c_str());
}

void log_error(apr_pool_t* pool, const char* message)
{
    log_message(pool, APLOG_CRIT, message);
}

void log_warning(apr_pool_t* pool, const char* message)
{
    log_message(pool, APLOG_WARNING, message);
}

VALUE ruby_version(VALUE self)
{
    return rb_str_new2(MODRUBY_RELEASE_VERSION);
}

VALUE ruby_release_date(VALUE self)
{
    return rb_str_new2(MODRUBY_RELEASE_DATE);
}

int ruby_init_module(apr_pool_t* p, server_rec* server)
{
    ap_log_perror( APLOG_MARK, APLOG_NOTICE, 0, p,
                   "mod_ruby[%i]: startup_module", getpid() );

    int x = getpid();

    //> Get the module configuration
    // ruby_config* cfg = ruby_server_config(server->module_config);

    // Initialize the Ruby VM and load C extensions
    try
    {
        // Start up VM
        ruby::startup("ModRuby Ruby VM");

        // Apache log constants for log() in ruby_request.cpp
        rb_define_global_const("APLOG_EMERG",        INT2NUM(APLOG_EMERG));
        rb_define_global_const("APLOG_ALERT",        INT2NUM(APLOG_ALERT));
        rb_define_global_const("APLOG_CRIT",         INT2NUM(APLOG_CRIT));
        rb_define_global_const("APLOG_ERR",          INT2NUM(APLOG_ERR));
        rb_define_global_const("APLOG_WARNING",      INT2NUM(APLOG_WARNING));
        rb_define_global_const("APLOG_NOTICE",       INT2NUM(APLOG_NOTICE));
        rb_define_global_const("APLOG_INFO",         INT2NUM(APLOG_INFO));
        rb_define_global_const("APLOG_DEBUG",        INT2NUM(APLOG_DEBUG));
        rb_define_global_const("APLOG_LEVELMASK",    INT2NUM(APLOG_LEVELMASK));
        rb_define_global_const("APLOG_NOERRNO",      INT2NUM(APLOG_NOERRNO));
        rb_define_global_const("APLOG_TOCLIENT",     INT2NUM(APLOG_TOCLIENT));
        rb_define_global_const("APLOG_STARTUP",      INT2NUM(APLOG_STARTUP));

        // Apache method constants
        rb_define_global_const("M_GET",              INT2NUM(M_GET));
        rb_define_global_const("M_PUT",              INT2NUM(M_PUT));
        rb_define_global_const("M_POST",             INT2NUM(M_POST));
        rb_define_global_const("M_DELETE",           INT2NUM(M_DELETE));
        rb_define_global_const("M_CONNECT",          INT2NUM(M_CONNECT));
        rb_define_global_const("M_OPTIONS",          INT2NUM(M_OPTIONS));
        rb_define_global_const("M_TRACE",            INT2NUM(M_TRACE));
        rb_define_global_const("M_PATCH",            INT2NUM(M_PATCH));
        rb_define_global_const("M_PROPFIND",         INT2NUM(M_PROPFIND));
        rb_define_global_const("M_PROPPATCH",        INT2NUM(M_PROPPATCH));
        rb_define_global_const("M_MKCOL",            INT2NUM(M_MKCOL));
        rb_define_global_const("M_COPY",             INT2NUM(M_COPY));
        rb_define_global_const("M_MOVE",             INT2NUM(M_MOVE));
        rb_define_global_const("M_LOCK",             INT2NUM(M_LOCK));
        rb_define_global_const("M_UNLOCK",           INT2NUM(M_UNLOCK));
        rb_define_global_const("M_VERSION_CONTROL",  INT2NUM(M_VERSION_CONTROL));
        rb_define_global_const("M_CHECKOUT",         INT2NUM(M_CHECKOUT));
        rb_define_global_const("M_UNCHECKOUT",       INT2NUM(M_UNCHECKOUT));
        rb_define_global_const("M_CHECKIN",          INT2NUM(M_CHECKIN));
        rb_define_global_const("M_UPDATE",           INT2NUM(M_UPDATE));
        rb_define_global_const("M_LABEL",            INT2NUM(M_LABEL));
        rb_define_global_const("M_REPORT",           INT2NUM(M_REPORT));
        rb_define_global_const("M_MKWORKSPACE",      INT2NUM(M_MKWORKSPACE));
        rb_define_global_const("M_MKACTIVITY",       INT2NUM(M_MKACTIVITY));
        rb_define_global_const("M_BASELINE_CONTROL", INT2NUM(M_BASELINE_CONTROL));
        rb_define_global_const("M_MERGE",            INT2NUM(M_MERGE));
        rb_define_global_const("M_INVALID",          INT2NUM(M_INVALID));
        rb_define_global_const("M_METHODS",          INT2NUM(64));

        // The following modules are registered under the Apache
        // namespace.

        VALUE apache = rb_define_module("Apache");

        // ModRuby module functions
        VALUE ruby = rb_define_module("ModRuby");

        rb_define_module_function(ruby, "version", (fn)ruby_version, 0);
        rb_define_module_function(ruby, "release_date", (fn)ruby_release_date, 0);

        // RHTML scanner

        init_rhtml();

        // Initialize Ruby extensions/modules

        init_request(apache);     // Apache request Ruby wrapper
        init_server(apache);      // Apache server Ruby wrapper
        init_connection(apache);  // Apache connection Ruby wrapper
        init_process(apache);     // Apache process Ruby wrapper

        VALUE apr = rb_define_module("APR");

        init_apr(apr);           // Apache Portable Runtime (APR)
        init_apr_array(apr);     // APR Arrays
        init_apr_file_info(apr); // APR file functions
        init_apr_pool(apr);      // APR pools
        init_apr_table(apr);     // APR tables

        // Load the Ruby handler module, which is a pure Ruby class
        ruby::require("modruby/handler");

        // Create the global Ruby handler instance.
        ruby_handler = new ruby::Object("ModRuby::Handler");
    }
    catch (const ruby::Exception& e)
    {
        fprintf(stderr, "Ruby Exception: %s", e.what());

        stringstream strm;
        strm << "FATAL ERROR: " << e.what();
        log_error(p, strm.str().c_str());

        return 1;
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "C++ Exception: %s\n", e.what());

        stringstream strm;
        strm << "FATAL ERROR: " << e.what();
        log_error(p, strm.str().c_str());

        return 1;
    }

    return 0;
}

int ruby_shutdown_module()
{
    if (ruby_handler != NULL)
    {
        delete ruby_handler;

        ruby_handler = NULL;
    }

    // Shutdown Ruby environment
    ruby::shutdown();

    return 0;
}

int ruby_log_error(request_rec* r, int level, const char* msg)
{
    // Something in the Ruby failed. Report it. This should not be an
    // application error, as all of those should (in theory) be caught in
    // ruby_handler. This error will have to something within the ruby_handler.

    // Create a C++ request object, for convenience
    apache::Request request(r);

    // Create the error message
    stringstream strm;
    strm << "ModRuby: " << msg;

    // Print error to content
    request.rputs(strm.str().c_str());

    // Log error
    ap_log_error( APLOG_MARK, level, 0, r->server,
                  "mod_ruby[%i] : %s",
                  getpid(), strm.str().c_str() );

    return 0;
}

bool merge_var( request_rec* r, ruby_dir_config* dir_cfg, apr::table& t,
                const char* var_name, const char* server_config)
{
    // Look for existence of value in dir_config
    if (apr_table_get(dir_cfg->options, var_name) != NULL)
    {
        t.set(var_name, apr_table_get(dir_cfg->options, var_name));
    }
    else
    {
        // If not there, set from server_config, if exists.
        if (server_config != NULL)
        {
            t.set(var_name, server_config);
        }
    }

    // Now check to see if it was set in either case. If not, return false
    // signifying that the value was NOT set.
    if (t.get(var_name) == NULL)
    {
        return false;
    }

    return true;
}

// Processes configuration file options.
//
// This happens AFTER directory/location merge.
//
// All variables are merged into the request notes field, which is the final
// version of all information needed to process the request.
//
// Directory config options override server config options.

int ruby_request_init_configuration(request_rec* r)
{
    apache::Request req(r);

    // Pass in module config params
    apr::table notes         = req.notes();
    ruby_config* cfg         = ruby_server_config(r->server->module_config);
    ruby_dir_config* dir_cfg = ruby_directory_config(r);

    // Merge in user-defined options from server and dir configurations
    apr::table server_options(cfg->options);
    apr::table dir_options(dir_cfg->options);

    // Copy in the settings from the server config.
    apr::table::iterator ti(server_options);
    while (ti.next())
    {
        notes.set(ti.key(), ti.data());
    }

    // Now copy in the settings from directory config.
    ti = dir_options.begin();

    while (ti.next())
    {
        notes.set(ti.key(), ti.data());
    }

    // ---------------------------------------------------------------------
    // Check for modules configuration variables
    // ---------------------------------------------------------------------

    // These check for the server_config value and then the dir_config. If the
    // dir_config exists, it will overwrite the server_config.

    // Module
    if (!merge_var(r, dir_cfg, notes, "RubyDefaultHandlerModule", cfg->default_handler_module))
    {
        //ruby_log_error(r, APLOG_CRIT, "RubyDefaultHandlerModule not defined.");
        //return -1;
    }

    // Class
    if (!merge_var(r, dir_cfg, notes, "RubyDefaultHandlerClass", cfg->default_handler_class))
    {
        //ruby_log_error(r, APLOG_CRIT, "RubyDefaultHandlerClass not defined.");
        //return -1;
    }

    // Method
    if (!merge_var(r, dir_cfg, notes, "RubyDefaultHandlerMethod", cfg->default_handler_method))
    {
        //ruby_log_error(r, APLOG_CRIT, "RubyDefaultHandlerMethod not defined.");
        //return -1;
    }

    // RubyHandler is optional.
    if (merge_var(r, dir_cfg, notes, "RubyHandler", cfg->handler))
    {
        // It is defined. So merge all of the handler's settings into the notes
        // table.
        apr_hash_t* h_config = ruby_handler_config(r, notes.get("RubyHandler"));

        if (h_config != NULL)
        {
            apr_hash_index_t* hi;
            char* key;
            apr_ssize_t klen;
            char* data;

            for (hi = apr_hash_first(r->pool, h_config); hi; hi = apr_hash_next(hi))
            {
                apr_hash_this(hi, (const void**)&key, &klen, (void**)&data);
                notes.set(key, data);
            }
        }
    }

    return 0;
}

// Loads a handler
modruby::Handler ruby_request_load_handler( request_rec* r,
        const char* module,
        const char* cls,
        const char* method )
{
    /* Handler resolution. Look for RubyHandler directive in dir_config or
    ** server_config. Handler must be a registered CustomHandler. If not
    ** present, use RubyDefaultHandler.
    **
    ** Handler can be in server or dir config. dir overrides server.
    */

    modruby::Handler handler(module, cls, method);

    ruby::require(handler.module());

    // Look up the handler (Ruby) opject's name by id()
    map<string, ruby::Object*>::iterator i = handlers.find(handler.id());

    // If not created
    if (i == handlers.end())
    {
        // Create and register the Ruby Object

        // We want to catch any exceptions here so we can package up a
        // new/better exception which provides more information.
        try
        {
            handler.object = new ruby::Object(cls);
        }
        catch (const ruby::Exception& e)
        {
            stringstream strm;
            strm << e.what() << "\n"
                 << "Module  : " << module   << "\n"
                 << "Class   : " << cls      << "\n"
                 << "Method  : " << method   << "\n"
                 << e.stackdump();

            ruby::Exception better(strm.str().c_str());
            throw better;
        }

        handlers[handler.id()] = handler.object;
    }
    else
    {
        // Get it from storage
        handler.object = i->second;
    }

    return handler;
}

modruby::Handler::Handler( const char* module, const char* klass,
                           const char* method )

{
    if ((module == NULL) || (klass == NULL) || (method == NULL))
    {
        std::logic_error e("Handler is not defined.");
        throw e;
    }

    _module = module;
    _klass  = klass;
    _method = method;

    object = NULL;
}

// Gets selected handler
modruby::Handler ruby_request_get_handler(request_rec* r)
{
    /* Handler resolution. Look for RubyHandler directive in dir_config or
    ** server_config. Handler must be a registered CustomHandler. If not
    ** present, use RubyDefaultHandler.
    **
    ** Handler can be in server or dir config. dir overrides server.
    */

    apache::Request req(r);

    // This contains all the configuration options
    apr::table notes = req.notes();

    ruby_config* cfg = ruby_server_config(r->server->module_config);

    // Use default server config
    const char* module = cfg->default_handler_module;
    const char* cls    = cfg->default_handler_class;
    const char* method = cfg->default_handler_method;

    // Check for RubyHandler
    if (notes.get("RubyHandler") != NULL)
    {
        apr_hash_t* h_config;
        h_config = ruby_handler_config(r, notes.get("RubyHandler"));

        // If it doesn't exist
        if (h_config != NULL)
        {
            module = (const char*)apr_hash_get( h_config,
                                                "RubyHandlerModule",
                                                APR_HASH_KEY_STRING );

            cls    = (const char*)apr_hash_get( h_config,
                                                "RubyHandlerClass",
                                                APR_HASH_KEY_STRING );

            method = (const char*)apr_hash_get( h_config,
                                                "RubyHandlerMethod",
                                                APR_HASH_KEY_STRING );

            return ruby_request_load_handler(r, module, cls, method);
        }
    }
    else
    {
        if (notes.get("RubyDefaultHandlerModule") != NULL)
        {
            module = notes.get("RubyDefaultHandlerModule");
        }

        if (notes.get("RubyDefaultHandlerClass") != NULL)
        {
            cls    = notes.get("RubyDefaultHandlerClass");
        }

        if (notes.get("RubyDefaultHandlerMethod") != NULL)
        {
            method = notes.get("RubyDefaultHandlerMethod");
        }

        if ((module == NULL) || (cls == NULL) || (method == NULL))
        {
            std::logic_error e("Default Handler is not defined.");
            throw e;
        }

        // Add these for reference, as they are what is being run as the current
        // handler.
        notes.set("RubyHandlerModule", module);
        notes.set("RubyHandlerClass", cls);
        notes.set("RubyHandlerMethod", method);

        return ruby_request_load_handler(r, module, cls, method);
    }

    // Empty handler -- signifies error
    return modruby::Handler();
}

int ruby_request_handler(request_rec* r)
{
    apache::Request req(r);

    req.setup_cgi();

    // Double check the handler name
    if (strcmp(req.handler(), "ruby-handler") != 0)
    {
        return DECLINED;
    }

    //> Process the request

    // Set the default request to OK (200). The Ruby handler is free to change
    // this, but if it doesn't we return OK below.

    int rc = r->status;

    try
    {
        // Load configuration
        if (ruby_request_init_configuration(r) != 0)
        {
            // Configuration failed
            return OK;
        }

        modruby::Handler handler = ruby_request_get_handler(r);

        if (handler.object == NULL)
        {
            // Print error to content
            req.rputs("Failed to load handler");

            // Log error (critical)
            ap_log_error( APLOG_MARK, APLOG_CRIT, 0, r->server,
                          "mod_ruby[%i] : %s",
                          getpid(), "Failed to load handler" );

            return OK;
        }

        // Call the method, passing in the request object
        VALUE result = handler.object->method( handler.method(), 1,
                                               make_request(r) );
    }
    catch (const ruby::Exception& e)
    {
        // Something in the Ruby failed. Report it. This should not be an
        // application error, as all of those should (in theory) be caught in
        // ruby_handler. This error will have to something within the ruby_handler.

        // Create a C++ request object, for convenience
        apache::Request request(r);

        // Check the exception type. If it is a redirect or RequestTermination,
        // then we need to do special handling.
        string exception_type = e.type();

        if (exception_type == "ModRuby::Redirect")
        {
            // This is a utility exception used within the framework. It does
            // not indicate an error.

            return OK;
        }

        if (exception_type == "ModRuby::RequestTermination")
        {
            // This is a utility exception used within the framework. It does
            // not indicate an error.

            return OK;
        }

        // Else it's a bonified exception. Get the message and stacktrace.

        // Create the error message
        stringstream strm;
        strm << "ModRuby FATAL ERROR: Ruby Exception: " << e.what() << "\n"
             << e.stackdump();

        // Print error to content
        request.rputs(strm.str().c_str());

        // Log error (critical)
        ap_log_error( APLOG_MARK, APLOG_CRIT, 0, r->server,
                      "mod_ruby[%i] : %s",
                      getpid(), strm.str().c_str() );

        // The unit test function should pick up on this, even though we return
        // HTTP OK. By convention, the HTTP response status is not what matters
        // here, it's what's in the headers.

        return OK;
    }
    catch (const std::exception& e)
    {
        // Create a C++ request object, for convenience
        apache::Request request(r);

        // Print error to content
        request.rprintf("mod_ruby Error: %s", e.what());

        // Log error (critical)
        ap_log_error( APLOG_MARK, APLOG_CRIT, 0, r->server,
                      "mod_ruby[%i] : %s",
                      getpid(), e.what() );

        return OK;
    }

    // If the status changed, use its value
    if (r->status != rc)
    {
        return r->status;
    }
    else
    {
        // Otherewise everything was OK.
        return OK;
    }
}

/* Common code used for RHTML and Ruby script handlers */
int ruby_generic_handler( request_rec* r,
                          const char* handler_name,
                          const char* method_name )
{
    apache::Request req(r);

    req.setup_cgi();

    // Double check the handler name
    if (strcmp(req.handler(), handler_name) != 0)
    {
        return DECLINED;
    }

    //> Process the request

    // Set the default request to OK (200). The Ruby handler is free to change
    // this, but if it doesn't we return OK below.

    int rc = r->status;

    try
    {
        // Create a Ruby request object
        VALUE ruby_req = make_request(r);

        // Pass into the ruby_handler handler to process the request
        ruby_handler->method(method_name, 1, ruby_req);
    }
    catch (const ruby::Exception& e)
    {
        // Something in the Ruby failed. Report it. This should not be an
        // application error, as all of those should (in theory) be caught in
        // ruby_handler. This error will have to something within the
        // ruby_handler.

        // Create a C++ request object, for convenience
        apache::Request request(r);

        // Check the exception type. If it is a redirect or RequestTermination,
        // then we need to do special handling.
        string exception_type = e.type();

        if (exception_type == "ModRuby::Redirect")
        {
            // This is a utility exception used within the framework. It does
            // not indicate an error.

            // Set the TestResult Header
            request.headers_out().set("TestResult", "PASS");

            return OK;
        }

        if (exception_type == "ModRuby::RequestTermination")
        {
            // This is a utility exception used within the framework. It does
            // not indicate an error.

            // Set the TestResult Header
            request.headers_out().set("TestResult", "PASS");

            return OK;
        }

        // Else it's a bonified exception. Get the message and stacktrace.

        // Create the error message
        stringstream strm;
        strm << "ModRuby FATAL ERROR: Ruby Exception: " << e.what() << "\n"
             << e.stackdump();

        // Print error to content
        request.rputs(strm.str().c_str());

        // Log error (critical)
        ap_log_error( APLOG_MARK, APLOG_CRIT, 0, r->server,
                      "mod_ruby[%i] : %s", getpid(), strm.str().c_str() );

        // The unit test function should pick up on this, even though we return
        // HTTP OK. By convention, the HTTP response status is not what matters
        // here, it's what's in the headers.

        return OK;
    }
    catch (const std::exception& e)
    {
        // Create a C++ request object, for convenience
        apache::Request request(r);

        // Print error to content
        request.rprintf("mod_ruby Error: %s", e.what());

        // Log error (critical)
        ap_log_error( APLOG_MARK, APLOG_CRIT, 0, r->server,
                      "mod_ruby[%i] : %s", getpid(), e.what() );

        return OK;
    }

    // If the status changed, use its value
    if (r->status != rc)
    {
        return r->status;
    }
    else
    {
        // Otherewise everything was OK.
        return OK;
    }
}

int ruby_request_rhtml_handler(request_rec* r)
{
    return ruby_generic_handler(r, "ruby-rhtml-handler", "rhtml");
}

int ruby_request_script_handler(request_rec* r)
{
    return ruby_generic_handler(r, "ruby-script-handler", "script");
}
