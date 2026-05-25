#include <iostream>
#include <fstream>
#include <windows.h>// да може visualstudio да чете кирилица
#include "XMLNode.hpp"
#include "XMLTree.hpp"

int main()
{
  SetConsoleOutputCP(CP_UTF8); // Настройва изхода към конзолата да е UTF-8
  std::ifstream file("test.xml");

  if (!file.is_open()) {
    std::cerr << "Грешка: Файлът test.xml не може да бъде намерен в работната директория!\n";
    return 1;
  }

  try {
    XMLTree tree(file);
    std::cout << "УСПЕХ! XML файлът беше парснат без никакви грешки!\n";
    tree.print();
  }
  catch (const std::exception& e) {
    std::cerr << "Синтактична грешка при парсване: " << e.what() << "\n";
  }

  

  return 0;
}
