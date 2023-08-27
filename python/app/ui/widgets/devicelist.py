''' app/ui/widgets/devicelist.py '''

import typing
import hid
from PyQt6.QtWidgets import QComboBox,QWidget


class DeviceListBox(QComboBox):
  def __init__(self, parent: QWidget | None = ...) -> None:
    super().__init__(parent)
    self.activated.connect(self.refreshItems)
    self.addItem(self.refreshItems())
  
  
  def refreshItems(self):
    devices = hid.enumerate()
    unique_devices = DeviceListBox.filter_unique_devices(devices)

    self.clear()
    #self.addItem("------")
    for device_info in unique_devices:
      device_name = f"{device_info['manufacturer_string']} - {device_info['product_string']}"
      self.addItem(device_name, userData=device_info)

  def showPopup(self) -> None:
    self.refreshItems()
    return super().showPopup()


  @staticmethod
  def filter_unique_devices(devices) -> list:
    unique_devices = {}

    for device_info in devices:
      key = (device_info['vendor_id'], device_info['product_id'], device_info['serial_number'])
      if key not in unique_devices:
        unique_devices[key] = device_info

    return list(unique_devices.values())