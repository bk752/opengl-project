#version 330 core

in vec2 UV;

out vec3 color;

uniform sampler2D renderTexture;
uniform sampler2D velocityTexture;


void main(){
	color = vec3(0, 0, 0);
   for (int i = 0; i < 10; ++i) {
      color +=  texture(renderTexture, UV + ((i - 5) / 10.0f) * texture(velocityTexture, UV).xy).xyz;
   }
   color /= float(10);
}
