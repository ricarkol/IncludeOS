#define DEBUG
#define DEBUG2
#include "solo5blk.hpp"

#include <hw/pci.hpp>
#include <fs/common.hpp>
#include <cassert>
#include <stdlib.h>

#include <statman>

extern "C" {
#include <solo5.h>
}

Solo5Blk::Solo5Blk()
  : hw::Block_device()
{
  struct solo5_block_info bi;
  solo5_block_info(&bi);
  INFO("Solo5Blk", "Block device with %lu sectors", bi.capacity);
}

Solo5Blk::block_t Solo5Blk::size() const noexcept {
  struct solo5_block_info bi;
  solo5_block_info(&bi);
  return bi.capacity;
}

Solo5Blk::buffer_t Solo5Blk::read_sync(block_t blk)
{
  auto buffer = fs::construct_buffer(block_size());
  int rlen = SECTOR_SIZE;

  solo5_block_read((solo5_off_t) blk, buffer->data(), rlen);
  return buffer;
}

Solo5Blk::buffer_t Solo5Blk::read_sync(block_t blk, size_t count) {
  auto buffer = fs::construct_buffer(block_size() * count);
  int rlen = SECTOR_SIZE * count;

  solo5_block_read((solo5_off_t) blk, buffer->data(), rlen);
  return buffer;
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
