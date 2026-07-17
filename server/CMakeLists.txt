cmake_minimum_required(VERSION 3.16)
project(tetris_server LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
find_package(Drogon REQUIRED)
find_path(MYSQL_INCLUDE_DIR mysql.h
    HINTS
        /opt/homebrew/opt/mysql-client/include
        /usr/local/opt/mysql-client/include
        /opt/homebrew/opt/mysql-connector-c/include
        /usr/local/opt/mysql-connector-c/include
    PATH_SUFFIXES mysql)
find_library(MYSQL_CLIENT_LIBRARY mysqlclient
    HINTS
        /opt/homebrew/opt/mysql-client/lib
        /usr/local/opt/mysql-client/lib
        /opt/homebrew/opt/mysql-connector-c/lib
        /usr/local/opt/mysql-connector-c/lib)
if(NOT MYSQL_INCLUDE_DIR OR NOT MYSQL_CLIENT_LIBRARY)
    message(FATAL_ERROR "MySQL client not found. Install mysql-client and configure MYSQL_INCLUDE_DIR/MYSQL_CLIENT_LIBRARY.")
endif()

add_executable(tetris_server
    src/app/main.cpp
    src/config/config.cpp
    src/data/database.cpp
    src/services/mail.cpp
    src/services/auth_service.cpp
    src/services/score_service.cpp
    src/auth/session.cpp
    src/controllers/api_controller.cpp
    src/repositories/user_repository.cpp
    src/repositories/email_code_repository.cpp
    src/repositories/score_repository.cpp
)
set_target_properties(tetris_server PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_include_directories(tetris_server PRIVATE ${MYSQL_INCLUDE_DIR} src)
target_link_libraries(tetris_server PRIVATE
    ${MYSQL_CLIENT_LIBRARY}
    CURL::libcurl
    Drogon::Drogon)
if(NOT WIN32)
    target_link_libraries(tetris_server PRIVATE pthread)
endif()
