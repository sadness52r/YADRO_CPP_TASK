# YADRO_CPP_TASK
Приложение, симулирующее работу компьютерного клуба

# Сборка
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON
make

##### Запуск основного приложения
./task.exe ..\tests\test.txt
##### Запуск всех тестов
./tests/unit_tests

Для тестов используется GTest.
