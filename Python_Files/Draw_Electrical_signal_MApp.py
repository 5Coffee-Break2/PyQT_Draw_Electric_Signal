from main_window import Ui_MainWindow
from PyQt6.QtWidgets import QMainWindow, QApplication
from PyQt6.QtCore import Qt,QTimer
import main_window_handlers.main_window_handlers_Ctrl as mwh
import sys
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.ma_Window = Ui_MainWindow()
        self.ma_Window.setupUi(self)
        self.ma_Window_Handlers= mwh.Main_Wind_Handlers(self)
        self.rx_Timer= QTimer()
        self.rx_Timer.setInterval(500)
        self.ma_Window.ser_Port_Info_LabCrl.setStyleSheet("font-size: 13px;color: #000;")
        self.Widget_Styles()
        css = """
    QMainWindow {
        background-color: #fff0f000;
    }
    QLabel {
        font-size: 14px;
        color: #000;
        background-color: #fff0f0bb;
        
    }
    QPushButton {
        background-color: #f0f0f0;
        border-radius: 5px;
        border: 1px solid #000;
    }
    QPushButton:hover {
        background-color: #f0f;
    }

    QListWidget::item {
        
        border-radius: 2px;
        border: 1px solid #000;
        selection-color: #ff0000;
    }
    QListWidget::item::selected {
        
        border-radius: 2px;
        border: 1px solid #ff0000;
        selection-color: #f0a;
        selection-background-color: #00ff00;
        
    }
    QListWidget::item:hover {  
        background-color: #f0f0f0;
        border-radius: 5px;
        border: 1px solid #000;
    }
    """
        self.setStyleSheet(css)
        self.ma_Window.volt_time_Tabel.setColumnCount(2)
        self.ma_Window.volt_time_Tabel.setHorizontalHeaderLabels(["Time","Voltage"])
        
        self.Events_Handlers()
        
    def Events_Handlers(self):
        self.ma_Window.get_Sys_portsBtn.clicked.connect(self.ma_Window_Handlers.onlist_Sys_Ports_btn)  
        self.ma_Window.select_Port_Btn.clicked.connect(self.ma_Window_Handlers.onSelectPort)
        self.ma_Window.read_Rx_Data_Btn.clicked.connect(self.ma_Window_Handlers.onReceiveData)
        self.rx_Timer.timeout.connect(self.ma_Window_Handlers.onrxTimer)
    
    def Widget_Styles(self):
        """
        Sets the alignment and style for labels in the main window.

        This method aligns the text of `label` and `label_2` to the center and 
        applies a bold font weight and black color to their text.
        """
        self.ma_Window.label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.ma_Window.label_2.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.ma_Window.label.styleSheet = "font-weigt:bold ;color: #000;"
        self.ma_Window.label_2.styleSheet = "font-weigt:bold ;color: #000;"
            
if __name__ == "__main__":
    mApp = QApplication([])
    MainWindow = MainWindow()
    MainWindow.show()
    sys.exit(mApp.exec())