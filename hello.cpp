#include "file.hpp"

#include <iostream>

void print_from_memory(std::span<std::byte> span) {
	for(auto c: span)
		std::cout << (char)c;
}

int main() {
	stylizer::load_from_file(print_from_memory, "../test.txt");
	std::cout << "Hello World" << std::endl;
}