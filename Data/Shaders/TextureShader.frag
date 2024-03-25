precision mediump float;
varying vec2 v_uv;
uniform float u_alpha;
uniform sampler2D u_texture;

void main()
{
	vec4 texColor = texture2D(u_texture, v_uv);
	gl_FragColor = vec4(texColor.rgb, texColor.a * u_alpha);
}	