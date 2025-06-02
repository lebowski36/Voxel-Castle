#include "core/BinaryReader.h"
#include "core/BinaryWriter.h"
#include "core/Serializable.h"
#include <iostream>
#include <fstream>
#include <cstdio>

namespace VoxelCastle {
namespace Core {

// Simple test class to verify serialization works
class TestSerializable : public Serializable {
private:
    int32_t testInt_;
    float testFloat_;
    std::string testString_;
    
public:
    TestSerializable() : testInt_(0), testFloat_(0.0f) {}
    
    TestSerializable(int32_t i, float f, const std::string& s) 
        : testInt_(i), testFloat_(f), testString_(s) {}
    
    void serialize(BinaryWriter& writer) const override {
        writer.writeInt32(testInt_);
        writer.writeFloat(testFloat_);
        writer.writeString(testString_);
    }
    
    bool deserialize(BinaryReader& reader) override {
        testInt_ = reader.readInt32();
        testFloat_ = reader.readFloat();
        testString_ = reader.readString();
        return true;
    }
    
    uint32_t getVersion() const override {
        return 1;
    }
    
    const char* getTypeId() const override {
        return "TestSerializable";
    }
    
    // Getters for testing
    int32_t getInt() const { return testInt_; }
    float getFloat() const { return testFloat_; }
    const std::string& getString() const { return testString_; }
    
    // Equality check for testing
    bool equals(const TestSerializable& other) const {
        return testInt_ == other.testInt_ && 
               testFloat_ == other.testFloat_ && 
               testString_ == other.testString_;
    }
};

bool testSerialization() {
    const std::string testFile = "test_serialization.bin";
    
    // Create test data
    TestSerializable original(42, 3.14159f, "Hello, Voxel Castle!");
    
    try {
        // Test writing
        {
            std::ofstream file(testFile, std::ios::binary);
            if (!file) {
                std::cerr << "[SerializationTest] Failed to create test file for writing" << std::endl;
                return false;
            }
            
            BinaryWriter writer(file);
            original.serialize(writer);
            std::cout << "[SerializationTest] Successfully wrote test data" << std::endl;
        }
        
        // Test reading
        TestSerializable loaded;
        {
            std::ifstream file(testFile, std::ios::binary);
            if (!file) {
                std::cerr << "[SerializationTest] Failed to open test file for reading" << std::endl;
                return false;
            }
            
            BinaryReader reader(file);
            loaded.deserialize(reader);
            std::cout << "[SerializationTest] Successfully read test data" << std::endl;
        }
        
        // Verify data integrity
        if (loaded.equals(original)) {
            std::cout << "[SerializationTest] ✓ Serialization test PASSED!" << std::endl;
            std::cout << "  Original: int=" << original.getInt() 
                      << ", float=" << original.getFloat() 
                      << ", string='" << original.getString() << "'" << std::endl;
            std::cout << "  Loaded:   int=" << loaded.getInt() 
                      << ", float=" << loaded.getFloat() 
                      << ", string='" << loaded.getString() << "'" << std::endl;
        } else {
            std::cerr << "[SerializationTest] ✗ Data mismatch after serialization!" << std::endl;
            std::cerr << "  Original: int=" << original.getInt() 
                      << ", float=" << original.getFloat() 
                      << ", string='" << original.getString() << "'" << std::endl;
            std::cerr << "  Loaded:   int=" << loaded.getInt() 
                      << ", float=" << loaded.getFloat() 
                      << ", string='" << loaded.getString() << "'" << std::endl;
            return false;
        }
        
        // Clean up test file
        std::remove(testFile.c_str());
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[SerializationTest] Exception: " << e.what() << std::endl;
        std::remove(testFile.c_str()); // Clean up on error
        return false;
    }
}

} // namespace Core
} // namespace VoxelCastle
