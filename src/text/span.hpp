
#pragma once
#include <common/external.hpp>

#include "token.hpp"

class TokenSpan {

	private:

		const Token& last_token;
		const std::vector<Token>& tokens;
		const int start, end;

		/// raises an internal parser error
		[[noreturn]] void raise(const std::string& message) const;

	public:

		TokenSpan(const std::vector<Token>& tokens);
		TokenSpan(const std::vector<Token>& tokens, int start, int end);

	public:

		/// for error reporting, returns the last token in this span, this is done even if the span is empty
		const Token& last() const;

		/// get token at the specific index
		const Token& get(int index) const;

		/// get the number of tokens in this span
		int size() const;

		/// returns true if this span contains no tokens
		bool empty() const;

		/// returns a subspan of this span starting including 'from' but excluding 'to'
		TokenSpan sub(int from, int to) const;

		/// returns a new span that omits the first and last token
		TokenSpan unpack() const;

};
