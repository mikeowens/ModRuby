#ifndef MODRUBY_RUBY_APR_TABLE_DECL
#define MODRUBY_RUBY_APR_TABLE_DECL

#include "apr.h"

extern "C" {

void init_apr_table(VALUE module);
VALUE apr_table_class();
VALUE make_apr_table(apr_table_t* ptr);

} // extern "C"

#endif
