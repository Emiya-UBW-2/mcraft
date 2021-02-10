#include "DxLib.h"
#include<cmath>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	int ModelHandle;
	int PixelShaderHandle;
	int VertexShaderHandle;

	SetOutApplicationLogValidFlag(false ? TRUE : FALSE);				/*log*/
	SetMainWindowText("");												/*�^�C�g��*/
	ChangeWindowMode(TRUE);												/*���\��*/
	SetUseDirect3DVersion(DX_DIRECT3D_9EX);								/*directX ver*/
	SetUseDirectInputFlag(TRUE);										/**/
	SetDirectInputMouseMode(TRUE);										/**/
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);						/*�E�C���h�E�T�C�Y���蓮�s�A�E�C���h�E�T�C�Y�ɍ��킹�Ċg������Ȃ��悤�ɂ���*/
	SetFullSceneAntiAliasingMode(4, 2);									/*�A���`�G�C���A�X*/
	SetUsePixelLighting(TRUE);
	SetEnableXAudioFlag(TRUE);											/**/
	Set3DSoundOneMetre(1.0f);											/**/
	DxLib_Init();														/**/
	SetSysCommandOffFlag(TRUE);											/**/
	SetAlwaysRunFlag(TRUE);												/*background*/
	SetUseZBuffer3D(TRUE);												/*zbufuse*/
	SetWriteZBuffer3D(TRUE);											/*zbufwrite*/

	VertexShaderHandle = LoadVertexShader("NormalMesh_NoLightVS.vso");	// ���_�V�F�[�_�[��ǂݍ���
	PixelShaderHandle = LoadPixelShader("NormalMesh_NoLightPS.pso");	// �s�N�Z���V�F�[�_�[��ǂݍ���
	ModelHandle = MV1LoadModel("NormalBox.mqo");	// ���̃��b�V�����f����ǂݍ���

	FLOAT4 g_fTime = { 0.0f,0.0f,0.0f,0.0f };
	int GameTime, OldTime = GetNowCount();

	float tt = 0.f;
	// ESC�L�[���������܂Ń��[�v
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
		SetLightDirection(VGet(0.5f*sin(tt), -0.5f, -0.5f*cos(tt)));
		if (CheckHitKey(KEY_INPUT_SPACE) != 0) {
			tt += DX_PI_F / GetFPS();
		}

		{
			auto Time = GetNowCount();	// ���݂̎��Ԃ𓾂�
			GameTime = Time - OldTime;	// ���t���[���Ōo�߂������Ԃ𓾂�
			OldTime = Time;				// ���݂̎��Ԃ�ۑ�
			//
			g_fTime.x += float(GameTime) / 1000.0f;
			SetVSConstF(0, g_fTime);
			SetPSConstF(0, g_fTime);
		}
		//MV1SetRotationXYZ(ModelHandle, VGet(DX_PI_F / 2.f, 0.f, 0.f));			// ���f���̉�]�l�����f���ɃZ�b�g
		MV1SetOpacityRate(ModelHandle, 0.5f);
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		SetCameraPositionAndTarget_UpVecY(VGet(-0.f, 150.f, 150.f), VGet(0, 0, 0));
		{
			SetUseVertexShader(VertexShaderHandle);	// �g�p���钸�_�V�F�[�_�[���Z�b�g
			SetUsePixelShader(PixelShaderHandle);	// �g�p����s�N�Z���V�F�[�_�[���Z�b�g
			MV1SetUseOrigShader(TRUE);
			MV1SetPosition(ModelHandle, VGet(-64.f, 0.f, -64.f));
			MV1DrawModel(ModelHandle);			// ���f����`��
			MV1SetUseOrigShader(FALSE);

			MV1SetPosition(ModelHandle, VGet(64.f, 0.0f, -64.0f));
			MV1DrawModel(ModelHandle);			// ���f����`��
		}
		ScreenFlip();
	}

	DeleteShader(VertexShaderHandle);	// �ǂݍ��񂾒��_�V�F�[�_�[�̍폜
	DeleteShader(PixelShaderHandle);	// �ǂݍ��񂾃s�N�Z���V�F�[�_�[�̍폜
	MV1DeleteModel(ModelHandle);		// �ǂݍ��񂾃��f���̍폜
	DxLib_End();
	return 0;
}

