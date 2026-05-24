#include <iostream>
#include <fstream>
#include <Windows.h>// да може visualstudio да чете кирилица
#include "XMLNode.hpp"
#include "XMLTree.hpp"

int main()
{
  SetConsoleOutputCP(CP_UTF8); // Настройва изхода към конзолата да е UTF-8
  SetConsoleCP(CP_UTF8);       // Настройва входа от конзолата да е UTF-8
  std::setlocale(LC_ALL, "bg_BG.UTF-8");// да може visualstudio да чете кирилица
  std::ifstream file("test.xml");

  if (!file.is_open()) {
    std::cerr << "Грешка: Файлът test.xml не може да бъде намерен в работната директория!\n";
    return 1;
  }

  try {
    XMLTree tree(file);
    std::cout << "УСПЕХ! XML файлът беше парснат без никакви грешки!\n";
  }
  catch (const std::exception& e) {
    std::cerr << "Синтактична грешка при парсване: " << e.what() << "\n";
  }

  return 0;
}
