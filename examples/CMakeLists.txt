include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${INCLUDE_PATH}
  )

function(add_cli_bin T)
    add_executable(${T} ${ARGN} ${HEKATE_headers})
    target_include_directories(${T} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    set_target_properties(
         ${T} PROPERTIES
         FOLDER "Examples"
         )

    if(CLANG_TIDY_BIN)
    set_target_properties(
        ${T} PROPERTIES
        CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
        )
    endif()
endfunction()

file(GLOB EXAMPLES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

foreach(EXP ${EXAMPLES})
  get_filename_component(EX ${EXP} NAME_WE)
  message(STATUS "Compiling example: ${EX}")
  add_cli_bin(${EX} ${EXP})
endforeach(EXP)
