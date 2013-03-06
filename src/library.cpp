#include <ruby.h>
#include <ruby/encoding.h>
#include <QTextStream>

#include "library.h"

namespace ruby {

//------------------------------------------------------------------------------
// Memory Management 
//------------------------------------------------------------------------------

Objects* Objects::singleton = 0;
VALUE Objects::objects;

Objects* Objects::instance()
{
    if(singleton == NULL)
    {
        singleton = new Objects();
    }

    return singleton;
}

Objects::Objects()
{
    objects = rb_ary_new();
    rb_gc_register_address(&objects);
}

Objects::~Objects()
{
    rb_gc_unregister_address(&objects);
}

void Objects::register_object(VALUE object)
{
    rb_ary_push(objects, object);
}

void Objects::free_object(VALUE object)
{
    rb_ary_delete(objects, object);
}

void Objects::free_all()
{
    if(singleton == NULL)
    {
        delete singleton;
        singleton = NULL;
    }
}

void register_object(VALUE object)
{
    Objects::instance()->register_object(object);
}

void free_object(VALUE object)
{
    Objects::instance()->free_object(object);
}

void free_all()
{
    Objects::free_all();
}

//------------------------------------------------------------------------------
// VM Routines 
//------------------------------------------------------------------------------

static bool running = false;

void startup(const char* script_name)
{
    if(running)
    {
        return;
    }
    
    int fake_argc = 0;
    char *fake_args[fake_argc];
    char **fake_argv = fake_args;

    ruby_sysinit(&fake_argc, &fake_argv);
    
    // Initialize Ruby itself
    RUBY_INIT_STACK;    
    ruby_init();

    ruby_init_loadpath();

    // To load prelude.rb
    static char* args[] = { "ruby", "/dev/null" };
    ruby_process_options(2, args);

    // Load Ruby encodings, otherwise we'll get all kinds of "Unitialized
    // constanct Encoding::UTF-7", etc. everywhere.
    rb_enc_find_index("encdb");

    VALUE gem;
    gem = rb_define_module("Gem");
    rb_const_set(gem, rb_intern("Enable"), Qtrue);
    rb_require("rubygems");

    ruby_script((char*)script_name);

    running = true;
}

void shutdown(int exit_code)
{
    free_all();
    rb_exit(exit_code);

    if(running == false)
    {
        return;
    }
    
    running = false;
}

//------------------------------------------------------------------------------
// Exceptions
//------------------------------------------------------------------------------

Exception::Exception(const char* msg)
{
    if(msg != NULL)
    {
        _msg = msg;
    }    
}

Exception::Exception(const Exception& e)
{
    *this = e;
}

Exception::~Exception() throw()
{

}

const Exception& Exception::operator=(const Exception& e)
{
    if(this != &e)
    {
        _msg       = e._msg;
        _backtrace = e._backtrace;
        _type      = e._type;
    }

    return *this;
}

const char* Exception::type() const
{
    return _type.c_str();
}

const char* Exception::what() const throw()
{
    return _msg.c_str();
}

const char* Exception::stackdump() const
{
    return _backtrace.c_str();
}

// Convert a ruby exception into C++. 
void Exception::backtrace() throw()
{
    _backtrace.clear();

    /*
    const char* file = ruby_sourcefile;

    if(file == NULL)
    {
        file = "(Ruby VM)";
    }

    msg << "File    : " << file << "\n";
    */

    // Get the glocal exception object
    VALUE exception_instance = rb_gv_get("$!");

    // Get the exception instance class
    VALUE klass = rb_class_path(CLASS_OF(exception_instance));
    _backtrace += "Type    : " + (std::string)RSTRING_PTR(klass) + (std::string)"\n";

    // Store it for later reference
    _type = (std::string)RSTRING_PTR(klass);

    //> Now generate the error message

    // Store exception class name
    _backtrace += RSTRING_PTR(klass);

    // Message: call $!.to_s() to get string representation
    VALUE message = rb_obj_as_string(exception_instance);
    _backtrace += "Message : " + (std::string)RSTRING_PTR(message) + (std::string)"\n";

    // Backtrace:  p (*(RArray*)ary).as.ary
    if(!NIL_P(exception_instance))
    {
        VALUE ary = rb_funcall(exception_instance, rb_intern("backtrace"), 0);

        int c;
        for (c=0; c<RARRAY_LEN(ary); c++)
        {   // gdb: p *((RString*)(*(RArray*)ary).as.ary[0])
            _backtrace += (std::string)"From    : " + 
                          (std::string)RSTRING_PTR(RARRAY_PTR(ary)[c]) +
                          (std::string)"\n";
        }
    }
}

//------------------------------------------------------------------------------
// Function Calls
//------------------------------------------------------------------------------

VALUE method_wrap(VALUE arg)
{
    Arguments &a = *reinterpret_cast<Arguments*>(arg);

    return rb_funcall2(a.recv, a.id, a.n, a.argv);
}

/* Purpose: Call a ruby function in a safe way. Translate ruby errors into c++
** exceptions.
**
**    VALUE Unsafe() {
**        return rb_funcall(
**            self, 
**            rb_intern("test"), 
**            1, 
**            INT2NUM(42)
**        );
**    }
**
**    VALUE Safe() {
**        return ruby::method(
**            self, 
**            rb_intern("test"), 
**            1, 
**            INT2NUM(42)
**        );
**    }
*/

VALUE method(VALUE recv, ID id, int n, ...)
{
    VALUE *argv = 0;

    if (n > 0) 
    {
        argv = ALLOCA_N(VALUE, n);
        va_list ar;
        va_start(ar, n);

        int i;
        for(i=0; i<n ;i++)
        {
            argv[i] = va_arg(ar, VALUE);
        }

        va_end(ar);
    } 

    Arguments arg;
    arg.recv = recv;
    arg.id   = id;
    arg.n    = n;
    arg.argv = argv;

    int error = 0;
    VALUE result = rb_protect(method_wrap, reinterpret_cast<VALUE>(&arg), &error);

    if(error)
    {
        Exception e;
        e.backtrace();
        throw e;
    }

    return result;
}

VALUE vm_method(VALUE recv, ID id, int n, va_list ar)
{
    VALUE *argv = 0;

    if (n > 0) 
    {
        argv = ALLOCA_N(VALUE, n);

        int i;
        for(i=0; i<n ;i++)
        {
            argv[i] = va_arg(ar, VALUE);
        }
    } 

    Arguments arg;
    arg.recv = recv;
    arg.id   = id;
    arg.n    = n;
    arg.argv = argv;    

    int error = 0;
    VALUE result = rb_protect(method_wrap, reinterpret_cast<VALUE>(&arg), &error);

    if(error)
    {
        Exception e;
        e.backtrace();
        throw e;
    }

    return result;
}

//------------------------------------------------------------------------------
// Module and Class Instantiation
//------------------------------------------------------------------------------

void eval(const char* code, const char* filename, int sl, VALUE binding)
{
    int error = 0;

    const char* fn = filename;

    if(fn == NULL)
    {
        fn = "eval";
    }
    
    // We use this rather than ruby::eval() because this allows us
    // to associated a file name with the eval code, giving us more
    // informative backtraces. Otherwise, ruby::eval() would simply
    // list the source of every frame as "(eval)".
    
    method( Qnil, rb_intern("eval"), 4,
            rb_str_new2(code),     // code
            binding,               // binding
            rb_str_new2(fn),       // filename
            INT2NUM(sl) );         // source line
    
    /* old method

    // Set the Ruby source line to 0. This is a Ruby hack. There might be a
    // better way to do this, but we are telling Ruby that we are starting a new
    // file and that the first line of this chunk of code is line 1.

    ruby_sourceline = sl;

    rb_eval_string_protect(code, &error);

    if(error)
    {
        modruby::buffer msg;

        throw Exception();
    }
    */
}

VALUE require_protect(VALUE arg)
{
    const char *filename = reinterpret_cast<const char*>(arg);
    rb_require(filename);

    return Qnil;
}

void require(const char* filename)
{
    int error = 0;
    rb_protect(require_protect, reinterpret_cast<VALUE>(filename), &error);

    if(error)
    {
        QString msg;
        QTextStream strm(&msg);

        strm << "error loading " << filename << ".rb";

        Exception e(msg.toAscii().data());
        e.backtrace();
        throw e;
    }
}

void load(const char* filename, int anonymous)
{
    int error = 0;
    rb_load_protect(rb_str_new2(filename), anonymous, &error);

    if(error)
    {
        Exception e;
        e.backtrace();
        throw e;
    }
}

struct NewArguments
{
    const char* class_name;
    int n;
    VALUE* argv;

    NewArguments( const char* cname, 
                  int n, 
                  VALUE* argv) 
        : class_name(cname), n(n), argv(argv)
    {

    }
};

VALUE create_object_protect(VALUE arg)
{
    NewArguments &a = *reinterpret_cast<NewArguments*>(arg);
    //VALUE class_name = rb_const_get(rb_cObject, rb_intern(a.class_name));
    VALUE class_name = rb_path2class(a.class_name);

    VALUE self = rb_class_new_instance(a.n, a.argv, class_name);
    //VALUE self = rb_funcall2(class_name, rb_intern("new"), a.n, a.argv);

    return self;
}

VALUE create_object(const char* class_name)
{
    NewArguments arg(class_name, 0, 0);

    int error = 0;
    VALUE self = rb_protect(create_object_protect, reinterpret_cast<VALUE>(&arg), &error);

    if(error)
    {
        QString msg;
        QTextStream strm(&msg);
        strm << "Error creating Ruby class '" << class_name << "'";

        Exception e(msg.toAscii().data());
        e.backtrace();
        throw e;
    }

    return self;
}

void require_class(VALUE x, VALUE cls)
{
    if(rb_obj_is_instance_of(x,cls) == Qfalse)
    {
        rb_raise( rb_eRuntimeError, 
                  "wrong argument type %s (expected %s)",
                  rb_obj_classname(x),
                  rb_class2name(cls) );
    }
}

} // end namespace ruby
