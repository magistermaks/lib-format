
#include "span.hpp"

[[noreturn]] void TokenSpan::raise(const std::string& message) const {
	throw std::runtime_error {"TokenSpan: " + message};
}

TokenSpan::TokenSpan(const std::vector<Token>& tokens)
: TokenSpan(tokens, 0, tokens.size()) {}

TokenSpan::TokenSpan(const std::vector<Token>& tokens, int start, int end)
: tokens(tokens), start(start), end(end), last_token(end == 0 ? tokens.at(0) : tokens.at(end - 1)) {}

const Token& TokenSpan::last() const {
	return last_token;
}

const Token& TokenSpan::get(int index) const {
	return tokens.at(start + index);
}

int TokenSpan::size() const {
	return end - start;
}

bool TokenSpan::empty() const {
	return size() == 0;
}

TokenSpan TokenSpan::sub(int from, int to) const {
	int child_start = start + from;
	int child_end = start + to;

	// sanity checks
	if (child_start < start) raise("outside lower bound");
	if (child_end > end) raise("outside upper bound");

	return {tokens, child_start, child_end};
}

TokenSpan TokenSpan::unpack() const {
	if (size() < 2) {
		raise("unable to unpack");
	}

	return {tokens, start + 1, end - 1};
}
