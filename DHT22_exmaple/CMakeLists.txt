cmake_minimum_required(VERSION 3.20.0)

# Include 디렉토리 설정
set(CMAKE_C_STANDARD 99)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(DHT_sensor)

# Include 디렉토리 추가
target_include_directories(app PRIVATE include)

# 소스 파일들 추가
target_sources(app PRIVATE 
    src/main.c
    src/dht_sensor.c
)