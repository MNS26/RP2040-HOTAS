''' app/ui/main_window.py '''
import os
import re
import hid
import json
from PyQt6.QtCore import Qt
from PyQt6.QtWidgets import QMainWindow, QWidget, QHBoxLayout, QTextEdit

from ..utils.config import AppConfig
from ..utils.filewatcher import FileWatcher
from .widgets.menubar import MenuBar
from .widgets.toolbar import ToolBar
from .widgets.statusbar import StatusBar
from .widgets.settings import Settings

class MainWindow(QMainWindow):
  """
  MainWindow

  Args:
  QMainWindow (QMainWindow): Inheritance
  """
  # default is TinyUSB (0xcafe), Adafruit (0x239a), RaspberryPi (0x2e8a), Espressif (0x303a) VID
  USB_VID = (0xcafe, 0x239a, 0x2e8a, 0x303a)
  
  def __init__(self) -> None:
    """
    Initialize the Main-Window.
    """
    super().__init__()
    self.appconfig = AppConfig()
    self.fs_watcher = FileWatcher()


    # Setting configs
    self.settings = None
    self.app_path = self.appconfig.main_dir
    self.path_config = AppConfig.config_file_path
    self.game_log_directory = str(self.appconfig.read_config('Elite Dangerous Logs','directory'))

    self.fs_watcher.initialize(self.game_log_directory)
    self.fs_watcher.start()

    # Window-Settings
    self.setWindowTitle(AppConfig.APP_NAME)
    self.setGeometry(100, 100, 800, 600)
    central_widget = QWidget(self)
    self.setCentralWidget(central_widget)

    layout = QHBoxLayout(central_widget)
    central_widget.setLayout(layout)

    # Create Widgets
    self.editbox = self.create_edit()

    self.editbox2 = self.create_edit2()
    self.editbox2.setReadOnly(True)

    self.create_toolbars()

    # Add Widgets to Window
    self.setMenuBar(MenuBar(self))
    self.setStatusBar(StatusBar(self))

    layout.addWidget(self.editbox, stretch=1)
    layout.addWidget(self.editbox)
    
    layout.addWidget(self.editbox2, stretch=1)
    layout.addWidget(self.editbox2)
    
  
  

  
  def create_toolbars(self) -> None:
    """
    Creates and adds the top and right toolbars to the main window.
    """
    # Top Toolbar [PyQt6.QtWidgets.QToolBar]
    self.topbar = ToolBar(
      self,
      orientation=Qt.Orientation.Horizontal,
      style=Qt.ToolButtonStyle.ToolButtonTextUnderIcon,
      icon_size=(24, 24))


    # Top Toolbar Buttons
    self.topbar.add_button(
      "Settings", 
      self.app_path + "/resources/assets/icons/window/settings.ico",
      self.settings_window)
    
    #self.topbar.add_button(
    #  "Privacy",
    #  self.app_path + "/resources/assets/icons/window/privacy.ico",
    #  self.privacy_window)
    
    self.topbar.add_separator()
    
    self.topbar.add_button(
      "Exit",
      self.app_path + "/resources/assets/icons/window/exit.ico",
      self.exit_app)


    self.addToolBar(
      Qt.ToolBarArea.TopToolBarArea,
      self.topbar)

  def create_edit(self) -> QTextEdit:
    """
    Creates and adds the QTextEdit widget to the main window.
    """
    return QTextEdit(self)

  def create_edit2(self) -> QTextEdit:
    """
    Creates and adds the QTextEdit widget to the main window.
    """
    return QTextEdit(self)

  def exit_app(self) -> None:
    """
    Event handler for the "Exit" button. Closes the application.
    """
    if self.fs_watcher is not None:
      self.fs_watcher.stop

    if self.settings is not None:
      self.settings.close()
    
    self.close()


  def settings_window(self) -> None:
    """
    Event handler for the "Settings" button. Displays the "Settings" window.
    """
    if self.settings is None:
        self.settings = Settings()
        self.settings.directory.textChanged.connect(lambda x: self.fs_watcher.update_directory(x))

    # Set the position of the settings window relative to the main window
    self.settings.move(self.pos())

    # Show the settings window
    self.settings.activateWindow()
    self.settings.show()


  def privacy_window(self) -> None:
    """
    Event handler for the "Privacy" button. Displays the "Privacy" window.
    """
    print("privacy_window")

