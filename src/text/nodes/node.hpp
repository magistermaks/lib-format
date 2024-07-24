
#pragma once
#include <common/external.hpp>

class TextTreeNode {

	protected:

		static inline const TextTreeNode* parseValue(TokenParser& parser);

	public:

		static constexpr const char* name = "any";

		virtual ~TextTreeNode() = default;

		template <std::derived_from<TextTreeNode> T = TextTreeNode>
		const T* as() const {
			return dynamic_cast<const T*>(this);
		}

};
