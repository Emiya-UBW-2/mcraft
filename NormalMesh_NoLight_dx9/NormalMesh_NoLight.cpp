#include "DxLib.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ){
	int ModelHandle ;
	int PixelShaderHandle ;
	int VertexShaderHandle ;
	float RotateAngle ;

	SetOutApplicationLogValidFlag(false ? TRUE : FALSE);				/*log*/
	SetMainWindowText("");												/*�^�C�g��*/
	ChangeWindowMode(TRUE);												/*���\��*/
	SetUseDirect3DVersion(DX_DIRECT3D_9);								/*directX ver*/
	SetUseDirectInputFlag(TRUE);										/**/
	SetDirectInputMouseMode(TRUE);										/**/
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);						/*�E�C���h�E�T�C�Y���蓮�s�A�E�C���h�E�T�C�Y�ɍ��킹�Ċg������Ȃ��悤�ɂ���*/
	SetFullSceneAntiAliasingMode(4, 2);									/*�A���`�G�C���A�X*/
	SetEnableXAudioFlag(TRUE);											/**/
	Set3DSoundOneMetre(1.0f);											/**/
	DxLib_Init();														/**/
	SetSysCommandOffFlag(TRUE);											/**/
	SetAlwaysRunFlag(TRUE);												/*background*/
	SetUseZBuffer3D(TRUE);												/*zbufuse*/
	SetWriteZBuffer3D(TRUE);											/*zbufwrite*/

	// ���_�V�F�[�_�[��ǂݍ���
	VertexShaderHandle = LoadVertexShader( "NormalMesh_NoLightVS.vso" ) ;

	// �s�N�Z���V�F�[�_�[��ǂݍ���
	PixelShaderHandle = LoadPixelShader( "NormalMesh_NoLightPS.pso" ) ;

	// ���̃��b�V�����f����ǂݍ���
	ModelHandle = MV1LoadModel( "NormalBox.mqo" ) ;

	// ���f������]�����l��������
	RotateAngle = 0.0f ;

	// ���f���̕`��ɃI���W�i���V�F�[�_�[���g�p����ݒ���n�m�ɂ���
	MV1SetUseOrigShader( TRUE ) ;

	SetUseVertexShader(VertexShaderHandle);	// �g�p���钸�_�V�F�[�_�[���Z�b�g
	SetUsePixelShader(PixelShaderHandle);	// �g�p����s�N�Z���V�F�[�_�[���Z�b�g
	// ESC�L�[���������܂Ń��[�v
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
		RotateAngle += 0.02f;			// ���f������]�l�����Z
		MV1SetRotationXYZ(ModelHandle, VGet(0.0f, RotateAngle, 0.0f));			// ���f���̉�]�l�����f���ɃZ�b�g

		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		{
			MV1SetUseOrigShader(TRUE);
			MV1SetPosition(ModelHandle, VGet(320.0f, 240.0f, 0.0f));
			MV1DrawModel(ModelHandle);			// ���f����`��
			MV1SetUseOrigShader(FALSE);
			MV1SetPosition(ModelHandle, VGet(320.0f + 120.f, 240.0f, -1.0f));
			MV1DrawModel(ModelHandle);			// ���f����`��
		}
		ScreenFlip();
	}

	DeleteShader(VertexShaderHandle);	// �ǂݍ��񂾒��_�V�F�[�_�[�̍폜
	DeleteShader(PixelShaderHandle);	// �ǂݍ��񂾃s�N�Z���V�F�[�_�[�̍폜
	MV1DeleteModel(ModelHandle);	// �ǂݍ��񂾃��f���̍폜
	DxLib_End();
	return 0;
}

