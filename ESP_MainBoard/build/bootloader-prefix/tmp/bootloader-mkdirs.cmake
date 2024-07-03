# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Espressif/frameworks/esp-idf-v5.2.2/components/bootloader/subproject"
  "D:/Docs/Thesis/AVR-Bootloader/ESP_MainBoard/build/bootloader"
  "D:/Docs/Thesis/AVR-Bootloader/ESP_MainBoard/build/bootloader-prefix"
  "D:/Docs/Thesis/AVR-Bootloader/ESP_MainBoard/build/bootloader-prefix/tmp"
  "D:/Docs/Thesis/AVR-Bootloader/ESP_MainBoard/build/bootloader-prefix/src/bootloader-stamp"
  "D:/Docs/Thesis/AVR-Bootloader/ESP_MainBoard/build/bootloader-prefix/src"
  "D:/Docs/Thesis/AVR-Bootloader/ESP_MainBoard/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Docs/Thesis/AVR-Bootloader/ESP_MainBoard/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Docs/Thesis/AVR-Bootloader/ESP_MainBoard/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
