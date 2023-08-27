void SolarSystem::createPlanetMesh(float radius, std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx) {
    const int numLatitudes = 50;   // Number of latitude divisions
    const int numLongitudes = 50;  // Number of longitude divisions

    // Generate vertices for the sphere
    for (int lat = 0; lat <= numLatitudes; ++lat) {
        float theta = static_cast<float>(lat) * M_PI / static_cast<float>(numLatitudes); // Latitude angle
        for (int lon = 0; lon <= numLongitudes; ++lon) {
            float phi = static_cast<float>(lon) * 2.0f * M_PI / static_cast<float>(numLongitudes); // Longitude angle

            // Convert spherical coordinates to Cartesian coordinates
            float x = -std::cos(phi) * std::sin(theta);
            float y = std::cos(theta);
            float z = std::sin(phi) * std::sin(theta);

            // Vertex position
            glm::vec3 position(x * radius, y * radius, z * radius);
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

void SolarSystem::createSaturnRing(float radius, std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx) {
    /*
    A_____________B
    |             | BC <-> outer ring
    D-------------C

    UV coor:
	0--->1
	|
	v
	1

    */
   
    const int numSlices = 100;

    for (int i = 0; i < numSlices; i++) {
        float theta = i*2*M_PI/numSlices;

        float x_rel = std::cos(theta)*radius*1.2;
        float y_rel = std::sin(theta)*radius*1.2;

        float x_a = x_rel;
        float y_a = y_rel;

        float x_b = x_rel*1.5;
        float y_b = y_rel*1.5;

        float x_c = x_rel*1.5;
        float y_c = y_rel*1.5;

        float x_d = x_rel;
        float y_d = y_rel;

        vDef.push_back({{x_a, 0, y_a}, {0,1,0}, {0, 0}}); //A
		vDef.push_back({{x_b, 0, y_b}, {0,1,0}, {1, 0}}); //B
		vDef.push_back({{x_c, 0, y_c}, {0,1,0}, {1, 1}}); //C
		vDef.push_back({{x_d, 0, y_d}, {0,1,0}, {0, 1}}); //D
    }

	for(int i = 0; i < numSlices*4; i++) {
		vIdx.push_back((i+0) % (numSlices*4));
		vIdx.push_back((i+1) % (numSlices*4));
		vIdx.push_back((i+2) % (numSlices*4));

		vIdx.push_back((i+2) % (numSlices*4));
		vIdx.push_back((i+3) % (numSlices*4));
		vIdx.push_back((i+0) % (numSlices*4));
	}
}

void createCube(float radius, std::vector<VertexSkydome>& vDef, std::vector<uint32_t>& vIdx) {


    /*
	   _ _ _ _ 
	 _|		  |_ _ _ _ _
    |					|
	|					|
	|					|
	|_		   _ _ _ _ _|
	  |_ _ _ _|

	UV coor:
	0--->1
	|
	v
	1
	*/

	float u = 0.25f; 
	float v = 0.33f;

	// centered at the origin with edge length of radius*2
    std::vector<VertexSkydome> vertices = {
		// front
		{{-radius,-radius, radius}, {0,0,0.5}, {4*u,2*v}},
		{{ radius,-radius, radius}, {0,0,0.5}, {3*u,2*v}},
		{{ radius, radius, radius}, {0,0,0.5}, {3*u,1*v}},
		{{-radius, radius, radius}, {0,0,0.5}, {4*u,1*v}},
		// back
		{{-radius,-radius, -radius}, {0,0,1}, {4*u,2*v}},
		{{ radius,-radius, -radius}, {0,0,1}, {3*u,2*v}},
		{{ radius, radius, -radius}, {0,0,1}, {3*u,1*v}},
		{{-radius, radius, -radius}, {0,0,1}, {4*u,1*v}},
		// bottom
		{{-radius,-radius, -radius}, {0,1,0}, {4*u,2*v}},
		{{ radius,-radius, -radius}, {0,1,0}, {3*u,2*v}},
		{{ radius,-radius,  radius}, {0,1,0}, {3*u,1*v}},
		{{-radius,-radius,  radius}, {0,1,0}, {4*u,1*v}},
		// top
		{{-radius, radius, radius}, {0,1,0}, {4*u,2*v}},
		{{ radius, radius, radius}, {0,1,0}, {3*u,2*v}},
		{{ radius, radius,-radius}, {0,1,0}, {3*u,1*v}},
		{{-radius, radius,-radius}, {0,1,0}, {4*u,1*v}},
		// left
		{{-radius,-radius,-radius}, {1,0,0}, {4*u,2*v}},
		{{-radius,-radius, radius}, {1,0,0}, {3*u,2*v}},
		{{-radius, radius, radius}, {1,0,0}, {3*u,1*v}},
		{{-radius, radius,-radius}, {1,0,0}, {4*u,1*v}},
		//right
		{{ radius,-radius, radius}, {1,0,0}, {4*u,2*v}},
		{{ radius,-radius,-radius}, {1,0,0}, {3*u,2*v}},
		{{ radius, radius,-radius}, {1,0,0}, {3*u,1*v}},
		{{ radius, radius, radius}, {1,0,0}, {4*u,1*v}},
	};

	std::vector<glm::vec3> triangles = {
		{0, 1, 2}, {2, 3, 0},
		{4, 5, 6}, {6, 7, 4},
		{8, 9, 10}, {10, 11, 8},
		{12, 13, 14}, {14, 15, 12},
		{16, 17, 18}, {18, 19, 16},
		{20, 21, 22}, {22, 23, 20},
	};

	for(int i = 0; i < vertices.size(); i++) {
		vDef.push_back(vertices[i]);
	}

	for(int i = 0; i < triangles.size(); i++) {
		vIdx.push_back(triangles[i][0]);
		vIdx.push_back(triangles[i][1]);
		vIdx.push_back(triangles[i][2]);
	}
}



void SolarSystem::createSkydome(float radius, std::vector<VertexSkydome>& vDef, std::vector<uint32_t>& vIdx) {
    const int numLatitudes = 50;   // Number of latitude divisions
    const int numLongitudes = 50;  // Number of longitude divisions
    
    //createCube(radius, vDef, vIdx);
    //return;

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
            glm::vec3 position(x * radius, y * radius, z * radius);
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

