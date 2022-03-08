#ifndef MODRUBY_RUBY_APACHE_SERVER_DECL
#define MODRUBY_RUBY_APACHE_SERVER_DECL

extern "C" {

void init_server(VALUE module);
VALUE server_class();
VALUE make_server(const server_rec* ptr);

} // extern "C"

#endif
