cmake_minimum_required(VERSION 3.26)
project(DA2324_PRJ1_G163)

set(CMAKE_CXX_STANDARD 20)

# Doxygen Build
find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(BUILD_DOC_DIR "${CMAKE_SOURCE_DIR}/docs/output")
    if (NOT EXISTS "${BUILD_DOC_DIR}")
        file(MAKE_DIRECTORY "${BUILD_DOC_DIR}")
    endif ()

    set(DOXYGEN_IN "${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile")
    set(DOXYGEN_OUT "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
    configure_file("${DOXYGEN_IN}" "${DOXYGEN_OUT}" @ONLY)

    message("Doxygen build started")
    add_custom_target(Doxygen ALL
            COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYGEN_OUT}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
else (DOXYGEN_FOUND)
    message("Doxygen needs to be installed to generate the documentation.")
endif (DOXYGEN_FOUND)

# Project build
add_executable(DA2324_PRJ1_G163 main.cpp
        lib/Graph.h
        lib/MutablePriorityQueue.h
        lib/UFDS.cpp lib/UFDS.h
        src/Utils.h src/Utils.cpp
        src/Parser.cpp src/Parser.h
        src/Runtime.cpp src/Runtime.h
        src/data/Data.cpp src/data/Data.h
        src/CSV.cpp src/CSV.h
        src/data/Info.cpp src/data/Info.h
)
