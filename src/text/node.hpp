
class TextTreeNode {

	protected:

		static inline const TextTreeNode* parseValue(TokenParser& parser);

	public:

		static constexpr const char* name = "any";

		virtual ~TextTreeNode() = default;

		template <typename T> requires (std::derived_from<T, TextTreeNode>)
		const T* as() {
			return dynamic_cast<const T*>(this);
		}

};

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

		auto begin() {
			return nodes.begin();
		}

		auto end() {
			return nodes.end();
		}


};

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

		auto begin() {
			return nodes.begin();
		}

		auto end() {
			return nodes.end();
		}

};

class TextTreeValue : public TextTreeNode {

	public:

		static constexpr const char* name = "primitive";

		static inline const TextTreeValue* parse(TokenSpan span);

};

class TextTreeBool : public TextTreeValue {

	private:

		bool value;

	public:

		static constexpr const char* name = "bool";

		TextTreeBool(bool value)
		: value(value) {}

		operator bool() const {
			return value;
		}

};

class TextTreeString : public TextTreeValue {

	private:

		std::string_view value;

	public:

		static constexpr const char* name = "string";

		TextTreeString(std::string_view value)
		: value(value) {}

		operator std::string_view() const {
			return value;
		}

		std::string copy() const {
			return std::string {value};
		}

};

class TextTreeNumber : public TextTreeValue {

	private:

		double value;

	public:

		static inline bool tryParse(double* result, const std::string& string) {

			char* end;
			double value;

			errno = 0;
			value = strtod(string.c_str(), &end);

			if (errno == ERANGE && (value == HUGE_VAL || value == -HUGE_VAL)) {
				return false;
			}

			if (string.size() == 0 || *end != '\0') {
				return false;
			}

			*result = value;
			return true;
		}

	public:

		static constexpr const char* name = "number";

		TextTreeNumber(double value)
		: value(value) {}

		operator double() const {
			return value;
		}

		operator float() const {
			return (float) value;
		}

		explicit operator int() const {
			return (int) value;
		}

};

class TextTreeInteger : public TextTreeNumber {

	private:

		long value;

	public:

		static inline bool tryParse(long* result, const std::string& string) {

			char* end;
			long value;

			errno = 0;
			value = strtol(string.c_str(), &end, 0);

			if (errno == ERANGE && (value == LONG_MAX || value == LONG_MIN)) {
				return false;
			}

			if (string.size() == 0 || *end != '\0') {
				return false;
			}

			*result = value;
			return true;
		}

	public:

		static constexpr const char* name = "integer";

		TextTreeInteger(long value)
		: TextTreeNumber(value), value(value) {}

		operator bool() const {
			return value;
		}

		operator int() const {
			return value;
		}

};

const TextTreeNode* TextTreeNode::parseValue(TokenParser& parser) {

	TokenSpan span = parser.nextValue();

	if (span.size() == 1) {
		return TextTreeValue::parse(span);
	}

	if (span.size() >= 2) {
		TokenSpan unpacked = span.unpack();

		if (span.get(0).isSymbolEqual('[')) {
			return TextTreeList::parse(unpacked);
		}

		if (span.get(0).isSymbolEqual('{')) {
			return TextTreeDict::parse(unpacked);
		}
	}

	// this shouldn't ever happen
	throw std::runtime_error {"Failed to parse value"};

}

const TextTreeValue* TextTreeValue::parse(TokenSpan span) {
	const Token& token = span.get(0);

	bool isTrue = (token.view() == "true");
	bool isFalse = (token.view() == "false");

	if (isTrue || isFalse) {
		return new TextTreeBool(isTrue);
	}

	if (token.type == Token::STRING) {
		return new TextTreeString(token.view().substr(1, token.view().size() - 2));
	}

	double dv;
	long lv;

	std::string copy (token.view());

	if (TextTreeInteger::tryParse(&lv, copy)) {
		return new TextTreeInteger(lv);
	}

	if (TextTreeNumber::tryParse(&dv, copy)) {
		return new TextTreeNumber(dv);
	}

	// this can happen
	token.expected("valid primitive value");
}
