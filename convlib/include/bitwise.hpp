#pragma once
#include <TokenRedux.hpp>

#include <queue>
#include <regex>

namespace bitwise {
	enum class Lexeme : unsigned char {
		NONE,
		OR,
		AND,
		XOR,
		NEGATE,
		DIGIT,
		PERIOD,
		SUBTRACT,
		BRACKET_OPEN,
		BRACKET_CLOSE,
		LETTER,
		QUOTE,
		WHITESPACE,
		END,
	};

	struct LexemeDict : token::base::LexemeDictBase<Lexeme> {
		Lexeme char_to_lexeme(const char& c) const noexcept override
		{
			if (std::isdigit(c))
				return Lexeme::DIGIT;
			else if (std::isalpha(c) || c == '_') // allow underscores to be letters
				return Lexeme::LETTER;
			else if (std::isspace(c))
				return Lexeme::WHITESPACE;
			switch (c) {
			case EOF: return Lexeme::END;
			case '\'': [[fallthrough]];
			case '\"': return Lexeme::QUOTE;
			case '.': return Lexeme::PERIOD;
			case '(': return Lexeme::BRACKET_OPEN;
			case ')': return Lexeme::BRACKET_CLOSE;
			case '|': return Lexeme::OR;
			case '&': return Lexeme::AND;
			case '^': return Lexeme::XOR;
			case '~': return Lexeme::NEGATE;
			case '-': return Lexeme::SUBTRACT;
			default:  return Lexeme::NONE;
			}
		}
	};

	enum class TokenType : unsigned char {
		NONE,
		END,
		OR,
		AND,
		XOR,
		NEGATE,
		SUBTRACT,
		ENCLOSED, // with brackets
		BINARY,
		OCTAL,
		DECIMAL,
		HEXADECIMAL,
	};

	inline std::string tokenTypeToString(const TokenType& t)
	{
		switch (t) {
		case TokenType::AND:
			return "AND";
		case TokenType::OR:
			return "OR";
		case TokenType::XOR:
			return "XOR";
		case TokenType::NEGATE:
			return "NOT";
		case TokenType::BINARY:
			return "Binary";
		case TokenType::DECIMAL:
			return "Decimal";
		case TokenType::HEXADECIMAL:
			return "Hexadecimal";
		case TokenType::ENCLOSED:
			return "Bracket Content";
		case TokenType::SUBTRACT:
			return "NEGATIVE";
		default:
			return{};
		}
	}

	inline std::ostream& operator<<(std::ostream& os, const TokenType& t)
	{
		return os << tokenTypeToString(t);
	}

	using Token = token::base::TokenBase<TokenType>;

	class Tokenizer : token::base::TokenizerBase<Lexeme, LexemeDict, TokenType, Token> {
		using regex = std::basic_regex<char>;
		regex literal_or{ "[oO][rR]", std::regex_constants::ECMAScript | std::regex_constants::optimize };
		regex literal_xor{ "[xX][oO][rR]", std::regex_constants::ECMAScript | std::regex_constants::optimize };
		regex literal_and{ "[aA][nN][dD]", std::regex_constants::ECMAScript | std::regex_constants::optimize };
		regex literal_not{ "[nN][oO][tT]", std::regex_constants::ECMAScript | std::regex_constants::optimize };

		Token getNextToken(const char& c) override
		{
			switch (get_lexeme(c)) {
			case Lexeme::SUBTRACT: [[fallthrough]];
			case Lexeme::PERIOD: [[fallthrough]];
			case Lexeme::DIGIT: {
				std::string num{ c + getsimilar(Lexeme::DIGIT, Lexeme::LETTER, Lexeme::PERIOD) };

				if (str::startsWith(num, "0b"))
					return Token{ TokenType::BINARY, num };
				else if (str::startsWith(num, '\\'))
					return Token{ TokenType::OCTAL, num };
				else if (str::startsWith(num, "0x"))
					return Token{ TokenType::HEXADECIMAL, num };
				else
					return Token{ TokenType::DECIMAL, num };
			}
			case Lexeme::BRACKET_OPEN:
				return Token{ TokenType::ENCLOSED, getBrackets(Lexeme::BRACKET_OPEN, Lexeme::BRACKET_CLOSE) };
			case Lexeme::BRACKET_CLOSE:
				throw make_exception("bitwise::Tokenizer::getNext() error:  Unmatched closing bracket \'", c, "\' at position ", getCurrentPos(), '/', getStreamSize(), '!');
			case Lexeme::OR:
				return Token{ TokenType::OR, c };
			case Lexeme::XOR:
				return Token{ TokenType::XOR, c };
			case Lexeme::AND:
				return Token{ TokenType::AND, c };
			case Lexeme::NEGATE:
				return Token{ TokenType::NEGATE, c };
			case Lexeme::LETTER: {
				std::string word{ c + getsimilar(Lexeme::LETTER) };

				if (std::regex_match(word, literal_or))
					return Token{ TokenType::OR, word };
				else if (std::regex_match(word, literal_xor))
					return Token{ TokenType::XOR, word };
				else if (std::regex_match(word, literal_and))
					return Token{ TokenType::AND, word };
				else if (std::regex_match(word, literal_not))
					return Token{ TokenType::NEGATE, word };
				else [[fallthrough]];
			}
			case Lexeme::END:
				return Token{ TokenType::END, c };
			case Lexeme::NONE:
				throw make_exception("bitwise::Tokenizer::getNextToken() failed:  Illegal character '", (c == '\0' ? "(NULL)" : "" + c), "' at position ", getCurrentPos(), '/', getStreamSize(), '!');
			default:
				if (hasMore())
					return getNextToken(getch()); // recurse until a valid token is found
				else return Token{ TokenType::END };
			}
			throw make_exception("bitwise::Tokenizer::getNextToken() failed:  No valid token type detected for character '", c, "' at position ", getCurrentPos(), '/', getStreamSize(), '!');
		}
	public:
		Tokenizer(std::stringstream&& ss) : TokenizerBase<Lexeme, LexemeDict, TokenType, Token>(std::move(ss), Lexeme::WHITESPACE, Lexeme::END) {}
		Tokenizer(std::vector<std::string> const& strvec) : TokenizerBase<Lexeme, LexemeDict, TokenType, Token>(std::move([](auto&& strvec) -> std::stringstream { std::stringstream ss; for (const auto& s : strvec) ss << ' ' << s; return std::move(ss); }(strvec)), Lexeme::WHITESPACE, Lexeme::END) {}
		/**
		 * @brief				Tokenize overload that provides the EOF token for you.
		 * @param reserve_sz	Expand the vector's capacity by this number of elements each time the capacity limit is reached.
		 * @returns				std::vector<TokenT>
		 */
		[[nodiscard]] std::vector<TokenT> tokenize(const size_t& reserve_sz = 64ull)
		{
			return TokenizerBase::tokenize(TokenType::END, reserve_sz);
		}
	};

	inline long long parse(std::string const& str, TokenType const& type) noexcept(false)
	{
		switch (type) {
		case TokenType::BINARY:
			return str::toBase10(str, 2);
		case TokenType::DECIMAL:
			return str::stoll(str);
		case TokenType::HEXADECIMAL:
			return str::toBase10(str, 16);
		default:
			throw make_exception("Invalid token type received \"", tokenTypeToString(type), '\"');
		}
	}

	enum class Operator : char {
		NONE = '\0',
		AND = '&',
		OR = '|',
		XOR = '^',
		NEGATE = '~',
	};
	inline std::ostream& operator<<(std::ostream& os, const Operator& op)
	{
		return os << static_cast<char>(op);
	}

	namespace exceptions {
		struct invalid_operation_except : ex::except {
			Operator op;
			int expected, actual;
			std::string message;

			invalid_operation_except(const Operator& op, const int& expected_operand_count, const int& actual_operand_count) : op{ op }, expected{ expected_operand_count }, actual{ actual_operand_count } {}
			invalid_operation_except(const Operator& op, const int& expected_operand_count, const std::string& error) : op{ op }, expected{ expected_operand_count }, actual{ 0 }, message{ error } {}

			virtual void format() const noexcept override
			{
				auto& msg{ *get_message() };
				msg += str::stringify("Operator ", op, " requires ", expected, " operand", (expected > 1 ? "s" : ""), ", but ");
				if (actual == 0)
					msg += message;
				else
					msg += str::stringify(actual, ' ', (actual != 1 ? "were" : "was"), " given!");
			}
		};
	}

	/// @brief	Represents a numerical operand.
	using operand = long long;

	/**
	 * @struct	operation
	 * @brief	Recursive bitwise operation object that contains a single bitwise expression.
	 *\n		Valid Bitwise Expression Types:
	 *			- Negation Operation
	 *			  - 1x operand / sub-operation
	 *			  - 1x Operator
	 *			    _Must be set to __Operator::NEGATE___
	 *			- Operand
	 *			  - 1x operand
	 *			  - 1x Operator
	 *			    _Must be set to __Operator::NONE___
	 *			- Normal Operation
	 *			  - 2x operand / sub-operation
	 *			  - 1x Operator
	 *			    _May be any Operator type except for __Operator::NONE__ & __Operator::NEGATE___
	 */
	struct operation {
		using suboperation = std::unique_ptr<operation>;
		using variant_t = std::variant<std::monostate, suboperation, operand>;

		/// @brief	The operator type. This determines which bitwise operation is performed.
		Operator type;
		/// @brief	The left-side operand/sub-operation. This is never allowed to be null.
		variant_t left;
		/// @brief	The right-side operand/sub-operation. This may be null only in negation operations.
		variant_t right;

		/**
		 * @brief	Default Constructor
		 */
		operation() : type{ Operator::NONE }, left{ std::monostate{} }, right{ std::monostate{} } {}

		/**
		 * @brief			Normal Operation Constructor
		 * @param type		Operator Type.
		 * @param left		Left-side operand or suboperation.
		 * @param right		Right-side operand or suboperation.
		 */
		operation(const Operator& type, variant_t&& left, variant_t&& right) : type{ type }, left{ std::move(left) }, right{ std::move(right) }
		{
			if (type == Operator::NONE || type == Operator::NEGATE)
				throw make_custom_exception<exceptions::invalid_operation_except>(type, 1, 2);

			if (const bool leftNull{ !hasv(this->left) }, rightNull{ !hasv(this->right) }; leftNull && rightNull)
				throw make_custom_exception<exceptions::invalid_operation_except>(type, 2, "both were null!");
			else if (rightNull)
				throw make_custom_exception<exceptions::invalid_operation_except>(type, 2, "the right operand was null!");
			else if (leftNull)
				throw make_custom_exception<exceptions::invalid_operation_except>(type, 2, "the left operand was null!");
		}

		/**
		 * @brief			Normal Operation Constructor
		 * @param left		Left-side operand or suboperation.
		 * @param type		Operator Type.
		 * @param right		Right-side operand or suboperation.
		 */
		operation(variant_t&& left, const Operator& type, variant_t&& right) : operation(type, std::move(left), std::move(right)) {}

		/**
		 * @brief			Negation Operation / Operand Constructor
		 * @param value		Left-side operand or suboperation.
		 * @param type		Operator Type.
		 * @throws			exceptions::invalid_operation_except invalid parameters for a bitwise negation operation.
		 */
		operation(const Operator& type, variant_t&& value) : type{ type }, left{ std::move(value) }, right{ std::monostate{} }
		{
			if (type != Operator::NONE && type != Operator::NEGATE)
				throw make_custom_exception<exceptions::invalid_operation_except>(type, 2, 1);
			if (!hasv(left))
				throw make_custom_exception<exceptions::invalid_operation_except>(type, 1, "the left operand was null!");
			if (hasv(right))
				throw make_custom_exception<exceptions::invalid_operation_except>(type, 1, 2);
		}

		/**
		 * @brief			Negation Operation / Operand Constructor
		 * @param type		Operator Type. Must be Operator::NEGATE.
		 * @param value		The operand or suboperation to negate.
		 */
		operation(variant_t&& value, const Operator& type) : operation(type, std::move(value)) {}

		/**
		 * @brief		Check if the given variant type contains an operand or operation.
		 *\n			If the given variant is std::monostate (null), an exception is *not* thrown.
		 * @param v		Input variant type.
		 * @returns		bool
		 *				true:	The given variant is an operand or operation type.
		 *				false:	The given variant is std::monostate.
		 */
		static constexpr bool hasv(const variant_t& v) noexcept
		{
			return v.index() != 0;
		}
		static constexpr bool is_suboperation_t(const variant_t& v) noexcept
		{
			return v.index() == 1;
		}
		static constexpr bool is_operand_t(const variant_t& v) noexcept
		{
			return v.index() == 2;
		}

		/**
		 * @brief				Get the resulting numerical value from the given variant type. If the given variant is std::monostate, an exception is thrown.
		 *\n					If the given variant is an operand type, the operand's value is returned.
		 *\n					If the given variant is an operation type, the result of the operation is returned by calling operation::result().
		 * @param v				Input variant type.
		 * @returns				operand
		 * @throws ex::except	Variant type was set to std::monostate
		 */
		static operand get(const variant_t& v) noexcept(false)
		{
			if (auto* value = std::get_if<operand>(&v))
				return *value;
			else if (auto* oper = std::get_if<suboperation>(&v)) {
				auto* oper_ptr{ oper->get() };
				if (oper_ptr == nullptr)
					throw make_exception("operation::get() failed:  Operation pointer was null!");
				return oper_ptr->result();
			}
			throw make_exception("operation::get() failed:  Received null variant type!");
		}

		/**
		 * @brief
		 * @returns	bool
		 */
		bool isOperand() const
		{
			return (type == Operator::NONE) && hasv(left) && !hasv(right);
		}

		/**
		 * @brief	Calculate this operation and retrieve the result.
		 * @returns	operand
		 */
		virtual operand result() const noexcept(false)
		{
			if (!hasv(left))
				throw make_exception("operation::result() failed:  Left operand is null!");

			operand l{ get(left) };

			if (isOperand())
				return l;

			switch (type) {
			case Operator::AND: {
				if (!hasv(right))
					throw make_exception("operation::result() failed:  Cannot perform ", type, " operation when right operand is null!");
				return (l & get(right));
			}
			case Operator::OR: {
				if (!hasv(right))
					throw make_exception("operation::result() failed:  Cannot perform ", type, " operation when right operand is null!");
				return (l | get(right));
			}
			case Operator::XOR: {
				if (!hasv(right))
					throw make_exception("operation::result() failed:  Cannot perform ", type, " operation when right operand is null!");
				return (l ^ get(right));
			}
			case Operator::NEGATE:
				if (hasv(right))
					throw make_exception("operation::result() failed:  Cannot perform ", type, " operation when right operand is not null!");
				return ~l;
			case Operator::NONE:
				if (hasv(right))
					throw make_exception("operation::result() failed:  Cannot perform ", type, " operation when right operand is not null!");
				return l;
			default:break;
			}
			throw make_exception("operation::result() failed:  Invalid operator ", type);
		}
	};

	class Parser : token::base::TokenParserBase<operation, Token> {
	public:
		Parser(std::stringstream&& ss) : TokenParserBase<operation, Token>(std::move(Tokenizer(std::move(ss)).tokenize())) {}
		Parser(const std::string& str) : Parser(std::move(std::stringstream{ str })) {}

		OutputT parse() const
		{
			// create a buffer for values
			using std::optional;
			optional<operation::variant_t> l, r;
			optional<Operator> type;
			bool negateNext{ false };

			const auto& push{ [&](auto&& v) {
				// first check if there is a full operation present in the buffer
				if (l.has_value() && r.has_value() && type.has_value()) {
					l = std::move(std::make_unique<operation>(type.value(), std::move(l.value()), std::move(r.value())));
					r = std::nullopt;
					type = std::nullopt;
				}
				if (!l.has_value())
					l = std::move(v);
				else if (!r.has_value())
					r = std::move(v);
				else throw make_exception("Too many operands!");
			} };

			bool doBreak{ false };
			for (auto it{ tokens.begin() }; !doBreak && it != tokens.end(); ++it) {
				switch (it->type) {
				case TokenType::END:
					doBreak = true;
					break;
				case TokenType::NEGATE:
					negateNext = !negateNext;
					break;
				case TokenType::AND:
					if (type.has_value())
						throw make_exception("Too many operators!");
					else if (negateNext)
						throw make_exception("Cannot negate an operator!");
					type = Operator::AND;
					break;
				case TokenType::OR:
					if (type.has_value())
						throw make_exception("Too many operators!");
					else if (negateNext)
						throw make_exception("Cannot negate an operator!");
					type = Operator::OR;
					break;
				case TokenType::XOR:
					if (type.has_value())
						throw make_exception("Too many operators!");
					else if (negateNext)
						throw make_exception("Cannot negate an operator!");
					type = Operator::XOR;
					break;
				case TokenType::BINARY: {
					std::string s{ it->str.substr((str::startsWith(it->str, "0b") ? 2ull : 0ull)) };
					operand value{ std::move(str::toBase10(std::move(s), 2)) };
					if (negateNext) {
						push(std::move(std::make_unique<operation>(Operator::NEGATE, std::move(value))));
						negateNext = false;
					}
					else push(value);
					break;
				}
				case TokenType::HEXADECIMAL: {
					std::string s{ it->str.substr((str::startsWith(it->str, "0x") ? 2ull : 0ull)) };
					operand value{ std::move(str::toBase10(std::move(s), 16)) };
					if (negateNext) {
						push(std::move(std::make_unique<operation>(Operator::NEGATE, std::move(value))));
						negateNext = false;
					}
					else push(value);
					break;
				}
				case TokenType::DECIMAL: {
					operand value{ std::move(str::stoll(it->str)) };
					if (negateNext) {
						push(std::move(std::make_unique<operation>(Operator::NEGATE, std::move(value))));
						negateNext = false;
					}
					else push(std::move(value));
					break;
				}
				case TokenType::ENCLOSED: {
					std::unique_ptr<operation> oper{ std::make_unique<operation>(std::move(Parser(it->str).parse())) };
					if (negateNext) {
						push(std::make_unique<operation>(Operator::NEGATE, std::move(oper)));
						negateNext = false;
					}
					else push(std::move(oper));
					break;
				}
				case TokenType::NONE:
					break;
				default:
					throw make_exception("Unrecognized token type: \"", typeid(it->type).name(), "!");
				}
			}

			if (!type.has_value()) {
				if (l.has_value() && operation::is_suboperation_t(l.value()))
					return std::move(*std::get<operation::suboperation>(l.value()).get());
				else throw make_exception("No operator specified!");
			}

			if (r.has_value())
				return std::move(operation{ type.value(), std::move(l.value()), std::move(r.value()) });
			return std::move(operation{ type.value(), std::move(l.value()) });
		}
	};

	/**
	 * @brief			Parse a single bitwise expression into a calculable operation.
	 * @param input		Any input type allowed by the Parser object's constructor.
	 * @returns			operation
	 */
	inline operation parse(auto&& input)
	{
		try {
			return std::move(Parser(std::forward<decltype(input)>(input)).parse());
		} catch (const ex::except& ex) {
			throw make_exception(
				"An exception occurred while parsing a bitwise expression!\n",
				indent(10), "Expression:  '", std::forward<decltype(input)>(input), "'\n",
				indent(10), "Exception:   '", ex.what(), '\''
			);
		}
	}
}

