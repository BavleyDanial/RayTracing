#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Core {
    
    class Camera {
    public:
        Camera();
        Camera(const glm::vec3& position, const glm::vec2& viewport, float fov, float nearClip, float farClip);

        void OnResize(const glm::vec2& viewport);
        
        const glm::mat4& GetProjection() const { return mProjectionMatrix; }
        const glm::mat4& GetInverseProjection() const { return mInverseProjectionMatrix; }
        const glm::mat4& GetView() const { return mViewMatrix; }
        const glm::mat4& GetInverseView() const { return mInverseViewMatrix; }
	
        void SetPosition(const glm::vec3& position);
        const glm::vec3& GetPosition() const { return mPosition; }
        
        const std::vector<glm::vec3>& GetRayDirections() const { return mRayDirections; }
    private:
        void CalculateRayDirections();
    private:
        glm::vec3 mPosition{0};
        glm::vec2 mViewport{16/9.0f, 1};
        
        float mFOV = 45.0f;
        float mNearClip = 0.1f;
        float mFarClip = 1000.0f;
        float mAspectRatio = 16/9.0f;

        glm::mat4 mViewMatrix{1};
        glm::mat4 mProjectionMatrix{1};
        glm::mat4 mInverseViewMatrix{1};
        glm::mat4 mInverseProjectionMatrix{1};

        std::vector<glm::vec3> mRayDirections;
    };
    
}