#pragma once

#include "ofConstants.h"
#include "ofShader.h"
#include "ofFbo.h"
#include "ofParameter.h"

#ifndef OF_VER_09X
	#define OF_VER_09X (OF_VERSION_MAJOR < 1 && OF_VERSION_MINOR < 10)
#endif // !OF_VER_09X

#if OF_VER_09X
	#include "extMaterial.hpp"
#endif

class ofxShadowMap{
public:
	enum Resolution{
		_32,
		_24,
		_16,
	};
	bool setup(int size, Resolution resolution = _32);
	void setupMaterialWithShadowMap(ofMaterial & material);

	void begin(ofLight & light, float fustrumSize, float nearClip, float farClip);
	void end();

	void updateMaterial(ofMaterial & material);
	const ofTexture & getDepthTexture() const;
	ofTexture & getDepthTexture();

	ofParameter<float> shadowSub{"darken", 0.5, 0, 1};
	ofParameter<float> biasFactor{"bias", 0.005, 0, 1};
	ofParameter<bool> hardShadows{"hard shadows", true};
	ofParameter<float> shadowSoftScatter{"soft shadows scatter", 300, 0, 1000};
	ofParameterGroup parameters{
		"shadows",
		shadowSub,
		biasFactor,
		hardShadows,
		shadowSoftScatter,
	};
private:
	ofShader writeMapShader;
	ofFbo fbo;
	glm::mat4 lastBiasedMatrix;
	static void deleteSampler(GLuint * sampler){
		glDeleteSamplers(1, sampler);
	}
	std::unique_ptr<GLuint, decltype(&deleteSampler)> samplerID{new GLuint, &deleteSampler};
#if OF_VER_09X
	std::unordered_map<ofMaterial*, extMaterial> proxyMaterials;
public:
	auto& getShadowedMaterial(ofMaterial& material) {
		return proxyMaterials[&material];
	}
	auto& operator[](ofMaterial& material) {
		return getShadowedMaterial(material);
	}
#endif
};
