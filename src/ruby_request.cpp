#include <apache2/httpd.h>
#include <apache2/http_log.h>

#include "util.hpp"
#include "library.h"
#include "request.h"
#include "server.h"
#include "ruby_apr_table.h"
#include "ruby_apr_file_info.h"
#include "ruby_apr_array.h"
#include "ruby_request.h"
#include "ruby_server.h"
#include "ruby_connection.h"

#include <ruby/encoding.h>

typedef VALUE (*fn)(...);

#define CLASS_NAME "Request"

using std::string;
using modruby::to_upper;

extern "C" {

static VALUE m_allow_options(VALUE self);
static VALUE m_allow_overrides(VALUE self);
static VALUE m_allowed(VALUE self);
static VALUE m_args(VALUE self);
static VALUE m_assbackwards(VALUE self);
static VALUE m_auth_type(VALUE self);
static VALUE m_bytes_sent(VALUE self);
static VALUE m_chunked(VALUE self);
static VALUE m_clength(VALUE self);
static VALUE m_connection(VALUE self);
static VALUE m_content(VALUE self);
static VALUE m_content_encoding(VALUE self);
static VALUE m_content_languages(VALUE self);
static VALUE m_content_type(VALUE self);
static VALUE m_default_type(VALUE self);
static VALUE m_discard_request_body(VALUE self);
static VALUE m_document_root(VALUE self);
static VALUE m_err_headers_out(VALUE self);
static VALUE m_filename(VALUE self);
static VALUE m_finalize_request_protocol(VALUE self);
static VALUE m_finfo(VALUE self);
static VALUE m_get_error(VALUE self);
static VALUE m_get_remote_logname(VALUE self);
static VALUE m_get_server_name(VALUE self);
static VALUE m_handler(VALUE self);
static VALUE m_header_only(VALUE self);
static VALUE m_hostname(VALUE self);
static VALUE m_init(VALUE self);
static VALUE m_internal_redirect(VALUE self, VALUE url);
static VALUE m_internal_redirect_handler(VALUE self, VALUE url);
static VALUE m_is_initial_req(VALUE self);
static VALUE m_log(VALUE self, VALUE level, VALUE msg);
static VALUE m_main(VALUE self);
static VALUE m_make_content_type(VALUE self);
static VALUE m_make_etag(VALUE self, VALUE force_weak);
static VALUE m_meets_conditions(VALUE self);
static VALUE m_method(VALUE self);
static VALUE m_method_number(VALUE self);
static VALUE m_mtime(VALUE self);
static VALUE m_next(VALUE self);
static VALUE m_no_cache(VALUE self);
static VALUE m_no_local_copy(VALUE self);
static VALUE m_note_auth_failure(VALUE self);
static VALUE m_note_basic_auth_failure(VALUE self);
static VALUE m_note_digest_auth_failure(VALUE self);
static VALUE m_notes(VALUE self);
static VALUE m_parsed_uri(VALUE self);
static VALUE m_params(VALUE self);
static VALUE m_path_info(VALUE self);
static VALUE m_prev(VALUE self);
static VALUE m_print(VALUE self, VALUE str);
static VALUE m_puts(VALUE self, VALUE str);
static VALUE m_proto_num(VALUE self);
static VALUE m_protocol(VALUE self);
static VALUE m_proxyreq(VALUE self);
static VALUE m_queries(VALUE self);
static VALUE m_range(VALUE self);
static VALUE m_rationalize_mtime(VALUE self, VALUE mtime);
static VALUE m_read(VALUE self, VALUE bytes);
static VALUE m_read_body(VALUE self);
static VALUE m_read_chunked(VALUE self);
static VALUE m_read_length(VALUE self);
static VALUE m_read_line(VALUE self);
static VALUE m_remaining(VALUE self);
static VALUE m_request_time(VALUE self);
static VALUE m_rflush(VALUE self);
static VALUE m_send_error_response(VALUE self, VALUE recursive_error);
static VALUE m_send_file(VALUE self, VALUE path);
static VALUE m_sent_bodyct(VALUE self);
static VALUE m_server(VALUE self);
static VALUE m_set_allowed(VALUE self, VALUE bitmask);
static VALUE m_set_content_length(VALUE self, VALUE length);
static VALUE m_set_content_type(VALUE self, VALUE type);
static VALUE m_set_etag(VALUE self);
static VALUE m_set_keepalive(VALUE self);
static VALUE m_set_max_content_length(VALUE self, VALUE bytes);
static VALUE m_set_status(VALUE self, VALUE status);
static VALUE m_setup_client_block(VALUE self, VALUE policy);
static VALUE m_should_client_block(VALUE self);
static VALUE m_some_auth_required(VALUE self);
static VALUE m_status(VALUE self);
static VALUE m_status_line(VALUE self);
static VALUE m_subprocess_env(VALUE self);
static VALUE m_table_headers_in(VALUE self);
static VALUE m_table_headers_out(VALUE self);
static VALUE m_the_request(VALUE self);
static VALUE m_unparsed_uri(VALUE self);
static VALUE m_uri(VALUE self);
static VALUE m_user(VALUE self);
static VALUE m_vlist_validator(VALUE self);
static VALUE m_write(VALUE self, VALUE data, VALUE bytes);

}

static apache::Request* get_object(VALUE self);

static void deallocator(void* x)
{
    if(x != NULL)
    {
        delete (apache::Request*)x;
        x = NULL;
    }
}

// This is not the way to create a request. If you do this -- kaboom.
static VALUE allocator(VALUE cls)
{
    apache::Request* x = NULL;

    rb_raise( rb_eRuntimeError, 
              "Cannot create a Apache::Request this way" );

    return Data_Wrap_Struct(cls, NULL, deallocator, x);
}

static VALUE cls;

void init_request(VALUE module)
{
    cls = rb_define_class_under(module, CLASS_NAME, rb_cObject);
    rb_define_alloc_func(cls, allocator);

    rb_define_method(cls, "initialize", (fn)m_init, 0);
    rb_define_method(cls, "server", (fn)m_server, 0);
    rb_define_method(cls, "allow_options", (fn)m_allow_options, 0);
    rb_define_method(cls, "allow_overrides", (fn)m_allow_overrides, 0);
    rb_define_method(cls, "allowed", (fn)m_allowed, 0);
    rb_define_method(cls, "allowed=", (fn)m_set_allowed, 1);
    rb_define_method(cls, "args", (fn)m_args, 0);
    rb_define_method(cls, "assbackwards", (fn)m_assbackwards, 0);
    rb_define_method(cls, "auth_type", (fn)m_auth_type, 0);
    rb_define_method(cls, "bytes_sent", (fn)m_bytes_sent, 0);
    rb_define_method(cls, "cgi", (fn)m_subprocess_env, 0);
    rb_define_method(cls, "chunked", (fn)m_chunked, 0);
    rb_define_method(cls, "clength", (fn)m_clength, 0);
    rb_define_method(cls, "connection", (fn)m_connection, 0);
    rb_define_method(cls, "content", (fn)m_content, 0);
    rb_define_method(cls, "content_encoding", (fn)m_content_encoding, 0);
    rb_define_method(cls, "content_languages", (fn)m_content_languages, 0);
    rb_define_method(cls, "content_type", (fn)m_content_type, 0);
    rb_define_method(cls, "default_type", (fn)m_default_type, 0);
    rb_define_method(cls, "discard_request_body", (fn)m_discard_request_body, 0);
    rb_define_method(cls, "document_root", (fn)m_document_root, 0);
    rb_define_method(cls, "err_headers_out", (fn)m_err_headers_out, 0);
    rb_define_method(cls, "filename", (fn)m_filename, 0);
    rb_define_method(cls, "finalize_request_protocol", (fn)m_finalize_request_protocol, 0);
    rb_define_method(cls, "finfo", (fn)m_finfo, 0);
    rb_define_method(cls, "error", (fn)m_get_error, 0);
    rb_define_method(cls, "get_remote_logname", (fn)m_get_remote_logname, 0);
    rb_define_method(cls, "get_server_name", (fn)m_get_server_name, 0);
    rb_define_method(cls, "handler", (fn)m_handler, 0);
    rb_define_method(cls, "header_only", (fn)m_header_only, 0);
    rb_define_method(cls, "hostname", (fn)m_hostname, 0);
    rb_define_method(cls, "init", (fn)m_init, 0);
    rb_define_method(cls, "internal_redirect", (fn)m_internal_redirect, 1);
    rb_define_method(cls, "internal_redirect_handler", (fn)m_internal_redirect_handler, 1);
    rb_define_method(cls, "is_initial_req", (fn)m_is_initial_req, 0);
    rb_define_method(cls, "log", (fn)m_log, 2);
    rb_define_method(cls, "main", (fn)m_main, 0);
    rb_define_method(cls, "make_content_type", (fn)m_make_content_type, 0);
    rb_define_method(cls, "make_etag", (fn)m_make_etag, 1);
    rb_define_method(cls, "meets_conditions", (fn)m_meets_conditions, 0);
    rb_define_method(cls, "method", (fn)m_method, 0);
    rb_define_method(cls, "method_number", (fn)m_method_number, 0);
    rb_define_method(cls, "mtime", (fn)m_mtime, 0);
    rb_define_method(cls, "next", (fn)m_next, 0);
    rb_define_method(cls, "no_cache", (fn)m_no_cache, 0);
    rb_define_method(cls, "no_local_copy", (fn)m_no_local_copy, 0);
    rb_define_method(cls, "note_auth_failure", (fn)m_note_auth_failure, 0);
    rb_define_method(cls, "note_basic_auth_failure", (fn)m_note_basic_auth_failure, 0);
    rb_define_method(cls, "note_digest_auth_failure", (fn)m_note_digest_auth_failure, 0);
    rb_define_method(cls, "notes", (fn)m_notes, 0);
    rb_define_method(cls, "out", (fn)m_puts, 1);
    rb_define_method(cls, "params", (fn)m_params, 0);
    rb_define_method(cls, "parsed_uri", (fn)m_parsed_uri, 0);
    rb_define_method(cls, "path_info", (fn)m_path_info, 0);
    rb_define_method(cls, "prev", (fn)m_prev, 0);
    rb_define_method(cls, "print", (fn)m_print, 1);
    rb_define_method(cls, "puts", (fn)m_puts, 1);
    rb_define_method(cls, "proto_num", (fn)m_proto_num, 0);
    rb_define_method(cls, "protocol", (fn)m_protocol, 0);
    rb_define_method(cls, "proxyreq", (fn)m_proxyreq, 0);
    rb_define_method(cls, "queries", (fn)m_queries, 0);
    rb_define_method(cls, "range", (fn)m_range, 0);
    rb_define_method(cls, "rationalize_mtime", (fn)m_rationalize_mtime, 1);
    rb_define_method(cls, "read", (fn)m_read, 1);
    rb_define_method(cls, "read_body", (fn)m_read_body, 0);
    rb_define_method(cls, "read_chunked", (fn)m_read_chunked, 0);
    rb_define_method(cls, "read_length", (fn)m_read_length, 0);
    rb_define_method(cls, "readline", (fn)m_read_line, 0);
    rb_define_method(cls, "remaining", (fn)m_remaining, 0);
    rb_define_method(cls, "request_time", (fn)m_request_time, 0);
    rb_define_method(cls, "rflush", (fn)m_rflush, 0);
    rb_define_method(cls, "rputs", (fn)m_print, 1);
    rb_define_method(cls, "send_error_response", (fn)m_send_error_response, 1);
    rb_define_method(cls, "send_file", (fn)m_send_file, 1);
    rb_define_method(cls, "sent_sent_bodyct", (fn)m_sent_bodyct, 0);
    rb_define_method(cls, "server", (fn)m_server, 0);
    rb_define_method(cls, "set_content_length", (fn)m_set_content_length, 1);
    rb_define_method(cls, "set_content_type", (fn)m_set_content_type, 1);
    rb_define_method(cls, "set_etag", (fn)m_set_etag, 0);
    rb_define_method(cls, "set_keepalive", (fn)m_set_keepalive, 0);
    rb_define_method(cls, "set_max_content_length", (fn)m_set_max_content_length, 2);
    rb_define_method(cls, "set_status", (fn)m_set_status, 1);
    rb_define_method(cls, "setup_client_block", (fn)m_setup_client_block, 1);
    rb_define_method(cls, "should_client_block", (fn)m_should_client_block, 0);
    rb_define_method(cls, "some_auth_required", (fn)m_some_auth_required, 0);
    rb_define_method(cls, "status", (fn)m_status, 0);
    rb_define_method(cls, "status_line", (fn)m_status_line, 0);
    rb_define_method(cls, "subprocess_env", (fn)m_subprocess_env, 0);
    rb_define_method(cls, "headers_in", (fn)m_table_headers_in, 0);
    rb_define_method(cls, "headers_out", (fn)m_table_headers_out, 0);
    rb_define_method(cls, "the_request", (fn)m_the_request, 0);
    rb_define_method(cls, "unparsed_uri", (fn)m_unparsed_uri, 0);
    rb_define_method(cls, "uri", (fn)m_uri, 0);
    rb_define_method(cls, "m_user", (fn)m_user, 0);
    rb_define_method(cls, "vlist_validator", (fn)m_vlist_validator, 0);
    rb_define_method(cls, "write", (fn)m_write, 2);
}

// This is the way to make a request object. They cannot be instantiated from
// Ruby, only from C. C calls this function and gets back the VALUE for the
// instantiated request object.
VALUE make_request(request_rec* r)
{
    apache::Request* ptr = new apache::Request(r);

    VALUE obj = Data_Wrap_Struct(cls, NULL, deallocator, ptr);

    m_init(obj);

    return obj;
}

VALUE m_init(VALUE self)
{
    return self;
}

VALUE request_class()
{
    return cls;
}

static apache::Request* get_object(VALUE self);

apache::Request* get_object(VALUE self)
{
    apache::Request* obj;
    Data_Get_Struct(self, apache::Request, obj);

    return obj;
}

VALUE m_server(VALUE self)
{
    // Get the C++ request object
    apache::Request* obj = get_object(self);

    // Get the Apache server_rec pointer.
    server_rec* s = obj->server();

    // Create a new Ruby Server object
    VALUE server = make_server(s);

    // Assign the Ruby request instance to server::request to keep a reference
    // count on request.
    rb_ivar_set(server, rb_intern("request"), self);
    
    return server;
}

VALUE m_print(VALUE self, VALUE str)
{
    apache::Request* req = get_object(self);

    Check_Type(str, T_STRING);
    req->rputs(StringValuePtr(str));

    return Qnil;
}

VALUE m_puts(VALUE self, VALUE val)
{
    apache::Request* req = get_object(self);

    VALUE strval = rb_obj_as_string(val);
    req->rputs(StringValuePtr(strval));
    req->rputs("\n");

    return Qnil;
}

VALUE m_allow_options(VALUE self)
{
    apache::Request* req = get_object(self);
    
    return INT2FIX(req->allow_options());
}

VALUE m_allow_overrides(VALUE self)
{
    apache::Request* req = get_object(self);
    
    return INT2FIX(req->allow_overrides());
}

VALUE m_allowed(VALUE self)
{
    apache::Request* req = get_object(self);
    
    return INT2FIX(req->allowed());
}

VALUE m_set_allowed(VALUE self, VALUE bitmask)
{
    apache::Request* req = get_object(self);

    Check_Type(bitmask, T_FIXNUM);
    
    req->allowed(NUM2INT(bitmask));

    return Qnil;
}

VALUE m_args(VALUE self)
{
    apache::Request* req = get_object(self);
    
    if(req->args() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->args());
}

VALUE m_assbackwards(VALUE self)
{
    apache::Request* req = get_object(self);
    
    return INT2FIX(req->assbackwards());
}

VALUE m_auth_type(VALUE self)
{
    apache::Request* req = get_object(self);
    
    if(req->ap_auth_type() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->ap_auth_type());
}

VALUE m_sent_bodyct(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->sent_bodyct());
}

VALUE m_bytes_sent(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->bytes_sent());
}

VALUE m_chunked(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->chunked());
}

VALUE m_clength(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->clength());
}

VALUE m_connection(VALUE self)
{
    // Get the C++ request object
    apache::Request* obj = get_object(self);

    // Get the C++ connection object.
    conn_rec* c = obj->connection();

    // Create a new Ruby Server object
    VALUE connection = make_connection(c);

    // Assign the Ruby request instance to server::request to keep a reference
    // count on request.
    rb_ivar_set(connection, rb_intern("request"), self);
    
    return connection;
}

bool yield_content(const void* content, modruby::i32 len, void* ud = NULL)
{
    rb_yield(rb_str_new((const char*)content, len));
}

VALUE m_content(VALUE self)
{
    apache::Request* req = get_object(self);

    if(rb_block_given_p() == Qfalse)
    {
        // Caller wants one big blob
        const char* content = (const char*)req->content().data();

        if(content == NULL)
        {
            return Qnil;
        }

        return rb_str_new(content, req->content_length());
    }
    else
    {
        // Read it out little by little
        req->content(yield_content);
    }
    
    return Qnil;
}

VALUE m_content_encoding(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->content_encoding() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->content_encoding());
}

VALUE m_content_languages(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->content_languages() == NULL)
    {
        return Qnil;
    }

    return make_apr_array(req->content_languages());
}

VALUE m_content_type(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->content_type() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->content_type());
}

VALUE m_default_type(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->default_type() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->default_type());
}

VALUE m_discard_request_body(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->discard_request_body());
}

VALUE m_document_root(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->document_root() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->document_root());
}

VALUE m_err_headers_out(VALUE self)
{
    apache::Request* req = get_object(self);

    return make_apr_table(req->err_headers_out().handle);
}

VALUE m_filename(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->filename() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->filename());
}

VALUE m_finalize_request_protocol(VALUE self)
{
    apache::Request* req = get_object(self);

    req->finalize_request_protocol();

    return Qnil;
}

VALUE m_finfo(VALUE self)
{
    apache::Request* req = get_object(self);

    apr_finfo_t f = req->finfo();

    // Have to explicity pass f.name and f.fname because g++ sucks and screws up
    // the stack here for some reason.
    return make_apr_finfo_from_struct(f, req->filename(), f.fname);
}

VALUE m_get_error(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->error() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->error());
}

VALUE m_get_remote_logname(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->get_remote_logname() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->get_remote_logname());
}

VALUE m_get_server_name(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->get_server_name() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->get_server_name());
}

VALUE m_handler(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->handler() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->handler());
}

VALUE m_header_only(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->header_only());
}

VALUE m_hostname(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->hostname() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->hostname());
}

VALUE m_internal_redirect(VALUE self, VALUE url)
{
    apache::Request* req = get_object(self);

    Check_Type(url, T_STRING);

    req->internal_redirect(StringValuePtr(url));

    return Qnil;
}

VALUE m_internal_redirect_handler(VALUE self, VALUE url)
{
    apache::Request* req = get_object(self);

    Check_Type(url, T_STRING);

    req->internal_redirect_handler(StringValuePtr(url));

    return Qnil;
}

VALUE m_is_initial_req(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->is_initial_req());
}

VALUE m_log(VALUE self, VALUE level, VALUE msg)
{
    Check_Type(level,  T_FIXNUM);
    Check_Type(msg,    T_STRING);

    apache::Request* req = get_object(self);

    ap_log_rerror( APLOG_MARK,
                   NUM2INT(level),
                   0,
                   req->get_request_rec(),
                   StringValuePtr(msg) );

    return Qnil;
}

VALUE m_main(VALUE self)
{
    apache::Request* req = get_object(self);
    
    request_rec* main_req = req->main();

    if(main_req == NULL)
    {
        return Qnil;
    }

    return make_request(main_req);
}

VALUE m_make_content_type(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->content_type() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->content_type());
}

VALUE m_make_etag(VALUE self, VALUE force_weak)
{
    apache::Request* req = get_object(self);

    Check_Type(force_weak, T_FIXNUM);

    return INT2FIX(req->make_etag(NUM2INT(force_weak)));
}

VALUE m_meets_conditions(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->meets_conditions());
}

VALUE m_method(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->method() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->method());
}

VALUE m_method_number(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->method_number());
}

VALUE m_mtime(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->mtime());
}

VALUE m_next(VALUE self)
{
    apache::Request* req = get_object(self);

    request_rec* next_req = req->next();

    if(next_req == NULL)
    {
        return Qnil;
    }

    return make_request(next_req);
}

VALUE m_no_cache(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->no_cache());
}

VALUE m_no_local_copy(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->no_local_copy());
}

VALUE m_note_auth_failure(VALUE self)
{
    apache::Request* req = get_object(self);

    req->note_auth_failure();

    return Qnil;
}

VALUE m_note_basic_auth_failure(VALUE self)
{
    apache::Request* req = get_object(self);

    req->note_basic_auth_failure();

    return Qnil;
}

VALUE m_note_digest_auth_failure(VALUE self)
{
    apache::Request* req = get_object(self);

    req->note_digest_auth_failure();

    return Qnil;
}

VALUE m_notes(VALUE self)
{
    apache::Request* req = get_object(self);

    return make_apr_table(req->notes().handle);
}

VALUE m_params(VALUE self)
{
    apache::Request* req = get_object(self);

    apr_table_t* t = req->form_data();
    
    if(t == NULL)
    {
        // Return empty hash
        return rb_hash_new();
    }

    return make_apr_table(t);
}

// TODO: Wrap apr_uri_t
VALUE m_parsed_uri(VALUE self)
{
    apache::Request* req = get_object(self);

    //return rb_str_new2(req->parsed_uri());
    return Qnil;
}

VALUE m_path_info(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->path_info() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->path_info());
}

VALUE m_prev(VALUE self)
{
    apache::Request* req = get_object(self);

    request_rec* prev_req = req->prev();

    if(prev_req == NULL)
    {
        return Qnil;
    }

    return make_request(prev_req);
}

VALUE m_proto_num(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->proto_num());
}

VALUE m_protocol(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->protocol() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->protocol());
}

VALUE m_proxyreq(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->proxyreq());
}

VALUE m_queries(VALUE self)
{
    apache::Request* req = get_object(self);

    apr_table_t* t = req->queries();
    
    if(t == NULL)
    {
        return Qnil;
    }

    return make_apr_table(t);
}

VALUE m_range(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->range() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->range());
}

VALUE m_rationalize_mtime(VALUE self, VALUE mtime)
{
    apache::Request* req = get_object(self);

    Check_Type(mtime, T_FIXNUM);

    return INT2FIX(req->rationalize_mtime(NUM2INT(mtime)));
}

VALUE m_read_body(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->read_body());
}

VALUE m_read_chunked(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->read_chunked());
}

VALUE m_read_length(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->read_length());
}

VALUE m_read_line(VALUE self)
{
    apache::Request* req = get_object(self);

    // Check content type
    string content_type = to_upper(req->headers_in().get("Accept-Charset"));

    // Search for matching encoding
    int index = rb_enc_find_index(content_type.c_str());
    if(index > 0)
    {
        rb_encoding* encoding = rb_enc_from_index(index);

        return rb_enc_str_new( req->read_line().c_str(), 
                               req->read_line().size(), 
                               encoding );
    }
    else
    {    
        // Default UTF8
        return rb_external_str_new_cstr(req->read_line().c_str());
    }
}

VALUE m_read(VALUE self, VALUE bytes)
{
    apache::Request* req = get_object(self);

    Check_Type(bytes, T_FIXNUM);

    // Allocate space
    char* buff = (char*)malloc(NUM2INT(bytes));

    // Do the read
    int n = req->read(&buff, NUM2INT(bytes));

    if(n <= 0)
    {
        // Free the space
        free(buff);

        return Qnil;
    }

    // Create a string from data
    VALUE str = rb_str_new(buff, n);

    // Free the space
    free(buff);

    return str;
}

VALUE m_remaining(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->remaining());
}

VALUE m_request_time(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->request_time());
}

VALUE m_rflush(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->rflush());
}

VALUE m_send_error_response(VALUE self, VALUE recursive_error)
{
    apache::Request* req = get_object(self);

    Check_Type(recursive_error, T_FIXNUM);

    req->send_error_response(NUM2INT(recursive_error));

    return Qnil;
}

VALUE m_send_file(VALUE self, VALUE path)
{
    apache::Request* req = get_object(self);

    Check_Type(path, T_STRING);

    return INT2FIX(req->send_file(StringValuePtr(path)));
}

VALUE m_set_content_length(VALUE self, VALUE length)
{
    apache::Request* req = get_object(self);

    Check_Type(length, T_FIXNUM);

    req->set_content_length(NUM2INT(length));

    return Qnil;
}

VALUE m_set_content_type(VALUE self, VALUE type)
{
    apache::Request* req = get_object(self);

    Check_Type(type, T_STRING);

    req->set_content_type(StringValuePtr(type));

    return Qnil;
}

VALUE m_set_etag(VALUE self)
{
    apache::Request* req = get_object(self);

    req->set_etag();

    return Qnil;
}

VALUE m_set_keepalive(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->set_keepalive());
}

VALUE m_set_max_content_length(VALUE self, VALUE bytes)
{
    Check_Type(bytes, T_FIXNUM);

    apache::Request::max_content_length = NUM2INT(bytes);

    return Qnil;
}

VALUE m_set_status(VALUE self, VALUE status)
{
    apache::Request* req = get_object(self);

    Check_Type(status, T_FIXNUM);

    req->set_status(NUM2INT(status));

    return Qnil;
}

VALUE m_setup_client_block(VALUE self, VALUE policy)
{
    apache::Request* req = get_object(self);

    Check_Type(policy, T_STRING);

    return INT2FIX(req->setup_client_block(NUM2INT(policy)));
}

VALUE m_should_client_block(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->should_client_block());
}

VALUE m_some_auth_required(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->some_auth_required());
}

VALUE m_status(VALUE self)
{
    apache::Request* req = get_object(self);

    return INT2FIX(req->status());
}

VALUE m_status_line(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->status_line() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->status_line());
}

VALUE m_subprocess_env(VALUE self)
{
    apache::Request* req = get_object(self);

    return make_apr_table(req->subprocess_env().handle);
}

VALUE m_table_headers_in(VALUE self)
{
    apache::Request* req = get_object(self);

    return make_apr_table(req->headers_in().handle);
}

VALUE m_table_headers_out(VALUE self)
{
    apache::Request* req = get_object(self);

    return make_apr_table(req->headers_out().handle);
}

VALUE m_the_request(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->the_request() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->the_request());
}

VALUE m_unparsed_uri(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->unparsed_uri() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->unparsed_uri());
}

VALUE m_uri(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->uri() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->uri());
}

VALUE m_user(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->user() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->user());
}

VALUE m_vlist_validator(VALUE self)
{
    apache::Request* req = get_object(self);

    if(req->vlist_validator() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(req->vlist_validator());
}

VALUE m_write(VALUE self, VALUE data, VALUE bytes)
{
    apache::Request* req = get_object(self);

    req->rwrite((void*)StringValuePtr(data), NUM2INT(bytes));

    return Qnil;
}
