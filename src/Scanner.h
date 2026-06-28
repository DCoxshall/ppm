#pragma once
#include <cstdint>
#include <vector>

#include "Token.h"

class Scanner {
   private:
    std::vector<uint8_t> source;
    std::vector<Token> tokens;
    size_t at;

    void scan_number();
    void scan_magic_number();
    void scan_blob();

    void skip_whitespace();
    void skip_single_whitespace();
    void skip_to_next_line();

    bool at_end() {
        return at == source.size();
    }

   public:
    Scanner(std::vector<uint8_t> source) {
        this->source = source;
        this->at = 0;
    }

    std::vector<Token> generate_tokens();
};