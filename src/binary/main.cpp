
#include <binary/nodes.hpp>
#include <binary/header.hpp>
#include <common/file.hpp>

#include <iostream>

void save(const char* path, std::vector<uint8_t>& output) {

	FILE* file = fopen(path, "wb");

	if (!file) {
		throw std::runtime_error {"Failed to open file"};
	}

	fwrite(output.data(), 1, output.size(), file);
	fclose(file);

}

void padding(std::vector<bool>& flags) {

	if (flags.empty()) {
		return;
	}

	for (int i = 0; i < flags.size() - 1; i ++) {
		std::cout << (flags[i] ? "│  " : "   ");
	}

	std::cout << (flags.back() ? "├─ " : "└─ ");

}

void visit(BinaryTreeNode node, int depth, std::vector<bool> flags = {}) {

	std::cout << node.name();

	if (node.type() == BinaryNode::LONG) {
		std::cout << " " << node.as<BinaryTreeLong>() << "\n";
		return;
	}

	if (node.type() == BinaryNode::INT) {
		std::cout << " " << node.as<BinaryTreeInt>() << "\n";
		return;
	}

	if (node.type() == BinaryNode::SHORT) {
		std::cout << " " << node.as<BinaryTreeShort>() << "\n";
		return;
	}

	if (node.type() == BinaryNode::BYTE) {
		std::cout << " " << (int) node.as<BinaryTreeByte>() << "\n";
		return;
	}

	if (node.type() == BinaryNode::DOUBLE) {
		std::cout << " " << node.as<BinaryTreeDouble>() << "\n";
		return;
	}

	if (node.type() == BinaryNode::FLOAT) {
		std::cout << " " << node.as<BinaryTreeFloat>() << "\n";
		return;
	}

	if (node.type() == BinaryNode::DICT) {
		auto dict = node.as<BinaryTreeDict>();
		std::cout << " (" << dict.size() << " entries)\n";

		int counter = dict.size();

		if (flags.size() > depth) {
			std::vector<bool> child {flags};
			child.push_back(false);

			padding(child);
			std::cout << "(... " << dict.size() << " more)\n";
			return;
		}

		for (auto [key, node] : dict) {
			std::vector<bool> child {flags};
			child.push_back(-- counter);

			padding(child);
			std::cout << "" << key << " ";
			visit(node, depth, child);
		}

		return;
	}

	if (node.type() == BinaryNode::TEXT) {
		std::cout << " \"" << node.as<BinaryTreeText>().data() << "\"\n";
		return;
	}

	if (node.type() == BinaryNode::LIST) {
		auto array = node.as<BinaryTreeArray<BinaryTreeNode>>();
		std::cout << "<" << array.name() << "> (" << array.size() << " entries)\n";

		int counter = array.size();

		if (flags.size() > depth) {
			std::vector<bool> child {flags};
			child.push_back(false);

			padding(child);
			std::cout << "(... " << array.size() << " more)\n";
			return;
		}

		int i = 0;

		for (auto node : array) {
			std::vector<bool> child {flags};
			child.push_back(-- counter);

			padding(child);
			std::cout << "" << (i ++) << " ";
			visit(node, depth, child);
		}

		return;
	}

}

bool make(const std::string& path) {

	SectionManager context;
	BinaryTreeNode::Writer root {&context, context.allocate()};

	auto dict1 = root.as<BinaryTreeDict>();
	dict1.put(1).as<BinaryTreeDouble>(213.7);
	dict1.put(2).as<BinaryTreeFloat>(423.6);

	auto dict2 = dict1.put(4).as<BinaryTreeDict>();
	dict2.put(0).as<BinaryTreeText>("Hello World!");
	dict2.put(1).as<BinaryTreeText>("Bit by bit into the abyss!");
	auto list1 = dict2.put(2).as<BinaryTreeArray<BinaryTreeText>>();

	list1.put("I see you're driven by your detestation");
	list1.put("Your every step is stoked with animus");
	list1.put("You need a different type of motivation");
	list1.put("Or there's no way that you can handle this");

	auto dict3 = dict1.put(3).as<BinaryTreeDict>();
	dict3.put(0).as<BinaryTreeLong>(0xAAAAAAAAAAAAAAA);
	dict3.put(1).as<BinaryTreeInt>(0xBBBBBBB);
	dict3.put(2).as<BinaryTreeShort>(0xCCC);
	dict3.put(3).as<BinaryTreeByte>(0xD);

	auto dict4 = dict1.put(8).as<BinaryTreeDict>();
	dict4.put(0).as<BinaryTreeLong>(0xAAAAAAAAAAAAAAA);
	dict4.put(1).as<BinaryTreeInt>(0xBBBBBBB);
	dict4.put(2).as<BinaryTreeShort>(0xCCC);
	dict4.put(3).as<BinaryTreeByte>(0xD);

	dict1.put(0x5).as<BinaryTreeText>("Bit by bit into the abyss!");

	std::vector<uint8_t> output;
	context.emit(output);

	save(path.c_str(), output);

	std::cout << "File generated, it contain at least one instance of each node type.\n";
	std::cout << "You can display it using 'bt show " << path << "'\n";
	return 0;
}

bool info(const std::string& path) {

	InputFile file {path.c_str()};

	if (file.size() < 12) {
		std::cout << "Not a valid BT file, expected at least 12 bytes! Aborting...\n";
		return 1;
	}

	Reader reader {file.data()};
	BinaryTreeHeader header {reader};

	std::cout << "Size             : " << file.size() << " bytes (" << (file.size() - 12) << " bytes of data)\n";
	std::cout << "Version    +0x04 : BT v" << (int) header.version << "\n";
	std::cout << "Endianness +0x05 : " << (header.endian ? "little-endian" : "big-endian") << "\n";
	std::cout << "Flags      +0x06 : " << header.flags << "\n";
	std::cout << "Root       +0x08 : 0x" << std::hex << header.offset << std::dec << "\n";

	if (!header.readable()) {
		std::cout << "Encoding differs, this file can't be read! Aborting...\n";
		return 1;
	}

	return 0;
}

bool tree(const std::string& path) {

	InputFile file {path.c_str()};

	Reader reader {file.data()};
	BinaryTreeHeader header {reader};

	if (!header.readable()) {
		std::cout << "Encoding differs, this file can't be read! Aborting...\n";
		return 1;
	}

	BinaryTreeNode node {reader};

	visit(node, 3);
	return 0;
}


bool show(const std::string& path) {
	int a = info(path);

	if (a) {
		return a;
	}

	std::cout << "\n";
	return tree(path);
}


const char* endianness() {
	if constexpr (std::endian::native == std::endian::big) {
		return "big-endian";
	}

	if constexpr (std::endian::native == std::endian::little) {
		return "little-endian";
	}

	return "mixed-endian";
}

int usage(bool hint) {
	std::cout << "Usage: bt [mode] [file]\n";

	if (hint) {
		std::cout << "Try 'bt help' for more details\n";
	}

	return 1;
}

int help() {
	usage(false);
	std::cout << "A helper utility for the BinaryTree file format\n\n";

	std::cout << "Modes:\n";
	std::cout << "   version     : Show version and exit\n";
	std::cout << "   help        : Show this page and exit\n";
	std::cout << "   info [file] : Show basic stats about a BT file\n";
	std::cout << "   tree [file] : Show the structure stored in given file\n";
	std::cout << "   show [file] : Show a combination of the 'stat' and 'tree' modes\n";
	std::cout << "   make [file] : Generate an example file and exit\n\n";

	return 0;
}

int version() {
	std::cout << "BT v" << BT_VERSION << " (" << endianness() << ")\n";
}

int main(int argc, char** argv) {

	// +1 to skip program name
	std::vector<std::string> args(argv + 1, argv + argc);

	if (args.size() == 1 && args[0] == "help") {
		return help();
	}

	if (args.size() == 1 && args[0] == "version") {
		return version();
	}

	if (args.size() == 2 && args[0] == "info") {
		return info(args[1]);
	}

	if (args.size() == 2 && args[0] == "tree") {
		return tree(args[1]);
	}

	if (args.size() == 2 && args[0] == "show") {
		return show(args[1]);
	}

	if (args.size() == 2 && args[0] == "make") {
		return make(args[1]);
	}

	usage(true);
	return 1;

}
