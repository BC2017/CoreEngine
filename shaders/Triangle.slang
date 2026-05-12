struct VertexOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

static const float2 positions[3] =
{
    float2(0.0, 0.5),
    float2(0.5, -0.5),
    float2(-0.5, -0.5),
};

static const float4 colors[3] =
{
    float4(0.95, 0.20, 0.18, 1.0),
    float4(0.10, 0.65, 0.95, 1.0),
    float4(0.20, 0.85, 0.35, 1.0),
};

[shader("vertex")]
VertexOutput VSMain(uint vertexId : SV_VertexID)
{
    VertexOutput output;
    output.position = float4(positions[vertexId], 0.0, 1.0);
    output.color = colors[vertexId];
    return output;
}

[shader("fragment")]
float4 PSMain(VertexOutput input) : SV_TARGET
{
    return input.color;
}
