


# Overwite the default config.h with one that's tailored for Navitab.
execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_LIST_DIR}/patches/config.h" "${mupdf_SOURCE_DIR}/include/mupdf/fitz/config.h")
