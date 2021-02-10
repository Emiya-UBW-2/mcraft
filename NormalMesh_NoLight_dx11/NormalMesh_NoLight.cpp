#include "DxLib.h"
#include<cmath>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	int ModelHandle;
	int PixelShaderHandle;
	int VertexShaderHandle;
	int vscbhandle;
	int pscbhandle;


	SetOutApplicationLogValidFlag(false ? TRUE : FALSE);				/*log*/
	SetMainWindowText("");												/*�^�C�g��*/
	ChangeWindowMode(TRUE);												/*���\��*/
	SetUseDirect3DVersion(DX_DIRECT3D_11);								/*directX ver*/
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

	vscbhandle = CreateShaderConstantBuffer(sizeof(float) * 4);
	VertexShaderHandle = LoadVertexShader("NormalMesh_DirLight_PhongVS.vso");	// ���_�V�F�[�_�[��ǂݍ���
	pscbhandle = CreateShaderConstantBuffer(sizeof(float) * 4);
	PixelShaderHandle = LoadPixelShader("NormalMesh_DirLight_PhongPS.pso");	// �s�N�Z���V�F�[�_�[��ǂݍ���
	ModelHandle = MV1LoadModel("NormalBox.mqo");	// ���̃��b�V�����f����ǂݍ���


	SetUseVertexShader(VertexShaderHandle);	// �g�p���钸�_�V�F�[�_�[���Z�b�g
	SetUsePixelShader(PixelShaderHandle);	// �g�p����s�N�Z���V�F�[�_�[���Z�b�g
	FLOAT4 *f4;
	float g_fTime = 0.f;
	int GameTime, OldTime = GetNowCount();
	float tt = 0.f;
	// ESC�L�[���������܂Ń��[�v
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
		SetLightDirection(VGet(0.5f*sin(tt), -0.5f, 0.5f*cos(tt)));
		if (CheckHitKey(KEY_INPUT_SPACE) != 0) {
			tt += DX_PI_F / GetFPS();
		}

		{
			auto Time = GetNowCount();	// ���݂̎��Ԃ𓾂�
			GameTime = Time - OldTime;	// ���t���[���Ōo�߂������Ԃ𓾂�
			OldTime = Time;				// ���݂̎��Ԃ�ۑ�
			//
			g_fTime += float(GameTime) / 1000.0f;
			//
			f4 = (FLOAT4 *)GetBufferShaderConstantBuffer(vscbhandle);			// ���_�V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
			f4->x = g_fTime;
			UpdateShaderConstantBuffer(vscbhandle);								// ���_�V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
			SetShaderConstantBuffer(vscbhandle, DX_SHADERTYPE_VERTEX, 4);		// ���_�V�F�[�_�[�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^�S�ɃZ�b�g
			//
			f4 = (FLOAT4 *)GetBufferShaderConstantBuffer(pscbhandle);			// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�̃A�h���X���擾
			f4->x = g_fTime;
			UpdateShaderConstantBuffer(pscbhandle);								// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���X�V���ď������񂾓��e�𔽉f����
			SetShaderConstantBuffer(pscbhandle, DX_SHADERTYPE_PIXEL, 3);		// �s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@��萔�o�b�t�@���W�X�^�R�ɃZ�b�g
		}
		//MV1SetRotationXYZ(ModelHandle, VGet(0.0f, 0.f, 0.0f));			// ���f���̉�]�l�����f���ɃZ�b�g

		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		SetCameraPositionAndTarget_UpVecY(VGet(-0.f, 150.f, 150.f), VGet(0, 0, 0));
		{
			MV1SetUseOrigShader(TRUE);
			MV1SetPosition(ModelHandle, VGet(-64.f, 0.f, -64.f));
			MV1DrawModel(ModelHandle);			// ���f����`��
			MV1SetUseOrigShader(FALSE);

			MV1SetPosition(ModelHandle, VGet(64.f, 0.0f, -64.0f));
			MV1DrawModel(ModelHandle);			// ���f����`��
		}
		ScreenFlip();
	}

	DeleteShader(VertexShaderHandle);		// �ǂݍ��񂾒��_�V�F�[�_�[�̍폜
	DeleteShaderConstantBuffer(vscbhandle);	// ���_�V�F�[�_�[�p�萔�o�b�t�@���폜

	DeleteShader(PixelShaderHandle);		// �ǂݍ��񂾃s�N�Z���V�F�[�_�[�̍폜
	MV1DeleteModel(ModelHandle);			// �ǂݍ��񂾃��f���̍폜
	DxLib_End();
	return 0;
}
