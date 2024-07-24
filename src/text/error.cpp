
#include "error.hpp"

ParseError::ParseError(const std::string& message, int line, int column)
: message(message), line(line), column(column) {}

void ParseError::print(const std::string& unit) const {
	std::cout << unit << ":" << line << ":" << column << ": " << message + "\n";
}
