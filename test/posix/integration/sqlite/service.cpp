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

#include <os>
#include <timers>
#include <net/inet4>
#include <fs/disk.hpp>
#include <fs/vfs.hpp>

extern "C" int main();
static std::shared_ptr<fs::Disk> disk;

void Service::start(const std::string&)
{
  auto& device = hw::Devices::drive(0);
  disk = std::make_shared<fs::Disk> (device);

  INFO("FAT32", "Running tests for FAT32");
  CHECKSERT(disk, "VirtioBlk disk created");

  // which means that the disk can't be empty
  CHECKSERT(not disk->empty(), "Disk not empty");

  auto my_disk = disk;
  // auto-mount filesystem
  my_disk->init_fs(
  [my_disk] (fs::error_t err, auto& fs)
  {
    if (err) {
      INFO("VFS_test", "Error mounting disk: %s \n", err.to_string().c_str());
      return;
    }

    auto dir1 = disk->fs().stat("/db");
    fs::mount("/db", dir1, "db");

    auto t1 = RTC::now();
    main();
    auto t2 = RTC::now();
    std::cout << t2 - t1 << "seconds";
    extern void __arch_poweroff();
    __arch_poweroff();
  });

  OS::shutdown();
}