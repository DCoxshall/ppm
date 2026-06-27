#include "Token.h"

void token_show(Token t) {
    switch (t.type) {
        case Number:
            std::cout << "Number: ";
            break;
        case MagicNumber:
            std::cout << "MagicNumber: ";
            break;
        case EndOfFile:
            std::cout << "EndOfFile: ";
            break;
    };

    std::cout << t.lexeme << std::endl;
}