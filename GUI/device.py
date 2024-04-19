from PyQt6.QtCore import QTimer
class Device():
    def __init__(self, ui):
        self.ui = ui
        self.device_info = {
            'STM32_F411-DIS': {
                'Flash size': '512k',
                'Page': '7',
                'SRAM': '128k'
            },
            'STM32_F103': {
                'Flash size': '128k',
                'Page': '128',
                'SRAM': '16k'
            },
            'ATMEGA328p': {
                'Flash size': 'none',
                'Page': 'none',
                'SRAM': 'none'
            }
        }
        self.ui.cbDevice.addItems(self.device_info.keys())
        self.ui.cbDevice.currentIndexChanged.connect(self.set_info)
        self.ui.btnAuto.clicked.connect(self.auto_handler)
        self.set_info()

    def set_info(self):
        device = self.ui.cbDevice.currentText()
        details = self.device_info[device]
        text = ""
        for key, value in details.items():
            text += f"{key}: {value}\n"
        self.ui.ptedDevice.setPlainText(text)
    def auto_handler(self):
        pass
        #todo1: handle connecting device
        #todo2: limited lenght of flash address
        