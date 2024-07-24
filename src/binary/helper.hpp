
#pragma once
#include <common/file.hpp>
#include "nodes.hpp"
#include "header.hpp"

struct BinaryTree {

	class Input {

		private:

			InputFile file;
			Reader reader;

		public:

			Input(const std::string& path)
			: file(path.c_str()) {

				Reader reader {file.data()};
				BinaryTreeHeader header {reader};

				if (!header.readable()) {
					throw std::runtime_error {"Unsupported encoding"};
				}

				this->reader = reader;
			}

			BinaryTreeNode root() {
				return {reader};
			}

	};

};
