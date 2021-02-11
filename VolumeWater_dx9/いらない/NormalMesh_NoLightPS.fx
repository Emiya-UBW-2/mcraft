// �s�N�Z���V�F�[�_�[�̓���
struct PS_INPUT
{
	float2 TexCoords0      	: TEXCOORD0;	// �e�N�X�`��
	float3 VPosition     	: TEXCOORD1;	//���C�g�����i�ڋ�ԁj
	float3 VNormal     		: TEXCOORD2;	//�����i�ڋ�ԁj
};

// �s�N�Z���V�F�[�_�[�̏o��
struct PS_OUTPUT
{
	float4 Color0          : COLOR0;
};

// �}�e���A���p�����[�^
struct MATERIAL
{
	float4 Diffuse;      // �f�B�t���[�Y�J���[
	float4 Specular;     // �X�y�L�����J���[
	float4 Power;        // �X�y�L�����̋���
};
// ���C�g�p�����[�^
struct LIGHT
{
	float4 Position;               // ���W( �r���[��� )���g�p���܂���
	float3 Direction;              // ����( �r���[��� )
	float4 Diffuse;                // �f�B�t���[�Y�J���[
	float4 Specular;               // �X�y�L�����J���[
	float4 Ambient;                // �A���r�G���g�J���[�ƃ}�e���A���̃A���r�G���g�J���[����Z��������
	float4 Range_FallOff_AT0_AT1;
	float4 AT2_SpotP0_SpotP1;
};
// C++ ���Őݒ肷��e�N�X�`����萔�̒�`
sampler  DiffuseMapTexture             : register(s0);		// �f�B�t���[�Y�}�b�v�e�N�X�`��
sampler  NormalMapTexture              : register(s1);		// �@���}�b�v�e�N�X�`��
float4   cfAmbient_Emissive            : register(c1);		// �G�~�b�V�u�J���[ + �}�e���A���A���r�G���g�J���[ * �O���[�o���A���r�G���g�J���[
MATERIAL cfMaterial : register(c2);		// �}�e���A���p�����[�^
LIGHT    cfLight : register(c32);		// ���C�g�p�����[�^
float4   cfFactorColor                 : register(c5);		// �s�����x��
float4 g_fTime                 : register(c0);		// ����

// main�֐�
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

	//�����x�N�g���𐳋K��
	V_to_Eye = normalize(PSInput.VNormal);

	// �@���̏���
	{
		// �e�N�X�`�����W
		float2 TexCoords0 = PSInput.TexCoords0;
		//�����
		float WaveM;
		float2 WaveUV, WaveMove, DirU, DirV, WaveDir = { 1.0, 0.0 };

		WaveUV.x = dot(WaveDir, TexCoords0);
		WaveUV.y = dot(-WaveDir.yx, TexCoords0);
		WaveM = fmod((g_time + WaveUV.x * 5) * 6.28, 6.28);
		WaveMove.x = sin(WaveM) * 0.01 / 2;
		WaveM = fmod((g_time + WaveUV.y * 5) * 6.28, 6.28);
		WaveMove.y = sin(WaveM) * 0.01 / 2;

		TexCoords0 += WaveMove;
		//�m�[�}���}�b�v��d���v��
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

	// �f�B�t���[�Y�J���[�ƃX�y�L�����J���[�̒~�ϒl��������
	TotalDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	TotalSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	// �f�B���N�V���i�����C�g�̏��� +++++++++++++++++++++++++++++++++++++++++++++++++++++( �J�n )

	// ���C�g�����x�N�g���̃Z�b�g
	lLightDir = PSInput.VPosition;

	// �f�B�t���[�Y�F�v�Z
	// DiffuseAngleGen = �f�B�t���[�Y�p�x�������v�Z
	DiffuseAngleGen = saturate(dot(Normal, -lLightDir));
	// �f�B�t���[�Y�J���[�~�ϒl += ���C�g�̃f�B�t���[�Y�J���[ * �}�e���A���̃f�B�t���[�Y�J���[ * �f�B�t���[�Y�J���[�p�x������ + ���C�g�̃A���r�G���g�J���[�ƃ}�e���A���̃A���r�G���g�J���[����Z�������� 
	TotalDiffuse += cfLight.Diffuse * cfMaterial.Diffuse * DiffuseAngleGen + cfLight.Ambient;

	// �X�y�L�����J���[�v�Z
	// �n�[�t�x�N�g���̌v�Z
	TempF3 = normalize(V_to_Eye - lLightDir);
	// Temp = pow( max( 0.0f, N * H ), cfMaterial.Power.x )
	Temp = pow(max(0.0f, dot(Normal, TempF3)), cfMaterial.Power.x);
	// �X�y�L�����J���[�~�ϒl += Temp * ���C�g�̃X�y�L�����J���[
	TotalSpecular += Temp * cfLight.Specular;

	// �f�B���N�V���i�����C�g�̏��� +++++++++++++++++++++++++++++++++++++++++++++++++++++( �I�� )


	// �o�̓J���[�v�Z +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( �J�n )

	// TotalDiffuse = ���C�g�f�B�t���[�Y�J���[�~�ϒl + ( �}�e���A���̃A���r�G���g�J���[�ƃO���[�o���A���r�G���g�J���[����Z�������̂ƃ}�e���A���G�~�b�V�u�J���[�����Z�������� )
	TotalDiffuse += cfAmbient_Emissive;

	// SpecularColor = ���C�g�̃X�y�L�����J���[�~�ϒl * �}�e���A���̃X�y�L�����J���[
	SpecularColor = TotalSpecular * cfMaterial.Specular;

	// �o�̓J���[ = TotalDiffuse * �e�N�X�`���J���[ + SpecularColor
	TextureDiffuseColor = tex2D(DiffuseMapTexture, PSInput.TexCoords0);
	PSOutput.Color0.rgb = TextureDiffuseColor.rgb * TotalDiffuse.rgb + SpecularColor.rgb;

	// �A���t�@�l = �e�N�X�`���A���t�@ * �}�e���A���̃f�B�t���[�Y�A���t�@ * �s�����x
	PSOutput.Color0.a = TextureDiffuseColor.a * cfMaterial.Diffuse.a * cfFactorColor.a;

	// �o�̓J���[�v�Z +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( �I�� )

	// �o�̓p�����[�^��Ԃ�
	return PSOutput;
}