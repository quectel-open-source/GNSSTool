option(USEVERSIONFILE "use version file" ON)

function(CMake_Version inipath rcpath)
    message("${inipath} ${CMAKE_CURRENT_FUNCTION} ${inipath}")
    message("${PROJECT_NAME} ${CMAKE_CURRENT_FUNCTION} ${PROJECT_VERSION}")
    message("${PROJECT_NAME} ${CMAKE_CURRENT_FUNCTION} ${PROJECT_VERSION_MAJOR}")
    message("${PROJECT_NAME} ${CMAKE_CURRENT_FUNCTION} ${PROJECT_VERSION_MINOR}")

configure_file(
  ${inipath}
  ${CMAKE_CURRENT_BINARY_DIR}/version.h
  @ONLY
  )
  if(MSVC)
  configure_file(
  ${rcpath}
  ${CMAKE_CURRENT_BINARY_DIR}/version.rc
  )

  endif()
endfunction(CMake_Version)
