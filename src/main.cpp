#include <dim/lexer/lexer.hpp>

#include <iostream>

int main(int argc, char** argv) {
	std::cout << "Welcome to the DIM Programming Language\n";

	std::vector<struct dim::lexer::Token> tokens = {};

	std::string src = "1 + 2";

	dim::lexer::lex(
		tokens,
		src
	);

	return 0;
}
