#include <vector>
#include <string_view>

#include <tinyalgebralib/math/math.hpp>

using vertex_t = ta::vec3;
using normal_t = ta::vec3;

class Model
{
public:

    Model();
    ~Model();

    void load_from_file(std::string_view file);
    size_t vertex_count() const noexcept;

    float* vdata() noexcept;
    unsigned int* indices() noexcept;

private:
    std::vector<ta::vec3> vertices_;
    std::vector<ta::vec3> normals_;
    std::vector<unsigned int> triangles_;
    std::vector<unsigned int> solid_ranges_;
};