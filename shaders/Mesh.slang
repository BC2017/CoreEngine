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

float4 BuildSandboxClipPosition(float3 position)
{
    const float yaw = 0.62;
    const float pitch = -0.34;
    const float sinYaw = sin(yaw);
    const float cosYaw = cos(yaw);
    const float sinPitch = sin(pitch);
    const float cosPitch = cos(pitch);

    float3 yawed;
    yawed.x = (position.x * cosYaw) + (position.z * sinYaw);
    yawed.y = position.y;
    yawed.z = (-position.x * sinYaw) + (position.z * cosYaw);

    float3 view;
    view.x = yawed.x;
    view.y = (yawed.y * cosPitch) - (yawed.z * sinPitch);
    view.z = (yawed.y * sinPitch) + (yawed.z * cosPitch) + 3.0;

    const float aspect = 16.0 / 9.0;
    const float yScale = 2.41421356237;
    const float xScale = yScale / aspect;
    const float nearPlane = 0.1;
    const float farPlane = 50.0;

    float4 clip;
    clip.x = view.x * xScale;
    clip.y = view.y * yScale;
    clip.z = (view.z * farPlane / (farPlane - nearPlane)) -
        (nearPlane * farPlane / (farPlane - nearPlane));
    clip.w = view.z;
    return clip;
}

[shader("vertex")]
VertexOutput VSMain(VertexInput input)
{
    VertexOutput output;
    output.position = BuildSandboxClipPosition(input.position);
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
