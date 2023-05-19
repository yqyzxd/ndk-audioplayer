cmake_minimum_required(VERSION 3.22.1)

set(name "lock")

project(${name})


add_library(
        ${name}

        STATIC


        lock.cpp
        condition.cpp
        )

target_link_libraries( # Specifies the target library.
        ${name}
        )