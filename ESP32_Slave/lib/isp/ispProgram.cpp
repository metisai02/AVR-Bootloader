#include "ispProgram.h"
#include "SPI.h"
SPISettings fuses_spisettings = SPISettings(100000, MSBFIRST, SPI_MODE0);
SPISettings flash_spisettings = SPISettings(1000000, MSBFIRST, SPI_MODE0);
uint16_t spi_transactionaa (uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  uint8_t n, m, r;
  SPI.transfer(a); 
  n = SPI.transfer(b);
  //if (n != a) error = -1;
  m = SPI.transfer(c);
  r = SPI.transfer(d);
  return 0xFFFFFF & (((uint32_t)n<<16)+(m<<8) + r);
}
void IspProgram::ispPollBSY()
{
    Serial.println("Entering ispPollBSY");
    byte busybit;
    do
    {
        busybit = spi_transactionaa(0xF0, 0x0, 0x0, 0x0);
    } while (busybit & 0x01);
    Serial.println("Exiting ispPollBSY");
}

void IspProgram::flash(uint8_t hilo, uint16_t addr, uint8_t data)
{
    
    spi_transaction(0x40 + 8 * hilo, 0x00, addr & 0xFF, data);
    
}

IspProgram::IspProgram(uint32_t sck)
    : spiSckNow(sck)
{
    Serial.println("IspProgram constructor");
}

void IspProgram::ispStartPro()
{
    Serial.println("Entering ispStartPro");
    
    pinMode(PIN_RESET, OUTPUT);
    SPI.begin();
    SPI.beginTransaction(SPISettings(this->spiSckNow, MSBFIRST, SPI_MODE0));

    digitalWrite(PIN_SCK, LOW);
    delayMicroseconds(20);

    digitalWrite(PIN_RESET, HIGH);
    delayMicroseconds(100);
    digitalWrite(PIN_RESET, LOW);

    delay(50);

    // Send enable programming command
    uint8_t response = spi_transaction(0xAC, 0x53, 0x00, 0x00);
    Serial.print("Response: ");
    Serial.println(response, HEX);
    
    SPI.endTransaction();
    Serial.println("Exiting ispStartPro");
}

uint8_t IspProgram::spi_transaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    SPI.transfer(a);
    SPI.transfer(b);
    SPI.transfer(c);
    return SPI.transfer(d);
    
}

uint8_t IspProgram::ispWritePage(uint16_t addr, uint8_t *data, uint16_t length)
{
    Serial.println("Entering ispWritePage");
    
    SPI.beginTransaction(fuses_spisettings);
    uint16_t cnt = 0;
    uint16_t data_int = 0;
    // Load data into flash page buffer
    while (cnt < length)
    {
        flash(LOW, data_int, data[cnt++]);
        flash(HIGH, data_int++, data[cnt++]);
    }
    // Write flash page
    spi_transaction(0x4C, (addr >> 8) & 0xFF, addr & 0xFF, 0);
    ispPollBSY();
    SPI.endTransaction();
    
    Serial.println("Exiting ispWritePage");
}

uint8_t IspProgram::ispErasePages()
{
    Serial.println("Entering ispErasePages");
    
    SPI.beginTransaction(fuses_spisettings);
    spi_transaction(0xAC, 0x80, 0, 0);
    delay(100);
    ispPollBSY();
    SPI.endTransaction();
    
    Serial.println("Exiting ispErasePages");
}

uint8_t IspProgram::ispWriteFuseLow(uint8_t data)
{
    Serial.println("Entering ispWriteFuseLow");
    
    SPI.beginTransaction(fuses_spisettings);
    spi_transaction(0xAC, 0xA0, 0, data);
    ispPollBSY();
    SPI.endTransaction();
    
    Serial.println("Exiting ispWriteFuseLow");
}

uint8_t IspProgram::ispWriteFuseHigh(uint8_t data)
{
    Serial.println("Entering ispWriteFuseHigh");
    
    SPI.beginTransaction(fuses_spisettings);
    spi_transaction(0xAC, 0xA8, 0, data);
    ispPollBSY();
    SPI.endTransaction();
    
    Serial.println("Exiting ispWriteFuseHigh");
}

uint8_t IspProgram::ispWriteLockBit(uint8_t data)
{
    Serial.println("Entering ispWriteLockBit");
    
    SPI.beginTransaction(fuses_spisettings);
    spi_transaction(0xAC, 0xA0, 0, data);
    ispPollBSY();
    SPI.endTransaction();
    
    Serial.println("Exiting ispWriteLockBit");
}

uint8_t IspProgram::ispEnd()
{
    Serial.println("Entering ispEnd");
    
    digitalWrite(PIN_RESET, HIGH);
    
    Serial.println("Exiting ispEnd");
}
