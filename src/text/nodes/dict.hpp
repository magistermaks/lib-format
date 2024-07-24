
#pragma once
#include <common/external.hpp>

class TextTreeDict : public TextTreeNode {

	private:

		std::unordered_map<std::string_view, std::unique_ptr<const TextTreeNode>> nodes;

	public:

		static constexpr const char* name = "dictionary";

		static inline const TextTreeDict* parse(TokenSpan span) {

			TokenParser parser {span};
			TextTreeDict* dict = new TextTreeDict;

			while (parser.remaining() > 0) {

				const Token& name = parser.nextToken();
				std::string_view key = name.view();

				if (name.type != Token::WORD) {
					name.expected("property name");
				}

				if (dict->nodes.contains(key)) {
					name.expected("unique property name");
				}

				dict->nodes.emplace(key, parseValue(parser));
				parser.consumeSeparator();
			}

			return dict;

		}

	public:

		int size() const {
			return nodes.size();
		}

		template <std::derived_from<TextTreeNode> T = TextTreeNode>
		const T* getNullable(std::string_view key) const {
			auto it = nodes.find(key);

			if (it != nodes.end()) {
				return dynamic_cast<const T*>(it->second.get());
			}

			return nullptr;
		}

		template <std::derived_from<TextTreeNode> T = TextTreeNode>
		const T* get(std::string_view key) const {
			const T* node = getNullable<T>(key);

			if (!node) {
				throw std::runtime_error {"Expected key: '" + std::string(key) + "' of " + T::name + " type, but it was not found in the dictionary"};
			}

			return node;
		}

		auto begin() const {
			return nodes.begin();
		}

		auto end() const {
			return nodes.end();
		}


};
