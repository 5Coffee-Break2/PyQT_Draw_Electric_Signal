import wx

#  win.Connect(-1, -1, EVT_RESULT_ID, func)
#Data_Transmitted_Event_ID = wx.NewId()
Data_Rxed_Txed_Event_ID = wx.NewIdRef()
EVT_Data_Received_Event_TYPE = wx.NewEventType()

Data_RxED_ERROR_EVENT_ID= wx.NewIdRef()
EVT_Data_RxED_ERROR_EVTYPE = wx.NewEventType()

def Data_Rxed_Txed_Event_Type_Binder(ev_typ,win:wx.Control,func):
    win.Connect(-1, -1, ev_typ, func)

class Data_Received_Event(wx.PyEvent):
    def __init__(self,ev_memb):  # (self, id=0, eventType=...):
        super().__init__()  # (id, eventType)
        # Set the event type from its ID remember that each event can have multtiple event types
        self.EVT_Data_RxED_EVType = self.SetEventType(EVT_Data_Received_Event_TYPE)
        self.Data_Rxed_Txed_Event_Type=self.SetEventType(EVT_Data_RxED_ERROR_EVTYPE)
        self.SetId(Data_Rxed_Txed_Event_ID)
        self.event_Members = ev_memb
        # self.by_Rxed_Num_type = None
        self.bytes_Rxed_Count = 0
        self.error_Event_Msg = ""

    # def Set_by_Rxed_Num_type(self,status_f:bool)->None:
    #    self.by_Rxed_Num_type = status_f
    #
    # def Get_by_Rxed_Num_type(self) -> bool:
    #    return self.by_Rxed_Num_type
    #
    
    def Set_Data_Rxed_Txed_Event_Type(self,evt_type):
        self.Data_Rxed_Txed_Event_Type = evt_type
    
    def Get_Data_Rxed_Txed_Event_Type(self):
        return self.Data_Rxed_Txed_Event_Type
    
    def Register_4Serial_Event(self,evtype,evHnd):
        Data_Rxed_Txed_Event_Type_Binder(evtype,self.event_Members,evHnd)#(self.event_Members,evHnd,evtype)
    
    def Fire_SCom_Event(self,dest,evt_data):#(self,dest,obstatus,obprime):
        #peven = Prime_EVENT(wx.NewId(),IsPrime_EVENT_Type)
        self.Set_error_Event_Msg(evt_data)
        wx.PostEvent(self.event_Members,self)
    
    def Set_by_Rxed_Count(self, cnt_v: int) -> None:
        """_summary_
            Set the Total bytes or characters received
        Args:
            cnt_v (int): _description_ : is the total bytes or characters counts received in the Rx buffer
        """
        self.bytes_Rxed_Count = cnt_v

    def Get_by_Rxed_Count(self) -> int:
        """_summary_ : Get the Total bytes or characters received

        Returns:
            int: _description_ : the total bytes or characters counts received in the Rx buffer
        """
        return self.bytes_Rxed_Count

    def Set_error_Event_Msg(self, msg_e: str) -> None:
        """_summary_
            Set the the error message that the event could send
        Args:
            
        """
        self.error_Event_Msg = msg_e

    def Get_error_Event_Msg(self) -> str:
        """_summary_ : Get the error message that could be sent by the event

        Returns:
            
        """
        return self.error_Event_Msg