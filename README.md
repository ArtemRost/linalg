# linalg

Библиотека линейной алгебры, реализующая класс `linalg::Matrix`.

## Структура репозитория

```
include/
  linalg/Matrix.hpp   # Заголовок с публичным интерфейсом класса Matrix
source/
  Matrix.cpp          # Реализация методов Matrix
CMakeLists.txt        # Описание библиотеки для CMake
.gitignore            # Игнорирование артефактов сборки и IDE
```

## Минимальные требования к окружению
- CMake 3.20+
- Компилятор с поддержкой C++20

## Сборка библиотеки

```bash
cmake -S . -B build
cmake --build build
```

## Подключение в проекте-тестере
1. Создайте внешний репозиторий, в котором будут располагаться тесты.
2. Добавьте данный репозиторий как git-подмодуль:
   ```bash
   git submodule add <ssh-or-https-url-на-этот-репозиторий> linalg
   ```
3. В корне тестового репозитория организуйте структуру
   ```
   source/
   linalg/             # Папка с подмодулем
   CMakeLists.txt
   .gitignore
   ```
4. В `CMakeLists.txt` тестового проекта подключите библиотеку:
   ```cmake
   cmake_minimum_required(VERSION 3.20)
   project(linalg_tests LANGUAGES CXX)

   add_subdirectory(linalg)

   add_executable(matrix_tests
       source/main.cpp
   )

   target_link_libraries(matrix_tests PRIVATE linalg)
   ```
5. Настройте GoogleTest и добавьте тесты в `source/main.cpp` (или в отдельные файлы).

## Проверка шага
- Убедитесь, что команды сборки выполняются без ошибок.
- Проверьте, что подмодуль корректно подключается в тестовом репозитории: `git submodule status`.
- Выполните `ctest` (после добавления тестов), чтобы убедиться в корректном подключении.

