#include "Particle.hlsli"


Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input, uint32_t instanceId : SV_InstanceID)
{	
	PixelShaderOutput output;
	float32_t4 textureColor = gTexture.Sample(gSampler, input.uv);
    output.color = gTransformationMatrix[instanceId].color * textureColor;
    if (output.color.a == 0.0)
    {
        discard;
    }
    output.color = gTransformationMatrix[instanceId].color * textureColor;

	return output;
}