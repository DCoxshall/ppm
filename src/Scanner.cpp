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
    if (at_end()) throw std::runtime_error("Could not scan number.");
    bool scanned_at_least_one = false;
    std::string num_lexeme;
    char c = static_cast<char>(source[at]);
    while (isdigit(c)) {
        scanned_at_least_one = true;
        num_lexeme.push_back(source[at]);
        at++;
        if (at_end()) break;
        c = static_cast<char>(source[at]);
    }
    if (!scanned_at_least_one) throw std::runtime_error("Expected number.");
    tokens.push_back(Token{Number, num_lexeme});
}

void Scanner::scan_magic_number() {
    if (at_end()) throw std::runtime_error("No magic number found.");

    if (!isalnum(source[at]))
        throw std::runtime_error("Magic number not in correct format.");

    std::string magic_lexeme;

    while (isalnum(source[at])) {
        magic_lexeme.push_back(source[at]);
        at++;
        if (at_end()) break;
    }

    tokens.push_back(Token{MagicNumber, magic_lexeme});
}

void Scanner::skip_to_next_line() {
    while (source[at] != '\n') {
        at++;
        if (at_end()) break;
    }
}

void Scanner::skip_whitespace() {
    if (at_end())
        throw std::runtime_error("EOF found when trying to scan whitespace.");

    bool skipped_at_least_one = false;

    while (source[at] == '#') {
        skip_to_next_line();

        // We set this flag because upon moving to a newline we're guaranteed to
        // have scanned at least one '\n' character.
        skipped_at_least_one = true;
    }

    // While we're on a whitespace character, progress.
    while (std::string(" \t\r\n").find(source[at]) != std::string::npos) {
        skipped_at_least_one = true;
        at++;
        if (at_end()) break;

        // While we're at the start of a comment, skip to the start of the next
        // line.
        while (source[at] == '#') skip_to_next_line();
    }

    if (!skipped_at_least_one) {
        throw std::runtime_error(
            "No whitespace character found before next token.");
    }
}

void Scanner::skip_single_whitespace() {
    if (at_end()) throw std::runtime_error("Pixel data not found.");

    // We might also have a comment immediately before that single whitespace.
    if (source[at] == '#') {
        skip_to_next_line();
    } else if (std::string(" \t\r\n").find(source[at]) == std::string::npos) {
        throw std::runtime_error("Invalid character after maxval.");
    }
    // Consume the whitespace character. We should now be sitting at the first
    // byte of the blob.
    at++;
}

void Scanner::scan_blob() {
    if (at_end()) throw std::runtime_error("No image data found.");
    std::string blob_data = "";
    while (!at_end()) {
        blob_data.push_back(source[at]);
        at++;
    }
    tokens.push_back(Token{Blob, blob_data});
}

std::vector<Token> Scanner::generate_tokens() {
    tokens.clear();

    scan_magic_number();       // Magic number.
    skip_whitespace();         // Whitespace.
    scan_number();             // Width.
    skip_whitespace();         // Whitespace.
    scan_number();             // Height.
    skip_whitespace();         // Whitespace.
    scan_number();             // Maxval.
    skip_single_whitespace();  // Single whitespace character.
    scan_blob();               // The rest of the data.

    Token eof = Token{EndOfFile, "EOF"};
    tokens.push_back(eof);

    return tokens;
}
