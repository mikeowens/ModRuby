#ifndef MODRUBY_RUBY_APR_FILE_INFO_DECL
#define MODRUBY_RUBY_APR_FILE_INFO_DECL

extern "C" {

void init_apr_file_info(VALUE module);
VALUE apr_file_info_class();
VALUE make_apr_file_info(VALUE path, VALUE pool, VALUE want_flags);
VALUE make_apr_finfo_from_struct( const apr_finfo_t& x, const char* name,
                                  const char* fname );

} // extern "C"

#endif
