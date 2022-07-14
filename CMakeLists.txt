project(video-streaming-project)
cmake_minimum_required(VERSION 3.10)

find_package(vsomeip3 REQUIRED)
include_directories(/usr/local/include/opencv4 include)
set(
    OpenCV_LIB
    /usr/local/lib/libopencv_highgui.so
    /usr/local/lib/libopencv_imgcodecs.so
    /usr/local/lib/libopencv_core.so
    /usr/local/lib/libopencv_videoio.so
)

add_executable(provider ../src/provider.cpp)
target_link_libraries(provider vsomeip3 ${OpenCV_LIB})

add_executable(consumer ../src/consumer.cpp)
target_link_libraries(consumer vsomeip3 ${OpenCV_LIB})