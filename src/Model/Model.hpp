#include <vector>
#include <string_view>

#include <tinyalgebralib/math/math.hpp>
#include <threadpoollib/threadpoollib.hpp>

class Model
{
public:

    Model();
    ~Model();

    void load_from_file(std::string_view file);
    size_t vertex_count() const noexcept;
    
    size_t indices_count() const noexcept;
    size_t indices_sizeof() const noexcept;
    
    float* vdata() noexcept;
    unsigned int* indices() noexcept;

    std::vector<ta::vec3> transform(ta::mat4 view, ta::mat4 projection) noexcept;

    void load_identity() noexcept;
    void rotare(const ta::vec3& axis, float angle);

private:
    std::vector<ta::vec3> vertices_;
    std::vector<ta::vec3> normals_;
    std::vector<unsigned int> triangles_;
    std::vector<unsigned int> solid_ranges_;

    threadpool::ThreadPool pool_;
    ta::mat4 model_;
    
};