
#include "nodes.hpp"
#include "file.hpp"
#include <iostream>

#define PAD "\t"

void save(const char* path, std::vector<uint8_t>& output) {

	FILE* file = fopen(path, "wb");

	if (!file) {
		throw std::runtime_error {"Failed to open file"};
	}

	fwrite(output.data(), 1, output.size(), file);

//	// signature
//	uint8_t magic[4] = {0x00, 'B', 'T', 0xFF};
//	fwrite(magic, 1, 4, file);
//
//	// keychain length
//	uint32_t length = keychain.offset();
//	fwrite(&length, 1, 4, file);

	fclose(file);

}

void show(BinaryTreeNode node, std::string padding) {

	std::cout << node.name();

	if (node.type() == BinaryNode::INT) {
		std::cout << ": " << node.as<BinaryTreeInt>() << "\n";
		return;
	}

	if (node.type() == BinaryNode::FLOAT) {
		std::cout << ": " << node.as<BinaryTreeFloat>() << "\n";
		return;
	}

	if (node.type() == BinaryNode::DICT) {
		auto dict = node.as<BinaryTreeDict>();
		std::cout << ": (" << dict.size() << " entries)\n";

		for (auto [key, node] : dict) {
			std::cout << padding << "$" << key << " ";
			show(node, padding + PAD);
		}

		return;
	}

	if (node.type() == BinaryNode::TEXT) {
		std::cout << ": \"" << node.as<BinaryTreeText>().data() << "\"\n";
		return;
	}

	if (node.type() == BinaryNode::LIST) {
		auto array = node.as<BinaryTreeArray<BinaryTreeNode>>();
		std::cout << "<" << array.name() << ">: (" << array.size() << " entries)\n";

		int index = 0;

		for (auto node : array) {
			std::cout << padding << "#" << (index ++) << ": ";
			show(node, padding + PAD);
		}

		return;
	}

}

void write(const char* path) {

	SectionManager context;
	BinaryTreeNode::Writer root {&context, context.allocate()};

	auto dict1 = root.as<BinaryTreeDict>();
	dict1.put(1).as<BinaryTreeInt>(69);
	dict1.put(2).as<BinaryTreeInt>(420);

	auto dict2 = dict1.put(4).as<BinaryTreeDict>();
	dict2.put(0).as<BinaryTreeText>("Hello World!");
	dict2.put(1).as<BinaryTreeText>("Bit by bit into the abyss!");
	auto list1 = dict2.put(2).as<BinaryTreeArray<BinaryTreeText>>();

	list1.put("I see you're driven by your detestation");
	list1.put("Your every step is stoked with animus");
	list1.put("You need a different type of motivation");
	list1.put("Or there's no way that you can handle this");

	auto dict3 = dict1.put(3).as<BinaryTreeDict>();
	dict3.put(0).as<BinaryTreeInt>(0xAAAAAA);
	dict3.put(1).as<BinaryTreeInt>(0xBBBBBB);
	dict3.put(2).as<BinaryTreeInt>(0xCCCCCC);

	auto dict4 = dict1.put(4).as<BinaryTreeDict>();
	dict4.put(0).as<BinaryTreeInt>(0xAAAAAA);
	dict4.put(1).as<BinaryTreeInt>(0xBBBBBB);
	dict4.put(2).as<BinaryTreeInt>(0xCCCCCC);

	dict1.put(0x5).as<BinaryTreeText>("Bit by bit into the abyss!");

	std::vector<uint8_t> output;
	context.emit(output);

	save(path, output);
	printf("\n");

}

void read(const char* path) {

	InputFile file {path};

	Reader reader {file.data()};
	BinaryTreeNode node {reader};

	show(node, PAD);
	printf("\n");

}

int main() {

	write("test.bt");
	read("test.bt");

}
