#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/camera.h"


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        Tema2();
        ~Tema2();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;


        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void RenderRotor(const glm::mat4& droneModelMatrix, const glm::vec3& localPosition, float rotorAngle, bool clockwise);
        void RenderDroneBody(const glm::mat4& droneModelMatrix);
        void RenderDrone(float deltaTimeSeconds);
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, int isGrid);
        void GenerateObstacles();
        void RenderObstacles();
        void RenderTree(const glm::vec3& position, float trunkHeight, float crownHeight, float scale);
        void RenderBuilding(const glm::vec3& position, float width, float height, float depth);
        bool CheckSphereSphereCollision(glm::vec3 center1, float radius1, glm::vec3 center2, float radius2);
        bool CheckSphereBoxCollision(glm::vec3 sphereCenter, float sphereRadius, glm::vec3 boxMin, glm::vec3 boxMax);
        float GetTerrainHeight(float x, float z);
        bool CheckCollisions(const glm::vec3& potentialPosition);
       
        void GenerateCheckpoints();
        void RenderCheckpoints();
        void RenderCheckpoint(const glm::vec3& position, const glm::vec3& dimensions, const glm::vec3& color);
        void CheckCheckpointCollision();
        void RenderArrowIndicator();
        bool IsSphereCompletelyInsideBox(const glm::vec3& sphereCenter, float sphereRadius, const glm::vec3& boxMin, const glm::vec3& boxMax);
        void RenderTimeBar();

    protected:
        implemented::Camera* camera;
        glm::vec3 thirdPersonCameraOffset = glm::vec3(0.0f, 0.5f, -0.95f);
        bool isThirdPerson = true;

        // Projection
        float fov = RADIANS(60.0f);
        float aspectRatio = window->props.aspectRatio;
        float nearPlane = 0.01f;
        float farPlane = 200.0f;
        float fovStep = RADIANS(2.0f);
        glm::mat4 projectionMatrix;

        // Drone
        glm::vec3 dronePosition = glm::vec3(0.0f, 6.5f, -25.0f);
        glm::vec3 localOx = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 localOy = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 localOz = glm::vec3(0.0f, 0.0f, 1.0f);
        float rotationAngle = 0.0f;
        float rotorAngle = 0;
        float droneRadius = 0.25f;

        // Vector of tuples : position - type - dimensions
        std::vector<std::tuple<glm::vec3, std::string, glm::vec3>> obstacles;

        struct Checkpoint {
            glm::vec3 position;
            glm::vec3 dimensions;
            bool isPassed;
        };
        std::vector<Checkpoint> checkpoints;
        int currentCheckpointIndex = 0; // For checkpoint order passing
        float timeRemaining = 180.0f;
        bool gameEnded = false;

    };
}
