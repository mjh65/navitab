
# Replace the LVGL custom.cmake file with a version that does not include the Neon and Helium assembler sources.
# These give the MSVC toolset problems, and they are not required in Navitab.
file(MD5 "${CMAKE_CURRENT_LIST_DIR}/patches/custom.cmake" refhash)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/env_support/cmake/custom.cmake")
    file(MD5 "${CMAKE_CURRENT_SOURCE_DIR}/env_support/cmake/custom.cmake" trghash)
else()
    set(trghash "")
endif()
if(NOT trghash STREQUAL refhash)
    execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_LIST_DIR}/patches/custom.cmake" "${CMAKE_CURRENT_SOURCE_DIR}/env_support/cmake/custom.cmake")
endif()
