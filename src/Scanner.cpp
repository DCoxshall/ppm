#include "Scanner.h"

#include <stdexcept>

#include "Token.h"

static bool is_whitespace(uint8_t b) {
    char c = static_cast<char>(b);
    switch (c) {
        case ' ':
        case '\r':
        case '\n':
        case '\t':
            return true;
        default:
            return false;
    }
}

void Scanner::scan_number() {
    std::string num_lexeme;
    char c = static_cast<char>(source[at]);
    while (isdigit(c)) {
        num_lexeme.push_back(source[at]);
        at++;
        if (at_end()) break;
        c = static_cast<char>(source[at]);
    }
    tokens.push_back(Token{Number, num_lexeme});
}

void Scanner::scan_magic_number() {
    std::string magic_lexeme;
    char c = static_cast<char>(source[at]);

    while (isalnum(source[at])) {
        magic_lexeme.push_back(source[at]);
        at++;
        if (at_end()) break;
        c = static_cast<char>(source[at]);
    }
    tokens.push_back(Token{MagicNumber, magic_lexeme});
}

void Scanner::skip_whitespace() {
    while (std::string(" \t\r\n").find(source[at]) != std::string::npos) {
        at++;
        if (at_end()) break;
    }
}

void Scanner::skip_to_next_line() {
    while (source[at] != '\n') {
        at++;
        if (at_end()) break;
    }
}

std::vector<Token> Scanner::generate_tokens() {
    while (!at_end()) {
        char c = static_cast<char>(source[at]);
        if (isdigit(c)) {
            scan_number();
        } else if (isalpha(c)) {
            scan_magic_number();
        } else if (is_whitespace(c)) {
            skip_whitespace();
        } else if (c == '#') {
            skip_to_next_line();
        } else {
            throw std::runtime_error("Unrecognised character: " + c);
        }
    }

    Token eof = Token{EndOfFile, "EOF"};
    tokens.push_back(eof);

    return tokens;
}
