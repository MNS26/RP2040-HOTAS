#ifndef USBD_MSC_H_
#define USBD_MSC_H_
#include "usbd_device.h"

class USBD_MSC : public USBD_Interface
{
public:
  typedef int32_t (*read_callback_t)(uint32_t lba, void *buffer,
                                     uint32_t bufsize);
  typedef int32_t (*write_callback_t)(uint32_t lba, uint8_t *buffer,
                                      uint32_t bufsize);
  typedef void (*flush_callback_t)(void);
  typedef bool (*ready_callback_t)(void);
  typedef bool (*writable_callback_t)(void);
  typedef bool (*start_stop_callback_t)(uint8_t power_condition, bool start,
                                        bool load_eject);

  USBD_MSC(void);

  ~USBD_MSC();

  bool begin(void);

  void setMaxLun(uint8_t maxlun);
  uint8_t getMaxLun(void);

  //------------- Multiple LUN API -------------//
  void setID(uint8_t lun, const char *vendor_id, const char *product_id,
             const char *product_rev);
  void setCapacity(uint8_t lun, uint32_t block_count, uint16_t block_size);
  void setUnitReady(uint8_t lun, bool ready);
  void setReadWriteCallback(uint8_t lun, read_callback_t rd_cb,
                            write_callback_t wr_cb, flush_callback_t fl_cb);
  void setReadyCallback(uint8_t lun, ready_callback_t cb);
  void setWritableCallback(uint8_t lun, writable_callback_t cb);
  void setStartStopCallback(uint8_t lun, start_stop_callback_t cb);

  //------------- Single LUN API -------------//
  void setID(const char *vendor_id, const char *product_id,
             const char *product_rev) {
    setID(0, vendor_id, product_id, product_rev);
  }
uint8_t setStringIndex(uint8_t index);

  void setCapacity(uint32_t block_count, uint16_t block_size) {
    setCapacity(0, block_count, block_size);
  }

  void setUnitReady(bool ready) { setUnitReady(0, ready); }

  void setReadWriteCallback(read_callback_t rd_cb, write_callback_t wr_cb,
                            flush_callback_t fl_cb) {
    setReadWriteCallback(0, rd_cb, wr_cb, fl_cb);
  }

  void setReadyCallback(ready_callback_t cb) { setReadyCallback(0, cb); }
  void setWritableCallback(writable_callback_t cb) {
    setWritableCallback(0, cb);
  }
  void setStartStopCallback(start_stop_callback_t cb) {
    setStartStopCallback(0, cb);
  }

  // from Adafruit_USBD_Interface
  virtual uint16_t getInterfaceDescriptor(uint8_t itfnum_deprecated,
                                          uint8_t *buf, uint16_t bufsize);

private:
  enum { MAX_LUN = 2 }; // TODO make it configurable
  struct {
    read_callback_t rd_cb;
    write_callback_t wr_cb;
    flush_callback_t fl_cb;
    ready_callback_t ready_cb;
    writable_callback_t writable_cb;
    start_stop_callback_t start_stop_cb;

    const char *_inquiry_vid;
    const char *_inquiry_pid;
    const char *_inquiry_rev;

    uint32_t block_count;
    uint16_t block_size;
    bool unit_ready;

  } _lun_info[MAX_LUN];
  
  uint8_t _maxlun;

  uint8_t stringIndex = 0;

  // Make all tinyusb callback friend to access private data
  friend void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8],
                                 uint8_t product_id[16],
                                 uint8_t product_rev[4]);
  friend bool tud_msc_test_unit_ready_cb(uint8_t lun);
  friend void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count,
                                  uint16_t *block_size);
  friend int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset,
                                   void *buffer, uint32_t bufsize);
  friend int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset,
                                    uint8_t *buffer, uint32_t bufsize);
  friend void tud_msc_write10_complete_cb(uint8_t lun);
  friend bool tud_msc_is_writable_cb(uint8_t lun);
  friend bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition,
                                    bool start, bool load_eject);};
#endif