CMAKE_MINIMUM_REQUIRED(VERSION 3.13.2)

project(MODULE_MC_HARDWARE_INTERFACES_IMPLEMENTATION_FOR_STM32)

if (NOT DEFINED HARDWARE_FLAGS)
    message(FATAL_ERROR "The value of the variable <<HARDWARE_FLAGS>> is not specified!")
endif ()

if (NOT DEFINED ${PROJECT_NAME}_COMP_FLAGS)
    message(FATAL_ERROR "The value of the variable <<${PROJECT_NAME}_COMP_FLAGS>> is not specified!")
endif ()

if (NOT DEFINED LIB_CFG_H_FILES_PATH)
    message(FATAL_ERROR "The value of the variable <<LIB_CFG_H_FILES_PATH>> is not specified!")
endif ()

if (NOT DEFINED MODULE_STM32F4_LOW_LEVEL_BY_ST_H_FILES_PATH)
    message(FATAL_ERROR "The value of the variable <<MODULE_STM32F4_LOW_LEVEL_BY_ST_H_FILES_PATH>> is not specified!")
endif ()

if (NOT DEFINED USER_OS_H_FILES_PATH)
    message(FATAL_ERROR "The value of the variable <<USER_OS_H_FILES_PATH>> is not specified!")
endif ()

if (NOT DEFINED MODULE_MC_HARDWARE_INTERFACES_H_FILES_PATH)
    message(FATAL_ERROR "The value of the variable <<MODULE_MC_HARDWARE_INTERFACES_H_FILES_PATH>> is not specified!")
endif ()

include_directories(
        ${LIB_CFG_H_FILES_PATH}
        ${MODULE_STM32F4_LOW_LEVEL_BY_ST_H_FILES_PATH}
        ${USER_OS_H_FILES_PATH}
        ${MODULE_MC_HARDWARE_INTERFACES_H_FILES_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# Компиляции полежат все <<.cpp>> файлы из директории src.
file(GLOB ${PROJECT_NAME}_SRC_LIST
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SRC_LIST})

target_compile_options(${PROJECT_NAME} PRIVATE ${HARDWARE_FLAGS} ${${PROJECT_NAME}_COMP_FLAGS})