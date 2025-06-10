# Subtask 5: UI Components Implementation

**Priority:** User Experience
**Estimated Time:** 4-5 days  
**Dependencies:** SeedWorldGenerator Integration

## Overview

Implement the user interface components for geological world configuration and real-time progress visualization, providing players with intuitive control over the geological realism system.

## Technical Requirements

### Core UI Components

#### World Configuration UI
```cpp
class WorldConfigurationUI {
private:
    GeologicalConfig geoConfig;
    bool useGeologicalRealism = true;
    WorldSeed customSeed;
    
public:
    // Quality preset selection
    void renderQualityPresets();
    void onQualityPresetChanged(GeologicalPreset preset);
    
    // Custom seed input
    void renderSeedInput();
    void onSeedChanged(const std::string& seedText);
    
    // Advanced geological options (for CUSTOM preset)
    void renderAdvancedOptions();
    
    // Performance preview
    void renderPerformancePreview();
};
```

#### Geological Progress UI
```cpp
class GeologicalProgressUI {
private:
    PhaseInfo currentPhase;
    PerformanceMetrics metrics;
    std::chrono::steady_clock::time_point startTime;
    
public:
    // Real-time progress display
    void renderProgressDisplay();
    void updateProgress(const PhaseInfo& info);
    
    // Performance monitoring
    void renderPerformanceMetrics();
    void renderMemoryUsage();
    
    // Visual timeline
    void renderGeologicalTimeline();
    void renderPhaseTransitions();
};
```

## Implementation Steps

### Phase 1: World Configuration UI (Day 1-2)

#### Quality Preset Selection Interface
```cpp
void WorldConfigurationUI::renderQualityPresets() {
    ImGui::Text("Geological Realism Quality");
    ImGui::Separator();
    
    // Quality preset radio buttons with descriptions
    for (auto preset : {PERFORMANCE, BALANCED, QUALITY, ULTRA_REALISM}) {
        QualityPreview preview = getQualityPreview(preset);
        
        if (ImGui::RadioButton(preview.name.c_str(), geoConfig.preset == preset)) {
            geoConfig.preset = preset;
            onQualityPresetChanged(preset);
        }
        
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                          "(%s, %s)", 
                          preview.expectedTime.c_str(),
                          preview.memoryUsage.c_str());
        
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", preview.description.c_str());
            ImGui::Text("Features: %s", preview.visualFeatures.c_str());
            ImGui::EndTooltip();
        }
    }
}
```

#### Custom Seed Input
```cpp
void WorldConfigurationUI::renderSeedInput() {
    ImGui::Text("World Seed");
    ImGui::Separator();
    
    // Seed input field
    char seedBuffer[256];
    strncpy(seedBuffer, customSeed.seedText.c_str(), sizeof(seedBuffer));
    
    if (ImGui::InputText("##SeedInput", seedBuffer, sizeof(seedBuffer))) {
        customSeed.seedText = std::string(seedBuffer);
        customSeed.numericSeed = hashSeed(customSeed.seedText);
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Random")) {
        customSeed = generateRandomSeed();
    }
    
    // Display numeric seed
    ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), 
                      "Numeric: %llu", customSeed.numericSeed);
}
```

#### Performance Preview Display
```cpp
void WorldConfigurationUI::renderPerformancePreview() {
    QualityPreview preview = getQualityPreview(geoConfig.preset);
    
    ImGui::Text("Performance Preview");
    ImGui::Separator();
    
    // Expected generation time
    ImGui::Text("Generation Time: %s", preview.expectedTime.c_str());
    
    // Memory usage estimate
    ImGui::Text("Memory Usage: %s", preview.memoryUsage.c_str());
    
    // Visual features included
    ImGui::Text("Geological Features:");
    ImGui::BulletText("%s", preview.visualFeatures.c_str());
    
    // Hardware recommendation
    if (geoConfig.preset >= QUALITY) {
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f), 
                          "Recommended: 16GB RAM, Modern CPU");
    }
}
```

### Phase 2: Geological Progress UI (Day 3-4)

#### Real-Time Progress Display
```cpp
void GeologicalProgressUI::renderProgressDisplay() {
    ImGui::Text("Geological Simulation Progress");
    ImGui::Separator();
    
    // Current phase indicator
    const char* phaseNames[] = {"Tectonic", "Erosion", "Detail"};
    ImGui::Text("Phase: %s", phaseNames[(int)currentPhase.currentPhase]);
    
    // Current process within phase
    ImGui::Text("Process: %s", currentPhase.currentProcess.c_str());
    
    // Phase progress bar
    ImGui::ProgressBar(currentPhase.phaseProgress, 
                      ImVec2(-1.0f, 0.0f), 
                      "Phase Progress");
    
    // Overall progress bar
    ImGui::ProgressBar(currentPhase.totalProgress, 
                      ImVec2(-1.0f, 0.0f), 
                      "Overall Progress");
    
    // Time remaining
    int minutes = (int)(currentPhase.timeRemaining / 60.0f);
    int seconds = (int)(currentPhase.timeRemaining) % 60;
    ImGui::Text("Time Remaining: %d:%02d", minutes, seconds);
}
```

#### Performance Metrics Display
```cpp
void GeologicalProgressUI::renderPerformanceMetrics() {
    ImGui::Text("Performance Metrics");
    ImGui::Separator();
    
    // Memory usage with bar
    float memoryPercent = metrics.memoryUsage / metrics.totalMemory;
    ImGui::ProgressBar(memoryPercent, ImVec2(-1.0f, 0.0f));
    ImGui::SameLine();
    ImGui::Text("Memory: %.1fGB / %.1fGB", 
               metrics.memoryUsage / 1024.0f / 1024.0f / 1024.0f,
               metrics.totalMemory / 1024.0f / 1024.0f / 1024.0f);
    
    // Generation rate
    ImGui::Text("Processing Rate: %.1fM samples/sec", 
               metrics.processingRate / 1000000.0f);
    
    // Chunk generation rate (if applicable)
    if (metrics.chunkGenerationRate > 0) {
        ImGui::Text("Chunk Generation: %.1f chunks/sec", 
                   metrics.chunkGenerationRate);
    }
}
```

#### Visual Geological Timeline
```cpp
void GeologicalProgressUI::renderGeologicalTimeline() {
    ImGui::Text("Geological Timeline");
    ImGui::Separator();
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    
    // Timeline background
    drawList->AddRectFilled(canvas_pos, 
                           ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + 50),
                           IM_COL32(50, 50, 50, 255));
    
    // Phase divisions
    float phaseWidth = canvas_size.x / 3.0f;
    for (int i = 0; i < 3; i++) {
        float x = canvas_pos.x + i * phaseWidth;
        
        // Phase color based on completion
        ImU32 color = (i < (int)currentPhase.currentPhase) ? 
                      IM_COL32(0, 255, 0, 128) : 
                      (i == (int)currentPhase.currentPhase) ? 
                      IM_COL32(255, 255, 0, 128) : 
                      IM_COL32(128, 128, 128, 128);
        
        drawList->AddRectFilled(ImVec2(x, canvas_pos.y),
                               ImVec2(x + phaseWidth, canvas_pos.y + 50),
                               color);
        
        // Phase labels
        const char* phaseNames[] = {"Tectonics", "Erosion", "Detail"};
        drawList->AddText(ImVec2(x + 10, canvas_pos.y + 15), 
                         IM_COL32(255, 255, 255, 255), 
                         phaseNames[i]);
    }
    
    // Current progress indicator
    float progressX = canvas_pos.x + currentPhase.totalProgress * canvas_size.x;
    drawList->AddLine(ImVec2(progressX, canvas_pos.y),
                     ImVec2(progressX, canvas_pos.y + 50),
                     IM_COL32(255, 0, 0, 255), 3.0f);
    
    ImGui::Dummy(ImVec2(canvas_size.x, 60)); // Reserve space
}
```

### Phase 3: Advanced Options UI (Day 5)

#### Custom Geological Settings (for CUSTOM preset)
```cpp
void WorldConfigurationUI::renderAdvancedOptions() {
    if (geoConfig.preset != CUSTOM) return;
    
    ImGui::Text("Advanced Geological Options");
    ImGui::Separator();
    
    // Process toggles
    ImGui::Checkbox("Chemical Weathering", &geoConfig.custom.enableChemicalWeathering);
    ImGui::Checkbox("Joint Systems", &geoConfig.custom.enableJointSystems);
    ImGui::Checkbox("Cave Systems", &geoConfig.custom.enableCaveSystems);
    ImGui::Checkbox("Glacial Features", &geoConfig.custom.enableGlacialFeatures);
    
    // Time scale multiplier
    ImGui::SliderFloat("Time Scale Multiplier", 
                      &geoConfig.custom.timeScaleMultiplier, 
                      0.1f, 10.0f, "%.1fx");
    
    // Simulation detail level
    ImGui::SliderInt("Simulation Detail", 
                    &geoConfig.custom.simulationDetailLevel, 
                    100, 10000, "%d steps");
    
    // Performance warning
    if (geoConfig.custom.simulationDetailLevel > 5000) {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), 
                          "Warning: High detail may take very long to generate");
    }
}
```

## Integration with Existing UI System

### WorldSimulationUI Integration
```cpp
// Modify existing WorldSimulationUI to show geological progress
void WorldSimulationUI::render() {
    if (worldGenerator->isUsingGeologicalRealism()) {
        // Show geological progress instead of legacy progress
        geologicalProgressUI.render();
    } else {
        // Show legacy tectonic progress
        renderLegacyProgress();
    }
}
```

### WorldConfigurationUI Integration
```cpp
// Add geological options to world creation screen
void WorldConfigurationUI::render() {
    // Existing world options...
    renderBasicWorldOptions();
    
    ImGui::Separator();
    
    // NEW: Geological realism section
    if (ImGui::CollapsingHeader("Geological Realism", ImGuiTreeNodeFlags_DefaultOpen)) {
        renderGeologicalOptions();
    }
    
    // Existing advanced options...
    renderAdvancedWorldOptions();
}
```

## Files to Create/Modify

### New Files
- `engine/src/ui/GeologicalProgressUI.cpp`
- `engine/include/ui/GeologicalProgressUI.h`
- `engine/src/ui/components/QualityPresetSelector.cpp`
- `engine/src/ui/components/SeedInputComponent.cpp`
- `engine/src/ui/components/GeologicalTimeline.cpp`

### Modified Files
- `engine/src/ui/WorldConfigurationUI.cpp` - Add geological options
- `engine/src/ui/WorldSimulationUI.cpp` - Integrate progress display
- `engine/include/ui/WorldConfigurationUI.h` - Interface updates

### Supporting Files
- `engine/include/world/GeologicalConfig.h` - Configuration structures
- `assets/ui/geological_icons.png` - UI icon assets
- `assets/ui/phase_timeline.png` - Timeline visualization assets

## User Experience Workflow

### World Creation Flow
1. **Basic Options:** Name, size, game mode
2. **Geological Quality:** Select preset with time/memory preview
3. **Custom Seed:** Optional seed input with random generation
4. **Advanced Options:** Custom geological settings (if CUSTOM preset)
5. **Create World:** Start geological simulation with progress display

### Progress Monitoring Experience
1. **Phase Visualization:** Clear indication of current geological phase
2. **Process Details:** Specific geological process currently running
3. **Time Estimates:** Accurate remaining time predictions
4. **Performance Metrics:** Memory usage and processing rate
5. **Pause/Resume:** Ability to pause simulation and resume later

## Testing Strategy

### UI Functionality Tests
- Quality preset selection and configuration
- Custom seed input and validation
- Advanced options toggle behavior
- Progress display accuracy

### User Experience Tests
- World creation workflow completeness
- Progress visualization clarity
- Performance preview accuracy
- Responsive UI during simulation

### Integration Tests
- UI integration with geological simulation
- Data flow between UI and backend systems
- Error handling and user feedback

## Success Criteria

- [ ] Intuitive quality preset selection with clear descriptions
- [ ] Working custom seed input with random generation
- [ ] Accurate real-time progress display with time estimates
- [ ] Beautiful geological timeline visualization
- [ ] Advanced options for power users
- [ ] Seamless integration with existing UI systems
- [ ] Performance metrics display for monitoring
- [ ] Responsive UI during long simulations

## Performance Targets

- **UI Responsiveness:** 60fps during geological simulation
- **Progress Update Rate:** 10Hz for smooth visual feedback
- **Memory Overhead:** < 50MB for UI components
- **Startup Time:** < 1 second for UI initialization

## Notes

The UI is the player's primary interaction with the geological realism system. Focus on clarity, beauty, and providing useful feedback. Players should feel excited watching their world being geologically formed, and should have clear understanding of what quality level will work best for their hardware.
