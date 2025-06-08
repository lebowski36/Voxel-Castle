#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace VoxelCastle {
namespace World {

/**
 * Wind patterns and directions
 */
enum class WindPattern : uint8_t {
    CALM = 0,           // Very low wind
    GENTLE = 1,         // Light breeze
    MODERATE = 2,       // Moderate wind
    STRONG = 3,         // Strong wind
    STORM = 4           // Storm-force winds
};

/**
 * Precipitation types
 */
enum class PrecipitationType : uint8_t {
    NONE = 0,
    RAIN = 1,
    SNOW = 2,
    SLEET = 3,
    HAIL = 4,
    FOG = 5,
    MIST = 6
};

/**
 * Climate zones
 */
enum class ClimateZone : uint8_t {
    ARCTIC = 0,         // Below -10°C average
    SUBARCTIC = 1,      // -10°C to 0°C average
    TEMPERATE = 2,      // 0°C to 20°C average
    SUBTROPICAL = 3,    // 20°C to 25°C average
    TROPICAL = 4,       // Above 25°C average
    DESERT = 5,         // Arid climate
    MEDITERRANEAN = 6,  // Dry summer, wet winter
    CONTINENTAL = 7,    // Large temperature variation
    OCEANIC = 8         // Maritime influence
};

/**
 * Monthly climate data
 */
struct MonthlyClimate {
    float temperature;          // Average temperature (°C)
    float precipitation;        // Total precipitation (mm)
    float humidity;             // Relative humidity (0-100%)
    float windSpeed;            // Average wind speed (m/s)
    float windDirection;        // Prevailing wind direction (radians)
    PrecipitationType primaryPrecip; // Most common precipitation type
    
    MonthlyClimate();
    MonthlyClimate(float temp, float precip, float humid, float wind);
    
    // Serialization
    bool SerializeToBinary(std::vector<uint8_t>& buffer) const;
    bool DeserializeFromBinary(const std::vector<uint8_t>& buffer, size_t& offset);
    
    // Validation
    bool IsValid() const;
    void SetDefaults();
    
    // Utility
    std::string ToString() const;
};

/**
 * Comprehensive climate data for a region
 * Includes temperature, precipitation, wind patterns, and seasonal variations
 */
struct ClimateData {
    static constexpr uint32_t MONTHS_PER_YEAR = 12;
    static constexpr uint32_t CURRENT_VERSION = 1;
    
    // Basic climate classification
    ClimateZone climateZone;    // Primary climate zone
    float latitude;             // Latitude for solar calculations (-90 to +90)
    float longitude;            // Longitude for time zone calculations
    float altitude;             // Altitude above sea level (meters)
    
    // Annual averages
    float annualTemperature;    // Average annual temperature (°C)
    float annualPrecipitation;  // Total annual precipitation (mm)
    float annualHumidity;       // Average annual humidity (0-100%)
    float annualWindSpeed;      // Average annual wind speed (m/s)
    
    // Temperature extremes
    float minTemperature;       // Record minimum temperature (°C)
    float maxTemperature;       // Record maximum temperature (°C)
    float temperatureRange;     // Daily temperature variation (°C)
    int frostDays;              // Days below 0°C per year
    int growingSeason;          // Growing season length (days)
    
    // Precipitation patterns
    float rainySeasonStart;     // Start of rainy season (month 0-11.99)
    float rainySeasonLength;    // Length of rainy season (months)
    float droughtProbability;   // Probability of drought (0.0-1.0)
    float floodProbability;     // Probability of flooding (0.0-1.0)
    
    // Wind and weather patterns
    WindPattern prevailingWind; // Dominant wind pattern
    float prevailingDirection;  // Prevailing wind direction (radians)
    float stormFrequency;       // Number of storms per year
    float microclimateFactor;   // Local microclimate modifier (-1.0 to +1.0)
    
    // Monthly detailed data
    MonthlyClimate monthlyData[MONTHS_PER_YEAR];
    
    // Solar and daylight
    float solarRadiation;       // Average solar radiation (kWh/m²/day)
    float cloudCover;           // Average cloud cover (0.0-1.0)
    float uvIndex;              // UV index (0-11+)
    
    // Atmospheric pressure and humidity
    float airPressure;          // Average air pressure (hPa)
    float dewPoint;             // Average dew point (°C)
    float evapotranspiration;   // Potential evapotranspiration (mm/year)
    
    // Climate stability and change
    float climateStability;     // Climate stability index (0.0-1.0)
    float yearlyVariation;      // Year-to-year variation factor (0.0-1.0)
    
    // Constructors
    ClimateData();
    
    // Serialization methods
    bool SerializeToBinary(std::vector<uint8_t>& buffer) const;
    bool DeserializeFromBinary(const std::vector<uint8_t>& buffer, size_t& offset);
    
    // Validation methods
    bool IsValid() const;
    void SetDefaults();
    
    // Monthly data access
    const MonthlyClimate& GetMonthlyData(int month) const; // month 0-11
    void SetMonthlyData(int month, const MonthlyClimate& data);
    
    // Climate calculations
    float GetTemperatureForMonth(int month) const;
    float GetPrecipitationForMonth(int month) const;
    float GetSeasonalTemperatureVariation() const;
    bool IsGrowingSeason(int month) const;
    
    // Weather prediction helpers
    PrecipitationType GetLikelyPrecipitation(int month, float temperature) const;
    float GetComfortIndex() const; // Human comfort index (0.0-1.0)
    float GetAridity() const; // Aridity index for vegetation
    
    // Utility methods
    std::string ToString() const;
    size_t GetSerializedSize() const;
};

// Utility functions
const char* WindPatternToString(WindPattern pattern);
const char* PrecipitationTypeToString(PrecipitationType type);
const char* ClimateZoneToString(ClimateZone zone);
WindPattern StringToWindPattern(const std::string& str);
PrecipitationType StringToPrecipitationType(const std::string& str);
ClimateZone StringToClimateZone(const std::string& str);

// Climate calculation utilities
float CalculateHeatIndex(float temperature, float humidity);
float CalculateWindChill(float temperature, float windSpeed);
float CalculateEvapotranspiration(float temperature, float humidity, float windSpeed, float solarRadiation);
ClimateZone DetermineClimateZone(float temperature, float precipitation, float latitude);

} // namespace World
} // namespace VoxelCastle
