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

#define PRINT_INFO
#define DEBUG // Allow debuging
#define DEBUG2

#include "solo5net.hpp"
#include <net/packet.hpp>
#include <kernel/irq_manager.hpp>
#include <kernel/syscalls.hpp>
#include <hw/pci.hpp>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <solo5.h>

using namespace net;

const char* Solo5Net::driver_name() const { return "Solo5Net"; }

Solo5Net::Solo5Net(hw::PCI_Device& d)
  : Link(Link_protocol{{this, &Solo5Net::transmit}, mac()},
         2048u, sizeof(net::Packet) + MTU()),
    packets_rx_{Statman::get().create(Stat::UINT64, device_name() + ".packets_rx").get_uint64()},
    packets_tx_{Statman::get().create(Stat::UINT64, device_name() + ".packets_tx").get_uint64()}
{
  INFO("Solo5Net", "Driver initializing");
}

#include <cstdlib>
void Solo5Net::transmit(net::Packet_ptr pckt) {
  net::Packet_ptr tail = std::move(pckt);

  // Transmit all we can directly
  while (tail) {
    // next in line
    auto next = tail->detach_tail();
    // write data to network
    // explicitly release the data to prevent destructor being called
    net::Packet* pckt = tail.release();
    net::BufferStore::buffer_t buf = pckt->buffer();
    solo5_net_write_sync(buf, pckt->size());

    tail = std::move(next);
    // Stat increase packets transmitted
    packets_tx_++;
  }

  // Buffer the rest
  if (UNLIKELY(tail)) {
    INFO("solo5net", "Could not send all packets..\n");
  }
}


std::unique_ptr<Packet>
Solo5Net::recv_packet(uint8_t* data, uint16_t size)
{
/*
  // @WARNING: Initializing packet as "full", i.e. size == capacity
  Packet* packet = (Packet*) data;
  new (packet) Packet(1580, size, nullptr);
  return Packet_ptr(packet);
*/

 // auto* pckt = (Packet*) bufstore().get_buffer();
  auto* pckt = (Packet*) (data);
  new (pckt) Packet(bufsize(), size, &bufstore());
  //uint8_t* data_ = reinterpret_cast<uint8_t *>(pckt->buffer());
  //memcpy(data_, data, size);
  //return Packet_ptr(pckt);
  return std::unique_ptr<Packet> (pckt);
}

void Solo5Net::upstream_received_packet(uint8_t *data, int len)
{
  auto pckt_ptr = recv_packet(data, len);
  Link::receive(std::move(pckt_ptr));
}

void Solo5Net::deactivate()
{
  INFO("Solo5Net", "deactivate?");
}

#include <kernel/solo5_manager.hpp>

/** Register Solo5Net's driver factory at the Solo5_manager */
struct Autoreg_solo5net {
  Autoreg_solo5net() {
    Solo5_manager::register_driver<hw::Nic>(hw::PCI_Device::VENDOR_SOLO5, 0x1000, &Solo5Net::new_instance);
  }
} autoreg_solo5net;
