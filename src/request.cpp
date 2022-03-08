#include <string>
#include <sstream>

#include "request.h"
#include "apr.h"
#include "apr_table.h"
#include "connection.h"
#include "util.hpp"

using std::string;
using std::vector;
using std::stringstream;
using namespace apache;
using namespace modruby;

i32 Request::max_content_length = 20 * 1024 * 1024;

Request::Request(request_rec* r)
    : _server(NULL), _form_data(NULL), _queries(NULL), has_read_content(false),
      _has_setup_client_read(false), _module_config(NULL)
{
    _req = r;
}

Request::~Request()
{

}

string format_pair(const char* key, const char* value)
{
    char buf[256];
    i32 len = snprintf(buf, 255, "    %-24s: %s\n", key, value);

    return buf;
}

string format_pair(const char* key, i32 value)
{
    char buf[256];
    i32 len = snprintf(buf, 255, "    %-24s: %i\n", key, value);

    return buf;
}

string Request::repr() const
{
    stringstream out;

    const apache::Connection c = connection();

    out << "Request:\n\n"

        << format_pair("Hostname", this->hostname())
        << format_pair("Document Root", this->document_root())
        << format_pair("Request", this->the_request())
        << format_pair("Method", this->method())
        << format_pair("Status", this->status())
        << format_pair("URI", this->uri())
        << format_pair("Args", this->args())
        << format_pair("Filename", this->filename())
        << format_pair("Path", this->path_info())
        << format_pair("Bytes Sent", this->bytes_sent())
        << format_pair("Content Type", this->content_type())
        << format_pair("Remote Host", c.remote_host())
        << format_pair("Remote IP", c.remote_ip())
        << format_pair("Protocol", this->protocol())
        << format_pair("Proto Number", this->proto_num())
        << format_pair("Status Line", this->status_line())
        << format_pair("Method Number", this->method_number())
        << format_pair("Handler", this->handler())
        << format_pair("User", this->user())
        << format_pair("Local Host", c.local_host())
        << format_pair("Local IP", c.local_ip())
        << format_pair("Assbackwards", this->assbackwards())
        << format_pair("Proxy Req", this->proxyreq())
        << format_pair("Header Only", this->header_only())
        << format_pair("Allowed", this->allowed()) << "\n"

        << "Headers in:\n\n"
        << table_string(this->headers_in()) << "\n"

        << "Environment Variables:\n\n"
        << table_string(this->subprocess_env()) << "\n";

    apr_table_t* params = form_data();

    if (params != NULL)
    {
        apr::table t(params);

        out << "POST Parameters:\n\n";
        out << table_string(t);
    }

    if (this->notes().size() > 0)
    {
        out << "Notes:\n\n"
            << table_string(this->notes());
    }

    if (this->headers_out().size() > 0)
    {
        out << "Headers out:\n\n"
            << table_string(this->headers_out()) << "\n";
    }

    if (this->err_headers_out().size() > 0)
    {
        out << "Error Headers_out:\n\n"
            << table_string(this->err_headers_out()) << "\n";
    }

    return out.str();
}

void Request::dump() const
{
    this->rputs(repr().c_str());
}

string Request::table_string(const modruby::apr::table& t) const
{
    stringstream out;
    char buf[256];

    modruby::apr::table::iterator i(t);

    while (i.next())
    {
        // Format
        i32 len = snprintf(buf, 255, "  %3i %-22s: %s\n", i.index(), i.key(), i.data());

        // Append
        out << buf;
    }

    return out.str();
}

void Request::dump(const modruby::apr::table& pTable) const
{
    rputs(table_string(pTable).c_str());
}

apr_table_t* Request::queries() const
{
    // If we have copied this out before
    if (_queries != NULL)
    {
        // Return the table
        return _queries;
    }

    // Parse query string to table
    if (_req->args != NULL)
    {
        apr::table t(pool());

        parse_query_string(_req->args, t);

        _queries = t.handle;
    }
    else
    {
        apr::table t(pool());
        _queries = t.handle;
    }

    // Get reference to apr_table_t. We will just return this if this function
    // is ever called again.
    return _queries;
}

bool Request::setup_cgi() const
{
    ap_add_cgi_vars(_req);
    ap_add_common_vars(_req);

    return true;
}

bool Request::has_form_data() const
{
    if (strcmp(method(), "POST") != 0)
    {
        return false;
    }

    string x = headers_in().get("Content-Type");

    if (x.find("application/x-www-form-urlencoded") != string::npos)
    {
        return true;
    }

    return false;
}

bool Request::setup_client_read() const
{
    if (_has_setup_client_read == true)
    {
        return true;
    }

    _has_setup_client_read = true;

    if (setup_client_block(REQUEST_CHUNKED_DECHUNK) != OK)
    {
        _error = "Request::form_data(): setup_client_block failed.";

        return false;
    }

    if (should_client_block() == 0)
    {
        _error = "Request::form_data(): should_client_block failed.";

        return false;
    }

    return true;
}

const vector<unsigned char>& Request::content(read_content fn, void* user_data) const
{
    if (has_read_content == true)
    {
        _content;
    }

    // If send more than max_post_bytes, forget it. If max_post_bytes == -1,
    // check is not performed.

    if ((max_content_length != -1) && (bytes_sent() > max_content_length))
    {
        _error.clear();

        stringstream strm;

        strm << "Request::form_data(): exceded bytes_sent: "
             << max_content_length;

        _error = strm.str();
    }

    // Set things up in Apache to get ready to read data from client.
    setup_client_read();

    //> Copy data out

    char buff[4096];
    int len = 0;

    while ((len = get_client_block(buff, sizeof(buff) - 1)) > 0)
    {
        // NULL terminate
        buff[len] = 0;

        if (fn != NULL)
        {
            // Caller wants it fed to them.
            fn(&buff[0], len, user_data);
        }
        else
        {
            // Caller wants to collect it all in a buffer. Append.
            _content.insert(_content.end(), &buff[0], &buff[len]);
        }
    }

    // Flag that we've read all the content, so we don't try this again.
    has_read_content = true;

    return _content;
}

string Request::read_line() const
{
    if (has_read_content == true)
    {
        return "";
    }

    // Set things up in Apache to get ready to read data from client.
    setup_client_read();

    //> Copy data out

    string content;
    content.reserve(100);
    char buff[2];
    int len = 0;

    while (1)
    {
        int len = get_client_block(buff, 1);

        if (len > 0)
        {
            // NULL terminate
            buff[len] = 0;

            // Copy
            content += buff;

            // Check for CRLF
            if (buff[0] == '\n')
            {
                if (content[content.length() - 2] == '\r')
                {
                    break;
                }
            }
        }
        else
        {
            // Flag that we've read all the content, so we don't try this again.
            has_read_content = true;

            break;
        }
    }

    return content;
}

i32 Request::read(char** buff, i32 len) const
{
    if (has_read_content == true)
    {
        return -1;
    }

    // Set things up in Apache to get ready to read data from client.
    setup_client_read();

    //> Copy data out

    int n = get_client_block(*buff, len);

    if (n <= 0)
    {
        // Flag that we've read all the content, so we don't try this again.
        has_read_content = true;
    }

    return n;
}

bool Request::parse_query_string(const char* in, modruby::apr::table& t)
{
    vector<string> pairs;
    split(in, '&', pairs);

    std::vector<std::string>::iterator i;
    for (i = pairs.begin(); i != pairs.end(); i++)
    {
        std::size_t found = i->find("=");

        if (found != string::npos)
        {
            // Guard against empty/NULL value. In theory this should never
            // happen. But if it ever did it might cause problems.
            if (found == (i->size() - 1))
            {
                continue;
            }

            t.merge( url_decode(i->substr(0, found).c_str()).c_str(),
                     url_decode(i->substr(found + 1, i->size() - 1).c_str()).c_str() );
        }
    }

    return true;
}

apr_table_t* Request::form_data() const
{
    // If we have copied this out before
    if (_form_data != NULL)
    {
        // Return the table
        return _form_data;
    }

    // Guard against passing NULL into string
    const char* value = headers_in().get("Content-Type");

    if (value == NULL)
    {
        return NULL;
    }

    string content_type = value;

    if (content_type.find("application/x-www-form-urlencoded") == string::npos)
    {
        _error = "Request::form_data(): unsupported content type";

        // BREAKAGE:(?) The previous behavior here was to return NULL. I don't
        // see any code that depends on this value, and the right thing to do is
        // to return an empty table, not NULL.

        // Create an empty table
        apr::table t(pool());

        t.merge( "content-type", content_type.c_str());

        // Get reference to apr_table_t. We will just return this if this function
        // is ever called again.
        _form_data = t.handle;

        return _form_data;
    }

    //> Parse query string to table

    apr::table t(pool());

    // Read in all data
    content();

    // Null-terminate it
    _content.push_back(char(0));

    parse_query_string((const char*)_content.data(), t);

    // Get reference to apr_table_t. We will just return this if this function
    // is ever called again.
    _form_data = t.handle;

    return t.handle;
}

void* Request::get_dir_config(module* m) const
{
    return ap_get_module_config(_req->per_dir_config, m);
}

int Request::get_basic_auth_pw(const char** sent_pw) const
{
    return ap_get_basic_auth_pw(_req, sent_pw);
}

int Request::rputs(const char* str) const
{
    return ap_rputs(str, _req);
}

int Request::rputc(int c) const
{
    return ap_rputc(c, _req);
}

int Request::rwrite(const void* buf, int nbyte) const
{
    return ap_rwrite(buf, nbyte, _req);
}

int Request::rprintf(const char* fmt, ...) const
{
    int result;
    va_list vp;
    va_start(vp, fmt);
    result = ap_vrprintf(_req, fmt, vp);
    va_end(vp);

    return result;
}

int Request::rflush() const
{
    return ap_rflush(_req);
}

void Request::allow_methods(int reset, ...) const
{
    va_list vp;
    va_start(vp, reset);
    ap_allow_methods(_req, reset, vp);
    va_end(vp);
}

int Request::send_file(const char* path)
{
    apr::subpool pool;

    // Get the file status
    apr::file_info stat = apr::stat(path, pool.handle);

    // Ensure that the file exists
    if (stat.valid() != true)
    {
        return -1;
    }

    // Should be a regular file
    if (stat.type() != APR_REG)
    {
        return -1;
    }

    // Open the file.
    apr_file_t* file;
    apr_status_t rc = apr_file_open( &file, path, APR_READ,
                                     APR_OS_DEFAULT, pool.handle );

    if (rc != APR_SUCCESS)
    {
        return -1;
    }

    // Send it
    apr_size_t sent;
    rc = ap_send_fd(file, _req, 0, stat.size(), &sent);

    // Close it
    apr_file_close(file);

    return sent;
}
