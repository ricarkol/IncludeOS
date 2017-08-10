#define DEBUG
#define DEBUG2
#include "solo5blk.hpp"

#include <kernel/irq_manager.hpp>
#include <hw/pci.hpp>
#include <cassert>
#include <stdlib.h>

#include <statman>

extern "C" {
#include <solo5.h>
}

Solo5Blk::Solo5Blk()
  : hw::Block_device()
{
  INFO("Solo5Blk", "Block device with %llu sectors", solo5_blk_sectors());
}

Solo5Blk::block_t Solo5Blk::size() const noexcept {
  return solo5_blk_sectors();
}

Solo5Blk::buffer_t Solo5Blk::read_sync(block_t blk) {
  auto buffer = new uint8_t[block_size()];
  int rlen = SECTOR_SIZE;

  solo5_blk_read_sync((uint64_t) blk, buffer, &rlen);
  return buffer_t{buffer, std::default_delete<uint8_t[]>()};
}

Solo5Blk::buffer_t Solo5Blk::read_sync(block_t blk, size_t count) {
  auto buffer = new uint8_t[block_size() * count];
  int rlen = SECTOR_SIZE * count;

  solo5_blk_read_sync((uint64_t) blk, buffer, &rlen);
  return buffer_t{buffer, std::default_delete<uint8_t[]>()};
}

int Solo5Blk::write_sync(block_t blk, size_t count, char *buf) {
  int len = SECTOR_SIZE * count;
  int rc = solo5_blk_write_sync((uint64_t) blk, (uint8_t *) buf, len);
  return (rc == 0)? len : 0;
}

void Solo5Blk::deactivate()
{
  INFO("Solo5Net", "deactivate");
}

#include <kernel/solo5_manager.hpp>

struct Autoreg_solo5blk {
  Autoreg_solo5blk() {
    Solo5_manager::register_blk(&Solo5Blk::new_instance);
  }
} autoreg_solo5blk;
