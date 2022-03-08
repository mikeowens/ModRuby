#include <ruby.h>

#include "apr.h"
#include "ruby.hpp"
#include "ruby_apr_file_info.h"
#include "ruby_apr_pool.h"

#include <string>

using std::string;

typedef VALUE (*fn)(...);

#define CLASS_NAME "FileInfo"

extern "C" {

static VALUE m_init(VALUE self, VALUE dir, VALUE pool, VALUE want_flags);

static VALUE m_atime(VALUE self);
static VALUE m_csize(VALUE self);
static VALUE m_ctime(VALUE self);
static VALUE m_device(VALUE self);
static VALUE m_fname(VALUE self);
static VALUE m_group(VALUE self);
static VALUE m_inode(VALUE self);
static VALUE m_md5(VALUE self);
static VALUE m_mode(VALUE self);
static VALUE m_name(VALUE self);
static VALUE m_nlink(VALUE self);
static VALUE m_protection(VALUE self);
static VALUE m_user(VALUE self);
static VALUE m_sha1(VALUE self);
static VALUE m_size(VALUE self);
static VALUE m_mtime(VALUE self);
static VALUE m_type(VALUE self);
static VALUE m_valid(VALUE self);

static VALUE m_stat(VALUE file);

} // extern "C"

using namespace modruby;

struct wrapper
{
    modruby::apr::file_info* finfo;
};

static modruby::apr::file_info* get_object(VALUE self);

static void deallocator(void* x)
{
    if (x != NULL)
    {
        wrapper* w = (wrapper*)x;

        if (w->finfo != NULL)
        {
            delete (modruby::apr::file_info*)w->finfo;

            w->finfo = NULL;
        }
    }
}

static VALUE allocator(VALUE cls)
{
    wrapper* w = (wrapper*)malloc(sizeof(wrapper));

    return Data_Wrap_Struct(cls, NULL, deallocator, w);
}

static VALUE cls;

void init_apr_file_info(VALUE module)
{
    cls = rb_define_class_under(module, CLASS_NAME, rb_cObject);

    rb_define_alloc_func(cls, allocator);

    rb_define_method(cls, "atime",      (fn)m_atime, 0);
    rb_define_method(cls, "ctime",      (fn)m_ctime, 0);
    rb_define_method(cls, "csize",      (fn)m_csize, 0);
    rb_define_method(cls, "device",     (fn)m_device, 0);
    rb_define_method(cls, "gid",        (fn)m_group, 0);
    rb_define_method(cls, "filetype",   (fn)m_type, 0);
    rb_define_method(cls, "fname",      (fn)m_fname, 0);
    rb_define_method(cls, "group",      (fn)m_group, 0);
    rb_define_method(cls, "initialize", (fn)m_init,  2);
    rb_define_method(cls, "inode",      (fn)m_inode, 0);
    rb_define_method(cls, "mode",       (fn)m_mode, 0);
    rb_define_method(cls, "md5",        (fn)m_md5, 0);
    rb_define_method(cls, "sha1",       (fn)m_sha1, 0);
    rb_define_method(cls, "mtime",      (fn)m_mtime, 0);
    rb_define_method(cls, "name",       (fn)m_name, 0);
    rb_define_method(cls, "nlink",      (fn)m_nlink, 0);
    rb_define_method(cls, "protection", (fn)m_protection, 0);
    rb_define_method(cls, "size",       (fn)m_size, 0);
    rb_define_method(cls, "type",       (fn)m_type, 0);
    rb_define_method(cls, "uid",        (fn)m_user, 0);
    rb_define_method(cls, "user",       (fn)m_user, 0);
    rb_define_method(cls, "valid",      (fn)m_valid, 0);

    // APR file type constants
    rb_define_global_const("APR_NOFILE",  INT2FIX(APR_NOFILE));
    rb_define_global_const("APR_REG",     INT2FIX(APR_REG));
    rb_define_global_const("APR_DIR",     INT2FIX(APR_DIR));
    rb_define_global_const("APR_CHR",     INT2FIX(APR_CHR));
    rb_define_global_const("APR_BLK",     INT2FIX(APR_BLK));
    rb_define_global_const("APR_PIPE",    INT2FIX(APR_PIPE));
    rb_define_global_const("APR_LNK",     INT2FIX(APR_LNK));
    rb_define_global_const("APR_SOCK",    INT2FIX(APR_SOCK));
    rb_define_global_const("APR_UNKFILE", INT2FIX(APR_UNKFILE));

    rb_define_global_const("APR_FINFO_LINK",    INT2FIX(0x00000001));
    rb_define_global_const("APR_FINFO_MTIME",   INT2FIX(0x00000010));
    rb_define_global_const("APR_FINFO_CTIME",   INT2FIX(0x00000020));
    rb_define_global_const("APR_FINFO_ATIME",   INT2FIX(0x00000040));
    rb_define_global_const("APR_FINFO_SIZE",    INT2FIX(0x00000100));
    rb_define_global_const("APR_FINFO_CSIZE",   INT2FIX(0x00000200));
    rb_define_global_const("APR_FINFO_DEV",     INT2FIX(0x00001000));
    rb_define_global_const("APR_FINFO_INODE",   INT2FIX(0x00002000));
    rb_define_global_const("APR_FINFO_NLINK",   INT2FIX(0x00004000));
    rb_define_global_const("APR_FINFO_TYPE",    INT2FIX(0x00008000));
    rb_define_global_const("APR_FINFO_USER",    INT2FIX(0x00010000));
    rb_define_global_const("APR_FINFO_GROUP",   INT2FIX(0x00020000));
    rb_define_global_const("APR_FINFO_UPROT",   INT2FIX(0x00100000));
    rb_define_global_const("APR_FINFO_GPROT",   INT2FIX(0x00200000));
    rb_define_global_const("APR_FINFO_WPROT",   INT2FIX(0x00400000));
    rb_define_global_const("APR_FINFO_ICASE",   INT2FIX(0x01000000));
    rb_define_global_const("APR_FINFO_NAME",    INT2FIX(0x02000000));
    rb_define_global_const("APR_FINFO_MIN",     INT2FIX(0x00008170));
    rb_define_global_const("APR_FINFO_IDENT",   INT2FIX(0x00003000));
    rb_define_global_const("APR_FINFO_OWNER",   INT2FIX(0x00030000));
    rb_define_global_const("APR_FINFO_PROT",    INT2FIX(0x00700000));
    rb_define_global_const("APR_FINFO_NORM",    INT2FIX(0x0073b170));
    rb_define_global_const("APR_FINFO_DIRENT",  INT2FIX(0x02000000));
}

VALUE m_init(VALUE self, VALUE path, VALUE pool, VALUE want_flags)
{
    Check_Type(path, T_STRING);
    Check_Type(pool, T_DATA);
    Check_Type(want_flags, T_FIXNUM);

    // Ensure that the pool argument is indeed a pool class
    ruby::require_class(pool, apr_pool_class());

    // Assign the pool instance to keep a reference count on it.
    rb_ivar_set(self, rb_intern("pool"), pool);

    // Get the pool handle
    apr_pool_t* p;
    Data_Get_Struct(pool, apr_pool_t, p);

modruby:
    apr::file_info f =
        modruby::apr::stat(StringValuePtr(path), p, NUM2INT(want_flags));

    // Get the wrapper
    wrapper* w;
    Data_Get_Struct(self, wrapper, w);

    // Create a new file info
    w->finfo = new modruby::apr::file_info(f);

    return self;
}

VALUE make_apr_file_info(VALUE path, VALUE pool, VALUE want_flags)
{
    wrapper* w = (wrapper*)malloc(sizeof(wrapper));
    VALUE obj = Data_Wrap_Struct(cls, NULL, deallocator, w);

    return m_init(obj, path, pool, want_flags);
}

VALUE make_apr_finfo_from_struct( const apr_finfo_t& x, const char* name,
                                  const char* fname)
{
    wrapper* w = (wrapper*)malloc(sizeof(wrapper));
    w->finfo = new modruby::apr::file_info(x);
    VALUE obj = Data_Wrap_Struct(cls, NULL, deallocator, w);

    return obj;
}

VALUE file_apr_info_class()
{
    return cls;
}

modruby::apr::file_info* get_object(VALUE self)
{
    wrapper* w;

    Data_Get_Struct(self, wrapper, w);

    return w->finfo;
}


VALUE m_atime(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    if (f->valid(APR_FINFO_ATIME) == true)
    {
        // Convert microseconds to seconds
        return INT2NUM(f->atime() / 1000000);
    }

    return Qnil;
}

VALUE m_csize(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    return INT2NUM(f->csize());
}

VALUE m_ctime(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    if (f->valid(APR_FINFO_CTIME) == true)
    {
        // Convert microseconds to seconds
        return INT2NUM(f->ctime() / 1000000);
    }

    return Qnil;
}

VALUE m_device(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    if (f->valid(APR_FINFO_DEV) == true)
    {
        return INT2FIX(f->device());
    }

    return Qnil;
}

VALUE m_fname(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    if (f->fname() != NULL)
    {
        return rb_str_new2(f->fname());
    }

    return Qnil;
}

VALUE m_group(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    if (f->valid(APR_FINFO_GROUP) == true)
    {
        return INT2FIX(f->gid());
    }

    return Qnil;
}

VALUE m_inode(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    if (f->valid(APR_FINFO_INODE) == true)
    {
        return INT2FIX(f->inode());
    }

    return Qnil;
}

VALUE m_mode(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    apr_fileperms_t perm = f->perms();

    int mode = 0;

    mode |= 0700 & (perm >> 2); /* User  is off-by-2 bits */
    mode |= 0070 & (perm >> 1); /* Group is off-by-1 bit */
    mode |= 0007 & (perm);      /* World maps 1 for 1 */

    return INT2FIX(mode);
}

VALUE m_md5(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    string result = f->md5();

    if (result.length() == 0)
    {
        return rb_str_new2("");
    }

    return rb_str_new2(result.c_str());
}

VALUE m_mtime(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    if (f->valid(APR_FINFO_MTIME) == true)
    {
        // Convert microseconds to seconds
        return INT2NUM(f->mtime() / 1000000);
    }

    return Qnil;
}

VALUE m_name(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    if (f->valid(APR_FINFO_NAME) == true)
    {
        if (f->name() != NULL)
        {
            return rb_str_new2(f->name());
        }
    }

    return Qnil;
}

VALUE m_nlink(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    if (f->valid(APR_FINFO_NLINK) == true)
    {
        return INT2FIX(f->nlink());
    }

    return Qnil;
}


VALUE m_protection(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    if (f->valid(APR_FINFO_UPROT) == true)
    {
        return INT2FIX(f->perms());
    }

    return Qnil;
}

VALUE m_sha1(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    string result = f->sha1();

    if (result.length() == 0)
    {
        return rb_str_new2("");
    }

    return rb_str_new2(result.c_str());
}

VALUE m_size(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    if (f->valid(APR_FINFO_SIZE) == true)
    {
        return INT2NUM(f->size());
    }

    return Qnil;
}

VALUE m_type(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    if (f->valid(APR_FINFO_TYPE) == true)
    {
        return INT2FIX(f->type());
    }

    return Qnil;
}


VALUE m_user(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    if (f->valid(APR_FINFO_USER) == true)
    {
        return INT2FIX(f->uid());
    }

    return Qnil;
}

VALUE m_valid(VALUE self)
{
    modruby::apr::file_info* f = get_object(self);

    return f->valid() == true ? Qtrue : Qfalse;
}
