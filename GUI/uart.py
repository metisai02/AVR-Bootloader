from PyQt6.QtCore import QTimer
import serial, serial.tools.list_ports


class Uart_serial:
    def __init__(self, ui):
        self.ui = ui
        self.baudList = {
            "4800": 4800,
            "9600": 9600,
            "19200": 19200,
            "38400": 38400,
            "115200": 115200,
        }
        self.portList = []
        self.serialPort = serial.Serial()
        self.ui.btnConnect.setText("Connect")

        self.timer = QTimer()
        self.timer.timeout.connect(self.update_serial)
        self.timer.start(200)
        self.ui.cbBuad.addItems(self.baudList)
        self.ui.btnConnect.clicked.connect(self.connect_serial)

    def connect_serial(self):
        if self.ui.btnConnect.isChecked():
            port = self.ui.cbCOM.currentText()
            baud = self.ui.cbBuad.currentText()
            self.serialPort.port = port
            self.serialPort.baudrate = baud
            try:
                self.serialPort.open()
                if self.serialPort.is_open:
                    self.ui.btnConnect.setText("Disconnect")
                    print("Kết nối thành công")
            except:
                print("Chưa kết nối được ")
        else:
            self.ui.btnConnect.setText("Connect")
            self.serialPort.close()
            print("ngắt kết nối thành công")

    def update_serial(self):
        ports = serial.tools.list_ports.comports()
        self.portList.clear()
        for port in ports:
            self.portList.append(port.device)
        self.ui.cbCOM.clear()
        self.ui.cbCOM.addItems(self.portList)

    def send_data_serial(self, data):
        if self.serialPort.is_open():
            self.serialPort.write(data)
        else:
            print("Please connect COM")

    def receive_data_serial(self):
        if self.serialPort.is_open():
            data_bytes = self.serialPort.read_all()
            data_string = data_bytes.decode()
            return data_string
        else:
            print("Please connect COM")
            return ""
