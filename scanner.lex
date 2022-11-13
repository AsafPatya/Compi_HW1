%{
/* Declarations section */
#include <stdio.h>
#include "tokens.hpp"
%}

%option yylineno
%option noyywrap
digit   		                              ([0-9])
letter  		                              ([a-zA-Z])
printable_letters                             ([\x20-\x21\x23-\x7e]|((\\)(\\))|((\\)(\"))|((\\)(n))|((\\)(r))|((\\)(t))|((\\)x))
whitespace                                    ([\t\n\r ])

%x                                            STRING_START

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
"=="|"!="|"<"|">"|"<="|">="                   return RELOP;
"+"|"-"|"*"|"/"	                              return BINOP;
\/\/[^\r\n]*                                  return COMMENT;
[a-zA-Z]+([0-9]|[a-zA-Z])*                    return ID;
0|[1-9]+{digit}*                              return NUM;
[0]{digit}+                                   return ZERO_FIRST;
{whitespace}                                  return WHITESPACE;

(\")                                          BEGIN(STRING_START);
<STRING_START><<EOF>>                         return UNCLOSED_STRING;
<STRING_START>{printable_letters}*(\")        { BEGIN(INITIAL); return STRING; }
<STRING_START>([^(\")])*((\")?)               return UNCLOSED_STRING;
<STRING_START>.                               return UNCLOSED_STRING;

.                                             return WRONG_CHAR;
%%