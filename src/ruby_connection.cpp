#include <ruby.h>

#include "request.h"
#include "server.h"
#include "connection.h"
#include "ruby_request.h"
#include "ruby_server.h"
#include "ruby_connection.h"

typedef VALUE (*fn)(...);

#define CLASS_NAME "Connection"

extern "C" {

    static VALUE m_aborted(VALUE self);
    static VALUE m_init(VALUE self);
    static VALUE m_id(VALUE self);
    static VALUE m_keepalive(VALUE self);
    static VALUE m_local_host(VALUE self);
    static VALUE m_local_ip(VALUE self);
    static VALUE m_local_port(VALUE self);
    static VALUE m_remote_host(VALUE self);
    static VALUE m_remote_ip(VALUE self);
    static VALUE m_remote_port(VALUE self);
    static VALUE m_remote_logname(VALUE self);

}

static apache::Connection* get_object(VALUE self);

static void deallocator(void* x)
{
    if (x != NULL)
    {
        delete (apache::Connection*)x;
        x = NULL;
    }
}

static VALUE allocator(VALUE cls)
{
    apache::Connection* x = NULL;

    rb_raise( rb_eRuntimeError,
              "Cannot create a Apache::Server object this way" );

    return Data_Wrap_Struct(cls, NULL, deallocator, x);
}

static VALUE cls;

void init_connection(VALUE module)
{
    cls = rb_define_class_under(module, CLASS_NAME, rb_cObject);
    rb_define_alloc_func(cls, allocator);

    rb_define_method(cls, "initialize",     (fn)m_init, 0);
    rb_define_method(cls, "aborted",        (fn)m_aborted, 0);
    rb_define_method(cls, "id",             (fn)m_id, 0);
    rb_define_method(cls, "keepalive",      (fn)m_keepalive, 0);
    rb_define_method(cls, "local_host",     (fn)m_local_host, 0);
    rb_define_method(cls, "local_ip",       (fn)m_local_ip, 0);
    rb_define_method(cls, "local_port",     (fn)m_local_port, 0);
    rb_define_method(cls, "remote_host",    (fn)m_remote_host, 0);
    rb_define_method(cls, "remote_ip",      (fn)m_remote_ip, 0);
    rb_define_method(cls, "remote_port",    (fn)m_remote_port, 0);
    rb_define_method(cls, "remote_logname", (fn)m_remote_ip, 0);
}

VALUE make_connection(conn_rec* c)
{
    apache::Connection* p_conn = new apache::Connection(c);

    VALUE obj = Data_Wrap_Struct(cls, NULL, deallocator, p_conn);

    m_init(obj);

    return obj;
}

VALUE m_init(VALUE self)
{
    return self;
}

VALUE connection_class()
{
    return cls;
}

apache::Connection* get_object(VALUE self)
{
    apache::Connection* obj;
    Data_Get_Struct(self, apache::Connection, obj);

    return obj;
}

VALUE m_aborted(VALUE self)
{
    apache::Connection* obj = get_object(self);

    return INT2FIX(obj->aborted());
}

VALUE m_id(VALUE self)
{
    apache::Connection* obj = get_object(self);

    return INT2FIX(obj->id());
}

VALUE m_keepalive(VALUE self)
{
    apache::Connection* obj = get_object(self);

    return INT2FIX(obj->keepalive());
}

VALUE m_local_host(VALUE self)
{
    apache::Connection* obj = get_object(self);

    if (obj->local_host() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(obj->local_host());
}

VALUE m_local_ip(VALUE self)
{
    apache::Connection* obj = get_object(self);

    return rb_str_new2(obj->local_ip());
}

VALUE m_local_port(VALUE self)
{
    apache::Connection* obj = get_object(self);

    return INT2FIX(obj->local_port());
}

VALUE m_remote_host(VALUE self)
{
    apache::Connection* obj = get_object(self);

    if (obj->remote_host() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(obj->remote_host());
}

VALUE m_remote_ip(VALUE self)
{
    apache::Connection* obj = get_object(self);

    return rb_str_new2(obj->remote_ip());
}

VALUE m_remote_port(VALUE self)
{
    apache::Connection* obj = get_object(self);

    return INT2FIX(obj->remote_port());
}

VALUE m_remote_logname(VALUE self)
{
    apache::Connection* obj = get_object(self);

    return rb_str_new2(obj->remote_logname());
}

VALUE m_server(VALUE self)
{
    // Get the C++ request object
    apache::Connection* obj = get_object(self);

    // Get the Apache server_rec pointer.
    const server_rec* s = obj->server();

    // Create a new Ruby Server object
    VALUE server = make_server(s);

    // Assign the Ruby request instance to server::request to keep a reference
    // count on request.
    rb_ivar_set(server, rb_intern("connection"), self);

    return server;
}
