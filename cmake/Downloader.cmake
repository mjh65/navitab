# This is a Navitab project file. See the project documentation for further information.

# This is Navitab's simplified equivalent of FetchContent.
# It supports a local cache of all the downloaded and extracted packages to avoid unnecessary
# and duplicated downloads, particularly during development.
# In some ways it's a shallow, simplified version of git's submodule.

function(IncludeDownloadedPackage name url)
    cmake_path(APPEND cachedir "${NAVITAB_DOWNLOAD_PREFIX}" "_cache")
    cmake_path(APPEND pkgsig "${cachedir}" "${name}_srcdir")

    # if the extracted package directory does not exist then create it
    execute_process(COMMAND "${CMAKE_COMMAND}" -E cat "${pkgsig}" OUTPUT_VARIABLE srcpath RESULT_VARIABLE res ERROR_QUIET)
    if(${res} EQUAL 0)
        # check the unpacked source directory by looking for our signature file
        execute_process(COMMAND "${CMAKE_COMMAND}" -E cat "${srcpath}/_sig_navitab" RESULT_VARIABLE res ERROR_QUIET)
    endif()
    if(NOT "${res}" EQUAL 0)
        # first download the source archive if we don't already have it
        cmake_path(GET url EXTENSION ext)
        set(fname "${name}${ext}")
        cmake_path(APPEND pkgfile "${cachedir}" "${fname}")
        execute_process(COMMAND "${CMAKE_COMMAND}" -E compare_files "${pkgfile}" "${pkgfile}" RESULT_VARIABLE res)
        if(NOT "${res}" EQUAL 0)
            message(STATUS "Downloading ${name} : ${fname} <- ${url}")
            file(DOWNLOAD "${url}" "${pkgfile}" STATUS s)
            list(POP_FRONT s dlres dlerrstring)
            if(NOT ${dlres} EQUAL 0)
                message(FATAL_ERROR "Download of ${name} required package failed (${dlerrstring}).")
            endif()
        endif()
        # get a list of the files in the package
        execute_process(COMMAND "${CMAKE_COMMAND}" -E tar tf "${pkgfile}" WORKING_DIRECTORY "${cachedir}" OUTPUT_FILE _tmp.txt)
        file(STRINGS "${cachedir}/_tmp.txt" pkgcontents)
        foreach(f ${pkgcontents})
            # get the first part of the path
            string(REGEX REPLACE "/.*$" "" f "${f}")
            list(APPEND tops "${f}")
        endforeach()
        list(REMOVE_DUPLICATES tops)
        list(LENGTH tops ndirs)
        if("${ndirs}" EQUAL 1)
            # archive has one top-level directory, we can unpack directly
            list(POP_FRONT tops srcpath)
            cmake_path(APPEND srcpath "${NAVITAB_DOWNLOAD_PREFIX}" "${srcpath}")
            execute_process(COMMAND "${CMAKE_COMMAND}" -E tar xf "${pkgfile}" WORKING_DIRECTORY "${NAVITAB_DOWNLOAD_PREFIX}")
        else()
            # archive has multiple top-level items, unpack into a subdirectory
            cmake_path(APPEND srcpath "${NAVITAB_DOWNLOAD_PREFIX}" "${name}")
            execute_process(COMMAND "${CMAKE_COMMAND}" -E make_directory "${srcpath}")
            execute_process(COMMAND "${CMAKE_COMMAND}" -E tar xf "${pkgfile}" WORKING_DIRECTORY "${srcpath}")            
        endif()

        # run the patch script, if there is one
        set(${name}_SOURCE_DIR "${srcpath}")
        include("${name}/patch.cmake" OPTIONAL)

        # write the files that will bypass these steps next time.
        file(WRITE "${pkgsig}" "${srcpath}")
        file(WRITE "${srcpath}/_sig_navitab" "")
    endif()

    # the package has been downloaded and extracted and the source is in ${srcpath}
    # set some (parent) variables to provide source/build directores for the package
    set(${name}_SOURCE_DIR "${srcpath}")
    set(${name}_BINARY_DIR "${PROJECT_BINARY_DIR}/download/${name}")

    # now include Navitab's custom CMake script for the package
    message(STATUS "Configuring build for Navitab third-party package ${name}")
    unset(DOWNLOADER_PROMOTE_VARS)
    add_subdirectory("${name}")
    foreach(v ${DOWNLOADER_PROMOTE_VARS})
        set(${v} ${${v}} PARENT_SCOPE)
    endforeach()

endfunction()
