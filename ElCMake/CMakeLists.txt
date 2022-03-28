# CMakeList.txt: проект CMake для ElCMake; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

# Добавьте источник в исполняемый файл этого проекта.
add_executable (ElCMake "ElCMake.cpp" "ElCMake.h" "src/bmpK3D/bmpK3D.h" "src/bmpK3D/bmpK3D.cpp" "src/yuvK3D/yuvK3D.h" "src/yuvK3D/yuvK3D.cpp")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
