// world_lock.vert — vertex shader (Phase 3 stub)
// Applies quaternion-derived counter-rotation to world-anchor the desktop.

uniform mat4 u_transform;  // built from IMU quaternion each frame

void main() {
    // TODO Phase 3: apply u_transform to vertex position
    gl_Position = cogl_position_in;
    cogl_tex_coord_out[0] = cogl_tex_coord_in_0;
}
