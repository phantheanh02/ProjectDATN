attribute vec3 a_posL;
attribute vec4 a_color;
varying vec4 vColor;
void main()
{
vec4 posL = vec4(a_posL, 1.0);
gl_Position = posL;
vColor = a_color;
}
   