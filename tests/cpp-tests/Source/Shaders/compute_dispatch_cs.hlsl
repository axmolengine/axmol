RWStructuredBuffer<float4> OutColors;

[numthreads(16, 1, 1)]
void main(uint3 dtid : SV_DispatchThreadID)
{
    float r = float(dtid.x & 3u) / 3.0f;
    float g = float((dtid.x >> 2) & 3u) / 3.0f;
    float b = float(dtid.x) / 15.0f;
    OutColors[dtid.x] = float4(r, g, b, 1.0f);
}
