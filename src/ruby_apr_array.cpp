#include <ruby.h>

#include "apr.h"
#include "apr_table.h"
#include "library.h"
#include "ruby_apr_array.h"
#include "ruby_apr_pool.h"

typedef VALUE (*fn)(...);

#define CLASS_NAME "Array"

extern "C" {

static VALUE m_init(VALUE self);
static VALUE m_each(VALUE self);
static VALUE m_keys(VALUE self);
static VALUE m_size(VALUE self);

}

static apr_array_header_t* get_object(VALUE self);

static void deallocator(void* x)
{
    
}

static VALUE allocator(VALUE cls)
{
    rb_raise( rb_eRuntimeError, 
              "Cannot create a ARP::array object from Ruby" );
}

static VALUE cls;

void init_apr_array(VALUE module)
{
    cls = rb_define_class_under(module, CLASS_NAME, rb_cObject);
    rb_define_alloc_func(cls, allocator);

    rb_define_method(cls, "initialize", (fn)m_init, 1);
    rb_define_method(cls, "size", (fn)m_size, 0);
    rb_define_method(cls, "each", (fn)m_each, 0);
}

VALUE make_apr_array(const apr_array_header_t* h)
{
    VALUE obj = Data_Wrap_Struct(cls, NULL, deallocator, (void*)h);

    return obj;
}

VALUE m_init(VALUE self)
{
    return self;
}

VALUE apr_array_class()
{
    return cls;
}

apr_array_header_t* get_object(VALUE self)
{
    apr_array_header_t* obj;
    Data_Get_Struct(self, apr_array_header_t, obj);

    return obj;
}

VALUE m_size(VALUE self)
{
    apr_array_header_t* h = get_object(self);

    printf("%x\n", h);

    return INT2FIX(h->nelts);
}

VALUE m_each(VALUE self)
{
    apr_array_header_t* h = get_object(self);

    int i;
    for(i=0; i < h->nelts; i++)
    {
        const char* e = ((const char**)h->elts)[i];

        rb_yield(rb_ary_new3(1, rb_str_new2(e)));
    }

    return Qnil;
}
