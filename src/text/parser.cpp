
#include "parser.hpp"

int TokenParser::countUntilBalanced(char open, char close) {
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

TokenParser::TokenParser(TokenSpan span)
: span(span), index(0) {}

int TokenParser::remaining() const {
	return span.size() - index;
}

const Token& TokenParser::nextToken() {
	if (index < span.size()) {
		return span.get(index ++);
	}

	span.last().raise("Unexpected end of scope");
}

TokenSpan TokenParser::nextValue() {
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

void TokenParser::consumeSeparator() {
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
