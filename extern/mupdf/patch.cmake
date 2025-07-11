
# Provide a top-level CMakeLists.txt file for MuPDF (it doesn't have one of its own)
file(MD5 "${CMAKE_CURRENT_LIST_DIR}/patches/CMakeLists.txt" refhash)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")
    file(MD5 "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt" trghash)
else()
    set(trghash "")
endif()
if(NOT trghash STREQUAL refhash)
    execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_LIST_DIR}/patches/CMakeLists.txt" "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")
endif()

# Overwite the default config.h with one that's tailored for Navitab.
file(MD5 "${CMAKE_CURRENT_LIST_DIR}/patches/config.h" refhash)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/mupdf/fitz/config.h")
    file(MD5 "${CMAKE_CURRENT_SOURCE_DIR}/include/mupdf/fitz/config.h" trghash)
else()
    set(trghash "")
endif()
if(NOT trghash STREQUAL refhash)
    execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_LIST_DIR}/patches/config.h" "${CMAKE_CURRENT_SOURCE_DIR}/include/mupdf/fitz/config.h")
endif()

