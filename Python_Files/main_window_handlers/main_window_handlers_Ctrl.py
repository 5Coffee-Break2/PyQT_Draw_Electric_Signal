import App_Serial_Port.serial_model as sm
import global_modules as gm
from PyQt6.QtWidgets import QListWidgetItem, QMainWindow
from Draw_Electrical_signal_MApp import MainWindow,Ui_MainWindow
import csv
from icecream import ic
RX_PERIOD = 500
class Main_Wind_Handlers:
    
    def __init__(self,client:MainWindow):
        
        self.main_Wind =client
        self.main_Wind_Ui = client.ma_Window
        self.app_serPrt_model = sm.Serial_Model(self.main_Wind_Ui,self.onRx_TxSerial)
        self.ser_port_name = None
        self.rx_Thread = gm.thrd.Thread(target=self.Get_Received_Data)
        self.thread_Is_Alive:bool=False
        self.all_data_received:bool = False
        self.thread_started:bool = False
        self.thread_finished:bool = False
        self.total_bytes_received:int=0
        self.sc_thrd = gm.sthd.Serial_Comm_Thread(self.app_serPrt_model,self.Display_Received_Data)
        #self.sc_thrd.start()
        #self.c:int=0
        self.receved_numbers_count:int = 1
         
    def inside_thread(self):
        self.c+=1
        ic("From inside the serial thread",self.c)
       
    def onrxTimer(self):
        """_summary_
        This function to check periodically(time period = RX_CHECK_EVERY) if there's a received data in the rx serial buffeer
        this version of the function will executed only if there was a received data in the buffer and no other data will be received
            tim_evt (_type_): _description_
        """
        ic("Evwnt timer started")
        #@ if there's a received data in the buffer
        if self.app_serPrt_model.Get_Rxed_Bytes_Count: 
            #@ if received data in the buffer is completed and no other data will be received
            if self.receved_numbers_count == self.app_serPrt_model.Get_Rxed_Bytes_Count  :
                try:
                    #@ if thread didn't start yet
                    if not self.rx_Thread.is_alive():
                        #@ start the thread
                        self.rx_Thread.start()    #@ self.Get_Received_Data() #gm.mwm.Pan_1_Main_window.read_Data_btn

                    #if self.thread_started :
                    #    self.main_Wind_Ui.Pan_2_Mw.rx_tx_Data_TxtCtrl.AppendText("I'm still in the thread\n")
                except:
                    
                    self.rx_Thread = gm.thrd.Thread(target=self.Get_Received_Data)
                    self.rx_Thread.start()
                #-self.main_Wind_Ui.rx_Timer.Start(gm.mwm.RX_CHECK_EVERY)
                
            #@ if receiving data is in progess and not finished yet
            else:
                #@ update the received data count with the last data size that received
                self.receved_numbers_count = self.app_serPrt_model.Get_Rxed_Bytes_Count
            ic("Evwnt timer ended")   
        #@ if all data was read and there's no received data in the buffer        
        #if self.thread_finished == True and self.receved_numbers_count == 1:
                #@for debug only
                           
    
    def onlist_Sys_Ports_btn(self,btn_evnt):
        """_summary_
            Event Handler for Get Ports list button
        Args:
            btn_evnt (_type_: CommandEvent): event argument
        """
        self.app_serPrt_model.Get_System_Comm_Ports()
        ic(type(btn_evnt))
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
                ic(type(lst_ctrl_evt))
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
            ic(type(btn_evt),btn_evt.GetEventObject())
            if self.app_serPrt_model.Is_Active_Port_Opend():
                self.App_Serial_Port= self.app_serPrt_model.Get_Active_Port()
                self.main_Wind_Ui.ser_Port_Info_LabCrl.setText(self.app_serPrt_model.Get_Active_Port_info())
                self.main_Wind.rx_Timer.start()
       
       
    
    def Get_Received_Data(self, disab_btn: gm.QtWidgets.QPushButton = None):
        #region
        """
        Handles the reception of data and updates the UI accordingly.
        This method performs the following tasks:
        1. Temporarily disables the event timer.
        2. Updates the total bytes received.
        3. Clears the contents of the voltage-time table.
        4. Starts a worker thread to process the received data.
        5. Iterates through the received voltage data and updates the table.
        6. Updates the status bar with the current received data information.
        7. Re-enables the "Get data" button after processing is complete.
        8. Resets the received data count if all data has been read.
        Args:
            disab_btn (gm.QtWidgets.QPushButton, optional): The button to disable during data reception. Defaults to None.
        """
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
            
            self.main_Wind.worker.total_val = self.app_serPrt_model.rx_Voltages_Array_Buff.size
            self.main_Wind.statusBar_L3.setText(f"Total inc : {self.main_Wind.worker.total_val /100}")
            self.main_Wind.thread.start()
            self.main_Wind.thread.started.connect(self.main_Wind.worker.run)
            
            #@Start processing the received data here
            for ind,vol_valu in enumerate( self.app_serPrt_model.rx_Voltages_Array_Buff ):
                self.main_Wind.worker.count = ind
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
                    self.main_Wind.statusBar_L1.setText(f"Items count : {self.main_Wind_Ui.volt_time_Tabel.rowCount()}")
                    #self.main_Wind_Ui.m_window_status_bar.SetStatusText(f"Items count : {self.main_Wind_Ui.Pan_2_Mw.data_lstBx.GetItemCount()}",2)
                    if  ind ==  self.main_Wind.worker.total_val -1:
                        self.main_Wind.worker.stop()
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
            self.receved_numbers_count = 1
            #self.main_Wind.Setup_Matplot_Widget()
            
       #else:
       #    self.Get_Received_Data()
          
    def Display_Received_Data(self):
        ic(type(self.Display_Received_Data))
        for ind,vol_valu in enumerate( self.app_serPrt_model.rx_Voltages_Array_Buff ):
               #@ dissable the Get data button
               #if disab_btn is not None:
               #    disab_btn.Disable()
               try:
                   
                   #@ add a row data to the list contrl and return the index of the current row 
                   index =self.main_Wind_Ui.Pan_2_Mw.data_lstBx.InsertItem(ind,str(vol_valu))
                   #@ Add columns data to the list contro
                   self.main_Wind_Ui.Pan_2_Mw.data_lstBx.SetItem(ind,1,str(self.app_serPrt_model.rx_Times_Array_Buff[ind]))
                   self.main_Wind_Ui.m_window_status_bar.SetStatusText(f"(Received count Time/ Volt : {self.app_serPrt_model.rx_Times_Array_Buff.size} / {self.app_serPrt_model.rx_Voltages_Array_Buff.size}",1)
                   self.main_Wind_Ui.m_window_status_bar.SetStatusText(f"Items count : {self.main_Wind_Ui.Pan_2_Mw.data_lstBx.GetItemCount()}",2)
                   self.main_Wind_Ui.Pan_2_Mw.rx_tx_Data_TxtCtrl.AppendText(f"{self.app_serPrt_model.Get_Rxed_Bytes_Count}")
               except IndexError:
                   
                   ic(f"error in indwx", ind)
                   self.main_Wind_Ui.m_window_status_bar.SetStatusText((str(self.app_serPrt_model.Get_Rxed_Bytes_Count)),0)
           
           #@ after data received finished re-enabel the Get received data button
           
       
        if self.app_serPrt_model.Get_Rxed_Bytes_Count == 0:
            self.all_data_received = True
            self.thread_finished = True
            self.thread_started = False
            self.total_bytes_received = 0
            
             
    def onReceiveData(self,evt_btn):
        raise NotImplementedError("This method is not implemented yet")
        # if self.main_Wind.thread.isRunning():
        #    self.main_Wind.worker.stop()
        # else:
        #    self.main_Wind_Ui.read_Rx_Data_Btn.setText('Stop')
        #    self.main_Wind.thread.start()
        #    self.main_Wind.thread.started.connect(self.main_Wind.worker.run)

       
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
                                                              
                                                                 
                
        
        with open("Voly_Time_response.csv","w") as file:
            writer = csv.writer(file)
            writer.writerow(["Time","Voltage"])
            for ind,vol_valu in enumerate( self.app_serPrt_model.rx_Voltages_Array_Buff ):
                writer.writerow([self.app_serPrt_model.rx_Times_Array_Buff[ind],vol_valu])
        ic("Data saved")
        gm.QtWidgets.QMessageBox(parent=self.main_Wind,text="Data saved").show()
        
        
    def onhandleFinished(self):
        ic("Thread finished")
        self.thread_finished = True
        self.thread_started = False
        self.total_bytes_received = 0
        self.receved_numbers_count = 1
        self.main_Wind_Ui.rx_Timer.start(RX_PERIOD)
        #self.rx_Thread.start()#self.Get_Received_Data(evt_btn)    
        #self.rx_Thread.join()    
        
        
        
    def onDisplayCurve(self,evt_btn):
        """_summary_
            Event handler for Display Curve button
        """
        self.main_Wind.Setup_Matplot_Widget()
           
        
        
        
        
        
             