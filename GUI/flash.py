from PyQt6.QtWidgets import QFileDialog, QMessageBox
import math, time
import threading
import os
import firebase_admin
from firebase_admin import db, credentials, storage, firestore
from dotenv import load_dotenv
from confidental import Confidental

ESP_HEADER = "AAAA"
HEADER = "FFFF"
START = "55"
READ_ALL = "67"
READ_PAGE = "68"
READ_ADDR = "69"
WRITE = "96"
ERASE_ALL = "77"
ACK = 0x22
NACK = 0x11
global_time = 0
flag_dict = {
    'cmdEarseOldFirmware': False,
    'updateReadOldFirmware': False,
    'cmdReadOldFirmware': False,
    'newFirmwareUpdate': False
}
Config = {
    "apiKey": "AIzaSyDvKWztVgwZCJwx52fc4EufKnomrq1jSrs",
    "authDomain": "thesis-esp.firebaseapp.com",
    "projectId": "thesis-esp",
    "storageBucket": "thesis-esp.appspot.com",
    "messagingSenderId": "343564142183",
    "appId": "343564142183:web:9ce92e8732d4ba539576c2",
    "measurementId": "G-GQ7LXTN34X",
    "serviceAccount": {
        "type": "service_account",
        "project_id": "thesis-esp",
        "private_key_id": "ed660c91e2ca5805eb74aac864c1fe6e50d12a79",
        "private_key": "-----BEGIN PRIVATE KEY-----\nMIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQDzRpB5GgfjdmQq\nqw9VWVTfcPRK4CYyWT7mTdwEsc7v2IszoyLfAAVnyMcY6e9P3eE4qTRjBZ43TMPE\ngSgvlK7oCGUg/5Yg4CW6opom3CKU1jTb569w+rrygOwBh0E8Kw0aNkHXUqiSXVWH\n4ltY0Iq8h2oR5QOc4/X589SC2xCOkSoGZPPX8oAPoRixlSWFfw3H7NPXtErjd1NL\njgVC/SwX+WpO/aywbzCikms6TmvHyqhWXe/C9Vb/hG6SaN+XUjszk3RkeOy6GKdC\nTSmkXdiugQr2jeWz4PyZUVNxlcHOoL2TZ+kdwoJmoTQPbknBajkDOGPHHCAv0Mr0\nWVpDTZE9AgMBAAECggEAFiI1bXnmKIEnsTofCwTgU/P9m1Je0fXiDdIc/SWwTjdw\nHXfvlEUhjhprrY3wgrO36g/QVFy5KGQzljf/ZVTXFQVZLhy9lqrOGUzx+YHNzlIu\n56hiA0V3Ui5TSqWjqcsWRhbppqLpSL0f1bJ33Sdxd/WvZjFlQsD61Y2kUsOGSMd+\n7IebArkpkhjsPRLmYsklUECeuPkP+KMdTRAvhJlFsz6hwK06AEMrR5tCr2aKhyuD\nToLU7ftivTN97Lj/GqZhICPKdz77Od82L0RwF9wpOXKte6bsW655V5S/dmmU3e4C\ny/Cz4sIEYhgLggGibZGZL/GKifYcwn+gEq0GmTpxgQKBgQD7UhGK4gHp7ccWXGrV\nkZkWNA1BgGC51grOuLwusZTGbz9o1u42WxqVIX8SvUGJTcEP4mAzHLD5WgSzGAHY\ny6GA8FfOUdObls+fYNjnudCAFmDUtddhDkJ2wAVWy3s3UoQw8otVoA9FN8Pt3fSz\nl5g9OnxfAFXpAS94BCDZrxNe2wKBgQD3ziY0xIekwvJaO451CEwz2po8D8eBLgkd\nS7b3G1Wi8kWyurzMxEdJ+4c5zivUJjNHMVLFFjmSoVa5oxe/4lpauhd48qQQ30m3\nHa2Hr2g0Z0zLZAQHDcxVC4aITj6xjAQ/COW+KWkzY67k5y3VKsrS9ZyoAOhGJ3U/\n1Y34wRAPxwKBgElKXaNK1ZQKq4xx1xYBJ8u50kPP1GVdbjlq1jYnw2M6/hrs0vs3\nBkGHSy+D0aDg8/RV6Af/zfxprAUucxtuBuM7M+APLM6q5FFuRNMO9SblX8eoX+8F\n4YwP4360SHKRtcswcEHAM7PT12d+2uWkx9jTPYHW7zbMCbPOQGY7EEMRAoGAI4r1\n/+uQaRltGXQhYrfl9ioMpUC+JNEtjX4mqBrLlauaQ6B1JAmjEAF7y9a+gpdYE4YH\nkS025MkvX0D9TxBwVUMLytVCowjj0df349ZM4/dNFH8r9LcN78TPbw2KAH1Mvp3Q\nFg5qgGxz0XE3KbQRk2CXL6Gxrx5JuvYuftdyLlECgYBMbAk+bI0zoRqG1vUxPFI/\njvYWJZ2gbbiMYkNfb9/wJ2RyMnJ///TJ2uOLIZ0dnlQOvBmmoxjMdfFuGngERK/p\nqyEVTe2aCDXMaq4iylPtL1M+r9iSAl/FLhAmmoI6rWuR2VtK225E3RGvwb4zXfl/\nsKZ7IaK03n1lZsw648S8AQ==\n-----END PRIVATE KEY-----\n",
        "client_email": "firebase-adminsdk-4cmnu@thesis-esp.iam.gserviceaccount.com",
        "client_id": "109955780547737722729",
        "auth_uri": "https://accounts.google.com/o/oauth2/auth",
        "token_uri": "https://oauth2.googleapis.com/token",
        "auth_provider_x509_cert_url": "https://www.googleapis.com/oauth2/v1/certs",
        "client_x509_cert_url": "https://www.googleapis.com/robot/v1/metadata/x509/firebase-adminsdk-4cmnu%40thesis-esp.iam.gserviceaccount.com",
        "universe_domain": "googleapis.com",
    },
}

# Initialize Firebase app
# firebase_admin.initialize_app(credentials.Certificate(Config["serviceAccount"]))

try:
    firebase_admin.initialize_app(
        credentials.Certificate(Config["serviceAccount"]),
        {
            "storageBucket": Config["storageBucket"],
            "databaseURL": "https://thesis-esp-default-rtdb.asia-southeast1.firebasedatabase.app/",
        },
    )
    print("Firebase app initialization successful!")
except ValueError as e:
    print(f"Firebase app initialization failed: {e}")

ref = db.reference("/flag/")

ref.update(flag_dict)
# Initialize Firebase Storage
bucket = storage.bucket()
print(f"Connected to bucket: {bucket.name}")


def timer():
    global global_time
    while True:
        global_time += 1
        time.sleep(0.001)

def download_firmware():
    bucket = storage.bucket()
    blob = bucket.blob('storage/OldFirmware.txt')
    local_path = 'boot/oldFirmware.txt'
    os.makedirs(os.path.dirname(local_path), exist_ok=True)
    with open(local_path, 'wb') as file_obj:
        blob.download_to_file(file_obj)
    print("File downloaded successfully")
    return True

# Function to check the flag and download the file if needed
def check_flag_and_download():
    ref = db.reference('/flag/updateReadOldFirmware')
    end_time = time.time() + 60  # Run the thread for 40 seconds

    while time.time() < end_time:
        value = ref.get()
        if value:
            print("Flag updateReadOldFirmware is set to True. Downloading firmware...")
            download_firmware()
            ref.set(False)
            return
        time.sleep(5)

    print("Thread has finished execution")

# Function to start the thread
def start_thread():
    thread = threading.Thread(target=check_flag_and_download)
    thread.start()
    thread.join()  
    
    

def crc16(data: bytes) -> int:
    poly = 0x8005
    int_val = 0x0000
    crc = int_val
    for byte in data:
        crc ^= byte << 8
        for _ in range(8):
            if crc & 0x8000:
                crc = (crc << 1) ^ poly
            else:
                crc <<= 1
            crc &= 0xFFFF
    return crc


class Flash:

    def __init__(self, ui, uart, table):
        self.ui = ui
        self.uart = uart
        self.table = table
        self.message_box = QMessageBox()
        self.ui.btnEraseOld.clicked.connect(self.erase_all_secctor)
        self.ui.btnReadOld.clicked.connect(self.read_all_data_from_flash_AVR)
        self.ui.btnUpdateFi.clicked.connect(self.browser_file)
        self.ui.btnPush.clicked.connect(self.hex_file_handler)
        self.length_total = 0
        self.hex_data = []
        self.hex_string = ""
        self.base_addr = ""
        self.first_time = True
        self.timer_thread = threading.Thread(target=timer)
        self.timer_thread.start()

        self.temp_length = 0

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
            self.tmp_length = len(self.hex_string)
            self.send_data_to_board_AVR(
                self.base_addr, self.tmp_length, self.hex_string
            )
            self.tmp_length = 0
            return
        elif record_type == "03":
            pass
        elif record_type == "02":
            pass
        elif record_type == "04":
            self.base_addr = data[9:13]

    def send_data_to_board_AVR(self, address, length, data):
        self.tmp_length = 0
        crc_data = 0
        print(f"ADDRESS: {address}")
        print(f"LENGTH: {length}")
        length_data = math.ceil(length / 256)
        print(f"length_data: {length_data}")
        if os.path.exists("boot/firmware.txt"):
            os.truncate("boot/firmware.txt", 0)
        else:
            pass

        for i in range(length_data):
            left = i * 256
            print(f"LEFT: {left}")

            if length - left > 256:
                right = left + 256
                length_frame = "80"
            else:
                right = length
                length_frame = hex(int((right - left) / 2))[2:].zfill(2)
                i = length_data

            print(f"RIGHT: {right}")

            address_int = int(address, 16)
            new_address = hex(address_int + int(left / 2))[2:].zfill(4)

            data_frame = START + length_frame + WRITE + new_address + data[left:right]
            print(f"length: {length_frame}")
            print(f"DATA: {data_frame}")

            data_bytes = bytes.fromhex(data_frame)
            crc_data = crc16(data_bytes)

            data_bytes += crc_data.to_bytes(2, byteorder="big")

            if self.ui.chb_internet.isChecked():
                data_byte = bytes.fromhex(ESP_HEADER)
                data_bytes = data_byte + data_bytes
                crc = crc16(data_bytes)
                data_bytes += crc.to_bytes(2, byteorder="big")
                hex_string = data_bytes.hex()

                # Save hex string to a file
                with open("boot/firmware.txt", "a") as file:
                    file.write(hex_string)
                    file.write("\n")
                if i == length_data:
                    self.upload_file_to_firebase("boot/firmware.txt")
                    ref.update(
                        {
                            "newFirmwareUpdate": True,
                        }
                    )

            else:
                # check ACK
                if not self.ack_handle(data_bytes, 10):
                    self.message_box.setWindowTitle("Notification")
                    self.message_box.setText("Could not write data to the BOARD")
                    self.message_box.exec()
                    return
                else:
                    # self.message_box.setWindowTitle("Notification")
                    # self.message_box.setText("It is DONE")
                    # self.message_box.exec()
                    pass
        self.length_total = 0
        self.hex_data = []
        self.hex_string = ""
        self.base_addr = ""
        self.first_time = True

    def upload_file_to_firebase(self, file_path):
        bucket = storage.bucket()
        blob = bucket.blob("storage/firmware.txt")

        # Delete the old file if it exists
        if blob.exists():
            blob.delete()
            print("Old firmware file deleted from Firebase Storage")

        # Upload the new file
        blob.upload_from_filename(file_path)
        blob.make_public()

        print(f"File URL: {blob.public_url}")

    def erase_all_secctor(self):
        length = "00"
        frame_read = START + length + ERASE_ALL + "02"
        data_bytes = bytes.fromhex(frame_read)
        crc = crc16(data_bytes)

        data_bytes = data_bytes + crc.to_bytes(2, byteorder="big")
        retransmit = 10

        if self.ui.chb_internet.isChecked():
            ref.update(
                {
                    "cmdEarseOldFirmware": True,
                }
            )

        else:
            if self.internet_handle(data_bytes, retransmit) == True:
                self.message_box.setWindowTitle("Notification")
                self.message_box.setText("erase all succeefully")
                self.message_box.exec()
            else:
                self.message_box.setWindowTitle("Notification")
                self.message_box.setText("erase all not succeefully")
                self.message_box.exec()

    def read_all_data_from_flash_AVR(self):
        if self.ui.chb_internet.isChecked():
            ref.update(
                {
                    "cmdReadOldFirmware": True,
                }
            )
            start_thread()

        else:
            length = "00"
            frame_read = START + length + READ_ALL + "02"
            data_bytes = bytes.fromhex(frame_read)
            restransmit = 10
            crc = crc16(data_bytes)

            data_bytes = data_bytes + crc.to_bytes(2, byteorder="big")
            ack_frame = False
            frame_buffer = []
            frame_size = 130
            remain = 0
            if self.ack_handle(data_bytes, restransmit) == True:
                # waititng reading data from the board
                array_page = [0] * 5
                time_ack = global_time
                while (global_time - time_ack) < 1000:
                    if remain == 1:
                        new_data += self.uart.serialPort.read_all()
                    new_data = self.uart.serialPort.read_all()
                    print(f"DATA: {new_data}")
                    if len(new_data) > 0:
                        for byte in new_data:
                            if not is_in_frame:
                                if byte == 0x55:
                                    is_in_frame = True
                                    frame_buffer.clear()
                                    frame_buffer.append(byte)
                            else:
                                frame_buffer.append(byte)
                                if len(frame_buffer) == frame_size:
                                    is_in_frame = False
                                    page_index = self.send_read_data_on_GUI(frame_buffer)
                                    if page_index != -1 and page_index < 6:
                                        array_page[page_index] = 1
                                    frame_buffer.clear()

            else:
                self.message_box.setWindowTitle("Notification")
                self.message_box.setText("Could not read data form the BOARD")
                self.message_box.exec()

    def send_read_data_on_GUI(self, frame):
        self.table.update_frame(frame)
        print(frame)
        return frame[1]

    def process_ack(self, frame):
        return frame[1] == ACK

    def ack_handle(self, data_bytes, retransmits):
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
                                    return True
                                frame_buffer.clear()
                                break

            # Nếu không nhận được ACK sau 10 lần thử, thoát vòng lặp
            if cout_retransmit == retransmits:
                return False

    def internet_handle(self, data_bytes, retransmit):
        if self.ui.chb_internet.isChecked():
            data_byte = bytes.fromhex(ESP_HEADER)
            data_bytes = data_byte + data_bytes
            crc = crc16(data_bytes)
            data_bytes = data_bytes + crc.to_bytes(2, byteorder="big")

        else:
            return self.ack_handle(data_bytes, retransmit)
