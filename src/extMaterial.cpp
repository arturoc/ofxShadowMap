#include "extMaterial.hpp"

std::map<ofGLProgrammableRenderer*, std::map<std::string, std::weak_ptr<extMaterial::Shaders>>> extMaterial::shadersMap;

namespace {
	string vertexSource(string defaultHeader, int maxLights, bool hasTexture, bool hasColor);
	string fragmentSource(string defaultHeader, string customUniforms, string postFragment, int maxLights, bool hasTexture, bool hasColor);
}

void extMaterial::setup(const extMaterial::Settings & settings) {
	if (settings.customUniforms != data.customUniforms || settings.postFragment != data.postFragment) {
		shaders.clear();
		uniforms1f.clear();
		uniforms2f.clear();
		uniforms3f.clear();
		uniforms4f.clear();
		uniforms1i.clear();
		uniforms2i.clear();
		uniforms3i.clear();
		uniforms4i.clear();
	}
	data = settings;
}

void extMaterial::setCustomUniform1f(const std::string & name, float value) {
	uniforms1f[name] = value;
}

void extMaterial::setCustomUniform2f(const std::string & name, glm::vec2 value) {
	uniforms2f[name] = value;
}

void extMaterial::setCustomUniform3f(const std::string & name, glm::vec3 value) {
	uniforms3f[name] = value;
}

void extMaterial::setCustomUniform4f(const std::string & name, glm::vec4 value) {
	uniforms4f[name] = value;
}

void extMaterial::setCustomUniform1i(const std::string & name, int value) {
	uniforms1i[name] = value;
}

void extMaterial::setCustomUniform2i(const std::string & name, glm::tvec2<int> value) {
	uniforms2i[name] = value;
}

void extMaterial::setCustomUniform3i(const std::string & name, glm::tvec3<int> value) {
	uniforms3i[name] = value;
}

void extMaterial::setCustomUniform4i(const std::string & name, glm::tvec4<int> value) {
	uniforms4i[name] = value;
}

void extMaterial::setCustomUniformMatrix4f(const std::string & name, glm::mat4 value) {
	uniforms4m[name] = value;
}

void extMaterial::setCustomUniformMatrix3f(const std::string & name, glm::mat3 value) {
	uniforms3m[name] = value;
}

void extMaterial::setCustomUniformTexture(const std::string & name, const ofTexture & value, int textureLocation) {
	uniformstex[name] = { value.getTextureData().textureTarget, int(value.getTextureData().textureID), textureLocation };
}

void extMaterial::setCustomUniformTexture(const string & name, int textureTarget, GLint textureID, int textureLocation) {
	uniformstex[name] = { textureTarget, textureID, textureLocation };
}

void extMaterial::initShaders(ofGLProgrammableRenderer & renderer) const {
	auto rendererShaders = shaders.find(&renderer);
	if (rendererShaders == shaders.end() || rendererShaders->second->numLights != ofLightsData().size()) {
		if (shadersMap[&renderer].find(data.postFragment) != shadersMap[&renderer].end()) {
			auto newShaders = shadersMap[&renderer][data.postFragment].lock();
			if (newShaders == nullptr || newShaders->numLights != ofLightsData().size()) {
				shadersMap[&renderer].erase(data.postFragment);
				shaders[&renderer] = nullptr;
			}
			else {
				shaders[&renderer] = newShaders;
			}
		}
	}

	if (shaders[&renderer] == nullptr) {
#ifndef TARGET_OPENGLES
		string vertexRectHeader = renderer.defaultVertexShaderHeader(GL_TEXTURE_RECTANGLE);
		string fragmentRectHeader = renderer.defaultFragmentShaderHeader(GL_TEXTURE_RECTANGLE);
#endif
		string vertex2DHeader = renderer.defaultVertexShaderHeader(GL_TEXTURE_2D);
		string fragment2DHeader = renderer.defaultFragmentShaderHeader(GL_TEXTURE_2D);
		auto numLights = ofLightsData().size();
		shaders[&renderer].reset(new Shaders);
		shaders[&renderer]->numLights = numLights;
		shaders[&renderer]->noTexture.setupShaderFromSource(GL_VERTEX_SHADER, vertexSource(vertex2DHeader, numLights, false, false));
		shaders[&renderer]->noTexture.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentSource(fragment2DHeader, data.customUniforms, data.postFragment, numLights, false, false));
		shaders[&renderer]->noTexture.bindDefaults();
		shaders[&renderer]->noTexture.linkProgram();

		shaders[&renderer]->texture2D.setupShaderFromSource(GL_VERTEX_SHADER, vertexSource(vertex2DHeader, numLights, true, false));
		shaders[&renderer]->texture2D.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentSource(fragment2DHeader, data.customUniforms, data.postFragment, numLights, true, false));
		shaders[&renderer]->texture2D.bindDefaults();
		shaders[&renderer]->texture2D.linkProgram();

#ifndef TARGET_OPENGLES
		shaders[&renderer]->textureRect.setupShaderFromSource(GL_VERTEX_SHADER, vertexSource(vertexRectHeader, numLights, true, false));
		shaders[&renderer]->textureRect.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentSource(fragmentRectHeader, data.customUniforms, data.postFragment, numLights, true, false));
		shaders[&renderer]->textureRect.bindDefaults();
		shaders[&renderer]->textureRect.linkProgram();
#endif

		shaders[&renderer]->color.setupShaderFromSource(GL_VERTEX_SHADER, vertexSource(vertex2DHeader, numLights, false, true));
		shaders[&renderer]->color.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentSource(fragment2DHeader, data.customUniforms, data.postFragment, numLights, false, true));
		shaders[&renderer]->color.bindDefaults();
		shaders[&renderer]->color.linkProgram();


		shaders[&renderer]->texture2DColor.setupShaderFromSource(GL_VERTEX_SHADER, vertexSource(vertex2DHeader, numLights, true, true));
		shaders[&renderer]->texture2DColor.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentSource(fragment2DHeader, data.customUniforms, data.postFragment, numLights, true, true));
		shaders[&renderer]->texture2DColor.bindDefaults();
		shaders[&renderer]->texture2DColor.linkProgram();

#ifndef TARGET_OPENGLES
		shaders[&renderer]->textureRectColor.setupShaderFromSource(GL_VERTEX_SHADER, vertexSource(vertexRectHeader, numLights, true, true));
		shaders[&renderer]->textureRectColor.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentSource(fragmentRectHeader, data.customUniforms, data.postFragment, numLights, true, true));
		shaders[&renderer]->textureRectColor.bindDefaults();
		shaders[&renderer]->textureRectColor.linkProgram();
#endif

		shadersMap[&renderer][data.postFragment] = shaders[&renderer];
	}

}

const ofShader & extMaterial::getShader(int textureTarget, ofGLProgrammableRenderer & renderer) const {
	bool geometryHasColor = false;
	return getShader(textureTarget, geometryHasColor, renderer);
}

const ofShader& extMaterial::getShader(int textureTarget, bool geometryHasColor, ofGLProgrammableRenderer & renderer) const {
	initShaders(renderer);
	switch (textureTarget) {
	case OF_NO_TEXTURE:
		if (geometryHasColor) {
			return shaders[&renderer]->color;
		}
		else {
			return shaders[&renderer]->noTexture;
		}
		break;
	case GL_TEXTURE_2D:
		if (geometryHasColor) {
			return shaders[&renderer]->texture2DColor;
		}
		else {
			return shaders[&renderer]->texture2D;
		}
		break;
	default:
		if (geometryHasColor) {
			return shaders[&renderer]->textureRectColor;
		}
		else {
			return shaders[&renderer]->textureRect;
		}
		break;
	}
}

void extMaterial::updateMaterial(const ofShader & shader, ofGLProgrammableRenderer & renderer) const {
	shader.setUniform4fv("mat_ambient", &data.ambient.r);
	shader.setUniform4fv("mat_diffuse", &data.diffuse.r);
	shader.setUniform4fv("mat_specular", &data.specular.r);
	shader.setUniform4fv("mat_emissive", &data.emissive.r);
	shader.setUniform4fv("global_ambient", &ofGetGlobalAmbientColor().r);
	shader.setUniform1f("mat_shininess", data.shininess);
	for (auto & uniform : uniforms1f) {
		shader.setUniform1f(uniform.first, uniform.second);
	}
	for (auto & uniform : uniforms2f) {
		shader.setUniform2f(uniform.first, toOf(uniform.second));
	}
	for (auto & uniform : uniforms3f) {
		shader.setUniform3f(uniform.first, toOf(uniform.second));
	}
	for (auto & uniform : uniforms4f) {
		shader.setUniform4f(uniform.first, toOf(uniform.second));
	}
	for (auto & uniform : uniforms1i) {
		shader.setUniform1i(uniform.first, uniform.second);
	}
	for (auto & uniform : uniforms2i) {
		shader.setUniform2i(uniform.first, uniform.second.x, uniform.second.y);
	}
	for (auto & uniform : uniforms3i) {
		shader.setUniform3i(uniform.first, uniform.second.x, uniform.second.y, uniform.second.z);
	}
	for (auto & uniform : uniforms4i) {
		shader.setUniform4i(uniform.first, uniform.second.x, uniform.second.y, uniform.second.z, uniform.second.w);
	}
	for (auto & uniform : uniforms4m) {
		shader.setUniformMatrix4f(uniform.first, toOf(uniform.second));
	}
	for (auto & uniform : uniforms3m) {
		shader.setUniformMatrix3f(uniform.first, toOf(uniform.second));
	}
	for (auto & uniform : uniformstex) {
		shader.setUniformTexture(uniform.first,
			uniform.second.textureTarget,
			uniform.second.textureID,
			uniform.second.textureLocation);
	}
}

void extMaterial::updateLights(const ofShader & shader, ofGLProgrammableRenderer & renderer) const {
	for (size_t i = 0; i<ofLightsData().size(); i++) {
		string idx = ofToString(i);
		shared_ptr<ofLight::Data> light = ofLightsData()[i].lock();
		if (!light || !light->isEnabled) {
			shader.setUniform1f("lights[" + idx + "].enabled", 0);
			continue;
		}
		auto lightEyePosition = toGlm(renderer.getCurrentViewMatrix()) * toGlm(light->position);
		shader.setUniform1f("lights[" + idx + "].enabled", 1);
		shader.setUniform1f("lights[" + idx + "].type", light->lightType);
		shader.setUniform4f("lights[" + idx + "].position", toOf(lightEyePosition));
		shader.setUniform4f("lights[" + idx + "].ambient", light->ambientColor);
		shader.setUniform4f("lights[" + idx + "].specular", light->specularColor);
		shader.setUniform4f("lights[" + idx + "].diffuse", light->diffuseColor);

		if (light->lightType != OF_LIGHT_DIRECTIONAL) {
			shader.setUniform1f("lights[" + idx + "].constantAttenuation", light->attenuation_constant);
			shader.setUniform1f("lights[" + idx + "].linearAttenuation", light->attenuation_linear);
			shader.setUniform1f("lights[" + idx + "].quadraticAttenuation", light->attenuation_quadratic);
		}

		if (light->lightType == OF_LIGHT_SPOT) {
			auto direction = toGlm(light->position).xyz() + light->direction;
			auto direction4 = toGlm(renderer.getCurrentViewMatrix()) * glm::vec4(direction, 1.0);
			direction = direction4.xyz() / direction4.w;
			direction = direction - toGlm(lightEyePosition).xyz();
			shader.setUniform3f("lights[" + idx + "].spotDirection", toOf(glm::normalize(direction)));
			shader.setUniform1f("lights[" + idx + "].spotExponent", light->exponent);
			shader.setUniform1f("lights[" + idx + "].spotCutoff", light->spotCutOff);
			shader.setUniform1f("lights[" + idx + "].spotCosCutoff", cos(ofDegToRad(light->spotCutOff)));
		}
		else if (light->lightType == OF_LIGHT_DIRECTIONAL) {
			auto halfVector = glm::normalize(glm::vec4(0.f, 0.f, 1.f, 0.f) + toGlm(lightEyePosition));
			shader.setUniform3f("lights[" + idx + "].halfVector", toOf(halfVector.xyz()));
		}
		else if (light->lightType == OF_LIGHT_AREA) {
			shader.setUniform1f("lights[" + idx + "].width", light->width);
			shader.setUniform1f("lights[" + idx + "].height", light->height);
			auto direction = toGlm(light->position).xyz() + light->direction;
			auto direction4 = toGlm(renderer.getCurrentViewMatrix()) * glm::vec4(direction, 1.0);
			direction = direction4.xyz() / direction4.w;
			direction = direction - toGlm(lightEyePosition).xyz();
			shader.setUniform3f("lights[" + idx + "].spotDirection", toOf(glm::normalize(direction)));
			auto right = toGlm(light->position).xyz() + light->right;
			auto right4 = toGlm(renderer.getCurrentViewMatrix()) * glm::vec4(right, 1.0);
			right = right4.xyz() / right4.w;
			right = right - toGlm(lightEyePosition).xyz();
			auto up = glm::cross(toGlm(right), direction);
			shader.setUniform3f("lights[" + idx + "].right", toOf(glm::normalize(toGlm(right))));
			shader.setUniform3f("lights[" + idx + "].up", toOf(glm::normalize(up)));
		}
	}
}

#include "shaders/phong.vert"
#include "shaders/phong.frag"

namespace {
	string shaderHeader(string header, int maxLights, bool hasTexture, bool hasColor) {
		header += "#define MAX_LIGHTS " + ofToString(max(1, maxLights)) + "\n";
		if (hasTexture) {
			header += "#define HAS_TEXTURE 1\n";
		}
		else {
			header += "#define HAS_TEXTURE 0\n";
		}
		if (hasColor) {
			header += "#define HAS_COLOR 1\n";
		}
		else {
			header += "#define HAS_COLOR 0\n";
		}
		return header;
	}

	string vertexSource(string defaultHeader, int maxLights, bool hasTexture, bool hasColor) {
		return shaderHeader(defaultHeader, maxLights, hasTexture, hasColor) + vertexShader;
	}

	string fragmentSource(string defaultHeader, string customUniforms, string postFragment, int maxLights, bool hasTexture, bool hasColor) {
		auto source = fragmentShader;
		if (postFragment.empty()) {
			postFragment = "vec4 postFragment(vec4 localColor){ return localColor; }";
		}
		ofStringReplace(source, "%postFragment%", postFragment);
		ofStringReplace(source, "%custom_uniforms%", customUniforms);

		source = shaderHeader(defaultHeader, maxLights, hasTexture, hasColor) + source;
		return source;
	}
}