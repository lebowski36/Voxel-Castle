#include "core/BinaryWriter.h"
#include <stdexcept>

namespace VoxelCastle {
namespace Core {

BinaryWriter::BinaryWriter(std::ostream& stream) : stream_(stream) {
}

size_t BinaryWriter::write(const void* data, size_t size) {
    std::streampos before = stream_.tellp();
    stream_.write(static_cast<const char*>(data), size);
    std::streampos after = stream_.tellp();
    return static_cast<size_t>(after - before);
}

void BinaryWriter::writeInt8(int8_t value) {
    if (write(&value, sizeof(value)) != sizeof(value)) {
        throw std::runtime_error("Failed to write int8");
    }
}

void BinaryWriter::writeUInt8(uint8_t value) {
    if (write(&value, sizeof(value)) != sizeof(value)) {
        throw std::runtime_error("Failed to write uint8");
    }
}

void BinaryWriter::writeInt16(int16_t value) {
    // Little-endian conversion
    uint8_t bytes[2] = {
        static_cast<uint8_t>(value & 0xFF),
        static_cast<uint8_t>((value >> 8) & 0xFF)
    };
    if (write(bytes, 2) != 2) {
        throw std::runtime_error("Failed to write int16");
    }
}

void BinaryWriter::writeUInt16(uint16_t value) {
    // Little-endian conversion
    uint8_t bytes[2] = {
        static_cast<uint8_t>(value & 0xFF),
        static_cast<uint8_t>((value >> 8) & 0xFF)
    };
    if (write(bytes, 2) != 2) {
        throw std::runtime_error("Failed to write uint16");
    }
}

void BinaryWriter::writeInt32(int32_t value) {
    // Little-endian conversion
    uint8_t bytes[4] = {
        static_cast<uint8_t>(value & 0xFF),
        static_cast<uint8_t>((value >> 8) & 0xFF),
        static_cast<uint8_t>((value >> 16) & 0xFF),
        static_cast<uint8_t>((value >> 24) & 0xFF)
    };
    if (write(bytes, 4) != 4) {
        throw std::runtime_error("Failed to write int32");
    }
}

void BinaryWriter::writeUInt32(uint32_t value) {
    // Little-endian conversion
    uint8_t bytes[4] = {
        static_cast<uint8_t>(value & 0xFF),
        static_cast<uint8_t>((value >> 8) & 0xFF),
        static_cast<uint8_t>((value >> 16) & 0xFF),
        static_cast<uint8_t>((value >> 24) & 0xFF)
    };
    if (write(bytes, 4) != 4) {
        throw std::runtime_error("Failed to write uint32");
    }
}

void BinaryWriter::writeInt64(int64_t value) {
    // Little-endian conversion
    uint8_t bytes[8] = {
        static_cast<uint8_t>(value & 0xFF),
        static_cast<uint8_t>((value >> 8) & 0xFF),
        static_cast<uint8_t>((value >> 16) & 0xFF),
        static_cast<uint8_t>((value >> 24) & 0xFF),
        static_cast<uint8_t>((value >> 32) & 0xFF),
        static_cast<uint8_t>((value >> 40) & 0xFF),
        static_cast<uint8_t>((value >> 48) & 0xFF),
        static_cast<uint8_t>((value >> 56) & 0xFF)
    };
    if (write(bytes, 8) != 8) {
        throw std::runtime_error("Failed to write int64");
    }
}

void BinaryWriter::writeUInt64(uint64_t value) {
    // Little-endian conversion
    uint8_t bytes[8] = {
        static_cast<uint8_t>(value & 0xFF),
        static_cast<uint8_t>((value >> 8) & 0xFF),
        static_cast<uint8_t>((value >> 16) & 0xFF),
        static_cast<uint8_t>((value >> 24) & 0xFF),
        static_cast<uint8_t>((value >> 32) & 0xFF),
        static_cast<uint8_t>((value >> 40) & 0xFF),
        static_cast<uint8_t>((value >> 48) & 0xFF),
        static_cast<uint8_t>((value >> 56) & 0xFF)
    };
    if (write(bytes, 8) != 8) {
        throw std::runtime_error("Failed to write uint64");
    }
}

void BinaryWriter::writeFloat(float value) {
    // Reinterpret the float as uint32_t
    uint32_t intValue = *reinterpret_cast<uint32_t*>(&value);
    writeUInt32(intValue);
}

void BinaryWriter::writeDouble(double value) {
    // Reinterpret the double as uint64_t
    uint64_t intValue = *reinterpret_cast<uint64_t*>(&value);
    writeUInt64(intValue);
}

void BinaryWriter::writeString(const std::string& value) {
    // Write length first
    writeUInt32(static_cast<uint32_t>(value.length()));
    
    // Write string data
    if (value.length() > 0) {
        if (write(value.data(), value.length()) != value.length()) {
            throw std::runtime_error("Failed to write string data");
        }
    }
}

bool BinaryWriter::writeCompressed(const void* data, size_t size) {
    // For now, just write uncompressed data
    // TODO: Implement actual compression (zlib/LZ4)
    
    // Write compression type (0 = none)
    writeUInt8(0);
    
    // Write compressed size (same as uncompressed for no compression)
    writeUInt32(static_cast<uint32_t>(size));
    
    // Write uncompressed size
    writeUInt32(static_cast<uint32_t>(size));
    
    // Write actual data
    return write(data, size) == size;
}

void BinaryWriter::flush() {
    stream_.flush();
}

std::streampos BinaryWriter::getPosition() const {
    return stream_.tellp();
}

bool BinaryWriter::seek(std::streampos position) {
    stream_.seekp(position);
    return !stream_.fail();
}

} // namespace Core
} // namespace VoxelCastle
