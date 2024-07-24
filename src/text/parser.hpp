
#pragma once
#include <common/external.hpp>

#include "span.hpp"

class TokenParser {

	private:

		TokenSpan span;
		int index;

		/// returns the distance from the opening bracket to the equallibrium
		int countUntilBalanced(char open, char close);

	public:

		TokenParser(TokenSpan span);

		/// returns the number of remaining tokens
		int remaining() const;

		/// get next single token
		const Token& nextToken();

		/// get next value scope, so either a single token or a []/{} enclosed scope
		TokenSpan nextValue();

		/// validates that a next value or name-value-pair can be read
		void consumeSeparator();

};
