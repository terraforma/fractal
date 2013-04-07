#ifndef _SHADERDEF_H_
#define _SHADERDEF_H_

const char* TerrainVS = "void main() { \
    gl_FrontColor = gl_Color; \
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; \
}";

const char* TerrainFS = "void main() { \
    gl_FragColor = gl_Color; \
}";

#endif /* _SHADERDEF_H_ */
