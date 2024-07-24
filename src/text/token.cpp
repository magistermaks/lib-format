
#include "token.hpp"

[[noreturn]] void Token::raise(const std::string& message) const {
	throw ParseError {message, line, column};
}

[[noreturn]] void Token::expected(const std::string& value) const {
	raise("Expected " + value + " but got: '" + std::string {start, (size_t) (end - start)} + "'");
}

std::string Token::identify(char chr) {
	if (chr >= ' ' && chr <= '~') {
		return std::string("'") + chr + "'";
	}

	if (chr == '\0') return "nul byte (\'\\0\')";
	if (chr == '\n') return "new line (\'\\n\')";
	if (chr == '\t') return "tab (\'\\t\')";
	if (chr == '\r') return "carriage return (\'\\n\')";

	return "ascii+" + std::to_string((int) chr);
}

bool Token::isWhite(char chr) {
	return chr == ' ' || chr == '\n' || chr == '\t' || chr == '\r';
}

bool Token::isSymbol(char chr) {
	return chr == '[' || chr == ']' || chr == '{' || chr == '}' || chr == ',';
}

bool Token::isJoint(char chr) {
	return chr == '_' || chr == '-' || chr == '.';
}

bool Token::isPrintable(char chr) {
	return chr >= ' ' && chr <= '~';
}

bool Token::isAlphaNumeric(char chr) {
	return (chr >= '0' && chr <= '9') || (chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z');
}

bool Token::isWord(char chr) {
	return isAlphaNumeric(chr) || isJoint(chr);
}

Token::Token(const char* start, int length, Type type, int line, int column)
: start(start), end(start + length), type(type), line(line), column(column) {}

const char* Token::name() const {
	if (type == WORD) return "word";
	if (type == STRING) return "string";
	if (type == SYMBOL) return "symbol";

	return "error";
}

std::string_view Token::view() const {
	return {start, (size_t) std::distance(start, end)};
}

bool Token::isSymbolEqual(char symbol) const {
	return start[0] == symbol;
}

std::vector<Token> Token::tokenize(const char* start, int size) {

	std::vector<Token> tokens;

	enum {
		OUTER,
		WORD,
		STRING,
		COMMENT
	} state = OUTER;

	int last = size - 1;
	int line = 1;
	int column = 1;

	int begin = 0;

	for (int i = 0; i < size; i ++) {

		char c = *(start + i);
		char n = (i == last) ? 0 : *(start + i + 1);

		// keep track of the position in the input file
		if (c == '\n') {
			line ++;
			column = 1;
		} else {
			column ++;
		}

		// outside and specific token
		// looks at the next character to switch states, emits symbol tokens
		if (state == OUTER) {

			// begin comment
			if (c == '/' && n == '*') {
				state = COMMENT;

				// skip the '*'
				i ++;
				column ++;
				continue;
			}

			// begin string, words and strings require speration per tt spec
			if (c == '"') {
				begin = i + 1;
				state = STRING;
				continue;
			}

			// begin identifier or value
			if (isWord(n)) {
				begin = i + 1;
				state = WORD;
				continue;
			}

			// check for symbols
			if (isSymbol(c)) {
				tokens.emplace_back(start + i, 1, Token::SYMBOL, line, column);
				continue;
			}

			// don't care
			if (isWhite(c)) {
				continue;
			}

			throw ParseError {"Unexpected " + identify(c) + " in scope", line, column};

		}

		// identifier or number, any non-quoted sequance of characters
		// isWord(c) is quaranteed to be true, emits word tokens
		if (state == WORD) {

			// end token
			if (isWhite(n) || isSymbol(n)) {
				tokens.emplace_back(start + begin, i - begin + 1, Token::WORD, line, column);
				state = OUTER;
				continue;
			}

			if (isWord(n)) {
				continue;
			}

			throw ParseError {"Unexpected " + identify(n) + " in token", line, column};

		}

		// comment, emits no tokens
		// consumes characters until '*/' is found
		if (state == COMMENT) {

			if (c == '*' && n == '/') {
				state = OUTER;

				// skip the '/'
				i ++;
				column ++;
				continue;
			}

			// no throw here; we accept all character in comments
			continue;

		}

		// strings, emits string tokens
		// reads characters until and un-escaped '"' is found
		if (state == STRING) {

			if (c == '\\') {

				if (!isPrintable(n)) {
					throw ParseError {"Unexpected " + identify(n) + " in escape", line, column + 1};
				}

				// skip the escaped character
				i ++;
				column ++;
				continue;
			}

			if (c == '"') {
				tokens.emplace_back(start + begin, i - begin, Token::STRING, line, column);
				state = OUTER;
				continue;
			}

			// no new lines and null bytes in strings
			if (c != '\n' && c != '\0') {
				continue;
			}

			throw ParseError {"Unexpected " + identify(c) + " in string", line, column};

		}

	}

	return tokens;

}
