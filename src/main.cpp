#include <dim/lexer/lexer.hpp>
#include <dim/parser/parser.hpp>

#include <iostream>

int main(int argc, char** argv) {
	std::cout << "Welcome to the DIM Programming Language\n";

	std::vector<struct dim::lexer::Token> tokens = {};

	std::string src = "1 - 3 + 2";

	std::cout << "SOURCE -----\n\n";
	std::cout << src << "\n";

	dim::lexer::Lex(
		tokens,
		src
	);

	std::cout << "\nTOKENS-----\n\n";
	for(const auto& token : tokens) {
		std::cout << dim::lexer::TokenRepr(token) << std::endl;		
	}

	auto expressions = dim::parser::Parse(tokens);

	std::cout << "\nEXPRESSIONS-----\n\n";
	for(const auto& expression : expressions) {
		std::cout << expression->Repr() << "\n";
	}

	return 0;
}
