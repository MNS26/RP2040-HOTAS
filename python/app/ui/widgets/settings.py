''' app/ui/widgets/treeview.py '''
import sys
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
    super(Settings, self).__init__()
    self.setWindowTitle("Settings")
    self.setGeometry(0, 0,  650, 500)


    # Setup Grid
    main_layout = QGridLayout()
    self.setLayout(main_layout)
    
    # Create tab widget
    tab = QTabWidget(self)

    # Journal tab
    journal_page = QWidget(self)
    layout = QGridLayout()
    journal_page.setLayout(layout)

    file_browser_btn = QPushButton('Browse')
    file_browser_btn.clicked.connect(self.open_directory_dialog)
    self.directory = QLineEdit()    
    if self.directory.text == "":
      self.directory.setText(str(Path.home()))

    layout.addWidget(QLabel('Directory:'), 0, 0, alignment=Qt.AlignmentFlag.AlignTop)
    layout.addWidget(self.directory, 0, 1, alignment=Qt.AlignmentFlag.AlignTop)
    layout.addWidget(file_browser_btn, 0 ,2, alignment=Qt.AlignmentFlag.AlignTop)

    # Coriolis tab
    coriolis_page = QWidget(self)
    layout = QGridLayout()
    coriolis_page.setLayout(layout)
    
    # EDDN tab
    eddn_page = QWidget(self)
    layout = QGridLayout()
    eddn_page.setLayout(layout)
    
    # EDSM tab
    edsm_page = QWidget(self)
    layout = QGridLayout()
    edsm_page.setLayout(layout)
        
    # Inara tab
    inara_page = QWidget(self)
    layout = QGridLayout()
    inara_page.setLayout(layout)
    

    # Add tabds to page
    tab.addTab(journal_page,"Journal")
    tab.addTab(coriolis_page, "Coriolis")
    tab.addTab(eddn_page, "EDDN")
    tab.addTab(edsm_page, "EDSM")
    tab.addTab(inara_page, "Inara")
    main_layout.addWidget(tab,0,0)



  def open_directory_dialog(self):
    self.directory.text = QFileDialog.getExistingDirectory(self, "Select Journal directory", self.directory.text) # type: ignore

    if self.directory.text:
      path = Path(self.directory.text)
      self.directory.setText(str(path))