cmake_minimum_required(VERSION 3.16)

project(Occurity VERSION 1.3.0 LANGUAGES CXX)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DVERSION=${VERSION} )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

# Set build type defined with '-DBUILD_TYPE=type'
if(DEFINED BUILD_TYPE)
  string(TOLOWER "${BUILD_TYPE}" BUILD_TYPE)
  if(BUILD_TYPE STREQUAL "debug")
    set(CMAKE_BUILD_TYPE debug CACHE STRING "Build type" FORCE)
  elseif(BUILD_TYPE STREQUAL "release")
    set(CMAKE_BUILD_TYPE release CACHE STRING "Build type" FORCE)
  else()
    message(FATAL_ERROR "Unknown build type: ${BUILD_TYPE}")
  endif()
endif()

# Default to 'release' type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE release CACHE STRING "Build type" FORCE)
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
  src/videoserver.cpp
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

set(TRANSLATIONS
  occurity_da_DK.ts
)

add_custom_target(update_translations
  COMMAND lupdate ${SOURCES} -ts ${TRANSLATIONS}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Updating translation files..."
)

if(CMAKE_BUILD_TYPE STREQUAL "debug")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/debug)
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/release)
endif()

qt_add_executable(Occurity
  ${SOURCES}
  ${QT_RESOURCES}
  ${APP_ICON_RESOURCE}
)

if(CMAKE_BUILD_TYPE STREQUAL "debug")
  target_compile_options(Occurity PRIVATE -g)
  target_compile_definitions(Occurity PRIVATE LUA_COMPAT_MODULE USE_DEBUG)
endif()

add_dependencies(Occurity update_translations)

target_link_libraries(Occurity PRIVATE Qt6::Widgets Qt6::Xml Qt6::SvgWidgets Qt6::MultimediaWidgets Qt6::Multimedia Qt6::Network)
