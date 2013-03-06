#ifndef MODRUBY_RUBY_RHTML_DECL
#define MODRUBY_RUBY_RHTML_DECL

extern "C" {

void init_rhtml();
VALUE rhtml_eval(int args, VALUE* argv, VALUE self);
VALUE rhtml_compile(int args, VALUE* argv, VALUE self);

}

#endif
