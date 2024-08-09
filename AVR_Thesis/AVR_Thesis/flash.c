#include "flash.h"

uint8_t is_spm_busy()
{
    return SPMCSR & (1 << SPMEN);
}

void flash_erase_page(uint16_t address)
{
    SPMCSR = __BOOT_PAGE_ERASE;
    __asm__ __volatile__(
        "spm\n\t" ::"z"(address));
    while (is_spm_busy())
        ;
}

void flash_fill_buffer(uint16_t address, uint16_t data)
{
    SPMCSR = __BOOT_PAGE_FILL;
    __asm__ __volatile__(
        "movw r0, %0\n\t"
        "spm\n\t" ::"r"(data),
        "z"(address)
        : "r0");
}

void flash_write_data(uint16_t address, const uint8_t *data, uint16_t size)
{
    uint8_t sreg = SREG;
    uint16_t tmpAddress = address;
    cli();
    uint16_t tmpData = (size + SPM_PAGESIZE - 1) / SPM_PAGESIZE;
    uint8_t lastDate = (size - 1) % SPM_PAGESIZE;
    int loop = SPM_PAGESIZE;
    for (int i = 0; i < tmpData; i++)
    {
        tmpAddress = address + SPM_PAGESIZE * i;
        flash_erase_page(tmpAddress);
        if (i == tmpData - 1)
            loop = lastDate + 1;
        for (uint16_t j = 0; j < loop; j += 2)
        {
            uint16_t w = *data++;
            w += (*data++) << 8;
            boot_page_fill(tmpAddress + j, w);
        }

        SPMCSR = __BOOT_PAGE_WRITE;
        __asm__ __volatile__(
            "spm\n\t" ::"z"(tmpAddress));
        while (is_spm_busy())
            ;
    }
    SPMCSR = __BOOT_RWW_ENABLE;
    __asm__ __volatile__(
        "spm\n\t");
    SREG = sreg;
}
