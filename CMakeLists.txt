cmake_minimum_required(VERSION 3.16)

project(Occurity VERSION 1.3.1 LANGUAGES CXX)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DVERSION=${VERSION} )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

# Set build type with '-DCMAKE_BUILD_TYPE=type' where type is usually 'Debug' or 'Release'.
# Default to 'Release' type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

find_package(Qt6 REQUIRED COMPONENTS Widgets Xml SvgWidgets MultimediaWidgets Multimedia Network LinguistTools)

qt_standard_project_setup()

qt_add_resources(QT_RESOURCES
  occurity.qrc
)

set(SOURCES
  src/main.cpp
  src/mainwindow.cpp
  src/videoplayer.cpp
  src/pindialog.cpp
  src/preferences.cpp
  src/about.cpp
  src/slider.cpp
  src/combobox.cpp
  src/checkbox.cpp
  src/abstractchart.cpp
  src/svgchart.cpp
  src/optotypechart.cpp
  src/optotyperow.cpp
  src/optosymbol.cpp
  src/touchcontrolitem.cpp
  src/touchcontrols.cpp
  src/jobrunner.cpp
  src/messagebox.cpp
)

# Update translation from sources, use 'make Occurity_lupdate' to update the ts file
# This function needs to be updated when migrating to Qt6.7 or later as the format has changed
qt_add_lupdate(Occurity
  TS_FILES occurity_da_DK.ts
  SOURCES ${SOURCES}
)

qt_add_executable(Occurity
  ${SOURCES}
  ${QT_RESOURCES}
  ${APP_ICON_RESOURCE}
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
# Add LUA debug defines. I'm not sure what LUA_COMPAT_MODULE is used for but it's always been there so...
  target_compile_definitions(Occurity PRIVATE LUA_COMPAT_MODULE USE_DEBUG)
# target_compile_options(Occurity PRIVATE -g) <-- This is auto-added by cmake for a 'Debug' release
endif()

target_link_libraries(Occurity PRIVATE Qt6::Widgets Qt6::Xml Qt6::SvgWidgets Qt6::MultimediaWidgets Qt6::Multimedia Qt6::Network)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/debug CACHE PATH "Install path prefix" FORCE)
  else()
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/release CACHE PATH "Install path prefix" FORCE)
  endif()
endif()

install(TARGETS Occurity
  RUNTIME DESTINATION .
)

install(FILES
  ${CMAKE_SOURCE_DIR}/AUTHORS
  ${CMAKE_SOURCE_DIR}/charts.xml.example
  ${CMAKE_SOURCE_DIR}/LICENSE
  ${CMAKE_SOURCE_DIR}/occurity_da_DK.qm
  ${CMAKE_SOURCE_DIR}/README.md
  DESTINATION .
)

install(DIRECTORY
  ${CMAKE_SOURCE_DIR}/optotypes
  ${CMAKE_SOURCE_DIR}/docs
  ${CMAKE_SOURCE_DIR}/flirc_configs
  ${CMAKE_SOURCE_DIR}/scripts
  ${CMAKE_SOURCE_DIR}/jobs
  ${CMAKE_SOURCE_DIR}/svg
  ${CMAKE_SOURCE_DIR}/videos
  DESTINATION .
)
