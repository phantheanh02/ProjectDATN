precision mediump float;

uniform 	sampler2D 	u_texture;
uniform		float		u_alpha;
varying 	vec2 		v_uv;

void main()
{
	vec4 fragColor = texture2D(u_texture,  v_uv);
	gl_FragColor = vec4(fragColor.rgb, fragColor.a * u_alpha);
}
