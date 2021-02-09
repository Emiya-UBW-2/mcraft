// �s�N�Z���V�F�[�_�[�̓���
struct PS_INPUT
{
	float2 TexCoords0      : TEXCOORD0 ;
} ;

// �s�N�Z���V�F�[�_�[�̏o��
struct PS_OUTPUT
{
	float4 Color0          : COLOR0 ;
} ;


// C++ ���Őݒ肷��e�N�X�`���̒�`
sampler  DiffuseMapTexture             : register( s0 ) ;		// �f�B�t���[�Y�}�b�v�e�N�X�`��
float4   cfFactorColor                 : register( c5 ) ;		// �s�����x��


// main�֐�
PS_OUTPUT main( PS_INPUT PSInput )
{
	PS_OUTPUT PSOutput ;
	float4 TextureDiffuseColor ;

	// �e�N�X�`���J���[�̓ǂݍ���
	TextureDiffuseColor = tex2D( DiffuseMapTexture, PSInput.TexCoords0 ) ;

	// �o�̓J���[ = �e�N�X�`���J���[
	PSOutput.Color0 = TextureDiffuseColor ;

	// �o�̓A���t�@ = �e�N�X�`���A���t�@ * �s�����x
	PSOutput.Color0.a = TextureDiffuseColor.a * cfFactorColor.a ;

	// �o�̓p�����[�^��Ԃ�
	return PSOutput ;
}

