#include <string>
#include <iostream>
#include "tokens.hpp"

#define COMMENT_LEX "//"

const char SKIP_CHARACHTER = '\0';
const char MINIMUM_CHAR_BOUND = '\x00';
const char MAXIMUM_CHAR_BOUND = '\x7f';

using namespace std;

bool charInBound(char chr)
{
    bool res = not (chr < MINIMUM_CHAR_BOUND or chr > MAXIMUM_CHAR_BOUND);
    return res;
}

string getEscapeString(int esc_seq_idx)
{
    string sequence;
    char firstChar  = (yytext != nullptr)? yytext[++esc_seq_idx] : '\0';
    char secondChar = (yytext != nullptr)? yytext[++esc_seq_idx] : '\0';

    if (firstChar != '\0' and firstChar != '"') {
        sequence.push_back(firstChar);
    }

    if (secondChar != '\0' and secondChar != '"') {
        sequence.push_back(secondChar);
    }

    return sequence;
}

/// handles function - start

void handleWrongChar()
{
    const char* error_message = "ERROR";
    cout << error_message << " " << yytext <<  endl;
    exit(0);
}

void handleStartWithZero()
{
    const char* error_message = "ERROR";
    cout << error_message << " " << yytext <<  endl;
    exit(0);
}

void handleUnclosedString()
{
    const char* error_message = "Error unclosed string";
    cout << error_message <<  endl;
    exit(0);
}

void handleInvalidEscapeSequenceError(const string& lexeme)
{
    // for handleEscapeSequence function
    const char* error_message = "Error undefined escape sequence";
    cout << error_message << " " << lexeme <<  endl;
    exit(0);
}

/// handles function - end

void printTokenString(const int token, const string& lexeme = yytext)
{
    cout << yylineno << " " << tokenNamesArray[token] << " " << lexeme << endl;
}

char escapeSequenceHandler(int& escape_seq_idx)
{
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
        string sequence = getEscapeString(escape_seq_idx);
        if (sequence.length() == 2 and
            isxdigit((int)yytext[escape_seq_idx + 1]) and
            isxdigit((int)yytext[escape_seq_idx + 2])) {
            char ch = (char) stoi(sequence, nullptr, 16);
            escape_seq_idx += 2;

            if (charInBound(ch)) {
                return ch;
            }
        }
    }
    handleWrongChar();
    return SKIP_CHARACHTER;
}

void printStringToken()
{
    string Lexeme;
    string text(yytext);
    int         string_index = 0;
    bool        null_inside  = false;

    while (true) {
        if (text[string_index] == '\0') {
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

        else if (ch == '"') {
            printTokenString(STRING, Lexeme);
            goto end;
        }

        if (not null_inside) {
            Lexeme.push_back(ch);
        }
        string_index += 1U;
    }
    return handleUnclosedString();

end:
    return;
}

int main()
{
    int token;
    while ((token = yylex()))
    {
        if (token == WRONG_CHAR) {
            handleWrongChar();
        }
        else if (token == ZERO_FIRST) {
            handleStartWithZero();
        }
        else if (token == UNCLOSED_STRING) {
            handleUnclosedString();
        }
        else if (token == WHITESPACE) {
            continue;
        }
        else if (token == STRING) {
            printStringToken();
        }
        else if (token == COMMENT) {
            printTokenString(COMMENT, COMMENT_LEX);
        }
        else {
            printTokenString(token);
        }
    }
    return 0;
}