#ifndef MODRUBY_RUBY_LIBRARY_H
#define MODRUBY_RUBY_LIBRARY_H

#include <stdexcept>
#include <ruby.h>
#include <QString>

namespace ruby {

//------------------------------------------------------------------------------
// Embedded Interpreter
//------------------------------------------------------------------------------

// startup() must be invoked before using this library.
// shutdown() must be invoked after using this library.

void startup(const char* script_name);
void shutdown(int exit_code=0);

//------------------------------------------------------------------------------
// Memory Management
//------------------------------------------------------------------------------

// Ruby's garbage collector must know about our non-exported ruby instances in
// C++. Otherwise we can suddenly get killed without warning. In the "Pragmatic
// Programmers Guide" section "Sharing Data Between Ruby and C" they recommend C
// objects being registered in a global array.

class Objects
{
  private:

    static VALUE objects;

    Objects();
    ~Objects();

    static Objects* singleton;

  public:

    static Objects* instance();
    static void register_object(VALUE object);
    static void free_object(VALUE object);
    static void free_all();
};

void register_object(VALUE object);
void free_object(VALUE object);
void free_all();


//------------------------------------------------------------------------------
// Exceptions
//------------------------------------------------------------------------------

/* Translate a ruby exception into an c++ exception. 
** 
** Example:
**
** void Test()
** {
**     int error = 0;
**       rb_protect(WrapTest, reinterpret_cast<VALUE>(this), &error);
**
**       if(error)
**     {
**           throw ruby::Exception("error loading test.rb");
**     }
** }
*/

class Exception : public std::exception
{
    std::string _msg;
    std::string _backtrace;
    std::string _type;

  public:

    Exception(const char* msg=NULL);
    Exception(const Exception& e);

    virtual ~Exception() throw();

    const Exception& operator=(const Exception& e);

    void backtrace() throw();

    const char* type() const;
    virtual const char* what() const throw();
    const char* stackdump() const;
};

// Wrap rb_funcall
struct Arguments
{
    VALUE recv;
    ID id;
    int n;
    VALUE *argv;

    Arguments( VALUE recv, 
               ID id, 
               int n, 
               VALUE *argv ) 
        : recv(recv), id(id), n(n), argv(argv)
    {

    }
};

/* Call a ruby function in a safe way. Translate ruby errors into c++
** exceptions. Instead of calling rb_funcall(), do this:
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
VALUE method_wrap(VALUE arg);
VALUE method(VALUE recv, ID id, int n, ...);
VALUE vm_method(VALUE recv, ID id, int n, va_list ar);

// Eval ruby string in a safe way
void eval(const char* code, const char* filename=NULL, int sl=0, VALUE binding=Qnil);

// Require a ruby-file in a safe way
void require(const char* filename);

// Load a ruby-file in a safe way. If anonymous == 1, the loaded script will be
// executed under an anonymous module, protecting the calling program's global
// namespace.
void load(const char* filename, int anonymous=0);

// Ensure that x is of class cls
void require_class(VALUE x, VALUE cls);

// Create a new instance in a safe way
VALUE create_object(const char* class_name);

} // end namespace ruby
 

/* Translate from c++ 2 ruby exceptions

adapted from 
http://groups.google.com/groups?hl=en&lr=&ie=UTF-8&selm=20020408192019.E15413%40atdesk.com&rnum=10

RUBY_CATCH
Cant raise the exception from catch block, because 
the C++ exception wont get properly destroyed.
Thus we must delay it til after the catch block!

#define RUBY_TRY \
    extern VALUE ruby_errinfo; \
    ruby_errinfo = Qnil; \
    try

#define RUBY_CATCH \
    catch(const std::exception &e) { \
        buffer str;
        o << "c++error: " << e.what(); \
        ruby_errinfo = rb_exc_new2(rb_eRuntimeError, str.c_string()); \
    } catch(...) { \
        ruby_errinfo = rb_exc_new2(rb_eRuntimeError, "c++error: Unknown error"); \
    } \
    if(!NIL_P(ruby_errinfo)) { \
        rb_exc_raise(ruby_errinfo); \
    }
*/

#endif // EMBEDRUBY_LIBRARY_H
