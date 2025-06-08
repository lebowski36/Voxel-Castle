#include "world/climate_data.h"
#include <cstring>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace VoxelCastle {
namespace World {

// MonthlyClimate Implementation

MonthlyClimate::MonthlyClimate() {
    SetDefaults();
}

MonthlyClimate::MonthlyClimate(float temp, float precip, float humid, float wind) 
    : temperature(temp), precipitation(precip), humidity(humid), windSpeed(wind) {
    windDirection = 0.0f;
    primaryPrecip = PrecipitationType::RAIN;
}

bool MonthlyClimate::SerializeToBinary(std::vector<uint8_t>& buffer) const {
    size_t startSize = buffer.size();
    size_t totalSize = sizeof(float) * 5 + // temperature, precipitation, humidity, windSpeed, windDirection
                       sizeof(uint8_t); // primaryPrecip
    
    buffer.resize(startSize + totalSize);
    uint8_t* ptr = buffer.data() + startSize;
    
    // Write climate data
    std::memcpy(ptr, &temperature, sizeof(temperature)); ptr += sizeof(temperature);
    std::memcpy(ptr, &precipitation, sizeof(precipitation)); ptr += sizeof(precipitation);
    std::memcpy(ptr, &humidity, sizeof(humidity)); ptr += sizeof(humidity);
    std::memcpy(ptr, &windSpeed, sizeof(windSpeed)); ptr += sizeof(windSpeed);
    std::memcpy(ptr, &windDirection, sizeof(windDirection)); ptr += sizeof(windDirection);
    
    // Write precipitation type
    uint8_t precipValue = static_cast<uint8_t>(primaryPrecip);
    std::memcpy(ptr, &precipValue, sizeof(precipValue));
    
    return true;
}

bool MonthlyClimate::DeserializeFromBinary(const std::vector<uint8_t>& buffer, size_t& offset) {
    size_t requiredSize = sizeof(float) * 5 + sizeof(uint8_t);
    if (offset + requiredSize > buffer.size()) {
        return false;
    }
    
    const uint8_t* ptr = buffer.data() + offset;
    
    // Read climate data
    std::memcpy(&temperature, ptr, sizeof(temperature)); ptr += sizeof(temperature);
    std::memcpy(&precipitation, ptr, sizeof(precipitation)); ptr += sizeof(precipitation);
    std::memcpy(&humidity, ptr, sizeof(humidity)); ptr += sizeof(humidity);
    std::memcpy(&windSpeed, ptr, sizeof(windSpeed)); ptr += sizeof(windSpeed);
    std::memcpy(&windDirection, ptr, sizeof(windDirection)); ptr += sizeof(windDirection);
    
    // Read precipitation type
    uint8_t precipValue;
    std::memcpy(&precipValue, ptr, sizeof(precipValue));
    primaryPrecip = static_cast<PrecipitationType>(precipValue);
    
    offset += requiredSize;
    return IsValid();
}

bool MonthlyClimate::IsValid() const {
    return temperature >= -100.0f && temperature <= 100.0f &&
           precipitation >= 0.0f && precipitation <= 2000.0f &&
           humidity >= 0.0f && humidity <= 100.0f &&
           windSpeed >= 0.0f && windSpeed <= 200.0f &&
           static_cast<uint8_t>(primaryPrecip) <= 6;
}

void MonthlyClimate::SetDefaults() {
    temperature = 15.0f;
    precipitation = 50.0f;
    humidity = 60.0f;
    windSpeed = 5.0f;
    windDirection = 0.0f;
    primaryPrecip = PrecipitationType::RAIN;
}

std::string MonthlyClimate::ToString() const {
    std::ostringstream oss;
    oss << "MonthlyClimate{temp=" << temperature << "°C"
        << ", precip=" << precipitation << "mm"
        << ", humidity=" << humidity << "%"
        << ", wind=" << windSpeed << "m/s}";
    return oss.str();
}

// ClimateData Implementation

ClimateData::ClimateData() {
    SetDefaults();
}

bool ClimateData::SerializeToBinary(std::vector<uint8_t>& buffer) const {
    size_t startSize = buffer.size();
    
    // Calculate header size
    size_t headerSize = sizeof(uint8_t) * 2 + // climateZone, prevailingWind
                        sizeof(float) * 25 + // various climate parameters
                        sizeof(int) * 2; // frostDays, growingSeason
    
    // Calculate monthly data size
    size_t monthlySize = MONTHS_PER_YEAR * (sizeof(float) * 5 + sizeof(uint8_t));
    
    buffer.resize(startSize + headerSize + monthlySize);
    uint8_t* ptr = buffer.data() + startSize;
    
    // Write climate zone
    uint8_t zoneValue = static_cast<uint8_t>(climateZone);
    std::memcpy(ptr, &zoneValue, sizeof(zoneValue)); ptr += sizeof(zoneValue);
    
    // Write location data
    std::memcpy(ptr, &latitude, sizeof(latitude)); ptr += sizeof(latitude);
    std::memcpy(ptr, &longitude, sizeof(longitude)); ptr += sizeof(longitude);
    std::memcpy(ptr, &altitude, sizeof(altitude)); ptr += sizeof(altitude);
    
    // Write annual averages
    std::memcpy(ptr, &annualTemperature, sizeof(annualTemperature)); ptr += sizeof(annualTemperature);
    std::memcpy(ptr, &annualPrecipitation, sizeof(annualPrecipitation)); ptr += sizeof(annualPrecipitation);
    std::memcpy(ptr, &annualHumidity, sizeof(annualHumidity)); ptr += sizeof(annualHumidity);
    std::memcpy(ptr, &annualWindSpeed, sizeof(annualWindSpeed)); ptr += sizeof(annualWindSpeed);
    
    // Write temperature extremes
    std::memcpy(ptr, &minTemperature, sizeof(minTemperature)); ptr += sizeof(minTemperature);
    std::memcpy(ptr, &maxTemperature, sizeof(maxTemperature)); ptr += sizeof(maxTemperature);
    std::memcpy(ptr, &temperatureRange, sizeof(temperatureRange)); ptr += sizeof(temperatureRange);
    std::memcpy(ptr, &frostDays, sizeof(frostDays)); ptr += sizeof(frostDays);
    std::memcpy(ptr, &growingSeason, sizeof(growingSeason)); ptr += sizeof(growingSeason);
    
    // Write precipitation patterns
    std::memcpy(ptr, &rainySeasonStart, sizeof(rainySeasonStart)); ptr += sizeof(rainySeasonStart);
    std::memcpy(ptr, &rainySeasonLength, sizeof(rainySeasonLength)); ptr += sizeof(rainySeasonLength);
    std::memcpy(ptr, &droughtProbability, sizeof(droughtProbability)); ptr += sizeof(droughtProbability);
    std::memcpy(ptr, &floodProbability, sizeof(floodProbability)); ptr += sizeof(floodProbability);
    
    // Write wind and weather patterns
    uint8_t windValue = static_cast<uint8_t>(prevailingWind);
    std::memcpy(ptr, &windValue, sizeof(windValue)); ptr += sizeof(windValue);
    std::memcpy(ptr, &prevailingDirection, sizeof(prevailingDirection)); ptr += sizeof(prevailingDirection);
    std::memcpy(ptr, &stormFrequency, sizeof(stormFrequency)); ptr += sizeof(stormFrequency);
    std::memcpy(ptr, &microclimateFactor, sizeof(microclimateFactor)); ptr += sizeof(microclimateFactor);
    
    // Write solar and atmospheric data
    std::memcpy(ptr, &solarRadiation, sizeof(solarRadiation)); ptr += sizeof(solarRadiation);
    std::memcpy(ptr, &cloudCover, sizeof(cloudCover)); ptr += sizeof(cloudCover);
    std::memcpy(ptr, &uvIndex, sizeof(uvIndex)); ptr += sizeof(uvIndex);
    std::memcpy(ptr, &airPressure, sizeof(airPressure)); ptr += sizeof(airPressure);
    std::memcpy(ptr, &dewPoint, sizeof(dewPoint)); ptr += sizeof(dewPoint);
    std::memcpy(ptr, &evapotranspiration, sizeof(evapotranspiration)); ptr += sizeof(evapotranspiration);
    
    // Write climate stability
    std::memcpy(ptr, &climateStability, sizeof(climateStability)); ptr += sizeof(climateStability);
    std::memcpy(ptr, &yearlyVariation, sizeof(yearlyVariation)); ptr += sizeof(yearlyVariation);
    
    // Serialize monthly data
    for (uint32_t i = 0; i < MONTHS_PER_YEAR; ++i) {
        std::vector<uint8_t> monthBuffer;
        if (!monthlyData[i].SerializeToBinary(monthBuffer)) {
            return false;
        }
        std::memcpy(ptr, monthBuffer.data(), monthBuffer.size());
        ptr += monthBuffer.size();
    }
    
    return true;
}

bool ClimateData::DeserializeFromBinary(const std::vector<uint8_t>& buffer, size_t& offset) {
    size_t headerSize = sizeof(uint8_t) * 2 + sizeof(float) * 25 + sizeof(int) * 2;
    std::cout << "DEBUG: ClimateData - offset: " << offset << ", headerSize: " << headerSize 
              << ", buffer.size(): " << buffer.size() << ", needed: " << (offset + headerSize) << std::endl;
    
    if (offset + headerSize > buffer.size()) {
        std::cout << "DEBUG: ClimateData - Header size check failed!" << std::endl;
        return false;
    }
    
    const uint8_t* ptr = buffer.data() + offset;
    
    // Read climate zone
    uint8_t zoneValue;
    std::memcpy(&zoneValue, ptr, sizeof(zoneValue)); ptr += sizeof(zoneValue);
    climateZone = static_cast<ClimateZone>(zoneValue);
    
    // Read location data
    std::memcpy(&latitude, ptr, sizeof(latitude)); ptr += sizeof(latitude);
    std::memcpy(&longitude, ptr, sizeof(longitude)); ptr += sizeof(longitude);
    std::memcpy(&altitude, ptr, sizeof(altitude)); ptr += sizeof(altitude);
    
    // Read annual averages
    std::memcpy(&annualTemperature, ptr, sizeof(annualTemperature)); ptr += sizeof(annualTemperature);
    std::memcpy(&annualPrecipitation, ptr, sizeof(annualPrecipitation)); ptr += sizeof(annualPrecipitation);
    std::memcpy(&annualHumidity, ptr, sizeof(annualHumidity)); ptr += sizeof(annualHumidity);
    std::memcpy(&annualWindSpeed, ptr, sizeof(annualWindSpeed)); ptr += sizeof(annualWindSpeed);
    
    // Read temperature extremes
    std::memcpy(&minTemperature, ptr, sizeof(minTemperature)); ptr += sizeof(minTemperature);
    std::memcpy(&maxTemperature, ptr, sizeof(maxTemperature)); ptr += sizeof(maxTemperature);
    std::memcpy(&temperatureRange, ptr, sizeof(temperatureRange)); ptr += sizeof(temperatureRange);
    std::memcpy(&frostDays, ptr, sizeof(frostDays)); ptr += sizeof(frostDays);
    std::memcpy(&growingSeason, ptr, sizeof(growingSeason)); ptr += sizeof(growingSeason);
    
    // Read precipitation patterns
    std::memcpy(&rainySeasonStart, ptr, sizeof(rainySeasonStart)); ptr += sizeof(rainySeasonStart);
    std::memcpy(&rainySeasonLength, ptr, sizeof(rainySeasonLength)); ptr += sizeof(rainySeasonLength);
    std::memcpy(&droughtProbability, ptr, sizeof(droughtProbability)); ptr += sizeof(droughtProbability);
    std::memcpy(&floodProbability, ptr, sizeof(floodProbability)); ptr += sizeof(floodProbability);
    
    // Read wind and weather patterns
    uint8_t windValue;
    std::memcpy(&windValue, ptr, sizeof(windValue)); ptr += sizeof(windValue);
    prevailingWind = static_cast<WindPattern>(windValue);
    std::memcpy(&prevailingDirection, ptr, sizeof(prevailingDirection)); ptr += sizeof(prevailingDirection);
    std::memcpy(&stormFrequency, ptr, sizeof(stormFrequency)); ptr += sizeof(stormFrequency);
    std::memcpy(&microclimateFactor, ptr, sizeof(microclimateFactor)); ptr += sizeof(microclimateFactor);
    
    // Read solar and atmospheric data
    std::memcpy(&solarRadiation, ptr, sizeof(solarRadiation)); ptr += sizeof(solarRadiation);
    std::memcpy(&cloudCover, ptr, sizeof(cloudCover)); ptr += sizeof(cloudCover);
    std::memcpy(&uvIndex, ptr, sizeof(uvIndex)); ptr += sizeof(uvIndex);
    std::memcpy(&airPressure, ptr, sizeof(airPressure)); ptr += sizeof(airPressure);
    std::memcpy(&dewPoint, ptr, sizeof(dewPoint)); ptr += sizeof(dewPoint);
    std::memcpy(&evapotranspiration, ptr, sizeof(evapotranspiration)); ptr += sizeof(evapotranspiration);
    
    // Read climate stability
    std::memcpy(&climateStability, ptr, sizeof(climateStability)); ptr += sizeof(climateStability);
    std::memcpy(&yearlyVariation, ptr, sizeof(yearlyVariation)); ptr += sizeof(yearlyVariation);
    
    offset += headerSize;
    
    // Deserialize monthly data
    for (uint32_t i = 0; i < MONTHS_PER_YEAR; ++i) {
        if (!monthlyData[i].DeserializeFromBinary(buffer, offset)) {
            return false;
        }
    }
    
    return IsValid();
}

bool ClimateData::IsValid() const {
    if (static_cast<uint8_t>(climateZone) > 8 ||
        latitude < -90.0f || latitude > 90.0f ||
        longitude < -180.0f || longitude > 180.0f ||
        altitude < -500.0f || altitude > 10000.0f ||
        annualTemperature < -100.0f || annualTemperature > 100.0f ||
        annualPrecipitation < 0.0f || annualPrecipitation > 5000.0f ||
        annualHumidity < 0.0f || annualHumidity > 100.0f ||
        frostDays < 0 || frostDays > 365 ||
        growingSeason < 0 || growingSeason > 365) {
        return false;
    }
    
    // Validate all monthly data
    for (uint32_t i = 0; i < MONTHS_PER_YEAR; ++i) {
        if (!monthlyData[i].IsValid()) {
            return false;
        }
    }
    
    return true;
}

void ClimateData::SetDefaults() {
    climateZone = ClimateZone::TEMPERATE;
    latitude = 45.0f;
    longitude = 0.0f;
    altitude = 100.0f;
    
    annualTemperature = 15.0f;
    annualPrecipitation = 800.0f;
    annualHumidity = 65.0f;
    annualWindSpeed = 3.0f;
    
    minTemperature = -10.0f;
    maxTemperature = 35.0f;
    temperatureRange = 15.0f;
    frostDays = 30;
    growingSeason = 200;
    
    rainySeasonStart = 3.0f; // April
    rainySeasonLength = 3.0f; // 3 months
    droughtProbability = 0.1f;
    floodProbability = 0.05f;
    
    prevailingWind = WindPattern::GENTLE;
    prevailingDirection = 0.0f;
    stormFrequency = 5.0f;
    microclimateFactor = 0.0f;
    
    solarRadiation = 4.0f;
    cloudCover = 0.5f;
    uvIndex = 5.0f;
    airPressure = 1013.25f;
    dewPoint = 10.0f;
    evapotranspiration = 600.0f;
    
    climateStability = 0.8f;
    yearlyVariation = 0.2f;
    
    // Set default monthly data
    for (uint32_t i = 0; i < MONTHS_PER_YEAR; ++i) {
        monthlyData[i].SetDefaults();
        
        // Adjust temperature by season (simplified)
        float seasonalTemp = annualTemperature;
        if (i >= 3 && i <= 5) seasonalTemp += 5.0f;  // Spring
        else if (i >= 6 && i <= 8) seasonalTemp += 10.0f; // Summer
        else if (i >= 9 && i <= 11) seasonalTemp += 2.0f; // Autumn
        else seasonalTemp -= 8.0f; // Winter
        
        monthlyData[i].temperature = seasonalTemp;
        monthlyData[i].precipitation = annualPrecipitation / 12.0f;
    }
}

const MonthlyClimate& ClimateData::GetMonthlyData(int month) const {
    if (month < 0 || month >= MONTHS_PER_YEAR) {
        return monthlyData[0]; // Return January as fallback
    }
    return monthlyData[month];
}

void ClimateData::SetMonthlyData(int month, const MonthlyClimate& data) {
    if (month >= 0 && month < MONTHS_PER_YEAR) {
        monthlyData[month] = data;
    }
}

float ClimateData::GetTemperatureForMonth(int month) const {
    return GetMonthlyData(month).temperature;
}

float ClimateData::GetPrecipitationForMonth(int month) const {
    return GetMonthlyData(month).precipitation;
}

float ClimateData::GetSeasonalTemperatureVariation() const {
    float min = monthlyData[0].temperature;
    float max = monthlyData[0].temperature;
    
    for (uint32_t i = 1; i < MONTHS_PER_YEAR; ++i) {
        min = std::min(min, monthlyData[i].temperature);
        max = std::max(max, monthlyData[i].temperature);
    }
    
    return max - min;
}

bool ClimateData::IsGrowingSeason(int month) const {
    if (month < 0 || month >= MONTHS_PER_YEAR) {
        return false;
    }
    
    // Growing season is typically when temperature is above 5°C
    return monthlyData[month].temperature > 5.0f;
}

PrecipitationType ClimateData::GetLikelyPrecipitation(int month, float temperature) const {
    if (temperature < -5.0f) {
        return PrecipitationType::SNOW;
    } else if (temperature < 2.0f) {
        return PrecipitationType::SLEET;
    } else if (GetMonthlyData(month).precipitation < 10.0f) {
        return PrecipitationType::NONE;
    } else {
        return PrecipitationType::RAIN;
    }
}

float ClimateData::GetComfortIndex() const {
    // Simplified comfort index based on temperature and humidity
    float tempComfort = 1.0f - std::abs(annualTemperature - 22.0f) / 30.0f; // Ideal: 22°C
    float humidComfort = 1.0f - std::abs(annualHumidity - 50.0f) / 50.0f;   // Ideal: 50%
    
    return std::max(0.0f, std::min(1.0f, (tempComfort + humidComfort) * 0.5f));
}

float ClimateData::GetAridity() const {
    // Aridity index: higher values = more arid
    if (annualPrecipitation <= 0.0f) {
        return 1.0f;
    }
    
    // Simple approximation: evapotranspiration / precipitation
    return std::min(1.0f, evapotranspiration / annualPrecipitation);
}

std::string ClimateData::ToString() const {
    std::ostringstream oss;
    oss << "ClimateData{zone=" << ClimateZoneToString(climateZone)
        << ", temp=" << annualTemperature << "°C"
        << ", precip=" << annualPrecipitation << "mm"
        << ", lat=" << latitude << "°}";
    return oss.str();
}

size_t ClimateData::GetSerializedSize() const {
    size_t size = sizeof(uint8_t) * 2 + sizeof(float) * 25 + sizeof(int) * 2;
    size += MONTHS_PER_YEAR * (sizeof(float) * 5 + sizeof(uint8_t));
    return size;
}

// Utility Functions

const char* WindPatternToString(WindPattern pattern) {
    switch (pattern) {
        case WindPattern::CALM: return "Calm";
        case WindPattern::GENTLE: return "Gentle";
        case WindPattern::MODERATE: return "Moderate";
        case WindPattern::STRONG: return "Strong";
        case WindPattern::STORM: return "Storm";
        default: return "Unknown";
    }
}

const char* PrecipitationTypeToString(PrecipitationType type) {
    switch (type) {
        case PrecipitationType::NONE: return "None";
        case PrecipitationType::RAIN: return "Rain";
        case PrecipitationType::SNOW: return "Snow";
        case PrecipitationType::SLEET: return "Sleet";
        case PrecipitationType::HAIL: return "Hail";
        case PrecipitationType::FOG: return "Fog";
        case PrecipitationType::MIST: return "Mist";
        default: return "Unknown";
    }
}

const char* ClimateZoneToString(ClimateZone zone) {
    switch (zone) {
        case ClimateZone::ARCTIC: return "Arctic";
        case ClimateZone::SUBARCTIC: return "Subarctic";
        case ClimateZone::TEMPERATE: return "Temperate";
        case ClimateZone::SUBTROPICAL: return "Subtropical";
        case ClimateZone::TROPICAL: return "Tropical";
        case ClimateZone::DESERT: return "Desert";
        case ClimateZone::MEDITERRANEAN: return "Mediterranean";
        case ClimateZone::CONTINENTAL: return "Continental";
        case ClimateZone::OCEANIC: return "Oceanic";
        default: return "Unknown";
    }
}

WindPattern StringToWindPattern(const std::string& str) {
    if (str == "Calm") return WindPattern::CALM;
    if (str == "Gentle") return WindPattern::GENTLE;
    if (str == "Moderate") return WindPattern::MODERATE;
    if (str == "Strong") return WindPattern::STRONG;
    if (str == "Storm") return WindPattern::STORM;
    return WindPattern::GENTLE; // Default
}

PrecipitationType StringToPrecipitationType(const std::string& str) {
    if (str == "None") return PrecipitationType::NONE;
    if (str == "Rain") return PrecipitationType::RAIN;
    if (str == "Snow") return PrecipitationType::SNOW;
    if (str == "Sleet") return PrecipitationType::SLEET;
    if (str == "Hail") return PrecipitationType::HAIL;
    if (str == "Fog") return PrecipitationType::FOG;
    if (str == "Mist") return PrecipitationType::MIST;
    return PrecipitationType::RAIN; // Default
}

ClimateZone StringToClimateZone(const std::string& str) {
    if (str == "Arctic") return ClimateZone::ARCTIC;
    if (str == "Subarctic") return ClimateZone::SUBARCTIC;
    if (str == "Temperate") return ClimateZone::TEMPERATE;
    if (str == "Subtropical") return ClimateZone::SUBTROPICAL;
    if (str == "Tropical") return ClimateZone::TROPICAL;
    if (str == "Desert") return ClimateZone::DESERT;
    if (str == "Mediterranean") return ClimateZone::MEDITERRANEAN;
    if (str == "Continental") return ClimateZone::CONTINENTAL;
    if (str == "Oceanic") return ClimateZone::OCEANIC;
    return ClimateZone::TEMPERATE; // Default
}

// Climate calculation utilities

float CalculateHeatIndex(float temperature, float humidity) {
    // Simplified heat index calculation (Rothfusz equation approximation)
    if (temperature < 27.0f) {
        return temperature; // Heat index only applies at high temperatures
    }
    
    float T = temperature;
    float RH = humidity;
    
    float HI = -8.78469475556f + 1.61139411f * T + 2.33854883889f * RH 
               - 0.14611605f * T * RH - 0.012308094f * T * T 
               - 0.0164248277778f * RH * RH + 0.002211732f * T * T * RH 
               + 0.00072546f * T * RH * RH - 0.000003582f * T * T * RH * RH;
    
    return HI;
}

float CalculateWindChill(float temperature, float windSpeed) {
    // Wind chill only applies at low temperatures
    if (temperature > 10.0f || windSpeed < 1.3f) {
        return temperature;
    }
    
    float T = temperature;
    float V = windSpeed * 3.6f; // Convert m/s to km/h
    
    float WC = 13.12f + 0.6215f * T - 11.37f * std::pow(V, 0.16f) + 0.3965f * T * std::pow(V, 0.16f);
    
    return WC;
}

float CalculateEvapotranspiration(float temperature, float humidity, float windSpeed, float solarRadiation) {
    // Simplified Penman-Monteith equation approximation
    // Returns daily evapotranspiration in mm/day
    
    float T = temperature;
    float RH = humidity / 100.0f;
    float u = windSpeed;
    float Rs = solarRadiation;
    
    // Saturation vapor pressure
    float es = 0.6108f * std::exp(17.27f * T / (T + 237.3f));
    
    // Actual vapor pressure
    float ea = es * RH;
    
    // Vapor pressure deficit
    float vpd = es - ea;
    
    // Simplified calculation
    float ET = 0.0023f * (T + 17.8f) * std::sqrt(vpd) * (Rs + 50.0f) * (1.0f + 0.01f * u);
    
    return std::max(0.0f, ET);
}

ClimateZone DetermineClimateZone(float temperature, float precipitation, float latitude) {
    float absLat = std::abs(latitude);
    
    // Arctic/Subarctic zones
    if (absLat > 66.5f || temperature < -10.0f) {
        return ClimateZone::ARCTIC;
    } else if (absLat > 55.0f || temperature < 0.0f) {
        return ClimateZone::SUBARCTIC;
    }
    
    // Tropical zones
    if (absLat < 23.5f && temperature > 25.0f) {
        return ClimateZone::TROPICAL;
    } else if (absLat < 35.0f && temperature > 20.0f) {
        return ClimateZone::SUBTROPICAL;
    }
    
    // Arid zones
    if (precipitation < 250.0f) {
        return ClimateZone::DESERT;
    }
    
    // Mediterranean (dry summer, wet winter pattern would need seasonal data)
    if (absLat > 30.0f && absLat < 45.0f && precipitation < 800.0f) {
        return ClimateZone::MEDITERRANEAN;
    }
    
    // Continental vs Oceanic (would need continentality data)
    if (absLat > 40.0f) {
        return ClimateZone::CONTINENTAL;
    }
    
    // Default to temperate
    return ClimateZone::TEMPERATE;
}

} // namespace World
} // namespace VoxelCastle
