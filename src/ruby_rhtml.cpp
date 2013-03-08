#include <QTextStream>
#include <QFileInfo>

#include "library.h"
#include "object.h"
#include "rhtml.h"
#include "ruby_rhtml.h"

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
    if(args == 0)
    {
        rb_raise( rb_eRuntimeError, 
                  "At lease one argument is required." );
    }

    VALUE text    = argv[0];
    VALUE binding = Qnil;

    Check_Type(text, T_STRING);

    if(args == 2)
    {
        binding = argv[1];
    }

    ruby::RhtmlParser parser;

    parser.compile_text(StringValuePtr(text));

    return rb_str_new(parser.text.data(), parser.text.size());
}

VALUE rhtml_compile(int args, VALUE* argv, VALUE self)
{
    if(args == 0)
    {
        rb_raise( rb_eRuntimeError, 
                  "At lease one argument is required." );
    }

    Check_Type(argv[0], T_STRING);

    VALUE binding = Qnil;

    if(args == 2)
    {
        binding = argv[1];
    }

    const char* filename = StringValuePtr(argv[0]);

    QFileInfo info(filename);

    if((info.exists() == false) || (info.isReadable() == false))
    {
        QString msg;
        QTextStream strm(&msg);
        strm << "File does not exist: " << info.absoluteFilePath();
        
        rb_raise(rb_eIOError, msg.toAscii().data());
    }
    
    ruby::RhtmlParser parser;

    parser.compile_file(info.absoluteFilePath().toAscii().data());
    
    return rb_str_new(parser.text.data(), parser.text.size());
}
