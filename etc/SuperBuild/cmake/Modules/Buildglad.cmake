include(ExternalProject)

if(DJV_GL_ES2)
    set(glad_SOURCE_DIR ${CMAKE_SOURCE_DIR}/gladGL2ES)
else()
    set(glad_SOURCE_DIR ${CMAKE_SOURCE_DIR}/gladGL4_1)
endif()

set(glad_ARGS
    -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_POLICY_DEFAULT_CMP0091:STRING=NEW
    -DCMAKE_FIND_FRAMEWORK=${CMAKE_FIND_FRAMEWORK}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
    -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS})
if(BUILD_SHARED_LIBS)
    set(glad_ARGS ${glad_ARGS} -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE)
endif()
if(CMAKE_CXX_STANDARD)
    set(glad_ARGS ${glad_ARGS} -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD})
endif()

ExternalProject_Add(
    glad
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/glad
    SOURCE_DIR ${glad_SOURCE_DIR}
    CMAKE_ARGS ${glad_ARGS})
