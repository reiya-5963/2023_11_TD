struct VertexShaderOutput {
	float32_t4 position : SV_POSITION;
	float32_t2 uv : TEXCOORD;
};

 struct TransformationMat {
	float32_t4 color;
	float32_t4x4 mat;
};

StructuredBuffer<TransformationMat> gTransformationMatrices : register(t0);
