#version 330 core

in vec2 UV;

out vec3 color;

uniform sampler2D renderTexture;
uniform sampler2D velocityTexture;
uniform bool velDebug;
uniform bool stretchDebug;

void main(){
	if(velDebug) {
		color = vec3(texture(velocityTexture, UV))*10;
	}
	else if (stretchDebug) {
		color = vec3(texture(renderTexture, UV));
	}
	else {
		color = vec3(0, 0, 0);

	   for (int i = 0; i < 10; ++i) {
		  color +=  texture(renderTexture, UV + ((i - 5) / 10.0f) * texture(velocityTexture, UV).xy).xyz;
	   }
	   color /= float(10);
	}
	
}
