# This is a Navitab project file. See the project documentation for further information.

# This function applies a path prefix to a list of relative sources.
# It's mainly useful for downloaded packages which are built with a
# custom project file that's not in the same source tree.

function(PrefixSources varname pfx)
    foreach(src ${${varname}})
        cmake_path(APPEND afp "${pfx}" "${src}")
        list(APPEND srcs "${afp}")
    endforeach()
    set(${varname} ${srcs} PARENT_SCOPE)
endfunction()
