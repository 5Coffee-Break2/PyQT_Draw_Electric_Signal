from global_modules import QtCore
import  App_Serial_Port.serial_model as sm
from icecream import ic

class Ser_PyQt_Thread(QtCore.QThread):
    ser_Comm_Started_EVT = QtCore.pyqtSignal(str)
    ser_Comm_End_EVT = QtCore.pyqtSignal(int)

    def __init__(self,ser_prt:sm.Serial_Model,fn_to_run):
        super().__init__()
        self._stopped = True
        self.total_counts = 0
        self.thrd_ser_prt = ser_prt
        self.Run_this_fn = fn_to_run
        #self.thrd_ser_prt.serial_Events.number_Received_pyqt_Event.connect(self.data_Rxed)
    
    def run(self):
        ic ("pyqt serial thread is running")
        self._stopped = False
        
        if self.total_counts and self._stopped is False:
            self.ser_Comm_Started_EVT.emit("Started")
            self.thrd_ser_prt.Get_All_Data() 
            self.Run_this_fn()
            self._stopped = True
            self.ser_Comm_End_EVT.emit(self.total_counts)   
        else:
            self._stopped = True
            ic(self.thrd_ser_prt.Get_Rxed_Bytes_Count)
            #self.thrd_ser_prt.Get_All_Data()
            #self.Run_this_fn()
          

    def stop(self):
        self._stopped = True
        
    #def data_Rxed(self,evt):
    #    ic(evt)