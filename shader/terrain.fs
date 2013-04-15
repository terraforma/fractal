uniform sampler2D tf_GrassTexture;
uniform sampler2D tf_WaterTexture;
varying float height;
varying float lightFactor;
void main() {
    vec4 textureColor;
    if (height == 0.0f) {
        textureColor = texture2D(tf_WaterTexture, vec2(gl_TexCoord[0]));
    } else {
        textureColor = texture2D(tf_GrassTexture, vec2(gl_TexCoord[0]));
    }
    gl_FragColor = textureColor * lightFactor;
}