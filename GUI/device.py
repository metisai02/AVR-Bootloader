from PyQt6.QtCore import QTimer
class Device():
    def __init__(self, ui):
        self.ui = ui
        self.device_info = {
            'ATMEGA328p': {
                'Flash size': '32 KB',
                'Page': '64 bytes',
                'SRAM': '2 KB',
                'EEPROM': '1 KB',
                'Clock Speed': '20 MHz',
                'Operating Voltage': '1.8V - 5.5V',
                'Number of GPIO Pins': '23',
                'PWM Channels': '6',
                'ADC Channels': '6',
                'USART': '1',
                'SPI': '1',
                'I2C': '1'
            }
        }
        self.set_info()
        self.ui.specification.setReadOnly(True)

    def set_info(self):
        info = self.device_info['ATMEGA328p']
        info_text = "<br>".join(f"<b>{key}:</b> {value}" for key, value in info.items())
        self.ui.specification.setHtml(info_text)