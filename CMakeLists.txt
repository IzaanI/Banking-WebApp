cmake_minimum_required(VERSION 3.10)
project(CPP_Personal_Projects)

set(CMAKE_CXX_STANDARD 20)

find_package(Crow CONFIG REQUIRED)


# Define your executable target and source files
add_executable(CPP_Personal_Projects
        main.cpp
        sqlite_main.h
        passwordHandler.h
        sqlite3.h
        sqlite3.c
        db_instance.h
        baseAccountClass.h
        saveBankAccountClass.h
        creditScoreAccountClass.h
)
target_include_directories(CPP_Personal_Projects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(CPP_Personal_Projects PRIVATE Crow::Crow)