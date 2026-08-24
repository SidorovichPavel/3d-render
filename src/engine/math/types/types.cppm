export module math.types;

export import :matrix;
export import :vector;

export namespace math {

using mat4 = Matrix<float, 4, 4>;
using mat4i = Matrix<int, 4, 4>;

using mat3 = Matrix<float, 3, 3>;
using mat3i = Matrix<int, 3, 3>;

using mat2 = Matrix<float, 2, 2>;
using mat2i = Matrix<int, 2, 2>;

using vec4 = Vector<float, 4>;
using vec4i = Vector<int, 4>;

using vec3 = Vector<float, 3>;
using vec3i = Vector<int, 3>;

using vec2 = Vector<float, 2>;
using vec2i = Vector<int, 2>;

}  // namespace math
