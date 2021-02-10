// ���_�V�F�[�_�[�̓���
struct VS_INPUT
{
	float4 Position        : POSITION;	// ���[�J��
	float3 Normal     : NORMAL0;	// ���[�J��
	float2 TexCoords0        : TEXCOORD0;	// �e�N�X�`�����W
	float2 TexCoords1        : TEXCOORD1;	// �e�N�X�`�����W
	float3 Tan        : TANGENT0;	// ���[�J��
	float3 Bin        : BINORMAL0;	// ���[�J��
};
// ���_�V�F�[�_�[�̏o��
struct VS_OUTPUT
{
	float4 Position      : POSITION;	// �ˉe
	float2 TexCoords0      : TEXCOORD0;	// �e�N�X�`�����W
	float3 VPosition     	: TEXCOORD1;	// ���C�g�����x�N�g���i�ڋ�ԁj
	float3 VNormal     	: TEXCOORD2;	// �����x�N�g���i�ڋ�ԁj
};
// C++ ���Őݒ肷��萔�̒�`
float4x4 g_World : register(c94); 		// ���[�J�������[���h
float4x4 g_View  : register(c6);			// ���[���h���r���[
float4x4 g_Proj  : register(c2);			// �r���[���ˉe
float4 LightDirection : register(c15);	//���C�g�̕����i���[�J���j
float4 EyePos : register(c43);			//�����x�N�g���i���[�J���j
float4	g_fTime	 : register(c0);	//����
// main�֐�
VS_OUTPUT main(VS_INPUT VSInput)
{
	VS_OUTPUT VSOutput;

	float	g_time = g_fTime.x;

	//���_�͍��W�ϊ��O�Ɉړ�
	VSInput.Position.y = sin(VSInput.Position.z + g_time * 0.5f);
	VSInput.Position.z += sin((g_time * 8) + VSInput.Position.y) / 16;

	//���[�J�������[���h���r���[���ˉe�ϊ�
	VSOutput.Position = mul(mul(mul(VSInput.Position, g_World), g_View), g_Proj);

	// �����x�N�g�������[�J�����ڋ�Ԃ�
	float3 VNormal = EyePos.xyz - VSInput.Position.xyz;
	VSOutput.VNormal.x = dot(VNormal, VSInput.Tan);
	VSOutput.VNormal.y = dot(VNormal, VSInput.Bin);
	VSOutput.VNormal.z = dot(VNormal, VSInput.Normal);

	// ���C�g�����x�N�g����ڋ�Ԃ�
	float3 VPosition = LightDirection.xyz;
	VSOutput.VPosition.x = dot(VPosition, VSInput.Tan);
	VSOutput.VPosition.y = dot(VPosition, VSInput.Bin);
	VSOutput.VPosition.z = dot(VPosition, VSInput.Normal);

	// �e�N�X�`��
	VSOutput.TexCoords0 = VSInput.TexCoords0;

	return VSOutput;
}