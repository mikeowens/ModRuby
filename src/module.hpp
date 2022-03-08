#ifndef MODRUBY_APACHE_MODULE_HPP_DECL
#define MODRUBY_APACHE_MODULE_HPP_DECL

#include "ruby.hpp"
#include "request.h"
#include "server.h"
#include "module.h"
#include "ruby_request.h"
#include "config.h"

// This is for C++ functions that can't be included in module.h (which is
// strictly C).

namespace modruby
{

class Handler
{
    std::string _module;
    std::string _klass;
    std::string _method;
    std::string _id;

  public:

    ruby::Object* object;

    Handler( const char* module = "", const char* klass = "",
             const char* method = "");

    virtual ~Handler() {}

    // Unique key/name/id of this handler
    const char* id()
    {
        if(_id.size() == 0)
        {
            _id = _module + "::" + _klass  + "::" + _method;
        }

        return _id.c_str();
    }

    const char* module() const
    {
        return _module.size() == 0 ? NULL : _module.c_str();
    }

    const char* klass() const
    {
        return _klass.size() == 0 ? NULL : _klass.c_str();
    }

    const char* method() const
    {
        return _method.size() == 0 ? NULL : _method.c_str();
    }
};

} // end namespace modruby

modruby::Handler ruby_request_get_handler(request_rec* r);
modruby::Handler ruby_request_get_access_handler(request_rec* r);

#endif
