
set(BROTLI_DISABLE_TESTS ON CACHE INTERNAL "")

FetchContent_Declare(BrotliDec
    URL "https://github.com/google/brotli/archive/refs/tags/v1.1.0.tar.gz"
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(BrotliDec)
