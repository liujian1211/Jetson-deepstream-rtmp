cmake_minimum_required( VERSION 2.8 )

# 声明一个 cmake 工程
project(test_websocket)

set(CUDA_VER "10.2")  # 设置CUDA_VER的值，如果未设置则报错
if(NOT CUDA_VER)
    message(FATAL_ERROR "CUDA_VER is not set")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -Wno-error=deprecated-declarations")
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -std=c++11 -Wno-error=deprecated-declarations")

#添加OPENCV库
#指定OpenCV版本，代码如下
#find_package(OpenCV 4.2 REQUIRED)
#如果不需要指定OpenCV版本，代码如下
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)

PKG_CHECK_MODULES(GST REQUIRED gstreamer-1.0)
PKG_CHECK_MODULES(GLIB REQUIRED glib-2.0)
pkg_search_module(GST REQUIRED gstreamer-1.0>=1.4 gstreamer-sdp-1.0>=1.4 gstreamer-video-1.0>=1.4 gstreamer-app-1.0>=1.4)



set(TARGET_DEVICE ${CMAKE_SYSTEM_PROCESSOR})
if(TARGET_DEVICE STREQUAL "aarch64")
    # set(CFLAGS "-DPLATFORM_TEGRA")
	add_definitions("-DPLATFORM_TEGRA")
endif()

file(GLOB SRCS "*.cpp")
file(GLOB INCS "*.h")

set(NVDS_VERSION 5.0)
set(LIB_INSTALL_DIR "/opt/nvidia/deepstream/deepstream-${NVDS_VERSION}/lib/")
set(APP_INSTALL_DIR "/opt/nvidia/deepstream/deepstream-${NVDS_VERSION}/bin/")

#cuda部分的头文件和库
find_package(CUDA REQUIRED)
include_directories(${CUDA_INCLUDE_DIRS})
include_directories("../includes")
include_directories("/usr/local/cuda-${CUDA_VER}/include")

#deepstream部分的头文件和库
include_directories(${GST_INCLUDE_DIRS}
					/opt/nvidia/deepstream/deepstream-5.0/sources/includes
					/usr/include/gstreamer-1.0
					./include)

link_directories(
				/opt/nvidia/deepstream/deepstream-5.0/lib)

#=====deepsort以及Eigen部分=====
# include_directories(./deepsort/include)

# include_directories(/usr/include/eigen3)
# aux_source_directory(${PROJECT_SOURCE_DIR}/deepsort/src SRC_DIR)

# add_library(deepsort SHARED ${SRC_DIR})
# target_link_libraries(deepsort 
#     ${CUDA_LIBS} ${OpenCV_LIBS} 
#     cudart nvinfer nvonnxparser
# )

#添加OpenCV头文件
include_directories(${OpenCV_INCLUDE_DIRS})  #OpenCV_INCLUDE_DIRS为固定API，无需使用set自定义


set(SRCFILES customparser_yolov5/nvdsinfer_yolo_engine.cpp
			 customparser_yolov5/nvdsparsebbox_Yolo.cpp
			 customparser_yolov5/trt_utils.cpp
            #  customparser_yolov5/yolo.cpp
            #  customparser_yolov5/yoloPlugins.cpp
)

set(TARGET_LIB libnvdsinfer_custom_impl_Yolo_coco)  #需要生成libnvdsinfer_custom_impl_Yolo.so库

add_library(${TARGET_LIB} SHARED ${SRCFILES}) #生成TARGET_LIB动态库(即libnvdsinfer_custom_impl_Yolo.so)，由SRCFILES生成,生成的库在build里

# SET(CMAKE_BUILD_TYPE Debug)


# 添加可执行程序
add_executable(tcp_ip_message src/tcp_ip_message.cpp
							  src/messageGenerate.cpp
							  src/getGPS.cpp)
target_link_libraries(tcp_ip_message -pthread
									 -lserial)	

# add_executable(gps src/getGPS.cpp)
# target_link_libraries(gps -lserial
# 						  -lpthread)

add_executable(target main.cpp
				src/deepstream_test.cpp
				# src/Trtyolosort.cpp 				
					)

# 将库文件链接到可执行程序target上
target_link_libraries(target  ${OpenCV_LIBS}
							  pthread 
							  ${GST_LIBRARIES}
							  -lavutil -lswresample -lavcodec -lavformat
							  ${TARGET_LIB}
							  nvinfer_plugin
							  nvinfer
							  nvparsers
							  nvdsgst_meta
							  nvds_meta
							  ${CUDA_LIBRARIES}
							#   cudart
							  cublas
							  stdc++fs)
