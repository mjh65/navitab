
# This utility
function(ImportBuiltLibrary name)
    foreach(lib ${NAVITAB_${name}_PKGLIBS})
        if("${NAVITAB_${lib}_LOCATION}" STREQUAL "")
            add_library(${lib} INTERFACE)
        else()
            add_library(${lib} STATIC IMPORTED GLOBAL)
            set_property(TARGET ${lib} PROPERTY IMPORTED_LOCATION ${NAVITAB_${lib}_LOCATION})
        endif()
        # include dirs
        if(NOT "${NAVITAB_${lib}_INCDIRS}" STREQUAL "")
            target_include_directories(${lib} INTERFACE ${NAVITAB_${lib}_INCDIRS})
        endif()
        # preproc definitions
        if(NOT "${NAVITAB_${lib}_CPPDEFS}" STREQUAL "")
            target_compile_definitions(${lib} INTERFACE ${NAVITAB_${lib}_CPPDEFS})
        endif()
        # required libraries
        if(NOT "${NAVITAB_${lib}_LINKLIBS}" STREQUAL "")
            target_link_libraries(${lib} INTERFACE ${NAVITAB_${lib}_LINKLIBS})
        endif()
    endforeach()
endfunction()
