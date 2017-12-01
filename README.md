# ofxShadowMap

Original credit goes to https://github.com/arturoc/ofxShadowMap.

Switch to and check out ***Windows_OF_0.9.8** branch*.  
A workaround for openframeworks 0.9.8 is implemented (Original source depends on experimental OF 0.10.X).  
Also bugs and errors that renders wrong shadow texture sampling and mapping are fixed, though not sure whether Windows or OF being not 0.10.X is the culprit for the mess.  
​
Shadow map using PCF or gaussian disk sampling for soft borders.

This openFrameworks addon allows to render shadows using ofMaterial and ofLight. Right now only 1 directional light is supported. In the future this might become part of the core materials allowing more lights and light types.

![Screenshot](screenshot.png)
