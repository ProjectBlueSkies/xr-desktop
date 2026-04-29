// world_lock.frag — fragment shader (Phase 3 stub)

uniform sampler2D u_texture;

void main() {
    // TODO Phase 3: sample warped texture coordinates
    cogl_color_out = texture2D(u_texture, vec2(cogl_tex_coord_in_0));
}
