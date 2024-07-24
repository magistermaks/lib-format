
#pragma once
#include <common/external.hpp>

template <typename T>
class BinaryTreePrimitive {

	public:

		class Writer {

			public:

				template <typename A> requires std::integral<A> || (std::floating_point<A> && std::floating_point<T>)
				Writer(SectionManager* manager, SectionBuffer* buffer, A value) {

					if constexpr (std::is_floating_point<A>::value || std::is_floating_point<T>::value) {
						buffer->write<T>((T) value);
						return;
					}

					if constexpr (sizeof(A) == sizeof(T)) {
						buffer->write<T>(std::bit_cast<T>(value));
						return;
					}

					buffer->write<T>(static_cast<T>(value));

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
DefineTypeAdapter(BinaryTreeLong, int64_t, BinaryNode::LONG);
DefineTypeAdapter(BinaryTreeInt, int32_t, BinaryNode::INT);
DefineTypeAdapter(BinaryTreeShort, int16_t, BinaryNode::SHORT);
DefineTypeAdapter(BinaryTreeByte, int8_t, BinaryNode::BYTE);
