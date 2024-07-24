
#include "writer.hpp"
#include "header.hpp"

/*
 * SectionBuffer
 */

SectionBuffer::Link::Link(SectionBuffer* buffer, uint32_t offset)
: buffer(buffer), offset(offset) {}

void SectionBuffer::finalize(int bytes) {
	this->hashed = std::hash<std::string_view>()(std::string_view {(char*) data.data(), (size_t) std::min(bytes, (int) data.size())});
}

bool SectionBuffer::equal(std::vector<uint8_t>& output, uint32_t offset) {
	return memcmp(output.data() + offset, data.data(), data.size()) == 0;
}

size_t SectionBuffer::size() const {
	return data.size();
}

size_t SectionBuffer::hash() const {
	return hashed;
}

void SectionBuffer::pop() {
	if (!data.empty()) {
		data.pop_back();
	}
}

void SectionBuffer::link(SectionBuffer* other) {
	links.emplace_back(other, data.size());
	write<uint32_t>(0);
}

void SectionBuffer::write(const void* bytes, size_t size) {
	data.insert(data.end(), (const uint8_t*) bytes, ((const uint8_t*) bytes) + size);
}

void SectionBuffer::set(uint32_t offset, const void* bytes, size_t size) {
	int source = 0;

	while (offset < data.size() && source < size) {
		data[offset] = ((const uint8_t*) bytes)[source];

		source ++;
		offset ++;
	}
}

void SectionBuffer::emit(std::vector<uint8_t>& output) {
	this->offset = output.size();
	output.insert(output.end(), data.begin(), data.end());
}

void SectionBuffer::link(std::vector<uint8_t>& output) {
	uint8_t* self = output.data() + offset;

	for (Link link : links) {
		uint8_t* segment = self + link.offset;
		memcpy(segment, &link.buffer->offset, 4);
	}
}

/*
 * Section Cache
 */

SectionCache::SectionInfo::SectionInfo(SectionBuffer* buffer)
: length(buffer->size()), offset(buffer->offset), hashed(buffer->hash()) {}

SectionCache::SectionCache() {

	// make sure initialy all lookups will miss initialy
	// by making the full hash differ from the bucked offset
	for (int i = 0; i <= 0xFF; i ++) {
		bucket[i].hashed = ~i;
	}
}

void SectionCache::emit(SectionBuffer* buffer, std::vector<uint8_t>& output) {
	uint64_t hash = buffer->hash();

	// don't cache sections with links
	// links can differ or otherwise identical data
	if (buffer->links.empty()) {
		SectionInfo& info = bucket[hash & 0xFF];

		// verify if the full hash and length match
		if (info.hashed == hash && info.length == buffer->size()) {

			// actually comapre the data in the sections
			if (buffer->equal(output, info.offset)) {
				buffer->offset = info.offset;
				printf("Cache hit @%d, skipped %d bytes\n", info.offset, info.length);
				return;
			} else {
				printf("Cache near-miss @%d\n", info.offset);
			}
		}
	}

	// cache miss, write the buffer and add to cache
	buffer->emit(output);
	bucket[hash & 0xFF] = {buffer};
}

/*
 * SectionManager
 */

SectionManager::~SectionManager() {
	for (SectionBuffer* buffer : buffers) {
		delete buffer;
	}
}

SectionBuffer* SectionManager::allocate() {
	SectionBuffer* buffer = new SectionBuffer;
	buffers.push_back(buffer);
	return buffer;
}

void SectionManager::emit(std::vector<uint8_t>& output, const WriteConfig& config) {

	size_t total = 0;

	if (config.include_header) {
		BinaryTreeHeader header {0x00, 0x0C};
		header.emit(output);

		total += BinaryTreeHeader::size;
	}

	for (SectionBuffer* buffer : buffers) {
		total += buffer->size();
		buffer->finalize(config.hash_bytes);
	}

	output.reserve(total);
	SectionCache cache;

	for (SectionBuffer* buffer : buffers) {
		cache.emit(buffer, output);
	}

	for (SectionBuffer* buffer : buffers) {
		buffer->link(output);
	}

}
