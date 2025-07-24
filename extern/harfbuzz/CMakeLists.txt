
set(HB_HAVE_FREETYPE ON)
set(SKIP_INSTALL_ALL ON)

add_subdirectory("${harfbuzz_SOURCE_DIR}" "${harfbuzz_BINARY_DIR}" EXCLUDE_FROM_ALL)
