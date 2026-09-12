cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(main)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#Geant4 config
find_package(
    Geant4 REQUIRED ui_all vis_all
)
include(
    ${Geant4_USE_FILE}   
)
#Root config
find_package(
    ROOT REQUIRED
)
include(
    ${ROOT_USE_FILE}
)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${ROOT_INCLUDE_DIRS}
)

file(
    GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc
)

file(
    GLOB MACRO_FILES "macros/*.mac"
)

file(
    COPY ${MACRO_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

file(
    GLOB DATA_FILES "*.dat"
)

file(
    COPY ${DATA_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(
    main main.cc ${sources}
)

target_link_libraries(
    main ${Geant4_LIBRARIES}
)

add_custom_target(
    Prova DEPENDS main
)

#For the rec
#add_subdirectory(rec EXCLUDE_FROM_ALL)