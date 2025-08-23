#version 120
uniform sampler2D texture;
uniform vec2 resolution;

uniform sampler2D wavesNoiseTex;
uniform float time;

vec4 get(float x, float y)
{
    return texture2D(texture, (gl_TexCoord[0].xy + x/resolution.x + y/resolution.y));
}

void main(void)
{
    vec4 colHere = get(0, 0);
    vec4 col = colHere;
    vec4 wavesNoiseTexCol = texture2D(wavesNoiseTex, vec2(gl_TexCoord[0].x + time, gl_TexCoord[0].y + time));
    
    float reducedOffset = wavesNoiseTexCol.x / 70;

   	col = texture2D(texture,  gl_TexCoord[0].xy + vec2(reducedOffset, reducedOffset));

    gl_FragColor = col;
}