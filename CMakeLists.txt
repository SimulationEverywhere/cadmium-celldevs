cmake_minimum_required (VERSION 3.1)
set(Boost_USE_MULTITHREADED FALSE)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_COMPILER "g++")
project(cadmium_celldevs)
add_compile_options(-g)


enable_testing()

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cadmium/include)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cadmium/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/DESTimes/include)



find_package(Boost COMPONENTS program_options unit_test_framework REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})

add_executable(utils_test test/grid_test.cpp)
target_link_libraries(utils_test ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
add_test(celldevs/utils utils_test)

add_executable(basic_celldevs_test test/default/default_coupled.cpp)
target_link_libraries(basic_celldevs_test Boost::program_options)

add_executable(grid_celldevs_test test/grid/grid_coupled.cpp)
target_link_libraries(grid_celldevs_test Boost::program_options)

add_executable(pandemic_hoya test/hoya/hoya_coupled.cpp)
target_link_libraries(pandemic_hoya Boost::program_options)


add_executable(co2_lab test/co2_model_lab/CO2_coupled.cpp)
target_link_libraries(co2_lab Boost::program_options)

add_executable(pandemic_hoya_iranian test/hoya_iranian/hoya_coupled.cpp)
target_link_libraries(pandemic_hoya_iranian Boost::program_options)

add_executable(pandemic_hoya_spain test/hoya_spain/hoya_coupled.cpp)
target_link_libraries(pandemic_hoya_spain Boost::program_options)
