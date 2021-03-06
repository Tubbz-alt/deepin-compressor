#1.定义cmake版本
cmake_minimum_required(VERSION 3.9.5)

#2.设置项目名称
set(EXE_NAME compressor)
project(${EXE_NAME})

#SET(CMAKE_BUILD_TYPE "Debug")
message("CXXFLAGS: $ENV{CXXFLAGS}\nCMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# 添加c++ 11标准支持
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -std=c++11" )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--as-needed -fPIE") #-Wl,--as-needed 减少无效的库链接，用不到的库需要清理

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -g -ggdb -Wl,-rpath=./compressor-lib:/usr/lib/deepin-compressor/plugins:/lib") #-Wl,-rpath=./
else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wl,-O1")
endif()

add_definitions("-DQT_MESSAGELOGCONTEXT")
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#对DApplication 进行加速
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie")

#龙芯特有编译参数 -start
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "mips64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi")
    message(STATUS "this system is mips64=========\n${CMAKE_CXX_FLAGS}")
endif()
#龙芯特有编译参数 -end

#模糊测试设置 -start
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
 #   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
 #   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
  #  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
  #  set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fsanitize=address")
endif()
#模糊测试设置 -end

# Instruct CMake to run moc automatically when needed
# 打开全局moc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#3.1 设置lib输出目录
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/compressor-lib)
#3.2 设置可执行程序输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/)

#add_subdirectory(3rdparty/interface)

#add_subdirectory(3rdparty/ChardetDetector)

add_subdirectory(3rdparty) 

add_subdirectory(deepin-compressor)

option(USE_TEST "UT" ON)
if (USE_TEST)
    add_subdirectory(test)
endif(USE_TEST)

set(CMAKE_INSTALL_PREFIX /usr)

#file(GLOB INSTALL_DESKTOP_FILES deepin-compressor/*.desktop)
#install(FILES ${INSTALL_DESKTOP_FILES} DESTINATION share/deepin/dde-file-manager/oem-menuextensions/)

#file(GLOB INSTALL_LIBS_FILES ${PROJECT_SOURCE_DIR}/lib/*.so)
#install(FILES ${INSTALL_LIBS_FILES} DESTINATION lib/deepin-compressor/plugins/)

install(TARGETS cli7zplugin DESTINATION lib/deepin-compressor/plugins/)
install(TARGETS clirarplugin DESTINATION lib/deepin-compressor/plugins/)
install(TARGETS cliunarchiverplugin DESTINATION lib/deepin-compressor/plugins/)
install(TARGETS clizipplugin DESTINATION lib/deepin-compressor/plugins/)
install(TARGETS libzipplugin DESTINATION lib/deepin-compressor/plugins/)
install(TARGETS readwritelibarchiveplugin DESTINATION lib/deepin-compressor/plugins/)
install(TARGETS readonlylibarchiveplugin DESTINATION lib/deepin-compressor/plugins/)
install(TARGETS bz2plugin DESTINATION lib/deepin-compressor/plugins/)
install(TARGETS gzplugin DESTINATION lib/deepin-compressor/plugins/)
install(TARGETS xzplugin DESTINATION lib/deepin-compressor/plugins/)

#install(TARGETS compressor-interface DESTINATION lib/deepin-compressor/plugins/)
#install(TARGETS compressor-ChardetDetector DESTINATION lib/deepin-compressor/plugins/)
#install(TARGETS compressor-common DESTINATION lib/deepin-compressor/plugins/)
#install(TARGETS compressor-archive DESTINATION lib/deepin-compressor/plugins/)
install(TARGETS compressor-singlefile DESTINATION lib/deepin-compressor/plugins/)


