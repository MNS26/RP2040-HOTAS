''' app/ui/main_window.py '''
from PyQt6.QtCore import Qt
from PyQt6.QtWidgets import QMainWindow, QWidget, QHBoxLayout, QTextEdit

from ..utils.config import AppConfig
from ..utils.monitor import JournalLogs, StatusLog, ShipyardLog

from .widgets.menubar import MenuBar
from .widgets.toolbar import ToolBar
from .widgets.statusbar import StatusBar
from .widgets.treeview import TreeView
from .widgets.settings import Settings

class MainWindow(QMainWindow):
  """
  MainWindow

  Args:
  QMainWindow (QMainWindow): Inheritance
  """

  def __init__(self) -> None:
    """
    Initialize the Main-Window.
    """
    super().__init__()

    # Setting configs
    self.settings = None
    self.app_path = AppConfig.path
    self.path_config = AppConfig.path_config
    self.configfile = AppConfig.configfile

    # Window-Settings
    self.setWindowTitle(AppConfig.APP_NAME)
    self.setGeometry(100, 100, 800, 600)
    central_widget = QWidget(self)
    self.setCentralWidget(central_widget)

    layout = QHBoxLayout(central_widget)
    central_widget.setLayout(layout)

    # Create Widgets
    self.treeview = self.create_treeview()
    self.editbox = self.create_edit()

    self.create_toolbars()

    # Add Widgets to Window
    self.setMenuBar(MenuBar(self))
    self.setStatusBar(StatusBar(self))

    layout.addWidget(self.treeview)
    layout.addWidget(self.editbox, stretch=1)
    layout.addWidget(self.editbox)

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
    #self.topbar.add_button(
    #  "Open",
    #  self.app_path + "/resources/assets/icons/window/file.ico",
    #  self.open_file)
    
    #self.topbar.add_button(
    #  "Save",
    #  self.app_path + "/resources/assets/icons/window/save.ico",
    #  self.save_file)
    
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


    # Right Toolbar Buttons
    #self.rightbar.add_separator()
    #self.rightbar.add_button(
    #  "Privacy",
    #  self.app_path + "/resources/assets/icons/window/shell32-167.ico", self.privacy_window)


    self.addToolBar(
      Qt.ToolBarArea.TopToolBarArea,
      self.topbar)

  def create_treeview(self) -> TreeView:
    """
    Creates and adds the tree view widget to the main window.
    """
    return TreeView(self)


  def create_edit(self) -> QTextEdit:
    """
    Creates and adds the QTextEdit widget to the main window.
    """
    return QTextEdit(self)

  def open_file(self) -> None:
    """
    Event handler for the "Open" button. Displays the "Open File" dialog.
    """
    print("Open")

  def save_file(self) -> None:
    """
    Event handler for the "Save" button. Displays the "Save File" dialog.
    """
    print("Save")

  def exit_app(self) -> None:
    """
    Event handler for the "Exit" button. Closes the application.
    """
    if self.settings is not None:
      self.settings.close()
    self.close()

  #def settings_window(self) -> None:
  def settings_window(self):
    """
    Event handler for the "Settings" button. Displays the "Settings" window.
    """
    if self.settings is None:
      self.settings = Settings()
      self.settings.directory.textChanged.connect(lambda x: self.save_log_dir(x))

    self.settings.directory.text = AppConfig.game_log_dir   # type: ignore
    self.settings.window().move(self.pos())                 # type: ignore
    self.settings.show()
    
  def privacy_window(self) -> None:
    """
    Event handler for the "Privacy" button. Displays the "Privacy" window.
    """
    print("privacy_window")

  def save_log_dir(self,dir=None):
    if AppConfig.game_log_dir != str(dir) or not None:
      AppConfig.game_log_dir = str(dir)

    
