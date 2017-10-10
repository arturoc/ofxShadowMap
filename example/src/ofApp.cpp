#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	model.loadModel("amua.obj");
	model.setRotation(0,180,0,0,1);
	model.setScaleNormalization(false);
	shadowMap.setup(4096);

	for(size_t i=0; i<model.getNumMeshes(); i++){
		shadowMap.setupMaterialWithShadowMap(model.getMeshHelper(i).material);
	}

	shadowMap.setupMaterialWithShadowMap(groundMaterial);

	light.enable();
	camera.setDistance(200);
	ground.setOrientation(glm::angleAxis(ofDegToRad(-90.f), glm::vec3{1.f,0.f,0.f}));

	listener = enableShadows.newListener([this](bool & on){
		if(!on){
			shadowMap.begin(light, fustrumSize, 1, farClip);
			shadowMap.end();
		}
	});
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofEnableDepthTest();

//	light.setPosition(-200,200,200);
//	light.lookAt({0,0,0});
	float longitude = ofMap(ofGetMouseX(), 0, ofGetWidth(), -90, 90);
	float latitude = ofMap(ofGetMouseY(), 0, ofGetHeight(), -60, -10);
	light.orbitDeg(longitude, latitude, 200, {0,20,0});

	if(enableShadows){
		shadowMap.begin(light, fustrumSize, 1, farClip);
		model.drawFaces();
		shadowMap.end();

		for(size_t i=0; i<model.getNumMeshes(); i++){
			shadowMap.updateMaterial(model.getMeshHelper(i).material);
		}
		shadowMap.updateMaterial(groundMaterial);
	}

	camera.begin();
	groundMaterial.begin();
	ground.draw();
	groundMaterial.end();
	model.drawFaces();
	light.draw();
	camera.end();

	ofDisableDepthTest();
	shadowMap.getDepthTexture().draw(ofGetWidth()-256, 0, 256, 256);
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
