
#pragma once
#include "external.hpp"

struct WriteConfig {

	// controls whether to emit the BT header
	// into the output buffer
	bool include_header = true;

	// controls whether to try and eliminate
	// duplicated sections, needs the hash_bytes to also be set
	bool section_deduplication = true;

	// controls the number of bytes to include in
	// the section's hash value
	uint32_t hash_bytes = 20;

};

class SectionBuffer {

	private:

		struct Link {
			SectionBuffer* buffer;
			uint32_t offset;

			Link(SectionBuffer* buffer, uint32_t offset);
		};

		// used during emitting
		// contains the offset of this section within the
		// data of the combined file
		uint32_t offset = 0;
		uint64_t hashed = 0;

		std::vector<uint8_t> data;
		std::vector<Link> links;

		// FIXME
		friend class SectionCache;

	public:

		/// calculates the hash of this section, needs to be called after all the mutating calls like `write()` or `set()`
		void finalize(int bytes);

		/// checks if this section is equal to the data at offset in the output array
		bool equal(std::vector<uint8_t>& output, uint32_t offset);

	public:

		/// Returns the number of bytes in this section
		size_t size() const;

		/// Returns the hash of this section, needs to be called *after* `finalize()`
		size_t hash() const;

	public:

		/// Removes the last byte from the container
		void pop();

		/// Linkes the `other` section into this one, so that this offset will contain the offset of the other section
		void link(SectionBuffer* other);

		/// writes `size` bytes from `bytes` array
		void write(const void* bytes, size_t size);

		/// copies the `bytes` array into an alredy existing data at offset
		void set(uint32_t offset, const void* bytes, size_t size);

		/// copies the data from this section into the output buffer
		void emit(std::vector<uint8_t>& output);

		/// insert linkages to other sections, all data needs to be alredy emitted into the output
		void link(std::vector<uint8_t>& output);

	public:

		template <typename T, typename V>
		void write(V value) {
			write(&value, sizeof(T));
		}

};

class SectionCache {

	private:

		struct SectionInfo {

			uint32_t length = 0;
			uint32_t offset = 0;
			uint64_t hashed = 0;

			SectionInfo() = default;
			SectionInfo(SectionBuffer* buffer);

		};

		// each bucket containes only one cached value
		SectionInfo bucket[0xFF + 1];

	public:

		SectionCache();

		/// wrapped for the `buffer.emit()` call that tries to limit the number of those calls
		/// by mapping identical sections to the same memory range
		void emit(SectionBuffer* buffer, std::vector<uint8_t>& output);

};

class SectionManager {

	private:

		std::vector<SectionBuffer*> buffers;

	public:

		/// free buffers, maybe replace with std::unique_ptrs?
		~SectionManager();

		/// Returns a pointer to a newly allocated Section Buffer
		SectionBuffer* allocate();

		/// Emits all the stored data into the output vector in accordance with the WriteConfig
		void emit(std::vector<uint8_t>& output, const WriteConfig& config = {});

};
