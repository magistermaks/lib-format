
#pragma once
#include "external.hpp"

class BinaryTreeNode {

	public:

		class Writer {

			private:

				SectionManager* manager;
				SectionBuffer* buffer;

			public:

				Writer(SectionManager* manager, SectionBuffer* buffer)
				: manager(manager), buffer(buffer) {}

				template <typename T, typename... Args>
				inline typename T::Writer as(Args... args) {
					buffer->write<uint8_t>(T::header);
					return {manager, buffer, args...};
				}

		};

	public:

		static constexpr const char* nameOf(uint8_t node);
		static constexpr uint32_t sizeOf(uint8_t node);

	private:

		uint8_t node;
		Reader reader;

	public:

		BinaryTreeNode(Reader reader, uint8_t node)
		: node(node), reader(reader) {}

		BinaryTreeNode(Reader reader)
		: node(reader.read<uint8_t>()), reader(reader) {}

		template <typename T>
		inline bool is() {
			return node == T::header;
		}

		template <typename T>
		inline T as() {
			if (!is<T>()) throw std::runtime_error {std::string {"Expected node type: "} + nameOf(T::header) + ", but got: " + nameOf(node)};
			return {reader};
		}

		inline uint8_t type() {
			return node;
		}

		inline const char* name() {
			return nameOf(node);
		}

};
