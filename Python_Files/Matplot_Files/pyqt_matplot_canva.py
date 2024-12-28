from PySide6 import QtWidgets  # import PySide6 before matplotlib

import matplotlib
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg
from matplotlib.figure import Figure

matplotlib.use("QtAgg")

class MplCanvas(FigureCanvasQTAgg):
   def __init__(self, parent=None, width=5, height=4, dpi=100):
       fig = Figure(figsize=(width, height), dpi=dpi)
       self.axes = fig.add_subplot(111)
       super().__init__(fig)