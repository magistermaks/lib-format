
#pragma once
#include <common/external.hpp>

class TextTreeValue : public TextTreeNode {

	public:

		static constexpr const char* name = "primitive";

		static inline const TextTreeValue* parse(TokenSpan span);

};

class TextTreeBool : public TextTreeValue {

	private:

		bool value;

	public:

		static constexpr const char* name = "bool";

		TextTreeBool(bool value)
		: value(value) {}

		operator bool() const {
			return value;
		}

};

class TextTreeString : public TextTreeValue {

	private:

		std::string_view value;

	public:

		static constexpr const char* name = "string";

		TextTreeString(std::string_view value)
		: value(value) {}

		operator std::string_view() const {
			return value;
		}

		std::string copy() const {
			return std::string {value};
		}

};

class TextTreeNumber : public TextTreeValue {

	private:

		double value;

	public:

		static inline bool tryParse(double* result, const std::string& string) {

			char* end;
			double value;

			errno = 0;
			value = strtod(string.c_str(), &end);

			if (errno == ERANGE && (value == HUGE_VAL || value == -HUGE_VAL)) {
				return false;
			}

			if (string.size() == 0 || *end != '\0') {
				return false;
			}

			*result = value;
			return true;
		}

	public:

		static constexpr const char* name = "number";

		TextTreeNumber(double value)
		: value(value) {}

		operator double() const {
			return value;
		}

		operator float() const {
			return (float) value;
		}

};

class TextTreeInt : public TextTreeNumber {

	private:

		long value;

		// otherwise the int conversion is ambiguous
		operator double() const = delete;
		operator float() const = delete;

	public:

		static inline bool tryParse(long* result, const std::string& string) {

			char* end;
			long value;

			errno = 0;
			value = strtol(string.c_str(), &end, 0);

			if (errno == ERANGE && (value == LONG_MAX || value == LONG_MIN)) {
				return false;
			}

			if (string.size() == 0 || *end != '\0') {
				return false;
			}

			*result = value;
			return true;
		}

	public:

		static constexpr const char* name = "integer";

		TextTreeInt(long value)
		: TextTreeNumber(value), value(value) {}

		operator long() const {
			return value;
		}

		operator int() const {
			return value;
		}

};
