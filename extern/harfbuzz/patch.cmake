

# Overwite the default CMakeLists.txt with one that's tailored for Navitab.
execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_LIST_DIR}/patches/CMakeLists.txt" "${harfbuzz_SOURCE_DIR}/CMakeLists.txt")
