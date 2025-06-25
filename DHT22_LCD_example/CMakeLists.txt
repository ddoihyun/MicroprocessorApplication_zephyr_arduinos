cmake_minimum_required(VERSION 3.20.0)
# Zephyr 앱 보일러플레이트
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(dht22_lcd)

# 소스 파일 등록
target_sources(app PRIVATE
    src/main.c
    src/dht_sensor.c
    src/LCD.c
)

# 헤더 검색 경로
target_include_directories(app PRIVATE include)