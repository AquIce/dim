#include <lexer/lexer.hxx>
#include <parser/parser.hxx>

#include <fstream>
#include <iostream>

std::string GetSource(
	std::string filename
);

int main(int argc, char** argv) {
	std::cout << "Welcome to the DIM Programming Language\n";

	std::vector<struct dim::lexer::Token> tokens = {};

	std::string src = GetSource("main.dim");
	std::cout << src;

	std::cout << "> SOURCE\n";
	std::cout << src << "\n";

	dim::lexer::Lex(
		tokens,
		src
	);

	std::cout << "\n> TOKENS\n";
	for(const auto& token : tokens) {
		std::cout << dim::lexer::TokenRepr(token) << std::endl;		
	}

	auto expressions = dim::parser::Parse(tokens)->GetExpressions();

	std::cout << "\n> EXPRESSIONS\n";
	for(const auto& expression : expressions) {
		std::cout << expression->Repr() << "\n";
	}

	return 0;
}

std::string GetSource(
	std::string filename
) {
	auto file = std::ifstream(filename);
	std::string line;
	std::string source = "";

	while(std::getline(file, line)) {
		source += line + '\n';
	}

	return source;
}