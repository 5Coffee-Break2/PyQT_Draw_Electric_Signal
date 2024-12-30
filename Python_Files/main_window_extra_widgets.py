import global_modules as gm

class MainWindowExtraWidgets:
    def __init__(self, parent:gm.QtWidgets.QMainWindow):
        self.parent = parent
        self.create_widgets()
        self.place_widgets()
    
    def create_widgets(self):
        self.debug_label_DB1 = gm.QtWidgets.QLabel(self.parent, text="This is a Debug label 1")
        #self.progress = QProgressBar()
       #
        #self.progress.setGeometry(20,500,500,60)
        #self.layout().addWidget(self.progress)
        #self.progress.move(20,250)
    
    def place_widgets(self):
        self.debug_label_DB1.setGeometry(20,450,400,30)
        self.parent.layout().addWidget(self.debug_label_DB1)    