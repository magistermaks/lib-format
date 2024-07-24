
#pragma once
#include <common/external.hpp>

class ParseError {

	public:

		const std::string message;
		const int line;
		const int column;

	public:

		ParseError(const std::string& message, int line, int column);

		/// print the error in a readable form to the standard output
		void print(const std::string& unit) const;

};
