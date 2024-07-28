
#include "file.hpp"

#ifdef _WIN32
#	include <windows.h>
#else
#	include <sys/mman.h>
#	include <fcntl.h>
#	include <unistd.h>
#endif

InputFile::InputFile(const std::string& path)
: file_data(nullptr), file_size(0) {

#ifdef _WIN32
	HANDLE file_handle = CreateFileA(path.c_str(), GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file_handle == INVALID_HANDLE_VALUE) {
		throw std::runtime_error {"CreateFileA: Failed to open file"};
	}

	LARGE_INTEGER size;
	if (!GetFileSizeEx(file_handle, &size)) {
		CloseHandle(file_handle);
		throw std::runtime_error {"GetFileSizeEx: Failed to get file size"};
	}

	// ah, how i hate windows
	this->file_size = static_cast<size_t>(size.QuadPart);
	HANDLE map_handle = CreateFileMapping(file_handle, NULL, PAGE_READONLY, 0, 0, NULL);

	if (!map_handle) {
		CloseHandle(file_handle);
		throw std::runtime_error {"CreateFileMapping: Failed to create file mapping"};
	}

	this->file_data = (uint8_t*) MapViewOfFile(map_handle, FILE_MAP_READ, 0, 0, file_size);
	CloseHandle(map_handle);
	CloseHandle(file_handle);

	if (!file_data) {
		throw std::runtime_error {"MapViewOfFile: Failed to map view of file"};
	}
#else
	int file = open(path.c_str(), O_RDONLY);

	if (file == -1) {
		throw std::runtime_error {"open: Failed open file"};
	}

	this->file_size = lseek(file, 0, SEEK_END);

	if (file_size == -1) {
		close(file);
		throw std::runtime_error {"lseek: Failed get file size"};
	}

	this->file_data = (uint8_t*) mmap(NULL, file_size, PROT_READ, MAP_SHARED, file, 0);
	close(file);

	if (file_data == MAP_FAILED) {
		throw std::runtime_error {"mmap: Failed to map view of file"};
	}
#endif

}

InputFile::~InputFile() {
	if (file_data) {
#ifdef _WIN32
		UnmapViewOfFile(file_data);
#else
		munmap(file_data, file_size);
#endif
	}

	file_data = nullptr;
}

const uint8_t* InputFile::data() const {
	return file_data;
}

size_t InputFile::size() const {
	return file_size;
}
