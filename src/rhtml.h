#ifndef MODRUBY_RHTMLPARSER_DECLARE
#define MODRUBY_RHTMLPARSER_DECLARE

#include <string>
#include <sstream>

#include "ruby.hpp"

namespace ruby
{

struct yy_buffer_state;

/// This is a rhtml that works like eRuby. It is a stripped down, minimalist
/// version which that doesn't know anything about CGI or HTTP per se, just
/// parsing and executing RHTML code.
///
/// There are significant differences between eRuby and ERB/erubis in that the
/// former processes puts/print statements "correctly" and the latter do
/// not. That is, a puts statement in RHTML code should stream its output in the
/// order in which is is located in the RHTML document. THIS DOES NOT WORK IN
/// ERB/erubis. Rather, these latter two will stream all output from puts at the
/// head of the document -- out of order. I consider this a design flaw. Only
/// eRuby does this correctly.
///
/// Therefore, this rhtml is modeled after eRuby.

class RhtmlParser
{
    // Disallow assignment operator and default copy ctor
    const RhtmlParser &operator=(const RhtmlParser &old);
    RhtmlParser(const RhtmlParser &old);

    // A handle to the scanner
    void* scanner;

  public:

    enum state_t
    {
        CODE_STATE    = 1,
        VAR_STATE     = 2,
        COMMENT_STATE = 3,
        TEXT_STATE    = 4
    };

    std::string text;
    std::string line;

  private:

    ruby::Object _context;
    VALUE _binding;
    state_t _state;
    int _block_start_line;
    int _current_line;
    const char* _file_name;

  public:

    RhtmlParser();

    /// Process file contents
    bool compile_file(const char* filename);
    bool eval_file(const char* filename, VALUE binding=Qnil);

    state_t state() { return _state; }

    /// Process chunk of text
    bool compile_text(const char* str);
    bool eval_text(const char* str, VALUE binding=Qnil);

    void change(state_t s);
    void append(const char* text);
    void read(const char* text);
    void end_line();

  protected:

    bool eval(VALUE b);

    const char* content_name();
    void init();
    int finalize();

    int current_line();
    int block_start_line();
};

} // end namespace ruby

#endif
