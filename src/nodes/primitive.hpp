
#pragma once
#include "external.hpp"

template <typename T>
class BinaryTreePrimitive {

	public:

		class Writer {

			public:

				Writer(SectionManager* manager, SectionBuffer* buffer, T value) {
					buffer->write<T>(value);
				}

		};

	private:

		T value;

	public:

		BinaryTreePrimitive(Reader reader)
		: value(reader.read<T>()) {}

		operator T() const {
			return value;
		}

};

#define DefineTypeAdapter(name, type, node) struct name : public BinaryTreePrimitive<type> { \
	HEADER(node); name(Reader reader) : BinaryTreePrimitive(reader) {} \
}

DefineTypeAdapter(BinaryTreeDouble, double, BinaryNode::DOUBLE);
DefineTypeAdapter(BinaryTreeFloat, float, BinaryNode::FLOAT);
DefineTypeAdapter(BinaryTreeLong, uint64_t, BinaryNode::LONG);
DefineTypeAdapter(BinaryTreeInt, uint32_t, BinaryNode::INT);
DefineTypeAdapter(BinaryTreeShort, uint16_t, BinaryNode::SHORT);
DefineTypeAdapter(BinaryTreeByte, uint8_t, BinaryNode::BYTE);
