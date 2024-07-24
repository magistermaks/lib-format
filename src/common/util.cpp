
#include "util.hpp"

void padding(std::vector<bool>& flags) {

	if (flags.empty()) {
		return;
	}

	for (int i = 0; i < flags.size() - 1; i ++) {
		std::cout << (flags[i] ? "│  " : "   ");
	}

	std::cout << (flags.back() ? "├─ " : "└─ ");

}
