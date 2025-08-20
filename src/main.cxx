// TODO
//	- Add type checking
// 	- Check const correctness
// 	- Add == and != operators

#include <lexer/lexer.hxx>
#include <parser/parser.hxx>
#include <runner/interpreter/interpreter.hxx>

#include <fstream>
#include <iostream>

std::string GetSource(
	std::string filename
);

int main(int argc, char** argv) {
	std::cout << "Welcome to the DIM Programming Language\n";

	std::vector<struct dim::lexer::Token> tokens = {};

	std::string src;

	{
		src = GetSource("main.dim");

		std::cout << "> SOURCE\n";
		std::cout << src << "\n";
	}

	{
		std::expected<Success, std::string> result = dim::lexer::Lex(
			tokens,
			src
		);

		if(!result) {
			std::cerr << result.error() << std::endl;
			return 1;
		}

		std::cout << "\n> TOKENS\n";
		for(const auto& token : tokens) {
			std::cout << dim::lexer::TokenRepr(token) << std::endl;		
		}
	}

	std::shared_ptr<dim::parser::ScopeExpression> program;

	{
		std::expected<
			std::shared_ptr<dim::parser::ScopeExpression>,
			std::string
		> result = dim::parser::Parse(tokens);

		if(!result) {
			std::cerr << result.error() << std::endl;
			return 1;
		}

		program = result.value();

		std::cout << "\n> EXPRESSIONS\n";
		std::cout << program->Repr() << "\n";
	}

	std::shared_ptr<dim::interpreter::Value> value;

	{
		std::expected<
			std::shared_ptr<dim::interpreter::Value>,
			std::string
		> result = dim::interpreter::EvaluateProgram(program);

		if(!result) {
			std::cerr << result.error() << std::endl;
			return 1;
		}

		value = result.value();

		std::cout << "\n> VALUE\n";
		std::cout << value->Repr() << "\n";
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
