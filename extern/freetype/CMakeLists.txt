

# TODO: This is disabled until we enhance bzip2 to provide a pc (pkgconfig) file
# And maybe we don't even need this feature?
set(FT_DISABLE_BZIP2 ON CACHE INTERNAL "")

# This is disabled due to the circular dependency with freetype
set(FT_DISABLE_HARFBUZZ ON CACHE INTERNAL "")

include_directories("${zlib_SOURCE_DIR}" "${zlib_BINARY_DIR}")

FetchContent_Declare(freetype
    URL "https://download.savannah.gnu.org/releases/freetype/freetype-2.13.3.tar.gz"
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(freetype)
