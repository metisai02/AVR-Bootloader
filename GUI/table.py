from PyQt6.QtWidgets import QFileDialog, QMessageBox, QTableWidgetItem
import sys

COLUMN_NAME = ["00-01", "02-03", "04-05", "06-07", "08-09", "0A-0B", "0C-0D", "0E-0F"]


class Table:
    def __init__(self, ui):
        self.ui = ui
        self.init_table()

    def init_table(self):
        self.ui.tabFlash.setRowCount(int(65536 / 16))
        self.ui.tabFlash.setColumnCount(8)
        self.ui.tabFlash.setHorizontalHeaderLabels(COLUMN_NAME)
        for i in range(0, 65536, 16):  # Bước nhảy là 16 để cách nhau 16 bit
            hex_value = format(i, "04X")  # Định dạng giá trị hexa từ 0000 đến FFFF
            row_index = i // 16  # Tính chỉ mục hàng dựa trên giá trị hexa
            item = QTableWidgetItem(hex_value)
            self.ui.tabFlash.setVerticalHeaderItem(row_index, item)  # Đặt tên hàng
        init = False
        for i in range(int(65536 / 16)):
            for j in range(8):
                self.ui.tabFlash.setItem(i, j, QTableWidgetItem("FFFF"))
                if init != True:
                    self.ui.tabFlash.setColumnWidth(j, 55)
            init = True

    def update_frame(self, frame):
        page = frame[1]  # Lấy page từ frame
        print(f"page: {page}")
        # Lặp qua mỗi hàng và cột trong bảng
        for i in range(8):
            for j in range(8):
                data_start_index = 2 + i * 8 + j * 2
                data_bytes = frame[data_start_index : data_start_index + 2]
                hex_string = "".join([format(byte, "02X") for byte in data_bytes])
                print(f"HEX STRING :{hex_string}")
                self.ui.tabFlash.setItem(
                    i + page * 8, j, QTableWidgetItem(hex_string)
                )


if __name__ == "__main__":
    pass
