#pragma once
#include <iostream>
#include <string>

enum TokenType {
    Number,
    MagicNumber,
    EndOfFile,
    Blob,
};

struct Token {
    TokenType type;
    std::string lexeme;
};

void token_show(Token t);
