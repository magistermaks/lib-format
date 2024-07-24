
#include <common/external.hpp>
#include <common/file.hpp>

#include <iostream>

class ParseError {

	private:

		std::string message;
		int line;
		int column;

	public:

		ParseError(const std::string& message, int line, int column)
		: message(message), line(line), column(column) {}

		void print(const std::string& unit) {
			std::cout << unit << ":" << line << ":" << column << ": " << message + "\n";
		}

};

struct Token {

	public:

		enum Type {
			WORD,
			STRING,
			SYMBOL
		};

		const char* start;
		const char* end;
		Type type;
		int line;
		int column;

	public:

		[[noreturn]] void raise(const std::string& message) const {
			throw ParseError {message, line, column};
		}

		[[noreturn]] void expected(const std::string& value) const {
			throw ParseError {"Expected " + value + " but got: '" + std::string {start, (size_t) (end - start)} + "'", line, column};
		}

	private:

		static inline std::string identify(char chr) {
			if (chr >= ' ' && chr <= '~') {
				return std::string("'") + chr + "'";
			}

			if (chr == '\0') return "nul byte (\'\\0\')";
			if (chr == '\n') return "new line (\'\\n\')";
			if (chr == '\t') return "tab (\'\\t\')";
			if (chr == '\r') return "carriage return (\'\\n\')";

			if (chr >= 127) return "ascii+" + std::to_string((int) chr);
		}

		static inline bool isWhite(char chr) {
			return chr == ' ' || chr == '\n' || chr == '\t' || chr == '\r';
		}

		static inline bool isSymbol(char chr) {
			return chr == '[' || chr == ']' || chr == '{' || chr == '}' || chr == ',';
		}

		static inline bool isJoint(char chr) {
			return chr == '_' || chr == '-' || chr == '.';
		}

		static inline bool isPrintable(char chr) {
			return chr >= ' ' && chr <= '~';
		}

		static inline bool isAlphaNumeric(char chr) {
			return (chr >= '0' && chr <= '9') || (chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z');
		}

		static inline bool isWord(char chr) {
			return isAlphaNumeric(chr) || isJoint(chr);
		}

	public:

		Token(const char* start, int length, Type type, int line, int column)
		: start(start), end(start + length), type(type), line(line), column(column) {}

		void print() const {
			std::cout << "Token " << name() << " for span: '" << view() << "' at line: " << line << "\n";
		}

		const char* name() const {
			if (type == WORD) return "word";
			if (type == STRING) return "string";
			if (type == SYMBOL) return "symbol";

			return "error";
		}

		std::string_view view() const {
			return {start, end - start};
		}

		bool isSymbolEqual(char symbol) const {
			return start[0] == symbol;
		}

		static inline std::vector<Token> tokenize(const char* start, int size) {

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

};

class TokenSpan {

	private:

		const Token& last_token;
		const std::vector<Token>& tokens;
		const int start, end;

		[[noreturn]] void raise(const std::string& message) const {
			throw std::runtime_error {"TokenSpan: " + message};
		}

	public:

		TokenSpan(const std::vector<Token>& tokens)
		: TokenSpan(tokens, 0, tokens.size()) {}

		TokenSpan(const std::vector<Token>& tokens, int start, int end)
		: tokens(tokens), start(start), end(end), last_token(end == 0 ? tokens.at(0) : tokens.at(end - 1)) {}

		void print() const {
			std::cout << "TokenSpan for span:\n";

			for (int i = start; i < end; i ++) {
				std::cout << " * ";
				tokens[i].print();
			}
		}

	public:

		const Token& last() const {
			return last_token;
		}

		const Token& get(int index) const {
			return tokens.at(start + index);
		}

		int size() const {
			return end - start;
		}

		bool empty() const {
			return size() == 0;
		}

		TokenSpan sub(int from, int to) const {
			int child_start = start + from;
			int child_end = start + to;

			// sanity checks
			if (child_start < start) raise("outside lower bound");
			if (child_end > end) raise("outside upper bound");

			return {tokens, child_start, child_end};
		}

		TokenSpan unpack() const {
			if (size() < 2) {
				raise("unable to unpack");
			}

			return {tokens, start + 1, end - 1};
		}

};

class TokenParser {

	private:

		TokenSpan span;
		int index;

		int countUntilBalanced(char open, char close) {
			int depth = 1;
			int count = 0;

			while (depth > 0) {
				const Token& token = nextToken();
				count ++;

				if (token.isSymbolEqual(open)) {
					depth ++;
				}

				if (token.isSymbolEqual(close)) {
					depth --;
				}
			}

			return count;
		}

	public:

		TokenParser(TokenSpan span)
		: span(span), index(0) {}

		int remaining() const {
			return span.size() - index;
		}

		const Token& nextToken() {
			if (index < span.size()) {
				return span.get(index ++);
			}

			span.last().raise("Unexpected end of scope");
		}

		TokenSpan nextValue() {
			int start = index;
			int length = 1;

			const Token& first = nextToken();

			if (first.type == Token::SYMBOL) {
				     if (first.isSymbolEqual('[')) length += countUntilBalanced('[', ']');
				else if (first.isSymbolEqual('{')) length += countUntilBalanced('{', '}');
				else first.expected("property value");
			}

			return span.sub(start, start + length);
		}

		void consumeSeparator() {
			if (remaining() == 0) {
				return;
			}

			const Token& token = span.get(index);

			if (token.type == Token::SYMBOL && token.isSymbolEqual(',')) {
				index ++;
				return;
			}

			// new line can replace a separator
			if (index > 0 && span.get(index - 1).line != token.line) {
				return;
			}

			token.expected("','");
		}

};

#include "node.hpp"

int main() {

	InputFile file {"test.tt"};

	try {
		std::vector<Token> tokens = Token::tokenize((const char*) file.data(), file.size());
		TokenSpan span {tokens};
		//TokenParser parser {span.unpack()};

		//parser.parseObject();

		const TextTreeDict* dict = TextTreeDict::parse(span.unpack());

//		bool monospaced = dict->get("monospaced")->any<>();

//		if (auto node = dict->get<TextTreeInteger>("test_float")) {
			float v = *dict->get<TextTreeNumber>("test_float");
			std::cout << "monospaced! v=" << v << "\n";
//		} else {
//			std::cout << ":(\n";
//		}

		delete dict;

	} catch (ParseError error) {
		error.print("test.tt");
	}

}
