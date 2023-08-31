//ACTUAL MAIN ON MY PC



// This has been adapted from the Vulkan tutorial
#include "Starter.hpp"
#define M_PI 3.1415926

// The uniform buffer objects data structures
// Remember to use the correct alignas(...) value
//        float : alignas(4)
//        vec2  : alignas(8)
//        vec3  : alignas(16)
//        vec4  : alignas(16)
//        mat3  : alignas(16)
//        mat4  : alignas(16)

struct MeshUniformBlock {
	alignas(4) float amb;
	alignas(4) float gamma;
	alignas(16) glm::vec3 sColor;
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct OverlayUniformBlock {
	alignas(4) float visible;
};

struct GlobalUniformBlockDirect {
	alignas(16) glm::vec3 DlightDir;
	alignas(16) glm::vec3 DlightColor;
	alignas(16) glm::vec3 AmbLightColor;
	alignas(16) glm::vec3 eyePos;
};

struct GlobalUniformBlockPoint {
	alignas(16) glm::vec3 lightPos;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 AmbLightColor;
	alignas(16) glm::vec3 eyePos;
};

struct VertexMesh {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};

struct VertexOverlay {
	glm::vec2 pos;
	glm::vec2 UV;
};

struct CelestialObjectVulkanData {
	std::string name;
	float* rot;
	float* rev;
	Model<VertexMesh>* model;
	DescriptorSet* DS;
	Texture* tex;
	MeshUniformBlock* ubo;
};


class SolarSystem : public BaseProject {
protected:
	// Current aspect ratio (used by the callback that resized the window
	float Ar;

	DescriptorSetLayout DSLGubo, DSLPlanet, DSLSun, DSLSkydome, DSLOverlay;

	// Vertex formats
	VertexDescriptor VMesh;
	VertexDescriptor VOverlay;

	// Pipelines [Shader couples]
	Pipeline PPlanet, PSun, PSkydome, POverlay;

	// Models, textures and Descriptors (values assigned to the uniforms)
	// Please note that Model objects depends on the corresponding vertex structure
	Model<VertexMesh> MSun, MSkydome,
		MMercury, MVenus, MEarth, MMars, MJupiter,
		MSaturn, MSaturnRing, MUranus, MNeptune;
	Model<VertexMesh> MAsteroidsBelt;
	Model<VertexOverlay> MSplash, MKey;
	Model<VertexOverlay> MSunI, MMercuryI, MVenusI, MEarthI, MMarsI, MJupiterI, MSaturnI, MUranusI, MNeptuneI;

	DescriptorSet DSGubo, DSSun, DSSkydome,
		DSMercury, DSVenus, DSEarth, DSMars, DSJupiter,
		DSSaturn, DSSaturnRing, DSUranus, DSNeptune, DSSplash, DSKey;
	DescriptorSet DSSunI, DSMercuryI, DSVenusI, DSEarthI, DSMarsI, DSJupiterI, DSSaturnI, DSUranusI, DSNeptuneI;
	DescriptorSet DSAsteroidsBelt;

	Texture TSun, TSkydome,
		TMercury, TVenus, TEarth, TMars, TJupiter,
		TSaturn, TSaturnRing, TUranus, TNeptune, TSplash, TKey;
	Texture TSunI, TMercuryI, TVenusI, TEarthI, TMarsI, TJupiterI, TSaturnI, TUranusI, TNeptuneI;
	Texture TAsteroid;

	// C++ storage for uniform variables
	MeshUniformBlock uboSun, uboSkydome,
		uboMercury, uboVenus, uboEarth, uboMars, uboJupiter,
		uboSaturn, uboSaturnRing, uboUranus, uboNeptune;
	MeshUniformBlock uboAsteroid;

	std::vector<CelestialObjectVulkanData> celestialObjects = {};

	OverlayUniformBlock uboSplash, uboKey;
	OverlayUniformBlock uboSunI, uboMercuryI, uboVenusI, uboEarthI, uboMarsI, uboJupiterI, uboSaturnI, uboUranusI, uboNeptuneI;

	GlobalUniformBlockDirect dgubo;
	GlobalUniformBlockPoint pgubo;

	// Other application parameters
	glm::vec3 camPos = glm::vec3(5, 0, 20);
		
	float camAlpha = 0.0f;
	float camBeta = 0.0f;
	float camRho = 0.0f;

	int gameState = 1;
	int cameraIsColliding = 0;

	nlohmann::json solarSystemData;

	float MercuryRev = 0.0;
	float VenusRev = 0.0;
	float EarthRev = 0.0;
	float MarsRev = 0.0;
	float JupiterRev = 0.0;
	float SaturnRev = 0.0;
	float UranusRev = 0.0;
	float NeptuneRev = 0.0;

	float SunRot = 0.0;
	float MercuryRot = 0.0;
	float VenusRot = 0.0;
	float EarthRot = 0.0;
	float MarsRot = 0.0;
	float JupiterRot = 0.0;
	float SaturnRot = 0.0;
	float UranusRot = 0.0;
	float NeptuneRot = 0.0;

	// Camera Parameters
	const float FOVy = glm::radians(45.0f);
	const float nearPlane = 0.1f;
	const float farPlane = 400.0f;
	const float rotSpeed = glm::radians(90.0f);
	const float movSpeed = 15.0f;

	float radiusSkydome = 250.0f;
	int Splash = 9;
	int Key = 1;

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "Solar System";
		windowResizable = GLFW_TRUE;
		initialBackgroundColor = { 0.0f, 0.005f, 0.01f, 1.0f };

		// Descriptor pool sizes

		/* Update the requirements for the size of the pool */
		uniformBlocksInPool = 36+1;
		texturesInPool = 24+1;
		setsInPool = 36+1;

		Ar = (float)windowWidth / (float)windowHeight;
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		Ar = (float)w / (float)h;
	}


	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		celestialObjects.push_back({ "Sun", &SunRot, NULL, &MSun, &DSSun, &TSun, &uboSun });
		celestialObjects.push_back({ "Mercury", &MercuryRot, &MercuryRev, &MMercury, &DSMercury, &TMercury, &uboMercury });
		celestialObjects.push_back({ "Venus", &VenusRot, &VenusRev, &MVenus, &DSVenus, &TVenus, &uboVenus });
		celestialObjects.push_back({ "Earth", &EarthRot, &EarthRev, &MEarth, &DSEarth, &TEarth, &uboEarth });
		celestialObjects.push_back({ "Mars", &MarsRot, &MarsRev, &MMars, &DSMars, &TMars, &uboMars });
		celestialObjects.push_back({ "Jupiter", &JupiterRot, &JupiterRev, &MJupiter, &DSJupiter, &TJupiter, &uboJupiter });
		celestialObjects.push_back({ "Saturn", &SaturnRot, &SaturnRev, &MSaturn, &DSSaturn, &TSaturn, &uboSaturn });
		celestialObjects.push_back({ "Uranus", &UranusRot, &UranusRev, &MUranus, &DSUranus, &TUranus, &uboUranus });
		celestialObjects.push_back({ "Neptune", &NeptuneRot, &NeptuneRev, &MNeptune, &DSNeptune, &TNeptune, &uboNeptune });

		std::ifstream configFile("data/solarSystemData.json");

		if (!configFile.is_open()) {
			throw std::runtime_error("Failed to open planet data file.");
		}

		std::ostringstream configStringStream;
		configStringStream << configFile.rdbuf();
		configFile.close();

		solarSystemData = nlohmann::json::parse(configStringStream.str());

		// Descriptor Layouts [what will be passed to the shaders]
		DSLPlanet.init(this, {
			// this array contains the bindings:
			// first  element : the binding number
			// second element : the type of element (buffer or texture)
			//                  using the corresponding Vulkan constant
			// third  element : the pipeline stage where it will be used
			//                  using the corresponding Vulkan constant
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
			{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT}
		});

		DSLSun.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
			{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT},
			{3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});

		DSLOverlay.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});

		DSLSkydome.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
			{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});


		DSLGubo.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
			});

		// Vertex descriptors
		VMesh.init(this, {
			// this array contains the bindings
			// first  element : the binding number
			// second element : the stride of this binging
			// third  element : whether this parameter change per vertex or per instance
			//                  using the corresponding Vulkan constant
			{0, sizeof(VertexMesh), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
				// this array contains the location
				// first  element : the binding number
				// second element : the location number
				// third  element : the offset of this element in the memory record
				// fourth element : the data type of the element
				//                  using the corresponding Vulkan constant
				// fifth  elmenet : the size in byte of the element
				// sixth  element : a constant defining the element usage
				//                   POSITION - a vec3 with the position
				//                   NORMAL   - a vec3 with the normal vector
				//                   UV       - a vec2 with a UV coordinate
				//                   COLOR    - a vec4 with a RGBA color
				//                   TANGENT  - a vec4 with the tangent vector
				//                   OTHER    - anything else
				//
				// ***************** DOUBLE CHECK ********************
				//    That the Vertex data structure you use in the "offsetoff" and
				//	in the "sizeof" in the previous array, refers to the correct one,
				//	if you have more than one vertex format!
				// ***************************************************
				{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, pos),
					   sizeof(glm::vec3), POSITION},
				{0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, norm),
					   sizeof(glm::vec3), NORMAL},
				{0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexMesh, UV),
					   sizeof(glm::vec2), UV}
			});

		VOverlay.init(this, {
				  {0, sizeof(VertexOverlay), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
			  {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, pos),
					 sizeof(glm::vec2), OTHER},
			  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, UV),
					 sizeof(glm::vec2), UV}
			});


		// Pipelines [Shader couples]
		// The second parameter is the pointer to the vertex definition
		// Third and fourth parameters are respectively the vertex and fragment shaders
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..

		PPlanet.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/PlanetFrag.spv", { &DSLGubo, &DSLPlanet });
		// the following line says if the created mesh model will be visible from the inside or not
		PPlanet.setAdvancedFeatures(VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE, false);

		PSun.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/SunFrag.spv", { &DSLGubo, &DSLSun });
		// PSun.setAdvancedFeatures(VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL,
		// 	VK_CULL_MODE_NONE, false);

		PSkydome.init(this, &VSkydome, "shaders/SkydomeVert.spv", "shaders/SkydomeFrag.spv", { &DSLGubo, &DSLSkydome});
		PSkydome.setAdvancedFeatures(VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE, false);

		POverlay.init(this, &VOverlay, "shaders/OverlayVert.spv", "shaders/OverlayFrag.spv", { &DSLOverlay });
		POverlay.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE, false);

		// Models, textures and Descriptors (values assigned to the uniforms)

		// Create models
		// The second parameter is the pointer to the vertex definition for this model
		// The third parameter is the file name
		// The last is a constant specifying the file type: currently only OBJ or GLTF

		/*
		MSkydome.init(this, &VSkydome, "models/SkydomeCube.obj", OBJ);
		const char *T2fn[] = {
				"textures/sky/bkg1_right.png", "textures/sky/bkg1_left.png",
				"textures/sky/bkg1_top.png",   "textures/sky/bkg1_bot.png",
				"textures/sky/bkg1_front.png", "textures/sky/bkg1_back.png"};
		TSkydome.initCubic(this, T2fn);
		*/

		createSkydome(radiusSkydome, MSkydome.vertices, MSkydome.indices);
		MSkydome.initMesh(this, &VMesh);
		TSkydome.init(this, "textures/Skydome.png");

		// Creates a mesh with direct enumeration of vertices and indices
		for (CelestialObjectVulkanData co : celestialObjects) {
			float radius = (float)solarSystemData[co.name]["scale"]["x"];
			createPlanetMesh(
				radius,
				(*co.model).vertices,
				(*co.model).indices
			);
			(*co.model).initMesh(this, &VMesh);
		}

		float radius = (float) solarSystemData["Jupiter"]["orbit"]["distance-from-sun"];
		radius += (float) solarSystemData["Mars"]["orbit"]["distance-from-sun"];
		radius /= 2.0f;
		createAsteroidsBelt(radius, MAsteroidsBelt.vertices, MAsteroidsBelt.indices);
		MAsteroidsBelt.initMesh(this, &VMesh);
		TAsteroid.init(this, "textures/Asteroid.png");

		createSaturnRing(2.0f, MSaturnRing.vertices, MSaturnRing.indices);
		MSaturnRing.initMesh(this, &VMesh);

		MSplash.vertices = { {{-0.5f, 0.82f}, {0.0f,0.0f}},
						  {{-0.5f, 0.95f}, {0.0f,1.0f}},
						  {{ 0.5f, 0.82f}, {1.0f,0.0f}},
						  {{ 0.5f, 0.95f}, {1.0f,1.0f}} };
		MSplash.indices = { 0, 1, 2,    1, 2, 3 };
		MSplash.initMesh(this, &VOverlay);

		createDownBar(MKey.vertices, MKey.indices);
		MKey.initMesh(this, &VOverlay);

		MSunI.vertices = { {{-0.7f, -1.0f}, {0.0f, 0.0f}},
							 {{-0.7f, 0.8f}, {0.0f,1.0f}},
							 {{ 0.7f,-1.0f}, {1.0f,0.0f}},
							 {{ 0.7f, 0.8f}, {1.0f,1.0f}} };
		MSunI.indices = { 0, 1, 2,    1, 2, 3 };
		MSunI.initMesh(this, &VOverlay);

		MMercuryI.vertices = { {{-0.7f, -1.0f}, {0.0f, 0.0f}},
							 {{-0.7f, 0.8f}, {0.0f,1.0f}},
							 {{ 0.7f,-1.0f}, {1.0f,0.0f}},
							 {{ 0.7f, 0.8f}, {1.0f,1.0f}} };
		MMercuryI.indices = { 0, 1, 2,    1, 2, 3 };
		MMercuryI.initMesh(this, &VOverlay);

		MVenusI.vertices = { {{-0.7f, -1.0f}, {0.0f, 0.0f}},
							 {{-0.7f, 0.8f}, {0.0f,1.0f}},
							 {{ 0.7f,-1.0f}, {1.0f,0.0f}},
							 {{ 0.7f, 0.8f}, {1.0f,1.0f}} };
		MVenusI.indices = { 0, 1, 2,    1, 2, 3 };
		MVenusI.initMesh(this, &VOverlay);

		MEarthI.vertices = { {{-0.7f, -1.0f}, {0.0f, 0.0f}},
							 {{-0.7f, 0.8f}, {0.0f,1.0f}},
							 {{ 0.7f,-1.0f}, {1.0f,0.0f}},
							 {{ 0.7f, 0.8f}, {1.0f,1.0f}} };
		MEarthI.indices = { 0, 1, 2,    1, 2, 3 };
		MEarthI.initMesh(this, &VOverlay);

		MMarsI.vertices = { {{-0.7f, -1.0f}, {0.0f, 0.0f}},
							 {{-0.7f, 0.8f}, {0.0f,1.0f}},
							 {{ 0.7f,-1.0f}, {1.0f,0.0f}},
							 {{ 0.7f, 0.8f}, {1.0f,1.0f}} };
		MMarsI.indices = { 0, 1, 2,    1, 2, 3 };
		MMarsI.initMesh(this, &VOverlay);

		MJupiterI.vertices = { {{-0.7f, -1.0f}, {0.0f, 0.0f}},
							 {{-0.7f, 0.8f}, {0.0f,1.0f}},
							 {{ 0.7f,-1.0f}, {1.0f,0.0f}},
							 {{ 0.7f, 0.8f}, {1.0f,1.0f}} };
		MJupiterI.indices = { 0, 1, 2,    1, 2, 3 };
		MJupiterI.initMesh(this, &VOverlay);

		MSaturnI.vertices = { {{-0.7f, -1.0f}, {0.0f, 0.0f}},
							 {{-0.7f, 0.8f}, {0.0f,1.0f}},
							 {{ 0.7f,-1.0f}, {1.0f,0.0f}},
							 {{ 0.7f, 0.8f}, {1.0f,1.0f}} };
		MSaturnI.indices = { 0, 1, 2,    1, 2, 3 };
		MSaturnI.initMesh(this, &VOverlay);

		MUranusI.vertices = { {{-0.7f, -1.0f}, {0.0f, 0.0f}},
							 {{-0.7f, 0.8f}, {0.0f,1.0f}},
							 {{ 0.7f,-1.0f}, {1.0f,0.0f}},
							 {{ 0.7f, 0.8f}, {1.0f,1.0f}} };
		MUranusI.indices = { 0, 1, 2,    1, 2, 3 };
		MUranusI.initMesh(this, &VOverlay);

		MNeptuneI.vertices = { {{-0.7f, -1.0f}, {0.0f, 0.0f}},
							 {{-0.7f, 0.8f}, {0.0f,1.0f}},
							 {{ 0.7f,-1.0f}, {1.0f,0.0f}},
							 {{ 0.7f, 0.8f}, {1.0f,1.0f}} };
		MNeptuneI.indices = { 0, 1, 2,    1, 2, 3 };
		MNeptuneI.initMesh(this, &VOverlay);

		// Create the textures
		// The second parameter is the file name
		for (CelestialObjectVulkanData co : celestialObjects) {
			std::string texPath = "textures/" + co.name + ".png";
			(*co.tex).init(this, texPath.c_str());
		}

		TSaturnRing.init(this, "textures/SaturnRing.png");
		TSplash.init(this, "textures/Splash.png");
		TKey.init(this, "textures/PressSpace.png");
		TSunI.init(this, "textures/SunI.png");
		TMercuryI.init(this, "textures/MercuryI.png");
		TVenusI.init(this, "textures/VenusI.png");
		TEarthI.init(this, "textures/EarthI.png");
		TMarsI.init(this, "textures/MarsI.png");
		TJupiterI.init(this, "textures/JupiterI.png");
		TSaturnI.init(this, "textures/SaturnI.png");
		TUranusI.init(this, "textures/UranusI.png");
		TNeptuneI.init(this, "textures/NeptuneI.png");
	}

	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		PPlanet.create();
		PSun.create();
		PSkydome.create();
		POverlay.create();

		// Here you define the data set
		DSSkydome.init(this, &DSLSkydome, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TSkydome}
			});


		for (CelestialObjectVulkanData co : celestialObjects) {
			if (co.name.compare("Sun") == 0) {
				(*co.DS).init(this, &DSLSun, {
					// the second parameter, is a pointer to the Uniform Set Layout of this set
					// the last parameter is an array, with one element per binding of the set.
					// first  elmenet : the binding number
					// second element : UNIFORM or TEXTURE (an enum) depending on the type
					// third  element : only for UNIFORMs, the size of the corresponding C++ object. For texture, just put 0
					// fourth element : only for TEXTUREs, the pointer to the corresponding texture object. For uniforms, use nullptr
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &(*co.tex)},
					{2, UNIFORM, sizeof(GlobalUniformBlockPoint), nullptr},
					{3, TEXTURE, 0, &(*co.tex)}
					});
			}
			else {
				(*co.DS).init(this, &DSLPlanet, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &(*co.tex)},
					{2, UNIFORM, sizeof(GlobalUniformBlockPoint), nullptr}
					});
			}
		}

		DSSaturnRing.init(this, &DSLPlanet, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TSaturnRing},
			{2, UNIFORM, sizeof(GlobalUniformBlockPoint), nullptr}
			});

		DSAsteroidsBelt.init(this, &DSLPlanet, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TAsteroid},
			{2, UNIFORM, sizeof(GlobalUniformBlockPoint), nullptr}
		});

		DSSplash.init(this, &DSLOverlay, {
			{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
			{1, TEXTURE, 0, &TSplash}
			});

		DSKey.init(this, &DSLOverlay, {
			{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
			{1, TEXTURE, 0, &TKey}
			});

		DSSunI.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TSunI}
			});

		DSMercuryI.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TMercuryI}
			});

		DSVenusI.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TVenusI}
			});

		DSEarthI.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TEarthI}
			});

		DSMarsI.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TMarsI}
			});

		DSJupiterI.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TJupiterI}
			});

		DSSaturnI.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TSaturnI}
			});

		DSUranusI.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TUranusI}
			});

		DSNeptuneI.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TNeptuneI}
			});

		DSGubo.init(this, &DSLGubo, {
			{0, UNIFORM, sizeof(GlobalUniformBlockDirect), nullptr}
			});
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		PPlanet.cleanup();
		PSun.cleanup();
		PSkydome.cleanup();
		POverlay.cleanup();

		// Cleanup datasets
		for (CelestialObjectVulkanData co : celestialObjects) {
			(*co.DS).cleanup();
		}
		DSSaturnRing.cleanup();
		DSAsteroidsBelt.cleanup();
		DSSplash.cleanup();
		DSKey.cleanup();
		DSSunI.cleanup();
		DSMercuryI.cleanup();
		DSVenusI.cleanup();
		DSEarthI.cleanup();
		DSMarsI.cleanup();
		DSJupiterI.cleanup();
		DSSaturnI.cleanup();
		DSUranusI.cleanup();
		DSNeptuneI.cleanup();

		DSGubo.cleanup();
		DSSkydome.cleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {
		// Cleanup textures
		for (CelestialObjectVulkanData co : celestialObjects) {
			(*co.tex).cleanup();
		}
		TSaturnRing.cleanup();
		TAsteroid.cleanup();
		TSkydome.cleanup();
		TSplash.cleanup();
		TKey.cleanup();
		TSunI.cleanup();
		TMercuryI.cleanup();
		TVenusI.cleanup();
		TEarthI.cleanup();
		TMarsI.cleanup();
		TJupiterI.cleanup();
		TSaturnI.cleanup();
		TUranusI.cleanup();
		TNeptuneI.cleanup();

		// Cleanup models
		for (CelestialObjectVulkanData co : celestialObjects) {
			(*co.model).cleanup();
		}
		MAsteroidsBelt.cleanup();
		MSaturnRing.cleanup();
		MSkydome.cleanup();
		MSplash.cleanup();
		MKey.cleanup();
		MSunI.cleanup();
		MMercuryI.cleanup();
		MVenusI.cleanup();
		MEarthI.cleanup();
		MMarsI.cleanup();
		MJupiterI.cleanup();
		MSaturnI.cleanup();
		MUranusI.cleanup();
		MNeptuneI.cleanup();

		// Cleanup descriptor set layouts
		DSLPlanet.cleanup();
		DSLGubo.cleanup();
		DSLSun.cleanup();
		DSLSkydome.cleanup();
		DSLOverlay.cleanup();

		// Destroys the pipelines
		PPlanet.destroy();
		PSun.destroy();
		PSkydome.destroy();
		POverlay.destroy();
	}

	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures

	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		// sets global uniforms (see below fro parameters explanation)
		DSGubo.bind(commandBuffer, PPlanet, 0, currentImage);

		// binds the pipeline
		PPlanet.bind(commandBuffer);

		for (CelestialObjectVulkanData co : celestialObjects) {
			if (co.name.compare("Sun") != 0) {
				(*co.model).bind(commandBuffer);
				(*co.DS).bind(commandBuffer, PPlanet, 1, currentImage);
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>((*co.model).indices.size()), 1, 0, 0, 0);
			}
		}

		MSaturnRing.bind(commandBuffer);
		DSSaturnRing.bind(commandBuffer, PPlanet, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MSaturnRing.indices.size()), 1, 0, 0, 0);

		MAsteroidsBelt.bind(commandBuffer);
		DSAsteroidsBelt.bind(commandBuffer, PPlanet, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MAsteroidsBelt.indices.size()), 1, 0, 0, 0);

		PSun.bind(commandBuffer);
		CelestialObjectVulkanData co = celestialObjects[0];
		(*co.model).bind(commandBuffer);
		(*co.DS).bind(commandBuffer, PSun, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>((*co.model).indices.size()), 1, 0, 0, 0);

		PSkydome.bind(commandBuffer);
		MSkydome.bind(commandBuffer);
		DSSkydome.bind(commandBuffer, PSkydome, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MSkydome.indices.size()), 1, 0, 0, 0);

		POverlay.bind(commandBuffer);
		MSplash.bind(commandBuffer);
		DSSplash.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MSplash.indices.size()), 1, 0, 0, 0);

		MKey.bind(commandBuffer);
		DSKey.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MKey.indices.size()), 1, 0, 0, 0);

		MSunI.bind(commandBuffer);
		DSSunI.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MSunI.indices.size()), 1, 0, 0, 0);

		MMercuryI.bind(commandBuffer);
		DSMercuryI.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MMercuryI.indices.size()), 1, 0, 0, 0);

		MVenusI.bind(commandBuffer);
		DSVenusI.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MVenusI.indices.size()), 1, 0, 0, 0);

		MEarthI.bind(commandBuffer);
		DSEarthI.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MEarthI.indices.size()), 1, 0, 0, 0);

		MMarsI.bind(commandBuffer);
		DSMarsI.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MMarsI.indices.size()), 1, 0, 0, 0);

		MJupiterI.bind(commandBuffer);
		DSJupiterI.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MJupiterI.indices.size()), 1, 0, 0, 0);

		MSaturnI.bind(commandBuffer);
		DSSaturnI.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MSaturnI.indices.size()), 1, 0, 0, 0);

		MUranusI.bind(commandBuffer);
		DSUranusI.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MUranusI.indices.size()), 1, 0, 0, 0);

		MNeptuneI.bind(commandBuffer);
		DSNeptuneI.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MNeptuneI.indices.size()), 1, 0, 0, 0);
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		// Integration with the timers and the controllers
		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);
		// getSixAxis() is defined in Starter.hpp in the base class.
		// It fills the float point variable passed in its first parameter with the time
		// since the last call to the procedure.
		// It fills vec3 in the second parameters, with three values in the -1,1 range corresponding
		// to motion (with left stick of the gamepad, or ASWD + RF keys on the keyboard)
		// It fills vec3 in the third parameters, with three values in the -1,1 range corresponding
		// to motion (with right stick of the gamepad, or Arrow keys + QE keys on the keyboard, or mouse)
		// If fills the last boolean variable with true if fire has been pressed:
		//          SPACE on the keyboard, A or B button on the Gamepad, Right mouse button

		// static float debounce = false;
		// static int curDebounce = 0;
		cameraIsColliding = 0;
		cameraIsColliding = glm::length(camPos-glm::vec3(2, 0, -3)) <= 4.0f;

		std::vector<glm::mat4> matrices = updateCamPos(deltaT, m, r);
		glm::mat4 View = matrices[0];
		glm::mat4 Prj = matrices[1];

		gameLogic(currentImage, fire, View, Prj, deltaT);

		static bool wasFireM = false;
		bool handleFireM = (wasFireM && (!fireM));
		wasFireM = fireM;

		static bool wasFire = false;
		bool handleFire = (wasFire && (!fire));
		wasFire = fire;

		switch (Key) {		// main state machine implementation
			case 0: // initial state - show splash screen
				if (handleFireM) {
					Key = 1;	// jump to the wait key state
				}
				break;
			case 1: // wait key state
				if (handleFireM) {
					Key = 0;	// jump to the moving handle state
				}
				break;
		}

		switch (Splash) {		// main state machine implementation
		case 0: // initial state - show splash screen
			if (handleFire) {
				Splash = 1;	// jump to the wait key state
			}
			break;
		case 1: // wait key state
			if (handleFire) {
				Splash = 2;	// jump to the moving handle state
			}
			break;
		case 2: // wait key state
			if (handleFire) {
				Splash = 3;	// jump to the moving handle state
			}
			break;
		case 3: // wait key state
			if (handleFire) {
				Splash = 4;	// jump to the moving handle state
			}
			break;
		case 4: // wait key state
			if (handleFire) {
				Splash = 5;	// jump to the moving handle state
			}
			break;
		case 5: // wait key state
			if (handleFire) {
				Splash = 6;	// jump to the moving handle state
			}
			break;
		case 6: // wait key state
			if (handleFire) {
				Splash = 7;	// jump to the moving handle state
			}
			break;
		case 7: // wait key state
			if (handleFire) {
				Splash = 8;	// jump to the moving handle state
			}
			break;
		case 8: // wait key state
			if (handleFire) {
				Splash = 9;	// jump to the moving handle state
			}
			break;

		case 9: // wait key state
			if (handleFire) {
				Splash = 0;	// jump to the moving handle state
			}
			break;
		}

		// Point light
		pgubo.lightPos = glm::vec3(0, 0, 0); // position of the sun
		pgubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		pgubo.AmbLightColor = glm::vec3(0.1f);
		pgubo.eyePos = camPos;

		// Direct light
		dgubo.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
		dgubo.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		dgubo.AmbLightColor = glm::vec3(0.1f);
		dgubo.eyePos = camPos;

		// Writes value to the GPU
		DSGubo.map(currentImage, &dgubo, sizeof(dgubo), 0);
		// the .map() method of a DataSet object, requires the current image of the swap chain as first parameter
		// the second parameter is the pointer to the C++ data structure to transfer to the GPU
		// the third parameter is its size
		// the fourth parameter is the location inside the descriptor set of this uniform block

		// update Skydome uniforms
		glm::mat4 World;

		World = glm::mat4(1);
		World = glm::translate(World, camPos); // skydome follows camPos
		// World = glm::rotate(World, camAlpha, glm::vec3(0,1,0)); // skydome does not rotate
		// World = glm::rotate(World, camBeta, glm::vec3(1,0,0)); // skydome does not rotate

		// World = glm::translate(World, camPos);
		uboSkydome.amb = 1.0f;
		uboSkydome.gamma = 180.0f;
		uboSkydome.sColor = glm::vec3(0.3f);
		uboSkydome.mMat = World;
		uboSkydome.nMat = glm::inverse(glm::transpose(World));
		uboSkydome.mvpMat = Prj * View * World;// translate(World, camPos);
		DSSkydome.map(currentImage, &uboSkydome, sizeof(uboSkydome), 0);

		// update Planets uniforms
		for(CelestialObjectVulkanData co : celestialObjects) {
			if(co.rev == NULL) continue;
			float revSpeed = (float) solarSystemData[co.name]["orbit"]["speed"];
			(*co.rev) += revSpeed * deltaT;
		}


		for(CelestialObjectVulkanData co : celestialObjects) {
			float distanceFromSun = (float) solarSystemData[co.name]["orbit"]["distance-from-sun"];
			World = glm::mat4(1);
			if(co.name.compare("Sun") != 0 && Key == 0 && co.rev != NULL) { // rotating planets
				World = glm::rotate(World, (*co.rev), glm::vec3(0, 1, 0));
			}
			World = glm::translate(World, glm::vec3(distanceFromSun, 0, 0)); // distance from sun
			World = glm::rotate(World, (*co.rot) / 400, glm::vec3(0, 1, 0)); // planets rotation
			World = glm::rotate(World, 0.1f, glm::vec3(1, 0, 0)); // planets axis tilt

			(*co.ubo).amb = 1.0f;
			(*co.ubo).gamma = 180.0f;
			(*co.ubo).sColor = glm::vec3(1.0f);
			(*co.ubo).mvpMat = Prj * View * World;
			(*co.ubo).mMat = World;
			(*co.ubo).nMat = glm::inverse(glm::transpose(World));
			(*co.DS).map(currentImage, co.ubo, sizeof((*co.ubo)), 0);
			(*co.DS).map(currentImage, &pgubo, sizeof(pgubo), 2);

			if (co.name.compare("Saturn") == 0) { // saturn ring case
				World = glm::mat4(1);
				if (Key == 0) {
					World = glm::rotate(World, glm::radians((float)solarSystemData[co.name]["orbit"]["inclination"]), glm::vec3(0, 0, 1));
					World = glm::rotate(World, (*co.rev) / 3, glm::vec3(0, 1, 0));
				}
				World = glm::translate(World, glm::vec3(distanceFromSun, 0, 0));
				World = rotate(World, glm::radians(28.0f), glm::vec3(1, 0, 0));
				uboSaturnRing.amb = 1.0f;
				uboSaturnRing.gamma = 180.0f;
				uboSaturnRing.sColor = glm::vec3(1.0f);
				uboSaturnRing.mvpMat = Prj * View * World;
				uboSaturnRing.mMat = World;
				uboSaturnRing.nMat = glm::inverse(glm::transpose(World));
				DSSaturnRing.map(currentImage, &uboSaturnRing, sizeof(uboSaturnRing), 0);
				DSSaturnRing.map(currentImage, &pgubo, sizeof(pgubo), 2);
			}
		}

		// Asteroid belt
		MeshUniformBlock ubo{};
		float radius = (float) solarSystemData["Jupiter"]["orbit"]["distance-from-sun"];
		radius += (float) solarSystemData["Mars"]["orbit"]["distance-from-sun"];
		radius /= 2.0f;

		World = glm::mat4(1);
		// asteroid rotation = slightly faster than jupiter
		CelestialObjectVulkanData coJupiter = celestialObjects[5];
		World = glm::rotate(World, (*coJupiter.rev)*1.1f, glm::vec3(0, 1, 0));

		ubo.amb = 1.0f;
		ubo.gamma = 180.0f;
		ubo.sColor = glm::vec3(1.0f);
		ubo.mvpMat = Prj * View * World;
		ubo.mMat = World;
		ubo.nMat = glm::inverse(glm::transpose(World));
		DSAsteroidsBelt.map(currentImage, &ubo, sizeof(ubo), 0);
		DSAsteroidsBelt.map(currentImage, &pgubo, sizeof(pgubo), 2);

	}

	void gameLogic(uint32_t currentImage, bool fire, glm::mat4 View, glm::mat4 Prj, float deltaT) {
		bool fireM = glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS;

		static bool wasFireM = false;
		bool handleFireM = (wasFireM && (!fireM));
		wasFireM = fireM;

		// static bool wasFire = false;
		// bool handleFire = (wasFire && (!fire));
		// wasFire = fire;

		// switch (Key) {		// main state machine implementation
		// 	case 0: // initial state - show splash screen
		// 		if (handleFireM) {
		// 			Key = 1;	// jump to the wait key state
		// 		}
		// 		break;
		// 	case 1: // wait key state
		// 		if (handleFireM) {
		// 			Key = 0;	// jump to the moving handle state
		// 		}
		// 		break;
		// }

		// switch (Splash) {		// main state machine implementation
		// 	case 0: // initial state - show splash screen
		// 		if (handleFire) {
		// 			Splash = 1;	// jump to the wait key state
		// 		}
		// 		break;
		// 	case 1: // wait key state
		// 		if (handleFire) {
		// 			Splash = 2;	// jump to the moving handle state
		// 		}
		// 		break;
		// 	case 2: // wait key state
		// 		if (handleFire) {
		// 			Splash = 3;	// jump to the moving handle state
		// 		}
		// 		break;
		// 	case 3: // wait key state
		// 		if (handleFire) {
		// 			Splash = 4;	// jump to the moving handle state
		// 		}
		// 		break;
		// 	case 4: // wait key state
		// 		if (handleFire) {
		// 			Splash = 5;	// jump to the moving handle state
		// 		}
		// 		break;
		// 	case 5: // wait key state
		// 		if (handleFire) {
		// 			Splash = 6;	// jump to the moving handle state
		// 		}
		// 		break;
		// 	case 6: // wait key state
		// 		if (handleFire) {
		// 			Splash = 7;	// jump to the moving handle state
		// 		}
		// 		break;
		// 	case 7: // wait key state
		// 		if (handleFire) {
		// 			Splash = 8;	// jump to the moving handle state
		// 		}
		// 		break;
		// 	case 8: // wait key state
		// 		if (handleFire) {
		// 			Splash = 9;	// jump to the moving handle state
		// 		}
		// 		break;

		// 	case 9: // wait key state
		// 		if (handleFire) {
		// 			Splash = 0;	// jump to the moving handle state
		// 		}
		// 		break;
		// }


		if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			// Convert pixel coordinates to normalized screen coordinates
			int screenWidth, screenHeight;
			glfwGetWindowSize(window, &screenWidth, &screenHeight);
			float nscX = mouseX*2 / (screenWidth-1) - 1;
			float nscY = mouseY*2 / (screenHeight-1) - 1;

			std::cerr << "[" << nscX << "," << nscY << "]" << std::endl;

			// Each group of 4 vertices in MKey.vertices represent a button 
			// ordered in A, B, C, D top left clockwise
			for(int i = 0; i < MKey.vertices.size(); i += 4) {
				// check if the normalized screen coordinate inside a button
				float xL = MKey.vertices[i+0].pos[0]; // x of A
				float xR = MKey.vertices[i+1].pos[0]; // x of B
				float yU = MKey.vertices[i+0].pos[1]; // y of A
				float yD = MKey.vertices[i+2].pos[1]; // y of C

				if(
					nscX >= xL &&
					nscX <= xR &&
					nscY <= yD && // since y down
					nscY >= yU
				) {
					Splash = (int) i/4;
					// RebuildPipeline();
					std::cerr << "Clicked on: " << Splash << std::endl;
					break;
				}
			}

		}

		std::vector<OverlayUniformBlock> uboCoI = {
			uboSunI, uboMercuryI, uboVenusI, uboEarthI, uboMarsI,
			uboJupiterI, uboSaturnI, uboUranusI, uboNeptuneI
		};

		std::vector<DescriptorSet> DSCoI = {
			DSSunI, DSMercuryI, DSVenusI, DSEarthI, DSMarsI,
			DSJupiterI, DSSaturnI, DSUranusI, DSNeptuneI
		};

		if(Splash < 9) {
			for(int i = 0; i < uboCoI.size(); i++) {
				uboCoI[i].visible = 0;
				if(i == Splash) {
					uboCoI[i].visible = 1;
				}
				DSCoI[i].map(currentImage, &(uboCoI[i]), sizeof(uboCoI[i]), 0);
			}

		}
		else if(Splash == 9) { //close all
			for(int i = 0; i < uboCoI.size(); i++) {
				uboCoI[i].visible = 0;
				DSCoI[i].map(currentImage, &(uboCoI[i]), sizeof(uboCoI[i]), 0);
			}
		}
		else if(Splash == 10) Key = 0; //move
		else if(Splash == 11) Key = 1; //stop

		uboKey.visible = 1;
		DSKey.map(currentImage, &uboKey, sizeof(uboKey), 0);
	}

	std::vector<glm::mat4> updateCamPos(float deltaT, glm::vec3 m, glm::vec3 r) {
		camAlpha = camAlpha - rotSpeed * deltaT * r.y;
		camBeta = camBeta - rotSpeed * deltaT * r.x;
		camBeta = camBeta < glm::radians(-90.0f) ? glm::radians(-90.0f) :
			(camBeta > glm::radians(90.0f) ? glm::radians(90.0f) : camBeta);
		// camRho = camRho - rotSpeed * deltaT * r.z;
		// camRho = camRho < glm::radians(-180.0f) ? glm::radians(-180.0f) :
		// 	(camRho > glm::radians(180.0f) ? glm::radians(180.0f) : camRho);

		glm::vec3 ux = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0,1,0)) * glm::vec4(1,0,0,1);
		glm::vec3 uz = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0,1,0)) * glm::vec4(0,0,-1,1);

		glm::vec3 oldPos = camPos;

		camPos = camPos + movSpeed * m.x * ux * deltaT;
		camPos = camPos + movSpeed * m.y * glm::vec3(0, 1, 0) * deltaT;
		camPos = camPos + movSpeed * m.z * uz * deltaT;

		cameraIsColliding = 0;

		// check if collision with a planet
		for(CelestialObjectVulkanData co : celestialObjects) {
			float coRadius = (float) solarSystemData[co.name]["scale"]["x"];

			// get position after rotating and translating due to revolution
			glm::vec3 coPos = glm::vec3((*co.ubo).mMat * glm::vec4(0,0,0,1));

			cameraIsColliding = glm::length(camPos-coPos) <= coRadius*1.2;
			if(cameraIsColliding) {
				camPos = oldPos;
				break;
			}
		}

		float distanceFromOrigin = glm::length(camPos-glm::vec3(0.0f));
		if(distanceFromOrigin >= radiusSkydome*0.9 || cameraIsColliding) {
			camPos = oldPos;
		}
		

		glm::mat4 Prj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
		Prj[1][1] *= -1;

		glm::mat4 View = glm::mat4(1);
		glm::vec3 oppPos = camPos;
		oppPos[0] *= -1;
		oppPos[1] *= -1;
		oppPos[2] *= -1;
		View = glm::rotate(glm::mat4(1.0), -CamBeta, glm::vec3(1,0,0)) *
				glm::rotate(glm::mat4(1.0), -CamAlpha, glm::vec3(0,1,0)) *
				// glm::rotate(glm::mat4(1.0), -CamRho, glm::vec3(0,0,1)) *
				glm::translate(glm::mat4(1.0), -camPos);
		
		return { View, Prj };
	}

	void createDownBar(std::vector<VertexOverlay>& vDef, std::vector<uint32_t>& vIdx);

	void createPlanetMesh(float radius, std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx);

	void createSaturnRing(float radius, std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx);

	void createSkydome(float radius, std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx);
	
	void createAsteroidsBelt(float radius, std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx);
};

#include "planetCreate.hpp"

// This is the main: probably you do not need to touch this!
int main() {
	SolarSystem app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}