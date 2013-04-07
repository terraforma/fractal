#ifndef _SHADERDEF_H_
#define _SHADERDEF_H_

const char* TerrainVS = "uniform vec4 tf_LightPos; \
varying float lightFactor; \
void main() { \
    vec4 vl = normalize(tf_LightPos - gl_Vertex); \
    lightFactor = dot(vl, vec4(gl_Normal, 0.0f)); \
    gl_FrontColor = gl_Color; \
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; \
}";

const char* TerrainFS = "varying float lightFactor; \
void main() { \
    gl_FragColor = gl_Color * lightFactor; \
}";

#endif /* _SHADERDEF_H_ */
