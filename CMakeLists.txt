# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(periodic_thread_DigIO)

target_sources(app PRIVATE src/main.c)

zephyr_include_directories(button) #Add this line
target_include_directories(app PRIVATE src/button) #Add this line

target_sources(app PRIVATE src/button/button.c) # Add module c source

zephyr_include_directories(threads) #Add this line
target_include_directories(app PRIVATE src/threads) #Add this line

target_sources(app PRIVATE src/threads/threads.c) # Add module c source

zephyr_include_directories(uart) #Add this line
target_include_directories(app PRIVATE src/uart) #Add this line

target_sources(app PRIVATE src/uart/uart.c) # Add module c source