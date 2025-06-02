#pragma once

#include <cstdint>

namespace VoxelCastle {
namespace Core {

// Forward declarations
class BinaryReader;
class BinaryWriter;

/**
 * @brief Interface for objects that can be serialized to/from binary format
 * 
 * This interface provides a standard way to serialize game objects
 * for save files with versioning support.
 */
class Serializable {
public:
    virtual ~Serializable() = default;
    
    /**
     * @brief Serialize object to binary format
     * @param writer The binary writer to use
     */
    virtual void serialize(BinaryWriter& writer) const = 0;
    
    /**
     * @brief Deserialize object from binary format
     * @param reader The binary reader to use
     * @return True if deserialization successful
     */
    virtual bool deserialize(BinaryReader& reader) = 0;
    
    /**
     * @brief Get the version number of this object's data format
     * @return Version number (for compatibility checking)
     */
    virtual uint32_t getVersion() const = 0;
    
    /**
     * @brief Get the type identifier for this object
     * @return Type identifier string
     */
    virtual const char* getTypeId() const = 0;
};

} // namespace Core
} // namespace VoxelCastle
