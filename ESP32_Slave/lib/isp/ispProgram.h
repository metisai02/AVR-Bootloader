#ifndef ISP_PROGRAMMING_H
#define ISP_PROGRAMMING_H
#include "Arduino.h"

#define SPI_BUARD 10000
#ifdef ISP_ESP
#define SPI_BUARD 10000
#define PIN_MOSI 51
#define PIN_MISO 50
#define PIN_SCK 52
#define PIN_RESET 53
#else
#define SPI_BUARD 10000
#define PIN_MOSI 23
#define PIN_MISO 19
#define PIN_SCK 18
#define PIN_RESET 32
#endif

typedef enum
{
    FUSE_HIGH_BOOTRST = 0,
    FUSE_HIGH_BOOTSZ0,
    FUSE_HIGH_BOOTSZ1,
    FUSE_HIGH_EESAVE,
    FUSE_HIGH_WDTON,
    FUSE_HIGH_SPIEN,
    FUSE_HIGH_DWEN,
    FUSE_HIGH_RSTDISBL
} FUSE_HIGH;
typedef enum
{
    FUSE_LOW_CKSEL0 = 0,
    FUSE_LOW_CKSEL1,
    FUSE_LOW_CKSEL2,
    FUSE_LOW_CKSEL3,
    FUSE_LOW_SUT0,
    FUSE_LOW_SUT1,
    FUSE_LOW_CKOUT,
    FUSE_LOW_CKDIV8
} FUSE_LOW_T;

class IspProgram
{
private:
    void flash(uint8_t hilo, uint16_t addr, uint8_t data);
    uint8_t spi_transaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
    void ispPollBSY();
    uint32_t spiSckNow;
    uint32_t spiSckOld;
    

public:
    void ispStartPro();
    IspProgram(uint32_t sck);
    uint8_t ispWritePage(uint16_t addr, uint8_t *data, uint16_t length);
    uint8_t ispErasePages();
    uint8_t ispWriteFuseLow(uint8_t data);
    uint8_t ispWriteFuseHigh(uint8_t data);
    uint8_t ispWriteLockBit(uint8_t data);
    uint8_t ispEnd();
};

#endif // ISP_PROGRAMMING_H