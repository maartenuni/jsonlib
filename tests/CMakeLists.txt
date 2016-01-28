

add_executable(cppsmalltest smalltest.cpp)
add_executable(csmalltest smalltest.c)
add_executable(cpptest cpp_test.cpp)

target_link_libraries(cppsmalltest jrep)
target_link_libraries(csmalltest jrep)
target_link_libraries(cpptest jrep)
