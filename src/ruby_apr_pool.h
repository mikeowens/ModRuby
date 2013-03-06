#ifndef MODRUBY_RUBY_APR_POOL_DECL
#define MODRUBY_RUBY_APR_POOL_DECL

extern "C" {
void init_apr_pool(VALUE module);
VALUE apr_pool_class();
VALUE make_apr_pool();
}

#endif
