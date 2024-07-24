
#pragma once
#include <common/external.hpp>

#include "parser.hpp"

#include "nodes/node.hpp"
#include "nodes/primitive.hpp"
#include "nodes/list.hpp"
#include "nodes/dict.hpp"

const TextTreeNode* TextTreeNode::parseValue(TokenParser& parser) {

	TokenSpan span = parser.nextValue();

	if (span.size() == 1) {
		return TextTreeValue::parse(span);
	}

	if (span.size() >= 2) {
		TokenSpan unpacked = span.unpack();

		if (span.get(0).isSymbolEqual('[')) {
			return TextTreeList::parse(unpacked);
		}

		if (span.get(0).isSymbolEqual('{')) {
			return TextTreeDict::parse(unpacked);
		}
	}

	// this shouldn't ever happen
	throw std::runtime_error {"Failed to parse value"};

}

const TextTreeValue* TextTreeValue::parse(TokenSpan span) {
	const Token& token = span.get(0);

	bool isTrue = (token.view() == "true");
	bool isFalse = (token.view() == "false");

	if (isTrue || isFalse) {
		return new TextTreeBool(isTrue);
	}

	if (token.type == Token::STRING) {
		return new TextTreeString(token.view());
	}

	double dv;
	long lv;

	std::string copy (token.view());

	if (TextTreeInt::tryParse(&lv, copy)) {
		return new TextTreeInt(lv);
	}

	if (TextTreeNumber::tryParse(&dv, copy)) {
		return new TextTreeNumber(dv);
	}

	// this can happen
	token.expected("valid primitive value");
}
