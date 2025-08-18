# This file is part of the Navitab project. See the README and LICENSE for details.

# This function 'exports' (copies) the third-party packages' libraries
# to a common location, and gives each library a well-known name.
function(ExportThirdPartyLibrary name pathvar)
    if(NOT "${NAVITAB_${name}_LOCATION}" STREQUAL "")
        if(MSVC)
            set(outlib "${NAVITAB_LIBS3RD_DIR}/${name}.lib")
        else()
            set(outlib "${NAVITAB_LIBS3RD_DIR}/lib${name}.a")
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
function(ImportBuiltLibraries name)
    set(impname ${name}_3rd)
    if(NOT TARGET ${impname})
        if("${NAVITAB_${name}_LOCATION}" STREQUAL "")
            add_library(${impname} INTERFACE IMPORTED GLOBAL)
        else()
            if(NOT ${BUILD_NAVITAB_THIRDPARTY} AND NOT EXISTS "${NAVITAB_${name}_LOCATION}")
                message(WARNING "BUILD_NAVITAB_THIRDPARTY has been disabled, but library ${NAVITAB_${name}_LOCATION} does not appear to exist. The build will fail.")
            endif()
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
