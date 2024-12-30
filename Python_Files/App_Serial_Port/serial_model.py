import Py_AVR_USART_Lib_Wind as pyAvrSer
#import threading as scthrd
#from time import sleep
from wx import MessageBox,CallAfter,Frame       
import numpy as np
#import data_figure_mod as dfm                

     
clear_Rx_Tx_buffer_flag = False
draw_Me = False 
def fakeFn():
    pass 
class Serial_Model:
    #@ VCC value ADC converting factor and equal to the real value of the voltage source of the microcontroller
    VCC_VALUE:float = 5.0 #volt
    def __init__(self,evnt_memb,evt_hndl) -> None:
        #@ define the main serial object
        self.__active_Serial_Port__:pyAvrSer.Atmega_USART
        self.rx_Times_Buffer:list[float] = []
        self.rx_Voltages_Buffer:list[float] =[]
        #-self.rx_Voltages_Array_Buff = np.array(self.rx_Voltages_Buffer,float)
        #-self.rx_Times_Array_Buff=np.array(self.rx_Times_Buffer,dtype=float)
        # self.rxed_Data_Type = {  
        #                                             "float_Type":pyAvrSer.Atmega_USART.SOB_F_By,
        #                                             "int_Type":pyAvrSer.Atmega_USART.SOB_I_By,
        #                                             "str_Type":pyAvrSer.Atmega_USART.SOB_S_By
        #                                         }
        self.total_Rxed_Data =0
        self.total_Rxed_Float =0
        self.total_Rxed_Int =0
        self.total_Rxed_Msg =0
        #____________________________________ Error Events Section ________________________________#
        
        self.Eror_Evt_Member = evnt_memb
        self.Eror_Event_Handler = evt_hndl
       
       
        
        #_____________________________ End of Error Events Section ________________________________#
        
    @property
    def Get_Rxed_Bytes_Count (self) -> int:
        """
        Purpose: Return the total number of the received bytes i n the serial rx buffer
        """
        return self.__active_Serial_Port__.Get_Rx_Waiting_count
    # end def
   
    def Activate_Serial_Port(self,port_num,baud_rate):
        self.__active_Serial_Port__ = pyAvrSer.Atmega_USART(baud_rate= baud_rate,port_num=port_num)
    
    def Close_Port(self):
        if self.__active_Serial_Port__ is not None :
            self.__active_Serial_Port__.Close_Main_Port()
    
    def Is_Active_Port_Opend(self):
        return self.__active_Serial_Port__.Get_Main_Port_Status
    
    def Get_Active_Port(self):
        return  self.__active_Serial_Port__.Get_Main_Port
    
    def Get_A_Rxed_Number(self) :               #-> None | tuple[bytes | Literal[True], float] | tuple[bytes | Literal[True], int] | Literal[False]:
        """
        Purpose: Return a single received number
        """
        #rt,rn =None,None
        try:
            rv = self.__active_Serial_Port__.Receive_Single_Number()# rt,rn  = self.__active_Serial_Port__.Receive_Single_Number():
            if rv is None:
                return
            rt,rn = rv
        
        except TypeError as e :
            #MessageBox(f"{e.__doc__}: {e.args[0]} -> {self.__active_Serial_Port__.Get_Rx_Waiting_count})")
            #@ Fire Data RXed with Error Event 
        
           #- print(f"{e.__doc__}: {e.args[0]} -> {self.__active_Serial_Port__.Get_Rx_Waiting_count})")
            return
            
        #if rt is pyAvrSer.Atmega_USART.EM_By  or rn is pyAvrSer.Atmega_USART.EM_By or rn == pyAvrSer.pa_en_de.ENCODE_DECODE_ERROR:     #rt != self.rxed_Data_Type["int_Type"] | rt != self.rxed_Data_Type["float_Type"]:
        #    return
        
                #!replace list elements Syntax: l=list(map(lambda x: x.replace(‘old_value’,’new_value’),l))
        if isinstance(rn,int):
                self.rx_Voltages_Buffer.append(rn* self.VCC_VALUE /1024)
                return rt,rn
        elif isinstance(rn,float):
                self.rx_Times_Buffer.append(rn)
                return rt,rn
        return
    
    
    def Send_Number(self,ntype,n_val):
        if ntype == pyAvrSer.Atmega_USART.SOB_I_By:
            self.__active_Serial_Port__.Send_Int(n_val)
    
    def Refresh_Active_Port(self):
        """Refresh and reset the active opened port"""
        self.__active_Serial_Port__.Refresh_Port()
        
    def Get_System_Comm_Ports(self)->list[str]:
        """_summary_ Get an Active Ports list in the operating system
        
        Returns:
            _type_:  a list of type string represents the total active ports in the system
        """
        return [str(s_port) for s_port in pyAvrSer.serial.tools.list_ports.comports()]
    
    def Get_Active_Port_info(self):
        """
        Return the active port information
        """
        return self.__active_Serial_Port__.Port_Info
    
    def Reset_RX_TX_Buffer(self,rx_b:bool = False,tx_b:bool = False)->None:
        """_summary_ : Reset the RX/Tx serial buffer

        Args:
            rx_b (bool, optional): a flag to reset the active serial port rx buffer ( value must be true). Defaults to False.
            tx_b (bool, optional): a flag to reset the active serial port tx buffer (value must be true). Defaults to False.
        """
        if rx_b:
            self.__active_Serial_Port__.Get_Main_Port.reset_input_buffer()
        if tx_b :
            self.__active_Serial_Port__.Get_Main_Port.reset_output_buffer()
    
    def Reset_RX_TX_Data_Buffer(self,int_b:bool = False,float_b:bool = False)->None:
        """_summary_ : Reset the RX/Tx serial buffer

        Args:
            rx_b (bool, optional): a flag to reset the active serial port rx buffer ( value must be true). Defaults to False.
            tx_b (bool, optional): a flag to reset the active serial port tx buffer (value must be true). Defaults to False.
        """
        if int_b:
             self.rx_Voltages_Buffer = [] #self.rx_Voltages_Buffer.clear()
             self.rx_Voltages_Array_Buff = np.array(self.rx_Voltages_Buffer)
        if float_b :
            self.rx_Times_Buffer = [] #self.rx_Times_Buffer.clear()
            self.rx_Times_Array_Buff=np.array(self.rx_Times_Buffer)
    
    
    def Get_All_Data(self):
        if  self.Get_Rxed_Bytes_Count <= 0:
            return
        self.Reset_RX_TX_Data_Buffer(True,True)
            
        #ind=0
        while  self.Get_Rxed_Bytes_Count > 0:
            self.Get_A_Rxed_Number()
       
        self.rx_Times_Array_Buff=np.array(self.rx_Times_Buffer)
        self.rx_Voltages_Array_Buff = np.array(self.rx_Voltages_Buffer)
        fl = self.rx_Times_Array_Buff.size
        il = self.rx_Voltages_Array_Buff.size
        return fl,il
              #  self.m_curveFigure.Set_X_Y_axisesLimits((0, max_x),(0,max_y))    
                    
    #def Decode_Rxed_Data(self,float_lst:list[float]=[],int_lst:list[int]=[],str_lst: list[str]=[]):
    #    """_summary_
#
    #    _extended_summary_
#
    #    Args:
    #        float_lst (list[float], optional): a float list for storing rxed float numbers. Defaults to [].
    #        int_lst (list[int], optional): an int list for storing rxed int numbers. Defaults to [].
    #        str_lst (list[str], optional): a str list for storing rxed string messages. Defaults to [].
#
    #    Returns:
    #        _type_: _description_
    #    """
#
    #    bf_isempty = False
    #    #@ total received float numbers
    #    cnt_f:int = 0
    #    #@ total received int numbers
    #    cnt_i:int =0
    #    #@ total received string message
    #    cnt_msg:int = 0
    #    
    #    tf = 0.0
    #    ti = 0
    #    try:
    #        rtyp, rdata = self.Get_A_Rxed_Number()
    #        
    #        # rfn = self.active_Serial_Port.Receive_Single_Number()[2]
    #        #@ if no received bytes in the main Rx buffer
    #        if rdata == None:        #and error == False:
    #            MessageBox(message="No Data in the Rx Buffer OR Decoding error or number format error")
    #            return 
    #       
    #        if rtyp == self.rxed_Data_Type["int_Type"]:  # type(rfn) is Instance(int):
    #            cnt_i+=1
    #            sleep(0.2)
    #            int_lst.append(rdata)
    #            return  rtyp, rdata
    #        elif rtyp == self.rxed_Data_Type["float_Type"]:  # type(rfn) is Instance(float):
    #            cnt_f+=1
    #            sleep(0.2)
    #            float_lst.append(rdata)
    #            return  rtyp, rdata
    #        elif rtyp == self.rxed_Data_Type["str_Type"]:  # type(rfn) is Instance(int):
    #            cnt_msg += 1
    #            sleep(0.2)
    #            str_lst.append(rdata)
    #            return  rtyp, rdata
    #    except TypeError as e:
    #        # wx.RichMessageDialog(caption="attention",message=(f"{e}")).ShowModal()
    #        MessageBox(message=f"{e}")


#-from matplotlib.pyplot import close
#-from wx import CallAfter
#-class Serial_Comms_Thread(scthrd.Thread):
#-    def __init__(self,ser_obj:Serial_Model,daemon=True,**kwargs) -> None:
#-        """_summary_ : A thread for monitoring the Rx serial communication in the background
#-        Args:
#-            ser_obj (Serial_Model): the main serial object that will be monitored
#-            
#-            keyword args:
#-            cfig : Data_Figure
#-            cafn : (thread call after function) callable function
#-            
#-        """
#-        
#-        scthrd.Thread.__init__(self)
#-        
#-        #super().__init__(group, target, name, args, kwargs, daemon=daemon)
#-        if ser_obj == None:
#-            return
#-        self.m_Ser_Obj = ser_obj
#-        self.plot_Done:bool = False 
#-        self.new_Plot:bool = False
#-        self.get_New_Fig = False
#-        
#-        if "cafn" in kwargs.keys():
#-            self.calAf_fn = kwargs["cafn"]
#-        else:
#-             self.calAf_fn = None
#-        if "call_this" in  kwargs.keys():
#-            self.call_this = kwargs["call_this"]
#-        else:
#-            self.call_this = None
#-            
#-        self.ser_Thrd_Alive = True
#-        #for t_char_key, t_char_value in __Ascii_Encod_Decode_ATMEGA__.items():
#-        #  if tx_value == t_char_key:
#-        if "cfig" in kwargs.keys():
#-            self.fig_thrd = kwargs["cfig"]
#-        else:
#-            self.Creat_Thrd_Fig()
#-        #    self.fig_thrd = dfm.Data_Figure(None) 
#-        ##for k,v, in kwargs.items():
#-        ##   if k == "cfig":
#-        ##    self.fig_thrd = v
#-        ##   else:
#-        ##    self.fig_thrd = dfm.Data_Figure(None) 
#-        #
#-        #    self.fig_thrd.Set_X_Y_sources(self.m_Ser_Obj.rx_Times_Buffer,self.m_Ser_Obj.rx_Voltages_Buffer)
#-        self.reset_data =False
#-     
#-        
#-    def Creat_Thrd_Fig(self,ft="Thrid Figure",xt="td Time",yt="td Voltage"):
#-        self.fig_thrd = dfm.Data_Figure(None) 
#-        self.fig_thrd.Set_X_Y_sources(self.m_Ser_Obj.rx_Times_Buffer,self.m_Ser_Obj.rx_Voltages_Buffer)
#-        
#-    def run(self) -> None:
#-        #global clear_Rx_Tx_buffer_flag, draw_Me
#-        #tmp_int_buff:list[int]= []
#-        til:int = 0
#-        vol:int = 0
#-        while self.ser_Thrd_Alive:
#-#             self.fig_thrd.Clear_Axes()
#-            try:
#-                rx_counts =  0
#-                if self.fig_thrd.im_Closed:
#-                       #close(self.fig_thrd.Get_Fig)
#-                       #self.fig_thrd.Close_All()
#-                       #self.fig_thrd.Init_Figure()#self.Creat_Thrd_Fig() 
#-                       
#-                       print("from thread if self.fig_thrd.im_Closed")
#-                       
#-                if self.m_Ser_Obj.Get_Rxed_Bytes_Count > 0:
#-                    self.new_Plot = True
#-                    self.plot_Done = False
#-                    #self.Creat_Thrd_Fig()
#-                    #!if self.get_New_Fig:
#-                    #!    self.Creat_Thrd_Fig()   #self.fig_thrd.Set_X_Y_sources(self.m_Ser_Obj.rx_Times_Buffer,self.m_Ser_Obj.rx_Voltages_Buffer)
#-                    #!    self.get_New_Fig = False
#-                    #til,vol= self.m_Ser_Obj.Get_All_Data()
#-                
#-                # if self.plot_Done == False and self.new_Plot:
#-                    # close(self.fig_thrd.Get_Fig)
#-                    # self.Creat_Thrd_Fig()#self.fig_thrd.Init_Figure()
#-                    # #elf.new_Plot = False
#-                    #@ if the rx buffer is not empty new numbers was received (Get all received numbers)
#-                    while self.m_Ser_Obj.Get_Rxed_Bytes_Count :#rx_counts > 0:
#-                           #rx_counts =self.m_Ser_Obj.Get_Rxed_Bytes_Count
#-                           self.m_Ser_Obj.Get_A_Rxed_Number()
#-                           #self.ser_Thrd_Alive = True
#-                           #rx_counts = rx_counts - self.m_Ser_Obj.Get_Rxed_Bytes_Count
#-                           #tmp_int_buff.append(rx_counts)
#-                           sleep(.002)
#-                           if self.call_this is not None:
#-                               self.call_this()
#-                           print (self.m_Ser_Obj.Get_Rxed_Bytes_Count,end="\r") # (self.m_Ser_Obj.Get_Rxed_Bytes_Count,'\t\t',"Diff = ",rx_counts,end="\r")
#-                    
#-                
#-                #@if all received numbers has been read
#-                if self.m_Ser_Obj.Get_Rxed_Bytes_Count == 0:
#-                    if self.new_Plot:
#-                        #print("Time :",self.m_Ser_Obj.rx_Times_Buffer.__len__(),"Volt:",self.m_Ser_Obj.rx_Voltages_Buffer.__len__(),end="\r")
#-                        #print("Time :",til,"\t\t","Volt:",vol,end="\r")
#-                        self.plot_Done = True
#-                        self.new_Plot = False
#-                        self.get_New_Fig = True 
#-                        
#-                        #-self.fig_thrd.Set_X_Y_Limits_toMax()
#-                        self.fig_thrd.Draw_Data_Curve()
#-                        
#-                        #-self.m_Ser_Obj.Reset_RX_TX_Data_Buffer(True,True)
#-                        #-self.fig_thrd.Set_X_Y_sources(self.m_Ser_Obj.rx_Times_Buffer,self.m_Ser_Obj.rx_Voltages_Buffer)
#-                       
#-                   
#-                              
#-                       #    #print
#-                       # (f"Time Max,MIN    {self.m_Ser_Obj.rx_Times_Array_Buff.min()},{self.m_Ser_Obj.rx_Times_Array_Buff.max()}")
#-                       #    #print(f"Voltage MAX,MIN     {self.m_Ser_Obj.rx_Voltages_Array_Buff.min()},{self.m_Ser_Obj.rx_Voltages_Array_Buff.max()}")
#-                print("Im closed flag from thread= ",self.fig_thrd.im_Closed,end="\r")        
#-            
#-            except Exception as e:
#-                 print(e.__repr__(),e.args)
#-                 print(self.fig_thrd.Get_Fig.canvas.__repr__())
#-                 
#-                            
#-
#-           
#-            #sleep(0.4)
#-        return super().run()
#-    
    
     #_________________________old working run try ____________________#
            #rx_counts = self.m_Ser_Obj.Get_Rxed_Bytes_Count
            ##if clear_Rx_Tx_buffer_flag:
            ##     self.m_Ser_Obj.Reset_RX_TX_Buffer(rx_b=True)
            #
            #while rx_counts > 0:
            #    if self.m_Ser_Obj.Get_A_Rxed_Number() == None:
            #        self.draw_f = True
            #        draw_Me = True
            #        rx_counts -= 1
            #        print (self.m_Ser_Obj.Get_Rxed_Bytes_Count,'\t\t',rx_counts,end="\r")
            #        
            #        if self.draw_f:
            #    #    if self.reset_data:
            #    #        self.m_Ser_Obj.Reset_RX_TX_Buffer(True)
            #    #    self.m_Ser_Obj.Reset_RX_TX_Buffer(True)
            #    #    self.fig_thrd.Clear_Axes()
            #            self.fig_thrd.Draw_Data_Curve()
            #            self.draw_f =False
            #        
            #        continue
            #    self.draw_f = True
            #    draw_Me = True
            #    rx_counts -= 1
            #    print (self.m_Ser_Obj.Get_Rxed_Bytes_Count,'\t\t',rx_counts,end="\r")
            #    #print(f"Time Max,MIN    {self.m_Ser_Obj.rx_Times_Array_Buff.min()},{self.m_Ser_Obj.rx_Times_Array_Buff.max()}")
            #    #print(f"Voltage MAX,MIN     {self.m_Ser_Obj.rx_Voltages_Array_Buff.min()},{self.m_Ser_Obj.rx_Voltages_Array_Buff.max()}")
            #    #-sleep(0.2)
            ##if  rx_counts == 0 :
            ##        self.draw_f = False
            #        #self.m_Ser_Obj.Reset_RX_TX_Buffer(rx_b=True)
            #        #draw_Me = False
            #        
            #    if self.draw_f:
            #    #    if self.reset_data:
            #    #        self.m_Ser_Obj.Reset_RX_TX_Buffer(True)
            #    #    self.m_Ser_Obj.Reset_RX_TX_Buffer(True)
            #    #    self.fig_thrd.Clear_Axes()
            #        self.fig_thrd.Draw_Data_Curve()
            #        self.draw_f =False
            #    #    #CallAfter(self.calAf_fn)
            #    #    #self.draw_f = False