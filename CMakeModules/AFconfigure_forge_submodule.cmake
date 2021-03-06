# Copyright (c) 2019, ArrayFire
# All rights reserved.
#
# This file is distributed under 3-clause BSD license.
# The complete license agreement can be obtained at:
# http://arrayfire.com/licenses/BSD-3-Clause

set(ArrayFireInstallPrefix ${CMAKE_INSTALL_PREFIX})
set(ArrayFireBuildType ${CMAKE_BUILD_TYPE})
set(CMAKE_INSTALL_PREFIX ${ArrayFire_BINARY_DIR}/extern/forge/package)
set(CMAKE_BUILD_TYPE Release)
set(FG_BUILD_EXAMPLES OFF CACHE BOOL "Used to build Forge examples")
set(FG_BUILD_DOCS OFF CACHE BOOL "Used to build Forge documentation")
set(FG_WITH_FREEIMAGE OFF CACHE BOOL "Turn on usage of freeimage dependency")
if (AF_BUILD_FORGE)
  add_subdirectory(extern/forge)
else (AF_BUILD_FORGE)
  add_subdirectory(extern/forge EXCLUDE_FROM_ALL)
endif (AF_BUILD_FORGE)
mark_as_advanced(
    FG_BUILD_EXAMPLES
    FG_BUILD_DOCS
    FG_WITH_FREEIMAGE
    FG_USE_WINDOW_TOOLKIT
    FG_USE_SYSTEM_CL2HPP
    FG_ENABLE_HUNTER
    glfw3_DIR
    glm_DIR
    )
set(CMAKE_BUILD_TYPE ${ArrayFireBuildType})
set(CMAKE_INSTALL_PREFIX ${ArrayFireInstallPrefix})

if (AF_BUILD_FORGE)
  install(FILES
      $<TARGET_FILE:forge>
      $<TARGET_SONAME_FILE:forge>
      DESTINATION "${AF_INSTALL_LIB_DIR}"
      COMPONENT common_backend_dependencies)
endif (AF_BUILD_FORGE)
