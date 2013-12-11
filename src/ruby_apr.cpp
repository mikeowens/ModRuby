#include <ruby.h>

#include <string>

#include "apr.h"
#include "apr_table.h"
#include "ruby_apr.h"
#include "ruby_apr_file_info.h"
#include "ruby_apr_pool.h"

using std::string;

typedef VALUE (*fn)(...);

#define CLASS_NAME "APR"

extern "C" {

static VALUE m_chmod(VALUE cls, VALUE path, VALUE perms);
static VALUE m_cp(VALUE cls, VALUE source_path, VALUE dest_path);
static VALUE m_cwd(VALUE cls);
static VALUE m_mkdir(VALUE cls, VALUE dir);
static VALUE m_mv(VALUE cls, VALUE source_path, VALUE dest_path);
static VALUE m_rm(VALUE cls, VALUE dir);
static VALUE m_rmdir(VALUE cls, VALUE dir);
static VALUE m_stat(int args, VALUE* argv, VALUE self);
static VALUE m_filepath_is_relative(VALUE self, VALUE path);
static VALUE m_filepath_merge(VALUE self, VALUE root_path, VALUE add_path);
static VALUE m_filepath_root(VALUE self, VALUE path);
static VALUE m_init(VALUE self, VALUE file);

}

using namespace modruby;

static void deallocator(void* x)
{

}

static VALUE allocator(VALUE cls)
{
    return Qnil;
}

static VALUE cls;

void init_apr(VALUE module)
{
    //cls = rb_define_class_under(module, CLASS_NAME, rb_cObject);
    //rb_define_alloc_func(cls, allocator);

    rb_define_module_function(module, "chmod", (fn)m_chmod, 2);
    rb_define_module_function(module, "cp",    (fn)m_cp, 2);
    rb_define_module_function(module, "cwd",   (fn)m_cwd, 0);
    rb_define_module_function(module, "mkdir", (fn)m_mkdir, 1);
    rb_define_module_function(module, "mv",    (fn)m_mv, 2);
    rb_define_module_function(module, "rm",    (fn)m_rm, 1);
    rb_define_module_function(module, "rmdir", (fn)m_rmdir, 1);
    rb_define_module_function(module, "stat",  (fn)m_stat, -1);
    rb_define_module_function(module, "filepathIsRelative", (fn)m_filepath_is_relative, 1);
    rb_define_module_function(module, "filepathMerge", (fn)m_filepath_merge, 2);
    rb_define_module_function(module, "filepathRoot", (fn)m_filepath_root, 1);

    // APR error constants
    rb_define_global_const("APR_SUCCESS",      INT2FIX(APR_SUCCESS));
    rb_define_global_const("APR_ENOSTAT",      INT2FIX(APR_ENOSTAT));
    rb_define_global_const("APR_ENOPOOL",      INT2FIX(APR_ENOPOOL));
    rb_define_global_const("APR_EBADDATE",     INT2FIX(APR_EBADDATE));
    rb_define_global_const("APR_EINVALSOCK",   INT2FIX(APR_EINVALSOCK));
    rb_define_global_const("APR_ENOPROC",      INT2FIX(APR_ENOPROC));
    rb_define_global_const("APR_ENOTIME",      INT2FIX(APR_ENOTIME));
    rb_define_global_const("APR_ENODIR",       INT2FIX(APR_ENODIR));
    rb_define_global_const("APR_ENOLOCK",      INT2FIX(APR_ENOLOCK));
    rb_define_global_const("APR_ENOPOLL",      INT2FIX(APR_ENOPOLL));
    rb_define_global_const("APR_ENOSOCKET",    INT2FIX(APR_ENOSOCKET));
    rb_define_global_const("APR_ENOTHREAD",    INT2FIX(APR_ENOTHREAD));
    rb_define_global_const("APR_ENOTHDKEY",    INT2FIX(APR_ENOTHDKEY));
    rb_define_global_const("APR_ENOSHMAVAIL",  INT2FIX(APR_ENOSHMAVAIL));
    rb_define_global_const("APR_EDSOOPEN",     INT2FIX(APR_EDSOOPEN));
    rb_define_global_const("APR_EGENERAL",     INT2FIX(APR_EGENERAL));  
    rb_define_global_const("APR_EBADIP",       INT2FIX(APR_EBADIP));
    rb_define_global_const("APR_EBADMASK",     INT2FIX(APR_EBADMASK));
    rb_define_global_const("APR_ESYMNOTFOUND", INT2FIX(APR_ESYMNOTFOUND));

    rb_define_global_const("APR_FPROT_USETID",   INT2FIX(APR_FPROT_USETID));
    rb_define_global_const("APR_FPROT_UREAD",    INT2FIX(APR_FPROT_UREAD));
    rb_define_global_const("APR_FPROT_UWRITE",   INT2FIX(APR_FPROT_UWRITE));
    rb_define_global_const("APR_FPROT_UEXECUTE", INT2FIX(APR_FPROT_UEXECUTE));
    rb_define_global_const("APR_FPROT_GSETID",   INT2FIX(APR_FPROT_GSETID));
    rb_define_global_const("APR_FPROT_GREAD",    INT2FIX(APR_FPROT_GREAD));
    rb_define_global_const("APR_FPROT_GWRITE",   INT2FIX(APR_FPROT_GWRITE));
    rb_define_global_const("APR_FPROT_GEXECUTE", INT2FIX(APR_FPROT_GEXECUTE));
    rb_define_global_const("APR_FPROT_WSTICKY",  INT2FIX(APR_FPROT_WSTICKY));
    rb_define_global_const("APR_FPROT_WREAD",    INT2FIX(APR_FPROT_WREAD));
    rb_define_global_const("APR_FPROT_WWRITE",   INT2FIX(APR_FPROT_WWRITE));
    rb_define_global_const("APR_FPROT_WEXECUTE", INT2FIX(APR_FPROT_WEXECUTE));
}

VALUE m_init(VALUE self, VALUE file)
{
    return self;
}

VALUE apr_class()
{
    return cls;
}

VALUE m_chmod(VALUE cls, VALUE path, VALUE perms)
{
    Check_Type(path, T_STRING);
    Check_Type(perms, T_FIXNUM);

    return INT2FIX(modruby::apr::chmod( StringValuePtr(path),
                                         NUM2INT(perms) ));
}

VALUE m_cp(VALUE cls, VALUE source_path, VALUE dest_path)
{
    Check_Type(source_path, T_STRING);
    Check_Type(dest_path, T_STRING);

    return INT2FIX(modruby::apr::cp( StringValuePtr(source_path),
                                      StringValuePtr(dest_path) ) );
}

VALUE m_mkdir(VALUE cls, VALUE dir)
{
    Check_Type(dir, T_STRING);

    return INT2FIX(modruby::apr::mkdir(StringValuePtr(dir)));
}

VALUE m_rm(VALUE cls, VALUE dir)
{
    Check_Type(dir, T_STRING);

    return INT2FIX(modruby::apr::rm(StringValuePtr(dir)));
}

VALUE m_mv(VALUE cls, VALUE source_path, VALUE dest_path)
{
    Check_Type(source_path, T_STRING);
    Check_Type(dest_path, T_STRING);

    return INT2FIX(modruby::apr::mv( StringValuePtr(source_path),
                                      StringValuePtr(dest_path) ) );
}

VALUE m_rmdir(VALUE cls, VALUE dir)
{
    Check_Type(dir, T_STRING);

    return INT2FIX(modruby::apr::rmdir(StringValuePtr(dir)));
}

VALUE m_cwd(VALUE self)
{
    return rb_str_new2(modruby::apr::cwd().c_str());
}

VALUE m_filepath_is_relative(VALUE self, VALUE path)
{
    Check_Type(path, T_STRING);

    bool ret = modruby::apr::path_is_relative(StringValuePtr(path));

    return ret == true ? Qtrue : Qfalse;
}

VALUE m_filepath_merge(VALUE self, VALUE root_path, VALUE add_path)
{
    Check_Type(root_path, T_STRING);
    Check_Type(add_path, T_STRING);

    string path = modruby::apr::path_merge( StringValuePtr(root_path),
                                             StringValuePtr(add_path) );
    
    if(path.length() == 0)
    {
        return Qnil;
    }

    return rb_str_new2(path.c_str());
}

VALUE m_filepath_root(VALUE self, VALUE path)
{
    Check_Type(path, T_STRING);
    
    string root_path = modruby::apr::path_root(StringValuePtr(path));
    
    if(root_path.length() == 0)
    {
        return Qnil;
    }

    return rb_str_new2(root_path.c_str());
}

VALUE m_stat(int args, VALUE* argv, VALUE self)
{
    if(args == 0)
    {
        rb_raise( rb_eRuntimeError,
                  "At lease one argument is required." );
    }

    Check_Type(argv[0], T_STRING);   

    VALUE want_flags;

    if(args == 2)
    {
        Check_Type(argv[1], T_FIXNUM);
        want_flags = argv[1];
    }
    else
    {
        want_flags = INT2NUM(0);
    }

    return make_apr_file_info(argv[0], make_apr_pool(), want_flags);
}
