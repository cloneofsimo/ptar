cmake_minimum_required(VERSION 3.12)
project(ptar)

set(CMAKE_CXX_STANDARD 11)

execute_process(
    COMMAND /home/ubuntu/py39cuda/bin/python -c "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_VARIABLE pybind11_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
list(APPEND CMAKE_PREFIX_PATH ${pybind11_DIR})

set(Python3_EXECUTABLE /home/ubuntu/py39cuda/bin/python)
execute_process(COMMAND ${Python3_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_paths()['include'])"
    OUTPUT_VARIABLE Python3_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${Python3_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_paths()['platlib'])"
    OUTPUT_VARIABLE Python3_LIBRARY
    OUTPUT_STRIP_TRAILING_WHITESPACE)

include_directories(/usr/include)
link_directories(/usr/lib/x86_64-linux-gnu)

find_package(pybind11 REQUIRED)

add_library(cptar MODULE cptar.cpp ParallelTarReader.cpp)
set_target_properties(cptar PROPERTIES PREFIX "" OUTPUT_NAME "cptar")
target_link_libraries(cptar PRIVATE pybind11::module archive)
