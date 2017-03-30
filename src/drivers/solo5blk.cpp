#define DEBUG
#define DEBUG2
#include "solo5blk.hpp"

#include <kernel/irq_manager.hpp>
#include <hw/pci.hpp>
#include <cassert>
#include <stdlib.h>

#include <statman>

Solo5Blk::Solo5Blk(hw::PCI_Device& d)
  : hw::Block_device()
{
  INFO("Solo5Blk", "Block_devicer initializing");

/*
  // Hook up IRQ handler (inherited from Virtio)
  if (is_msix())
  {
    // update IRQ subscriptions
    IRQ_manager::get().subscribe(irq() + 0, {this, &Solo5Blk::service_RX});
    IRQ_manager::get().subscribe(irq() + 1, {this, &Solo5Blk::msix_conf_handler});
  }
  else
  {
    auto del(delegate<void()>{this, &Solo5Blk::irq_handler});
    IRQ_manager::get().subscribe(irq(), del);
  }
*/

  // Done
  INFO("Solo5Blk", "Block device with %llu sectors capacity", 12345);
}


Solo5Blk::block_t Solo5Blk::size() const noexcept {
  return 4000000000 / 512;
}

Solo5Blk::buffer_t Solo5Blk::read_sync(block_t blk) {
  INFO("Solo5Blk", "%s\n", __FUNCTION__);
  return buffer_t{};
}

Solo5Blk::buffer_t Solo5Blk::read_sync(block_t blk, size_t count) {
  INFO("Solo5Blk 2", "%s\n", __FUNCTION__);
  return buffer_t{};
}


void Solo5Blk::deactivate()
{
}

#include <kernel/solo5_manager.hpp>

/** Global constructor - register Solo5Blk's driver factory at the PCI_manager */
struct Autoreg_solo5blk {
  Autoreg_solo5blk() {
    Solo5_manager::register_driver<hw::Block_device>(hw::PCI_Device::VENDOR_SOLO5, 0x1001, &Solo5Blk::new_instance);
  }
} autoreg_solo5blk;
