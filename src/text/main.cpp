
#include <common/external.hpp>
#include <common/file.hpp>
#include <common/util.hpp>

#include "nodes.hpp"

void visit(const TextTreeNode* node, int depth, std::vector<bool> flags = {}) {

	if (auto dict = node->as<TextTreeDict>()) {
		int count = dict->size();
		std::cout << "Dictionary (" << count << " entries)\n";

		for (auto& [key, value] : *dict) {
			std::vector<bool> child {flags};
			child.push_back(-- count);

			padding(child);
			std::cout << key << " ";
			visit(value.get(), depth, child);
		}
	}

	if (auto list = node->as<TextTreeList>()) {
		int count = list->size();
		int index = 0;
		std::cout << "List (" << count << " entries)\n";

		for (auto& value : *list) {
			std::vector<bool> child {flags};
			child.push_back(-- count);

			padding(child);
			std::cout << (index ++) << " ";
			visit(value.get(), depth, child);
		}
	}

	if (auto self = node->as<TextTreeInt>()) {
		std::cout << "Int " << (long) *self << "\n";
		return;
	}

	if (auto self = node->as<TextTreeNumber>()) {
		std::cout << "Number " << (double) *self << "\n";
		return;
	}

	if (auto self = node->as<TextTreeBool>()) {
		std::cout << "Bool " << ((bool) *self ? "true" : "false") << "\n";
		return;
	}

	if (auto self = node->as<TextTreeString>()) {
		std::cout << "String \"" << (std::string_view) *self << "\"\n";
		return;
	}

}

int tree(const std::string& path) {

	InputFile file {path.c_str()};

	try {
		std::vector<Token> tokens = Token::tokenize((const char*) file.data(), file.size());
		TokenSpan span {tokens};

		auto root = TextTreeDict::parse(span.unpack());
		visit(root, 30);
		delete root;

	} catch (ParseError error) {
		error.print(path);
		return 1;
	}

	return 0;
}

int usage(bool hint) {
	std::cout << "Usage: tt [mode] [file]\n";

	if (hint) {
		std::cout << "Try 'tt help' for more details\n";
	}

	return 1;
}

int help() {
	usage(false);
	std::cout << "A helper utility for the TextTree file format\n\n";

	std::cout << "Modes:\n";
	std::cout << "   help        : Show this page and exit\n";
	std::cout << "   tree [file] : Show the structure stored in given file\n";
	std::cout << "   show [file] : Alias for the 'tree' mode\n";

	return 0;
}

int main(int argc, char** argv) {

	// +1 to skip program name
	std::vector<std::string> args(argv + 1, argv + argc);

	if (args.size() == 1 && args[0] == "help") {
		return help();
	}

	if (args.size() == 2 && args[0] == "tree") {
		return tree(args[1]);
	}

	// for compatiblity with BT
	if (args.size() == 2 && args[0] == "show") {
		return tree(args[1]);
	}

	usage(true);
	return 1;

}
