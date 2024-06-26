
#version 120
uniform sampler2D texture;
uniform vec2 resolution;

uniform sampler2D wavesNoiseTex;
uniform float time;

uniform sampler2D capsNoiseTex;
uniform float capsTime;

vec4 get(float x, float y)
{
    return texture2D(texture, (gl_TexCoord[0].xy + x/resolution.x + y/resolution.y));
}

void main(void)
{
    vec4 colHere = get(0, 0);
    vec4 col = colHere;
    if (colHere.b > colHere.r && colHere.b > colHere.g) {
	    vec4 wavesNoiseTexCol = texture2D(wavesNoiseTex, vec2(gl_TexCoord[0].x + time, gl_TexCoord[0].y + time));
	
	    float reducedOffset = wavesNoiseTexCol.x / 70;

   	    col = texture2D(texture,  gl_TexCoord[0].xy + vec2(reducedOffset, reducedOffset));
        
        
        if (!(col.b > col.r && col.b > col.g)) col = colHere;
        
        vec4 capsNoiseTexCol = texture2D(capsNoiseTex, vec2(gl_TexCoord[0].x - capsTime, gl_TexCoord[0].y - capsTime));
        if (capsNoiseTexCol == vec4(1, 1, 1, 1) && reducedOffset > 0.75 / 70) {
            col = vec4(0x85 / 255.f, 0xDE / 255.f, 0xFF / 255.f, 1);
        }

    }
	gl_FragColor = col;
}