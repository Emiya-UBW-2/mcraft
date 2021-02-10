// 頂点シェーダーの入力
struct VS_INPUT
{
	float4 Position        : POSITION;	// ローカル
	float3 Normal     : NORMAL0;	// ローカル
	float2 TexCoords0        : TEXCOORD0;	// テクスチャ座標
	float2 TexCoords1        : TEXCOORD1;	// テクスチャ座標
	float3 Tan        : TANGENT0;	// ローカル
	float3 Bin        : BINORMAL0;	// ローカル
};
// 頂点シェーダーの出力
struct VS_OUTPUT
{
	float4 Position      : POSITION;	// 射影
	float2 TexCoords0      : TEXCOORD0;	// テクスチャ座標
	float3 VPosition     	: TEXCOORD1;	// ライト方向ベクトル（接空間）
	float3 VNormal     	: TEXCOORD2;	// 視線ベクトル（接空間）
};
// C++ 側で設定する定数の定義
float4x4 g_World : register(c94); 		// ローカル→ワールド
float4x4 g_View  : register(c6);			// ワールド→ビュー
float4x4 g_Proj  : register(c2);			// ビュー→射影
float4 LightDirection : register(c15);	//ライトの方向（ローカル）
float4 EyePos : register(c43);			//視線ベクトル（ローカル）
float4	g_fTime	 : register(c0);	//時間
// main関数
VS_OUTPUT main(VS_INPUT VSInput)
{
	VS_OUTPUT VSOutput;

	float	g_time = g_fTime.x;

	//頂点は座標変換前に移動
	VSInput.Position.y = sin(VSInput.Position.z + g_time * 0.5f);
	VSInput.Position.z += sin((g_time * 8) + VSInput.Position.y) / 16;

	//ローカル→ワールド→ビュー→射影変換
	VSOutput.Position = mul(mul(mul(VSInput.Position, g_World), g_View), g_Proj);

	// 視線ベクトルをローカル→接空間へ
	float3 VNormal = EyePos.xyz - VSInput.Position.xyz;
	VSOutput.VNormal.x = dot(VNormal, VSInput.Tan);
	VSOutput.VNormal.y = dot(VNormal, VSInput.Bin);
	VSOutput.VNormal.z = dot(VNormal, VSInput.Normal);

	// ライト方向ベクトルを接空間へ
	float3 VPosition = LightDirection.xyz;
	VSOutput.VPosition.x = dot(VPosition, VSInput.Tan);
	VSOutput.VPosition.y = dot(VPosition, VSInput.Bin);
	VSOutput.VPosition.z = dot(VPosition, VSInput.Normal);

	// テクスチャ
	VSOutput.TexCoords0 = VSInput.TexCoords0;

	return VSOutput;
}