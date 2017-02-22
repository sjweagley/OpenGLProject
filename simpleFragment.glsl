
# version 330 core

in vec3 Position;
in vec3 Normal;
in vec4 Color;
in vec2 TextCoord;
uniform vec3 HeadLightPosition;
uniform vec3 HeadLightIntensity;
uniform vec3 PointLightPosition;
uniform vec3 PointLightIntensity;
uniform bool HeadLightOn; // toggles set in application
uniform bool PointLightOn;
uniform bool AmbientOn; // glUniform1f(debugSetOn, debugOn);
uniform bool IsTexture;
uniform sampler2D Texture;
out vec4 FragColor;

vec3 vLight(vec3 LightPosition, vec3 LightIntensity,bool directional) {
	float ambient = 0.2f; // scale directional ambient
	float diffuse = 0.0f; // compute diffuse in all cases
	vec3 n, s; // normal, light source
	if (directional)
		s = normalize(LightPosition);
	else { // point light has no ambient
		s = normalize(LightPosition - Position);
		ambient = 0.0f; // no ambient w/ Point light
	}
	n = normalize(Normal);
	diffuse = max(dot(s, n), 0.0); // reflected light
	if(!AmbientOn){
		ambient = 0.0f;
	}
	return ambient * LightIntensity + diffuse * LightIntensity;
}

void main() {

	vec3 tempColor = vec3(Color) * 0.1f; // initial value
	if (HeadLightOn) 
		tempColor += vLight(HeadLightPosition,HeadLightIntensity, true);

	if (PointLightOn) 
		tempColor += vLight(PointLightPosition, PointLightIntensity, false);

	
	if (IsTexture) {
	FragColor = texture(Texture, TextCoord); 
	} else {
	FragColor = vec4(tempColor, 1.0);
	}

}
