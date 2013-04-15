uniform vec4 tf_LightPos;
varying float lightFactor;
varying float height;
void main() {
    vec3 normal = normalize(gl_NormalMatrix*gl_Normal).xyz;
    vec3 vert = (gl_ModelViewMatrix * gl_Vertex).xyz;
    vec3 lightPos = (gl_ModelViewMatrix * tf_LightPos).xyz;
    vec3 lightDir = normalize(lightPos-vert).xyz;
    lightFactor = dot(lightDir, normal);
    height = gl_Vertex.z;
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}