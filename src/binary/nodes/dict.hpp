
#pragma once
#include <common/external.hpp>

class BinaryTreeDict {

	public:

		class Writer {

			private:

				uint8_t count;
				SectionManager* manager;
				SectionBuffer* writer;

			public:

				Writer(SectionManager* manager, SectionBuffer* buffer)
				: count(0), manager(manager), writer(manager->allocate()) {
					writer->write<uint8_t>(0);
					buffer->link(writer);
				}

				BinaryTreeNode::Writer put(uint16_t key) {
					if (count == 0xFF) {
						throw std::runtime_error {"Unable to add another key, maximum dictionary capacity reached"};
					}

					count ++;
					writer->set(0, &count, 1);
					writer->write<uint16_t>(key);
					return {manager, writer};
				}

		};

	public:

		class Iterator {

			private:

				Reader reader;
				uint16_t key;
				uint32_t remaining;

			public:

				using iterator_category = std::forward_iterator_tag;
				using value_type = std::pair<uint16_t, BinaryTreeNode>;
				using difference_type = std::ptrdiff_t;
				using pointer = value_type*;
				using reference = value_type&;

				Iterator(Reader reader, uint32_t count)
				: reader(reader), remaining(count) {}

				bool operator==(const Iterator& other) const {
					return remaining == other.remaining;
				}

				bool operator!=(const Iterator& other) const {
					return !(*this == other);
				}

				value_type operator*() const {
					Reader value {reader};
					uint16_t key = value.read<uint16_t>();
					return {key, value};
				}

				// pre-increment
				Iterator& operator++() {
					if (remaining > 0) {
						reader.skip(2);
						reader.skip(BinaryTreeNode::sizeOf(reader.read<uint8_t>()));
						remaining --;
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

		uint32_t count;
		Reader reader;

	public:

		HEADER(BinaryNode::DICT);

		BinaryTreeDict(Reader head)
		: reader(head) {
			reader.jump(reader.read<uint32_t>());
			count = reader.read<uint8_t>();
		}

	public:

		int size() const {
			return count;
		}

		BinaryTreeNode get(uint16_t key) {
			for (auto [entry, node] : *this) {
				if (key == entry) return node;
			}

			throw std::runtime_error {"Expected key: " + std::to_string(key) + ", but it was not found in the dictionary"};
		}

		bool has(uint16_t key) {
			for (auto [entry, node] : *this) {
				if (key == entry) return true;
			}

			return false;
		}

		Iterator begin() {
			return {reader, count};
		}

		Iterator end() {
			return {reader, 0};
		}

};
