# from PySide2 import QtCore, QtWidgets
#
# class progreeBar_Woker(QtCore.QObject):
#     progressChanged = QtCore.Signal(int)
#     finished = QtCore.Signal()

from PyQt6 import QtCore, QtWidgets
from icecream import ic 
# class progreeBar_Woker(QtCore.QObject):
#     progressChanged = QtCore.pyqtSignal(int)
#     finished = QtCore.pyqtSignal()

#     def __init__(self,total_val):
#         super().__init__()
#         self._stopped = True
#         self.total_val=total_val
#         self.count = 0
    
#     def run(self):
        
#         self._stopped = False
#         increment = int( self.total_val/100)
    
#         while self.count < self.total_val and not self._stopped:  #
#             #count += increment
#             #increment += increment
#             self.progressChanged.emit(self.count)
#             #if increment >= 100:
#             #    self._stopped=True
#         else:
#             self.count = 0
#             self._stopped = True
#             self.finished.emit()

#     def stop(self):
#         self._stopped = True

class CProgess_Bar_Thread(QtCore.QThread):
    PBar_Value_Changed_EVT = QtCore.pyqtSignal(int)
    def __init__(self, parent ,total_val,run_fn,prog_bar):
        super().__init__(parent)
        self.run_Fun= run_fn
        self._stopped = False
        self.total_val=total_val
        self.count = 0
        self.prog_bar =prog_bar
        
    def run(self):
        self._stopped = False
        while  not self._stopped:
            #self.run_Fun(self.count)
            self.prog_bar.setValue(self.count)
            ic(self.count)
        #self._stopped = True
        
    def stop(self):
        self._stopped = True

from PySide6.QtCore import (QRunnable,QThreadPool,QObject,Signal,Slot)
from PySide6.QtWidgets import QProgressBar
class Progress_Bar_Worker_Signals(QObject):
   #def __init__(self, parent = None,*args,**argkw):
   #    super().__init__(parent)
    """ Worker Thread events
    
    Args:
        QObject (_type_): _description_
       """
    Pbar_worker_Started_EVT  = Signal(str)
    Pbar_worker_Emit_progress_EVT =Signal(int)
    Pbar_worker_Finished_EVT = Signal(str)
    
class Progress_Bar_Worker(QRunnable):
    def __init__(self,mx_val: int,pbar:QProgressBar,rn_fn):
        super().__init__()
        self.Wrk_Thread_Events = Progress_Bar_Worker_Signals()
        self.max_Val = mx_val
        self.Pr_bar = pbar
        self.run_fn = rn_fn
        self.Pr_bar.setRange(0,self.max_Val)
        self.cur_val=0
        self._stop = False
    @Slot()
    def run(self):
        cv=0
        self.Wrk_Thread_Events.Pbar_worker_Started_EVT.emit("Progress Bar Thread Started")
       # cv= self.run_fn()
        while self.cur_val <= self.max_Val and not self._stop:
            self.Pr_bar.setValue(self.cur_val)
        self.Wrk_Thread_Events.Pbar_worker_Finished_EVT.emit("Progess Bar Thread finished")
        
    def stop(self):
       self._stop = True
        #return super().run()