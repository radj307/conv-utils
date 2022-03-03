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
		Token getNext() override
		{
			char c{ getch() };
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

				if (base::validBinary(s))
					return Token{ TokenType::BINARY, s };
				else if (base::validDecimal(s))
					return Token{ TokenType::DECIMAL, s };
				else if (base::validHexadecimal(s))
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

	inline base::value parse(std::string const& str, TokenType const& type) noexcept(false)
	{
		switch (type) {
		case TokenType::BINARY:
			return base::binaryToDecimal(str);
		case TokenType::DECIMAL:
			return str::stoll(str);
		case TokenType::HEXADECIMAL:
			return base::hexToDecimal(str);
		default:
			throw make_exception("Invalid token type received \"", tokenTypeToString(type), '\"');
		}
	}

	struct Operand {
		bool negated;
		base::value value;
		std::string original_string;

		Operand(std::string const& str, TokenType const& type, const bool& negated = false) : value{ parse(str, type) }, negated{ negated }, original_string{ str } {}
		Operand(Token const& token, const bool& negated = false) : value{ parse(token.str, token.type) }, negated{ negated }, original_string{ token.str } {}

		operator base::value() const { return value; }

		base::value operator&(const Operand& o) const
		{
			if (negated && !o.negated)
				return ~value & o.value;
			else if (!negated && !o.negated)
				return value & o.value;
			else if (!negated && o.negated)
				return value & ~o.value;
			else if (negated && o.negated)
				return ~value & ~o.value;
			else throw make_exception("Unhandled operation");
		}
		base::value operator|(const Operand& o) const
		{
			if (negated && !o.negated)
				return ~value | o.value;
			else if (!negated && !o.negated)
				return value | o.value;
			else if (!negated && o.negated)
				return value | ~o.value;
			else if (negated && o.negated)
				return ~value | ~o.value;
			else throw make_exception("Unhandled operation");
		}
		base::value operator^(const Operand& o) const
		{
			if (negated && !o.negated)
				return ~value ^ o.value;
			else if (!negated && !o.negated)
				return value ^ o.value;
			else if (!negated && o.negated)
				return value ^ ~o.value;
			else if (negated && o.negated)
				return ~value ^ ~o.value;
			else throw make_exception("Unhandled operation");
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

	inline base::value calculateOperation(Operand const& left, OperationType const& operation, Operand const& right)
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
	inline base::value calculateOperation(std::tuple<Operand, OperationType, Operand> const& tpl)
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

		base::value getResult() const
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

