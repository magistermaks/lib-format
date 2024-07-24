
#pragma once
#include <common/external.hpp>

// don't use
#include "writer.hpp"
#include "reader.hpp"
#include "types.hpp"

#include "nodes/node.hpp"
#include "nodes/primitive.hpp"
#include "nodes/text.hpp"
#include "nodes/array.hpp"
#include "nodes/dict.hpp"

constexpr const char* BinaryTreeNode::nameOf(uint8_t node) {

	// primitives
	if (node == BinaryNode::DOUBLE) return "Double";
	if (node == BinaryNode::FLOAT) return "Float";
	if (node == BinaryNode::LONG) return "Long";
	if (node == BinaryNode::INT) return "Int";
	if (node == BinaryNode::SHORT) return "Short";
	if (node == BinaryNode::BYTE) return "Byte";

	// compounds
	if (node == BinaryNode::TEXT) return "Text";
	if (node == BinaryNode::DICT) return "Dictionary";
	if (node == BinaryNode::LIST) return "Array";

	return "Undefined";
}

constexpr uint32_t BinaryTreeNode::sizeOf(uint8_t node) {
	return node & 0x0f;
}
