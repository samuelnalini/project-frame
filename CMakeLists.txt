cmake_minimum_required(VERSION 3.20)
project(Framework VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS On)

set(SOURCE src)
set(LIB ${SOURCE}/lib)

add_executable(process
    ${SOURCE}/main.c
    ${SOURCE}/processor.c
    ${SOURCE}/utils.c
    ${SOURCE}/component.c
    ${SOURCE}/lexer.c
    ${SOURCE}/parser.c
    ${SOURCE}/file_vec.c
    ${SOURCE}/sema.c
    ${SOURCE}/ast.c
)
