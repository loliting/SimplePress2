# This file is part of Simple Press 2
# Copyright (C) 2022 Karol Maksymowicz
#
# Simple Press 2 is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, If not,
# see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.10)

if(WIN32 OR APPLE)
	set(CMAKE_PREFIX_PATH "$ENV{QTDIR}")
endif()

project(SimplePress2 VERSION 1.0 LANGUAGES CXX)

set(CXX_STANDARD C++11)

set(PROJECT_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/Presentation.cpp
    src/PresentationWindow.cpp
    src/Application.cpp
    src/PresentationSlideView.cpp
)

set(HEADER_FILES
    include/MainWindow.hpp
    include/Presentation.hpp
    include/PresentationWindow.hpp
    include/Application.hpp
    include/PresentationSlideView.hpp
)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(libzip REQUIRED)
set(CMAKE_AUTOMOC ON)

qt_add_resources(PROJECT_SOURCES Resources/res.qrc)

qt_wrap_cpp(${Target_Name} PROJECT_SOURCES ${HEADER_FILES}) # MOC for header files in seperate folder


if(WIN32)
    set(WIN32_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Resources/res.rc")
    add_executable(${PROJECT_NAME} WIN32 ${WIN32_RESOURCES} ${PROJECT_SOURCES})
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE icon.icns)
    set(APP_ICON_MACOSX ${CMAKE_CURRENT_SOURCE_DIR}/Resources/icon.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set(CMAKE_OSX_ARCHITECTURES arm64;x86_64)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${APP_ICON_MACOSX} ${PROJECT_SOURCES} Resources/libzip.5.dylib Resources/libzstd.1.dylib)
else()
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
endif()

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Resources/libzip.5.dylib")

    set(RESOURCE_FILES
    Resources/libzip.5.dylib
    Resources/libzstd.1.dylib)
    
    set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    OUTPUT_NAME "Simple Press 2"
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Info.plist
    RESOURCE "${RESOURCE_FILES}")
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE libzip::zip)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
