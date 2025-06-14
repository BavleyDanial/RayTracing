#pragma once

#include <cstdint>
#include <vector>

namespace RT {

    class IndexBuffer {
    public:
        IndexBuffer() = default;
        IndexBuffer(const std::vector<uint32_t>& data);

        void Bind() const;
        void UnBind() const;

    private:
        uint32_t mRendererID;
    };

}
