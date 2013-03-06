#ifndef MODRUBY_RUBY_APACHE_REQUEST_DECL
#define MODRUBY_RUBY_APACHE_REQUEST_DECL

#include <apache2/http_request.h>

#include "request.h"

extern "C" {
void init_request(VALUE module);
VALUE request_class();
VALUE make_request(request_rec* r);
}

#endif
