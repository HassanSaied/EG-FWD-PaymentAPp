cmake_minimum_required (VERSION 3.13)
project (PaymentApplication)

SET(SRC_FILES Application/app.c Card/card.c Server/server.c Terminal/terminal.c Utilities/inputUtils.c)

add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})