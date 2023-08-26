//ACTUAL MAIN ON MY PC



// This has been adapted from the Vulkan tutorial
#include "Starter.hpp"
#define M_PI 3.141595f

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

struct GlobalUniformBlock {
	alignas(16) glm::vec3 DlightDir;
	alignas(16) glm::vec3 DlightColor;
	alignas(16) glm::vec3 AmbLightColor;
	alignas(16) glm::vec3 eyePos;
};

// The vertices data structures 8
struct VertexMesh {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};


/* 3 */
/* Add the C++ datastructure for the required vertex format */



// MAIN ! 
class SolarSystem : public BaseProject {
protected:

	// Current aspect ratio (used by the callback that resized the window
	float Ar;

	DescriptorSetLayout DSLGubo, DSLMesh;

	// Vertex formats
	VertexDescriptor VMesh;

	/* A16 */
	/* Add the variable that will contain the required Vertex format definition */

	// Pipelines [Shader couples]
	Pipeline PMesh;

	// Models, textures and Descriptors (values assigned to the uniforms)
	// Please note that Model objects depends on the corresponding vertex structure
	Model<VertexMesh> MSun, MMercury, MVenus, MEarth, MMars, MJupiter, MSaturn, MUranus, MNeptune;

	DescriptorSet DSGubo, DSSun, DSMercury, DSVenus, DSEarth, DSMars, DSJupiter, DSSaturn, DSUranus, DSNeptune;

	/* Add the variable that will contain the Descriptor Set for the room */
	Texture TSun, TMercury, TVenus, TEarth, TMars, TJupiter, TSaturn, TUranus, TNeptune;

	// C++ storage for uniform variables
	MeshUniformBlock uboSun, uboMercury, uboVenus, uboEarth, uboMars, uboJupiter, uboSaturn, uboUranus, uboNeptune;

	/* Add the variable that will contain the Uniform Block in slot 0, set 1 of the room */
	GlobalUniformBlock gubo;

	// Other application parameters
	glm::vec3 camPos = glm::vec3(0.440019, 0.5, 3.45706);
	float CamAlpha = 0.0f;
	float CamBeta = 0.0f;
	float CamRho = 0.0f;

	int gameState;
	float HandleRot = 0.0;
	float Wheel1Rot = 0.0;
	float Wheel2Rot = 0.0;
	float Wheel3Rot = 0.0;



	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "Solar System";
		windowResizable = GLFW_TRUE;
		initialBackgroundColor = { 0.0f, 0.005f, 0.01f, 1.0f };

		// Descriptor pool sizes
		/* A16 */
		/* Update the requirements for the size of the pool */
		uniformBlocksInPool = 10;
		texturesInPool = 9;
		setsInPool = 10;

		Ar = (float)windowWidth / (float)windowHeight;
	}

	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		Ar = (float)w / (float)h;
	}

	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]
		DSLMesh.init(this, {
			// this array contains the bindings:
			// first  element : the binding number
			// second element : the type of element (buffer or texture)
			//                  using the corresponding Vulkan constant
			// third  element : the pipeline stage where it will be used
			//                  using the corresponding Vulkan constant
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});

		/* A16 */
		/* Init the new Data Set Layout */

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

		/* A16 */
		/* Define the new Vertex Format */

		// Pipelines [Shader couples]
		// The second parameter is the pointer to the vertex definition
		// Third and fourth parameters are respectively the vertex and fragment shaders
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
		PMesh.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/MeshFrag.spv", { &DSLGubo, &DSLMesh });

		/* Create the new pipeline, using shaders "VColorVert.spv" and "VColorFrag.spv" */

		// Models, textures and Descriptors (values assigned to the uniforms)

		// Create models
		// The second parameter is the pointer to the vertex definition for this model
		// The third parameter is the file name
		// The last is a constant specifying the file type: currently only OBJ or GLTF

		// Creates a mesh with direct enumeration of vertices and indices
		createPlanetMesh(3.0f, MSun.vertices, MSun.indices);
		MSun.initMesh(this, &VMesh);

		createPlanetMesh(0.4f, MMercury.vertices, MMercury.indices);
		MMercury.initMesh(this, &VMesh);

		createPlanetMesh(1.2f, MVenus.vertices, MVenus.indices);
		MVenus.initMesh(this, &VMesh);

		createPlanetMesh(1.2f, MEarth.vertices, MEarth.indices);
		MEarth.initMesh(this, &VMesh);

		createPlanetMesh(0.6f, MMars.vertices, MMars.indices);
		MMars.initMesh(this, &VMesh);

		createPlanetMesh(5.0f, MJupiter.vertices, MJupiter.indices);
		MJupiter.initMesh(this, &VMesh);

		createPlanetMesh(4.0f, MSaturn.vertices, MSaturn.indices);
		MSaturn.initMesh(this, &VMesh);

		createPlanetMesh(2.0f, MUranus.vertices, MUranus.indices);
		MUranus.initMesh(this, &VMesh);

		createPlanetMesh(2.0f, MNeptune.vertices, MNeptune.indices);
		MNeptune.initMesh(this, &VMesh);
		// Create the textures
		// The second parameter is the file name
		TSun.init(this, "textures/Sun.png");
		TMercury.init(this, "textures/Mercury.png");
		TVenus.init(this, "textures/Venus.png");
		TEarth.init(this, "textures/Earth.png");
		TMars.init(this, "textures/Mars.png");
		TJupiter.init(this, "textures/Jupiter.png");
		TSaturn.init(this, "textures/Saturn.png");
		TUranus.init(this, "textures/Uranus.png");
		TNeptune.init(this, "textures/Neptune.png");
	}

	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		PMesh.create();

		// Here you define the data set
		DSSun.init(this, &DSLMesh, {
			// the second parameter, is a pointer to the Uniform Set Layout of this set
			// the last parameter is an array, with one element per binding of the set.
			// first  elmenet : the binding number
			// second element : UNIFORM or TEXTURE (an enum) depending on the type
			// third  element : only for UNIFORMs, the size of the corresponding C++ object. For texture, just put 0
			// fourth element : only for TEXTUREs, the pointer to the corresponding texture object. For uniforms, use nullptr
						{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
						{1, TEXTURE, 0, &TSun}
			});
		DSMercury.init(this, &DSLMesh, {
			// the second parameter, is a pointer to the Uniform Set Layout of this set
			// the last parameter is an array, with one element per binding of the set.
			// first  elmenet : the binding number
			// second element : UNIFORM or TEXTURE (an enum) depending on the type
			// third  element : only for UNIFORMs, the size of the corresponding C++ object. For texture, just put 0
			// fourth element : only for TEXTUREs, the pointer to the corresponding texture object. For uniforms, use nullptr
						{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
						{1, TEXTURE, 0, &TMercury}
			});

		DSVenus.init(this, &DSLMesh, {
			// the second parameter, is a pointer to the Uniform Set Layout of this set
			// the last parameter is an array, with one element per binding of the set.
			// first  elmenet : the binding number
			// second element : UNIFORM or TEXTURE (an enum) depending on the type
			// third  element : only for UNIFORMs, the size of the corresponding C++ object. For texture, just put 0
			// fourth element : only for TEXTUREs, the pointer to the corresponding texture object. For uniforms, use nullptr
						{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
						{1, TEXTURE, 0, &TVenus}
			});

		DSEarth.init(this, &DSLMesh, {
			// the second parameter, is a pointer to the Uniform Set Layout of this set
			// the last parameter is an array, with one element per binding of the set.
			// first  elmenet : the binding number
			// second element : UNIFORM or TEXTURE (an enum) depending on the type
			// third  element : only for UNIFORMs, the size of the corresponding C++ object. For texture, just put 0
			// fourth element : only for TEXTUREs, the pointer to the corresponding texture object. For uniforms, use nullptr
						{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
						{1, TEXTURE, 0, &TEarth}
			});

		DSMars.init(this, &DSLMesh, {
			// the second parameter, is a pointer to the Uniform Set Layout of this set
			// the last parameter is an array, with one element per binding of the set.
			// first  elmenet : the binding number
			// second element : UNIFORM or TEXTURE (an enum) depending on the type
			// third  element : only for UNIFORMs, the size of the corresponding C++ object. For texture, just put 0
			// fourth element : only for TEXTUREs, the pointer to the corresponding texture object. For uniforms, use nullptr
						{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
						{1, TEXTURE, 0, &TMars}
			});

		DSJupiter.init(this, &DSLMesh, {
			// the second parameter, is a pointer to the Uniform Set Layout of this set
			// the last parameter is an array, with one element per binding of the set.
			// first  elmenet : the binding number
			// second element : UNIFORM or TEXTURE (an enum) depending on the type
			// third  element : only for UNIFORMs, the size of the corresponding C++ object. For texture, just put 0
			// fourth element : only for TEXTUREs, the pointer to the corresponding texture object. For uniforms, use nullptr
						{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
						{1, TEXTURE, 0, &TJupiter}
			});

		DSSaturn.init(this, &DSLMesh, {
			// the second parameter, is a pointer to the Uniform Set Layout of this set
			// the last parameter is an array, with one element per binding of the set.
			// first  elmenet : the binding number
			// second element : UNIFORM or TEXTURE (an enum) depending on the type
			// third  element : only for UNIFORMs, the size of the corresponding C++ object. For texture, just put 0
			// fourth element : only for TEXTUREs, the pointer to the corresponding texture object. For uniforms, use nullptr
						{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
						{1, TEXTURE, 0, &TSaturn}
			});

		DSUranus.init(this, &DSLMesh, {
			// the second parameter, is a pointer to the Uniform Set Layout of this set
			// the last parameter is an array, with one element per binding of the set.
			// first  elmenet : the binding number
			// second element : UNIFORM or TEXTURE (an enum) depending on the type
			// third  element : only for UNIFORMs, the size of the corresponding C++ object. For texture, just put 0
			// fourth element : only for TEXTUREs, the pointer to the corresponding texture object. For uniforms, use nullptr
						{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
						{1, TEXTURE, 0, &TUranus}
			});

		DSNeptune.init(this, &DSLMesh, {
			// the second parameter, is a pointer to the Uniform Set Layout of this set
			// the last parameter is an array, with one element per binding of the set.
			// first  elmenet : the binding number
			// second element : UNIFORM or TEXTURE (an enum) depending on the type
			// third  element : only for UNIFORMs, the size of the corresponding C++ object. For texture, just put 0
			// fourth element : only for TEXTUREs, the pointer to the corresponding texture object. For uniforms, use nullptr
						{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
						{1, TEXTURE, 0, &TNeptune}
			});

		DSGubo.init(this, &DSLGubo, {
					{0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
			});
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		PMesh.cleanup();

		// Cleanup datasets
		DSSun.cleanup();
		DSMercury.cleanup();
		DSVenus.cleanup();
		DSEarth.cleanup();
		DSMars.cleanup();
		DSJupiter.cleanup();
		DSSaturn.cleanup();
		DSUranus.cleanup();
		DSNeptune.cleanup();
		DSGubo.cleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {
		// Cleanup textures
		TSun.cleanup();
		TMercury.cleanup();
		TVenus.cleanup();
		TEarth.cleanup();
		TMars.cleanup();
		TJupiter.cleanup();
		TSaturn.cleanup();
		TUranus.cleanup();
		TNeptune.cleanup();
		// Cleanup models
		MSun.cleanup();
		MMercury.cleanup();
		MVenus.cleanup();
		MEarth.cleanup();
		MMars.cleanup();
		MJupiter.cleanup();
		MSaturn.cleanup();
		MUranus.cleanup();
		MNeptune.cleanup();
		// Cleanup descriptor set layouts
		DSLMesh.cleanup();

		/* Cleanup the new Descriptor Set Layout */
		DSLGubo.cleanup();

		// Destroys the pipelines
		PMesh.destroy();
	}

	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures

	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		// sets global uniforms (see below fro parameters explanation)
		DSGubo.bind(commandBuffer, PMesh, 0, currentImage);

		// binds the pipeline
		PMesh.bind(commandBuffer);
		// For a pipeline object, this command binds the corresponing pipeline to the command buffer passed in its parameter

		// binds the model
		MSun.bind(commandBuffer);

		// For a Model object, this command binds the corresponing index and vertex buffer
		// to the command buffer passed in its parameter

		// binds the data set
		DSSun.bind(commandBuffer, PMesh, 1, currentImage);

		// For a Dataset object, this command binds the corresponing dataset
		// to the command buffer and pipeline passed in its first and second parameters.
		// The third parameter is the number of the set being bound
		// As described in the Vulkan tutorial, a different dataset is required for each image in the swap chain.
		// This is done automatically in file Starter.hpp, however the command here needs also the index
		// of the current image in the swap chain, passed in its last parameter

		// record the drawing command in the command buffer
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MSun.indices.size()), 1, 0, 0, 0);
		// the second parameter is the number of indexes to be drawn. For a Model object,
		// this can be retrieved with the .indices.size() method.
		MMercury.bind(commandBuffer);
		DSMercury.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MMercury.indices.size()), 1, 0, 0, 0);

		MVenus.bind(commandBuffer);
		DSVenus.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MVenus.indices.size()), 1, 0, 0, 0);

		MEarth.bind(commandBuffer);
		DSEarth.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MEarth.indices.size()), 1, 0, 0, 0);

		MMars.bind(commandBuffer);
		DSMars.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MMars.indices.size()), 1, 0, 0, 0);

		MJupiter.bind(commandBuffer);
		DSJupiter.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MJupiter.indices.size()), 1, 0, 0, 0);

		MSaturn.bind(commandBuffer);
		DSSaturn.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MSaturn.indices.size()), 1, 0, 0, 0);

		MUranus.bind(commandBuffer);
		DSUranus.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MUranus.indices.size()), 1, 0, 0, 0);

		MNeptune.bind(commandBuffer);
		DSNeptune.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MNeptune.indices.size()), 1, 0, 0, 0);
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		// Standard procedure to quit when the ESC key is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

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




		// Parameters
		// Camera FOV-y, Near Plane and Far Plane
		const float FOVy = glm::radians(45.0f);
		const float nearPlane = 0.1f;
		const float farPlane = 100.0f;
		const float rotSpeed = glm::radians(90.0f);
		const float movSpeed = 1.0f;

		CamAlpha = CamAlpha - rotSpeed * deltaT * r.y;
		CamBeta = CamBeta - rotSpeed * deltaT * r.x;
		CamBeta = CamBeta < glm::radians(-90.0f) ? glm::radians(-90.0f) :
			(CamBeta > glm::radians(90.0f) ? glm::radians(90.0f) : CamBeta);
		CamRho = CamRho - rotSpeed * deltaT * r.z;
		CamRho = CamRho < glm::radians(-180.0f) ? glm::radians(-180.0f) :
			(CamRho > glm::radians(180.0f) ? glm::radians(180.0f) : CamRho);

		glm::mat3 CamDir = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0, 1, 0)) *
			glm::rotate(glm::mat4(1.0f), CamBeta, glm::vec3(1, 0, 0)) *
			glm::rotate(glm::mat4(1.0f), CamRho, glm::vec3(0, 0, 1));

		glm::vec3 ux = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0, 1, 0)) * glm::vec4(1, 0, 0, 1);
		glm::vec3 uz = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0, 1, 0)) * glm::vec4(0, 0, -1, 1);
		camPos = camPos + movSpeed * m.x * ux * deltaT;
		camPos = camPos + movSpeed * m.y * glm::vec3(0, 1, 0) * deltaT;
		camPos = camPos + movSpeed * m.z * uz * deltaT;

		glm::mat4 Prj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
		Prj[1][1] *= -1;

		glm::mat4 View = glm::mat4(1);

		glm::vec3 oppPos = camPos;
		oppPos[0] *= -1;
		oppPos[1] *= -1;
		oppPos[2] *= -1;
		View = glm::rotate(glm::mat4(1.0), -CamRho, glm::vec3(0, 0, 1)) *
			glm::rotate(glm::mat4(1.0), -CamBeta, glm::vec3(1, 0, 0)) *
			glm::rotate(glm::mat4(1.0), -CamAlpha, glm::vec3(0, 1, 0)) *
			glm::translate(glm::mat4(1.0), oppPos);

		gubo.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
		gubo.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.AmbLightColor = glm::vec3(0.1f);
		gubo.eyePos = camPos;

		// Writes value to the GPU
		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);
		// the .map() method of a DataSet object, requires the current image of the swap chain as first parameter
		// the second parameter is the pointer to the C++ data structure to transfer to the GPU
		// the third parameter is its size
		// the fourth parameter is the location inside the descriptor set of this uniform block

		glm::mat4 World = glm::mat4(1);
		World = glm::translate(World, glm::vec3(2, 0, -3)); // Position for the Sun
		uboSun.amb = 1.0f;
		uboSun.gamma = 180.0f;
		uboSun.sColor = glm::vec3(1.0f);
		uboSun.mvpMat = Prj * View * World;
		uboSun.mMat = World;
		uboSun.nMat = glm::inverse(glm::transpose(World));
		DSSun.map(currentImage, &uboSun, sizeof(uboSun), 0);


		World = glm::translate(World, glm::vec3(4, 0, -3)); // Position for Mercury
		uboMercury.amb = 1.0f;
		uboMercury.gamma = 180.0f;
		uboMercury.sColor = glm::vec3(1.0f);
		uboMercury.mvpMat = Prj * View * World;
		uboMercury.mMat = World;
		uboMercury.nMat = glm::inverse(glm::transpose(World));
		DSMercury.map(currentImage, &uboMercury, sizeof(uboMercury), 0);

		World = glm::translate(World, glm::vec3(6, 0, -3)); // Position for Venus
		uboVenus.amb = 1.0f;
		uboVenus.gamma = 180.0f;
		uboVenus.sColor = glm::vec3(1.0f);
		uboVenus.mvpMat = Prj * View * World;
		uboVenus.mMat = World;
		uboVenus.nMat = glm::inverse(glm::transpose(World));
		DSVenus.map(currentImage, &uboVenus, sizeof(uboVenus), 0);

		World = glm::translate(World, glm::vec3(8, 0, -3)); // Position for Earth
		uboEarth.amb = 1.0f;
		uboEarth.gamma = 180.0f;
		uboEarth.sColor = glm::vec3(1.0f);
		uboEarth.mvpMat = Prj * View * World;
		uboEarth.mMat = World;
		uboEarth.nMat = glm::inverse(glm::transpose(World));
		DSEarth.map(currentImage, &uboEarth, sizeof(uboEarth), 0);

		World = glm::translate(World, glm::vec3(10, 0, -3)); // Position for Mars
		uboMars.amb = 1.0f;
		uboMars.gamma = 180.0f;
		uboMars.sColor = glm::vec3(1.0f);
		uboMars.mvpMat = Prj * View * World;
		uboMars.mMat = World;
		uboMars.nMat = glm::inverse(glm::transpose(World));
		DSMars.map(currentImage, &uboMars, sizeof(uboMars), 0);

		World = glm::translate(World, glm::vec3(12, 0, -3)); // Position for Jupiter
		uboJupiter.amb = 1.0f;
		uboJupiter.gamma = 180.0f;
		uboJupiter.sColor = glm::vec3(1.0f);
		uboJupiter.mvpMat = Prj * View * World;
		uboJupiter.mMat = World;
		uboJupiter.nMat = glm::inverse(glm::transpose(World));
		DSJupiter.map(currentImage, &uboJupiter, sizeof(uboJupiter), 0);

		World = glm::translate(World, glm::vec3(14, 0, -3)); // Position for Saturn
		uboSaturn.amb = 1.0f;
		uboSaturn.gamma = 180.0f;
		uboSaturn.sColor = glm::vec3(1.0f);
		uboSaturn.mvpMat = Prj * View * World;
		uboSaturn.mMat = World;
		uboSaturn.nMat = glm::inverse(glm::transpose(World));
		DSSaturn.map(currentImage, &uboSaturn, sizeof(uboSaturn), 0);

		World = glm::translate(World, glm::vec3(16, 0, -3)); // Position for Uranus
		uboUranus.amb = 1.0f;
		uboUranus.gamma = 180.0f;
		uboUranus.sColor = glm::vec3(1.0f);
		uboUranus.mvpMat = Prj * View * World;
		uboUranus.mMat = World;
		uboUranus.nMat = glm::inverse(glm::transpose(World));
		DSUranus.map(currentImage, &uboUranus, sizeof(uboUranus), 0);

		World = glm::translate(World, glm::vec3(18, 0, -3)); // Position for Neptune
		uboNeptune.amb = 1.0f;
		uboNeptune.gamma = 180.0f;
		uboNeptune.sColor = glm::vec3(1.0f);
		uboNeptune.mvpMat = Prj * View * World;
		uboNeptune.mMat = World;
		uboNeptune.nMat = glm::inverse(glm::transpose(World));
		DSNeptune.map(currentImage, &uboNeptune, sizeof(uboNeptune), 0);



		/*
		uboKey.visible = (gameState == 1) ? 1.0f : 0.0f;
		DSKey.map(currentImage, &uboKey, sizeof(uboKey), 0);

		uboSplash.visible = (gameState == 0) ? 1.0f : 0.0f;
		DSSplash.map(currentImage, &uboSplash, sizeof(uboSplash), 0);
		*/
	}

	/*void createPlanetMesh(float radius, std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx) {
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
	}*/
	void createPlanetMesh(float radius, std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx);
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