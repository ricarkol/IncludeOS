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

#ifndef VIRTIO_SOLO5NET_HPP
#define VIRTIO_SOLO5NET_HPP

#include <common>
#include <hw/pci_device.hpp>
#include <net/buffer_store.hpp>
#include <net/link_layer.hpp>
#include <net/ethernet/ethernet.hpp>
#include <delegate>
#include <deque>
#include <statman>
#include <solo5.h>

class Solo5Net : public net::Link_layer<net::Ethernet> {
public:
  using Link          = net::Link_layer<net::Ethernet>;
  using Link_protocol = Link::Protocol;

  static std::unique_ptr<Nic> new_instance(hw::PCI_Device& d)
  {
    INFO2("Solo5Net constructor");
    return std::make_unique<Solo5Net>(d);
  }

  /** Human readable name. */
  const char* driver_name() const override;

  /** Mac address. */
  const hw::MAC_addr& mac() const noexcept override
  {
    char *smac = solo5_net_mac_str();
    INFO2("MAC from solo5: %s", smac);
    hw::MAC_addr mac(smac);
    INFO2("MAC after translation: %s", mac.str().c_str());
    return mac;
  }

  uint16_t MTU() const noexcept override
  { return 1500; }

  net::downstream create_physical_downstream()
  { return {this, &Solo5Net::transmit}; }

  /** Linklayer input. Hooks into IP-stack bottom, w.DOWNSTREAM data.*/
  void transmit(net::Packet_ptr pckt);

  /** Constructor. @param pcidev an initialized PCI device. */
  Solo5Net(hw::PCI_Device& pcidev);

  /** Space available in the transmit queue, in packets */
  size_t transmit_queue_available() override {
    return 1; // any big random number for now
  }

  /** Number of incoming packets waiting in the RX-queue */
  size_t receive_queue_waiting() override {
    return 0;
  }

  void deactivate() override;

  void upstream_received_packet(uint8_t *data, int len) override;

private:

  std::unique_ptr<net::Packet> recv_packet(uint8_t* data, uint16_t sz);
  /** Stats */
  uint64_t& packets_rx_;
  uint64_t& packets_tx_;

};

#endif
