from PyQt6.QtWidgets import QMainWindow, QApplication
from PyQt6 import QtCore, QtGui, QtWidgets
from PyQt6.QtCore import Qt
from PyQt6.QtGui import QMouseEvent
from Matplot_Files.pyqt_matplot_canva import MplCanvas
from matplotlib.backends.backend_qtagg import (NavigationToolbar2QT as NavigationToolbar)


from icecream import ic
class Matplot_Window(QtWidgets.QDialog):
    def __init__(self,parent,x_data=None,y_data=None):
        super().__init__(parent)
        self.setObjectName("Matplot_Window")
        self.resize(800, 600)
        self.verticalLayout = QtWidgets.QVBoxLayout(self)
        self.verticalLayout.setObjectName("verticalLayout")
        self.mpl_widget = MplCanvas(x_data,y_data,None,width=5, height=4, dpi=100)
        
        self.toolbar = NavigationToolbar(self.mpl_widget, self)
        self.verticalLayout.addWidget(self.toolbar)
        self.verticalLayout.addWidget(self.mpl_widget)
        self.setLayout(self.verticalLayout)
          
        self.show()
    #-def on_mouse_move(self, event):
    #-    if event.inaxes:
    #-        self.mpl_widget.axes.lines[-1].remove() if len(self.mpl_widget.axes.lines) > 1 else None
    #-        self.mpl_widget.axes.axvline(x=event.xdata, color='gray', linestyle='--')
    #-        self.mpl_widget.axes.axhline(y=event.ydata, color='gray', linestyle='--')
    #-        self.mpl_widget.draw()
    