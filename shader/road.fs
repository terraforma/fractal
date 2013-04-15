uniform sampler2D tf_RoadTexture;
void main() {
    vec4 textureColor = texture2D(tf_RoadTexture, vec2(gl_TexCoord[0]));
    gl_FragColor = textureColor;
}