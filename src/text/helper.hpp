
#pragma once
#include <common/file.hpp>
#include "nodes.hpp"

struct TextTree {

	struct Input {

		private:

			InputFile file;
			const TextTreeDict* node;

		public:

			Input(const std::string& path)
			: file(path.c_str()) {
				std::vector<Token> tokens = Token::tokenize((const char*) file.data(), file.size());

				if (tokens.empty()) {
					throw ParseError {"Nothing to load", 1, 1};
				}

				// tokens must not be empty
				TokenSpan span {tokens};

				if (span.size() < 2) {
					span.last().expected("enclosing brace pair");
				}

				if (!span.get(0).isSymbolEqual('{')) {
					span.get(0).expected("'{'");
				}

				if (!span.last().isSymbolEqual('}')) {
					span.last().expected("'}'");
				}

				this->node = TextTreeDict::parse(span.unpack());
			}

			~Input() {
				delete node;
			}

			const TextTreeDict* root() {
				return node;
			}

	};

};
