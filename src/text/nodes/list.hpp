
#pragma once
#include <common/external.hpp>

class TextTreeList : public TextTreeNode {

	private:

		std::vector<std::unique_ptr<const TextTreeNode>> nodes;

	public:

		static constexpr const char* name = "list";

		static inline const TextTreeList* parse(TokenSpan span) {

			TokenParser parser {span};
			TextTreeList* list = new TextTreeList;

			while (parser.remaining() > 0) {
				list->nodes.emplace_back(parseValue(parser));
				parser.consumeSeparator();
			}

			return list;
		}

	public:

		int size() const {
			return nodes.size();
		}

		template <std::derived_from<TextTreeNode> T = TextTreeNode>
		const T* getNullable(int index) const {
			if (index < nodes.size()) {
				return dynamic_cast<const T*>(nodes[index].get());
			}

			return nullptr;
		}

		template <std::derived_from<TextTreeNode> T = TextTreeNode>
		const T* get(int index) const {
			const T* node = getNullable<T>(index);

			if (!node) {
				throw std::runtime_error {"Expected index: " + std::to_string(index) + " of " + T::name + " type, but it was outside list bounds"};
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
