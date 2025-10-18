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

## Настройка тестового репозитория с GoogleTest
Лабораторная работа предполагает отдельный репозиторий, в котором находятся
тесты на GoogleTest и куда эта библиотека подключается как подмодуль.

1. Создайте новый репозиторий, например `linalg-tests`.
2. Подключите текущую библиотеку:
   ```bash
   git submodule add <ssh-или-https-url-на-этот-репозиторий> linalg
   ```
3. Подключите официальный GoogleTest как подмодуль:
   ```bash
   git submodule add https://github.com/google/googletest.git third_party/googletest
   ```
4. Рекомендуемая структура тестового репозитория:
   ```
   linalg-tests/
     CMakeLists.txt
     source/
       Matrix_basic_tests.cpp
     linalg/                  # подмодуль библиотеки
     third_party/googletest/  # подмодуль GoogleTest
     .gitmodules
     .gitignore
   ```
5. Пример `CMakeLists.txt` тестового проекта:
   ```cmake
   cmake_minimum_required(VERSION 3.20)
   project(linalg_matrix_tests LANGUAGES CXX)

   set(CMAKE_CXX_STANDARD 20)
   set(CMAKE_CXX_STANDARD_REQUIRED ON)
   set(CMAKE_CXX_EXTENSIONS OFF)

   add_subdirectory(linalg)
   add_subdirectory(third_party/googletest)

   enable_testing()

   add_executable(matrix_basic_tests
       source/Matrix_basic_tests.cpp
   )

   target_link_libraries(matrix_basic_tests
       PRIVATE
           linalg
           GTest::gtest_main
   )

   if (MSVC)
       target_compile_options(matrix_basic_tests PRIVATE /W4 /permissive-)
   else()
       target_compile_options(matrix_basic_tests PRIVATE -Wall -Wextra -Wpedantic)
   endif()

   include(GoogleTest)
   gtest_discover_tests(matrix_basic_tests)
   ```
6. В каталоге `source/` разместите тесты с макросами GoogleTest (`TEST`,
   `EXPECT_EQ`, `EXPECT_THROW` и т.д.), которые покрывают требования методички.

### Пример файла `source/Matrix_basic_tests.cpp`
```cpp
#include <gtest/gtest.h>
#include "linalg/Matrix.hpp"

TEST(MatrixConstructors, FromInitializerList)
{
    const linalg::Matrix matrix = {{1.0, 2.0}, {3.0, 4.0}};

    EXPECT_EQ(matrix.rows(), 2);
    EXPECT_EQ(matrix.columns(), 2);
    EXPECT_DOUBLE_EQ(matrix(0, 1), 2.0);
}

TEST(MatrixAlgebra, MultiplicationAndInverse)
{
    const linalg::Matrix base = {
        {4.0, 7.0},
        {2.0, 6.0}
    };

    const linalg::Matrix identity = linalg::power(base * linalg::invert(base), 1);

    EXPECT_TRUE(identity == linalg::Matrix{{1.0, 0.0}, {0.0, 1.0}});
}

TEST(MatrixSolvers, SolveLinearSystem)
{
    const linalg::Matrix a = {
        {2.0, 1.0},
        {5.0, 7.0}
    };
    const linalg::Matrix f = {
        {11.0},
        {13.0}
    };

    const linalg::Matrix solution = linalg::solve(a, f);

    EXPECT_NEAR(solution(0, 0), 7.0, 1e-9);
    EXPECT_NEAR(solution(1, 0), -3.0, 1e-9);
}
```

### Рекомендации по наполнению тестов
- Покройте все специализированные конструкторы, включая копирование и перемещение.
- Проверьте методы управления памятью (`reserve`, `reshape`, `clear`, `shrink_to_fit`, `swap`).
- Для арифметических операторов используйте как положительные сценарии, так и проверки на выбрасывание исключений при несовместимых размерах.
- В вычислительных тестах сверяйте свойства из методички, например:
  - `A * A^{-1} = E`.
  - `(A_1 * A_2)^{-1} = A_2^{-1} * A_1^{-1}`.
  - `(A^{-1})^6 = (A^{-2})^3`.
- Добавьте тесты на корректную работу методов Гаусса (`gauss_forward`, `gauss_backward`) и ранга матрицы.

Для запуска тестов выполните в корне тестового репозитория:
```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Проверка шага
- Убедитесь, что сборка библиотеки (`cmake --build build`) проходит успешно.
- В тестовом репозитории выполните `git submodule update --init --recursive`,
  чтобы инициализировать оба подмодуля.
- Соберите тестовый проект и запустите `ctest --test-dir build` — убедитесь,
  что GoogleTest подключён корректно и тесты завершаются без ошибок.

## Пример проверки операций изменения матрицы
Чтобы убедиться в корректности методов `reserve`, `reshape`, `clear`, `shrink_to_fit` и `swap`,
можно собрать небольшую консольную утилиту, используя библиотеку:

```cpp
#include "linalg/Matrix.hpp"
#include <iostream>

int main() {
    linalg::Matrix matrix = {{1.0, 2.0}, {3.0, 4.0}};

    // Расширяем вместимость под будущие reshape-операции
    matrix.reserve(12);
    matrix.reshape(3, 2);

    double value = 0.0;
    for (std::size_t r = 0; r < matrix.rows(); ++r) {
        for (std::size_t c = 0; c < matrix.columns(); ++c) {
            matrix(r, c) = value++;
        }
    }

    matrix.clear();              // Размер 0x0, но память на 12 элементов сохранена
    matrix.reshape(6, 2);         // Использует уже зарезервированную память
    matrix.shrink_to_fit();       // Вместимость теперь ровно 12 элементов

    linalg::Matrix other(2, 6);   // Создаём вторую матрицу, чтобы проверить swap
    matrix.swap(other);

    std::cout << "Размер после swap: "
              << other.rows() << "x" << other.columns() << std::endl;
}
```

Если программа завершается без исключений и выводит ожидаемые размеры, значит
базовая работа с буфером матрицы реализована корректно.

## Пример вычислительных операций
Ниже приведён пример, демонстрирующий расчёт следа, определителя, обратной
матрицы и решение системы линейных уравнений. Этот код удобно использовать в
тестовом репозитории с GoogleTest.

```cpp
#include "linalg/Matrix.hpp"
#include <iostream>

int main() {
    const linalg::Matrix base = {
        {4.0, 7.0},
        {2.0, 6.0}
    };

    std::cout << "След: " << base.trace() << "\n";
    std::cout << "Определитель: " << base.det() << "\n";

    linalg::Matrix inverse = linalg::invert(base);
    std::cout << "Обратная матрица:\n" << inverse << "\n\n";

    const linalg::Matrix rhs = {
        {1.0},
        {0.0}
    };
    linalg::Matrix solution = linalg::solve(base, rhs);
    std::cout << "Решение A * x = f:\n" << solution << '\n';
}
```

При запуске программы вывод должен совпадать с ручными вычислениями. Если
возникают исключения о вырожденности матрицы, проверьте корректность входных
данных и наполнение тестов.

## Финальная проверка проекта
Перед сдачей лабораторной работы убедитесь, что выполнены все обязательные
проверки:

1. **Сборка библиотеки.** В корне текущего репозитория выполните команды
   `cmake -S . -B build` и `cmake --build build`, чтобы убедиться, что
   библиотека компилируется без предупреждений и ошибок.
2. **Актуализация подмодулей.** В тестовом репозитории запустите
   `git submodule update --init --recursive`, чтобы гарантировать, что и
   библиотека `linalg`, и GoogleTest находятся в актуальном состоянии.
3. **Сборка и запуск тестов.** В тестовом репозитории выполните `cmake -S . -B
   build`, `cmake --build build`, затем `ctest --test-dir build
   --output-on-failure`. Все тесты должны успешно пройти.
4. **Проверка git-истории.** Убедитесь, что в обоих репозиториях история
   коммитов отражает последовательную работу над задачами: отдельный коммит на
   каждый логический шаг, осмысленные сообщения, отсутствие временных файлов.
5. **Документация.** Просмотрите README обоих репозиториев — в них должны быть
   описаны структура, шаги сборки и инструкции по запуску тестов.

Только после успешного прохождения всех пунктов можно публиковать ссылку на
GitLab-группу с двумя репозиториями в системе SmartLMS.

## Публикация библиотеки на GitHub
Ниже приведён краткий план, как отправить текущий репозиторий в удалённый
GitHub-репозиторий. Все команды выполняются из корня проекта.

1. **Свяжите локальный репозиторий с удалённым.**
   ```bash
   git remote add origin git@github.com:<ваш-логин>/<имя-репозитория>.git
   ```
   Если `origin` уже существует (проверить можно командой `git remote -v`),
   просто убедитесь, что URL соответствует нужному репозиторию.
2. **Проверьте текущую ветку и выполните push.**
   ```bash
   git status            # убеждаемся, что все изменения закоммичены
   git branch            # проверяем имя активной ветки
   git push -u origin <имя-ветки>
   ```
   Параметр `-u` позволяет в дальнейшем вызывать `git push` без указания
   ветки.
3. **Обновляйте подмодули.** Если в проекте есть подмодули, после пуша
   основного репозитория выполните `git push` внутри каталогов подмодулей,
   чтобы синхронизировать их состояния с удалёнными репозиториями.
4. **Проверьте результат.** Откройте страницу GitHub-репозитория в браузере и
   убедитесь, что коммиты и файлы появились. При необходимости создайте
   pull-request или настройте защищённые ветки.

> ⚠️ Если вы работаете по HTTPS, используйте URL вида
> `https://github.com/<ваш-логин>/<имя-репозитория>.git` и будьте готовы
> ввести логин и токен доступа при выполнении `git push`.

