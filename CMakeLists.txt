cmake_minimum_required(VERSION 3.27)
project(DeepThought)

set(CMAKE_CXX_FLAGS "-Ofast")
set(CMAKE_CXX_STANDARD 20)

add_executable(DeepThought main.cpp
        ConjunctiveNormalForm.h
        Resolver.h
        ConjunctiveNormalFormCreator.h
        ResolverTester.h
)
