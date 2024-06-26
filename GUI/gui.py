# Form implementation generated from reading ui file '.\guiTest.ui'
#
# Created by: PyQt6 UI code generator 6.7.0
#
# WARNING: Any manual changes made to this file will be lost when pyuic6 is
# run again.  Do not edit this file unless you know what you are doing.


from PyQt6 import QtCore, QtGui, QtWidgets


class Ui_mainUI(object):
    def setupUi(self, mainUI):
        mainUI.setObjectName("mainUI")
        mainUI.resize(1029, 704)
        self.centralwidget = QtWidgets.QWidget(parent=mainUI)
        self.centralwidget.setObjectName("centralwidget")
        self.cbBuad = QtWidgets.QComboBox(parent=self.centralwidget)
        self.cbBuad.setGeometry(QtCore.QRect(170, 30, 161, 31))
        self.cbBuad.setObjectName("cbBuad")
        self.cbCOM = QtWidgets.QComboBox(parent=self.centralwidget)
        self.cbCOM.setGeometry(QtCore.QRect(10, 30, 151, 31))
        self.cbCOM.setObjectName("cbCOM")
        self.label = QtWidgets.QLabel(parent=self.centralwidget)
        self.label.setGeometry(QtCore.QRect(20, 10, 61, 16))
        self.label.setObjectName("label")
        self.label_2 = QtWidgets.QLabel(parent=self.centralwidget)
        self.label_2.setGeometry(QtCore.QRect(180, 10, 81, 16))
        self.label_2.setObjectName("label_2")
        self.tedHexfile = QtWidgets.QTextEdit(parent=self.centralwidget)
        self.tedHexfile.setGeometry(QtCore.QRect(170, 80, 161, 31))
        self.tedHexfile.setObjectName("tedHexfile")
        self.groupBox = QtWidgets.QGroupBox(parent=self.centralwidget)
        self.groupBox.setGeometry(QtCore.QRect(10, 220, 641, 421))
        self.groupBox.setObjectName("groupBox")
        self.tabFlash = QtWidgets.QTableWidget(parent=self.groupBox)
        self.tabFlash.setGeometry(QtCore.QRect(0, 20, 631, 381))
        self.tabFlash.setObjectName("tabFlash")
        self.tabFlash.setColumnCount(0)
        self.tabFlash.setRowCount(0)
        self.btnConnect = QtWidgets.QCheckBox(parent=self.centralwidget)
        self.btnConnect.setGeometry(QtCore.QRect(350, 30, 101, 31))
        self.btnConnect.setObjectName("btnConnect")
        self.btnUpdateFi = QtWidgets.QPushButton(parent=self.centralwidget)
        self.btnUpdateFi.setGeometry(QtCore.QRect(9, 80, 151, 31))
        self.btnUpdateFi.setObjectName("btnUpdateFi")
        self.btnReadOld = QtWidgets.QPushButton(parent=self.centralwidget)
        self.btnReadOld.setGeometry(QtCore.QRect(10, 127, 151, 31))
        self.btnReadOld.setObjectName("btnReadOld")
        self.btnEraseOld = QtWidgets.QPushButton(parent=self.centralwidget)
        self.btnEraseOld.setGeometry(QtCore.QRect(10, 173, 151, 31))
        self.btnEraseOld.setObjectName("btnEraseOld")
        self.groupBox_2 = QtWidgets.QGroupBox(parent=self.centralwidget)
        self.groupBox_2.setGeometry(QtCore.QRect(660, 20, 361, 611))
        self.groupBox_2.setObjectName("groupBox_2")
        self.disassembler = QtWidgets.QPlainTextEdit(parent=self.groupBox_2)
        self.disassembler.setGeometry(QtCore.QRect(0, 20, 361, 591))
        self.disassembler.setObjectName("disassembler")
        self.btnPush = QtWidgets.QPushButton(parent=self.centralwidget)
        self.btnPush.setGeometry(QtCore.QRect(350, 80, 61, 31))
        self.btnPush.setObjectName("btnPush")
        self.chBoxDiagnose = QtWidgets.QCheckBox(parent=self.centralwidget)
        self.chBoxDiagnose.setGeometry(QtCore.QRect(180, 130, 101, 31))
        self.chBoxDiagnose.setObjectName("chBoxDiagnose")
        self.btnReProgram = QtWidgets.QPushButton(parent=self.centralwidget)
        self.btnReProgram.setGeometry(QtCore.QRect(280, 130, 131, 31))
        self.btnReProgram.setObjectName("btnReProgram")
        self.groupBox_3 = QtWidgets.QGroupBox(parent=self.centralwidget)
        self.groupBox_3.setGeometry(QtCore.QRect(480, 20, 171, 201))
        self.groupBox_3.setObjectName("groupBox_3")
        self.specification = QtWidgets.QPlainTextEdit(parent=self.groupBox_3)
        self.specification.setGeometry(QtCore.QRect(0, 30, 171, 171))
        self.specification.setObjectName("specification")
        self.chb_internet = QtWidgets.QCheckBox(parent=self.centralwidget)
        self.chb_internet.setGeometry(QtCore.QRect(180, 170, 101, 31))
        self.chb_internet.setObjectName("chb_internet")
        mainUI.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(parent=mainUI)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 1029, 18))
        self.menubar.setObjectName("menubar")
        mainUI.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(parent=mainUI)
        self.statusbar.setObjectName("statusbar")
        mainUI.setStatusBar(self.statusbar)

        self.retranslateUi(mainUI)
        QtCore.QMetaObject.connectSlotsByName(mainUI)

    def retranslateUi(self, mainUI):
        _translate = QtCore.QCoreApplication.translate
        mainUI.setWindowTitle(_translate("mainUI", "MainWindow"))
        self.label.setText(_translate("mainUI", "COM"))
        self.label_2.setText(_translate("mainUI", "BUADRATE"))
        self.groupBox.setTitle(_translate("mainUI", "MONITOR"))
        self.btnConnect.setText(_translate("mainUI", "CONNECT"))
        self.btnUpdateFi.setText(_translate("mainUI", "Update New Firmware"))
        self.btnReadOld.setText(_translate("mainUI", "Read Old Firmware"))
        self.btnEraseOld.setText(_translate("mainUI", "Erase Old Firmware"))
        self.groupBox_2.setTitle(_translate("mainUI", "DISASSEMBLER"))
        self.btnPush.setText(_translate("mainUI", "PUSH"))
        self.chBoxDiagnose.setText(_translate("mainUI", "Diagnose"))
        self.btnReProgram.setText(_translate("mainUI", "Reprogram"))
        self.groupBox_3.setTitle(_translate("mainUI", "SPECIFICATION"))
        self.chb_internet.setText(_translate("mainUI", "INTERNET"))
