include(ExternalProject)

set(rkcommon_ARGS
    -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_INSTALL_LIBDIR=lib
    -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_POLICY_DEFAULT_CMP0091:STRING=NEW
    -DCMAKE_FIND_FRAMEWORK=${CMAKE_FIND_FRAMEWORK}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
    -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
    -DINSTALL_DEPS=OFF)
if(CMAKE_CXX_STANDARD)
    set(rkcommon_ARGS ${rkcommon_ARGS} -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD})
endif()

ExternalProject_Add(
    rkcommon
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/rkcommon
    DEPENDS TBB
    GIT_REPOSITORY https://github.com/ospray/rkcommon.git
    GIT_TAG v1.5.1
    CMAKE_ARGS ${rkcommon_ARGS})
