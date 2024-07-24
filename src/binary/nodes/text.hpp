
#pragma once
#include <common/external.hpp>

class BinaryTreeText {

	public:

		class Writer {

			public:

				SectionBuffer* writer;

				Writer(SectionManager* manager, SectionBuffer* buffer)
				: writer(manager->allocate()) {
					buffer->link(writer);
				}

				Writer(SectionManager* manager, SectionBuffer* buffer, std::string_view value)
				: Writer(manager, buffer) {
					append(value);
				}

				Writer& append(std::string_view value) {
					writer->pop();
					writer->write(value.data(), value.size());
					writer->write<uint8_t>(0);
					return *this;
				}

		};

	private:

		Reader reader;

	public:

		HEADER(BinaryNode::TEXT);

		BinaryTreeText(Reader head)
		: reader(head) {
			reader.jump(reader.read<uint32_t>());
		}

	public:

		const char* data() const {
			return reinterpret_cast<const char*>(reader.ptr());
		}

		std::string_view view() const {
			return {data()};
		}

		std::string copy() const {
			return {data()};
		}

};
