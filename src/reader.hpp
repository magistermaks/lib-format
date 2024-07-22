
#pragma once
#include "external.hpp"

class Reader {

	private:

		const uint8_t* base; // start of the data region
		const uint8_t* head; // current location

	public:

		Reader(const void* base);

		/// Move to the specified offset within the data array
		void jump(uint32_t offset);

		/// Skip `offset` bytes forward within the data array
		void skip(uint32_t offset);

		/// Get the pointer to the *current* location withing the data
		const void* ptr() const;

	public:

		/// read the type `T` from the underlying data array
		template <typename T>
		T read() {
			T value = *reinterpret_cast<const T*>(head);
			head += sizeof(T);
			return value;
		}

};
