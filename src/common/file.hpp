
#pragma once
#include "external.hpp"

class InputFile {

	private:

		uint8_t* file_data;
		size_t file_size;

	public:

		InputFile(const std::string& path);
		~InputFile();

		const uint8_t* data() const;
		size_t size() const;

};
