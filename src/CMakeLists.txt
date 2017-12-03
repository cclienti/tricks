project(google)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_FLAGS "-g -std=c++14")

add_executable(mergesort mergesort.cpp)
add_executable(quicksort quicksort.cpp)
add_executable(treeorder treeorder.cpp)
add_executable(vector vector.cpp)
add_executable(test_google test_google.cpp)
