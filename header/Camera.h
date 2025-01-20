#pragma once

#include <glm/glm.hpp>

namespace Core {
    
    class Camera {
    public:
        Camera();
        Camera(const glm::vec3& position, const glm::vec2& viewport);
        
        void SetPosition(const glm::vec3& position) { mPosition = position; }
        void SetViewport(const glm::vec2& viewport) { mViewport = viewport; }
        
        glm::vec3 GetPosition() const { return mPosition; }
        glm::vec2 GetViewport() const { return mViewport; }
    private:
        glm::vec3 mPosition;
        glm::vec2 mViewport;
    };
    
}