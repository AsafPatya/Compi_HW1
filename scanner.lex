%{
/* Declarations section */
#include <stdio.h>
#include "tokens.hpp"
%}

%option yylineno
%option noyywrap

digit   		                              ([0-9])
letter  		                              ([a-zA-Z])
printable_chars                               ([\x20-\x21\x23-\x7e]|additional_printable_chars)
additional_printable_chars                    (((\\)(\\))|((\\)(\"))|((\\)(n))|((\\)(r))|((\\)(t))|((\\)x))
no_new_line_chars                             [\x00-\x09\x0b-\x0c\x0e-\x7f]
whitespace                                    ([\t\n\r ])

%x                                            STRING_CONDITION

%%

void                                          return VOID;
int                                           return INT;
byte                                          return BYTE;
b                                             return B;
bool                                          return BOOL;
and                                           return AND;
or                                            return OR;
not                                           return NOT;
true                                          return TRUE;
false                                         return FALSE;
return                                        return RETURN;
if                                            return IF;
else                                          return ELSE;
while                                         return WHILE;
break                                         return BREAK;
continue                                      return CONTINUE;
";"                                           return SC;
","                                           return COMMA;
"("                                           return LPAREN;
")"                                           return RPAREN;
"{"                                           return LBRACE;
"}"                                           return RBRACE;
"="                                           return ASSIGN;
("=="|"!="|"<"|">"|"<="|">=")                 return RELOP;
"+"|"-"|"*"|"/"                               return BINOP;
\/\/{no_new_line_chars}*                      return COMMENT;
{letter}({letter}|{digit})*                   return ID;
[1-9]{digit}*|0                               return NUM;
[0]{digit}+                                   return STRART_WITH_ZERO;
{whitespace}                                  return WHITESPACE;

(\")                                          BEGIN(STRING_CONDITION);
<STRING_CONDITION><<EOF>>                      return UNCLOSED_STRING;
<STRING_CONDITION>{printable_chars}*(\")     { BEGIN(INITIAL); return STRING; }
<STRING_CONDITION>([^(\")])*((\")?)            return UNCLOSED_STRING;
<STRING_CONDITION>.                            return UNCLOSED_STRING;

.                                             return WRONG_CHAR;

%%