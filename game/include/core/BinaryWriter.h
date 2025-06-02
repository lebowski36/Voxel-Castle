#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

namespace VoxelCastle {
namespace Core {

/**
 * @brief Binary data writer for save file serialization
 * 
 * Provides methods to write binary data in a platform-independent way
 * with proper endianness handling for save file compatibility.
 */
class BinaryWriter {
public:
    /**
     * @brief Construct a new Binary Writer
     * @param stream Output stream to write to
     */
    explicit BinaryWriter(std::ostream& stream);

    /**
     * @brief Write raw data to stream
     * @param data Pointer to data to write
     * @param size Number of bytes to write
     * @return Number of bytes actually written
     */
    size_t write(const void* data, size_t size);

    /**
     * @brief Write a signed 8-bit integer
     * @param value The value to write
     */
    void writeInt8(int8_t value);

    /**
     * @brief Write an unsigned 8-bit integer
     * @param value The value to write
     */
    void writeUInt8(uint8_t value);

    /**
     * @brief Write a signed 16-bit integer (little-endian)
     * @param value The value to write
     */
    void writeInt16(int16_t value);

    /**
     * @brief Write an unsigned 16-bit integer (little-endian)
     * @param value The value to write
     */
    void writeUInt16(uint16_t value);

    /**
     * @brief Write a signed 32-bit integer (little-endian)
     * @param value The value to write
     */
    void writeInt32(int32_t value);

    /**
     * @brief Write an unsigned 32-bit integer (little-endian)
     * @param value The value to write
     */
    void writeUInt32(uint32_t value);

    /**
     * @brief Write a signed 64-bit integer (little-endian)
     * @param value The value to write
     */
    void writeInt64(int64_t value);

    /**
     * @brief Write an unsigned 64-bit integer (little-endian)
     * @param value The value to write
     */
    void writeUInt64(uint64_t value);

    /**
     * @brief Write a 32-bit float (little-endian)
     * @param value The value to write
     */
    void writeFloat(float value);

    /**
     * @brief Write a 64-bit double (little-endian)
     * @param value The value to write
     */
    void writeDouble(double value);

    /**
     * @brief Write a string (length-prefixed with uint32_t)
     * @param value The string to write
     */
    void writeString(const std::string& value);

    /**
     * @brief Compress data and write it
     * @param data Pointer to data to compress and write
     * @param size Size of data to compress
     * @return True if compression and write successful
     */
    bool writeCompressed(const void* data, size_t size);

    /**
     * @brief Flush the output stream
     */
    void flush();

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
    std::ostream& stream_;
};

} // namespace Core
} // namespace VoxelCastle
