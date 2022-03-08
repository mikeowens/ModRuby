#ifndef MODRUBY_RUBY_APACHE_PROCESS_DECL
#define MODRUBY_RUBY_APACHE_PROCESS_DECL

extern "C" {

void init_process(VALUE module);
VALUE process_class();
VALUE make_process(const server_rec* s);

} // extern "C"

#endif
