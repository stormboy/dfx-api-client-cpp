// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/utils/HexDump.hpp"
#include <fmt/format.h>
#include <sstream>

using fmt::format;
using namespace dfx::api::utils;

std::string dfx::api::utils::hexDump(const char* desc, const void* addr, size_t len)
{
    int i;
    unsigned char buff[17];
    unsigned char* pc = (unsigned char*)addr;

    std::stringstream dump;

    // Output description if given.
    if (desc != nullptr) {
        dump << desc;
    }

    if (len == 0) {
        dump << "  ZERO LENGTH\n";
        return dump.str();
    }
    if (len < 0) {
        dump << "  NEGATIVE LENGTH: " << len << "\n";
        return dump.str();
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0) {
                dump << "  " << buff << "\n";
            }

            // Output the offset.
            dump << format("  {:04x} ", i);
        }

        // Now the hex code for the specific character.
        dump << format(" {:02x}", pc[i]);
        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        dump << "   ";
        i++;
    }

    // And print the final ASCII bit.
    dump << "  " << buff << "\n";
    return dump.str();
}
