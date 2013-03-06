#ifndef MODRUBY_APACHE_REQUEST_DECL
#define MODRUBY_APACHE_REQUEST_DECL

#include <apache2/httpd.h>
#include <apache2/http_request.h>
#include <apache2/http_config.h>
#include <apache2/http_core.h>
#include <apache2/http_protocol.h>
#include <apache2/util_script.h>

#include <QString>

#include "apr_table.h"
#include "server.h"

typedef bool (*read_content)(const void* data, modruby::i32 len, void* ud);

namespace apache
{

class Request
{
  protected:

    request_rec* _req;
    Server* _server;
    mutable apr_table_t* _form_data;
    mutable apr_table_t* _queries;
    mutable QString _error;
    mutable QString _content;
    mutable bool has_read_content;
    mutable bool _has_setup_client_read;
    mutable apr_table_t* _module_config;

  public:

    static modruby::i32 max_content_length;

  public:

    Request(request_rec *r);

    ~Request();
    
    /** \return The pool associated with the request **/
    inline apr_pool_t* pool() const { return _req->pool; }

    /** \return The connection associated with the request **/
    inline conn_rec* connection() const { return _req->connection; }
    
    /** \return The server associated with the request **/
    inline server_rec* server() const { return _req->server; }

    /** \return Pointer to the redirected request if this is an external
     * redirect **/
    inline request_rec* next() const { return _req->next; }

    /** \return Pointer to the previous request if this is an internal
     * redirect **/
    inline request_rec* prev() const { return _req->prev; }

    /** \return Pointer to the main request if this is a sub-request **/
    inline request_rec* main() const { return _req->main; }

    /** \return the first line of actual http request content **/
    inline const char* the_request() const { return _req->the_request; }

    /** \return HTTP/0.9, "simple" request (e.g. GET /foo w/no headers) **/
    inline int assbackwards() const { return _req->assbackwards; }

    /** \return The last error string **/
    inline const char* error() const { return _error.toAscii(); }

    /** \return A proxy request (calculated during
     * post_read_request/translate_name) possible values PROXYREQ_NONE,
     * PROXYREQ_PROXY, PROXYREQ_REVERSE, PROXYREQ_RESPONSE **/
    inline int proxyreq() const { return _req->proxyreq; }
    
    /** \return HEAD request, as opposed to GET **/
    inline int header_only() const { return _req->header_only; }

    /** \return Sets up CGI environmental variables in the subprocess_env
     *   table */
    bool setup_cgi() const;

    /** \return Returns raw content (in request) **/
    const QString& content(read_content fn = NULL, void* user_data = NULL) const;

    /** \return Returns raw content (in request) length **/
    modruby::i32 content_length() const { return _content.length(); }

    /** Sets things up in Apache to read content from client. Only needs to be
     * called once per request. 
     */
    bool setup_client_read() const;

    /** \return Returns a single line of content **/
    QString read_line() const;

    /** Fills buffer buff with len bytes of data. **/
    /** \return The number of bytes of data read into the buffer **/
    modruby::i32 read(char** buff, modruby::i32 len) const;

    /** \return Whether the request has form data **/
    bool has_form_data() const;

    /** \return Returns decoded form data in APR table **/
    apr_table_t* form_data() const;

    /** \return Protocol string, as given to us, or HTTP/0.9 **/
    inline const char* protocol() const { return _req->protocol; }

    /** \return Protocol version number of protocol; 1.1 = 1001 **/
    inline int proto_num() const { return _req->proto_num; }

    /** \return Host, as set by full URI or Host: **/
    inline const char* hostname() const { return _req->hostname; }

    /** \return Time when the request started **/
    inline apr_time_t request_time() const { return _req->request_time; }

    /** \return Status line **/
    inline const char* status_line() const { return _req->status_line; }

    /** \return Status line **/
    inline int status() const { return _req->status; }

    /** \return Status line **/
    inline int set_status(int s) const { _req->status = s; }

    /** \return Request method (eg. GET, HEAD, POST, etc.) **/
    inline const char* method() const { return _req->method; }

    /** \return M_GET, M_POST, etc. **/
    inline int method_number() const { return _req->method_number; }

    /** 
     * \brief 'allowed' is a bitvector of the allowed methods.
     *
     * A handler must ensure that the request method is one that it is capable of handling. 
     * Generally modules should DECLINE any request methods they do not handle. 
     * Prior to aborting the handler like this the handler should set r->allowed 
     * to the list of methods that it is willing to handle. 
     * This bitvector is used to construct the "Allow:" header required for OPTIONS requests, 
     * and HTTP_METHOD_NOT_ALLOWED and HTTP_NOT_IMPLEMENTED status codes.
     *
     * Since the default_handler deals with OPTIONS, all modules can usually decline to deal with OPTIONS. 
     * TRACE is always allowed, modules don't need to set it explicitly.
     *
     * Since the default_handler will always handle a GET, a module which does *not* implement GET 
     * should probably return HTTP_METHOD_NOT_ALLOWED. Unfortunately this means that a Script GET 
     * handler can't be installed by mod_actions.
     *
     * \return A bit vector of allowed methods
     **/
    inline apr_int64_t allowed() const { return _req->allowed; }

    /** Sets allowed methods */
    inline void allowed(apr_int64_t x) { _req->allowed = x; }

    /** \return Array of extension methods **/
    inline const apr_array_header_t *allowed_xmethods() const 
    {
        return _req->allowed_xmethods;
    }

    /** \return List of allowed methods **/
    inline const ap_method_list_t* allowed_methods() const 
    {
        return _req->allowed_methods;
    }

    /** \return byte count in stream is for body **/
    inline apr_off_t sent_bodyct() const
    {
        return _req->sent_bodyct;
    }

    /** \return byte count of the body **/
    inline apr_off_t bytes_sent() const
    {
        return _req->bytes_sent;
    }

    /** \return Last modified time of the requested resource **/
    inline apr_time_t mtime() const { return _req->mtime; } 

    /** \return sending chunked transfer-coding **/
    inline int chunked() const { return _req->chunked; }

    /** \return The HTTP range header value **/
    inline const char* range() const { return _req->range; }

    /** \return The so-called 'real' content length **/
    inline apr_off_t clength() const { return _req->clength; }

    /** \return Remaining bytes to be read from the request **/
    inline apr_off_t remaining() const { return _req->remaining; }

    /** \return Number of bytes already read from the request **/
    apr_off_t read_length() const { return _req->read_length; }

    /** \return Method for reading the request body (e.g. REQUEST_CHUNKED_ERROR) **/
    inline int read_body() const { return _req->read_body; }

    /** \return Reading chunked encoding **/
    inline int read_chunked() const { return _req->read_chunked; }
    
    /** \return MIME headers from the request **/
    inline modruby::apr::table headers_in() const { return _req->headers_in; }

    /** \return MIME headers from the response **/
    inline modruby::apr::table headers_out() const { return _req->headers_out; }

    /** \return MIME headers from the response printed even on error **/
    inline modruby::apr::table err_headers_out()  const { return _req->err_headers_out; }
    
    /** \return An array of environment variables passed to the subprocess **/
    inline modruby::apr::table subprocess_env() const { return _req->subprocess_env; }
    
    /** \return Notes to pass from one module to another **/
    inline modruby::apr::table notes() const { return _req->notes; }
    
    /** \return The content type of the request **/
    const char* content_type(char* type = NULL) const 
    { 
        return type ? ( _req->content_type = 
                        apr_pstrdup(_req->pool, type)) : 
                        _req->content_type;
    }

    /** \return the name of the handler assigned this request **/
    inline const char* handler() const { return _req->handler; }

    /** \return How to encode the data **/
    inline const char* content_encoding() const
    {
        return _req->content_encoding;
    }

    /** \return Whether the request body will be discarded **/
    inline int discard_request_body() const
    { 
        return ap_discard_request_body(_req); 
    }
    
    /** \return Send blocking data to the client **/
    inline int get_client_block(char* buf, apr_size_t bufsiz) const
    {
        return ap_get_client_block(_req, buf, bufsiz);
    }
        
    /** \return Array of strings representing content languages **/
    inline const apr_array_header_t *content_languages() const
    { 
        return _req->content_languages;
    }
   
    /** \return variant list validator (if negotiated) **/
    inline const char* vlist_validator() const
    {
        return _req->vlist_validator;
    }

    /** \return If an authentication check was made, this gets set to the user name. */
    inline const char* user() const { return _req->user; }

    /** \return If an authentication check was made, this gets set to the auth type. */
    inline const char* ap_auth_type() const { return _req->ap_auth_type; }

    /** \return The URI without any parsing performed */
    inline const char* unparsed_uri() const { return _req->unparsed_uri; }

    /** \return Just the path portion of the URI */
    inline const char* uri() const { return _req->uri; }

    /** \return The filename on disk corresponding to this response */
    inline const char* filename() const { return _req->filename; }

    /** \return The PATH_INFO extracted from this request */
    inline const char* path_info() const { return _req->path_info; }

    /** \return The QUERY_ARGS extracted from this request */
    inline const char* args() const { return _req->args; }
    
    /** \return The QUERY_ARGS extracted from this request */
    apr_table_t* queries() const;

    /** \return true if this response can not be cached */
    int no_cache() const { return _req->no_cache; }

    /** \return true if there is no local copy of this response */
    inline int no_local_copy() const { return _req->no_local_copy; }
    
    /** \return finfo.protection (st_mode) set to zero if no such file */
    inline apr_finfo_t finfo() const { return _req->finfo; }

    /** \return a struct containing the components of URI */
    inline apr_uri_t parsed_uri() const { return _req->parsed_uri; }
    
    /** 
     * \brief pass in a module and this method will returrn you its per-dir config 
     * \param m a pointer to the static module definition
     * \return a void pointer to the config structure that was set for module m
     **/
    void* get_dir_config(module *m) const;

    /** 
     * \brief pass in a module and this method will returrn you its per-server config 
     * \param m a pointer to the static module definition
     * \return a void pointer to the config structure that was set for module m
     **/
    void* get_server_config(module *m) const;
    
    /**
     * \brief gives you access to the encoded password sent by the client
     * \param sent_pw a reference to a string pointer into the request where the password is
     * \return 0 (OK) if it set the 'pw' argument (and assured
     *         a correct value in r->user); otherwise it returns 
     *         an error code, either HTTP_INTERNAL_SERVER_ERROR if things are 
     *         really confused, HTTP_UNAUTHORIZED if no authentication at all 
     *         seemed to be in use, or DECLINED if there was authentication but 
     *         it wasn't Basic (in which case, the caller should presumably 
     *         decline as well).
     **/
    int get_basic_auth_pw(const char** sent_pw) const;

    /**
     * \brief this method writes a character for this request
     * \param c the character to write
     * \return the number of bytes sent
     **/
    int rputc(int c) const;

    /**
     * \brief this method writes a string for this request
     * \param c the character to write
     * \return the number of bytes sent
     **/
    int rputs(const char* str) const;

    /**
     * \brief writes a buffer for this request
     * \param buf the content to write
     * \param nbyte the number of bytes to send from buf
     * \return the number of bytes sent
     **/
    int rwrite(const void *buf, int nbyte) const;

    /**
     * \brief a printf style way to write data to the request
     * \param fmt The format string
     * \param ... Thee arguments used to fill the format string
     * \return the number of bytes sent
     **/
    int rprintf(const char* fmt, ...) const;

    /**
     * \brief Flush all the data for this request to the client
     * \return number of bytes sent
     **/
    int rflush() const;

    int send_file(const char* path);
   
    /**
     * \brief the allow options tell you what options are set for this request:
     * indexes, includes, sym links, execcgi
     * \return a bitmap of the options set
     **/
    inline int allow_options() const { return ap_allow_options(_req); }

    /** \return bitmask of the allowoverrides for this request **/
    inline int allow_overrides() const { return ap_allow_overrides(_req); }

    /** \return default type from the configuration, or text/plain if not set **/
    inline const char* default_type() const { return ap_default_type(_req); } 

    /**
     * \brief WARNING: This is in to be backward compatible, but is not always 
     * acurate (e.g. if mod_userdir is enabled).  This should not be used
     * \return the document root from the configuration (not necessarily the active one for this request)
     **/
    inline const char* document_root() const { return ap_document_root(_req); }

    /** \return Lookup the login name of the remote user.  Undefined if it cannot be determined **/
    inline const char* get_remote_logname() const { return ap_get_remote_logname(_req); }

    /** \return the server name from the request **/
    inline const char* get_server_name() const { return ap_get_server_name(_req); }

    /**
     * \brief Install a custom response handler for a given status)
     * \param status the status to hook into
     * \param str the custom response - it can be a static string, the full path to a file, or a URL
     **/
    inline void custom_response(int status, const char* str) const
    {
        ap_custom_response(_req, status, (char* )str);
    }
    
    /**
     * \brief run an internal redirect to another URI in this server
     * \param new_uri the new uri to fire
     **/
    inline void internal_redirect(const char* new_uri) const
    {
        ap_internal_redirect(new_uri, _req);
    }
    
    /**
     * \brief This function is designed for things like actions or CGI scripts, when
     * using AddHandler, and you want to preserve the content type across
     * an internal redirect.
     * \param new_uri the URI to replace the current request with
     **/
    inline void internal_redirect_handler(const char* new_uri) const
    {
        ap_internal_redirect(new_uri, _req);
    }
    
    inline int some_auth_required() const
    {
        return ap_some_auth_required(_req);
    }
    
    inline int is_initial_req() const
    {
        return ap_is_initial_req(_req);
    }
    
    inline void update_mtime(apr_time_t dependency_mtime) const
    {
        ap_update_mtime(_req, dependency_mtime);
    }
    
    void allow_methods(int reset, ...) const;
    
    inline void get_mime_headers() const { ap_get_mime_headers(_req); }
    inline void get_mime_headers_core( apr_bucket_brigade *bb) const
    {
        ap_get_mime_headers_core(_req, bb);
    }

    inline void finalize_request_protocol() const
    {
        ap_finalize_request_protocol(_req);
    }

    inline void send_error_response(int recursive_error) const
    {
        ap_send_error_response(_req, recursive_error);
    }

    inline void set_content_length(apr_off_t length) const
    {
        ap_set_content_length(_req, length);
    }

    inline int set_keepalive() const
    {
        return ap_set_keepalive(_req);
    }

    inline apr_time_t rationalize_mtime(apr_time_t mtime) const
    {
        return ap_rationalize_mtime(_req, mtime);
    }

    inline const char* make_content_type(const char* type) const
    {
        return ap_make_content_type(_req, type);
    }

    inline char* make_etag(int force_weak) const
    {
        return ap_make_etag(_req,  force_weak);
    }

    inline void set_etag() const { ap_set_etag(_req); }

    inline void set_last_modified() const {ap_set_last_modified(_req); }

    inline int meets_conditions() const
    {
        return ap_meets_conditions(_req);
    }

    inline void set_content_type(const char* ct) const
    {
        ap_set_content_type(_req, ct);
    }

    inline int setup_client_block(int read_policy) const
    {
        return ap_setup_client_block(_req, read_policy);
    }

    inline int should_client_block() const
    {
        return ap_should_client_block(_req);
    }

    inline void note_auth_failure() const
    {
        ap_note_auth_failure(_req);
    }

    inline void note_basic_auth_failure() const
    {
        ap_note_basic_auth_failure(_req);
    }

    inline void note_digest_auth_failure() const
    {
        ap_note_digest_auth_failure(_req);
    }

    inline void parse_uri(const char* uri) const
    {
        ap_parse_uri(_req, uri);
    }
    
    void dump() const;
    QString repr() const;
    QString table_string(const modruby::apr::table& pTable) const;
    void dump(const modruby::apr::table& pTable) const;
    request_rec *get_request_rec() { return _req; }
};

} // end namespace apache

#endif /* CP_REQUEST_H */
