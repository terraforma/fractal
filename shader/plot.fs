uniform sampler2D tf_ScraperTexture;
uniform vec4 tf_LightPos;
varying float lightFactor;
void main() {
    vec4 textureColor = texture2D(tf_ScraperTexture, vec2(gl_TexCoord[0]));
    gl_FragColor = textureColor * lightFactor;
}