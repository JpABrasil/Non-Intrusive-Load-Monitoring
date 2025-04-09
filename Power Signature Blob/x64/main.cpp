
#include <iostream>
#include <string>
#include <ranges>

int main() {
    std::string texto = "isso-é-um-split";
    for (auto palavra : texto | std::views::split('-')) {
        for (char c : palavra) std::cout << c;
        std::cout << std::endl;
    }
    return 0;
}