#include <Camera.h>

namespace Core {
    Camera::Camera(const glm::vec3& position, const glm::vec2& viewport, float fov, float nearClip, float farClip)
        :mPosition(position), mViewport(viewport),
            mFOV(fov), mNearClip(nearClip), mFarClip(farClip),
            mAspectRatio(viewport.x/viewport.y) {
        
        mProjectionMatrix = glm::perspective(mFOV, mAspectRatio, mNearClip, mFarClip);
        mViewMatrix = glm::lookAt(mPosition, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
        mRayDirections.reserve(viewport.x * viewport.y);
    }

    void Camera::OnResize(const glm::vec2& viewport) {
        if (mViewport.x == viewport.x && mViewport.y == viewport.y) {
            return;
        }
        
        mViewport = viewport;
        mAspectRatio = viewport.x/viewport.y;
        
        mProjectionMatrix = glm::perspective(mFOV, mAspectRatio, mNearClip, mFarClip);
        mInverseProjectionMatrix = glm::inverse(mProjectionMatrix);

        mRayDirections.reserve(viewport.x * viewport.y);
        CalculateRayDirections();
    }
    
    void Camera::SetPosition(const glm::vec3& position) {
        mPosition = position;
        mViewMatrix = glm::lookAt(mPosition, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
    }

    void Camera::CalculateRayDirections() {
        for (int y = 0; y < mViewport.y; y++) {
            for (int x = 0; x < mViewport.x; x++) {
                glm::vec2 fragPos = glm::vec2(x / mViewport.x, y / mViewport.y);
                fragPos = fragPos * 2.0f - 1.0f;

                glm::vec4 target = mInverseProjectionMatrix * glm::vec4(fragPos.x, fragPos.y, 1, 1);
                glm::vec3 rayDirection = glm::vec3(mInverseViewMatrix * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space
                mRayDirections[x + y * mViewport.x] = rayDirection;
            }
        }
    }
}
