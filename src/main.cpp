#include <dim/lexer/lexer.hpp>

#include <iostream>

int main(int argc, char** argv) {
	std::cout << "Welcome to the DIM Programming Language\n";

	std::vector<struct dim::lexer::Token> tokens = {};

	std::string src = "1453 2.3453";

	dim::lexer::Lex(
		tokens,
		src
	);

	for(const auto& token : tokens) {
		std::cout << dim::lexer::TokenRepr(token) << std::endl;		
	}

	return 0;
}
