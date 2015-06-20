#include <ruby.h>

#include "request.h"
#include "server.h"
#include "ruby_apr_table.h"
#include "ruby_apr_array.h"
#include "ruby_request.h"
#include "ruby_server.h"
#include "ruby_process.h"

typedef VALUE (*fn)(...);

#define CLASS_NAME "Server"

extern "C" {

    static VALUE m_init(VALUE self);
    static VALUE m_addrs(VALUE self);
    static VALUE m_defn_line_number(VALUE self);
    static VALUE m_defn_name(VALUE self);
    static VALUE m_error_log(VALUE self);
    static VALUE m_error_fname(VALUE self);
    static VALUE m_is_virtual(VALUE self);
    static VALUE m_keep_alive(VALUE self);
    static VALUE m_keep_alive_max(VALUE self);
    static VALUE m_keep_alive_timeout(VALUE self);
    static VALUE m_limit_req_fields(VALUE self);
    static VALUE m_limit_req_fieldsize(VALUE self);
    static VALUE m_limit_req_line(VALUE self);
    static VALUE m_loglevel(VALUE self);
    static VALUE m_lookup_defaults(VALUE self);
    static VALUE m_names(VALUE self);
    static VALUE m_next(VALUE self);
    static VALUE m_path(VALUE self);
    static VALUE m_pathlen(VALUE self);
    static VALUE m_port(VALUE self);
    static VALUE m_process(VALUE self);
    static VALUE m_server_admin(VALUE self);
    static VALUE m_server_hostname(VALUE self);
    static VALUE m_timeout(VALUE self);
    static VALUE m_wild_names(VALUE self);

}

static apache::Server* get_object(VALUE self);

static void deallocator(void* x)
{
    if (x != NULL)
    {
        delete (apache::Server*)x;
        x = NULL;
    }
}

static VALUE allocator(VALUE cls)
{
    apache::Server* x = NULL;

    rb_raise( rb_eRuntimeError,
              "Cannot create a Apache::Server object this way" );

    return Data_Wrap_Struct(cls, NULL, deallocator, x);
}

static VALUE cls;

void init_server(VALUE module)
{
    cls = rb_define_class_under(module, CLASS_NAME, rb_cObject);
    rb_define_alloc_func(cls, allocator);

    rb_define_method(cls, "initialize", (fn)m_init, 0);
    rb_define_method(cls, "addrs", (fn)m_addrs, 0);
    rb_define_method(cls, "defn_line_number", (fn)m_defn_line_number, 0);
    rb_define_method(cls, "defn_name", (fn)m_defn_name, 0);
    rb_define_method(cls, "error_log", (fn)m_error_log, 0);
    rb_define_method(cls, "error_fname", (fn)m_error_fname, 0);
    rb_define_method(cls, "is_virtual", (fn)m_is_virtual, 0);
    rb_define_method(cls, "keep_alive", (fn)m_keep_alive, 0);
    rb_define_method(cls, "keep_alive_max", (fn)m_keep_alive_max, 0);
    rb_define_method(cls, "keep_alive_timeout", (fn)m_keep_alive_timeout, 0);
    rb_define_method(cls, "limit_req_fields", (fn)m_limit_req_fields, 0);
    rb_define_method(cls, "limit_req_fieldsize", (fn)m_limit_req_fieldsize, 0);
    rb_define_method(cls, "limit_req_line", (fn)m_limit_req_line, 0);
    rb_define_method(cls, "loglevel", (fn)m_loglevel, 0);
    rb_define_method(cls, "lookup_defaults", (fn)m_lookup_defaults, 0);
    rb_define_method(cls, "names", (fn)m_names, 0);
    rb_define_method(cls, "next", (fn)m_next, 0);
    rb_define_method(cls, "path", (fn)m_path, 0);
    rb_define_method(cls, "pathlen", (fn)m_pathlen, 0);
    rb_define_method(cls, "port", (fn)m_port, 0);
    rb_define_method(cls, "process", (fn)m_process, 0);
    rb_define_method(cls, "server_admin", (fn)m_server_admin, 0);
    rb_define_method(cls, "server_hostname", (fn)m_server_hostname, 0);
    rb_define_method(cls, "timeout", (fn)m_timeout, 0);
    rb_define_method(cls, "wild_names", (fn)m_wild_names, 0);
}

VALUE make_server(const server_rec* s)
{
    apache::Server* p_server = new apache::Server(s);

    VALUE obj = Data_Wrap_Struct(cls, NULL, deallocator, p_server);

    m_init(obj);

    return obj;
}

VALUE m_init(VALUE self)
{
    return self;
}

VALUE server_class()
{
    return cls;
}

apache::Server* get_object(VALUE self)
{
    apache::Server* obj;
    Data_Get_Struct(self, apache::Server, obj);

    return obj;
}

VALUE m_addrs(VALUE self)
{
    return Qnil;
}

VALUE m_defn_line_number(VALUE self)
{
    apache::Server* obj = get_object(self);

    return INT2FIX(obj->defn_line_number());
}

VALUE m_defn_name(VALUE self)
{
    apache::Server* obj = get_object(self);

    if (obj->defn_name() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(obj->defn_name());
}

VALUE m_error_log(VALUE self)
{
    return Qnil;
}

VALUE m_error_fname(VALUE self)
{
    apache::Server* obj = get_object(self);

    if (obj->error_fname() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(obj->error_fname());
}

VALUE m_is_virtual(VALUE self)
{
    apache::Server* obj = get_object(self);

    return INT2FIX(obj->is_virtual());
}

VALUE m_keep_alive(VALUE self)
{
    apache::Server* obj = get_object(self);

    return INT2FIX(obj->keep_alive());
}

VALUE m_keep_alive_max(VALUE self)
{
    apache::Server* obj = get_object(self);

    return INT2FIX(obj->keep_alive_max());
}

VALUE m_keep_alive_timeout(VALUE self)
{
    apache::Server* obj = get_object(self);

    return INT2FIX(obj->keep_alive_timeout());
}

VALUE m_limit_req_fields(VALUE self)
{
    apache::Server* obj = get_object(self);

    return INT2FIX(obj->limit_req_fields());
}

VALUE m_limit_req_fieldsize(VALUE self)
{
    apache::Server* obj = get_object(self);

    return INT2FIX(obj->limit_req_fieldsize());
}

VALUE m_limit_req_line(VALUE self)
{
    apache::Server* obj = get_object(self);

    return INT2FIX(obj->limit_req_line());
}

VALUE m_loglevel(VALUE self)
{
    apache::Server* obj = get_object(self);

    return INT2FIX(obj->loglevel());
}

VALUE m_lookup_defaults(VALUE self)
{
    return Qnil;
}

VALUE m_names(VALUE self)
{
    apache::Server* obj = get_object(self);

    if (obj->names() == NULL)
    {
        return Qnil;
    }

    return make_apr_array(obj->names());
}

VALUE m_next(VALUE self)
{
    apache::Server* obj = get_object(self);

    if (obj->next() == NULL)
    {
        return Qnil;
    }

    return make_server(obj->next()->get_server_rec());
}

VALUE m_path(VALUE self)
{
    apache::Server* server = get_object(self);

    if (server->path() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(server->path());
}

VALUE m_pathlen(VALUE self)
{
    apache::Server* obj = get_object(self);

    return INT2FIX(obj->pathlen());
}

VALUE m_port(VALUE self)
{
    apache::Server* obj = get_object(self);

    return INT2FIX(obj->port());
}

VALUE m_process(VALUE self)
{
    // Get the C++ server object
    apache::Server* obj = get_object(self);

    // Get the Apache server_rec pointer.
    // const process_rec* p = obj->process();

    // Create a new Ruby Process object
    VALUE process = make_process(obj->get_server_rec());

    // Assign the Ruby request instance to server::request to keep a reference
    // count on request.
    rb_ivar_set(process, rb_intern("server"), self);

    return process;
}

VALUE m_server_admin(VALUE self)
{
    apache::Server* obj = get_object(self);

    if (obj->server_admin())
    {
        return Qnil;
    }

    return rb_str_new2(obj->server_admin());
}

VALUE m_server_hostname(VALUE self)
{
    apache::Server* obj = get_object(self);

    if (obj->server_hostname())
    {
        return Qnil;
    }

    return rb_str_new2(obj->server_hostname());
}

VALUE m_timeout(VALUE self)
{
    apache::Server* obj = get_object(self);

    return INT2FIX(obj->timeout());
}

VALUE m_wild_names(VALUE self)
{
    apache::Server* obj = get_object(self);

    if (obj->wild_names() == NULL)
    {
        return Qnil;
    }

    return make_apr_array(obj->wild_names());
}
