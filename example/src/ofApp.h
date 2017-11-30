#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxShadowMap.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		ofxAssimpModelLoader model;
		ofxShadowMap shadowMap;
		ofEasyCam camera;
		ofLight light;

		ofParameter<float> fustrumSize{"fustrum size", 100, 10, 1000};
		ofParameter<float> farClip{"far clip", 300, 10, 1000};
		ofParameter<bool> enableShadows{"enable shaodws", true};
		ofParameterGroup parameters{
			"parameters",
			fustrumSize,
			farClip,
			enableShadows,
			shadowMap.parameters,
		};
		ofxPanel gui{parameters};

		ofPlanePrimitive ground{400,400,2,2};
		ofMaterial groundMaterial;
#if OF_VER_09X
		void listenerFunction(bool& on) {
			if (!on) {
				shadowMap.begin(light, fustrumSize, 1, farClip);
				shadowMap.end();
			}
		}
#else
		ofEventListener listener;
#endif
};
