
# Default version values in case we can't get them from git.
set(VERSION_MAJOR 0)
set(VERSION_MINOR 3)
set(VERSION_PATCH 0)

if (NOT GIT_EXECUTABLE)
  message(WARNING "Git not found; falling back to hard-coded version")
else ()
  message(STATUS "Using git to determine version")

  execute_process(
    COMMAND "${GIT_EXECUTABLE}" describe --tags --abbrev=0
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE GIT_DESCRIBE_RESULT
    OUTPUT_VARIABLE GIT_DESCRIBE_OUTPUT
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if (GIT_DESCRIBE_RESULT EQUAL 0)
    if (GIT_DESCRIBE_OUTPUT MATCHES "^v([0-9]+)[.]([0-9]+)[.]([0-9]+)[.]*")
      set(VERSION_MAJOR ${CMAKE_MATCH_1})
      set(VERSION_MINOR ${CMAKE_MATCH_2})
      set(VERSION_PATCH ${CMAKE_MATCH_3})
      message(STATUS "Detected version from latest git tag: ${GIT_DESCRIBE_OUTPUT}")
    else()
      message(WARNING "Failed to get version from git: ${GIT_DESCRIBE_RESULT}; falling back to hard-coded version")
    endif()
  endif()
endif()

message(STATUS "Configuring netremote version v${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
