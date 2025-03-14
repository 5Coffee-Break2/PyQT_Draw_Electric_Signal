from PySide6 import QtWidgets  # import PySide6 before matplotlib
import matplotlib
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg
from matplotlib.figure import Figure
from matplotlib.pyplot import subplots
from icecream import ic
from matplotlib.backend_bases import MouseEvent     #used for event type hint
from matplotlib.lines import Line2D                 #used for event type hint
import numpy as np
matplotlib.use("QtAgg")

class MplCanvas(FigureCanvasQTAgg):
   def __init__(self,xdata=None,ydata=None,arent=None, width=5, height=4, dpi=100):
           self.mpl_Fig = Figure(figsize=(width, height), dpi=dpi)
           self.mpl_Axes = self.mpl_Fig.add_subplot(111)
           #_self.mpl_Fig,self.mpl_Axes = subplots()
           #_self.mpl_Fig.set_figwidth(width)
           #_self.mpl_Fig.set_figheight(height)
           #_self.mpl_Fig.set_dpi(dpi)
           super().__init__(self.mpl_Fig)
           self.canva_Cursor = None
           #self.fig.canvas.mpl_connect('motion_notify_event', self.canva_Cursor.on_mouse_move)
           self.xData=xdata
           self.yData =ydata  
           
   def Setup_Cursor(self):
        self.canva_Cursor = Cursor(self.mpl_Axes,self.xData,self.yData)
        self.mpl_Fig.canvas.mpl_connect('motion_notify_event', self.canva_Cursor.on_mouse_move)
        #ic(self.axes.add_table())
        
class Cursor:
   # """
   # A cross hair cursor.
   # """
    def __init__(self, ax,xd:np.ndarray,yd=np.ndarray):
       self.ax = ax
       self.x_Ax_Data = xd
       self.y_Ax_Data=yd
       self.horizontal_line:Line2D = ax.axhline(color='k', lw=0.8, ls='--')
       self.vertical_line:Line2D = ax.axvline(color='k', lw=0.8, ls='--')
       # text location in axes coordinates
       #self.text = ax.text(0.72, 0.9, '', transform=ax.transAxes)
       self.index=0
       
    def set_cross_hair_visible(self, visible):
        need_redraw = self.horizontal_line.get_visible() != visible
        self.horizontal_line.set_visible(visible)
        self.vertical_line.set_visible(visible)
        #-self.text.set_visible(visible)
        return need_redraw
    
    # def mouseMoveEvent(self, event: QMouseEvent): 
    #     ic(f"Mouse moved to ({event.position().x()}, {event.position().y()})")
   
    def on_mouse_move(self, event:MouseEvent):
         #tmpx=0.0
         if not event.inaxes:
             need_redraw = self.set_cross_hair_visible(False)
             if need_redraw:
                 self.ax.figure.canvas.draw()
         else:
             self.set_cross_hair_visible(True)
             x, y = event.xdata, event.ydata  #type of (event.xdata): <class 'numpy.float64'>
                        
             # update the line positions
             try:
                #@ make the y value cusor follow the x value cursor
                for i,v in enumerate(self.x_Ax_Data):
                 if np.round(x,2)==np.round(v,2):
                    #ic(f"{x} found in rx time")
                    yv= self.y_Ax_Data[i]
                    self.horizontal_line.set_ydata([yv])#([req_y]) #(self.y_Ax_Data[req_indx]) #
                    self.vertical_line.set_xdata([v])
                    #self.text.set_text(f"x={v:1.2f}, y={yv:1.2f}")
                    self.ax.figure.canvas.draw()
                #ic(self.ax.get_cursor_data(event))
                
             #print(event.x,event.xdata,"\r")
             except Exception as ex:
                 ic(ex)
             