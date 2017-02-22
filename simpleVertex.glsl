
# version 330 core
in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;
in vec2 vTextCoord;
out vec3 Position;
out vec3 Normal;
out vec4 Color;
out vec2 TextCoord;
uniform mat3 NormalMatrix;
uniform mat4 ModelView;
uniform mat4 MVP;
uniform bool IsTexture;	

void main() {
	gl_Position = MVP * vPosition;
	Position = (ModelView * vPosition).xyz;
	Normal = normalize(NormalMatrix * vNormal);
	
	if (IsTexture) {
		TextCoord = vTextCoord;
		Color = vec4(0.0, 0, 0, 0);
	} 
	else {
		TextCoord = vec2(0, 0);
		Color = vColor;
	}
}