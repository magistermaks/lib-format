
#include "reader.hpp"

Reader::Reader(const void* base)
: base((uint8_t*) base), head((uint8_t*) base) {}

void Reader::jump(uint32_t offset) {
	this->head = this->base + offset;
}

void Reader::skip(uint32_t offset) {
	this->head += offset;
}

const void* Reader::ptr() const {
	return head;
}
