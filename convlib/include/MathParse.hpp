/**
 * @file	MathParse.hpp
 * @author	radj307
 * @brief	Math recognition header, uses tokenization & parsing to read & apply order-of-operations to equations represented as strings.
 */
#pragma once
#include <make_exception.hpp>

#include <LEXEME.h>
#include <Token.hpp>
#include <TokenizerBase.hpp>
#include <TokenParserBase.hpp>

namespace math {
	namespace recog {
		enum class Type : char {
			NONE,
			END,
			ALPHA,			// Value:		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
			NUMBER,			// Value:		"0123456789."
			ADD,			// Operation:	'+'
			SUBTRACT,		// Operation:	'-'
			MULTIPLY,		// Operation:	'*'
			DIVIDE,			// Operation:	'/'
			MODULO,			// Operation:	'%'
			OPEN_BRACKET,	// Control:		'('
			CLOSE_BRACKET,	// Control:		')'
			GREATER_THAN,	// Compare:		'>'
			LESS_THAN,		// Compare:		'<'
			EXPONENT,		// Operation:	'^'
			EQUALS,			// Control:		'='
		};
		using Token = ::token::TokenBase<Type>;

		struct Tokenizer : public ::token::TokenizerBaseFunctions<Token> {
			Tokenizer(const std::string& eq) : ::token::TokenizerBaseFunctions<Token>(std::stringstream{eq}, 0ll) {}
			Token getNext() override
			{
				using namespace ::token;
				auto ch{ getch() };
				switch (get_lexeme(ch)) {
				case LEXEME::PARENTHESIS_OPEN:
					return Token{ ch, Type::OPEN_BRACKET };
				case LEXEME::PARENTHESIS_CLOSE:
					return Token{ ch, Type::CLOSE_BRACKET };
				case LEXEME::EQUALS:
					return Token{ ch, Type::EQUALS };
				case LEXEME::ADDITION:
					return Token{ ch, Type::ADD };
				case LEXEME::SUBTRACT: // this includes the subtraction symbol before negative numbers, parser should recognize double-negative
					return Token{ ch, Type::SUBTRACT };
				case LEXEME::STAR:
					return Token{ ch, Type::MULTIPLY };
				case LEXEME::SLASH:
					return Token{ ch, Type::DIVIDE };
				case LEXEME::CARET:
					return Token{ ch, Type::EXPONENT };
				case LEXEME::PERCENT:
					return Token{ ch, Type::MODULO };
				case LEXEME::ANGLEBRACKET_OPEN:
					return Token{ ch, Type::GREATER_THAN };
				case LEXEME::ANGLEBRACKET_CLOSE:
					return Token{ ch, Type::LESS_THAN };
				case LEXEME::UNDERSCORE: [[fallthrough]];
				case LEXEME::LETTER_LOWERCASE: [[fallthrough]];
				case LEXEME::LETTER_UPPERCASE:
					return Token{ std::string(1ull, ch) += getsimilar(LEXEME::UNDERSCORE, LEXEME::LETTER_LOWERCASE, LEXEME::LETTER_UPPERCASE), Type::ALPHA };
				case LEXEME::PERIOD: [[fallthrough]];
				case LEXEME::DIGIT:
					return Token{ std::string(1ull, ch) += getsimilar(LEXEME::DIGIT, LEXEME::PERIOD), Type::NUMBER };
				case LEXEME::_EOF:
					return Token{ ch, Type::END };
				default:
					return Token{ ch, Type::NONE };
				}
			}
		};

		template<typename T> concept Fundamental = std::integral<T> || std::floating_point<T>;

		class Operand;

		class Operation {
		public:
			Token token;
			std::unique_ptr<Operand> left;
			std::unique_ptr<Operand> right;

			Operation(const Token& t, Operand&& l, Operand&& r) : token{ t }, left{ std::make_unique<Operand>(std::forward<Operand>(l)) }, right{ std::make_unique<Operand>(std::forward<Operand>(r)) } {}
			//Operation(const Token& token, const Tl& left, const Tr& right) : token{ token }, left{ left }, right{ right } {}

			operator Token() const { return token; }
			operator Type() const { return token.type(); }
			Type type() const { return operator Type(); }

			std::unique_ptr<Operand>&& evaluate() const;
		};


		using IntT = long long;
		using FloatT = long double;
		using VariantT = std::variant<std::monostate, IntT, FloatT, Operation>;

		template<typename T> concept operand_type = var::any_same<T, IntT, FloatT, Operation>;

		template<class Func>
		inline VariantT op(const Func& func, const IntT l, const VariantT& r) // left is an integral
		{
			if (auto* pr = std::get_if<Operation>(&r))
				return op(func, l, *pr->evaluate().get()); // recurse
			else { // value type
				if (auto* prv = std::get_if<IntT>(&r))
					return func(l, *prv);
				if (auto* prv = std::get_if<FloatT>(&r))
					return func(static_cast<FloatT>(l), *prv);
			}
			throw make_exception("Cannot perform operation on null value!");
		}
		template<class Func>
		inline VariantT op(const Func& func, const FloatT& l, const VariantT& r) // left is a floating-point
		{
			if (auto* pr = std::get_if<Operation>(&r))
				return op(func, l, *pr->evaluate().get()); // recurse
			else { // value type
				if (auto* prv = std::get_if<IntT>(&r))
					return func(l, static_cast<FloatT>(*prv));
				if (auto* prv = std::get_if<FloatT>(&r))
					return func(l, *prv);
			}
			throw make_exception("Cannot perform operation on null value!");
		}
		template<class Func>
		inline VariantT op(const Func& func, const VariantT& l, const VariantT& r) // left is any type (or left is an operation)
		{
			if (auto* pl = std::get_if<Operation>(&l)) // left is an operation
				return op(func, *pl->evaluate().get(), r); // recurse
			else { // value type
				if (auto* plv = std::get_if<IntT>(&l)) // left is an integral
					return op(func, *plv, r);
				if (auto* plv = std::get_if<FloatT>(&l)) // left is a floating-point
					return op(func, *plv, r);
			}
			return{ std::monostate{} };
		}
		inline VariantT add(const VariantT& l, const VariantT& r) { return op([]<typename T>(const T & left, const T & right) { return left + right; }, l, r); }
		inline VariantT sub(const VariantT& l, const VariantT& r) { return op([]<typename T>(const T & left, const T & right) { return left - right; }, l, r); }
		inline VariantT mult(const VariantT& l, const VariantT& r) { return op([]<typename T>(const T & left, const T & right) { return left * right; }, l, r); }
		inline VariantT div(const VariantT& l, const VariantT& r) { return op([]<typename T>(const T & left, const T & right) { return left / right; }, l, r); }
		inline VariantT mod(const VariantT& l, const VariantT& r)
		{
			return op([]<typename T>(const T & left, const T & right)
			{
				if constexpr (std::integral<T>)
					return left % right;
				return static_cast<T>(fmod(left, right));
			}, l, r);
		}
		inline VariantT pow(const VariantT& l, const VariantT& r) { return op([]<typename T>(const T & left, const T & right) { return std::pow(left, right); }, l, r); }

		class Operand {
			mutable VariantT _value;
		public:
			Operand() : _value{ std::monostate{} } {}
			Operand(VariantT&& v) : _value{ std::move(v) } {}
			Operand(const Token& token, Operand l, Operand r) : _value{ Operation{token, std::move(l), std::move(r)} } {}

			bool is_null() const { return std::holds_alternative<std::monostate>(_value); }
			bool is_int() const { return std::holds_alternative<IntT>(_value); }
			bool is_float() const { return std::holds_alternative<FloatT>(_value); }
			bool is_operation() const { return std::holds_alternative<Operation>(_value); }

			template<std::same_as<IntT> T> [[nodiscard]] T get() const { return std::get<IntT>(_value); }
			template<std::same_as<FloatT> T> [[nodiscard]] T get() const { return std::get<FloatT>(_value); }
			template<std::same_as<Operation> T> [[nodiscard]] T get() const { return std::get<Operation>(_value); }

			template<operand_type T> [[nodiscard]] T get_or(const T& value) const { if (auto* p = std::get_if<T>(&_value)) return *p; return value; }

			operator VariantT && () { return std::forward<VariantT>(_value); }
			operator VariantT& () const { return _value; }

			Operand operator+(const Operand& o) const
			{
				return Operand{ add(_value, o._value) };
			}
			Operand operator-(const Operand& o) const
			{
				return Operand{ sub(_value, o._value) };
			}
			Operand operator*(const Operand& o) const
			{
				return Operand{ mult(_value, o._value) };
			}
			Operand operator/(const Operand& o) const
			{
				return Operand{ div(_value, o._value) };
			}
			Operand operator%(const Operand& o) const
			{
				return Operand{ mod(_value, o._value) };
			}
			Operand operator^(const Operand& o) const
			{
				return Operand{ pow(_value, o._value) };
			}
		};

		struct TokenParser : public ::token::TokenParserBase<Token> {
			
		};
	}
}