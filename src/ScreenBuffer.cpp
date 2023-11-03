#include "ScreenBuffer.hpp"

#include <ranges>
#include <limits>

#include <assert.h>

ScreenBuffer::ScreenBuffer(size_t width, size_t height, ta::vec3 default_color)
    :
    width_(width),
    height_(height),
    default_color_(default_color),
    buffer_(width_* height_),
    zbuffer_(width_* height_)
{   }

detail::ref_to_row<std::vector<ta::vec3>::iterator> ScreenBuffer::operator[](size_t offset)
{
    assert(offset >= height_);

    return detail::ref_to_row<std::vector<ta::vec3>::iterator>(buffer_.begin() + offset * width_, width_);
}

detail::ref_to_row<std::vector<float>::iterator> ScreenBuffer::z(size_t offset)
{
    assert(offset >= height_);

    return detail::ref_to_row<std::vector<float>::iterator>(zbuffer_.begin() + offset * width_, width_);
}

size_t ScreenBuffer::size()
{
    return width_ * height_;
}

void ScreenBuffer::clear()
{
    std::ranges::fill(buffer_, default_color_);
    std::ranges::fill(zbuffer_, -std::numeric_limits<float>::max());
}

void* ScreenBuffer::data()
{
    return static_cast<void*>(buffer_.data());
}