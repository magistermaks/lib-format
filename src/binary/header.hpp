
#pragma once
#include <common/external.hpp>

#include "reader.hpp"

#define BT_VERSION 1

class BinaryTreeHeader {

	private:

		uint8_t endianness() const {
			if constexpr (std::endian::native == std::endian::big) return 0x00;
			if constexpr (std::endian::native == std::endian::little) return 0xFF;

			return 0x11;
		}

	public:

		uint8_t version;
		uint8_t endian;
		uint16_t flags;
		uint32_t offset;

		static constexpr uint8_t signature[4] = {0x00, 'B', 'T', 0xFF};
		static constexpr size_t size = 12;

	public:

		BinaryTreeHeader(Reader& reader) {

			// check file magic bytes
			if (memcmp(reader.ptr(), signature, 4) != 0) {
				throw std::runtime_error {"Invalid file signature"};
			}

			reader.skip(4);

			this->version = reader.read<uint8_t>();
			this->endian = reader.read<uint8_t>();

			uint8_t first = reader.read<uint8_t>();
			uint8_t second = reader.read<uint8_t>();
			this->flags = (first << 8) | second; // big endian

			this->offset = reader.read<uint32_t>();
			reader.jump(this->offset);

		}

		BinaryTreeHeader(uint16_t flags, uint32_t offset) {

			this->version = BT_VERSION;
			this->endian = endianness();
			this->flags = flags;
			this->offset = offset;

		}

	public:

		void emit(std::vector<uint8_t>& buffer) const {
			buffer.insert(buffer.end(), signature, signature + 4);
			buffer.push_back(this->version);
			buffer.push_back(this->endian);
			buffer.push_back(this->flags >> 8);
			buffer.push_back(this->flags & 0xFF);
			buffer.insert(buffer.end(), (uint8_t*) &this->offset, (uint8_t*) &this->offset + 4);
		}

		bool readable() const {
			return (this->version == BT_VERSION) && (this->endian == endianness());
		}

};
