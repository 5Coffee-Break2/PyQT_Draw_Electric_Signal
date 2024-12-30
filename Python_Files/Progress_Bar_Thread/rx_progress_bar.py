# from PySide2 import QtCore, QtWidgets
#
# class progreeBar_Woker(QtCore.QObject):
#     progressChanged = QtCore.Signal(int)
#     finished = QtCore.Signal()

from PyQt6 import QtCore, QtWidgets

class progreeBar_Woker(QtCore.QObject):
    progressChanged = QtCore.pyqtSignal(int)
    finished = QtCore.pyqtSignal()

    def __init__(self,total_val):
        super().__init__()
        self._stopped = True
        self.total_val=total_val
        self.count = 0
    
    def run(self):
        
        self._stopped = False
        increment = int( self.total_val/100)
    
        while self.count < self.total_val and not self._stopped:  #
            #count += increment
            #increment += increment
            self.progressChanged.emit(self.count)
            #if increment >= 100:
            #    self._stopped=True
        else:
            self.count = 0
            self._stopped = True
            self.finished.emit()

    def stop(self):
        self._stopped = True



