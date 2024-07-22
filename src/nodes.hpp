
#pragma once
#include "external.hpp"

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
	if (node == BinaryTreeFloat::header) return "BinaryTreeFloat";
	if (node == BinaryTreeInt::header) return "BinaryTreeInt";
	if (node == BinaryTreeShort::header) return "BinaryTreeShort";

	// compounds
	if (node == BinaryTreeText::header) return "BinaryTreeText";
	if (node == BinaryTreeDict::header) return "BinaryTreeDict";
	if (node == BinaryTreeArray</* dummy */ BinaryTreeInt>::header) return "BinaryTreeArray";

	return "<UndefinedNode>";
}

constexpr uint32_t BinaryTreeNode::sizeOf(uint8_t node) {
	return node & 0x0f;
}
