#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

namespace VoxelCastle {
namespace Core {

/**
 * @brief Binary data reader for save file deserialization
 * 
 * Provides methods to read binary data in a platform-independent way
 * with proper endianness handling for save file compatibility.
 */
class BinaryReader {
public:
    /**
     * @brief Construct a new Binary Reader
     * @param stream Input stream to read from
     */
    explicit BinaryReader(std::istream& stream);

    /**
     * @brief Read raw data from stream
     * @param data Pointer to buffer to fill
     * @param size Number of bytes to read
     * @return Number of bytes actually read
     */
    size_t read(void* data, size_t size);

    /**
     * @brief Read a signed 8-bit integer
     * @return The read value
     */
    int8_t readInt8();

    /**
     * @brief Read an unsigned 8-bit integer
     * @return The read value
     */
    uint8_t readUInt8();

    /**
     * @brief Read a signed 16-bit integer (little-endian)
     * @return The read value
     */
    int16_t readInt16();

    /**
     * @brief Read an unsigned 16-bit integer (little-endian)
     * @return The read value
     */
    uint16_t readUInt16();

    /**
     * @brief Read a signed 32-bit integer (little-endian)
     * @return The read value
     */
    int32_t readInt32();

    /**
     * @brief Read an unsigned 32-bit integer (little-endian)
     * @return The read value
     */
    uint32_t readUInt32();

    /**
     * @brief Read a signed 64-bit integer (little-endian)
     * @return The read value
     */
    int64_t readInt64();

    /**
     * @brief Read an unsigned 64-bit integer (little-endian)
     * @return The read value
     */
    uint64_t readUInt64();

    /**
     * @brief Read a 32-bit float (little-endian)
     * @return The read value
     */
    float readFloat();

    /**
     * @brief Read a 64-bit double (little-endian)
     * @return The read value
     */
    double readDouble();

    /**
     * @brief Read a string (length-prefixed with uint32_t)
     * @return The read string
     */
    std::string readString();

    /**
     * @brief Read compressed data and decompress it
     * @param data Buffer to store decompressed data
     * @param maxSize Maximum size of buffer
     * @return Number of bytes decompressed
     */
    size_t readCompressed(void* data, size_t maxSize);

    /**
     * @brief Check if we've reached end of stream
     * @return True if at end of stream
     */
    bool isEOF() const;

    /**
     * @brief Get current position in stream
     * @return Current position
     */
    std::streampos getPosition() const;

    /**
     * @brief Seek to position in stream
     * @param position Position to seek to
     * @return True if seek successful
     */
    bool seek(std::streampos position);

private:
    std::istream& stream_;
};

} // namespace Core
} // namespace VoxelCastle
