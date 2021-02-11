// 頂点シェーダーの入力
struct VS_INPUT
{
	float4 Position                 : POSITION0;		// 座標( ローカル空間 )
	float3 Normal                   : NORMAL0;			// 法線( ローカル空間 )
	float2 TexCoords0  : TEXCOORD0; // テクスチャ座標
	float2 TexCoords1  : TEXCOORD1; // テクスチャ座標
	float3 Tan         : TANGENT0;  // ローカル
	float3 Bin         : BINORMAL0; // ローカル
};

// 頂点シェーダーの出力
struct VS_OUTPUT
{
	//*
	float4 Position    : POSITION;	// 座標( 射影空間 )
	float4 VPosition   : TEXCOORD0;	// 座標( ビュー空間 )
	float4 PositionSub : TEXCOORD1;	// 座標( 射影空間 )ピクセルシェーダーで参照する為の物
	float3 VNormal     : TEXCOORD2; // 視線ベクトル（接空間）
	float2 TexCoords0  : TEXCOORD3; // テクスチャ座標
	//*/
};



// C++ 側で設定する定数の定義
float4x4 g_World : register(c94); 		// ローカル→ワールド
float4x4 g_View  : register(c6);			// ワールド→ビュー
float4x4 g_Proj  : register(c2);			// ビュー→射影
float4 LightDirection : register(c15);	//ライトの方向（ローカル）
//float4              cfProjectionMatrix[4] : register(c2);		// ビュー　　→　射影行列
//float4              cfViewMatrix[3]       : register(c6);		// ワールド　→　ビュー行列
//float4              cfLocalWorldMatrix[3] : register(c94);		// ローカル　→　ワールド行列
float4 EyePos : register(c43);			//視線ベクトル（ローカル）
float4	g_fTime	 : register(c0);	//時間

// main関数
VS_OUTPUT main(VS_INPUT VSInput)
{
	/*
	VS_OUTPUT VSOutput;
	float4 lWorldPosition;
	float DiffuseAngleGen;

	float	g_time = g_fTime.x;


	//頂点は座標変換前に移動
	VSInput.Position.y = sin(VSInput.Position.z + g_time * 0.5f);
	VSInput.Position.z += sin((g_time * 8) + VSInput.Position.y) / 16;

	// 頂点座標変換 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 開始 )

	// ローカル座標をワールド座標に変換
	lWorldPosition.x = dot(VSInput.Position, cfLocalWorldMatrix[0]);
	lWorldPosition.y = dot(VSInput.Position, cfLocalWorldMatrix[1]);
	lWorldPosition.z = dot(VSInput.Position, cfLocalWorldMatrix[2]);
	lWorldPosition.w = 1.0f;

	// ワールド座標をビュー座標に変換
	VSOutput.VPosition.x = dot(lWorldPosition, cfViewMatrix[0]);
	VSOutput.VPosition.y = dot(lWorldPosition, cfViewMatrix[1]);
	VSOutput.VPosition.z = dot(lWorldPosition, cfViewMatrix[2]);
	VSOutput.VPosition.w = 1.0f;

	// ビュー座標を射影座標に変換
	VSOutput.Position.x = dot(VSOutput.VPosition, cfProjectionMatrix[0]);
	VSOutput.Position.y = dot(VSOutput.VPosition, cfProjectionMatrix[1]);
	VSOutput.Position.z = dot(VSOutput.VPosition, cfProjectionMatrix[2]);
	VSOutput.Position.w = dot(VSOutput.VPosition, cfProjectionMatrix[3]);

	// 頂点座標変換 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 終了 )

		// 視線ベクトルをローカル→接空間へ
	float3 VNormal = EyePos.xyz - VSInput.Position.xyz;
	VSOutput.VNormal.x = dot(VNormal, VSInput.Tan);
	VSOutput.VNormal.y = dot(VNormal, VSInput.Bin);
	VSOutput.VNormal.z = dot(VNormal, VSInput.Normal);

	// ピクセルシェーダーで使用するための射影座標をセット
	VSOutput.PositionSub = VSOutput.Position;

	// テクスチャ
	VSOutput.TexCoords0 = VSInput.TexCoords0;


	// 出力パラメータを返す
	return VSOutput;
	//*/
//-------------------------------
	//*
	VS_OUTPUT VSOutput;

	float	g_time = g_fTime.x;

	//頂点は座標変換前に移動
	VSInput.Position.y = sin(VSInput.Position.z + g_time * 0.5f)*20;
	VSInput.Position.z += sin((g_time * 8) + VSInput.Position.y/20) / 16;
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
	VSOutput.VPosition.w = 1.0f;
	// ピクセルシェーダーで使用するための射影座標をセット
	VSOutput.PositionSub = VSOutput.Position;
	// テクスチャ
	VSOutput.TexCoords0 = VSInput.TexCoords0;
	//
	return VSOutput;
	//*/
}
