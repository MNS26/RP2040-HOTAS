''' app/ui/widgets/treeview.py '''
import sys
import hid
import ast
from ...utils.config import AppConfig
from .devicelist import DeviceList
from pathlib import Path 
from PyQt6.QtCore import Qt, pyqtSignal
from PyQt6.QtWidgets import  QLabel, QWidget, QPushButton, QFileDialog, QGridLayout, QLineEdit, QTabWidget, QCheckBox, QHBoxLayout


class Settings(QWidget):
  # Report name change
  deviceNameChanged = pyqtSignal(str)
  # Report device dictionary
  deviceDict = pyqtSignal(dict)
  """
  This "window" is a QWidget. If it has no parent, it
  will appear as a free-floating window as we want.
  """
  def __init__(self):

    # Initialize settings window
    super(Settings, self).__init__()
    self.setWindowTitle("Settings")
    self.setGeometry(0, 0,  650, 500)


    self.appconfig = AppConfig()
    self.directory = QLineEdit()
    self.device = self.appconfig.usb_device
    self.devicelist = DeviceList(self,self.device)

  
    # Setup Grid
    main_layout = QGridLayout()
    self.setLayout(main_layout)
    
    # Create tab widget
    tab = QTabWidget(self)

    # Add tabds to page
    tab.addTab(self.usb_device(),"USB")
    tab.addTab(self.journal(),"Journal")
    #tab.addTab(coriolis_page, "Coriolis")
    #tab.addTab(eddn_page, "EDDN")
    #tab.addTab(edsm_page, "EDSM")
    #tab.addTab(inara_page, "Inara")
    main_layout.addWidget(tab,0,0)

  
  def getCurrentDevice(self):
    self.devices = self.devicelist.listDevices()
    # if We already have it we dont have to go through settings to get it
    for device_info in self.devices:
      device_name = f"{device_info['manufacturer_string']} - {device_info['product_string']}"
      if device_name == self.device:
        self.deviceDict.emit(device_info)

  
  def usb_device(self) -> QWidget:
    # USB tab
    self.devicelist.activated.connect(self.update_userdata)

    usb_page = QWidget(self)
    layout = QGridLayout()
    usb_page.setLayout(layout)
    self.devicelist.setInsertPolicy(self.devicelist.InsertPolicy.NoInsert)
    layout.addWidget(self.devicelist,0,0,alignment=Qt.AlignmentFlag.AlignTop)
    return usb_page


  # Journal tab
  def journal(self) -> QWidget:
    journal_page = QWidget(self)
    #layout = QBoxLayout(QBoxLayout.Direction.TopToBottom)
    layout = QGridLayout()
    toggle_log_chk = QCheckBox()

    file_browser_btn = QPushButton('Browse')

    file_browser_btn.clicked.connect(lambda: self.open_directory_dialog('Elite Dangerous Logs'))
    self.directory.setReadOnly(True)
    
    self.directory.setText(self.appconfig.game_log_dir)

    #layout.addWidget(toggle_log_chk, 0, alignment=Qt.AlignmentFlag.AlignTop)
    #layout.addWidget(QLabel('Directory:'), 1, alignment=Qt.AlignmentFlag.AlignTop)
    #layout.addWidget(self.directory, 1, alignment=Qt.AlignmentFlag.AlignTop)
    #layout.addWidget(file_browser_btn, 1, alignment=Qt.AlignmentFlag.AlignTop)
    #layout.addSpacing(100)
    
    layout.addWidget(toggle_log_chk,        0, 0, 1, 1, alignment=Qt.AlignmentFlag.AlignTop)
    layout.addWidget(QLabel('Directory:'),  1, 0, 1, 1, alignment=Qt.AlignmentFlag.AlignTop)
    layout.addWidget(self.directory,        1, 1, 1, 1, alignment=Qt.AlignmentFlag.AlignTop)
    layout.addWidget(file_browser_btn,      1 ,2, 1, 1, alignment=Qt.AlignmentFlag.AlignTop)
    layout.setVerticalSpacing(0)
    layout.setHorizontalSpacing(0)
    layout.setDefaultPositioning(0,Qt.Orientation.Vertical)

    journal_page.setLayout(layout)
    return journal_page

  def coriolis(self) -> QWidget:
    # Coriolis tab
    coriolis_page = QWidget(self)
    layout = QGridLayout()
    coriolis_page.setLayout(layout)
    return coriolis_page
  
  def eddn(self) -> QWidget:
    # EDDN tab
    eddn_page = QWidget(self)
    layout = QGridLayout()
    eddn_page.setLayout(layout)
    return eddn_page
  
  def edsm(self) -> QWidget:
    # EDSM tab
    edsm_page = QWidget(self)
    layout = QGridLayout()
    edsm_page.setLayout(layout)
    return edsm_page
  
  def inara(self) -> QWidget:
    # Inara tab
    inara_page = QWidget(self)
    layout = QGridLayout()
    inara_page.setLayout(layout)
    return inara_page



  def update_userdata(self,index):
#    if index>= 0:
      selected_index = self.devicelist.currentIndex()
      user_data = self.devicelist.itemData(selected_index) # Replace with the actual new data

      #print(self.devicelist.itemData(selected_index))
      #self.devicelist.setItemData(selected_index, user_data)
      print(f"{user_data['manufacturer_string']} - {user_data['product_string']}")
      self.deviceNameChanged.emit(f"{user_data['manufacturer_string']} - {user_data['product_string']}")
      self.deviceDict.emit(user_data)


  def open_directory_dialog(self,section=None ) -> None:

    # Open the file dialog in the specified directory
    initial_directory = str(Path.home()) if self.appconfig.game_log_dir == "" else self.appconfig.game_log_dir
    selected_directory = QFileDialog.getExistingDirectory(self, "Select Journal directory", initial_directory)

    if selected_directory:
      self.directory.setText(selected_directory)
      self.appconfig.game_log_dir = selected_directory
      self.appconfig.save_setting(section, 'directory', selected_directory)  
