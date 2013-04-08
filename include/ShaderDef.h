#ifndef _SHADERDEF_H_
#define _SHADERDEF_H_

const char* TerrainVS = "uniform vec4 tf_LightPos; \
varying float lightFactor; \
void main() { \
    gl_TexCoord[0] = gl_MultiTexCoord0; \
    vec4 vl = normalize(tf_LightPos - gl_Vertex); \
    lightFactor = dot(vl, vec4(gl_Normal, 0.0f)); \
    gl_FrontColor = gl_Color; \
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; \
}";

const char* TerrainFS = "uniform sampler2D tf_GrassTexture; \
varying float lightFactor; \
void main() { \
    vec4 textureColor = texture2D(tf_GrassTexture, vec2(gl_TexCoord[0])); \
    gl_FragColor = textureColor * lightFactor; \
}";

#endif /* _SHADERDEF_H_ */
