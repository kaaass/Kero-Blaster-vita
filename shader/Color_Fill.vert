uniform float4x4 proj_mtx;

void main(
    float2 position,
    float2 uv,
    float4 color,

    float2 out varing_frag_uv : TEXCOORD0,
    float4 out varing_frag_color : TEXCOORD8,
    float4 out gl_Position : POSITION
) {
    varing_frag_uv = uv;
    varing_frag_color = color;
    gl_Position = mul(proj_mtx, float4(position, 0.0f, 1.0f));
}
