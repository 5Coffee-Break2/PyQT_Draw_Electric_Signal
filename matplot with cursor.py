import sys

from PyQt6.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget

import numpy as np

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure


class SnappingCursor:
    """
    A cross hair cursor that snaps to the data point of a line, which is
    closest to the *x* position of the cursor.

    For simplicity, this assumes that *x* values of the data are sorted.
    """

    def __init__(self, ax, line):
        self.ax = ax
        self.horizontal_line = ax.axhline(color="k", lw=0.8, ls="--")
        self.vertical_line = ax.axvline(color="k", lw=0.8, ls="--")
        self.x, self.y = line.get_data()
        self._last_index = None
        # text location in axes coords
        self.text = ax.text(0.72, 0.9, "", transform=ax.transAxes)

    def set_cross_hair_visible(self, visible):
        need_redraw = self.vertical_line.get_visible() != visible
        self.vertical_line.set_visible(visible)
        self.horizontal_line.set_visible(visible)
        self.text.set_visible(visible)
        return need_redraw

    def on_mouse_move(self, event):
        if not event.inaxes:
            self._last_index = None
            need_redraw = self.set_cross_hair_visible(False)
            if need_redraw:
                self.ax.figure.canvas.draw()
        else:
            self.set_cross_hair_visible(True)
            x, y = event.xdata, event.ydata
            index = min(np.searchsorted(self.x, x), len(self.x) - 1)
            if index == self._last_index:
                return  # still on the same data point. Nothing to do.
            self._last_index = index
            x = self.x[index]
            y = self.y[index]
            # update the line positions
            self.horizontal_line.set_ydata(y)
            self.vertical_line.set_xdata(x)
            self.text.set_text("x=%1.2f, y=%1.2f" % (x, y))
            self.ax.figure.canvas.draw()


class Window(QMainWindow):
    def __init__(self):
        super().__init__()

        widget = QWidget()
        vbox = QVBoxLayout(widget)

        x = np.arange(0, 1, 0.01)
        y = np.sin(2 * 2 * np.pi * x)

        canvas = FigureCanvas(Figure(tight_layout=True, linewidth=3))
        ax = canvas.figure.subplots()
        ax.set_title("Snapping cursor")
        (line,) = ax.plot(x, y, "o")
        self.snap_cursor = SnappingCursor(ax, line)
        canvas.mpl_connect("motion_notify_event", self.snap_cursor.on_mouse_move)

        vbox.addWidget(canvas)
        self.setCentralWidget(widget)
        self.setWindowTitle("Example")


app = QApplication(sys.argv)
w = Window()
w.show()
app.exec()