#include "library.h"
#include "object.h"

using namespace ruby;

Object::Object(const char* name)
{
    self = ruby::create_object(name);
    ruby::register_object(self);

    _class_name = name;
}

Object::~Object()
{
    ruby::free_object(self);
}

const char* Object::class_name()
{
    return _class_name.c_str();
}

VALUE Object::method(const char* name, int n, ...)
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
    arg.recv = self;
    arg.id   = rb_intern(name);
    arg.n    = n;
    arg.argv = argv;
    
    int error = 0;
    VALUE result = rb_protect(ruby::method_wrap, reinterpret_cast<VALUE>(&arg), &error);

    if(error)
    {
        std::string msg;
        msg = "ruby::Object::method() invoking " 
              + _class_name + (std::string)"::" 
              + name + (std::string)"()" ;
        
        Exception e(msg.c_str());
        e.backtrace();

        throw e;
    }

    return result;
}
