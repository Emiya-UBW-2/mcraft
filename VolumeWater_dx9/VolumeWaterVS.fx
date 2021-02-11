// ���_�V�F�[�_�[�̓���
struct VS_INPUT
{
	float4 Position                 : POSITION0;		// ���W( ���[�J����� )
	float3 Normal                   : NORMAL0;			// �@��( ���[�J����� )
	float2 TexCoords0  : TEXCOORD0; // �e�N�X�`�����W
	float2 TexCoords1  : TEXCOORD1; // �e�N�X�`�����W
	float3 Tan         : TANGENT0;  // ���[�J��
	float3 Bin         : BINORMAL0; // ���[�J��
};

// ���_�V�F�[�_�[�̏o��
struct VS_OUTPUT
{
	//*
	float4 Position    : POSITION;	// ���W( �ˉe��� )
	float4 VPosition   : TEXCOORD0;	// ���W( �r���[��� )
	float4 PositionSub : TEXCOORD1;	// ���W( �ˉe��� )�s�N�Z���V�F�[�_�[�ŎQ�Ƃ���ׂ̕�
	float3 VNormal     : TEXCOORD2; // �����x�N�g���i�ڋ�ԁj
	float2 TexCoords0  : TEXCOORD3; // �e�N�X�`�����W
	//*/
};



// C++ ���Őݒ肷��萔�̒�`
float4x4 g_World : register(c94); 		// ���[�J�������[���h
float4x4 g_View  : register(c6);			// ���[���h���r���[
float4x4 g_Proj  : register(c2);			// �r���[���ˉe
float4 LightDirection : register(c15);	//���C�g�̕����i���[�J���j
//float4              cfProjectionMatrix[4] : register(c2);		// �r���[�@�@���@�ˉe�s��
//float4              cfViewMatrix[3]       : register(c6);		// ���[���h�@���@�r���[�s��
//float4              cfLocalWorldMatrix[3] : register(c94);		// ���[�J���@���@���[���h�s��
float4 EyePos : register(c43);			//�����x�N�g���i���[�J���j
float4	g_fTime	 : register(c0);	//����

// main�֐�
VS_OUTPUT main(VS_INPUT VSInput)
{
	/*
	VS_OUTPUT VSOutput;
	float4 lWorldPosition;
	float DiffuseAngleGen;

	float	g_time = g_fTime.x;


	//���_�͍��W�ϊ��O�Ɉړ�
	VSInput.Position.y = sin(VSInput.Position.z + g_time * 0.5f);
	VSInput.Position.z += sin((g_time * 8) + VSInput.Position.y) / 16;

	// ���_���W�ϊ� ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( �J�n )

	// ���[�J�����W�����[���h���W�ɕϊ�
	lWorldPosition.x = dot(VSInput.Position, cfLocalWorldMatrix[0]);
	lWorldPosition.y = dot(VSInput.Position, cfLocalWorldMatrix[1]);
	lWorldPosition.z = dot(VSInput.Position, cfLocalWorldMatrix[2]);
	lWorldPosition.w = 1.0f;

	// ���[���h���W���r���[���W�ɕϊ�
	VSOutput.VPosition.x = dot(lWorldPosition, cfViewMatrix[0]);
	VSOutput.VPosition.y = dot(lWorldPosition, cfViewMatrix[1]);
	VSOutput.VPosition.z = dot(lWorldPosition, cfViewMatrix[2]);
	VSOutput.VPosition.w = 1.0f;

	// �r���[���W���ˉe���W�ɕϊ�
	VSOutput.Position.x = dot(VSOutput.VPosition, cfProjectionMatrix[0]);
	VSOutput.Position.y = dot(VSOutput.VPosition, cfProjectionMatrix[1]);
	VSOutput.Position.z = dot(VSOutput.VPosition, cfProjectionMatrix[2]);
	VSOutput.Position.w = dot(VSOutput.VPosition, cfProjectionMatrix[3]);

	// ���_���W�ϊ� ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( �I�� )

		// �����x�N�g�������[�J�����ڋ�Ԃ�
	float3 VNormal = EyePos.xyz - VSInput.Position.xyz;
	VSOutput.VNormal.x = dot(VNormal, VSInput.Tan);
	VSOutput.VNormal.y = dot(VNormal, VSInput.Bin);
	VSOutput.VNormal.z = dot(VNormal, VSInput.Normal);

	// �s�N�Z���V�F�[�_�[�Ŏg�p���邽�߂̎ˉe���W���Z�b�g
	VSOutput.PositionSub = VSOutput.Position;

	// �e�N�X�`��
	VSOutput.TexCoords0 = VSInput.TexCoords0;


	// �o�̓p�����[�^��Ԃ�
	return VSOutput;
	//*/
//-------------------------------
	//*
	VS_OUTPUT VSOutput;

	float	g_time = g_fTime.x;

	//���_�͍��W�ϊ��O�Ɉړ�
	VSInput.Position.y = sin(VSInput.Position.z + g_time * 0.5f)*20;
	VSInput.Position.z += sin((g_time * 8) + VSInput.Position.y/20) / 16;
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
	VSOutput.VPosition.w = 1.0f;
	// �s�N�Z���V�F�[�_�[�Ŏg�p���邽�߂̎ˉe���W���Z�b�g
	VSOutput.PositionSub = VSOutput.Position;
	// �e�N�X�`��
	VSOutput.TexCoords0 = VSInput.TexCoords0;
	//
	return VSOutput;
	//*/
}
