
#pragma once
#include <common/external.hpp>

#include "error.hpp"

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

		/// raise parse error with a custom message
		[[noreturn]] void raise(const std::string& message) const;

		/// raise parse error of the "Expected X but got this" type
		[[noreturn]] void expected(const std::string& value) const;

	private:

		// helper method for creating a human readable character identifier
		static std::string identify(char chr);

		/// helper methods for the toknizer
		static bool isWhite(char chr);
		static bool isSymbol(char chr);
		static bool isJoint(char chr);
		static bool isPrintable(char chr);
		static bool isAlphaNumeric(char chr);
		static bool isWord(char chr);

	public:

		Token(const char* start, int length, Type type, int line, int column);

		/// returns a human readable type of this token, can be used for debbuging
		const char* name() const;

		/// returns a view at the data held by this token
		std::string_view view() const;

		/// a bit of a hacky method, used for testing for specific symbol tokens
		bool isSymbolEqual(char symbol) const;

		/// tokenizes the given string, may throw ParseError when there is some error in the input data
		/// or std::runtime_error if an internal parser error occures, the resulting token array should be
		/// then fed into the node parser, like for example `TextTreeDict::parse()`
		static std::vector<Token> tokenize(const char* start, int size);

};
