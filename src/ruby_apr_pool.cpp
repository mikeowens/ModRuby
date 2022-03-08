#include <ruby.h>

#include "apr.h"
#include "ruby_apr_pool.h"

typedef VALUE (*fn)(...);

#define CLASS_NAME "Pool"

extern "C" {

static VALUE m_init(VALUE self);
static VALUE m_id(VALUE self);
static VALUE m_set(VALUE self, VALUE key);

} // extern "C"

static apr_pool_t* get_object(VALUE self);

static void deallocator(void* x)
{
    if (x != NULL)
    {
        apr_pool_destroy((apr_pool_t*)x);
        x = NULL;
    }
}

static VALUE allocator(VALUE cls)
{
    apr_pool_t* p = NULL;
    apr_pool_create(&p, NULL);

    return Data_Wrap_Struct(cls, NULL, deallocator, p);
}

static VALUE cls;

void init_apr_pool(VALUE module)
{
    cls = rb_define_class_under(module, CLASS_NAME, rb_cObject);
    rb_define_alloc_func(cls, allocator);

    rb_define_method(cls, "initialize", (fn)m_init, 0);
}

VALUE m_init(VALUE self)
{
    return self;
}

VALUE apr_pool_class()
{
    return cls;
}

apr_pool_t* get_object(VALUE self)
{
    apr_pool_t* obj;
    Data_Get_Struct(self, apr_pool_t, obj);

    return obj;
}

VALUE make_apr_pool()
{
    apr_pool_t* p = NULL;
    apr_pool_create(&p, NULL);

    VALUE obj = Data_Wrap_Struct(cls, NULL, deallocator, p);

    return m_init(obj);
}
