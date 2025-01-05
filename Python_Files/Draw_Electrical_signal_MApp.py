from main_window import Ui_MainWindow
from PyQt6.QtWidgets import QMainWindow, QApplication, QProgressBar,QLabel
from PyQt6.QtCore import Qt,QTimer, QThread
import main_window_handlers.main_window_handlers_Ctrl as mwh
#from Progress_Bar_Thread.rx_progress_bar import progreeBar_Woker
from Progress_Bar_Thread.rx_progress_bar import CProgess_Bar_Thread

#from Matplot_Files.pyqt_matplot_canva import MplCanvas
from Matplot_Files.Matplot_Window import Matplot_Window
from main_window_extra_widgets import MainWindowExtraWidgets
import sys
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.ma_Window = Ui_MainWindow()
        self.ma_Window.setupUi(self)
        self.mW_Extra_Widgets = MainWindowExtraWidgets(self)
        self.ma_Window_Handlers= mwh.Main_Wind_Handlers(self)
        self.rx_Timer= QTimer()
        self.rx_Timer.setInterval(500)
        self.ma_Window.ser_Port_Info_LabCrl.setStyleSheet("font-size: 13px;color: #000;")
        self.Setup_StatusBar()
        self.statusBar().showMessage("Ready")
        #@ Serial communication thread
       
        #@ progress bar thread
        self.PBar_val = 0
        
        css = """
    QMainWindow {
        background-color:rgba(34, 34, 200, 0.63);
    }
    QLabel {
        font-size: 14px;
        color: rgba(255, 180, 239, 0.86);;
        background-color:rgba(133, 87, 219, 0.73);
        
    }
    QPushButton {
        background-color:rgba(198, 104, 41, 0.87);
        border-radius: 5px;
        border: 1px solid #000;
    }
    QPushButton:hover {
        background-color:  #f0f;
    }

    QListWidget {
        background-color:rgba(55, 80, 239, 0.46);
        border-radius: 5px;
        border: 1px solid #000;
    }
    
    QListWidget::item {
        
        border-radius: 2px;
        border: 1px solid #000;
        selection-color: #ff0000;
        background-color:rgba(133, 87, 219, 0.73);
    }
    QListWidget::item::selected {
        
        border-radius: 2px;
        border: 1px solid #ff0000;
        selection-color: #00fb00;
        selection-background-color: #00ff00;
        
    }
    QListWidget::item:hover {  
        background-color:rgba(55, 80, 239, 0.46);
        border-radius: 5px;
        border: 1px solid #000;
    }
    QTableWidget {  
        background-color:rgba(55, 80, 239, 0.46);
        border-radius: 5px;
        border: 1px solid #000;
    }
    """
        self.setStyleSheet(css)
        self.ma_Window.volt_time_Tabel.setColumnCount(2)
        self.ma_Window.volt_time_Tabel.setHorizontalHeaderLabels(["Time","Voltage"])
        
        self.Events_Handlers()
    
    def onProgressBar(self,pval):
        #self.ma_Window.data_rxed_prgBar.setValue(pval)
        self.statusBar_L2.setText(f"Progress: {pval}") 
        print(f"Progress: {pval}")
        assert isinstance(pval, int)
    
    def Setup_Matplot_Widget(self):
        """
        Sets the Matplotlib widget in the main window.

        This method creates a Matplotlib widget and sets it as the central widget
        of the main window.
        """
        self.mpl_widget = Matplot_Window(self)#MplCanvas(self, width=5, height=4, dpi=100)
        #self.layout().addWidget(self.mpl_widget)
        #self.ma_Window.volt_time_Tabel.setCellWidget(0,0,self.mpl_widget)
        
        self.mpl_widget.mpl_widget.axes.plot(self.ma_Window_Handlers.app_serPrt_model.rx_Times_Array_Buff,self.ma_Window_Handlers.app_serPrt_model.rx_Voltages_Array_Buff)      
        self.mpl_widget.mpl_widget.axes.set_xlabel("Time")
        self.mpl_widget.mpl_widget.axes.set_ylabel("Voltage")
        self.mpl_widget.mpl_widget.axes.set_title("Capcitor Charging Curve")
        self.mpl_widget.mpl_widget.axes.grid() 
        #self.layout().addWidget(self.mpl_widget)
        
        #self.ma_Window. volt_time_Tabel.setCellWidget(0,0,self.mpl_widget)
        #self.setCentralWidget(self.mpl_widget)
        #mpw=Matplot_Window(self.mpl_widget)
        #mpw.show()
        
    def Setup_StatusBar(self):
        """
        Sets the status bar of the main window.

        This method sets the status bar of the main window to display the message
        "Ready" when the application is launched.
        """
        self.statusBar_L1=QLabel(self)   
        self.statusBar_L2=QLabel(self)
        self.statusBar_L3=QLabel(self)
         
        self.statusBar().addPermanentWidget(self.statusBar_L1,stretch=1)
        self.statusBar().addPermanentWidget(self.statusBar_L2,1)
        self.statusBar().addPermanentWidget(self.statusBar_L3,1)
        #self.statusBar().showMessage("Ready")    
    
    def Events_Handlers(self):
        self.ma_Window.get_Sys_portsBtn.clicked.connect(self.ma_Window_Handlers.onlist_Sys_Ports_btn)  
        self.ma_Window.ser_ports_LstVu.doubleClicked.connect(self.ma_Window_Handlers.onSelectPort)
        self.ma_Window.select_Port_Btn.clicked.connect(self.ma_Window_Handlers.onSelectPort)
        self.ma_Window.read_Rx_Data_Btn.clicked.connect(self.ma_Window_Handlers.onReceiveData)
        self.rx_Timer.timeout.connect(self.ma_Window_Handlers.onrxTimer)
        self.ma_Window.save_Rx_Data_Btn.clicked.connect(self.ma_Window_Handlers.onSaveData)
        self.ma_Window.dispaly_Rx_Data_Btn.clicked.connect((self.ma_Window_Handlers.onDisplayCurve))
        self.ma_Window.data_rxed_prgBar.valueChanged.connect(self.onProgressBar)
    
    def oNhandleFinished(self):
        self.ma_Window.read_Rx_Data_Btn.setText('Start')
        self.progress_Bar_Thread.quit()
    
    def Widget_Styles(self):
        """
        Sets the alignment and style for labels in the main window.

        This method aligns the text of `label` and `label_2` to the center and 
        applies a bold font weight and black color to their text.
        """
        raise NotImplementedError("This method has not been implemented yet.")
        #self.ma_Window.label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        #self.ma_Window.label_2.setAlignment(Qt.AlignmentFlag.AlignCenter)
        #self.ma_Window.label.styleSheet = "font-weigt:bold ;color: #000;"
        #self.ma_Window.label_2.styleSheet = "font-weigt:bold ;color: #000;"
    def onCloseGraphBtn(self,evt):
         
         self.mpl_widget .destroy(True)
         self.mpl_widget .close()   
         
if __name__ == "__main__":
    mApp = QApplication([])
    MainWindow = MainWindow()
    MainWindow.show()
    sys.exit(mApp.exec())