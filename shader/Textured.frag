uniform sampler2D texture0;

float4 main(
    float2 varing_frag_uv : TEXCOORD0,
    float4 varing_frag_color : TEXCOORD8
) {
    float4 gl_FragColor = varing_frag_color * tex2D(texture0, varing_frag_uv.xy);
    return gl_FragColor;
}
