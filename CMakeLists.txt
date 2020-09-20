cmake_minimum_required(VERSION 3.17)
project(studychat)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS core widgets gui network REQUIRED)

set(SOURCE_FILES
        main.cpp
        studychat.cpp
        server.cpp
        clientsocket.cpp
        Headers/studychat.h
        Headers/server.h
        Headers/clientsocket.h
        studychat.ui
        )

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)