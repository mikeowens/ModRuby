%{

#include "rhtml.h"

#define YY_DECL int lexscan(ruby::RhtmlParser& s, yyscan_t yyscanner)
%}

%option noyywrap
%option reentrant

%x TEXT
%x CODE
%x INDENT
%x COMMENT

%%

\r\n|\n {

    yyless(0);
    BEGIN TEXT;
    //s.change(ruby::RhtmlParser::TEXT_STATE);
}

. {
    yyless(0);
    BEGIN TEXT;
}

<TEXT>"\\n" {

}

<TEXT>"\\r" {

}

<TEXT>"\\t" {

}

<TEXT>"<%" {
    s.change(ruby::RhtmlParser::CODE_STATE);

    BEGIN CODE;
}

<TEXT>"{%" {
    s.change(ruby::RhtmlParser::CODE_STATE);

    BEGIN CODE;
}

<TEXT>"<%=" {
    s.change(ruby::RhtmlParser::VAR_STATE);
    s.append("print((");

    BEGIN CODE;
}

<TEXT>"{%=" {
    s.change(ruby::RhtmlParser::VAR_STATE);
    s.append("print((");

    BEGIN CODE;
}

<CODE>"%>" {
    if(s.state() == ruby::RhtmlParser::VAR_STATE)
    {
        s.append("))");
    }

    s.change(ruby::RhtmlParser::TEXT_STATE);

    BEGIN TEXT;
}

<CODE>"%}" {
    if(s.state() == ruby::RhtmlParser::VAR_STATE)
    {
        s.append("))");
    }

    s.change(ruby::RhtmlParser::TEXT_STATE);

    BEGIN TEXT;
}

<TEXT>"<%--" {
    s.change(ruby::RhtmlParser::COMMENT_STATE);

    BEGIN COMMENT;
}

<COMMENT>"%>" {
    s.change(ruby::RhtmlParser::TEXT_STATE);

    BEGIN TEXT;
}


<TEXT>. {
    s.read(yytext);
}


<CODE>. {
    s.read(yytext);
}

<COMMENT>. {
    s.read(yytext);
}


<TEXT>\r\n|\n {
    s.end_line();
}

<CODE>\r\n|\n {
    s.end_line();
}

<COMMENT>\r\n|\n {
    s.end_line();
}

<COMMENT>"--%>" {
    BEGIN TEXT;
}

%%

using namespace ruby;

void RhtmlParser::init()
{
    line.clear();
    text.clear();
}

int RhtmlParser::finalize()
{    
    change(ruby::RhtmlParser::TEXT_STATE);

    return 0;
}

bool RhtmlParser::compile_file(const char* filename)
{
    FILE* in;

    if(filename != NULL)
    {
        // Check to see if file exists
        if(access(filename, R_OK) != 0)
        {
            return false;
        }
 
        in = fopen(filename, "r");

        _file_name = filename;
    }
    else
    {
        in = stdin;
    }

    init();
    yylex_init(&scanner);
    yyset_in(in, scanner);

    int ret = lexscan(*this, scanner);
    yylex_destroy(scanner);
    fclose(in);

    finalize();

    return true;
}

bool RhtmlParser::compile_text(const char* str)
{
    init();
    yylex_init(&scanner);
    YY_BUFFER_STATE lexstate = yy_scan_bytes(str, strlen(str), scanner);
    yy_switch_to_buffer(lexstate, scanner);
    int ret = lexscan(*this, scanner);
    finalize();
    yy_delete_buffer(lexstate, scanner);
    yylex_destroy(scanner);    

    return true;
}
