// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2016 Oslo and Akershus University College of Applied Sciences
// and Alfred Bratterud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <os>
#include <statman>
#include <kernel/irq_manager.hpp>
#include <kernel/timers.hpp>
#include <solo5.h>

// Keep track of blocking levels
static uint32_t* blocking_level = 0;
static uint32_t* highest_blocking_level = 0;


// Getters, mostly for testing
extern "C" uint32_t os_get_blocking_level() {
  return *blocking_level;
};

extern "C" uint32_t os_get_highest_blocking_level() {
  return *highest_blocking_level;
};


/**
 * A quick and dirty implementation of blocking calls, which simply halts,
 * then calls  the event loop, then returns.
 **/
void OS::block(){

  // Initialize stats
  if (not blocking_level) {
    blocking_level = &Statman::get()
      .create(Stat::UINT32, std::string("blocking.current_level")).get_uint32();
    *blocking_level = 0;
  }

  if (not highest_blocking_level) {
    highest_blocking_level = &Statman::get()
      .create(Stat::UINT32, std::string("blocking.highest")).get_uint32();
    *highest_blocking_level = 0;
  }

  // Increment level
  *blocking_level += 1;

  // Increment highest if applicable
  if (*blocking_level > *highest_blocking_level)
    *highest_blocking_level = *blocking_level;

  int rc;
  rc = solo5_poll(solo5_clock_monotonic() + 50000ULL); // now + 0.05 ms
  if (rc == 0) {
    Timers::timers_handler();
  } else {
    int len = 1520;
    uint8_t *data = (uint8_t *) malloc(1520);
    assert(data);
    memset(data, 0, 1520);

    if (solo5_net_read_sync(data, &len) == 0) {
      // make sure packet is copied
      for(auto& nic : hw::Devices::devices<hw::Nic>()) {
        nic->upstream_received_packet(data, len);
        break;
      }
    }

    free(data);
  }
  //OS::halt();

  // Process callbacks
  //IRQ_manager::get().process_interrupts();

  // Decrement level
  *blocking_level -= 1;
}
