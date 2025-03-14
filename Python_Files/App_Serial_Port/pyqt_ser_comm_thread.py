from global_modules import QtCore
import  App_Serial_Port.serial_model as sm
from icecream import ic
from Matplot_Files.Matplot_Window import Matplot_Window
import numpy as np
class Ser_PyQt_Thread(QtCore.QThread):
    ser_Comm_Started_EVT = QtCore.pyqtSignal(str)
    ser_Comm_End_EVT = QtCore.pyqtSignal(tuple)

    def __init__(self,ser_prt:sm.Serial_Model,fn_to_run):
        super().__init__()
        self._stopped = True
        self.total_counts = 0
        self.thrd_ser_prt = ser_prt
        self.Run_this_fn = fn_to_run
        self.total_rxed_numbers=0
        self.tot_flt=0
        self.tot_int=0
        #self.thrd_ser_prt.serial_Events.number_Received_pyqt_Event.connect(self.data_Rxed)
    
    def run(self):
        ic ("pyqt serial thread is running")
        self._stopped = False
        
        if self.total_counts and not self._stopped :#self._stopped is False:
            self.ser_Comm_Started_EVT.emit("Started")
            tn=self.thrd_ser_prt.Get_All_Data() 
            if tn is not None:
                self.ser_Comm_End_EVT.emit(tn)
                self.tot_flt = tn[0]
                self.tot_int = tn[1]
                self.total_rxed_numbers = self.tot_flt + self.tot_int
                #self.Run_this_fn()
               
                self.Run_this_fn()
                self._stopped = True
            
        else:
            self._stopped = True
            ic(self.thrd_ser_prt.Get_Rxed_Bytes_Count)
            #self.thrd_ser_prt.Get_All_Data()
            #self.Run_this_fn()
          

    def stop(self):
        self._stopped = True
        
    #def data_Rxed(self,evt):
    #    ic(evt)
class Display_A_Curve_Thread(QtCore.QThread):
    def __init__(self,fn_to_run,ser_prt:sm.Serial_Model,matplot_obj):
        super().__init__()
        self._stopped = False#True
        self.need_update = False
        self.Run_this_fn = fn_to_run
        self.thrd_ser_prt = ser_prt
        self.matplot_obj = matplot_obj
        #------------------------------------------------------------------------
        self.mpl_widget:Matplot_Window = Matplot_Window(None,self.thrd_ser_prt.rx_Times_Buffer,self.thrd_ser_prt.rx_Voltages_Buffer)#MplCanvas(self, width=5, height=4, dpi=100)
        #self.mpl_widget:Matplot_Window = Matplot_Window(self.main_Wind,self.app_serPrt_model.rx_Times_Buffer,self.app_serPrt_model.rx_Voltages_Buffer)#MplCanvas(self, width=5, height=4, dpi=100)
        self.matplot_obj= self.mpl_widget     
        self.matplot_obj.mpl_widget.mpl_Axes.set_xlabel("Time (s)")
        self.matplot_obj.mpl_widget.mpl_Axes.set_ylabel("Voltage (volt)")
        self.matplot_obj.mpl_widget.mpl_Axes.set_title("Thread Display Capcitor Charging  Curve")
        self.matplot_obj.mpl_widget.mpl_Axes.grid()
        self.plot_Lines= self.matplot_obj.mpl_widget.mpl_Axes.plot(self.thrd_ser_prt.rx_Times_Buffer,self.thrd_ser_prt.rx_Voltages_Buffer)
        #self.matplot_obj.mpl_widget.Setup_Cursor()  
     
#
# self.thrd_ser_prt.serial_Events.number_Received_pyqt_Event.connect(self.data_Rxed)
    # Function to update the plot
    def Update_Fig_Plot(self,start_ind,end_ind):
        #-if(self.need_update and new_x.__len__() > 0 and new_y.__len__() > 0):
        #-    if len(new_x) < len(new_y):
        #-        new_y.pop(-1)
        #-    
        #-    if new_x.__len__() > new_y.__len__():
        #-        new_x.pop(-1)
        #-               
            time_array = np.array(self.thrd_ser_prt.rx_Times_Buffer)
            voltage_array = np.array(self.thrd_ser_prt.rx_Voltages_Buffer)
           
            # Code review comment -> Added error handling to catch and print any exceptions that occur during the update process.
            try:
                #self.plot_Lines[-1].set_ydata(new_y)
                #self.plot_Lines[-1].set_xdata(new_x)
                
                #-self.plot_Lines[-1].set_ydata(new_y[:end_indx])
                #-self.plot_Lines[-1].set_xdata(new_x[:end_indx])

                #-self.matplot_obj.mpl_widget.mpl_Axes.draw_artist(self.plot_Lines[-1])
                #self.matplot_obj.mpl_widget.mpl_Fig.canvas.restore_region(self.matplot_obj.mpl_widget.mpl_Axes.bbox)
                if self.thrd_ser_prt.rx_Times_Array_Buff.size > 0 :
                    self.matplot_obj.mpl_widget.mpl_Axes.set_xlim(0, time_array.max())
                    self.matplot_obj.mpl_widget.mpl_Axes.set_ylim(0, voltage_array.max())
                
                self.plot_Lines[0].set_ydata(self.thrd_ser_prt.rx_Voltages_Buffer[start_ind:end_ind])
                self.plot_Lines[0].set_xdata(self.thrd_ser_prt.rx_Times_Buffer[start_ind:end_ind])
                #-self.matplot_obj.mpl_widget.mpl_Axes.draw_artist(self.plot_Lines[0])
                #-self.matplot_obj.mpl_widget.mpl_Fig.canvas.restore_region(self.matplot_obj.mpl_widget.mpl_Axes.bbox)
                
                #self.matplot_obj.mpl_widget.mpl_Fig.canvas.blit(self.matplot_obj.mpl_widget.mpl_Axes.bbox)
                self.matplot_obj.mpl_widget.mpl_Fig.canvas.draw()
                #self.matplot_obj.mpl_widget.mpl_Fig.canvas.flush_events()
                self.need_update = False
                
            except Exception as e:
                # Code review comment -> Added error handling to catch and print any exceptions that occur during the update process.
                print(f"An error occurred: {e}")

    def run(self):
        ic ("Displaying Curve thread is running")
        #-self._stopped = False
        #-self.Run_this_fn()
        start_indx,end_indx = 0,0
        
        while self._stopped is False:
            if len(self.thrd_ser_prt.rx_Times_Buffer,) < len(self.thrd_ser_prt.rx_Voltages_Buffer):
                end_indx = len(self.thrd_ser_prt.rx_Times_Buffer,)  
            elif  len(self.thrd_ser_prt.rx_Times_Buffer,) > len(self.thrd_ser_prt.rx_Voltages_Buffer):
                end_indx = len(self.thrd_ser_prt.rx_Voltages_Buffer)
            else:
                end_indx = len(self.thrd_ser_prt.rx_Times_Buffer,)   
            self.Update_Fig_Plot(start_indx,end_indx)#self.Update_Fig_Plot(self.thrd_ser_prt.rx_Times_Buffer,self.thrd_ser_prt.rx_Voltages_Buffer)
            start_indx = end_indx
            #-ic(self.thrd_ser_prt.rx_Times_Buffer.__len__())
            #-ic(self.thrd_ser_prt.rx_Voltages_Buffer.__len__( ))
        #    if self.thrd_ser_prt.rx_Times_Buffer.__len__() < self.thrd_ser_prt.rx_Voltages_Buffer.__len__():
        #        self.thrd_ser_prt.rx_Voltages_Buffer.pop()
        #    elif self.thrd_ser_prt.rx_Times_Buffer.__len__() > self.thrd_ser_prt.rx_Voltages_Buffer.__len__():
        #        self.thrd_ser_prt.rx_Times_Buffer.pop()
        #    self.matplot_obj.mpl_widget.axes.plot(self.thrd_ser_prt.rx_Times_Buffer,self.thrd_ser_prt.rx_Voltages_Buffer)
        
        #self._stopped = True
            
    def stop(self):
        self._stopped = True