import  threading as thrd
import wx #from wx import ThreadEvent as thevnt
from icecream import ic
import  App_Serial_Port.serial_model as sm
#ser_thrd_event = wx.ThreadEvent(wx.EVT_THREAD)
class Serial_Comm_Thread(thrd.Thread):
    def __init__(self,ser_prt:sm.Serial_Model,fn_to_run,group = None, target = None, name = None, args = ..., kwargs = None, *, daemon = None):
        super().__init__(group, target, name, args, kwargs, daemon=daemon)
        self.thrd_ser_prt = ser_prt
        self.Run_this_fn = fn_to_run
        #-self.thraed_timer = wx.Timer()
        #-self.thraed_timer.SetOwner(self)
        #-thrd_ctl=wx.Control(self)
        #thrd_ctl.Bind(wx.EVT_THREAD,self.onThread,self)
        #-thrd_ctl.Bind(wx.EVT_TIMER,self.onTimer)
        #-self.thraed_timer.Start(1000)
    
    def run(self):
        #self.total_bytes_received += self.thrd_ser_prt.Get_Rxed_Bytes_Count
        #self.main_wnd_ref.m_window_status_bar.SetStatusText((str(self.total_bytes_received)),0)
        
        if self.thrd_ser_prt.Get_All_Data() is not None:
            self.Run_this_fn()
            #-self.thread_started = True
            #-self.thread_finished = False
            #-for ind,vol_valu in enumerate( self.thrd_ser_prt.rx_Voltages_Array_Buff ):
            #-   
            #-    try:
            #-        #self.main_wnd_ref.Pan_2_Mw.rx_tx_Data_TxtCtrl.AppendText(f"Volt = {vol_valu:.4}        Time = {self.thrd_ser_prt.rx_Times_Array_Buff[ind]:.4}\n")
            #-        #@ add a row data to the list contrl and return the index of the current row 
            #-        index =self.main_wnd_ref.Pan_2_Mw.data_lstBx.InsertItem(ind,str(vol_valu))
            #-        #@ Add columns data to the list contro
            #-        self.main_wnd_ref.Pan_2_Mw.data_lstBx.SetItem(ind,1,str(self.thrd_ser_prt.rx_Times_Array_Buff[ind]))
            #-        self.main_wnd_ref.m_window_status_bar.SetStatusText(f"(Received count Time/ Volt : {self.thrd_ser_prt.rx_Times_Array_Buff.size} / {self.thrd_ser_prt.rx_Voltages_Array_Buff.size}",1)
            #-        self.main_wnd_ref.m_window_status_bar.SetStatusText(f"Items count : {self.main_wnd_ref.Pan_2_Mw.data_lstBx.GetItemCount()}",2)
            #-        self.main_wnd_ref.Pan_2_Mw.rx_tx_Data_TxtCtrl.AppendText(f"{self.thrd_ser_prt.Get_Rxed_Bytes_Count}")
            #-    except IndexError:
            #-        #self.thread_Is_Alive = False
            #-        ic(f"error in indwx", ind)
            #-        self.main_wnd_ref.m_window_status_bar.SetStatusText((str(self.thrd_ser_prt.Get_Rxed_Bytes_Count)),0)
            
            #@ after data received finished re-enabel the Get received data button
            
        #self.c+=1
        #ic(str(self.c))
        #ic(str(self.thrd_ser_prt.Get_Rxed_Bytes_Count)) 
       
        #if self.thrd_ser_prt.Get_Rxed_Bytes_Count == 0:
        #    self.all_data_received = True
        #    self.thread_finished = True
        #    self.thread_started = False
        #    self.total_bytes_received = 0
        #    
        #    ic("Thread finished")
        #     
            #self.thread_Is_Alive = False
       #else:
       #    self.Get_Received_Data()    
        return super().run()
    #def onThread(self,thrd_evt):
    #    ic(thrd_evt)
        
    def onTimer(self,tim_evt):
        ic(tim_evt)