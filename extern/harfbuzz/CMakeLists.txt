
set(HB_HAVE_FREETYPE ON CACHE INTERNAL "")

FetchContent_Declare(harfbuzz
    URL "https://github.com/harfbuzz/harfbuzz/archive/refs/tags/11.2.1.tar.gz"
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(HarfBuzz)
