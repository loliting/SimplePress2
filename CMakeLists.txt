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

cmake_minimum_required(VERSION 3.20)

if(WIN32)
	set(CMAKE_PREFIX_PATH "$ENV{QTDIR}") # When configurating this project on Windows you need to specify a environment variable QTDIR with a path to qt6 instalation.
elseif(APPLE)
	set(CMAKE_PREFIX_PATH "~/Qt/6.3.0/macos") # You might wanna change that.
endif()

project(SimplePress2 VERSION 1.0)

set(CXX_STANDARD C++11)

set(PROJECT_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/Presentation.cpp
    src/PresentationWindow.cpp
    src/Application.cpp
)

set(HEADER_FILES
    include/MainWindow.hpp
    include/Presentation.hpp
    include/PresentationWindow.hpp
    include/Application.hpp
)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(libzip REQUIRED)
set(CMAKE_AUTOMOC ON)

qt_add_resources(PROJECT_SOURCES Resources/res.qrc)

qt_wrap_cpp(${Target_Name} PROJECT_SOURCES ${HEADER_FILES}) # MOC for header files in seperate folder

set(WIN32_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Resources/res.rc")
set(MACOSX_BUNDLE_ICON_FILE icon.icns)
set(APP_ICON_MACOSX ${CMAKE_CURRENT_SOURCE_DIR}/Resources/icon.icns)
set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${WIN32_RESOURCES} ${PROJECT_SOURCES})
elseif(APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${APP_ICON_MACOSX} ${PROJECT_SOURCES})
    set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Info.plist
    )
else()
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE libzip::zip)
