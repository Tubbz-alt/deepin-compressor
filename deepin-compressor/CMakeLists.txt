#1.定义cmake版本
cmake_minimum_required(VERSION 3.9.5)
#2.设置项目名称
set(EXE_NAME deepin-compressor)
project(${EXE_NAME})

#配置版本号
configure_file(environments.h.in environments.h @ONLY)

#SET(CMAKE_BUILD_TYPE "Debug")
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")


#3.项目相关设置
#3.1 包含当前路径，使用*.ui文件时，需要加上这句，否则找不到头文件
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 测试点
option(PERF_ON "open the info of performance testing" ON)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Accessibility自动化
option(ACCESSIBILITY_ON "Support automated test" ON)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

#3.3 打开全局moc
set(CMAKE_AUTOMOC ON)
#set(CMAKE_CXX_FLAGS "-std=c++11")
#set(CMAKE_CXX_FLAGS_DEBUG "-O0")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")


#demo start
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_CXX_FLAGS "-g -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ldtkwidget -ldtkgui -ldtkcore")#${EXE_NAME}
#-ldtkwidget -ldtkgui -ldtkcore
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "sw_64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
endif ()


#4 查找依赖关系的包信息
find_package(PkgConfig REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5 REQUIRED Widgets)
find_package(Qt5Concurrent)
find_package(Qt5Svg REQUIRED SvgWidgets)
find_package(KF5Codecs REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
include(FindPkgConfig)


pkg_search_module(DtkWidget REQUIRED dtkwidget)
include_directories(${DtkWidget_INCLUDE_DIRS})

pkg_search_module(DtkCore REQUIRED dtkcore)
include_directories(${DtkCore_INCLUDE_DIRS})

pkg_search_module(GOBJECT REQUIRED gobject-2.0)
include_directories(${GOBJECT_INCLUDE_DIRS})

pkg_search_module(GIO REQUIRED gio-unix-2.0)
include_directories(${GIO_INCLUDE_DIRS})

pkg_search_module(DtkGui REQUIRED dtkgui)	
include_directories(${DtkGui_INCLUDE_DIRS})

set(LIBS_LIBRARIES_DIRS
        ${DtkWidget_LIBRARIES}
        ${DtkCore_LIBRARIES}
        ${GOBJECT_LIBRARIES}
        ${GIO_LIBRARIES}
        ${DtkGui_LIBRARIES}
)

set(LIBS_INCLUDE_DIRS
        ${DtkWidget_INCLUDE_DIRS}
        ${DtkCore_INCLUDE_DIRS}
        ${DtkGui_INCLUDE_DIRS}
        ${GOBJECT_INCLUDE_DIRS}
        ${GIO_INCLUDE_DIRS}
)

 
#6. 应用interface项目
#interface的h文件
include_directories(${PROJECT_SOURCE_DIR}/../3rdparty/interface)
#add_subdirectory(interface)

#7.主程序的h文件
include_directories(${PROJECT_SOURCE_DIR}/source/inc)
include_directories(${PROJECT_SOURCE_DIR}/source/inc/openwithdialog)

#8.主程序的源文件和头文件
file(GLOB_RECURSE c_files  source/*.cpp source/*.c main.cpp accessible.h accessibledefine.h DebugTimeManager.cpp)
file(GLOB_RECURSE h_files  source/*.h DebugTimeManager.h)


file(GLOB_RECURSE res_files *.qrc)
set(TS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../translations")
file( GLOB TS_FILES    ${TS_DIR}/*.ts)

QT5_ADD_RESOURCES(QRC_MOC ${res_files})

find_program(LUPDATE_EXECUTABLE lupdate)

##编译时更新.ts .qm
#foreach(_ts_file ${TS_FILES})
#    execute_process(
#        COMMAND ${LUPDATE_EXECUTABLE} -recursive ${CMAKE_SOURCE_DIR} -ts ${_ts_file})
#    execute_process(COMMAND lrelease ${_ts_file})
#endforeach()
##lupdate end

file (GLOB APP_TS_FILES ../translations/*.ts)

qt5_create_translation(APP_QM_FILES ${APP_TS_FILES} ${APP_QM_FILES})

#INCLUDE(${QT_USE_FILE})
#ADD_DEFINITIONS(${QT_DEFINITIONS})

#9.链接编译的lib
link_directories(compressor-lib compressor-interface)
link_directories(compressor-lib compressor-ChardetDetector)


#10.生成可执行程序
add_executable(${EXE_NAME}  ${c_files} ${h_files} ${QRC_MOC} ${APP_TS_FILES} ${APP_QM_FILES})
target_include_directories(${EXE_NAME} PRIVATE ${LIBS_INCLUDE_DIRS})


set_target_properties(${EXE_NAME} PROPERTIES AUTOMOC TRUE)
qt5_use_modules(${EXE_NAME}  Core  Widgets  Gui )

target_include_directories(${EXE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/source/inc )
target_include_directories(${EXE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/source/inc/openwithdialog )

target_link_libraries(${EXE_NAME}
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::DBus
    Qt5::Svg
    KF5::Codecs
    ${LIBS_LIBRARIES_DIRS}
    compressor-interface
    compressor-ChardetDetector
)


set(CMAKE_INSTALL_PREFIX /usr)


install(FILES ../output/deepin-compressor
        DESTINATION bin/
        PERMISSIONS OWNER_READ OWNER_EXECUTE
        GROUP_READ GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE)

install(FILES  deepin-compressor.desktop
        DESTINATION share/applications/
        PERMISSIONS OWNER_READ OWNER_EXECUTE
        GROUP_READ GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE)

file(GLOB INSTALL_QM_FILES ../translations/*.qm)
install(FILES ${INSTALL_QM_FILES} 
        DESTINATION share/deepin-compressor/translations)


# for i in `ls *.desktop`; do echo "install(FILES ${i} DESTINATION share/deepin/dde-file-manager/oem-menuextensions/)";done

install(FILES deepin-compress.desktop DESTINATION share/deepin/dde-file-manager/oem-menuextensions/)
install(FILES deepin-compress-multi.desktop DESTINATION share/deepin/dde-file-manager/oem-menuextensions/)
#install(FILES deepin-compressor.desktop DESTINATION share/deepin/dde-file-manager/oem-menuextensions/)
#install(FILES deepin-compress-to_7z.desktop DESTINATION share/deepin/dde-file-manager/oem-menuextensions/)
#install(FILES deepin-compress-to_zip.desktop DESTINATION share/deepin/dde-file-manager/oem-menuextensions/)
install(FILES deepin-decompress.desktop DESTINATION share/deepin/dde-file-manager/oem-menuextensions/)
install(FILES deepin-decompresshere.desktop DESTINATION share/deepin/dde-file-manager/oem-menuextensions/)
install(FILES deepin-decompresshere-multi.desktop DESTINATION share/deepin/dde-file-manager/oem-menuextensions/)
install(FILES deepin-decompresshere-split.desktop DESTINATION share/deepin/dde-file-manager/oem-menuextensions/)
install(FILES deepin-decompresshere-split_multi.desktop DESTINATION share/deepin/dde-file-manager/oem-menuextensions/)
#install(FILES deepin-decompressmkdir.desktop DESTINATION share/deepin/dde-file-manager/oem-menuextensions/)
#install(FILES deepin-decompressmkdir-split.desktop DESTINATION share/deepin/dde-file-manager/oem-menuextensions/)
install(FILES deepin-decompress-multi.desktop DESTINATION share/deepin/dde-file-manager/oem-menuextensions/)
install(FILES deepin-decompress-split.desktop DESTINATION share/deepin/dde-file-manager/oem-menuextensions/)
install(FILES deepin-decompress-split_multi.desktop DESTINATION share/deepin/dde-file-manager/oem-menuextensions/)

install(FILES assets/icons/deepin/builtin/icons/deepin-compressor.svg DESTINATION share/icons/hicolor/scalable/apps/)
install(FILES assets/mimetype/deepin-compressor.xml DESTINATION share/mime/packages/)

