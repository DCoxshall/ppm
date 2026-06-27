#pragma once
#include <iostream>
#include <string>

enum TokenType {
    Number,
    MagicNumber,
    EndOfFile,
};

struct Token {
    TokenType type;
    std::string lexeme;  // raw text, e.g. "P3", "255"
};

void token_show(Token t);
