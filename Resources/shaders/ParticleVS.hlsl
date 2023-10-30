#include "Particle.hlsli"

//float4 main( float4 pos : POSITION ) : SV_POSITION
//{
//	return pos;
//}


struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 uv : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID) {
	VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrix[instanceId].mat);
	output.uv = input.uv;
	return output;
}