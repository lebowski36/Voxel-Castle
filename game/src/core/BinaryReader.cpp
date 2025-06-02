#include "core/BinaryReader.h"
#include <algorithm>
#include <stdexcept>

namespace VoxelCastle {
namespace Core {

BinaryReader::BinaryReader(std::istream& stream) : stream_(stream) {
}

size_t BinaryReader::read(void* data, size_t size) {
    stream_.read(static_cast<char*>(data), size);
    return static_cast<size_t>(stream_.gcount());
}

int8_t BinaryReader::readInt8() {
    int8_t value;
    if (read(&value, sizeof(value)) != sizeof(value)) {
        throw std::runtime_error("Failed to read int8");
    }
    return value;
}

uint8_t BinaryReader::readUInt8() {
    uint8_t value;
    if (read(&value, sizeof(value)) != sizeof(value)) {
        throw std::runtime_error("Failed to read uint8");
    }
    return value;
}

int16_t BinaryReader::readInt16() {
    uint8_t bytes[2];
    if (read(bytes, 2) != 2) {
        throw std::runtime_error("Failed to read int16");
    }
    // Little-endian conversion
    return static_cast<int16_t>(bytes[0] | (bytes[1] << 8));
}

uint16_t BinaryReader::readUInt16() {
    uint8_t bytes[2];
    if (read(bytes, 2) != 2) {
        throw std::runtime_error("Failed to read uint16");
    }
    // Little-endian conversion
    return static_cast<uint16_t>(bytes[0] | (bytes[1] << 8));
}

int32_t BinaryReader::readInt32() {
    uint8_t bytes[4];
    if (read(bytes, 4) != 4) {
        throw std::runtime_error("Failed to read int32");
    }
    // Little-endian conversion
    return static_cast<int32_t>(
        bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24)
    );
}

uint32_t BinaryReader::readUInt32() {
    uint8_t bytes[4];
    if (read(bytes, 4) != 4) {
        throw std::runtime_error("Failed to read uint32");
    }
    // Little-endian conversion
    return static_cast<uint32_t>(
        bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24)
    );
}

int64_t BinaryReader::readInt64() {
    uint8_t bytes[8];
    if (read(bytes, 8) != 8) {
        throw std::runtime_error("Failed to read int64");
    }
    // Little-endian conversion
    return static_cast<int64_t>(
        static_cast<uint64_t>(bytes[0]) |
        (static_cast<uint64_t>(bytes[1]) << 8) |
        (static_cast<uint64_t>(bytes[2]) << 16) |
        (static_cast<uint64_t>(bytes[3]) << 24) |
        (static_cast<uint64_t>(bytes[4]) << 32) |
        (static_cast<uint64_t>(bytes[5]) << 40) |
        (static_cast<uint64_t>(bytes[6]) << 48) |
        (static_cast<uint64_t>(bytes[7]) << 56)
    );
}

uint64_t BinaryReader::readUInt64() {
    uint8_t bytes[8];
    if (read(bytes, 8) != 8) {
        throw std::runtime_error("Failed to read uint64");
    }
    // Little-endian conversion
    return static_cast<uint64_t>(
        static_cast<uint64_t>(bytes[0]) |
        (static_cast<uint64_t>(bytes[1]) << 8) |
        (static_cast<uint64_t>(bytes[2]) << 16) |
        (static_cast<uint64_t>(bytes[3]) << 24) |
        (static_cast<uint64_t>(bytes[4]) << 32) |
        (static_cast<uint64_t>(bytes[5]) << 40) |
        (static_cast<uint64_t>(bytes[6]) << 48) |
        (static_cast<uint64_t>(bytes[7]) << 56)
    );
}

float BinaryReader::readFloat() {
    uint32_t intValue = readUInt32();
    // Reinterpret the bits as a float
    return *reinterpret_cast<float*>(&intValue);
}

double BinaryReader::readDouble() {
    uint64_t intValue = readUInt64();
    // Reinterpret the bits as a double
    return *reinterpret_cast<double*>(&intValue);
}

std::string BinaryReader::readString() {
    uint32_t length = readUInt32();
    if (length == 0) {
        return std::string();
    }
    
    // Sanity check - prevent reading extremely large strings
    if (length > 1024 * 1024) { // 1MB limit
        throw std::runtime_error("String too large to read");
    }
    
    std::string result(length, '\0');
    size_t bytesRead = read(&result[0], length);
    if (bytesRead != length) {
        throw std::runtime_error("Failed to read complete string");
    }
    
    return result;
}

size_t BinaryReader::readCompressed(void* data, size_t maxSize) {
    // Read compression type
    uint8_t compressionType = readUInt8();
    
    // Read compressed size
    uint32_t compressedSize = readUInt32();
    
    // Read uncompressed size
    uint32_t uncompressedSize = readUInt32();
    
    if (uncompressedSize > maxSize) {
        throw std::runtime_error("Uncompressed data too large for buffer");
    }
    
    if (compressionType == 0) {
        // No compression - direct read
        size_t bytesRead = read(data, uncompressedSize);
        if (bytesRead != uncompressedSize) {
            throw std::runtime_error("Failed to read uncompressed data");
        }
        return bytesRead;
    } else {
        // TODO: Implement compression support (zlib/LZ4)
        throw std::runtime_error("Compression not yet implemented");
    }
}

bool BinaryReader::isEOF() const {
    return stream_.eof();
}

std::streampos BinaryReader::getPosition() const {
    return stream_.tellg();
}

bool BinaryReader::seek(std::streampos position) {
    stream_.seekg(position);
    return !stream_.fail();
}

} // namespace Core
} // namespace VoxelCastle
