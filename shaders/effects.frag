uniform float time;
uniform vec2 resolution;

void main()
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = gl_FragCoord.xy / resolution.xy;

    // Effect of a fire via noise
    float noise = sin(uv.y * 10.0 + time * 5.0) * 0.5 + 0.5;
    noise *= sin(uv.x * 5.0 + time * 10.0) * 0.5 + 0.5;

    // from black to red
    vec3 color = mix(vec3(0.0), vec3(1.0, 0.0, 0.0), noise);

    gl_FragColor = vec4(color, 1.0);
}