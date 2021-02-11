// ピクセルシェーダーの入力
struct PS_INPUT
{
	float2 TexCoords0      	: TEXCOORD0;	// テクスチャ
	float3 VPosition     	: TEXCOORD1;	//ライト方向（接空間）
	float3 VNormal     		: TEXCOORD2;	//視線（接空間）
};

// ピクセルシェーダーの出力
struct PS_OUTPUT
{
	float4 Color0          : COLOR0;
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
// C++ 側で設定するテクスチャや定数の定義
sampler  DiffuseMapTexture             : register(s0);		// ディフューズマップテクスチャ
sampler  NormalMapTexture              : register(s1);		// 法線マップテクスチャ
float4   cfAmbient_Emissive            : register(c1);		// エミッシブカラー + マテリアルアンビエントカラー * グローバルアンビエントカラー
MATERIAL cfMaterial : register(c2);		// マテリアルパラメータ
LIGHT    cfLight : register(c32);		// ライトパラメータ
float4   cfFactorColor                 : register(c5);		// 不透明度等
float4 g_fTime                 : register(c0);		// 時間

// main関数
PS_OUTPUT main(PS_INPUT PSInput)
{
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
	lLightDir = PSInput.VPosition;

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

	// アルファ値 = テクスチャアルファ * マテリアルのディフューズアルファ * 不透明度
	PSOutput.Color0.a = TextureDiffuseColor.a * cfMaterial.Diffuse.a * cfFactorColor.a;

	// 出力カラー計算 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 終了 )

	// 出力パラメータを返す
	return PSOutput;
}