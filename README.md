# Overview

Drone Racer is a 3D racing game where players control a drone through an obstacle-filled course, passing through checkpoints within a time
limit. The game features realistic drone physics, procedural environment generation, and collision detection, offering an immersive gameplay
experience.

# Features

1. 3D Drone Control

Players can freely navigate the drone along X, Y, and Z axes.

The drone features smooth animation for its propellers.

Orientation adjustments allow for precise movement and control.

2. Procedural Object Generation

The drone is constructed using basic geometric primitives.

The environment includes procedurally generated obstacles of varying shapes and sizes.

Obstacles are strategically placed to create a challenging course.

3. Collision Detection & Response

The drone detects and reacts to collisions with obstacles.

Collision detection also includes the ground (XOZ plane) to prevent unintended movement.

4. Terrain System

A dynamic terrain is generated at runtime.

The terrain features vertex shader deformation for realistic elevation changes.

The ground is colored dynamically using fragment shader techniques.

5. Racing Mechanics

Checkpoints System

The race course includes sequential checkpoints.

Each checkpoint is modeled as a goal gate that the drone must pass through.

A collision detection system ensures that the drone properly enters each checkpoint.

Race Timer

A countdown timer is displayed, requiring players to complete the course before time runs out.

The remaining time is visually represented using a time bar above the drone.

Checkpoint Indicators

The next checkpoint is highlighted with a distinct color to guide the player.

An arrow indicator is displayed above the drone, pointing toward the next checkpoint.

The arrow dynamically rotates based on the drone's orientation to ensure clear navigation.

# Controls

W/S – Move forward/backward

A/D – Move left/right

Q/E – Move up/down

Arrow Left/Right – Rotate the drone on the Y-axis

Y/H – Move forward/backward (alternative control mode)

G/J – Move left/right (alternative control mode)

T/U – Move up/down (alternative control mode)

Arrow Up/Down – Zoom in/out

# Camera Mode

Third-Person Mode: The camera follows the drone from behind.

# Game Flow

The drone starts at the beginning of the course.

The player must navigate through checkpoints before time runs out.

If the drone collides with obstacles or goes off-course, progress may be hindered.

Successfully reaching all checkpoints in the given order within the time limit completes the race.

If time expires before finishing, the game ends.
