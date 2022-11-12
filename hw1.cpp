#include <string>
#include <iostream>
#include "tokens.hpp"

#define COMMENT_LEXEME "//"

const char SKIP_CHAR = '\0';
const char MIN_CHAR_BOUND = '\x00';  // TODO- check the range
const char MAX_CHAR_BOUND = '\x7f';

using namespace std;

bool isPrintableChar(char chr)
{
    return chr >= MIN_CHAR_BOUND && chr <= MAX_CHAR_BOUND;
}

std::string getEscapeSequence(const int escapeSequenceIndex)
{
    std::string sequence;
    const char* const firstChar = yytext + escapeSequenceIndex + 1;
    char* secondChar = yytext + escapeSequenceIndex + 2;

    if (firstChar != nullptr && *firstChar != '"') {
        sequence.push_back(*firstChar);
    } else {
        secondChar = nullptr;
    }

    if (secondChar != nullptr && *secondChar != '"') {
        sequence.push_back(*secondChar);
    }

    return sequence;
}

/// handles function - start

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

void handleUnclosedString()
{
    const char* error_message = "Error unclosed string";
    cout << error_message <<  endl;
    exit(0);
}

void handleInvalidEscapeSequenceError(const std::string& lexeme)
{
    // for handleEscapeSequence function
    const char* error_message = "Error undefined escape sequence";
    cout << error_message << " " << lexeme <<  endl;
    exit(0);
}

/// handles function - end




void printTokenString(const int token, const std::string& lexeme = yytext)
{
    std::cout << yylineno << " " << tokenNames[token] << " " << lexeme << std::endl;
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
    return handleUnclosedString();

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
                handleWrongChar(); // changed V
                break;

            case ZERO_FIRST:
                handleStartWithZero();  // changed V
                break;

            case UNCLOSED_STRING:  // changed V
                handleUnclosedString();
                break;

            case WHITESPACE:
                break;
            case STRING:
                printStringToken();
                break;
            case COMMENT:
                printTokenString(COMMENT, COMMENT_LEXEME);
                break;
            default:
                printTokenString(token);
                break;
        }
    }
    return 0;
}