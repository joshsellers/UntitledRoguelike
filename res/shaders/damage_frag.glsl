#version 120
uniform sampler2D texture;
uniform vec2 resolution;

vec4 get(float x, float y)
{
    return texture2D(texture, (gl_TexCoord[0].xy + x/resolution.x + y/resolution.y));
}

void main(void)
{
    vec4 colHere = get(0, 0);
    vec4 col = colHere;
    if (colHere.a > 0) {
        col.r += 0.3;
    }
    gl_FragColor = col;
}