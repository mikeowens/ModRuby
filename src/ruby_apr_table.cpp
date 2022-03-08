#include <ruby.h>

#include "apr_table.h"
#include "ruby.hpp"
#include "ruby_apr_table.h"
#include "ruby_apr_pool.h"

#include <string>

using std::string;

typedef VALUE (*fn)(...);

#define CLASS_NAME "Table"

extern "C" {

static VALUE m_init(VALUE self, VALUE pool);
static VALUE m_id(VALUE self);

static VALUE m_each(VALUE self);
static VALUE m_keys(VALUE self);
static VALUE m_values(VALUE self);
static VALUE m_size(VALUE self);
static VALUE m_clear(VALUE self);
static VALUE m_has_key(VALUE self, VALUE key);
static VALUE m_get(VALUE self, VALUE key);
static VALUE m_set(VALUE self, VALUE key, VALUE value);
static VALUE m_add(VALUE self, VALUE key, VALUE value);
static VALUE m_merge(VALUE self, VALUE key, VALUE value);
static VALUE m_unset(VALUE self, VALUE key);
static VALUE m_join(VALUE self, VALUE delimiter);

} // extern "C"

static modruby::apr::table* get_object(VALUE self);

static void deallocator(void* x)
{
    if (x != NULL)
    {
        delete (modruby::apr::table*)x;
        x = NULL;
    }
}

static VALUE allocator(VALUE cls)
{
    apr_table_t* t = NULL;
    modruby::apr::table* x = new modruby::apr::table(t);

    return Data_Wrap_Struct(cls, NULL, deallocator, x);
}

static VALUE cls;

void init_apr_table(VALUE module)
{
    cls = rb_define_class_under(module, CLASS_NAME, rb_cObject);
    rb_define_alloc_func(cls, allocator);

    rb_define_method(cls, "initialize", (fn)m_init, 1);
    rb_define_method(cls, "each", (fn)m_each, 0);
    rb_define_method(cls, "keys", (fn)m_keys, 0);
    rb_define_method(cls, "values", (fn)m_values, 0);
    rb_define_method(cls, "size", (fn)m_size, 0);
    rb_define_method(cls, "clear", (fn)m_clear, 0);
    rb_define_method(cls, "get", (fn)m_get, 1);
    rb_define_method(cls, "[]", (fn)m_get, 1);
    rb_define_method(cls, "has_key?", (fn)m_has_key, 1);
    rb_define_method(cls, "set", (fn)m_set, 2);
    rb_define_method(cls, "add", (fn)m_add, 2);
    rb_define_method(cls, "[]=", (fn)m_set, 2);
    rb_define_method(cls, "merge", (fn)m_merge, 2);
    rb_define_method(cls, "unset", (fn)m_unset, 1);
    rb_define_method(cls, "join", (fn)m_join, 1);

}

VALUE make_apr_table(apr_table_t* t)
{
    modruby::apr::table* p_table = new modruby::apr::table(t);

    VALUE obj = Data_Wrap_Struct(cls, NULL, deallocator, p_table);

    return obj;
}

VALUE m_init(VALUE self, VALUE pool)
{
    Check_Type(pool, T_DATA);

    // Ensure that the pool argument is indeed a pool class
    ruby::require_class(pool, apr_pool_class());

    // Assign the pool instance to keep a reference count on it.
    rb_ivar_set(self, rb_intern("pool"), pool);

    modruby::apr::table* t = get_object(self);

    // Get the pool handle
    apr_pool_t* p;
    Data_Get_Struct(pool, apr_pool_t, p);

    // Create a apr table with it
    t->handle = apr_table_make(p, 5);;

    return self;
}

VALUE apr_table_class()
{
    return cls;
}

modruby::apr::table* get_object(VALUE self)
{
    modruby::apr::table* obj;
    Data_Get_Struct(self, modruby::apr::table, obj);

    return obj;
}

VALUE m_each(VALUE self)
{
    modruby::apr::table* table = get_object(self);

    modruby::apr::table::iterator i(*table);

    while (i.next())
    {
        rb_yield( rb_ary_new3( 2,
                               rb_str_new2(i.key()),
                               rb_str_new2(i.data())) );
    }

    return Qnil;
}

VALUE m_keys(VALUE self)
{
    modruby::apr::table* table = get_object(self);

    VALUE a = rb_ary_new();

    modruby::apr::table::iterator i(*table);

    while (i.next())
    {
        rb_ary_push(a, rb_str_new2(i.key()));
    }

    return a;
}

VALUE m_values(VALUE self)
{
    modruby::apr::table* table = get_object(self);

    VALUE a = rb_ary_new();

    modruby::apr::table::iterator i(*table);

    while (i.next())
    {
        rb_ary_push(a, rb_str_new2(i.data()));
    }

    return a;
}


VALUE m_size(VALUE self)
{
    modruby::apr::table* table = get_object(self);

    return INT2FIX(table->size());
}

VALUE m_clear(VALUE self)
{
    modruby::apr::table* table = get_object(self);

    table->clear();

    return Qnil;
}

VALUE m_set(VALUE self, VALUE key, VALUE value)
{
    Check_Type(key, T_STRING);
    Check_Type(value, T_STRING);

    modruby::apr::table* table = get_object(self);

    table->set(StringValuePtr(key), StringValuePtr(value));

    return Qnil;
}

VALUE m_add(VALUE self, VALUE key, VALUE value)
{
    Check_Type(key, T_STRING);
    Check_Type(value, T_STRING);

    modruby::apr::table* table = get_object(self);

    table->add(StringValuePtr(key), StringValuePtr(value));

    return Qnil;
}

VALUE m_get(VALUE self, VALUE key)
{
    Check_Type(key, T_STRING);

    modruby::apr::table* table = get_object(self);

    const char* value = table->get(StringValuePtr(key));

    if (value == NULL)
    {
        return Qnil;
    }

    return rb_str_new2(value);
}

VALUE m_has_key(VALUE self, VALUE key)
{
    Check_Type(key, T_STRING);

    modruby::apr::table* table = get_object(self);

    const char* value = table->get(StringValuePtr(key));

    if (value == NULL)
    {
        return Qfalse;
    }

    return Qtrue;
}

VALUE m_merge(VALUE self, VALUE key, VALUE value)
{
    Check_Type(key, T_STRING);
    Check_Type(value, T_STRING);

    modruby::apr::table* table = get_object(self);

    table->merge(StringValuePtr(key), StringValuePtr(value));

    return Qnil;
}

VALUE m_unset(VALUE self, VALUE key)
{
    Check_Type(key, T_STRING);

    modruby::apr::table* table = get_object(self);

    table->unset(StringValuePtr(key));

    return Qnil;
}

VALUE m_join(VALUE self, VALUE delimiter)
{
    Check_Type(delimiter, T_STRING);

    modruby::apr::table* table = get_object(self);

    string str = table->join(StringValuePtr(delimiter));

    return rb_str_new2(str.c_str());
}
