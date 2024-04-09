# THE AVR MICROCONTROLLER

## Overview

<img src=".\Image\blockdiagram.png" alt="OOP" style="zoom:;">
AVR là họ RISC (Reduce instruction set) 8bit kiến trúc Havard
Havard là kiến trúc phân biệt độc lập giữa `instruction memory` và `data memory`

1. Tóm tắt cơ bản

-   131 lệnh (instuctions)
-   Đa số các lệnh thực thi chỉ với 1 chu kỳ xung nhịp
-   Bộ bộ nhân 2 trên chip (MUX2 on chip)

2. Bộ nhớ (memory)

-   FLASH : 32kb
-   EERPROM :1kb
-   SRAM: 2kb

3. Các ngoại vi

-   32 I/O lập trình được
-   2 timer 8bit, 1 timer 16bit
-   6 kênh PWM
-   8 kênh ADC 10bit (Có ngõ vao vi sai lập trình được độ lợi)
-   2 UART, 1 SPI, 1 I2C
-   Watchdog
-   Ngắt và wakup trên các chân

4. Các tính năng đặc biệt

-   Bộ phát hiện sụt áp lập trình được `Brown-out`
-   Bộ giao động RC điều chỉnh đc bên trong chip
-   C ó nhiều nguồn ngắt ngoài và trong
-   6 chế độ ngủ tiết kiệm năng lượng

5. Các đặt tính khác

-   Tần số làm việc 0 – 20MHz.
-   Điện áp hoạt động 2.7V – 5.5V(1.8V – 5.5V với họ Atmega324PA)

## AVR CPU Core

<img src=".\Image\AVRArchitecture.png" alt="OOP" style="zoom:;">

-   Để tối đa được hiệu xuất và làm việc xong xong, thì AVR sử dụng **Harvard architecture**. Điều này sẽ có 2 được bus độc lập giữa **instruction memory** và **data memory**
-   Instructions được thực hiện **single level pipelining**

<img src=".\Image\pipelining.png" alt="OOP" style="zoom:;">

### ALU – Arithmetic Logic Unit

<img src=".\Image\AVRArchitecture.png" alt="OOP" style="zoom:;">
ALU Kết nói trực tiếp với 32 general purpose registers (GPRs).
ALU có thể thực hiện giữa 'immediate" với register hoặc register với register

### Status register

-   Địa chỉ I/O=0x3F,đọc/ghi truy xuất bit được
-   Chứa các cờ báo trạng thái kết quả thực hiện lệnh về các phép toán,chuyển điều khiển...
    <img src=".\Image\Rstatus.png" alt="OOP" style="zoom:;">

    -   Bit 7- I(Global Interrupt Enable): Cho phép ngắt toàn cục - Đặt I=1 cho phép toàn bộ các nguồn ngắt làm việc(xem chương 10) - Xóa I=0 cấm toàn bộ các nguồn ngắt
    -   Bit 6- T(Copy Storage): copy lưu bit

        -   Sử dụng lệnh BST và BLD với GPRs để **lưu** hoặc **lấy** ra 1 bit trong General purpose register

        ```asm
        Giả sử R18=11010101= 0xD5,R20=00111111=0x3F
        BST R18,6 ;lưu bit thứ 6 của r18 -> T bit
        BLD R20,7 ;
        ```

## Memory
