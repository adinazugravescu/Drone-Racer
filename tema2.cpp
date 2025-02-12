#include "lab_m1/Tema2/tema2.h"
#include "lab_m1/Tema2/camera.h"
#include "lab_m1/Tema2/transform3D.h"
#include <vector>
#include <string>
#include <iostream>
#include <tuple>
#include <cmath>

using namespace std;
using namespace m1;


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}

Mesh* CreateCubeMesh() {
    std::vector<VertexFormat> vertices = {
        // Front
        VertexFormat(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0)),
        VertexFormat(glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)),
        VertexFormat(glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0, 0, 1), glm::vec3(1, 1, 0)),

        // Back
        VertexFormat(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec3(1, 0, 1)),
        VertexFormat(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec3(0, 1, 1)),
        VertexFormat(glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec3(1, 1, 1)),
        VertexFormat(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec3(0, 0, 0)),
    };

    std::vector<unsigned int> indices = {
        // Front
        0, 1, 2,
        0, 2, 3,

        // Top
        3, 2, 6,
        3, 6, 7,

        // Back
        7, 6, 5,
        7, 5, 4,

        // Bottom
        4, 5, 1,
        4, 1, 0,

        // Left
        4, 0, 3,
        4, 3, 7,

        // Right
        1, 5, 6,
        1, 6, 2
    };

    Mesh* cubeMesh = new Mesh("cube");
    cubeMesh->InitFromData(vertices, indices);
    return cubeMesh;
}


void Tema2::RenderRotor(const glm::mat4& droneModelMatrix, const glm::vec3& localPosition, float rotorAngle, bool clockwise)
{
    glm::mat4 modelMatrix = droneModelMatrix;

    modelMatrix *= transform3D::Translate(localPosition.x, localPosition.y, localPosition.z);

    // Rotor's base cube
    glm::mat4 rotorCubeMatrix = modelMatrix;
    rotorCubeMatrix *= transform3D::Scale(0.2f, 0.2f, 0.2f);
    RenderSimpleMesh(meshes["cube"], shaders["ObjectShader"], rotorCubeMatrix, glm::vec3(0.5f, 0.5f, 0.5f), 0);

    // Change rotation direction based on which rotor is drawn
    float adjustedAngle = clockwise ? rotorAngle : -rotorAngle;

    // Rotor blade
    glm::mat4 rotorBladeMatrix = modelMatrix;
    rotorBladeMatrix *= transform3D::Translate(0.0f, 0.125f, 0.0f);
    rotorBladeMatrix *= transform3D::RotateOY(adjustedAngle);
    rotorBladeMatrix *= transform3D::Scale(0.05f, 0.05f, 0.6f);

    RenderSimpleMesh(meshes["cube"], shaders["ObjectShader"], rotorBladeMatrix, glm::vec3(0.0f, 0.0f, 0.0f), 0);
}



void Tema2::RenderDroneBody(const glm::mat4& droneModelMatrix)
{
    glm::mat4 bodyModelMatrix = droneModelMatrix;

    // First
    bodyModelMatrix *= transform3D::Scale(2.5f, 0.2f, 0.2f);
    RenderSimpleMesh(meshes["cube"], shaders["ObjectShader"], bodyModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f), 0);

    bodyModelMatrix = droneModelMatrix;
    // Second
    bodyModelMatrix *= transform3D::RotateOY(glm::radians(90.0f));
    bodyModelMatrix *= transform3D::Scale(2.5f, 0.2f, 0.2f);
    RenderSimpleMesh(meshes["cube"], shaders["ObjectShader"], bodyModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f), 0);
}


void Tema2::RenderDrone(float deltaTimeSeconds)
{
    glm::mat4 droneModelMatrix = glm::mat4(1);

    // Position the drone
    droneModelMatrix *= transform3D::Translate(dronePosition.x, dronePosition.y, dronePosition.z);
    droneModelMatrix *= transform3D::RotateOY(glm::radians(45.0f)); // X form
    droneModelMatrix *= transform3D::RotateOY(glm::radians(rotationAngle));
    droneModelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
    
    // Render Body
    RenderDroneBody(droneModelMatrix);

    glm::vec3 rotorOffsets[] = {
       glm::vec3(1.15f, 0.2f, 0.0f),
       glm::vec3(-1.15f, 0.2f, 0.0f),
       glm::vec3(0.0f, 0.2f, 1.15f),
       glm::vec3(0.0f, 0.2f, -1.15f)
    };

    rotorAngle += deltaTimeSeconds * 7;
    // Render Rotor
    for (int i = 0; i < 4; i++)
    {
        bool clockwise = (i % 2 == 0);
        RenderRotor(droneModelMatrix, rotorOffsets[i], rotorAngle, clockwise);
    }
}

void Tema2::GenerateObstacles()
{
    obstacles.clear();

    // Layout parameters
    const int obstacleCount = 20;
    const float minDistance = 5.5f;
    const float worldSize = 30.5f;

    for (int i = 0; i < obstacleCount; i++)
    {
        // Generate random position for each obstacle
        
        glm::vec3 position;
        bool validPosition = false; // Condition for inter obstacles space

        while (!validPosition)
        {
            validPosition = true;

            position = glm::vec3(
                rand() % static_cast<int>(worldSize) - worldSize / 2.0f, // Generate random x value in worldSize's range, centered around 0
                0.25f, // obstacles are on the ground
                rand() % static_cast<int>(worldSize) - worldSize / 2.0f  // Generate random z value in worldSize's range, centered around 0
            );

            // Iterate through obstacles and check if minDistance condition is verified
            for (const auto& obs : obstacles)
            {
                glm::vec3 existingPosition = std::get<0>(obs);
                if (glm::distance(position, existingPosition) < minDistance) // If not, then position is not valid and repeat the steps above
                {
                    validPosition = false;
                    break;
                }
            }
        }

        // Found valid position, add random obstacle with random dimensions

        if (rand() % 2 == 0)
        {
            // Add tree
            float trunkHeight = 1.0f + static_cast<float>(rand() % 100) / 100.0f; // Base height + random value in the range [0.0, 0.99]
            float crownHeight = 1.5f + static_cast<float>(rand() % 100) / 100.0f; // Base height + random value in the range [0.0, 0.99]
            float scale = 0.8f + static_cast<float>(rand() % 100) / 500.0f; // Base scale + random value in the range [0.0, 0.2];
            // position - type - dimensions for render
            obstacles.emplace_back(position, "tree", glm::vec3(trunkHeight, crownHeight, scale));
        }
        else
        {
            // Add building
            float width = 1.0f + static_cast<float>(rand() % 100) / 50.0f; // Base width + random value in the range [0.0, 1.2];
            float height = 2.0f + static_cast<float>(rand() % 100) / 50.0f; // Base height + random value in the range [0.0, 1.2];
            float depth = 1.0f + static_cast<float>(rand() % 100) / 50.0f; // Base depth + random value in the range [0.0, 1.2];
            // position - type - dimensions for render
            obstacles.emplace_back(position, "building", glm::vec3(width, height, depth));
        }
    }
}

void Tema2::RenderObstacles()
{
    // Iterate through obstacles and render based on their type
    for (const auto& obstacle : obstacles)
    {
        glm::vec3 position = std::get<0>(obstacle);
        std::string type = std::get<1>(obstacle);
        glm::vec3 dimensions = std::get<2>(obstacle);

        if (type == "tree")
        {
            RenderTree(position, dimensions.x, dimensions.y, dimensions.z);
        }
        else if (type == "building")
        {
            RenderBuilding(position, dimensions.x, dimensions.y + 0.25f, dimensions.z);
        }
    }
}

void Tema2::RenderTree(const glm::vec3& position, float trunkHeight, float crownHeight, float scale)
{
    // Render Trunk
    glm::mat4 trunkMatrix = glm::mat4(1);
    trunkMatrix *= transform3D::Translate(position.x, position.y, position.z);
    trunkMatrix *= transform3D::Scale(0.2f * scale, trunkHeight * scale * 1.25, 0.2f * scale);
    RenderSimpleMesh(meshes["box"], shaders["ObjectShader"], trunkMatrix, glm::vec3(0.55f, 0.27f, 0.07f), 0);

    // Render Crown
    float crownScale = 1.8f * scale; // Initial size of the largest sphere
    float scaleDecrement = 0.3f;
    float yOffset = trunkHeight * scale; // Above the trunk

    for (int i = 0; i < 5; ++i)
    {
        // Position and scale the current sphere
        glm::mat4 crownMatrix = glm::mat4(1);
        crownMatrix *= transform3D::Translate(position.x, position.y + yOffset, position.z);
        crownMatrix *= transform3D::Scale(crownScale, crownScale, crownScale);

        RenderSimpleMesh(meshes["sphere"], shaders["ObjectShader"], crownMatrix, glm::vec3(0.0f, 0.5f, 0.0f), 0);

        // Adjust yOffset
        yOffset += crownScale * 0.6f;

        crownScale -= scaleDecrement;
    }
}

void Tema2::RenderBuilding(const glm::vec3& position, float width, float height, float depth)
{
    glm::mat4 buildingMatrix = glm::mat4(1);
    buildingMatrix *= transform3D::Translate(position.x, position.y + height / 2.0f, position.z);
    buildingMatrix *= transform3D::Scale(width, height, depth);
    RenderSimpleMesh(meshes["box"], shaders["ObjectShader"], buildingMatrix, glm::vec3(0.6f, 0.6f, 0.6f), 0);
}

Mesh* GenerateGrid(int m, int n, float width, float height) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float dx = width / (n - 1);
    float dz = height / (m - 1);

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            float x = j * dx - width / 2.0f; // centers the grid
            float z = i * dz - height / 2.0f; /// centers the grid
            // position - normal - color
            vertices.emplace_back(glm::vec3(x, 0.0f, z), glm::vec3(0, 1, 0), glm::vec3(0.5f, 0.5f, 0.5f));
        }
    }

    for (int i = 0; i < m - 1; ++i) {
        for (int j = 0; j < n - 1; ++j) {
            // Form the triangles

            int idx = i * n + j;

            indices.push_back(idx);
            indices.push_back(idx + 1);
            indices.push_back(idx + n);

            indices.push_back(idx + 1);
            indices.push_back(idx + n + 1);
            indices.push_back(idx + n);
        }
    }

    Mesh* grid = new Mesh("grid");
    grid->InitFromData(vertices, indices);
    return grid;
}

bool Tema2::CheckSphereSphereCollision(glm::vec3 center1, float radius1, glm::vec3 center2, float radius2) {
    float distance = glm::distance(center1, center2);
    return distance <= (radius1 + radius2);
}


bool Tema2::CheckSphereBoxCollision(glm::vec3 sphereCenter, float sphereRadius, glm::vec3 boxMin, glm::vec3 boxMax) {
    // Closest point on the box to sphere center
    float closestX = glm::clamp(sphereCenter.x, boxMin.x, boxMax.x);
    float closestY = glm::clamp(sphereCenter.y, boxMin.y, boxMax.y);
    float closestZ = glm::clamp(sphereCenter.z, boxMin.z, boxMax.z);
    // Distance from sphere center to box
    float distanceSquared = (closestX - sphereCenter.x) * (closestX - sphereCenter.x) +
        (closestY - sphereCenter.y) * (closestY - sphereCenter.y) +
        (closestZ - sphereCenter.z) * (closestZ - sphereCenter.z);

    return distanceSquared <= (sphereRadius * sphereRadius);
}

bool Tema2::CheckCollisions(const glm::vec3& potentialPosition) {
    // Drone can't exit grid's perimeter
    if (potentialPosition.x < -35.0f || potentialPosition.x > 35.0f ||
        potentialPosition.z < -35.0f || potentialPosition.z > 35.0f) {
        return false;
    }

    // Compute terrain's height at drone's position using the same logic from VertexShader
    float groundHeight = GetTerrainHeight(potentialPosition.x, potentialPosition.z);

    // Check for terrain collision
    if (potentialPosition.y - droneRadius < groundHeight) {
        return false;
    }

    // Check for obstacle collision
    for (const auto& obstacle : obstacles) {
        glm::vec3 position = std::get<0>(obstacle);
        std::string type = std::get<1>(obstacle);
        glm::vec3 dimensions = std::get<2>(obstacle);

        if (type == "tree") {
            // Logic from RenderTree
            float scale = dimensions.z;
            float trunkHeight = dimensions.x * scale * 1.25f;
            glm::vec3 trunkBase = position;
            float trunkRadius = 0.2f * scale / 2.0f;

            float crownScale = 1.8f * scale;
            float scaleDecrement = 0.3f;
            float yOffset = trunkHeight * scale;

            for (int i = 0; i < 5; ++i)
            {
                float crownRadius = crownScale;
                glm::vec3 crownCenter = position + glm::vec3(0, yOffset, 0);
                // Check for collision for each sphere in crown
                if (CheckSphereSphereCollision(potentialPosition, droneRadius, crownCenter, crownRadius)) {
                    return false;
                }

                yOffset += crownScale * 0.6f;
                crownScale -= scaleDecrement;

                if (crownScale < 0.1f * scale) {
                    crownScale = 0.1f * scale;
                }
            }
        } else if (type == "building") {
            // Logic from RenderBuilding
            float width = dimensions.x;
            float height = dimensions.y + 0.25f;
            float depth = dimensions.z;

            float halfHeight = height / 2.0f;
            glm::vec3 buildingCenter = position + glm::vec3(0.0f, halfHeight, 0.0f);

            glm::vec3 buildingMin = buildingCenter - glm::vec3(width / 2.0f, halfHeight, depth / 2.0f);
            glm::vec3 buildingMax = buildingCenter + glm::vec3(width / 2.0f, halfHeight, depth / 2.0f);

            if (CheckSphereBoxCollision(potentialPosition, 0.6f, buildingMin, buildingMax)) {
                return false;
            }
        }

    }

    // Check collision with checkpoints
    for (int i = 0; i < checkpoints.size(); i++) {
        const Checkpoint& checkpoint = checkpoints[i];

        // Left pillar
        glm::vec3 leftPillarCenter = checkpoint.position + glm::vec3(-checkpoint.dimensions.x / 2.0f, checkpoint.dimensions.y / 2.0f, 0.0f);
        glm::vec3 leftPillarHalfExtents = glm::vec3(0.2f / 2.0f, checkpoint.dimensions.y / 2.0f, 0.2f / 2.0f);

        // Right pillar
        glm::vec3 rightPillarCenter = checkpoint.position + glm::vec3(checkpoint.dimensions.x / 2.0f, checkpoint.dimensions.y / 2.0f, 0.0f);
        glm::vec3 rightPillarHalfExtents = glm::vec3(0.2f / 2.0f, checkpoint.dimensions.y / 2.0f, 0.2f / 2.0f);

        // Top beam
        glm::vec3 topBeamCenter = checkpoint.position + glm::vec3(0.0f, checkpoint.dimensions.y, 0.0f);
        glm::vec3 topBeamHalfExtents = glm::vec3((checkpoint.dimensions.x + 0.2f) / 2.0f, 0.2f / 2.0f, 0.2f / 2.0f);

        // Check collision with left pillar
        if (CheckSphereBoxCollision(potentialPosition, 0.6f, leftPillarCenter - leftPillarHalfExtents, leftPillarCenter + leftPillarHalfExtents)) {
            return false;
        }

        // Check collision with right pillar
        if (CheckSphereBoxCollision(potentialPosition, 0.6f, rightPillarCenter - rightPillarHalfExtents, rightPillarCenter + rightPillarHalfExtents)) {
            return false;
        }

        // Check collision with top beam
        if (CheckSphereBoxCollision(potentialPosition, 0.6f, topBeamCenter - topBeamHalfExtents, topBeamCenter + topBeamHalfExtents)) {
            return false;
        }
    }
    return true;
}

// Extract fractional value from a number
float fract(float x) {
    return x - std::floor(x);
}

float noise(const glm::vec2& p) {
    return fract(sin(glm::dot(p, glm::vec2(13, 78))) * 43750);
}

// Generate height based on x an z respecting VertexShader logic
float Tema2::GetTerrainHeight(float x, float z) {
    float frequency = 0.2f;
    float amplitude = 0.5f;
    float height = noise(glm::vec2(x, z) * frequency) * amplitude;
    return height;
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, int isGrid) {
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);

    // Send model matrix
    GLint modelLoc = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Send view matrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    GLint viewLoc = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Send projection matrix
    GLint projLoc = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Send isGrid (1 for terrain)
    GLint isGridLoc = glGetUniformLocation(shader->program, "isGrid");
    glUniform1i(isGridLoc, isGrid);

    // Send objectColor
    GLint colorLoc = glGetUniformLocation(shader->program, "objectColor");
    glUniform3fv(colorLoc, 1, glm::value_ptr(color));

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// ADVANCED

void Tema2::GenerateCheckpoints() {
    checkpoints.clear();

    const int checkpointCount = 5;
    const float minDistance = 8.0f;
    const float worldSize = 50.5f;

    for (int i = 0; i < checkpointCount; i++) {
        // Generate random position for each checkpoint

        glm::vec3 position;
        glm::vec3 dimensions(3.0f, 5.0f, 0.5f);
        bool validPosition = false;

        while (!validPosition) {
            validPosition = true;

            position = glm::vec3(
                rand() % static_cast<int>(worldSize) - worldSize / 2.0f, // Generate random x value in worldSize's range, centered around 0
                0.0f, // Ground level
                rand() % static_cast<int>(worldSize) - worldSize / 2.0f // Generate random z value in worldSize's range, centered around 0
            );

            // Check for overlap with obstacles
            for (const auto& obstacle : obstacles) {
                glm::vec3 obstaclePos = std::get<0>(obstacle);
                if (glm::distance(position, obstaclePos) < minDistance) {
                    validPosition = false;
                    break;
                }
            }

            // Check for overlap with other checkpoints
            for (const auto& checkpoint : checkpoints) {
                if (glm::distance(position, checkpoint.position) < minDistance) {
                    validPosition = false;
                    break;
                }
            }
        }
        checkpoints.push_back({ position, dimensions, false });
    }
}

void Tema2::RenderCheckpoints() {
    for (int i = 0; i < checkpoints.size(); i++) {
        const Checkpoint& checkpoint = checkpoints[i];
        glm::vec3 position = checkpoint.position;
        glm::vec3 dimensions = checkpoint.dimensions;
        bool isPassed = checkpoint.isPassed;

        glm::vec3 color = glm::vec3(1.0f); // White
        if (!isPassed && i == currentCheckpointIndex) {
            color = glm::vec3(0.5f, 0.0f, 0.0f); // Red if the checkpoint is the next one to be passed
        }

        RenderCheckpoint(position, dimensions, color);
    }
}

void Tema2::RenderCheckpoint(const glm::vec3& position, const glm::vec3& dimensions, const glm::vec3& color) {
    // Left pillar
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(position.x - dimensions.x / 2.0f, position.y + dimensions.y / 2.0f, position.z);
    modelMatrix *= transform3D::Scale(0.2f, dimensions.y, 0.2f);
    RenderSimpleMesh(meshes["box"], shaders["ObjectShader"], modelMatrix, color, 0);

    // Right pillar
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(position.x + dimensions.x / 2.0f, position.y + dimensions.y / 2.0f, position.z);
    modelMatrix *= transform3D::Scale(0.2f, dimensions.y, 0.2f);
    RenderSimpleMesh(meshes["box"], shaders["ObjectShader"], modelMatrix, color, 0);

    // Top beam
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(position.x, position.y + dimensions.y, position.z);
    modelMatrix *= transform3D::Scale(dimensions.x + 0.2f, 0.2f, 0.2f);
    RenderSimpleMesh(meshes["box"], shaders["ObjectShader"], modelMatrix, color, 0);
}

void Tema2::CheckCheckpointCollision() {
    if (currentCheckpointIndex >= checkpoints.size()) {
        return; // All checkpoints are passed
    }

    const Checkpoint& checkpoint = checkpoints[currentCheckpointIndex];

    // Left bottom back
    glm::vec3 adjustedGateMin = checkpoint.position - glm::vec3(
        (checkpoint.dimensions.x / 2.0f - 0.1f) - droneRadius,
        0.0f, 0.2f + droneRadius);

    // Right up front
    glm::vec3 adjustedGateMax = checkpoint.position + glm::vec3(
        (checkpoint.dimensions.x / 2.0f - 0.1f) - droneRadius, 
        checkpoint.dimensions.y - 0.1f - droneRadius, 0.2f + droneRadius);


    bool isInsideGate = IsSphereCompletelyInsideBox(dronePosition, droneRadius, adjustedGateMin, adjustedGateMax);

    if (isInsideGate && CheckCollisions(dronePosition)) {
        checkpoints[currentCheckpointIndex].isPassed = true;
        currentCheckpointIndex++;
    }
}


bool Tema2::IsSphereCompletelyInsideBox(const glm::vec3& sphereCenter, float sphereRadius, const glm::vec3& boxMin, const glm::vec3& boxMax) {
    return (sphereCenter.x - sphereRadius >= boxMin.x) &&
        (sphereCenter.x + sphereRadius <= boxMax.x) &&
        (sphereCenter.y - sphereRadius >= boxMin.y) &&
        (sphereCenter.y + sphereRadius <= boxMax.y) &&
        (sphereCenter.z - sphereRadius >= boxMin.z) &&
        (sphereCenter.z + sphereRadius <= boxMax.z);
}

void Tema2::RenderArrowIndicator() {
    if (currentCheckpointIndex >= checkpoints.size()) {
        return;
    }

    glm::vec3 targetPosition = checkpoints[currentCheckpointIndex].position;

    // Direction vector from the drone to the checkpoint
    glm::vec3 direction = glm::normalize(targetPosition - dronePosition);

    // Aangle between the drone's forward direction and the direction to the checkpoint
    float angleOY = atan2(glm::dot(localOx, direction), glm::dot(localOz, direction));

    float yOffset = 0.2f;    // Height above
    glm::vec3 arrowPosition = dronePosition + localOz + localOy * yOffset;
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    modelMatrix *= transform3D::Translate(arrowPosition.x, arrowPosition.y, arrowPosition.z);
    modelMatrix *= transform3D::RotateOY(glm::radians(rotationAngle)); // To replicate drone's orientation
    modelMatrix *= transform3D::RotateOY(angleOY); // Rotate to next checkpoint

    modelMatrix *= transform3D::Scale(0.3f, 0.3f, 0.3f);

    RenderSimpleMesh(meshes["arrow"], shaders["ObjectShader"], modelMatrix, glm::vec3(0.5f, 0.0f, 0.0f), 0);
}

Mesh* CreateArrowMesh() {
    float shaftWidth = 0.1f;
    float shaftLength = 0.7f;
    float headWidth = 0.3f;
    float headHeight = 0.3f;

    std::vector<VertexFormat> vertices = {
        // Shaft (along Z)
        VertexFormat(glm::vec3(-shaftWidth / 2, 0, 0)),                    // Bottom L
        VertexFormat(glm::vec3(shaftWidth / 2, 0, 0)),                     // Bottom R
        VertexFormat(glm::vec3(shaftWidth / 2, 0, shaftLength)),           // Top R
        VertexFormat(glm::vec3(-shaftWidth / 2, 0, shaftLength)),          // Top L

        // Head
        VertexFormat(glm::vec3(-headWidth / 2, 0, shaftLength)),           //  Left point base
        VertexFormat(glm::vec3(headWidth / 2, 0, shaftLength)),            //  Right point base
        VertexFormat(glm::vec3(0, 0, shaftLength + headHeight)),           //  Tip
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
    };

    Mesh* arrowMesh = new Mesh("arrow");
    arrowMesh->InitFromData(vertices, indices);
    return arrowMesh;
}

void Tema2::RenderTimeBar() {
    if (currentCheckpointIndex >= checkpoints.size()) {
        return; // if all checkpoints are passed
    }

    float totalTime = 180.0f;
    float remainingTime = timeRemaining;
    int totalSegments = 10;
    int filledSegments = static_cast<int>(ceil(remainingTime / (totalTime / totalSegments)));

    glm::vec3 timeBarPosition = dronePosition + localOz * 1.5f + localOy * 0.3f;

    // Time bar segments dimensions
    float segmentWidth = 0.2f;
    float segmentHeight = 0.05f;
    float segmentDepth = 0.2f;
    float spacing = 0.05f;

    float totalBarWidth = totalSegments * segmentWidth + (totalSegments - 1) * spacing;
    float startX = -totalBarWidth / 2.0f + segmentWidth / 2.0f;

    glm::mat4 timeBarModelMatrix = glm::mat4(1.0f);
    timeBarModelMatrix *= transform3D::Translate(timeBarPosition.x, timeBarPosition.y, timeBarPosition.z);
    timeBarModelMatrix *= transform3D::RotateOY(glm::radians(rotationAngle + 180.0f)); // for drone Oy rotation

    for (int i = 0; i < totalSegments; ++i) {
        float xOffset = startX + i * (segmentWidth + spacing);
        glm::vec3 segmentPos = glm::vec3(xOffset, 0.0f, 0.0f); 

        glm::mat4 modelMatrix = timeBarModelMatrix;
        modelMatrix *= transform3D::Translate(segmentPos.x, segmentPos.y, segmentPos.z);
        modelMatrix *= transform3D::Scale(segmentWidth, segmentHeight, segmentDepth);

        // Color to match remaining time
        glm::vec3 color = (i < filledSegments) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.3f, 0.3f, 0.3f);

        RenderSimpleMesh(meshes["box"], shaders["ObjectShader"], modelMatrix, color, 0);
    }
}


void Tema2::Init()
{

    camera = new implemented::Camera();
    camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Shader* shader = new Shader("ObjectShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Mesh* grid = GenerateGrid(50, 50, 70.0f, 70.0f);
        meshes[grid->GetMeshID()] = grid;

        Shader* shader = new Shader("TerrainShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Mesh* arrowMesh = CreateArrowMesh();
        meshes[arrowMesh->GetMeshID()] = arrowMesh;
    }

    {
        Mesh* mesh = CreateCubeMesh();
        meshes[mesh->GetMeshID()] = mesh;
    }

    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, nearPlane, farPlane);
   
    GenerateObstacles();
    GenerateCheckpoints();

}


void Tema2::FrameStart()
{
    glClearColor(0.5f, 0.8f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();

    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
    // Render Grid
    RenderSimpleMesh(meshes["grid"], shaders["TerrainShader"], glm::mat4(1.0f), glm::vec3(0.0f), 1);

    if (isThirdPerson) {
        glm::vec3 cameraPosition = dronePosition + localOx * thirdPersonCameraOffset.x +
            localOy * thirdPersonCameraOffset.y +
            localOz * thirdPersonCameraOffset.z;
        camera->Set(cameraPosition, dronePosition,localOy);
    }

    // Render Scene's elements
    RenderObstacles();
    RenderDrone(deltaTimeSeconds);
    RenderCheckpoints();
    CheckCheckpointCollision();
    RenderArrowIndicator();
    RenderTimeBar();
   
    if (!gameEnded) {
        timeRemaining -= deltaTimeSeconds;

        if (timeRemaining <= 0.0f) {
            gameEnded = true;
        }

        if (currentCheckpointIndex >= checkpoints.size()) {
            gameEnded = true;
        }
    }
    camera->ReOrthogonalize();

}


void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    if (gameEnded) {
        return;
    }

    // Not third person camera movement
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT) && !isThirdPerson)
    {
        float cameraSpeed = 0.09f;

        if (window->KeyHold(GLFW_KEY_W)) {
            camera->TranslateForward(cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            camera->TranslateRight(-cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            camera->TranslateForward(-cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            camera->TranslateRight(cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            camera->TranslateUpward(-cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            camera->TranslateUpward(cameraSpeed);
        }
    }
    
    float speed = 6.5f * deltaTime;

    glm::vec3 movement(0.0f);

    // Drone movement keys based on game's view
    if (!isThirdPerson) {

        if (window->KeyHold(GLFW_KEY_Y)) {
            movement += localOz * speed;
        }
        if (window->KeyHold(GLFW_KEY_H)) {
            movement -= localOz * speed;
        }
        if (window->KeyHold(GLFW_KEY_G)) {
            movement += localOx * speed;
        }
        if (window->KeyHold(GLFW_KEY_J)) {
            movement -= localOx * speed;
        }
        if (window->KeyHold(GLFW_KEY_T)) {
            movement += localOy * speed;
        }
        if (window->KeyHold(GLFW_KEY_U)) {
            movement -= localOy * speed;
        }
    } else {
        if (window->KeyHold(GLFW_KEY_W)) {
            movement += localOz * speed;
        }
        if (window->KeyHold(GLFW_KEY_S)) {
            movement -= localOz * speed;
        }
        if (window->KeyHold(GLFW_KEY_A)) {
            movement += localOx * speed;
        }
        if (window->KeyHold(GLFW_KEY_D)) {
            movement -= localOx * speed;
        }
        if (window->KeyHold(GLFW_KEY_Q)) {
            movement += localOy * speed;
        }
        if (window->KeyHold(GLFW_KEY_E)) {
            movement -= localOy * speed;
        }
    }

    if (window->KeyHold(GLFW_KEY_UP)) {
        //  (zoom out)
        fov += fovStep;
        projectionMatrix = glm::perspective(fov, window->props.aspectRatio, nearPlane, farPlane);
    }

    if (window->KeyHold(GLFW_KEY_DOWN)) {
        //  (zoom in)
        fov -= fovStep;
        fov = glm::clamp(fov, RADIANS(10.0f), RADIANS(120.0f));
        projectionMatrix = glm::perspective(fov, window->props.aspectRatio, nearPlane, farPlane);
    }

    // Check for collisions for drone's potential position
    if (movement != glm::vec3(0.0f)) {
        glm::vec3 potentialPosition = dronePosition + movement;

        if (CheckCollisions(potentialPosition)) {
            dronePosition = potentialPosition; // If no collisions, then move
        }
    }

    // Oy orientation
    if (window->KeyHold(GLFW_KEY_LEFT)) {
        rotationAngle += 100.0f * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        rotationAngle -= 100.0f * deltaTime;
    }

    glm::mat4 rotationMatrix = transform3D::RotateOY(glm::radians(rotationAngle));
    localOx = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(1, 0, 0, 0)));
    localOz = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0, 0, 1, 0)));
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Not third person camera movement 
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT) && !isThirdPerson)
    {
        float sensitivityOX = 0.001f;
        float sensitivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            camera->RotateFirstPerson_OY(-deltaX * sensitivityOY);
            camera->RotateFirstPerson_OX(-deltaY * sensitivityOX);

        }

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            camera->RotateThirdPerson_OY(-deltaX * sensitivityOY);
            camera->RotateThirdPerson_OX(-deltaY * sensitivityOX);

        }
    }
}

void Tema2::OnKeyPress(int key, int mods)
{
}


void Tema2::OnKeyRelease(int key, int mods)
{
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema2::OnWindowResize(int width, int height)
{
}
