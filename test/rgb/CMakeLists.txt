include(../../cmake/TaiheWorkflows.cmake)
setup_taihe_cmake_env()
cmake_minimum_required(VERSION 3.18)
project(rgb)

file(GLOB IDL_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/idl/*.taihe")

file(GLOB AUTHOR_CPP_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/author/src/*.cpp")

file(GLOB USER_CPP_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/user/src/*.cpp")

set(USER_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/user/include)


if (NOT DEFINED TH_STDLIB_DIR)  
execute_and_set_variable(TH_STDLIB_DIR "--print-stdlib-path")
endif()

if (NOT DEFINED TH_RUNTIME_HEADER_DIR)  
execute_and_set_variable(TH_RUNTIME_HEADER_DIR "--print-runtime-header-path")
endif()

# TODO: need to remove, stdlib should not take user as input
set(IDL_FILES_WITH_STDLIB ${IDL_FILES} "${TH_STDLIB_DIR}/taihe.platform.ani.taihe")

set(MAIN_ABC "${CMAKE_CURRENT_BINARY_DIR}/main.abc")

# TODO: need to remove, now runtime is coupled with ani
# panda sdk 环境配置
setup_panda_sdk()
# ani 头文件
setup_ani_header()

add_taihe_library(rgb_taihegen "${IDL_FILES_WITH_STDLIB}" "cpp-author" "cpp-user" "" GEN_HEADER_DIR)

add_library(rgb SHARED ${AUTHOR_CPP_FILES})
target_compile_options(rgb PRIVATE "-Wno-attributes")
set_target_properties(rgb PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(rgb PRIVATE rgb_taihegen)
target_link_options(rgb PRIVATE "-Wl,--no-undefined")
target_include_directories(rgb
    PUBLIC
        ${GEN_HEADER_DIR}
        ${TH_RUNTIME_HEADER_DIR}
)

add_executable(rgb_program ${USER_CPP_FILES})
target_include_directories(rgb_program
    PRIVATE
        ${GEN_HEADER_DIR}
        ${USER_INCLUDE}
        ${TH_RUNTIME_HEADER_DIR}
)
target_link_libraries(rgb_program PRIVATE rgb)

add_custom_command(TARGET rgb_program POST_BUILD
    COMMAND rgb_program
    COMMENT "Running rgb_program"
)