#include <Camera.h>

namespace Core {

    Camera::Camera()
        :mPosition(0), mViewport(0) {}
    
    Camera::Camera(const glm::vec3& position, const glm::vec2& viewport)
        :mPosition(position), mViewport(viewport) {}
}