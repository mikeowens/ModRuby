%{

#include "rhtml.h"

#define YY_DECL int lexscan(int a, ruby::rhtml& s)
%}

%option noyywrap

%x TEXT
%x CODE
%x INDENT
%x COMMENT

%%

\r\n|\n {

    yyless(0);
    BEGIN TEXT;
    //s.change(ruby::rhtml::TEXT_STATE);
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
    s.change(ruby::rhtml::CODE_STATE);

    BEGIN CODE;
}

<TEXT>"{%" {
    s.change(ruby::rhtml::CODE_STATE);

    BEGIN CODE;
}

<TEXT>"<%=" {
    s.change(ruby::rhtml::VAR_STATE);
    s.append("print((");

    BEGIN CODE;
}

<TEXT>"{%=" {
    s.change(ruby::rhtml::VAR_STATE);
    s.append("print((");

    BEGIN CODE;
}

<CODE>"%>" {
    if(s.state() == ruby::rhtml::VAR_STATE)
    {
        s.append("))");
    }

    s.change(ruby::rhtml::TEXT_STATE);

    BEGIN TEXT;
}

<CODE>"%}" {
    if(s.state() == ruby::rhtml::VAR_STATE)
    {
        s.append("))");
    }

    s.change(ruby::rhtml::TEXT_STATE);

    BEGIN TEXT;
}

<TEXT>"<%--" {
    s.change(ruby::rhtml::COMMENT_STATE);

    BEGIN COMMENT;
}

<COMMENT>"%>" {
    s.change(ruby::rhtml::TEXT_STATE);

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

void rhtml::init()
{
    line.clear();
    text.clear();
}

int rhtml::finalize()
{    
    change(ruby::rhtml::TEXT_STATE);

    return 0;
}

bool rhtml::compile_file(const char* filename)
{
    init();

    if(filename != NULL)
    {
        yyin = fopen(filename, "r");

        _file_name = filename;
    }
    else
    {
        yyin = stdin;
    }

    yyrestart(yyin);
    int ret = lexscan(1, *this);
    fclose(yyin);

    finalize();

    return true;
}

bool rhtml::compile_text(const char* str)
{
    init();

    YY_BUFFER_STATE lexstate = yy_scan_bytes(str, strlen(str));
    yy_switch_to_buffer(lexstate);
    int ret = lexscan(1, *this);

    finalize();

    yy_delete_buffer(lexstate);
    
    return true;
}
