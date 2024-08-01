#version 410 core

out vec4 outColor;

in vec3 vertBarycentric;// barycentric coordinates inside the triangle
float f_thickness = 0.0f;// thickness of the rendered lines

void main()
{
    // Find the distance to the closest edge
    float f_closest_edge = min(vertBarycentric.x, min(vertBarycentric.y, vertBarycentric.z));

    // Compute the screen-space derivative of the closest edge distance
    float f_width = fwidth(f_closest_edge);

    // Compute alpha using smoothstep for antialiasing
    float f_alpha = smoothstep(f_thickness, f_thickness + f_width, f_closest_edge);// calculate alpha

    // Output the color with computed alpha
    if (f_alpha > 0.95) {
        discard;
    }
    outColor = vec4(0.0f, 0.0f, 0.0f, 1.0f - f_alpha);
}
