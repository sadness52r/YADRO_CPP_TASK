# YADRO_CPP_TASK
Приложение, симулирующее работу компьютерного клуба

## Сборка

### Сборка через CMake (Linux / Windows MinGW)

```bash
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON
make
```

### Сборка напрямую через компилятор

#### Linux

```bash
g++ -std=c++17 -O2 src/**/*.cpp -Isrc -o task
```

#### Windows

```bash
g++ -std=gnu++17 src\**\*.cpp -Isrc -o task.exe
```

## Запуск

### Запуск основного приложения

```bash
task.exe tests\test.txt
```

### Запуск unit-тестов (GoogleTest)
./tests/unit_tests
