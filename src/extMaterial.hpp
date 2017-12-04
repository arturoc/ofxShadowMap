#pragma once
#define GLM_META_PROG_HELPERS
#define GLM_FORCE_SWIZZLE
#define GLM_FORCE_SIZE_FUNC
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "ofMain.h"
#include "ofMaterial.h"
#include "extVectorMath.h"

/*
a dirty hack to hijack 0.9.X ofMaterial and execute 0.10.X ofMaterial code

stack trace when you call ofMaterial::begin() - that is,
overload only these called ofMaterial functions with updated 0.10.X codes

ofMaterial::begin()
	ofGLProgrammableRenderer::bind(const ofBaseMaterial & material)
		ofGLProgrammableRenderer::beginDefaultShader()
			ofMaterial::getShader(int textureTarget, ofGLProgrammableRenderer & renderer)
				ofMaterial::initShaders(ofGLProgrammableRenderer & renderer)
			ofGLProgrammableRenderer::bind(const ofShader & shader)
				ofGLProgrammableRenderer::uploadMatrices()
					ofBaseMaterial::uploadMatrices(const ofShader & shader,ofGLProgrammableRenderer & renderer)
				ofGLProgrammableRenderer::setDefaultUniforms()
					ofMaterial::updateMaterial(const ofShader & shader,ofGLProgrammableRenderer & renderer)
					ofMaterial::updateLights(const ofShader & shader,ofGLProgrammableRenderer & renderer)
end
*/
class extMaterial : public ofMaterial {
public:

	struct Settings : public ofMaterial::Data {
		ofFloatColor diffuse{ 0.8f, 0.8f, 0.8f, 1.0f }; ///< diffuse reflectance
		ofFloatColor ambient{ 0.2f, 0.2f, 0.2f, 1.0f }; //< ambient reflectance
		ofFloatColor specular{ 0.0f, 0.0f, 0.0f, 1.0f }; //< specular reflectance
		ofFloatColor emissive{ 0.0f, 0.0f, 0.0f, 1.0f }; //< emitted light intensity
		float shininess{ 0.2f }; //< specular exponent
		std::string postFragment;
		std::string customUniforms;
	};
	void setup(const extMaterial::Settings & settings);

	void setCustomUniform1f(const std::string & name, float value);
	void setCustomUniform2f(const std::string & name, glm::vec2 value);
	void setCustomUniform3f(const std::string & name, glm::vec3 value);
	void setCustomUniform4f(const std::string & name, glm::vec4 value);
	void setCustomUniformMatrix4f(const std::string & name, glm::mat4 value);
	void setCustomUniformMatrix3f(const std::string & name, glm::mat3 value);

	void setCustomUniform1i(const std::string & name, int value);
	void setCustomUniform2i(const std::string & name, glm::tvec2<int> value);
	void setCustomUniform3i(const std::string & name, glm::tvec3<int> value);
	void setCustomUniform4i(const std::string & name, glm::tvec4<int> value);
	void setCustomUniformTexture(const std::string & name, const ofTexture & value, int textureLocation);
	void setCustomUniformTexture(const std::string & name, int textureTarget, GLint textureID, int textureLocation);

private:
	void initShaders(ofGLProgrammableRenderer & renderer) const;
	const ofShader & getShader(int textureTarget, bool geometryHasColor, ofGLProgrammableRenderer & renderer) const;
	const ofShader & getShader(int textureTarget, ofGLProgrammableRenderer & renderer) const;
	void updateMaterial(const ofShader & shader, ofGLProgrammableRenderer & renderer) const;
	void updateLights(const ofShader & shader, ofGLProgrammableRenderer & renderer) const;

	Settings data;

	struct Shaders {
		ofShader noTexture;
		ofShader color;
		ofShader texture2DColor;
		ofShader textureRectColor;
		ofShader texture2D;
		ofShader textureRect;
		size_t numLights;
	};
	struct TextureUnifom {
		int textureTarget;
		GLint textureID;
		int textureLocation;
	};

	mutable std::map<ofGLProgrammableRenderer*, std::shared_ptr<Shaders>> shaders;
	static std::map<ofGLProgrammableRenderer*, std::map<std::string, std::weak_ptr<Shaders>>> shadersMap;
	static std::string vertexShader;
	static std::string fragmentShader;
	std::map<std::string, float> uniforms1f;
	std::map<std::string, glm::vec2> uniforms2f;
	std::map<std::string, glm::vec3> uniforms3f;
	std::map<std::string, glm::vec4> uniforms4f;
	std::map<std::string, float> uniforms1i;
	std::map<std::string, glm::tvec2<int>> uniforms2i;
	std::map<std::string, glm::tvec3<int>> uniforms3i;
	std::map<std::string, glm::tvec4<int>> uniforms4i;
	std::map<std::string, glm::mat4> uniforms4m;
	std::map<std::string, glm::mat3> uniforms3m;
	std::map<std::string, TextureUnifom> uniformstex;
};