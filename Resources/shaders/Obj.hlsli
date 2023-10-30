cbuffer WorldTransform : register(b0)
{
    matrix world; // ワールド行列
};

cbuffer ViewProjection : register(b1)
{
    matrix view;        // ビュー変換行列
    matrix projection;  // プロジェクション行列
    float3 cameraPos;   // カメラ座標 (ワールド座標)
};

cbuffer Material : register(b2)
{
    float3 m_ambient : packoffset(c0);  // アンビエント係数
    float3 m_diffuse : packoffset(c1);  // ディフューズ係数
    float3 m_specular : packoffset(c2); // スペキュラー係数
    float m_alpha : packoffset(c2.w);   // アルファ
}

// 平行光源の数
static const int DIRECTLIGHT_NUM = 3;

struct DirectLight
{
    float3 lightv;      // ライトへの方向の単位ベクトル
    float3 lightcolor;  // ライトの色(rgb)
    uint active;        // 有効かどうか
};

static const int POINTLIGHT_NUM = 3;

struct PointLight
{
    float3 lightPos;    // ライト座標
    float3 lightColor;  // ライトの色(rgb)
    float3 lightAtten;  // ライト距離減衰係数
    uint active;        // 有効かどうか
};

static const int SPOTLIGHT_NUM = 3;

//
struct SpotLight
{
    float3 lightv;              // ライトの光線方向の逆ベクトル (単位ベクトル)
    float3 lightPos;            // ライト座標
    float3 lightColor;          // ライトの色(rgb)
    float3 lightAtten;          // ライト距離減衰係数
    float2 lightFactorAngleCos; // ライト減衰角度のコサイン
    uint active;                // 有効かどうか
};

// 丸影の数
static const int CIRCLESHADOW_NUM = 3;

struct CircleShadow
{
    float3 direction;           // 投影方向の逆ベクトル (単位ベクトル)
    float3 casterPos;           // キャスター
    float distanceCasterLight;  // キャスターとライトの距離
    float3 atten;               // 距離減衰係数
    float2 factorAngleCos;      // 減衰角度のコサイン
    uint active;                // 有効かどうか
};


cbuffer LightGroup : register(b3)
{
    float3 ambientColor;
    DirectLight directLight[DIRECTLIGHT_NUM];
    PointLight pointLight[POINTLIGHT_NUM];
    SpotLight spotLight[SPOTLIGHT_NUM];
    CircleShadow circleShadow[CIRCLESHADOW_NUM];
}

struct VSOutput
{
    float4 svPos : SV_POSITION;
    float4 worldPos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};