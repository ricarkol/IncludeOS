// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2015 Oslo and Akershus University College of Applied Sciences
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

#include <assert.h>

#include <common>

#include <kernel/solo5_manager.hpp>
#include "../drivers/solo5blk.hpp"
#include "../drivers/solo5net.hpp"
#include <hw/devices.hpp>
#include <stdexcept>

#define NUM_BUSES 2

Solo5_manager::Device_registry Solo5_manager::devices_;

void Solo5_manager::init() {
  INFO("Solo5", "Looking for solo5 devices");

  uint32_t id_net {PCI::WTF};
  uint32_t id_blk {PCI::WTF};

  id_net = 0x1000CACA; // product solo5net and vendor solo5 (0xCACA)
  id_blk = 0x1001CACA; // product solo5blk and vendor solo5 (0xCACA)

  hw::PCI_Device dev_net {0xffff, id_net};
  hw::PCI_Device dev_blk {0xffff, id_blk};

  // store device
  devices_[dev_net.classcode()].emplace_back(dev_net);
  devices_[dev_blk.classcode()].emplace_back(dev_blk);

  bool registered = true;

  register_driver<hw::Nic>(hw::PCI_Device::VENDOR_SOLO5, 0x1000, &Solo5Net::new_instance);
  registered = register_device<hw::Nic>(dev_net);

  register_driver<hw::Block_device>(hw::PCI_Device::VENDOR_SOLO5, 0x1001,
                                    &Solo5Blk::new_instance);
  registered = register_device<hw::Block_device>(dev_blk);
}
