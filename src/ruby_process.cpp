#include <ruby.h>

#include "request.h"
#include "server.h"
#include "ruby_request.h"
#include "ruby_server.h"
#include "ruby_process.h"

typedef VALUE (*fn)(...);

#define CLASS_NAME "Process"

extern "C" {

    static VALUE m_argc(VALUE self);
    static VALUE m_argv(VALUE self);
    static VALUE m_init(VALUE self);
    static VALUE m_short_name(VALUE self);

}

static apache::Process* get_object(VALUE self);

static void deallocator(void* x)
{
    if (x != NULL)
    {
        delete (apache::Process*)x;
        x = NULL;
    }
}

static VALUE allocator(VALUE cls)
{
    apache::Process* x = NULL;

    rb_raise( rb_eRuntimeError,
              "Cannot create a Apache::Process object this way" );

    return Data_Wrap_Struct(cls, NULL, deallocator, x);
}

static VALUE cls;

void init_process(VALUE module)
{
    cls = rb_define_class_under(module, CLASS_NAME, rb_cObject);
    rb_define_alloc_func(cls, allocator);

    rb_define_method(cls, "initialize",     (fn)m_init, 0);
    rb_define_method(cls, "argc",           (fn)m_argc, 0);
    rb_define_method(cls, "id",             (fn)m_argv, 0);
    rb_define_method(cls, "short_name",     (fn)m_short_name, 0);
}

VALUE make_process(const server_rec* s)
{
    apache::Process* p_process = new apache::Process(s->process);

    VALUE obj = Data_Wrap_Struct(cls, NULL, deallocator, p_process);

    m_init(obj);

    return obj;
}

VALUE m_init(VALUE self)
{
    return self;
}

VALUE process_class()
{
    return cls;
}

apache::Process* get_object(VALUE self)
{
    apache::Process* obj;
    Data_Get_Struct(self, apache::Process, obj);

    return obj;
}

VALUE m_argc(VALUE self)
{
    apache::Process* obj = get_object(self);

    return INT2FIX(obj->argc());
}

VALUE m_argv(VALUE self)
{
    apache::Process* obj = get_object(self);

    VALUE a = rb_ary_new();

    int i;
    int count = obj->argc();

    const char* const* argv = obj->argv();

    for (i = 0; i < count; i++)
    {
        rb_ary_push(a, rb_str_new2(argv[i]));
    }

    return a;
}

VALUE m_short_name(VALUE self)
{
    apache::Process* obj = get_object(self);

    if (obj->short_name() == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(obj->short_name());
}
