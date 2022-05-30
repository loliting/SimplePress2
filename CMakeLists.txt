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

if(WIN32)
	set(CMAKE_PREFIX_PATH "$ENV{QTDIR}") # When configurating this project on Windows you need to specify a environment variable QTDIR with a path to qt6 instalation.
elseif(APPLE)
	set(CMAKE_PREFIX_PATH "~/Qt/6.3.0/macos") # You might wanna change that.
endif()

project(SimplePress2 VERSION 1.0 LANGUAGES CXX)

set(PROJECT_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/Presentation.cpp
)

set(HEADER_FILES
    include/MainWindow.hpp
    include/Presentation.hpp
)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(libzip REQUIRED)
set(CMAKE_AUTOMOC ON)

qt_wrap_cpp(${Target_Name} PROJECT_SOURCES ${HEADER_FILES}) # MOC for header files in seperate folder

add_executable(${PROJECT_NAME} WIN32 ${WIN32_RESOURCES}
    MACOSX_BUNDLE ${APP_ICON_MACOSX}
    ${PROJECT_SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE libzip::zip)
