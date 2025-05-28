#ifndef CAMERA_MODE_H
#define CAMERA_MODE_H

/**
 * Defines the different camera modes available in the game
 */
enum class CameraMode {
    FREE_FLYING,    // Original spectator camera with free movement in all directions
    FIRST_PERSON    // Physics-based first-person camera with gravity and collision
};

#endif // CAMERA_MODE_H
