from PyQt6.QtWidgets import QFileDialog, QMessageBox
import math, time
import threading
import os

HEADER = "FFFF"
START = "55"
READ_ALL = "67"
READ_PAGE = "68"
READ_ADDR = "69"
WRITE = "96"
ERASE_ALL = '77'
ACK = 0x22
NACK = 0x11
global_time = 0


def timer():
    global global_time
    while True:
        global_time += 1
        time.sleep(0.001)


class Flash:
    def __init__(self, ui, uart, table):
        self.ui = ui
        self.uart = uart
        self.table = table
        self.message_box = QMessageBox()
        self.ui.btnBrowser.clicked.connect(self.browser_file)
        self.ui.chbHexfile.clicked.connect(self.value_handler)
        self.ui.btnProgram.clicked.connect(self.program_handler)
        self.ui.btnErase.clicked.connect(self.erase_secctor)
        self.ui.btnEraseAll.clicked.connect(self.erase_all_secctor)
        self.ui.btnRead.clicked.connect(self.read_all_data_from_flash_AVR)
        self.length_total = 0
        self.hex_data = []
        self.hex_string = ""
        self.base_addr = ""
        self.first_time = True
        self.timer_thread = threading.Thread(target=timer)
        self.timer_thread.start()

    def browser_file(self):
        file_dialog = QFileDialog()
        file_dialog.setFileMode(QFileDialog.FileMode.ExistingFiles)
        file_dialog.setNameFilter("Data file (*.hex)")
        if file_dialog.exec():
            selected_files = file_dialog.selectedFiles()
            if selected_files:
                file_path = selected_files[0]
                self.ui.tedHexfile.setText(file_path)

    def hex_file_handler(self):
        file_path = self.ui.tedHexfile.toPlainText()
        try:
            with open(file_path, "r") as file:
                lines = file.readlines()
                for line in lines:
                    self.hex_data.append(line)
                    self.hex_string_handler(line)
                    line = line[:-1]
                    self.hex_string_hander_AVR(line)
        except Exception as e:
            self.message_box.setWindowTitle("Error")
            self.message_box.setText(f"Error while processing hex file: {e}")
            self.message_box.exec()

    def hex_string_hander_AVR(self, data):
        if data[0] != ":":
            return
        length = int(data[1:3], 16)
        print(length)
        offset_addr = data[3:7]
        record_type = data[7:9]
        if record_type == "00":  # data
            if self.first_time:
                self.base_addr = offset_addr
                self.first_time = False

            substring_data = data[9 : (9 + length * 2)]
            self.hex_string += substring_data
        elif record_type == "01":  # End of File Record
            tmp_length = len(self.hex_string)
            self.send_data_to_board_AVR(self.base_addr, tmp_length, self.hex_string)
            return
        elif record_type == "03":
            pass
        elif record_type == "02":
            pass
        elif record_type == "04":
            self.base_addr = data[9:13]

    def hex_string_handler(self, data):
        if data[0] != ":":
            return
        length = data[1:3]
        offset_addr = data[3:7]
        record_type = data[7:9]
        if record_type == "00":  # data
            address = self.base_addr + offset_addr
            substring_data = data[9:]
            self.send_data_to_board(address, length, substring_data)
            # todo send data
        elif record_type == "01":
            print("Done to write flash")
            return
        elif record_type == "02":
            pass
        elif record_type == "04":
            self.base_addr = data[9:13]

    def specific_value_handler(self):
        data = self.ui.tedValue.toPlainText()
        start_addr = self.ui.tedSAddr.toPlainText()
        length = self.ui.tedLength.toPlainText()
        if not all([data, start_addr, length]):
            self.message_box.setWindowTitle("Error")
            self.message_box.setText("Please enter all values")
            self.message_box.exec()
            return
        length = int(length)
        hex_length = hex(length)[2:].zfill(2)
        self.send_data_to_board(start_addr, hex_length, data)

    def send_data_to_board_AVR(self, address, length, data):
        print(f"ADDRESS: {address}")
        print(f"LENGTH: {length}")
        length_data = math.ceil(length / 256)
        print(f"length_data: {length_data}")

        for i in range(length_data):
            left = i * 256
            print(f"LEFT: {left}")

            if length - left > 256:
                right = left + 256
                length_frame = "80"
            else:
                right = length
                length_frame = hex(right + 1 - left)[2:].zfill(2)

            print(f"RIGHT: {right}")

            address_int = int(address, 16)
            new_address = hex(address_int + int(left / 2))[2:].zfill(4)

            data_frame = START + length_frame + WRITE + new_address + data[left:right]
            data_bytes = bytes.fromhex(data_frame)
            #check ACK
            ack_frame = False
            cout_retransmit = 0
            frame_buffer = []
            is_in_frame = False
            ack_frame_size = 2
            while not ack_frame:
                self.uart.serialPort.write(data_bytes)
                time_ack = global_time
                cout_retransmit += 1
                while (global_time - time_ack) < 10:
                    new_data = self.uart.serialPort.read_all()
                    if len(new_data) > 0:
                        for byte in new_data:
                            if not is_in_frame:
                                if byte == 0x55:
                                    is_in_frame = True
                                    frame_buffer.clear()
                                    frame_buffer.append(byte)
                            else:
                                frame_buffer.append(byte)
                                if len(frame_buffer) == ack_frame_size:
                                    is_in_frame = False
                                    if self.process_ack(frame_buffer) == 1:
                                        ack_frame = True
                                    frame_buffer.clear()
                                    break

                # Nếu không nhận được ACK sau 10 lần thử, thoát vòng lặp
                if cout_retransmit == 10:
                    cout_retransmit = 0
                    return

        self.length_total = 0
        self.hex_data = []
        self.hex_string = ""
        self.base_addr = ""
        self.first_time = True

    def send_data_to_board(self, address, length, data):
        data_frame = HEADER + length + address + data
        data_an_integer = int(data_frame, 16)
        data_bytes = bytes.fromhex(data_frame)
        # print(data_bytes)
        # self.uart.serialPort.write(data_bytes)

    def value_handler(self):
        if self.ui.chbHexfile.isChecked():
            self.ui.grFlash.setEnabled(False)
            self.ui.btnBrowser.setEnabled(True)
        else:
            self.ui.grFlash.setEnabled(True)
            self.ui.btnBrowser.setEnabled(False)

    def program_handler(self):
        self.length_total = 0
        self.hex_data = []
        self.hex_string = ""
        if self.ui.chbHexfile.isChecked():
            self.hex_file_handler()
        else:
            self.specific_value_handler()

    def erase_all_secctor(self):
        length = '00'
        frame_read = START + length + ERASE_ALL + "02"
        data_bytes = bytes.fromhex(frame_read)
        self.uart.serialPort.write(data_bytes)
        ack_frame = False
        cout_retransmit = 0
        frame_buffer = []
        is_in_frame = False
        ack_frame_size = 2
        while not ack_frame:
            self.uart.serialPort.write(data_bytes)
            time_ack = global_time
            cout_retransmit += 1
            while (global_time - time_ack) < 10:
                new_data = self.uart.serialPort.read_all()
                if len(new_data) > 0:
                    for byte in new_data:
                        if not is_in_frame:
                            if byte == 0x55:
                                is_in_frame = True
                                frame_buffer.clear()
                                frame_buffer.append(byte)
                        else:
                            frame_buffer.append(byte)
                            if len(frame_buffer) == ack_frame_size:
                                is_in_frame = False
                                if self.process_ack(frame_buffer) == 1:
                                    ack_frame = True
                                frame_buffer.clear()
                                break

                # Nếu không nhận được ACK sau 10 lần thử, thoát vòng lặp
            if cout_retransmit == 10:
                cout_retransmit = 0
                return
        self.message_box.setWindowTitle("Notification")
        self.message_box.setText("erase all succeefully")
        self.message_box.exec()
        
        
    def erase_secctor(self):
        try:
            sector = int(self.ui.tedSector.toPlainText())
            if sector > 128:
                self.message_box.setWindowTitle("Error")
                self.message_box.setText("too large")
                self.message_box.exec()
        except Exception as e:
            self.message_box.setWindowTitle("Error")
            self.message_box.setText(f"Error while processing hex file: {e}")
            self.message_box.exec()

        HEADER = "FFFF"
        data_frame = HEADER + str(sector)
        data_an_integer = int(data_frame, 16)
        data_bytes = bytes.fromhex(data_frame)
        print(data_bytes)

    def read_all_data_from_flash_AVR(self):
        length = "00"
        frame_read = START + length + READ_ALL + "02"
        data_bytes = bytes.fromhex(frame_read)
        ack_frame = False
        frame_buffer = []
        cout_retransmit = 0
        is_in_frame = False
        ack_frame_size = 2
        frame_size = 130
        remain = 0
        while not ack_frame:
            self.uart.serialPort.write(data_bytes)
            time_ack = global_time
            cout_retransmit += 1
            while (global_time - time_ack) < 4:
                new_data = self.uart.serialPort.read_all()
                if len(new_data) > 5:
                    remain = 1
                if len(new_data) > 0:
                    for byte in new_data:
                        if not is_in_frame:
                            if byte == 0x55:
                                is_in_frame = True
                                frame_buffer.clear()
                                frame_buffer.append(byte)
                        else:
                            frame_buffer.append(byte)
                            if len(frame_buffer) == ack_frame_size:
                                is_in_frame = False
                                if self.process_ack(frame_buffer) == 1:
                                    ack_frame = True
                                    break
                                frame_buffer.clear()
            if cout_retransmit == 40:
                cout_retransmit = 0
                return
            # waititng reading data from the board
            array_page = [0] * 5
            time_ack = global_time
            while (global_time - time_ack) < 400:
                if remain == 1:
                    new_data += self.uart.serialPort.read_all()
                new_data = self.uart.serialPort.read_all()
                print(F"DATA: {new_data}")
                if len(new_data) > 0:
                    for byte in new_data:
                        if not is_in_frame:
                            if byte == 0x55:
                                is_in_frame = True
                                frame_buffer.clear()
                                frame_buffer.append(byte)
                        else:
                            frame_buffer.append(byte)
                            print(frame_buffer)
                            if len(frame_buffer) == frame_size:
                                is_in_frame = False
                                page_index = self.send_read_data_on_GUI(frame_buffer)
                                if page_index != -1 and page_index < 6:
                                    array_page[page_index] = 1
                                frame_buffer.clear()
            
            # for page_index, received in enumerate(array_page):
            #     if not received:
            #         print(page_index)
            #         address = page_index * 128
            #         address_string = hex(address)[2:].zfill(4)
            #         frame_read_page = START + length + READ_ALL + address_string
            #         data_bytes_page = bytes.fromhex(frame_read_page)
            #         self.uart.serialPort.write(data_bytes_page)
                    

    def send_read_data_on_GUI(self, frame):
        self.table.update_frame(frame)
        print(frame)
        return frame[1]

    def process_ack(self, frame):
        return frame[1] == ACK
