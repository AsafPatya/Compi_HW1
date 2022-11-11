#include <string>
#include <iostream>
#include "tokens.hpp"
using namespace std;


//#define UNCLOSED_STRING_ERROR "Error unclosed string"
//#define INVALID_ESCAPE_SEQUENCE_ERROR "Error undefined escape sequence"
//
//const char SKIP_CHAR = '\0';
//const char MIN_CHAR_BOUND = '\x00';  // TODO- check the range
//const char MAX_CHAR_BOUND = '\x7f';
//
//bool isPrintableChar(char chr)
//{
//    return chr >= MIN_CHAR_BOUND && chr <= MAX_CHAR_BOUND;
//}
//
//std::string getEscapeSequence(const int escapeSequenceIndex)
//{
//    std::string sequence;
//    const char* const firstChar = yytext + escapeSequenceIndex + 1;
//    char* secondChar = yytext + escapeSequenceIndex + 2;
//
//    if (firstChar != nullptr && *firstChar != '"') {
//        sequence.push_back(*firstChar);
//    } else {
//        secondChar = nullptr;
//    }
//
//    if (secondChar != nullptr && *secondChar != '"') {
//        sequence.push_back(*secondChar);
//    }
//
//    return sequence;
//}
//
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
    const char* commentLexeme = "\\";
    cout << lineNumber << " " << currTokenName << " " << commentLexeme << endl;
}

void printToken(const int token)

{
    const char* currTokenName = tokenNames[token];
    const int lineNumber = yylineno;
    const string lexeme = yytext;
    cout << lineNumber << " " << currTokenName << " " << lexeme << endl;

}

char handleEscapeSequence(int& escape_seq_idx)
{
    std::string lexeme;
    escape_seq_idx += 1U;
    std::string arg(yytext + escape_seq_idx, 1);

    char ch = yytext[escape_seq_idx];
    if (ch == 'n') {
        return '\n';
    }
    else if (ch == '"') {
        return '\"';
    }
    else if (ch == '\t') {
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
        arg = "x" + sequence;
    }
    handleInvalidToken(INVALID_ESCAPE_SEQUENCE_ERROR, true, arg, 1));
    return SKIP_CHAR;
}

void printStringToken()
{
    std::string Lexeme;
    std::string text(yytext);
    u_int32_t   string_index = 0U;
    bool        null_inside  = false;

    while (true) {
        if (text[stringIndex] != 0) {
            break;
        }

        char ch = text[stringIndex];

        if (text.substr(string_index, 2) == "\\0"
            or text.substr(string_index, 4) == "\\x00") {
            null_inside = true;
        }

        else if (ch == '"') {  // this is the last char in the string
            printToken(STRING, Lexeme);
            goto end;
        }

        ch = (ch == '\\')? escapeSequenceHandler(string_index) : ch;

        if (not null_inside) {
            Lexeme.push_back(ch);
        }
        string_index += 1U;
    }
    return handleInvalidToken(UNCLOSED_STRING_ERROR, false);

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
                printToken(token);
                break;
        }
    }
}