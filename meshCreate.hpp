using namespace std;
using namespace glm;

// #define M_PI 3.141595f
void SolarSystem::createSphereMesh(float radius, vector<VertexMesh> &vDef, vector<uint32_t> &vIdx) {
    const int numLatitudes = 50;   // Number of latitude divisions
    const int numLongitudes = 50;  // Number of longitude divisions

    // Generate vertices for the sphere
    for (int lat = 0; lat <= numLatitudes; ++lat) {
        float theta = static_cast<float>(lat) * M_PI / static_cast<float>(numLatitudes); // Latitude angle
        for (int lon = 0; lon <= numLongitudes; ++lon) {
            float phi = static_cast<float>(lon) * 2.0f * M_PI / static_cast<float>(numLongitudes); // Longitude angle

            // Convert spherical coordinates to Cartesian coordinates
            float x = std::cos(phi) * std::sin(theta);
            float y = std::cos(theta);
            float z = std::sin(phi) * std::sin(theta);

            // Vertex position
            glm::vec3 position(x*radius, y*radius, z*radius);
            // Normal vector is simply the vertex position normalized, as it points from the origin to the surface
            glm::vec3 normal = glm::normalize(position);
            // Texture coordinates based on latitude and longitude
            float u = static_cast<float>(lon) / static_cast<float>(numLongitudes);
            float v = static_cast<float>(lat) / static_cast<float>(numLatitudes);

            // Add the vertex to the vertex definition array
            vDef.push_back({ position, normal, { u, v } });
        }
    }

    // Generate indices for the sphere triangles
    for (int lat = 0; lat < numLatitudes; ++lat) {
        for (int lon = 0; lon < numLongitudes; ++lon) {
            int index0 = lat * (numLongitudes + 1) + lon;
            int index1 = index0 + 1;
            int index2 = (lat + 1) * (numLongitudes + 1) + lon;
            int index3 = index2 + 1;

            // First triangle
            vIdx.push_back(index0);
            vIdx.push_back(index2);
            vIdx.push_back(index1);

            // Second triangle
            vIdx.push_back(index1);
            vIdx.push_back(index2);
            vIdx.push_back(index3);
        }
    }
}
