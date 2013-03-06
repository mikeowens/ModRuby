#ifndef MODRUBY_RUBY_BASE_OBJECT
#define MODRUBY_RUBY_BASE_OBJECT

#include <ruby.h>
#include <QString>

namespace ruby
{

/** This class implements a C handle for a Ruby Object. That is, the object
 ** instantiated is defined within a Ruby script or library; it exists within the
 ** Ruby language. This class creates and holds an instance of the class given by
 ** the name argument in the constructor. 
 **

 ** It holds this instance in memory, keeping it safe from the Ruby garbage
 ** collector (GC), by registering it in the underlying ruby::register_object
 ** API. This keeps the object in a Ruby array, which in turn keeps an active
 ** reference to that object, which keeps the GC from reaping it.
 */

class Object
{
  private:

    Object(){}

  protected:

    VALUE self;
    std::string _class_name;

  public:

    Object(const char* name);
    virtual ~Object();

    VALUE method(const char* name, int n=0, ...);

    const char* class_name();
};

} // end namespace ruby

#endif
