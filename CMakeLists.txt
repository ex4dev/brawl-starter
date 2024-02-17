cmake_minimum_required(VERSION 3.0)
project(kocity-qt)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt6Widgets CONFIG REQUIRED)
find_package(Qt6Network REQUIRED)
find_package(Qt6DBus REQUIRED)

# Find libsecret
find_package(PkgConfig REQUIRED)
pkg_check_modules(libsecret-1 REQUIRED IMPORTED_TARGET libsecret-1)

set(kocity-qt_SRC
        src/main.cpp
        src/ui/kocity-qt.cpp
        src/ui/add_server_dialog.cpp
        src/ui/settings_dialog.cpp
        src/installer.cpp
        src/server_query.cpp
        src/launcher.cpp
        src/constants.h # TODO remove this??
        src/secrets.cpp
)

# Tell CMake to create the helloworld executable
add_executable(kocity-qt ${kocity-qt_SRC})

# Use the Widgets module from Qt 5.
target_link_libraries(kocity-qt Qt6::Widgets Qt6::Network Qt6::DBus PkgConfig::libsecret-1)

# Install the executable
install(TARGETS kocity-qt DESTINATION bin)
