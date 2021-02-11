// ピクセルシェーダーの入力
struct PS_INPUT
{
	float4 VPosition   : TEXCOORD0;	// 座標( ビュー空間 )
	float4 PositionSub : TEXCOORD1;	// 座標( 射影空間 )ピクセルシェーダーで参照する為の物
	float3 VNormal     : TEXCOORD2; // 視線ベクトル（接空間）
	float2 TexCoords0  : TEXCOORD3; // テクスチャ座標

};

// ピクセルシェーダーの出力
struct PS_OUTPUT
{
	float4 Color0         : COLOR0;
};

// マテリアルパラメータ
struct MATERIAL
{
	float4 Diffuse;      // ディフューズカラー
	float4 Specular;     // スペキュラカラー
	float4 Power;        // スペキュラの強さ
};
// ライトパラメータ
struct LIGHT
{
	float4 Position;               // 座標( ビュー空間 )※使用しません
	float3 Direction;              // 方向( ビュー空間 )
	float4 Diffuse;                // ディフューズカラー
	float4 Specular;               // スペキュラカラー
	float4 Ambient;                // アンビエントカラーとマテリアルのアンビエントカラーを乗算したもの
	float4 Range_FallOff_AT0_AT1;
	float4 AT2_SpotP0_SpotP1;
};
// C++ 側で設定する定数の定義
sampler  DiffuseMapTexture  : register(s0);	// ディフューズマップテクスチャ
sampler  NormalMapTexture   : register(s1);	// 法線マップテクスチャ
sampler  DepthTexture       : register(s2); // 深度テクスチャ
float4   g_fTime		    : register(c0);	// 時間
float4   cfAmbient_Emissive : register(c1);	// エミッシブカラー + マテリアルアンビエントカラー * グローバルアンビエントカラー
MATERIAL cfMaterial         : register(c2);	// マテリアルパラメータ
float4   cfFactorColor      : register(c5);	// 不透明度等
LIGHT    cfLight            : register(c32);// ライトパラメータ

//float MaxOpacityDistance : register(c0);   // 不透明度が最大になる水中の距離
//float MinOpacity : register(c1);   // 最低不透明度


// main関数
PS_OUTPUT main(PS_INPUT PSInput)
{
//*
	PS_OUTPUT PSOutput;
	float4 TextureDiffuseColor;
	float4 SpecularColor;
	float3 Normal;
	float DiffuseAngleGen;
	float4 TotalDiffuse;
	float4 TotalSpecular;
	float3 TempF3;
	float Temp;
	float3 V_to_Eye;
	float3 lLightDir;
	float2 TexCoords;
	float4 Depth;

	float MaxOpacityDistance = 5000.f;   // 不透明度が最大になる水中の距離
	//float MinOpacity = 0.0f;   // 最低不透明度

	float	g_time = g_fTime.x;

	//視線ベクトルを正規化
	V_to_Eye = normalize(PSInput.VNormal);

	// 法線の準備
	{
		// テクスチャ座標
		float2 TexCoords0 = PSInput.TexCoords0;
		//ゆらゆら
		float WaveM;
		float2 WaveUV, WaveMove, DirU, DirV, WaveDir = { 1.0, 0.0 };

		WaveUV.x = dot(WaveDir, TexCoords0);
		WaveUV.y = dot(-WaveDir.yx, TexCoords0);
		WaveM = fmod((g_time + WaveUV.x * 5) * 6.28, 6.28);
		WaveMove.x = sin(WaveM) * 0.01 / 2;
		WaveM = fmod((g_time + WaveUV.y * 5) * 6.28, 6.28);
		WaveMove.y = sin(WaveM) * 0.01 / 2;

		TexCoords0 += WaveMove;
		//ノーマルマップ二重化計画
		TexCoords0.x += 0.5;
		TexCoords0.y += (sin(g_time * 3 + 8) / 512) + (g_time / 32);
		float h = tex2D(NormalMapTexture, TexCoords0).a;
		TexCoords0 = TexCoords0 + 0.016 * h * V_to_Eye.xy;
		Normal = 2.0f * tex2D(NormalMapTexture, TexCoords0).xyz - 1.0f;

		TexCoords0.x -= 0.5;
		TexCoords0.y -= ((sin(g_time * 3 + 8) / 512) + (g_time / 32))*2.0f;
		h = tex2D(NormalMapTexture, TexCoords0).a;
		TexCoords0 = TexCoords0 + 0.016 * h * V_to_Eye.xy;
		float3 Normal2 = 2.0f * tex2D(NormalMapTexture, TexCoords0).xyz - 1.0f;

		Normal = (Normal + Normal2) / 2;
	}

	// ディフューズカラーとスペキュラカラーの蓄積値を初期化
	TotalDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	TotalSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	// ディレクショナルライトの処理 +++++++++++++++++++++++++++++++++++++++++++++++++++++( 開始 )

	// ライト方向ベクトルのセット
	lLightDir = PSInput.VPosition.xyz;

	// ディフューズ色計算
	// DiffuseAngleGen = ディフューズ角度減衰率計算
	DiffuseAngleGen = saturate(dot(Normal, -lLightDir));
	// ディフューズカラー蓄積値 += ライトのディフューズカラー * マテリアルのディフューズカラー * ディフューズカラー角度減衰率 + ライトのアンビエントカラーとマテリアルのアンビエントカラーを乗算したもの 
	TotalDiffuse += cfLight.Diffuse * cfMaterial.Diffuse * DiffuseAngleGen + cfLight.Ambient;

	// スペキュラカラー計算
	// ハーフベクトルの計算
	TempF3 = normalize(V_to_Eye - lLightDir);
	// Temp = pow( max( 0.0f, N * H ), cfMaterial.Power.x )
	Temp = pow(max(0.0f, dot(Normal, TempF3)), cfMaterial.Power.x);
	// スペキュラカラー蓄積値 += Temp * ライトのスペキュラカラー
	TotalSpecular += Temp * cfLight.Specular;

	// ディレクショナルライトの処理 +++++++++++++++++++++++++++++++++++++++++++++++++++++( 終了 )


	// 出力カラー計算 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 開始 )

	// TotalDiffuse = ライトディフューズカラー蓄積値 + ( マテリアルのアンビエントカラーとグローバルアンビエントカラーを乗算したものとマテリアルエミッシブカラーを加算したもの )
	TotalDiffuse += cfAmbient_Emissive;

	// SpecularColor = ライトのスペキュラカラー蓄積値 * マテリアルのスペキュラカラー
	SpecularColor = TotalSpecular * cfMaterial.Specular;

	// 出力カラー = TotalDiffuse * テクスチャカラー + SpecularColor
	TextureDiffuseColor = tex2D(DiffuseMapTexture, PSInput.TexCoords0);
	PSOutput.Color0.rgb = TextureDiffuseColor.rgb * TotalDiffuse.rgb + SpecularColor.rgb;


	{
		// 深度テクスチャ座標の算出
		TexCoords.x = (PSInput.PositionSub.x / PSInput.PositionSub.w + 1.0f) / 2.0f;
		TexCoords.y = (-PSInput.PositionSub.y / PSInput.PositionSub.w + 1.0f) / 2.0f;

		// 深度テクスチャから深度を取得
		Depth = tex2D(DepthTexture, TexCoords);
	}
	//PSOutput.Color0.r = saturate((1.0f - MinOpacity) * (Depth.r - PSInput.VPosition.z) / MaxOpacityDistance + MinOpacity);
	//PSOutput.Color0.g = saturate((1.0f - MinOpacity) * (Depth.r - PSInput.VPosition.z) / MaxOpacityDistance + MinOpacity);
	//PSOutput.Color0.b = saturate((1.0f - MinOpacity) * (Depth.r - PSInput.VPosition.z) / MaxOpacityDistance + MinOpacity);
	// アルファ値 = テクスチャアルファ * マテリアルのディフューズアルファ * 不透明度
	PSOutput.Color0.a = saturate(((Depth.r - PSInput.VPosition.z)) / MaxOpacityDistance);//TextureDiffuseColor.a * cfMaterial.Diffuse.a * cfFactorColor.a;

	// 出力カラー計算 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 終了 )

	// 出力パラメータを返す
	return PSOutput;
//*/
/*
	PS_OUTPUT PSOutput;
	float2 TexCoords;
	float4 Depth;

	float MaxOpacityDistance = 6000.f;   // 不透明度が最大になる水中の距離
	float MinOpacity = 0.01f;   // 最低不透明度

	float	g_time = g_fTime.x;


	// 深度テクスチャ座標の算出
	TexCoords.x = (PSInput.PositionSub.x / PSInput.PositionSub.w + 1.0f) / 2.0f;
	TexCoords.y = (-PSInput.PositionSub.y / PSInput.PositionSub.w + 1.0f) / 2.0f;

	// 深度テクスチャから深度を取得
	Depth = tex2D(DepthTexture, TexCoords);

	// 不透明度を計算
	PSOutput.Color0.a = saturate((1.0f - MinOpacity) * (Depth.r - PSInput.VPosition.z) / MaxOpacityDistance + MinOpacity);

	// 色のセット
	PSOutput.Color0.rgb = cfMaterial.Diffuse.rgb;

	return PSOutput;
//*/
}