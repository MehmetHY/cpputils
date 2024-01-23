# C++ Utilities
A C++ library that provides commonly used types. 

## Include
### CMake
```cmake
include(FetchContent)

FetchContent_Declare(cpputils_git
    GIT_REPOSITORY https://github.com/MehmetHY/cpputils.git
    GIT_SHALLOW 1
)

FetchContent_MakeAvailable(cpputils_git)

target_link_libraries(my_project PRIVATE cpputils::cpputils)
```

### Use directly
Since this is a header-only library, you can directly copy-paste or include 'cpputils/include' into your project. 
