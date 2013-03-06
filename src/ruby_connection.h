#ifndef MODRUBY_APACHE_CONNECTION_DECL
#define MODRUBY_APACHE_CONNECTION_DECL

extern "C" {
void init_connection(VALUE module);
VALUE connection_class();
VALUE make_connection(conn_rec* ptr);
}

#endif
