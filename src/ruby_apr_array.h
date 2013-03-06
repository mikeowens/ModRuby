#ifndef MODRUBY_RUBY_APR_ARRAY_DECL
#define MODRUBY_RUBY_APR_ARRAY_DECL

#include "apr.h"

extern "C" {
void init_apr_array(VALUE module);
VALUE apr_array_class();
VALUE make_apr_array(const apr_array_header_t* h);
}

#endif
