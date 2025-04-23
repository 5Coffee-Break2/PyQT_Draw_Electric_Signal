import global_modules as gm

class MainWindowExtraWidgets:
    def __init__(self, parent:gm.QtWidgets.QMainWindow):
        self.parent = parent
        self.create_widgets()
        self.place_widgets()
    
    def create_widgets(self):
        self.debug_label_DB1 = gm.QtWidgets.QLabel(self.parent, text="This is a Debug label 1")
        self.close_btn= gm.QtWidgets.QPushButton(self.parent,text="Close graph")
        self.close_btn.clicked.connect(self.parent.onCloseGraphBtn)
        #self.progress = QProgressBar()
       #
        #self.progress.setGeometry(20,500,500,60)
        #self.layout().addWidget(self.progress)
        #self.progress.move(20,250)
    
    def place_widgets(self):
        self.debug_label_DB1.setGeometry(20,450,400,30)
        self.parent.layout().addWidget(self.debug_label_DB1)    
        
        self.close_btn.setGeometry(20,650,150,24)
        self.parent.layout().addWidget(self.close_btn)
        
    