#include "ofxShadowMap.h"
#include "ofLight.h"
#include "ofMaterial.h"
#include "ofGraphics.h"
#include "glm/gtc/matrix_transform.hpp"

bool ofxShadowMap::setup(int size, Resolution resolution){
	bool success = writeMapShader.setupShaderFromSource(GL_VERTEX_SHADER,
		R"(
		#version 330

		in vec4 position;
		uniform mat4 modelViewProjectionMatrix;

		void main(){
			gl_Position = modelViewProjectionMatrix * position;
		}
		)");

	success &= writeMapShader.setupShaderFromSource(GL_FRAGMENT_SHADER,
		R"(
		#version 330

		out float fragDepth;


		void main(){
			fragDepth = gl_FragCoord.z;
		}
		)");

	success &= writeMapShader.linkProgram();


	ofFbo::Settings fboSettings;
	fboSettings.depthStencilAsTexture = true;
	if(resolution == _32){
		fboSettings.depthStencilInternalFormat = GL_DEPTH_COMPONENT32;
	}else if(resolution == _24){
		fboSettings.depthStencilInternalFormat = GL_DEPTH_COMPONENT24;
	}else{
		fboSettings.depthStencilInternalFormat = GL_DEPTH_COMPONENT16;
	}
	fboSettings.width = size;
	fboSettings.height = size;
	fboSettings.minFilter = GL_NEAREST;
	fboSettings.maxFilter = GL_NEAREST;
	fboSettings.numColorbuffers = 0;
	fboSettings.textureTarget = GL_TEXTURE_2D;
	fboSettings.useDepth = true;
	fboSettings.useStencil = false;
	fboSettings.wrapModeHorizontal = GL_CLAMP;
	fboSettings.wrapModeVertical = GL_CLAMP;
	fbo.allocate(fboSettings);
	fbo.getDepthTexture().setRGToRGBASwizzles(true);

	glGenSamplers(1, samplerID.get());
	glSamplerParameteri(*samplerID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(*samplerID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(*samplerID, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glSamplerParameteri(*samplerID, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glSamplerParameteri(*samplerID, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glSamplerParameteri(*samplerID, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

//	fbo.getDepthTexture().bind();
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
//	fbo.getDepthTexture().unbind();

	return success;
}

void ofxShadowMap::setupMaterialWithShadowMap(ofMaterial & material){
	constexpr const char * visibilityFunc = R"(
		vec2 poissonDisk64[64] = vec2[](
		   vec2(-0.613392, 0.617481),
		   vec2(0.170019, -0.040254),
		   vec2(-0.299417, 0.791925),
		   vec2(0.645680, 0.493210),
		   vec2(-0.651784, 0.717887),
		   vec2(0.421003, 0.027070),
		   vec2(-0.817194, -0.271096),
		   vec2(-0.705374, -0.668203),
		   vec2(0.977050, -0.108615),
		   vec2(0.063326, 0.142369),
		   vec2(0.203528, 0.214331),
		   vec2(-0.667531, 0.326090),
		   vec2(-0.098422, -0.295755),
		   vec2(-0.885922, 0.215369),
		   vec2(0.566637, 0.605213),
		   vec2(0.039766, -0.396100),
		   vec2(0.751946, 0.453352),
		   vec2(0.078707, -0.715323),
		   vec2(-0.075838, -0.529344),
		   vec2(0.724479, -0.580798),
		   vec2(0.222999, -0.215125),
		   vec2(-0.467574, -0.405438),
		   vec2(-0.248268, -0.814753),
		   vec2(0.354411, -0.887570),
		   vec2(0.175817, 0.382366),
		   vec2(0.487472, -0.063082),
		   vec2(-0.084078, 0.898312),
		   vec2(0.488876, -0.783441),
		   vec2(0.470016, 0.217933),
		   vec2(-0.696890, -0.549791),
		   vec2(-0.149693, 0.605762),
		   vec2(0.034211, 0.979980),
		   vec2(0.503098, -0.308878),
		   vec2(-0.016205, -0.872921),
		   vec2(0.385784, -0.393902),
		   vec2(-0.146886, -0.859249),
		   vec2(0.643361, 0.164098),
		   vec2(0.634388, -0.049471),
		   vec2(-0.688894, 0.007843),
		   vec2(0.464034, -0.188818),
		   vec2(-0.440840, 0.137486),
		   vec2(0.364483, 0.511704),
		   vec2(0.034028, 0.325968),
		   vec2(0.099094, -0.308023),
		   vec2(0.693960, -0.366253),
		   vec2(0.678884, -0.204688),
		   vec2(0.001801, 0.780328),
		   vec2(0.145177, -0.898984),
		   vec2(0.062655, -0.611866),
		   vec2(0.315226, -0.604297),
		   vec2(-0.780145, 0.486251),
		   vec2(-0.371868, 0.882138),
		   vec2(0.200476, 0.494430),
		   vec2(-0.494552, -0.711051),
		   vec2(0.612476, 0.705252),
		   vec2(-0.578845, -0.768792),
		   vec2(-0.772454, -0.090976),
		   vec2(0.504440, 0.372295),
		   vec2(0.155736, 0.065157),
		   vec2(0.391522, 0.849605),
		   vec2(-0.620106, -0.328104),
		   vec2(0.789239, -0.419965),
		   vec2(-0.545396, 0.538133),
		   vec2(-0.178564, -0.596057)
		);

		vec2 poissonDisk16[16] = vec2[](
		  vec2( -0.94201624, -0.39906216 ),
		  vec2( 0.94558609, -0.76890725 ),
		  vec2( -0.094184101, -0.92938870 ),
		  vec2( 0.34495938, 0.29387760 ),
		  vec2( -0.91588581, 0.45771432 ),
		  vec2( -0.81544232, -0.87912464 ),
		  vec2( -0.38277543, 0.27676845 ),
		  vec2( 0.97484398, 0.75648379 ),
		  vec2( 0.44323325, -0.97511554 ),
		  vec2( 0.53742981, -0.47373420 ),
		  vec2( -0.26496911, -0.41893023 ),
		  vec2( 0.79197514, 0.19090188 ),
		  vec2( -0.24188840, 0.99706507 ),
		  vec2( -0.81409955, 0.91437590 ),
		  vec2( 0.19984126, 0.78641367 ),
		  vec2( 0.14383161, -0.14100790 )
		);

		vec2 poissonDisk4[4] = vec2[](
		 vec2( -0.94201624, -0.39906216 ),
		 vec2( 0.94558609, -0.76890725 ),
		 vec2( -0.094184101, -0.92938870 ),
		 vec2( 0.34495938, 0.29387760 )
		);

		float random(vec4 seed4){
			float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
			return fract(sin(dot_product) * 43758.5453);
		}

		vec4 postFragment(vec4 localColor){
			float visibility = 1.0;
			vec3 VP = lights[0].position.xyz - v_eyePosition.xyz;
			VP = normalize(VP);
			float nDotVP = max(0.0, dot(v_transformedNormal, VP));
			float bias = biasFactor * tan(acos(nDotVP));
			bias = clamp(bias, 0., 0.01);
			vec3 shadow_coord = (biasedMvp * vec4(v_worldPosition, 1.0)).xyz;
			shadow_coord.y = 1 - shadow_coord.y;

			if(hardShadows > 0.5){
				visibility -= shadowSub * (1-texture( shadowMap, vec3(shadow_coord.xy, shadow_coord.z - bias) ));
			}else{
			   const int totalPasses = 16;
			   float passSub = shadowSub/float(totalPasses);
			   const bool bailEarly = true;
			   int numPassesEstimate = 4;
			   if(!bailEarly) numPassesEstimate = totalPasses;
			   for (int i=0;i<numPassesEstimate;i++){
				   //int index = i;
				   // int index = int(float(totalPasses)*random(vec4(floor(v_position.xyz*1000.0), i)))%16;
				   int index = int(float(totalPasses)*random(vec4(gl_FragCoord.xyy, i)))%totalPasses;
				   visibility -= passSub*(1.0-texture( shadowMap, vec3(shadow_coord.xy + poissonDisk16[index]/shadowSoftScatter, shadow_coord.z-bias) ));
			   }
			   if(bailEarly && visibility < 1.0){
				   if(visibility > 1.0 - (passSub * 4.0)){
					   for (int i=numPassesEstimate;i<totalPasses;i++){
						   // int index = i;
						   // int index = int(float(totalPasses)*random(vec4(floor(v_position.xyz*1000.0), i)))%totalPasses;
						   int index = int(float(totalPasses)*random(vec4(gl_FragCoord.xyy, i)))%totalPasses;
						   visibility -= passSub*(1.0-texture( shadowMap, vec3(shadow_coord.xy + poissonDisk16[index]/shadowSoftScatter, shadow_coord.z-bias) ));
					   }
				   }else{
					   visibility = 1.0 - shadowSub;
				   }
			   }
			}
			return localColor * visibility;
		}
	)";


	ofMaterialSettings settings;
	settings.ambient = material.getAmbientColor();
	settings.diffuse = material.getDiffuseColor();
	settings.emissive = material.getEmissiveColor();
	settings.specular = material.getSpecularColor();
	settings.shininess = material.getShininess();
	settings.customUniforms = R"(
		uniform sampler2DShadow shadowMap;
		uniform mat4 biasedMvp;
		uniform float shadowSub;
		uniform float hardShadows;
		uniform float biasFactor;
		uniform float shadowSoftScatter;
	)";
	settings.postFragment = visibilityFunc;
	material.setup(settings);
}


void ofxShadowMap::begin(ofLight & light, float fustrumSize, float nearClip, float farClip){
	float left = -fustrumSize / 2.;
	float right = fustrumSize / 2.;
	float top = fustrumSize / 2.;
	float bottom = -fustrumSize / 2.;
	auto ortho = glm::ortho(left, right, bottom, top, nearClip, farClip);
	auto view = glm::inverse(light.getGlobalTransformMatrix());
	auto viewProjection = ortho * view;
	auto bias = glm::mat4(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0);
	lastBiasedMatrix = bias * viewProjection;
	writeMapShader.begin();
	fbo.begin(OF_FBOMODE_NODEFAULTS);
	ofPushView();
	ofSetMatrixMode(OF_MATRIX_PROJECTION);
	ofLoadMatrix(ortho);
	ofSetMatrixMode(OF_MATRIX_MODELVIEW);
	ofLoadViewMatrix(view);
	ofViewport(ofRectangle(0,0,fbo.getWidth(),fbo.getHeight()));
	ofClear(0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
}

void ofxShadowMap::end(){
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	writeMapShader.end();
	ofPopView();
	fbo.end();
}

void ofxShadowMap::updateMaterial(ofMaterial & material){
	material.begin();
	material.setCustomUniformMatrix4f("biasedMvp", lastBiasedMatrix);
	material.setCustomUniform1f("shadowSub", shadowSub);
	material.setCustomUniform1f("hardShadows", hardShadows ? 1.f : 0.f);
	material.setCustomUniform1f("biasFactor", biasFactor);
	material.setCustomUniform1f("shadowSoftScatter", shadowSoftScatter);

//	material.setCustomUniformTexture("shadowMap", getDepthTexture(), 0);
	const int texUnit = 1;
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, fbo.getDepthTexture().getTextureData().textureID);
	glBindSampler(texUnit, *samplerID);
	material.setCustomUniform1i("shadowMap", texUnit);
	glActiveTexture(GL_TEXTURE0);
	material.end();
}

const ofTexture & ofxShadowMap::getDepthTexture() const{
	return fbo.getDepthTexture();
}

ofTexture & ofxShadowMap::getDepthTexture(){
	return fbo.getDepthTexture();
}
