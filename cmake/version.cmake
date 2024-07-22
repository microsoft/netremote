
# Default version values in case we can't get them from git.
set(VERSION_MAJOR 0)
set(VERSION_MINOR 4)
set(VERSION_PATCH 0)

if (NOT GIT_EXECUTABLE)
  message(WARNING "Git not found; falling back to hard-coded version")
else ()
  message(STATUS "Using git to determine version")

  # Get a list of tags, reverse sorted by tag ref name such that latest tag appears first.
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" tag -l --sort=-v:refname
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE GIT_TAG_RESULT
    OUTPUT_VARIABLE GIT_TAG_OUTPUT
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # Extract the version from the latest tag.
  if (GIT_TAG_RESULT EQUAL 0)
    if (GIT_TAG_OUTPUT MATCHES "^v([0-9]+)[.]([0-9]+)[.]([0-9]+)[.]*")
      set(VERSION_MAJOR ${CMAKE_MATCH_1})
      set(VERSION_MINOR ${CMAKE_MATCH_2})
      set(VERSION_PATCH ${CMAKE_MATCH_3})
      message(STATUS "Detected version from latest git tag: v${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
    else()
      message(WARNING "Failed to get version from git: ${GIT_TAG_RESULT}; falling back to hard-coded version")
    endif()
  endif()
endif()
