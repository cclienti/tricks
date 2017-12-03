project(tricks)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_FLAGS "-g -std=c++14")

add_executable(mergesort src/mergesort.cpp)
add_executable(quicksort src/quicksort.cpp)
add_executable(treeorder src/treeorder.cpp)
add_executable(vector    src/vector.cpp)
