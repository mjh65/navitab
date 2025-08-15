# This file is part of the Navitab project. See the README and LICENSE for details.

# This function 'exports' (copies) the third-party packages' libraries
# to a common location, and gives each library a well-known name.
function(ExportThirdPartyLibrary name pathvar)
    if(NOT "${NAVITAB_${name}_LOCATION}" STREQUAL "")
        if(MSVC)
            set(outlib "${NAVITAB_IMPORT_LIBS}/${name}.lib")
        else()
            set(outlib "${NAVITAB_IMPORT_LIBS}/lib${name}.a")
        endif()
        add_custom_command(
            OUTPUT "${outlib}"
            COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:${name}>" "${outlib}"
            DEPENDS ${name}
            VERBATIM
        )
        add_library(${name}_3rd INTERFACE "${outlib}")
        set(${pathvar} ${outlib} PARENT_SCOPE)
    endif()
endfunction()

# This function 'imports' the third-party packages' libraries assuming they were
# previously installed.
function(ImportBuiltLibraries name fullbuild)
    set(impname ${name}_3rd)
    if(NOT TARGET ${impname})
        if("${NAVITAB_${name}_LOCATION}" STREQUAL "")
            add_library(${impname} INTERFACE IMPORTED GLOBAL)
        else()
            add_library(${impname} STATIC IMPORTED GLOBAL)
            set_target_properties(${impname} PROPERTIES IMPORTED_LOCATION "${NAVITAB_${name}_LOCATION}")
        endif()
    else()
        target_link_libraries(${impname} INTERFACE ${NAVITAB_${name}_LOCATION})
    endif()
    # include dirs
    if(NOT "${NAVITAB_${name}_INCDIRS}" STREQUAL "")
        target_include_directories(${impname} INTERFACE ${NAVITAB_${name}_INCDIRS})
    endif()
    # preproc definitions
    if(NOT "${NAVITAB_${name}_CPPDEFS}" STREQUAL "")
        target_compile_definitions(${impname} INTERFACE ${NAVITAB_${name}_CPPDEFS})
    endif()
endfunction()
