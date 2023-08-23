''' app/ui/widgets/treeview.py '''
from ...utils.config import AppConfig
from pathlib import Path 
from PyQt6.QtCore import Qt
from PyQt6.QtWidgets import  QLabel, QWidget, QPushButton, QFileDialog, QGridLayout, QLineEdit, QTabWidget

class Settings(QWidget):
  """
  This "window" is a QWidget. If it has no parent, it
  will appear as a free-floating window as we want.
  """
  def __init__(self):

    # Generate settings window
    self.appconfig = AppConfig()
    super(Settings, self).__init__()
    self.setWindowTitle("Settings")
    self.setGeometry(0, 0,  650, 500)



    # Setup Grid
    main_layout = QGridLayout()
    self.setLayout(main_layout)
    
    # Create tab widget
    tab = QTabWidget(self)

    # Add tabds to page
    tab.addTab(self.journal(),"Journal")
    #tab.addTab(coriolis_page, "Coriolis")
    #tab.addTab(eddn_page, "EDDN")
    #tab.addTab(edsm_page, "EDSM")
    #tab.addTab(inara_page, "Inara")
    main_layout.addWidget(tab,0,0)

  def journal(self):
    # Journal tab
    journal_page = QWidget(self)
    layout = QGridLayout()
    journal_page.setLayout(layout)

    file_browser_btn = QPushButton('Browse')
    file_browser_btn.clicked.connect(lambda: self.locate_journal('Journal'))
    self.directory = QLineEdit()    
    
    self.directory.setText(self.appconfig.game_log_dir)

    layout.addWidget(QLabel('Directory:'), 0, 0, alignment=Qt.AlignmentFlag.AlignTop)
    layout.addWidget(self.directory, 0, 1, alignment=Qt.AlignmentFlag.AlignTop)
    layout.addWidget(file_browser_btn, 0 ,2, alignment=Qt.AlignmentFlag.AlignTop)
    return journal_page

  def coriolis(self):
    # Coriolis tab
    coriolis_page = QWidget(self)
    layout = QGridLayout()
    coriolis_page.setLayout(layout)
    return coriolis_page
  
  def eddn(self):
    # EDDN tab
    eddn_page = QWidget(self)
    layout = QGridLayout()
    eddn_page.setLayout(layout)
    return eddn_page
  
  def edsm(self):
    # EDSM tab
    edsm_page = QWidget(self)
    layout = QGridLayout()
    edsm_page.setLayout(layout)
    return edsm_page
  
  def inara(self):
    # Inara tab
    inara_page = QWidget(self)
    layout = QGridLayout()
    inara_page.setLayout(layout)
    return inara_page

  def locate_journal(self,section ):
    # Open the file dialog in the specified directory
    initial_directory = str(Path.home()) if self.appconfig.game_log_dir == "" else self.appconfig.game_log_dir
    selected_directory = QFileDialog.getExistingDirectory(self, "Select Journal directory", initial_directory)

    if selected_directory:
      self.directory.setText(selected_directory)
      self.appconfig.game_log_dir = selected_directory
      self.appconfig.save_setting(section, 'directory', selected_directory)  
