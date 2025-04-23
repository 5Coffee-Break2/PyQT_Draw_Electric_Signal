import App_Serial_Port.serial_model as sm
import global_modules as gm
from PyQt6.QtWidgets import QListWidgetItem, QMainWindow
from PyQt6.QtCore import QThread
from Draw_Electrical_signal_MApp import MainWindow,Ui_MainWindow
from App_Serial_Port import pyqt_ser_comm_thread

from Progress_Bar_Thread.rx_progress_bar import *#CProgess_Bar_Thread,Progress_Bar_Worker
from Matplot_Files.Matplot_Window import Matplot_Window
import csv
import numpy as np
from icecream import ic
RX_PERIOD = 500
time_differnce_lst=[]
voltage_difference_list=[]
errors_list = []    
class Main_Wind_Handlers:
    
    def __init__(self,client:MainWindow):
        
        self.main_Wind =client
        self.main_Wind_Ui = client.ma_Window
        self.app_serPrt_model = sm.Serial_Model(self.main_Wind_Ui,self.onRx_TxSerial)
        self.ser_port_name = None
        #self.main_Wind_Ui.data_rxed_prgBar.setMaximum(2000)
        self.app_serPrt_model.serial_Events.number_Received_pyqt_Event.connect(self.onNumber_Received)
       #@ PyQt Serial communication thread
        self.pyqt_ser_thread = pyqt_ser_comm_thread.Ser_PyQt_Thread(self.app_serPrt_model,self.Display_Received_Data)
        self.receved_Bytes_count:int = 1
        self.fn_counts = 0
        self.int_counts = 0    
        self.mpl3_widget:Matplot_Window | None = None
        self.plot_thrd  = None  #-self.plot_thrd = pyqt_ser_comm_thread.Display_A_Curve_Thread(None,self.app_serPrt_model,None)
        self.mpl3_lines=None 
    
    def onSend_Command(self,btn_evt):
        snd_cmd_str:str = self.main_Wind_Ui.send_num_EdtWdg.text()
        self.app_serPrt_model.Get_Active_Port().flush()     #reset_output_buffer()
        self.app_serPrt_model.Send_Message(snd_cmd_str)
        ic(snd_cmd_str,"Command sent")
        #self.app_serPrt_model.Send_Command(snd_cmd_str)
       
    
    def oNSend_Number_Clk(self,btn_evt)-> None:
        snd_num_str:str = self.main_Wind_Ui.send_num_EdtWdg.text()
        snd_num_typ:bytes = None
        if (snd_num_str.__contains__('.')) and (snd_num_str.replace('.','').isnumeric()):
           snd_num_typ = sm.pyAvrSer.Atmega_USART.SOB_F_By
        elif snd_num_str.isnumeric():
            snd_num_typ = sm.pyAvrSer.Atmega_USART.SOB_I_By
        self.app_serPrt_model.Get_Active_Port().flush()     #reset_output_buffer()
        self.app_serPrt_model.Send_Number(snd_num_typ,snd_num_str)
        
        
    def onNumber_Received(self,num):
        self.main_Wind_Ui.data_rxed_prgBar.setValue(self.app_serPrt_model.Get_Rxed_Bytes_Count)
        #ic(num)
        #ic(self.main_Wind_Ui.data_rxed_prgBar.maximum())
    
    def onRx_Error(self,evt):
        """_summary_
            Event handler for serial port error
        Args:
            evt (_type_: Error_Event): _description_: event argument
        """
        errors_list.append(evt)
        gm.QtWidgets.QMessageBox(parent=self.main_Wind,
                                 icon=gm.QtWidgets.QMessageBox.Icon.Warning,
                                 text=f"Error : {evt}").show()
    
    def Display_Curve(self):
        """_summary_
            This function is used to display the curve of the received data
        """
        mpl2_widget:Matplot_Window = Matplot_Window(self.main_Wind,self.app_serPrt_model.rx_Times_Buffer,self.app_serPrt_model.rx_Voltages_Buffer)#MplCanvas(self, width=5, height=4, dpi=100)
             
        mpl2_widget.axes.set_xlabel("Time (s)")
        mpl2_widget.axes.set_ylabel("Voltage (volt)")
        mpl2_widget.axes.set_title("Display Capcitor Charging  Curve")
        mpl2_widget.axes.grid()
        mpl2_widget.axes.plot(self.app_serPrt_model.rx_Times_Buffer,self.app_serPrt_model.rx_Voltages_Buffer) 
        mpl2_widget.Setup_Cursor() 
    
    def Setup_MPL_Widget(self):
        """_summary_
            This function is used to setup the Matplot widget
        """ 
        self.mpl3_widget:Matplot_Window = Matplot_Window(self.main_Wind,self.app_serPrt_model.rx_Times_Buffer,self.app_serPrt_model.rx_Voltages_Buffer)#MplCanvas(self, width=5, height=4, dpi=100)
        self.mpl3_widget.mpl_widget.mpl_Axes.set_xlabel("Time Instantly (s)")
        self.mpl3_widget.mpl_widget.mpl_Axes.set_ylabel("Voltage Instantly (volt)")
        self.mpl3_widget.mpl_widget.mpl_Axes.set_title("Display Capcitor Charging  Curve instantly")
        self.mpl3_widget.mpl_widget.mpl_Axes.grid()
        # self.mpl3_lines = self.mpl3_widget.mpl_widget.mpl_Axes.plot(self.app_serPrt_model.rx_Times_Buffer,self.app_serPrt_model.rx_Voltages_Buffer) 
        # self.mpl3_widget.mpl_widget.Setup_Cursor() 
    
    def Receive_Data_Thrid_Instantly(self):
        """_summary_
            This function is used to receive data from the serial port instantly
        """
        ic("Waiting for data")
        ci:int=0
        rows_nums = 1
        int_nums_count = 0
        float_nums_count = 0
        
        self.main_Wind.rx_Timer.stop()   
        
        if self.app_serPrt_model.Get_Rxed_Bytes_Count :
            ic("Data received")
            self.main_Wind_Ui.volt_time_Tabel.clearContents() 
 
            if self.plot_thrd is None:
                self.plot_thrd = pyqt_ser_comm_thread.Display_A_Curve_Thread(None,self.app_serPrt_model,None)
                self.plot_thrd.start()
            elif not self.plot_thrd.isRunning():
                self.plot_thrd.start()
                    
            while self.app_serPrt_model.Get_Rxed_Bytes_Count :
                rd=self.app_serPrt_model.Get_A_Rxed_Number()
                if(rd is not None):
                    #--self.main_Wind_Ui.volt_time_Tabel.setRowCount(rows_nums)
                    #if it's a float number represents the Time
                    if rd[0] == b'A':#float:    
                        float_nums_count+=1
                        #self.main_Wind_Ui.volt_time_Tabel.insertRow(rows_nums)
                        self.main_Wind_Ui.volt_time_Tabel.setItem(ci,0,gm.QtWidgets.QTableWidgetItem(str(self.app_serPrt_model.rx_Times_Buffer[ci])))        
                     #if it's a n integer number represents the Voltage
                    elif rd[0] == b'I': #is int :
                        ##@ Add Row data to the list contro
                        self.main_Wind_Ui.volt_time_Tabel.setItem(ci,1,gm.QtWidgets.QTableWidgetItem(str(self.app_serPrt_model.rx_Voltages_Buffer[ci])))
                        int_nums_count +=1
                        rows_nums+=1
                        ci+=1
                        self.main_Wind_Ui.volt_time_Tabel.setRowCount(rows_nums) 
                        self.plot_thrd.need_update = True
                        
        else:
            if  self.plot_thrd is not None and self.plot_thrd.isRunning():
                self.plot_thrd.stop()
                ic("Thread stopped")                            
                       
        ic(f"Floats: {float_nums_count} , Ints: {int_nums_count}")
        ic("total numbers received",float_nums_count+int_nums_count)
        ic("Data received finished display thread has been stopped")    
        self.main_Wind.statusBar_L2.setText(f" FLOAT: {float_nums_count} <--> INT: {int_nums_count}")   
        self.main_Wind.statusBar_L3.setText(f"Total inc : {float_nums_count+int_nums_count}")       
       
        self.main_Wind.rx_Timer.start(RX_PERIOD)
            
    def Receive_Data_Immediately(self):
        """_summary_
            This function is used to receive data from the serial port immediately
        """
        ic("Waiting for data")
        ci:int=0
        rows_nums = 1
        int_nums_count = 0
        float_nums_count = 0
        #self.main_Wind_Ui.volt_time_Tabel.clearContents() 
        self.main_Wind.rx_Timer.stop()   
        if self.app_serPrt_model.Get_Rxed_Bytes_Count :
            ic("Data received")
            
            
            
            if self.mpl3_widget is None:
                self.Setup_MPL_Widget()
                self.mpl3_lines = self.mpl3_widget.mpl_widget.mpl_Axes.plot(self.app_serPrt_model.rx_Times_Buffer,self.app_serPrt_model.rx_Voltages_Buffer) 
                self.mpl3_widget.mpl_widget.Setup_Cursor()
           
            #!!if not self.plot_thrd.isRunning():
            #!!    self.plot_thrd.start()
           #!
            #!#-------------------------------
           #!
            
            #-self.mpl3_lines[0].set_xdata(self.app_serPrt_model.rx_Times_Buffer)
            #-self.mpl3_lines[0].set_ydata(self.app_serPrt_model.rx_Voltages_Buffer) 
            #!
          
            #ic(self.mpl3_lines[0])
            if not self.mpl3_widget.isVisible():
                self.mpl3_widget.show() 
            self.app_serPrt_model.Reset_RX_TX_Data_Buffer(True,True)
            while self.app_serPrt_model.Get_Rxed_Bytes_Count :
                rd=self.app_serPrt_model.Get_A_Rxed_Number()
                if(rd is not None):
                    #--self.main_Wind_Ui.volt_time_Tabel.setRowCount(rows_nums)

                    #if it's a float number represents the Time
                    if rd[0] == b'A':#float:    
                        float_nums_count+=1
                        self.main_Wind_Ui.volt_time_Tabel.insertRow(rows_nums)
                        self.main_Wind_Ui.volt_time_Tabel.setItem(ci,0,gm.QtWidgets.QTableWidgetItem(str(self.app_serPrt_model.rx_Times_Buffer[ci])))  
                        
                    #if it's a n integer number represents the Voltage
                    elif rd[0] == b'I': #is int :
                        ##@ Add Row data to the list contro
                        self.main_Wind_Ui.volt_time_Tabel.setItem(ci,1,gm.QtWidgets.QTableWidgetItem(str(self.app_serPrt_model.rx_Voltages_Buffer[ci])))
                        int_nums_count+=1
                        
                        rows_nums+=1
                        ci+=1
                         
                        #!!self.plot_thrd.need_update = True
                        
                            
                        self.mpl3_lines[0].set_xdata(self.app_serPrt_model.rx_Times_Buffer)
                        self.mpl3_lines[0].set_ydata(self.app_serPrt_model.rx_Voltages_Buffer)
                        self.app_serPrt_model.rx_Voltages_Array_Buff = np.array(self.app_serPrt_model.rx_Voltages_Buffer,float)
                        self.app_serPrt_model.rx_Times_Array_Buff=np.array(self.app_serPrt_model.rx_Times_Buffer,dtype=float)
                        
                        if self.app_serPrt_model.rx_Voltages_Buffer.__len__() > self.app_serPrt_model.rx_Times_Buffer.__len__():
                            self.app_serPrt_model.rx_Voltages_Buffer.pop(-1)
                        
                        if self.app_serPrt_model.rx_Voltages_Buffer.__len__() < self.app_serPrt_model.rx_Times_Buffer.__len__():
                            self.app_serPrt_model.rx_Times_Buffer.pop(-1)
                             
                        #self.mpl3_widget.mpl_widget.draw()
                        if self.app_serPrt_model.rx_Times_Array_Buff.size > 0 :
                            self.mpl3_widget.mpl_widget.mpl_Axes.set_xlim(0,self.app_serPrt_model.rx_Times_Array_Buff.max())
                            self.mpl3_widget.mpl_widget.mpl_Axes.set_ylim(0,self.app_serPrt_model.rx_Voltages_Array_Buff.max())
                        
                        self.mpl3_widget.mpl_widget.mpl_Fig.canvas.blit(self.mpl3_widget.mpl_widget.mpl_Axes.bbox)  
                        try:
                            self.mpl3_widget.mpl_widget.mpl_Fig.canvas.draw()#draw_artist(self.mpl3_lines[0])
                            #self.mpl3_widget.mpl_widget.mpl_Fig.canvas.draw()#draw_artist(self.mpl3_lines[0])
                        #self.mpl3_widget.mpl_widget.mpl_Fig.canvas.flush_events()   
                        except ValueError as e:#except Exception as e:#valueError as e:
                            ic()
                            ic(e)
                        self.mpl3_widget.mpl_widget.mpl_Fig.canvas.flush_events()
                            
        ic(f"Floats: {float_nums_count} , Ints: {int_nums_count}")
        ic("total numbers received",float_nums_count+int_nums_count)
        ic("Data received finished")    
        self.main_Wind.statusBar_L2.setText(f" FLOAT: {float_nums_count} <--> INT: {int_nums_count}")   
        self.main_Wind.statusBar_L3.setText(f"Total inc : {float_nums_count+int_nums_count}")       
        #!!else:
        #!!    if self.plot_thrd.isRunning():
        #!!        self.plot_thrd.stop()        
        self.main_Wind.rx_Timer.start(RX_PERIOD)
        # ic()
        # 
        # ic(self.app_serPrt_model.Get_Rxed_Bytes_Count)
        #ic(self.app_serPrt_model.rx_Times_Buffer.__len__())
        #
        #ic(self.app_serPrt_model.rx_Times_Array_Buff.size)  
        # self.app_serPrt_model.Get_Received_Data()
        # self.Display_Received_Data()
        #-if self.app_serPrt_model.Get_Rxed_Bytes_Count > 0:
        #-    try:
#-
        #-            self.main_Wind.statusBar_L2.setText(f"Progress: ")
        #-            self.main_Wind.statusBar_L1.setText(f"Total Rxed Nums Count : {self.app_serPrt_model.Get_Rxed_Bytes_Count}")
        #-            #@Reset the progress var value
        #-            #self.main_Wind_Ui.data_rxed_prgBar.setValue(0)
        #-            
        #-            #@ if thread didn't start yet creat anew one and start it
        #-            if not self.pyqt_ser_thread.isRunning(): #if not self.ser_thread.isRunning() :
        #-                #@ start the thread
        #-                self.pyqt_ser_thread = pyqt_ser_comm_thread.Ser_PyQt_Thread(self.app_serPrt_model,self.Display_Received_Data)
        #-                self.pyqt_ser_thread.total_counts = self.receved_Bytes_count
        #-                self.pyqt_ser_thread.ser_Comm_Started_EVT.connect(self.onSer_Thread_Started)
        #-                self.pyqt_ser_thread.ser_Comm_End_EVT.connect(self.onSer_Thread_Finished)
        #-                self.pyqt_ser_thread.start()
        #-                #Below statment has been moved to serial communiation thread stop or end event
                       
            #except Exception as e_thr:
            #    ic()
            #    ic("Error in starting thread I'm executing exception routine")
            #    ic(e_thr)
            #_    self.pyqt_ser_thread = pyqt_ser_comm_thread.Ser_PyQt_Thread(self.app_serPrt_model,self.Display_Received_Data)
            #_    self.pyqt_ser_thread.total_counts = self.receved_Bytes_count
            #_    self.pyqt_ser_thread.ser_Comm_Started_EVT.connect(self.onSer_Thread_Started)
            #_    self.pyqt_ser_thread.ser_Comm_End_EVT.connect(self.onSer_Thread_Finished)
            #_    self.pyqt_ser_thread.start()
            #_    #-self.rx_Thread = gm.thrd.Thread(target=self.Get_Received_Data)
            #_    #-self.rx_Thread.start()
            #_#-self.main_Wind_Ui.rx_Timer.Start(gm.mwm.RX_CHECK_EVERY)
                
        #@ if receiving data is in progess and not finished yet
        #else:
        #    #@ update the received data count with the last data size that received
        #    self.receved_Bytes_count = self.app_serPrt_model.Get_Rxed_Bytes_Count
        #    self.main_Wind.statusBar_L3.setText(f"Rxed Bytes Count: {self.receved_Bytes_count}" )
    
    
    def Receive_All_Data_and_Run(self):
        """_summary_
            This function is used to receive all data from the serial port and run the thread to process the received data
        """
        #@ if there's a received data in the buffer
        if self.app_serPrt_model.Get_Rxed_Bytes_Count: 
            #@ if received data in the buffer is completed and no other data will be received
            if self.receved_Bytes_count == self.app_serPrt_model.Get_Rxed_Bytes_Count  :
                try:
                    
                    self.main_Wind.statusBar_L2.setText(f"Progress: ")
                    self.main_Wind.statusBar_L1.setText(f"Total Rxed Nums Count :")
                    #@Reset the progress var value
                    self.main_Wind_Ui.data_rxed_prgBar.setValue(0)
                    
                    #@ if thread didn't start yet creat anew one and start it
                    if not self.pyqt_ser_thread.isRunning(): #if not self.ser_thread.isRunning() :
                        #@ approximation for the total received numbers count to set the progress bar max value (6.5and 31 values are estimated by experements)
                        self.main_Wind_Ui.data_rxed_prgBar.setMaximum(self.receved_Bytes_count)#(int(self.receved_Bytes_count/6.4+31))
                        #@ start the thread
                        self.pyqt_ser_thread = pyqt_ser_comm_thread.Ser_PyQt_Thread(self.app_serPrt_model,self.Display_Received_Data)
                        self.pyqt_ser_thread.total_counts = self.receved_Bytes_count
                        self.pyqt_ser_thread.ser_Comm_Started_EVT.connect(self.onSer_Thread_Started)
                        self.pyqt_ser_thread.ser_Comm_End_EVT.connect(self.onSer_Thread_Finished)
                        self.pyqt_ser_thread.start()
                        #Below statment has been moved to serial communiation thread stop or end event
                       
                except Exception as e_thr:
                    ic()
                    ic("Error in starting thread I'm executing exception routine")
                    ic(e_thr)
                #_    self.pyqt_ser_thread = pyqt_ser_comm_thread.Ser_PyQt_Thread(self.app_serPrt_model,self.Display_Received_Data)
                #_    self.pyqt_ser_thread.total_counts = self.receved_Bytes_count
                #_    self.pyqt_ser_thread.ser_Comm_Started_EVT.connect(self.onSer_Thread_Started)
                #_    self.pyqt_ser_thread.ser_Comm_End_EVT.connect(self.onSer_Thread_Finished)
                #_    self.pyqt_ser_thread.start()
                #_    #-self.rx_Thread = gm.thrd.Thread(target=self.Get_Received_Data)
                #_    #-self.rx_Thread.start()
                #_#-self.main_Wind_Ui.rx_Timer.Start(gm.mwm.RX_CHECK_EVERY)
                
            #@ if receiving data is in progess and not finished yet
            else:
                #@ update the received data count with the last data size that received
                self.receved_Bytes_count = self.app_serPrt_model.Get_Rxed_Bytes_Count
                self.main_Wind.statusBar_L3.setText(f"Rxed Bytes Count: {self.receved_Bytes_count}" )
        
        #@_ic("Serial thread running status",self.pyqt_ser_thread.isRunning()) 
        #@_ic("Timer event finished")
         
    def onrxTimer(self):
        """_summary_
        This function to check periodically(time period = RX_CHECK_EVERY) if there's a received data in the rx serial buffeer
        this version of the function will executed only if there was a received data in the buffer and no other data will be received
            tim_evt (_type_): _description_
        """
       
       #_ ic("Timer event started")
        #-self.Receive_All_Data_and_Run()  
        #__TMP_DISABLED     self.Receive_Data_Immediately()         
        #self.Receive_Data_Thrid_Instantly()
    
    def onSer_Thread_Started(self,evt_ob):
       #@ stop the event timer
        self.main_Wind.rx_Timer.stop()   
        #-ic(evt_ob)
        
    def onSer_Thread_Finished(self,evt_ob):
     
        #-ic("Thread finished")
        #-ic(type(evt_ob),evt_ob)
        #@ if all data was read and there's no received data in the buffer then reset the received data count 
        self.receved_Bytes_count=1
        #@ start the event timer to check if there's a received data in the buffer
        self.main_Wind.rx_Timer.start(RX_PERIOD)                       
        
        ic()
        ic(type(evt_ob))
        ic(evt_ob,evt_ob[0],evt_ob[1])
        
        self.fn_counts= evt_ob[0]
        self.int_counts = evt_ob[1] 
        #self.main_Wind_Ui.data_rxed_prgBar.setMaximum(evt_ob[0]+evt_ob[1])
        
    def onlist_Sys_Ports_btn(self,btn_evnt):
        """_summary_
            Event Handler for Get Ports list button
        Args:
            btn_evnt (_type_: CommandEvent): event argument
        """
        self.app_serPrt_model.Get_System_Comm_Ports()
        #-ic(type(btn_evnt))
        com_ports = self.app_serPrt_model.Get_System_Comm_Ports() 
        self.main_Wind_Ui.ser_ports_LstVu.clear()#Pan_1_Mw.ser_ports_lst_ctrl.Clear()
        self.main_Wind_Ui.ser_ports_LstVu.addItems(com_ports) 
        
    def onRx_TxSerial(self,serevt):
        """_summary_
            an event handler for the custom event Serial Communication Event
        Args:
            serevt (_type_: Data_Received_Event): _description_: serial event argument
        """
                
    def onSelectPort(self,lst_ctrl_evt):
        """
        _summary_
            An event handler for list box item is selcted
        Args:
            lst_ctrl_evt (CommandEvent): an event argument
        """
        selcted_item: QListWidgetItem | None = self.main_Wind_Ui.ser_ports_LstVu.currentItem()
        if selcted_item is not None:
            #@ Get the string of the selcted item from the list box
            selcted_string:str = selcted_item.text() #lst_ctrl_evt.GetString()
            #@ Extract the serial port name to open it(get the chars count until the 1st space char)
            #@ split the selected item string to list of words and select the first word
            self.ser_port_name:str = str.split(selcted_string)[0]
            #self.main_Wind_Ui.m_window_status_bar.SetForegroundColour(gm.wx.Colour(255,255,0,100))
            #self.main_Wind_Ui.m_window_status_bar.SetStatusText(self.ser_port_name,0)
            #@ from open port handler
            if self.ser_port_name is not None:
                self.app_serPrt_model.Activate_Serial_Port(self.ser_port_name,57600)
                #-ic(type(lst_ctrl_evt))
                if self.app_serPrt_model.Is_Active_Port_Opend():
                    self.App_Serial_Port= self.app_serPrt_model.Get_Active_Port()
                    self.main_Wind_Ui.ser_Port_Info_LabCrl.setText(self.app_serPrt_model.Get_Active_Port_info())
                    self.main_Wind.rx_Timer.start(RX_PERIOD)
        else:
            self.main_Wind_Ui.ser_Port_Info_LabCrl.setText("No port selected")
            gm.QtWidgets.QMessageBox(parent=self.main_Wind,
                                     icon=gm.QtWidgets.QMessageBox.Icon.Warning,
                                     text="No port selected").show()
            
    
    def onOpenPort_btn(self,btn_evt):
        """_summary_
            An event handler for Open port Button
        Args:
            btn_evt (_type_: CommandEvent): _description_: An event argument
        """
        if self.ser_port_name is not None:
            self.app_serPrt_model.Activate_Serial_Port(self.ser_port_name,57600)
            #@ Register the error event
            self.app_serPrt_model.Register_in_Error_Event(self.onRx_Error)
            #ic(type(btn_evt),btn_evt.GetEventObject())
            if self.app_serPrt_model.Is_Active_Port_Opend():
                self.App_Serial_Port= self.app_serPrt_model.Get_Active_Port()
                self.main_Wind_Ui.ser_Port_Info_LabCrl.setText(self.app_serPrt_model.Get_Active_Port_info())
            #_TEMP_diSABLED__    self.main_Wind.rx_Timer.start()
    
    
    def Get_Received_Data(self, disab_btn: gm.QtWidgets.QPushButton = None):
        #region
        """
        Handles the reception of data and updates the UI accordingly.
        This method performs the following tasks:
        1. Temporarily disables the event timer.
        2. Updates the total bytes received.
        3. Clears the contents of the voltage-time table.
        4. Starts a progreeBar_Woker thread to process the received data.
        5. Iterates through the received voltage data and updates the table.
        6. Updates the status bar with the current received data information.
        7. Re-enables the "Get data" button after processing is complete.
        8. Resets the received data count if all data has been read.
        Args:
            disab_btn (gm.QtWidgets.QPushButton, optional): The button to disable during data reception. Defaults to None.
        """
        
        #@@@@@@@@@@ This function needs an update for the code to work well since it has been replaced by  Display_Received_Data function  @@@@@@@@@@@@@@@
        #endregion
        #@ temporary dissable the event timer
   #-     self.main_Wind_Ui.rx_Timer.Stop()
        self.total_bytes_received += self.app_serPrt_model.Get_Rxed_Bytes_Count
        #self.main_Wind_Ui.m_window_status_bar.SetStatusText((str(self.total_bytes_received)),0)
        if self.app_serPrt_model.Get_All_Data() is not None:
            self.thread_started = True
            self.thread_finished = False
            self.main_Wind_Ui.volt_time_Tabel.clearContents()
            
            
            self.main_Wind_Ui.data_rxed_prgBar.setRange(0,self.app_serPrt_model.rx_Voltages_Array_Buff.size)
            self.main_Wind.mW_Extra_Widgets.debug_label_DB1.setText(f"PROGRESS BAR RANGE : {self.main_Wind_Ui.data_rxed_prgBar.maximum()}")
            
            self.main_Wind.progreeBar_Woker.total_val = self.app_serPrt_model.rx_Voltages_Array_Buff.size
            self.main_Wind.statusBar_L3.setText(f"Total inc : {self.main_Wind.progreeBar_Woker.total_val /100}")
            #self.main_Wind.progress_Bar_Thread.start()
            self.main_Wind.progress_Bar_Thread.started.connect(self.main_Wind.progreeBar_Woker.run)
            
            #@Start processing the received data here
            for ind,vol_valu in enumerate( self.app_serPrt_model.rx_Voltages_Array_Buff ):
                self.main_Wind.progreeBar_Woker.count = ind
                #self.main_Wind.progress.setValue(ind)
                #@ dissable the Get data button
                if disab_btn is not None:
                    disab_btn.setEnabled(False)
                try:
                    ##@ Add columns data to the list contro
                    
                    self.main_Wind_Ui.volt_time_Tabel.insertRow(ind)
                    self.main_Wind_Ui.volt_time_Tabel.setItem(ind,0,gm.QtWidgets.QTableWidgetItem(str(self.app_serPrt_model.rx_Times_Array_Buff[ind])))
                    self.main_Wind_Ui.volt_time_Tabel.setItem(ind,1,gm.QtWidgets.QTableWidgetItem(str(vol_valu)))   
                    #@ update the status bar with the current received data info
                    #self.main_Wind.statusBar().showMessage(f"(Received count Time/ Volt : {self.app_serPrt_model.rx_Times_Array_Buff.size} / {self.app_serPrt_model.rx_Voltages_Array_Buff.size}",1)
                    #self.main_Wind.statusBar_L1.setText(f"Items count : {self.main_Wind_Ui.volt_time_Tabel.rowCount()}")
                    #self.main_Wind_Ui.m_window_status_bar.SetStatusText(f"Items count : {self.main_Wind_Ui.Pan_2_Mw.data_lstBx.GetItemCount()}",2)
                    if  ind ==  self.main_Wind.progreeBar_Woker.total_val -1:
                        self.main_Wind.progreeBar_Woker.stop()
                        #self.main_Wind_Ui.rx_Timer.stop()
                        #self.main_Wind_Ui.rx_Timer.timeout.connect(self.onhandleFinished)
                        #self.main_Wind_Ui.rx_Timer.start(RX_PERIOD)
                        #self.main_Wind_Ui.rx_Timer.timeout.connect(self.onrxTimer) 
                except IndexError:
                    self.main_Wind.statusBar().showMessage("Index Error"+(str(self.app_serPrt_model.Get_Rxed_Bytes_Count)))
                  
            #@ after data received finished re-enabel the Get received data button
            if disab_btn is not None:
                 disab_btn.setEnabled(True)
        
         #@ if all data was read and there's no received data in the buffer        
        if self.app_serPrt_model.Get_Rxed_Bytes_Count == 0:
            self.thread_finished = True
            self.thread_started = False
            self.total_bytes_received = 0
            #@ reset the received data count to be ready for the next data receiving if  we replaceed the 1 with zero the thread will not start again
            self.receved_Bytes_count = 1
            #self.main_Wind.Setup_Matplot_Widget()
            
       #else:
       #    self.Get_Received_Data()
    #-def Update_Progess_Bar_Value(self,val ):
    #-        self.main_Wind_Ui.data_rxed_prgBar.setValue(val)
    #-        self.main_Wind.statusBar_L2.setText(f"Progress: {val}")
    
    def Display_Received_Data(self):
        voltage_difference_list.clear()
        time_differnce_lst.clear()
        #ic(type(self.Display_Received_Data))
        total_numbers_Received = self.pyqt_ser_thread .total_rxed_numbers# self.app_serPrt_model.rx_Times_Array_Buff.size
        #-max_index_value = total_numbers_Received -1
        #----self.main_Wind.progress_Bar_Thread.start()
        #-self.Prog_Bar_Thread_2= CProgess_Bar_Thread(self.main_Wind,self.app_serPrt_model.rx_Voltages_Array_Buff.size,self.Update_Progess_Bar_Value,self.main_Wind_Ui.data_rxed_prgBar)
        #-self.Prog_Bar_Thread_2.start()
        #@ below Moved to the timer rx event
        
        self.fn_counts = self.pyqt_ser_thread.tot_flt
        self.int_counts = self.pyqt_ser_thread.tot_int
        
        self.main_Wind_Ui.volt_time_Tabel.clearContents()
        if self.fn_counts >= self.int_counts:
            self.main_Wind_Ui.volt_time_Tabel.setRowCount(self.int_counts)
        else:   
            self.main_Wind_Ui.volt_time_Tabel.setRowCount(self.fn_counts)
        
        #@Set the progress bar
        #----prg_bar_thrd = Progress_Bar_Worker(total_numbers_Received,self.main_Wind_Ui.data_added_prgBar,None)
        #----thread_pool = QThreadPool()
        #----thread_pool.start(prg_bar_thrd)
        #-self.main_Wind_Ui.data_rxed_prgBar.setRange(0,(total_numbers_Received))
        
        #@ Update the statusbar with the total received data count
        self.main_Wind.statusBar_L1.setText(f"Total Rxed Nums Count :  {total_numbers_Received } = FLT: {self.fn_counts} + INT: {self.int_counts}")
        #-ic(self.app_serPrt_model.rx_Voltages_Array_Buff.shape)
        #-ic(self.app_serPrt_model.rx_Times_Array_Buff.shape)
        
        for ind,time_valu in enumerate( self.app_serPrt_model.rx_Times_Array_Buff ):#for ind,vol_valu in enumerate( self.app_serPrt_model.rx_Voltages_Array_Buff ):
           # try:
                #@ dissable the Get data button
                #if disab_btn is not None:
                #    disab_btn.setEnabled(False)
                ##@ Add columns data to the list contro
                self.main_Wind_Ui.volt_time_Tabel.setItem(ind,0,gm.QtWidgets.QTableWidgetItem(str(time_valu)))
                #self.main_Wind_Ui.volt_time_Tabel.setItem(ind,1,gm.QtWidgets.QTableWidgetItem(str(self.app_serPrt_model.rx_Voltages_Array_Buff[ind])))   
        for indvol,vol_valu in enumerate( self.app_serPrt_model.rx_Voltages_Array_Buff ):
                self.main_Wind_Ui.volt_time_Tabel.setItem(indvol,1,gm.QtWidgets.QTableWidgetItem(str(vol_valu)))    
                
                #__ self.main_Wind_Ui.volt_time_Tabel.setItem(ind,0,gm.QtWidgets.QTableWidgetItem(str(self.app_serPrt_model.rx_Times_Array_Buff[ind])))
                #__ self.main_Wind_Ui.volt_time_Tabel.setItem(ind,1,gm.QtWidgets.QTableWidgetItem(str(vol_valu)))   
              
                #@updat the progess bar value
                #-self.Prog_Bar_Thread_2.count = ind
                #--self.main_Wind_Ui.data_rxed_prgBar.setValue(ind+1)
                #----prg_bar_thrd.cur_val = ind+1
            
           # except IndexError:
           #     self.main_Wind.statusBar().showMessage("Index Error"+(str(self.app_serPrt_model.Get_Rxed_Bytes_Count)))
                
        #@ after data received finished re-enabel the Get received data button
        #if disab_btn is not None:
        #     disab_btn.setEnabled(True)
        #
        #@ after data received finished 
        #self.receved_Bytes_count = 1   
        
        #-ic("Serial thread running status",self.pyqt_ser_thread.isRunning()) 
        #----prg_bar_thrd.stop()
        
       
             
    def onReceiveData(self,evt_btn):
        raise NotImplementedError("This method is not implemented yet")
        # if self.main_Wind.progress_Bar_Thread.isRunning():
        #    self.main_Wind.progreeBar_Woker.stop()
        # else:
        #    self.main_Wind_Ui.read_Rx_Data_Btn.setText('Stop')
        #    self.main_Wind.progress_Bar_Thread.start()
        #    self.main_Wind.progress_Bar_Thread.started.connect(self.main_Wind.progreeBar_Woker.run)

       
       #ic(type(evt_btn))
       #ic (self.rx_Thread.is_alive())
        #-if self.app_serPrt_model.Get_Rxed_Bytes_Count:
        #-    self.rx_Thread.start()#self.Get_Received_Data(evt_btn)    
        #-    #self.rx_Thread.join()
            
    def onClearListCtrl(self,evt_btn):
         #@ clear the listbox control befor adding the new data
            self.main_Wind_Ui.ser_ports_LstVu.clear()
               
    def onSaveData(self,evt_btn):
        """_summary_
            Event handler for Save received data button
        Args:
            evt_btn (_type_: CommandEvent): _description_: event argument
        """
        # filename, _ = gm.QtWidgets.QFileDialog.getSaveFileName(self, "Save Page As", 
                                                            #    filter="CSV File (*.csv)|All files (*.*)")
        # if filename:
            # writer = csv.writer(filename)
            # writer.writerow(["Time","Voltage"])
            # writer.writerows([self.app_serPrt_model.rx_Times_Array_Buff,self.app_serPrt_model.rx_Voltages_Array_Buff])
            # #for ind,vol_valu in enumerate( self.app_serPrt_model.rx_Voltages_Array_Buff ):
            # #    writer.writerow([self.app_serPrt_model.rx_Times_Array_Buff[ind],vol_valu])
            # ic("Data saved")
            # gm.QtWidgets.QMessageBox(parent=self.main_Wind,text="Data saved").show()

        with open("Volt_Time_response.csv","w") as file:
            writer = csv.writer(file)
            writer.writerow(["Time","Voltage"])
            for ind,vol_valu in enumerate( self.app_serPrt_model.rx_Voltages_Array_Buff ):
                writer.writerow([self.app_serPrt_model.rx_Times_Array_Buff[ind],vol_valu])
                
        #-with open("Volt_Time_difference.csv","w") as file:
        #-    writer = csv.writer(file)
        #-    writer.writerow(["Time difference","Voltage difference"])
        #-    for ind,vol_diff_valu in enumerate( voltage_difference_list ):
        #-        writer.writerow([time_differnce_lst[ind],vol_diff_valu])
                
        ic("Data saved")
        gm.QtWidgets.QMessageBox(parent=self.main_Wind,text="Data saved").show()

    def onhandleFinished(self):
        ic("Thread finished")
        self.thread_finished = True
        self.thread_started = False
        self.total_bytes_received = 0
        self.receved_Bytes_count = 1
        self.main_Wind_Ui.rx_Timer.start(RX_PERIOD)
        #self.rx_Thread.start()#self.Get_Received_Data(evt_btn)    
        #self.rx_Thread.join()    

    def onDisplayCurve(self,evt_btn):
        """_summary_
            Event handler for Display Curve button
        """
        #ic("Serial thread running status",self.pyqt_ser_thread.isRunning()) 
        self.main_Wind.Setup_Matplot_Widget()
        #-self.onDisplayDifferCurve()
        #ic(self.main_Wind.mpl_widget.mpl_widget.canva_Cursor)
        #ic(self.main_Wind.mpl_widget.mpl_widget.inaxes((3.0,4.0)))
        #ic(self.main_Wind.mpl_widget.mpl_widget.mouseEventCoords())
        #ic(self.main_Wind.mpl_widget.mpl_widget.xData)
    
    def onDisplayDifferCurve(self):
        self.mpl_diff_widget:Matplot_Window = Matplot_Window(self.main_Wind,time_differnce_lst,voltage_difference_list)#MplCanvas(self, width=5, height=4, dpi=100)
        self.mpl_diff_widget.mpl_widget.mpl_Axes.plot(time_differnce_lst,voltage_difference_list)      
        self.mpl_diff_widget.mpl_widget.mpl_Axes.set_xlabel("Time Difference")
        self.mpl_diff_widget.mpl_widget.mpl_Axes.set_ylabel("Voltage Difference")
        self.mpl_diff_widget.mpl_widget.mpl_Axes.set_title("Capcitor Charging Difference Curve")
        self.mpl_diff_widget.mpl_widget.mpl_Axes.grid()
        self.mpl_diff_widget.mpl_widget.Setup_Cursor() 
        
        
    def onCalculate_Rx_diffeerence(self):
        #@ Get the time \ volt difference we used - 2 because we subtract the next value from the previouse value to get the difference
        ind_limit = self.app_serPrt_model.rx_Voltages_Array_Buff.size-2
        ind_nx =0 
        for i,v in enumerate(self.app_serPrt_model.rx_Times_Array_Buff):
            if i <=ind_limit :
                ind_nx = i+1
                time_differnce_lst.append( round(self.app_serPrt_model.rx_Times_Array_Buff[ind_nx]-v,4))
                voltage_difference_list.append(round(self.app_serPrt_model.rx_Voltages_Array_Buff[ind_nx]- self.app_serPrt_model.rx_Voltages_Array_Buff[i],4))
        nr= 0
        for i,td in enumerate(time_differnce_lst):
            if i <=ind_limit :
                self.Add_Tabel_Item(str(td),(nr,2),self.main_Wind_Ui.volt_time_Tabel)
                self.Add_Tabel_Item(str(voltage_difference_list[i]),(nr,3),self.main_Wind_Ui.volt_time_Tabel)
                nr+=2
            ic(nr)         
            #ic(time_differnce_lst)
            #ic(voltage_difference_list)
    
    def Add_Tabel_Item(self,item_str:str,pos: tuple,tabel:gm.QtWidgets.QTableWidget):
         ##@ Add row data to the tabel control
        tabel.setItem(pos[0],pos[1],gm.QtWidgets.QTableWidgetItem(item_str))
        
        """
        import autograd.numpy as np
from autograd import grad

def fct(x):
    y = x**2+1
    return y

grad_fct = grad(fct)
print(grad_fct(1.0))
        """