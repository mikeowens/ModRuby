#include <unistd.h>
#include <string>
#include <sstream>

#include "ruby.hpp"
#include "rhtml.h"
#include "ruby_rhtml.h"
#include "apr.h"

using std::stringstream;
using namespace modruby;

typedef VALUE (*fn)(...);

void init_rhtml()
{
    VALUE mod   = rb_define_module("ModRuby");
    VALUE rhtml = rb_define_module_under(mod, "RHTML");

    rb_define_module_function(rhtml, "eval", (fn)rhtml_eval, -1);
    rb_define_module_function(rhtml, "compile", (fn)rhtml_compile, -1);
}

VALUE rhtml_eval(int args, VALUE* argv, VALUE self)
{
    if (args == 0)
    {
        rb_raise( rb_eRuntimeError,
                  "At lease one argument is required." );
    }

    VALUE text    = argv[0];
    VALUE binding = Qnil;

    Check_Type(text, T_STRING);

    if (args == 2)
    {
        binding = argv[1];
    }

    ruby::RhtmlParser parser;

    parser.compile_text(StringValuePtr(text));

    return rb_str_new(parser.text.data(), parser.text.size());
}

VALUE rhtml_compile(int args, VALUE* argv, VALUE self)
{
    if (args == 0)
    {
        rb_raise( rb_eRuntimeError,
                  "At lease one argument is required." );
    }

    Check_Type(argv[0], T_STRING);

    VALUE binding = Qnil;

    if (args == 2)
    {
        binding = argv[1];
    }

    const char* filename = StringValuePtr(argv[0]);

    if (access(filename, R_OK) != 0)
    {
        stringstream strm;
        strm << "File does not exist: " << filename;
        rb_raise(rb_eIOError, strm.str().c_str());
    }

    ruby::RhtmlParser parser;
    parser.compile_file(filename);

    return rb_str_new(parser.text.data(), parser.text.size());
}
