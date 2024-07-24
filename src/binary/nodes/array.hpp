
#pragma once
#include <common/external.hpp>

template <typename T>
class BinaryTreeArray {

	public:

		class Writer {

			private:

				uint32_t count;
				SectionManager* manager;
				SectionBuffer* writer;

			public:

				Writer(SectionManager* manager, SectionBuffer* buffer)
				: count(0), manager(manager), writer(manager->allocate()) {
					writer->write<uint32_t>(0);
					writer->write<uint8_t>(T::header);
					buffer->link(writer);
				}

				template <typename... Args>
				inline typename T::Writer put(Args... args) {
					count ++;
					writer->set(0, &count, 4);
					return {manager, writer, args...};
				}

		};

	public:

		class Iterator {

			private:

				uint8_t node;
				Reader reader;
				uint32_t remaining;
				uint32_t stride;

			public:

				using iterator_category = std::forward_iterator_tag;
				using value_type = T;
				using difference_type = std::ptrdiff_t;
				using pointer = value_type*;
				using reference = value_type&;

				Iterator(Reader reader, uint32_t count, uint32_t stride, uint8_t node)
				: reader(reader), remaining(count), stride(stride), node(node) {}

				bool operator==(const Iterator& other) const {
					return remaining == other.remaining;
				}

				bool operator!=(const Iterator& other) const {
					return !(*this == other);
				}

				value_type operator*() const {
					if constexpr (std::is_same_v<T, BinaryTreeNode>) return {reader, node}; else return {reader}; // TODO
				}

				// pre-increment
				Iterator& operator++() {
					if (remaining > 0) {
						reader.skip(stride);
						--remaining;
					}
					return *this;
				}

				// post-increment
				Iterator operator++(int) {
					Iterator tmp = *this;
					++(*this);
					return tmp;
				}

		};

	private:

		uint8_t node;
		uint32_t stride;
		uint32_t count;
		Reader reader;

	public:

		HEADER(BinaryNode::LIST);

		BinaryTreeArray(Reader head)
		: reader(head) {
			reader.jump(reader.read<uint32_t>());
			count = reader.read<uint32_t>();
			node = reader.read<uint8_t>();
			stride = BinaryTreeNode::sizeOf(node);

			// array can be templated with a BinaryTreeNode to iterate in a generic way
			if constexpr (!std::is_same_v<T, BinaryTreeNode>) {
				if (node != T::header) {
					throw std::runtime_error {std::string {"Expected array type: "} + BinaryTreeNode::nameOf(T::header) + ", but got: " + BinaryTreeNode::nameOf(node)};
				}
			}
		}

	public:

		int size() const {
			return count;
		}

		int type() const {
			return node;
		}

		const char* name() const {
			return BinaryTreeNode::nameOf(node);
		}

		Iterator begin() {
			return {reader, count, stride, node};
		}

		Iterator end() {
			Reader after {reader};
			after.skip(stride * count);
			return {after, 0, stride, node};
		}

};
