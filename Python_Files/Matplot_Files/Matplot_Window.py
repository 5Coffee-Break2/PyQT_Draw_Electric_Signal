from PyQt6.QtWidgets import QMainWindow, QApplication
from PyQt6 import QtCore, QtGui, QtWidgets
from PyQt6.QtCore import Qt
from Matplot_Files.pyqt_matplot_canva import MplCanvas

class Matplot_Window(QtWidgets.QDialog):
    def __init__(self,parent):
        super().__init__(parent)
        self.setObjectName("Matplot_Window")
        self.resize(800, 600)
        self.verticalLayout = QtWidgets.QVBoxLayout(self)
        self.verticalLayout.setObjectName("verticalLayout")
        self.mpl_widget = MplCanvas(self, width=5, height=4, dpi=100)
        self.verticalLayout.addWidget(self.mpl_widget)
        self.setLayout(self.verticalLayout)
        self.show()
       # self.retranslateUi(self)
       # QtCore.QMetaObject.connectSlotsByName(self)
         

    #def retranslateUi(self, Matplot_Window):
    #    _translate = QtCore.QCoreApplication.translate
    #    Matplot_Window.setWindowTitle(_translate("Matplot_Window", "Matplot_Window"))