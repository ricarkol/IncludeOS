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

constexpr size_t MTU_ = 1520;
constexpr size_t BUFFER_CNT = 1000;
// TODO: check if the NIC bufstore is initialized
BufferStore solo5_bufstore{ BUFFER_CNT,  MTU_ };

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


void hexDump (char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != NULL)
        printf ("%s:\n", desc);

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %i\n",len);
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.
            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf ("  %s\n", buff);
}

std::unique_ptr<Packet>
Solo5Net::recv_packet(uint8_t* data, uint16_t size)
{
  auto* pckt = (Packet*) solo5_bufstore.get_buffer();
  new (pckt) Packet(MTU_, size, &solo5_bufstore);
  uint8_t* data_ = reinterpret_cast<uint8_t *>(pckt->buffer());
  memcpy(data_, data, size);
  return std::unique_ptr<Packet> (pckt);
}

void Solo5Net::upstream_received_packet(uint8_t *data, int len)
{
  auto pckt_ptr = recv_packet(data, len);
  INFO("Solo5Net", "receive packet of len %d", len);
  //hexDump("packet", data, len);
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
