from PySide6 import QtWidgets  # import PySide6 before matplotlib
import matplotlib
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg
from matplotlib.figure import Figure

matplotlib.use("QtAgg")

class MplCanvas(FigureCanvasQTAgg):
   def __init__(self, on_mouse_move_fn,parent=None, width=5, height=4, dpi=100):
       self.fig = Figure(figsize=(width, height), dpi=dpi)
       self.axes = self.fig.add_subplot(111)
       super().__init__(self.fig)
       self.canva_Cursor = None
       #self.fig.canvas.mpl_connect('motion_notify_event', self.canva_Cursor.on_mouse_move)
       
   def Setup_Cursor(self):
        self.canva_Cursor = Cursor(self.axes)
        self.fig.canvas.mpl_connect('motion_notify_event', self.canva_Cursor.on_mouse_move)
        
class Cursor:
   # """
   # A cross hair cursor.
   # """
    def __init__(self, ax):
       self.ax = ax
       self.horizontal_line = ax.axhline(color='k', lw=0.8, ls='--')
       self.vertical_line = ax.axvline(color='k', lw=0.8, ls='--')
       # text location in axes coordinates
       #-self.text = ax.text(0.72, 0.9, '', transform=ax.transAxes)
    
    def set_cross_hair_visible(self, visible):
        need_redraw = self.horizontal_line.get_visible() != visible
        self.horizontal_line.set_visible(visible)
        self.vertical_line.set_visible(visible)
        #-self.text.set_visible(visible)
        return need_redraw
    
    # def mouseMoveEvent(self, event: QMouseEvent): 
    #     ic(f"Mouse moved to ({event.position().x()}, {event.position().y()})")
    
    def on_mouse_move(self, event):
         if not event.inaxes:
             need_redraw = self.set_cross_hair_visible(False)
             if need_redraw:
                 self.ax.figure.canvas.draw()
         else:
             self.set_cross_hair_visible(True)
             x, y = event.xdata, event.ydata
             # update the line positions
             self.horizontal_line.set_ydata([y])
             self.vertical_line.set_xdata([x])
             #-self.text.set_text(f'x={x:1.2f}, y={y:1.2f}')
             self.ax.figure.canvas.draw()    