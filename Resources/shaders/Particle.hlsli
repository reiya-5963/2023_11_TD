struct VertexShaderOutput {
	float32_t4 position : SV_POSITION;
	float32_t2 uv : TEXCOORD;
    uint32_t stID :;
};

 struct TransformationMat {
	float32_t4 color;
	float32_t4x4 mat;
};

StructuredBuffer<TransformationMat> gTransformationMatrices : register(t1);
