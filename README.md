# YADRO_CPP_TASK
Приложение, симулирующее работу компьютерного клуба

## Сборка

Возможно, потребуется отключить антивирус, а также убедиться, что установлены cmake и компилятор mingw (для Windows)

### Сборка через CMake (Linux / Windows MinGW)

```bash
mkdir build
cd build
cmake -S .. -G "MinGW Makefiles" -DBUILD_TESTS=ON 
cmake --build .
```

## Запуск

### Запуск основного приложения

```bash
.\task.exe ..\tests\test.txt
```

### Запуск unit-тестов (GoogleTest)

```bash
./tests/unit_tests
```
