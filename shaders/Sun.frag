#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 1) uniform sampler2D tex;

layout(set = 1, binding = 2) uniform GlobalUniformBufferObject {
	vec3 lightPos;	// position of the point light
	vec4 lightColor;// color of the point light
	vec3 eyePos;	// position of the viewer
} gubo;

layout(set = 1, binding = 3) uniform sampler2D texEmit;

const float beta = 2.0f;	// decay exponent of the point light
const float g = 20;		// target distance of the point light
const float gamma = 160.0f;	// cosine power for the Blinn specular reflection

// coefficients for the spehrical harmonics ambient light term
const vec3 C00  = vec3( .38f, .43f, .45f)/8.0f;
const vec3 C1m1 = vec3( .29f, .36f, .41f)/8.0f;
const vec3 C10  = vec3( .04f, .03f, .01f)/8.0f;
const vec3 C11  = vec3(-.10f,-.10f,-.09f)/8.0f;
const vec3 C2m2 = vec3(-.06f,-.06f,-.04f)/8.0f;
const vec3 C2m1 = vec3( .01f,-.01f,-.05f)/8.0f;
const vec3 C20  = vec3(-.09f,-.13f,-.15f)/8.0f;
const vec3 C21  = vec3(-.06f,-.05f,-.04f)/8.0f;
const vec3 C22  = vec3( .02f, .00f,-.05f)/8.0f;

vec3 lightModelColor() {
    // point light
    float tmp = g/length(gubo.lightPos - fragPos);
	return vec3(gubo.lightColor)*pow(tmp, beta);
}

vec3 lightModelDirection() {
    return normalize(gubo.lightPos - fragPos);
}

vec3 LambertDiffuse(vec3 L, vec3 N, vec3 Md) {
	vec3 f_diffuse = Md*max(dot(L,N),0);
	return f_diffuse;
}

vec3 BlinnSpec(vec3 L, vec3 N, vec3 V, vec3 Ms, float gamma) {
	vec3 H = normalize(L+V);
	vec3 f_specular = Ms*pow(clamp(dot(N, H), 0, 1), gamma);
	return f_specular;
}

vec3 AmbientLight(vec3 Ma, vec3 Me, vec3 N) {
	vec3 l_A = C00 + N.x*C11 + N.y*C1m1+N.x*C10 +
		(N.x*N.y)*C2m2 + (N.y*N.z)*C1m1 + (N.z*N.x)*C11 +
		(N.x*N.x - N.y*N.y)*C22 + (3*N.z*N.z - 1)*C20;
	return Ma * l_A + Me;
}

void main() {
	vec3 N = normalize(fragNorm);				// surface normal
	vec3 V = normalize(gubo.eyePos - fragPos);	// viewer direction
	vec3 MD = texture(tex, fragUV).rgb;			// diffuse color
	vec3 MA = MD;								// ambient color
	vec3 MS = vec3(1);							// specular color
	vec3 ME = texture(texEmit, fragUV).rgb;		// emission color

    // pointlight
	vec3 lightColor = lightModelColor();
	vec3 L = lightModelDirection();

    // no specular since planet
	vec3 DiffSpec = LambertDiffuse(L, N, MD) + BlinnSpec(L, N, V, MS, gamma); // arbitrary gamma
	vec3 Ambient = AmbientLight(MD, ME, N);
	
	outColor = vec4(clamp(0.95 * DiffSpec * lightColor.rgb + Ambient,0.0,1.0), 1.0f);
}