from PyQt6.QtWidgets import QMainWindow, QApplication
from PyQt6 import QtCore, QtGui, QtWidgets
from PyQt6.QtCore import Qt
from PyQt6.QtGui import QMouseEvent
from Matplot_Files.pyqt_matplot_canva import MplCanvas
from matplotlib.backends.backend_qtagg import (NavigationToolbar2QT as NavigationToolbar)


from icecream import ic
class Matplot_Window(QtWidgets.QDialog):
    def __init__(self,parent):
        super().__init__(parent)
        self.setObjectName("Matplot_Window")
        self.resize(800, 600)
        self.verticalLayout = QtWidgets.QVBoxLayout(self)
        self.verticalLayout.setObjectName("verticalLayout")
        self.mpl_widget = MplCanvas(None,self, width=5, height=4, dpi=100)
        
        self.toolbar = NavigationToolbar(self.mpl_widget, self)
        self.verticalLayout.addWidget(self.toolbar)
        self.verticalLayout.addWidget(self.mpl_widget)
        self.setLayout(self.verticalLayout)
        #self.mpl_cursor = Cursor(self.mpl_widget.axes)
        

        #cursor = Cursor(self.mpl_widget.axes)
        #self.mpl_widget.mpl_connect('motion_notify_event', cursor.on_mouse_move)
    
   
        self.show()
    #-def on_mouse_move(self, event):
    #-    if event.inaxes:
    #-        self.mpl_widget.axes.lines[-1].remove() if len(self.mpl_widget.axes.lines) > 1 else None
    #-        self.mpl_widget.axes.axvline(x=event.xdata, color='gray', linestyle='--')
    #-        self.mpl_widget.axes.axhline(y=event.ydata, color='gray', linestyle='--')
    #-        self.mpl_widget.draw()
    #def mouseMoveEvent(self, event: QMouseEvent):
    #    ic(f"Mouse moved to ({event.position().x()}, {event.position().y()})")   
    #def mousePressEvent(self, event):
    #    if event.button() == Qt.MouseButton.LeftButton:
    #        ic('Mouse Event: Left Button Pressed')
    #    elif event.button() == Qt.MouseButton.RightButton:
    #        ic('Mouse Event: Right Button Pressed')
    #
    #def mouseMoveEvent(self, event):
    #    self.mpl_cursor.set_cross_hair_visible(True)
    #    x, y = event.xdata, event.ydata
    #    # update the line positions
    #    self.mpl_cursor.horizontal_line.set_ydata([y])
    #    self.mpl_cursor.vertical_line.set_xdata([x])
    #    ic(f'x={x:1.2f}, y={y:1.2f}')
    #    self.mpl_cursor.ax.figure.canvas.draw()    
    #    #return super().mouseMoveEvent(a0)

# class Cursor:
#    # """
#    # A cross hair cursor.
#    # """
#     def __init__(self, ax):
#        self.ax = ax
#        self.horizontal_line = ax.axhline(color='k', lw=0.8, ls='--')
#        self.vertical_line = ax.axvline(color='k', lw=0.8, ls='--')
#        # text location in axes coordinates
#        self.text = ax.text(0.72, 0.9, '', transform=ax.transAxes)
    
#     def set_cross_hair_visible(self, visible):
#         need_redraw = self.horizontal_line.get_visible() != visible
#         self.horizontal_line.set_visible(visible)
#         self.vertical_line.set_visible(visible)
#         self.text.set_visible(visible)
#         return need_redraw
    
#     # def mouseMoveEvent(self, event: QMouseEvent): 
#     #     ic(f"Mouse moved to ({event.position().x()}, {event.position().y()})")
    
#     def on_mouse_move(self, event):
#          if not event.inaxes:
#              need_redraw = self.set_cross_hair_visible(False)
#              if need_redraw:
#                  self.ax.figure.canvas.draw()
#          else:
#              self.set_cross_hair_visible(True)
#              x, y = event.xdata, event.ydata
#              # update the line positions
#              self.horizontal_line.set_ydata([y])
#              self.vertical_line.set_xdata([x])
#              self.text.set_text(f'x={x:1.2f}, y={y:1.2f}')
#              self.ax.figure.canvas.draw()    

        
# class Cursor:
    # """
    # A cross hair cursor.
    # """
    # def __init__(self, ax):
        # self.ax = ax
        # self.horizontal_line = ax.axhline(color='k', lw=0.8, ls='--')
        # self.vertical_line = ax.axvline(color='k', lw=0.8, ls='--')
        # # text location in axes coordinates
        # self.text = ax.text(0.72, 0.9, '', transform=ax.transAxes)

    # def set_cross_hair_visible(self, visible):
        # need_redraw = self.horizontal_line.get_visible() != visible
        # self.horizontal_line.set_visible(visible)
        # self.vertical_line.set_visible(visible)
        # self.text.set_visible(visible)
        # return need_redraw

    # def on_mouse_move(self, event):
        # if not event.inaxes:
            # need_redraw = self.set_cross_hair_visible(False)
            # if need_redraw:
                # self.ax.figure.canvas.draw()
        # else:
            # self.set_cross_hair_visible(True)
            # x, y = event.xdata, event.ydata
            # # update the line positions
            # self.horizontal_line.set_ydata([y])
            # self.vertical_line.set_xdata([x])
            # self.text.set_text(f'x={x:1.2f}, y={y:1.2f}')
            # self.ax.figure.canvas.draw()    