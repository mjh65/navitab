
# Replace the LVGL custom.cmake file with a version that does not include the Neon and Helium assembler sources.
# These give the MSVC toolset problems, and they are not required in Navitab.
execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_LIST_DIR}/patches/custom.cmake" "${lvgl_SOURCE_DIR}/env_support/cmake/custom.cmake")
