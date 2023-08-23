''' app/ui/main_window.py '''
from PyQt6.QtCore import Qt
from PyQt6.QtWidgets import QMainWindow, QWidget, QHBoxLayout, QTextEdit

from ..utils.monitor.journal import Journal
from ..utils.monitor.status import Status
from ..utils.monitor.shipyard import Shipyard
from ..utils.monitor.market import Market
from ..utils.monitor.modulesinfo import ModulesInfo
from ..utils.monitor.navroute import NavRoute
from ..utils.monitor.outfitting import Outfitting
from ..utils.monitor.cargo import Cargo

from ..utils.config import AppConfig
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

    """
    Initialize all threads different threads
    """
    #journalMonitor = Journal()
    #journalMonitor.start(AppConfig.game_log_dir)
    StatusMonitor = Status()
    StatusMonitor.start(AppConfig.game_log_dir)
    #ShipyardMonitor = Shipyard()
    #ShipyardMonitor.start(AppConfig.game_log_dir)
    #OutfittingMonitor = Outfitting()
    #OutfittingMonitor.start(AppConfig.game_log_dir)
    #MarketMonitor = Market()
    #MarketMonitor.start(AppConfig.game_log_dir)
    #ModulesInfoMonitor = ModulesInfo()
    #ModulesInfoMonitor.start(AppConfig.game_log_dir)
    #NavRouteMonitor = NavRoute()
    #NavRouteMonitor.start(AppConfig.game_log_dir)
    #CargoMonitor = Cargo()
    #CargoMonitor.start(AppConfig.game_log_dir)




    super().__init__()

    # Setting configs
    self.settings = None
    self.app_path = AppConfig.main_dir
    self.path_config = AppConfig.config_file_path

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

  def exit_app(self) -> None:
    """
    Event handler for the "Exit" button. Closes the application.
    """
    if self.settings is not None:
      self.settings.close()
    self.close()


  def settings_window(self) -> None:
    """
    Event handler for the "Settings" button. Displays the "Settings" window.
    """
    if self.settings is None:
        self.settings = Settings()

    # Set the settings window as modal
    #self.settings.setWindowModality(Qt.WindowModality.WindowModal)

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
