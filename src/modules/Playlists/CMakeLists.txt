cmake_minimum_required(VERSION 2.8.6)
if(COMMAND cmake_policy)
    if(POLICY CMP0003)
        cmake_policy(SET CMP0003 NEW)
    endif()
    if(POLICY CMP0020)
        cmake_policy(SET CMP0020 NEW)
    endif()
    if(POLICY CMP0043)
        cmake_policy(SET CMP0043 NEW)
    endif()
endif()
project(Playlists)

set(Playlists_HDR
    Playlists.hpp
    PLS.hpp
    M3U.hpp
)

set(Playlists_SRC
    Playlists.cpp
    PLS.cpp
    M3U.cpp
)

set(Playlists_RESOURCES
    icon.qrc
)

include_directories(../../qmplay2/headers)

if(USE_QT5)
    qt5_add_resources(Playlists_RESOURCES_RCC ${Playlists_RESOURCES})
else()
    qt4_add_resources(Playlists_RESOURCES_RCC ${Playlists_RESOURCES})
endif()

add_library(${PROJECT_NAME} MODULE
    ${Playlists_HDR}
    ${Playlists_SRC}
    ${Playlists_RESOURCES_RCC}
)

if(USE_QT5)
    qt5_use_modules(${PROJECT_NAME} Gui Widgets)
else()
    target_link_libraries(${PROJECT_NAME} Qt4::QtCore Qt4::QtGui)
endif()

add_dependencies(${PROJECT_NAME} qmplay2)
target_link_libraries(${PROJECT_NAME}
    ${libqmplay2}
)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION "${MODULES_INSTALL_PATH}")
