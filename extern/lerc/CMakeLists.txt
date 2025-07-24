


add_subdirectory("${lerc_SOURCE_DIR}" "${lerc_BINARY_DIR}" EXCLUDE_FROM_ALL)


# this aims to help find_package select this lerc library to help out 
if(MSVC)
    set(LERC_LIBRARY "${lerc_BINARY_DIR}/lerc.lib" CACHE INTERNAL "")
else()
    set(LERC_LIBRARY "${lerc_BINARY_DIR}/liblerc.a" CACHE INTERNAL "")
endif()
set(LERC_INCLUDE_DIR "${lerc_SOURCE_DIR}/src/LercLib/include" CACHE INTERNAL "")
