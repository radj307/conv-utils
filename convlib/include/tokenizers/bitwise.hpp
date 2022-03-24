#pragma once
#include <TokenRedux.hpp>

#include <queue>


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
		COMMA,
		WHITESPACE,
	};

	struct LexemeDict : token::base::LexemeDictBase<Lexeme> {
		Lexeme char_to_lexeme(const char& c) const noexcept override
		{
			if (std::isdigit(c))
				return Lexeme::DIGIT;
			else if (std::isalpha(c) || c == '_') // allow underscores to be letters
				return Lexeme::LETTER;
			switch (c) {
			case ',': return Lexeme::COMMA;
			case '.': return Lexeme::PERIOD;
			case '(': return Lexeme::BRACKET_OPEN;
			case ')': return Lexeme::BRACKET_CLOSE;
			case '|': return Lexeme::OR;
			case '&': return Lexeme::AND;
			case '^': return Lexeme::XOR;
			case '!': [[fallthrough]];
			case '~': return Lexeme::NEGATE;
			case '-': return Lexeme::SUBTRACT;
			case '\t': [[fallthrough]];
			case '\v': [[fallthrough]];
			case '\r': [[fallthrough]];
			case '\n': [[fallthrough]];
			case ' ': return Lexeme::WHITESPACE;
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
		Token getNextToken(const char& c) override
		{
			std::string s; //< used to pass data between case labels when falling through

			switch (get_lexeme(c)) {
			case Lexeme::OR:
				return Token{ TokenType::OR, c };
			case Lexeme::AND:
				return Token{ TokenType::AND, c };
			case Lexeme::XOR:
				return Token{ TokenType::XOR, c };
			case Lexeme::NEGATE:
				return Token{ TokenType::NEGATE, c };
			case Lexeme::LETTER: {
				s = c + getsimilar(Lexeme::LETTER, Lexeme::DIGIT);
				if (s.size() > 1ull && s.size() <= 6ull) {
					std::string slc{ str::tolower(s) };
					if (str::startsWith(slc, "bit"))
						slc = slc.substr(3ull);
					if (s.size() == 2ull && slc == "or")
						return Token{ TokenType::OR, s };
					else if (slc == "and")
						return Token{ TokenType::AND, s };
					else if (slc == "xor")
						return Token{ TokenType::XOR, s };
					else if (slc == "not")
						return Token{ TokenType::NEGATE, s };
				} // else:
				[[fallthrough]];
			}
			case Lexeme::SUBTRACT:
			case Lexeme::PERIOD:
			case Lexeme::DIGIT: {
				if (s.empty()) // control did not fall through from LETTER.
					s = c + getnotsimilar(Lexeme::WHITESPACE, Lexeme::OR, Lexeme::AND, Lexeme::XOR, Lexeme::NEGATE);
				if (s.empty())
					break;

				if (str::isbinary(s))
					return Token{ TokenType::BINARY, s };
				else if (str::isdecimal(s))
					return Token{ TokenType::DECIMAL, s };
				else if (str::ishex(s))
					return Token{ TokenType::HEXADECIMAL, s };
				else throw make_exception("bitwise::Tokenizer::getNext():  Input sequence \"", s, "\" doesn't match any known operators or numbers!");
			}
			case Lexeme::BRACKET_OPEN:
				return Token{ TokenType::ENCLOSED, getBrackets(Lexeme::BRACKET_OPEN, Lexeme::BRACKET_CLOSE) };
			case Lexeme::BRACKET_CLOSE:
				throw make_exception("bitwise::Tokenizer::getNext():  Unmatched closing bracket \'", c, "\'!");
			default:
				break;
			}
			return Token::NullToken;
		}
	public:
		Tokenizer(std::stringstream&& ss) : TokenizerBase<Lexeme, LexemeDict, TokenType, Token>(std::move(ss), Lexeme::WHITESPACE) {}
		Tokenizer(std::vector<std::string> const& strvec) : TokenizerBase<Lexeme, LexemeDict, TokenType, Token>(std::move([](auto&& strvec) -> std::stringstream { std::stringstream ss; for (const auto& s : strvec) ss << ' ' << s; return std::move(ss); }(strvec)), Lexeme::WHITESPACE) {}
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

	/**
	 * @struct	Operand
	 * @brief	An object that represents a numerical number in a specific base.
	 */
	struct Operand {
		bool negated;
		long long value;
		std::string original_string;

		Operand(std::string const& str, TokenType const& type, const bool& negated = false) : value{ parse(str, type) }, negated{ negated }, original_string{ str } {}
		Operand(Token const& token, const bool& negated = false) : value{ parse(token.str, token.type) }, negated{ negated }, original_string{ token.str } {}

		operator long long() const { return value; }

		long long operator&(const Operand& o) const
		{
			return (negated ? ~value : value) & (o.negated ? ~o.value : o.value);
		}
		long long operator|(const Operand& o) const
		{
			return (negated ? ~value : value) | (o.negated ? ~o.value : o.value);
		}
		long long operator^(const Operand& o) const
		{
			return (negated ? ~value : value) ^ (o.negated ? ~o.value : o.value);
		}

		friend std::ostream& operator<<(std::ostream& os, const Operand& op)
		{
			return os << (op.negated ? "~" : "") << op.original_string;
		}
	};

	enum class OperationType : unsigned char {
		NONE = '\0',
		OR = '|',
		XOR = '^',
		AND = '&',
	};

	inline std::ostream& operator<<(std::ostream& os, const OperationType& operation)
	{
		return os << static_cast<unsigned char>(operation);
	}

	inline long long calculateOperation(long long const& left, OperationType const& operation, long long const& right)
	{
		switch (operation) {
		case OperationType::AND:
			return (left & right);
		case OperationType::OR:
			return (left | right);
		case OperationType::XOR:
			return (left ^ right);
		case OperationType::NONE: [[fallthrough]];
		default:
			throw make_exception("bitwise::calculateOperation() failed:  Received invalid operation type!");
		}
	}
	inline long long calculateOperation(Operand const& left, OperationType const& operation, Operand const& right)
	{
		switch (operation) {
		case OperationType::OR:
			return left | right;
		case OperationType::XOR:
			return left ^ right;
		case OperationType::AND:
			return left & right;
		case OperationType::NONE: [[fallthrough]];
		default:
			throw make_exception("bitwise::calculateOperation() failed:  Received invalid operation type!");
		}
	}
	inline long long calculateOperation(std::tuple<Operand, OperationType, Operand> const& tpl)
	{
		const auto& [l, op, r] {tpl};
		return calculateOperation(l, op, r);
	}
	inline long long calculateOperation(std::tuple<long long, OperationType, long long> const& tpl)
	{
		const auto& [l, op, r] {tpl};
		return calculateOperation(l, op, r);
	}

	class Parser : token::base::TokenParserBase<std::vector<std::tuple<Operand, OperationType, Operand>>, Token> {
	public:
		Parser(std::vector<TokenT>&& tokens) : TokenParserBase<std::vector<std::tuple<Operand, OperationType, Operand>>, Token>(std::move(tokens)) {}

		OutputT parse() const
		{
			std::queue<Token> queue;
			OutputT vec;
			vec.reserve(tokens.size());
			OperationType type{ OperationType::NONE };
			std::unique_ptr<Operand> l, r;
			bool negateNext{ false };

			// lambda that creates a tuple (l, type, r) and pushes it to the output vector, then resets l, type, and r to default.
			const auto& pushOperation{ [&l, &r, &type, &vec]() {
				if (l == nullptr || r == nullptr || type == OperationType::NONE)
					throw make_exception("bitwise::Parser::parse()::pushOperation() failed:  Cannot push null operation!");
				vec.emplace_back(std::make_tuple(*l.get(), type, *r.get()));
				l = nullptr;
				type = OperationType::NONE;
				r = nullptr;
			} };
			// set the current operation (type). If the type is already set, throws an exception.
			const auto& setType{ [&type](OperationType&& newType) {
				if (type != OperationType::NONE)
					throw make_exception("bitwise::Parser::parse() failed:  Too many operation types specified!");
				type = std::move(newType);
			} };
			// set either (l or r) to an operand. If both operands are already set, throws an exception.
			const auto& setOperand{ [&pushOperation, &l, &r, &type, &vec](Operand&& op) {
				if (l != nullptr && r != nullptr && type != OperationType::NONE)
					pushOperation();
				if (l == nullptr)
					l = std::make_unique<Operand>(std::move(op));
				else if (r == nullptr)
					r = std::make_unique<Operand>(std::move(op));
				else throw make_exception(
						"bitwise::Parser::parse() failed:  Too many operands were specified!\n",
						indent(10), "Suggested Solutions:\n",
						indent(11), "1. Ensure there aren't any spaces between numbers.",
						indent(11), "2. Ensure that each operation has a clear beginning and end."
				);
				if (l != nullptr && r != nullptr && type != OperationType::NONE)
					pushOperation();
			} };

			for (auto it{ tokens.begin() }; it != tokens.end();) {
				switch (it->type) {
				case TokenType::END:
					return vec;
				case TokenType::ENCLOSED: // recurse into brackets
					for (const auto& operation : Parser(Tokenizer(std::move(std::stringstream{ it++->str })).tokenize()).parse())
						queue.push(Token{ std::to_string(calculateOperation(operation)), TokenType::DECIMAL });
					break;
				default:
					queue.push(*it++);
					break;
				}
				while (!queue.empty()) {
					const auto tkn{ queue.front() };
					queue.pop();

					switch (tkn.type) {
					case TokenType::NEGATE:
						negateNext = !negateNext;
						break;
					case TokenType::OR:
						if (negateNext)
							throw make_exception("bitwise::Parser::parse() failed:  Cannot negate operator OR!");
						setType(OperationType::OR);
						break;
					case TokenType::XOR:
						if (negateNext)
							throw make_exception("bitwise::Parser::parse() failed:  Cannot negate operator XOR!");
						setType(OperationType::XOR);
						break;
					case TokenType::AND:
						if (negateNext)
							throw make_exception("bitwise::Parser::parse() failed:  Cannot negate operator AND!");
						setType(OperationType::AND);
						break;
					case TokenType::BINARY: [[fallthrough]];
					case TokenType::DECIMAL: [[fallthrough]];
					case TokenType::HEXADECIMAL:
						setOperand(std::move(Operand(tkn, negateNext)));
						negateNext = false;
						break;
					default:
						break;
					}
				}
			}

			return vec;
		}
	};



	struct operand_base {
		virtual ~operand_base() = default;

		virtual long long result() const = 0;
	};

	struct operation : operand_base {
		using type = std::tuple<long long, OperationType, long long>;
		type _operation;

		virtual long long result() const override
		{
			return calculateOperation(_operation);
		}
	};

	struct step : operand_base {
		using type = std::unique_ptr<operand_base>;
		type _step;

		virtual long long result() const override
		{
			if (auto* ptr{ _step.get() }; ptr == nullptr)
				throw make_exception("step::result() failed:  Step is null!");
			else return ptr->result();
		}
	};



	struct Operation {
		enum class Type : unsigned char {
			NONE,
			AND,
			OR,
			XOR,
		};
		Type type;
		std::optional<Operand> l, r;

		Operation() : type{ Type::NONE } {}

		void setType(const bitwise::TokenType& tkntype)
		{
			switch (tkntype) {
			case TokenType::AND:
				type = Type::AND;
				break;
			case TokenType::OR:
				type = Type::OR;
				break;
			case TokenType::XOR:
				type = Type::XOR;
				break;
			default:
				type = Type::NONE;
				break;
			}
		}

		bool ready() const
		{
			return type != Type::NONE && l.has_value() && r.has_value();
		}

		void reset()
		{
			type = Type::NONE;
			l = std::nullopt;
			r = std::nullopt;
		}

		long long getResult() const
		{
			switch (type) {
			case Type::AND:
				return l.value() & r.value();
			case Type::OR:
				return l.value() | r.value();
			case Type::XOR:
				return l.value() ^ r.value();
			case Type::NONE: [[fallthrough]];
			default:
				throw make_exception("Operation is missing required components!");
			}
		}

		char getOperationSymbol() const
		{
			switch (type) {
			case Type::AND:
				return '&';
			case Type::OR:
				return '|';
			case Type::XOR:
				return '^';
			case Type::NONE: [[fallthrough]];
			default:
				throw make_exception("Operation doesn't have a valid type!");
			}
		}
	};
}

namespace bitwise2 {
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

	using operand = long long;

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

	struct operation {
		using suboperation = std::unique_ptr<operation>;
		using variant_t = std::variant<std::monostate, suboperation, operand>;

		Operator type{ Operator::NONE };
		variant_t left, right;

		operation() = default;

		operation(const Operator& type, variant_t&& left, variant_t&& right) : type{ type }, left{ std::move(left) }, right{ std::move(right) }
		{
			if (type == Operator::NONE || type == Operator::NEGATE)
				throw make_custom_exception<exceptions::invalid_operation_except>(type, 1, 2);

			if (const bool leftNull{ !hasv(left) }, rightNull{ !hasv(right) }; leftNull && rightNull)
				throw make_custom_exception<exceptions::invalid_operation_except>(type, 2, "both were null!");
			else if (rightNull)
				throw make_custom_exception<exceptions::invalid_operation_except>(type, 2, "the right operand was null!");
			else if (leftNull)
				throw make_custom_exception<exceptions::invalid_operation_except>(type, 2, "the left operand was null!");
		}
		operation(variant_t&& left, const Operator& type, variant_t&& right) : operation(type, std::move(left), std::move(right)) {}

		operation(const Operator& type, variant_t&& value) : type{ type }, left{ std::move(value) }, right{ std::monostate{} }
		{
			if (type != Operator::NONE && type != Operator::NEGATE)
				throw make_custom_exception<exceptions::invalid_operation_except>(type, 2, 1);
			if (!hasv(left))
				throw make_custom_exception<exceptions::invalid_operation_except>(type, 1, "the left operand was null!");
			if (hasv(right))
				throw make_custom_exception<exceptions::invalid_operation_except>(type, 1, 2);
		}
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

		virtual operand result() const
		{
			if (!hasv(left))
				throw make_exception("operation::result() failed:  Left operand is null!");
			operand l{ get(left) };

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

	using bitwise::Token;
	using bitwise::TokenType;
	using bitwise::Tokenizer;

	class Parser : token::base::TokenParserBase<operation, Token> {
	public:
		Parser(std::vector<TokenT>&& tokens) : TokenParserBase<operation, Token>(std::move(tokens)) {}
		Parser(std::stringstream&& ss) : TokenParserBase<operation, Token>(std::move(Tokenizer(std::move(ss)).tokenize())) {}

		OutputT parse() const
		{
			using std::optional;
			optional<operation::variant_t> l, r;
			optional<Operator> type;
			bool negateNext{ false };

			const auto& push{ [&](auto&& v) {
				if (!l.has_value())
					l = std::move(v);
				else if (!r.has_value())
					r = std::move(v);
				else throw make_exception("bitwise2::Parser::parse() failed:  Too many operands!");
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
					operand value{ std::move(str::toBase10(it->str, 2)) };
					if (negateNext) {
						push(std::move(std::make_unique<operation>(Operator::NEGATE, std::move(value))));
						negateNext = false;
					}
					else push(value);
					break;
				}
				case TokenType::HEXADECIMAL: {
					operand value{ std::move(str::toBase10(it->str, 16)) };
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
					std::unique_ptr<operation> oper{ std::make_unique<operation>(std::move(Parser(std::move(Tokenizer(std::move(std::stringstream{ it->str })).tokenize())).parse())) };
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

			if (!type.has_value())
				throw make_exception("bitwise2::Parser::parse() failed:  No operator specified!");
			else if ((type.value() != Operator::NEGATE && !l.has_value() && !r.has_value()) || (type.value() == Operator::NEGATE && (!l.has_value() || r.has_value())))
				throw make_exception("bitwise2::Parser::parse() failed:  Incorrect number of operands for operation type '", type.value(), "'!");

			return std::move(operation{ type.value(), std::move(l.value()), std::move(r.value()) });
		}
	};
}

