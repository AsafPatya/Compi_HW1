#include <string>
#include <iostream>
#include "tokens.hpp"
using namespace std;

#define INVALID_ESCAPE_SEQUENCE_ERROR "Error undefined escape sequence"

const char SKIP_CHAR      = '\0';
const char MIN_CHAR_BOUND = '\x00';
const char MAX_CHAR_BOUND = '\x7f';

bool isPrintableChar(char chr)
{
    return not(chr < MIN_CHAR_BOUND or chr > MAX_CHAR_BOUND);
}

std::string getEscapeSequence(const int escape_seq_idx)
{
    std::string sequence;
    char* ch1 = yytext + escape_seq_idx + 1;
    char* ch2 = ch1 + 1;

    if (ch1 != nullptr and *ch1 != '"') {
        sequence.push_back(*ch1);
    }
    else {
        ch2 = nullptr;
    }

    if (ch2 != nullptr and *ch2 != '"') {
        sequence.push_back(*ch2);
    }

    return sequence;
}

void handleWrongChar()
{
    const char* error_message = "ERROR";
    const char* lexeme = yytext;
    cout << error_message << " " << yytext <<  endl;
    exit(0);
}

void handleStartWithZero()
{
    const char* error_message = "ERROR";
    const char* lexeme = "0";
    cout << error_message << " " << yytext <<  endl;
    exit(0);
}

void handleLineEndedInMiddleString()
{
    const char* error_message = "Error unclosed string";
    cout << error_message <<  endl;
    exit(0);
}

void printTokenComment()
{
    const char* currTokenName = tokenNames[COMMENT];
    const int lineNumber = yylineno;
    const char* commentLexeme = "//";
    cout << lineNumber << " " << currTokenName << " " << commentLexeme << endl;
}
/*
void printToken(const int token, bool is_string_token = false)

{
    const char*  currTokenName = tokenNames[token];
    const int    lineNumber    = yylineno;
    const string lexeme        = yytext;
    if (is_string_token) {
        cout << lineNumber << " " << currTokenName << " " << lexeme.substr(0, lexeme.length()-1) << endl;
    }
    else {
        cout << lineNumber << " " << currTokenName << " " << lexeme << endl;
    }

} */

void printTokenString(const int token, string input_str=yytext) {
    const char*  currTokenName = tokenNames[token];
    const int    lineNumber    = yylineno;
    cout << lineNumber << " " << currTokenName << " " << input_str << endl;
}

char escapeSequenceHandler(int& escape_seq_idx)
{
    std::string lexeme;
    escape_seq_idx += 1U;

    char ch = yytext[escape_seq_idx];
    if (ch == 'n') {
        return '\n';
    }
    else if (ch == '"') {
        return '\"';
    }
    else if (ch == 't') {
        return '\t';
    }
    else if (ch == '\\') {
        return '\\';
    }
    else if (ch == 'r') {
        return '\r';
    }
    else if (ch == 'x') {
        std::string sequence = getEscapeSequence(escape_seq_idx);
        if (sequence.length() == 2 &&
            isxdigit((int)yytext[escape_seq_idx + 1]) &&
            isxdigit((int)yytext[escape_seq_idx + 2])) {  // check if the two following chars are hex
            char ch = (char) std::stoi(sequence, nullptr, 16);
            escape_seq_idx += 2;

            if (isPrintableChar(ch)) {  // check if the given sequence is printable
                return ch;
            }
        }
    }
    handleWrongChar();
    return SKIP_CHAR;
}

void printStringToken()
{
    std::string Lexeme;
    std::string text(yytext);
    int         string_index = 0;
    bool        null_inside  = false;

    while (true) {
        if (text[string_index] == 0) {
            break;
        }

        char ch = text[string_index];

        if (text.substr(string_index, 2) == "\\0"
            or text.substr(string_index, 4) == "\\x00") {
            null_inside = true;
        }

        else if (ch == '\\') {
            ch = escapeSequenceHandler(string_index);
        }

        else if (ch == '"') {  // this is the last char in the string
            printTokenString(STRING, Lexeme);
            goto end;
        }

        if (not null_inside) {
            Lexeme.push_back(ch);
        }
        string_index += 1U;
    }
    return handleLineEndedInMiddleString();

end:
    return;
}


int main()
{
    int token;
    while ((token = yylex()))
    {
        switch (token)
        {
            case WRONG_CHAR:
                handleWrongChar();
                break;
            case STRART_WITH_ZERO:
                handleStartWithZero();
                break;
            case UNCLOSED_STRING:
                handleLineEndedInMiddleString();
                break;
            case WHITESPACE:
                break;
            case STRING:
                printStringToken();
                break;
            case COMMENT:
                printTokenComment();
                break;
            default:
                printTokenString(token);
                break;
        }
    }
}