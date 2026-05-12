struct VertexInput
{
    [[vk::location(0)]]
    float3 position : POSITION;

    [[vk::location(1)]]
    float4 color : COLOR0;
};

struct VertexOutput
{
    float4 position : SV_POSITION;

    [[vk::location(0)]]
    float4 color : COLOR0;
};

[shader("vertex")]
VertexOutput VSMain(VertexInput input)
{
    VertexOutput output;
    output.position = float4(input.position, 1.0);
#ifdef HFENGINE_VULKAN
    output.position.y = -output.position.y;
#endif
    output.color = input.color;
    return output;
}

[shader("fragment")]
float4 PSMain(VertexOutput input) : SV_TARGET
{
    return input.color;
}
