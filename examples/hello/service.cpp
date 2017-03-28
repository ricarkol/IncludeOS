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

#include <service>
#include <cstdio>
#include <solo5.h>

#define SECTOR_SIZE     512

/* Space for 2 sectors for edge-case tests */
static uint8_t wbuf[SECTOR_SIZE * 2];
static uint8_t rbuf[SECTOR_SIZE * 2];

int check_sector_write(uint64_t sector)
{
    int rlen = SECTOR_SIZE;
    unsigned i;

    for (i = 0; i < SECTOR_SIZE; i++) {
        wbuf[i] = '0' + i % 10;
        rbuf[i] = 0;
    }

    if (solo5_blk_write_sync(sector, wbuf, SECTOR_SIZE) != 0)
        return 1;
    if (solo5_blk_read_sync(sector, rbuf, &rlen) != 0)
        return 1;

    if (rlen != SECTOR_SIZE)
        return 1;

    for (i = 0; i < SECTOR_SIZE; i++) {
        if (rbuf[i] != '0' + i % 10)
            /* Check failed */
            return 1;
    }

    return 0;
}


void Service::start(const std::string& args)
{
    size_t i, nsectors;
    int rlen;
    printf("Hello world - OS included!\n");
    printf("Args = %s\n", args.c_str());

    /*
     * Write and read/check one tenth of the disk.
     */
    nsectors = solo5_blk_sectors();
    printf("nsectors %d\n", nsectors);
    for (i = 0; i <= nsectors; i += 10) {
        if (check_sector_write(i))
            /* Check failed */
            goto failed;
    }
    printf("Check passed\n");

    /*
     * Check edge case: read/write of last sector on the device.
     */
    if (solo5_blk_write_sync(nsectors - 1, wbuf, SECTOR_SIZE) != 0)
        goto failed;
    rlen = SECTOR_SIZE;
    if (solo5_blk_read_sync(nsectors - 1, rbuf, &rlen) != 0)
        goto failed;
    if (rlen != SECTOR_SIZE)
        goto failed;

    /*
     * Check edge cases: should not be able to read or write beyond end
     * of device.
     *
     * XXX Multi-sector block operations currently work only on ukvm, virtio
     * will always return -1 here.
     */
    if (solo5_blk_write_sync(nsectors - 1, wbuf, 2 * SECTOR_SIZE) != -1)
        goto failed;
    rlen = 2 * SECTOR_SIZE;
    if (solo5_blk_read_sync(nsectors - 1, rbuf, &rlen) != -1)
        goto failed;

    puts("SUCCESS\n");
    return;

failed:
    printf("Check failed\n");
}
