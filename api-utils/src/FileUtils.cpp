// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/utils/FileUtils.hpp"

#include <fstream>
#include <iterator>

using namespace dfx::api::utils;

// https://stackoverflow.com/a/21802936
std::vector<uint8_t> dfx::api::utils::readFile(const std::filesystem::path& filename)
{
    // open the file:
    std::ifstream file(filename, std::ios::binary);

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    std::vector<uint8_t> vec;
    vec.reserve(fileSize);

    // read the data:
    vec.insert(vec.begin(), std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>());

    return vec;
}

bool dfx::api::utils::writeFile(const std::filesystem::path& filename, const std::vector<uint8_t>& data)
{
    std::ofstream out(filename, std::ios::out | std::ios::binary);
    if (!out) {
        return false;
    }
    out.write(reinterpret_cast<const char*>(data.data()), data.size());
    out.close();
    return true;
}
