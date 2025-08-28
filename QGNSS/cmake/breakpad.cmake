set(breakpadPath ${CMAKE_SOURCE_DIR}/submodule/breakpad)
file(GLOB_RECURSE  BreakpadHeads ${breakpadPath}/*.h)

include_directories(${breakpadPath})
