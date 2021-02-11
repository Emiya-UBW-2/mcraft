#include"DXLib_ref/DXLib_ref.h"
#include <math.h>
// ��ʂ̉𑜓x
#define SCREEN_W			(1920)
#define SCREEN_H			(1080)
// �J�����̑��x
#define CAMERA_SPEED		(32.0f)

// �[�x�̕`��Ɏg�p����V�F�[�_�[
int Depth_NormalMeshVS;
int Depth_SkinMeshVS;
int DepthPS;
// �[�x�����Đ��ʂ�`�悷��V�F�[�_�[
int VolumeWaterVS;
int VolumeWaterPS;

// ���ʂ̃��[���h���W
VECTOR WaterWorldPos[4] = {
	{ -80000.0f, -1.0f,  80000.0f },
	{  80000.0f, -1.0f,  80000.0f },
	{ -80000.0f, -1.0f, -80000.0f },
	{  80000.0f, -1.0f, -80000.0f },
};

// ���ʂ� Diffuse Color
COLOR_U8 MirrorDiffuseColor = { 255, 255,   0, 255, };

// WinMain�֐�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	int CameraAngle;		// �J�����̐����p�x
	VECTOR CameraEyePosition;	// �J�����̍��W
	VECTOR CameraDirection;	// �J�����̌����Ă������
	VECTOR CameraTargetPosition;	// �J�����̒����_

	SetOutApplicationLogValidFlag(false ? TRUE : FALSE);				/*log*/
	SetMainWindowText("");												/*�^�C�g��*/
	ChangeWindowMode(TRUE);												/*���\��*/
	SetUseDirect3DVersion(DX_DIRECT3D_9EX);								/*directX ver*/
	SetUseDirectInputFlag(TRUE);										/**/
	SetDirectInputMouseMode(TRUE);										/**/
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);						/*�E�C���h�E�T�C�Y���蓮�s�A�E�C���h�E�T�C�Y�ɍ��킹�Ċg������Ȃ��悤�ɂ���*/
	SetFullSceneAntiAliasingMode(4, 2);									/*�A���`�G�C���A�X*/
	SetGraphMode(SCREEN_W, SCREEN_H, 32);
	SetUsePixelLighting(TRUE);
	SetEnableXAudioFlag(TRUE);											/**/
	Set3DSoundOneMetre(1.0f);											/**/
	DxLib_Init();														/**/
	SetSysCommandOffFlag(TRUE);											/**/
	SetAlwaysRunFlag(TRUE);												/*background*/
	SetUseZBuffer3D(TRUE);												/*zbufuse*/
	SetWriteZBuffer3D(TRUE);											/*zbufwrite*/

	// �[�x���擾����X�N���[��
	GraphHandle DepthScreen;
	//model
	MV1 ModelHandle;
	MV1 StageModel;
	MV1 CharaModel;

	// �[�x��`�悷��e�N�X�`���̍쐬( �P�`�����l�����������_�P�U�r�b�g�e�N�X�`�� )
	{
		SetCreateDrawValidGraphChannelNum(1);
		SetDrawValidFloatTypeGraphCreateFlag(TRUE);
		SetCreateGraphChannelBitDepth(16);
		DepthScreen = GraphHandle::Make(SCREEN_W, SCREEN_H, false);
		SetCreateDrawValidGraphChannelNum(0);
		SetDrawValidFloatTypeGraphCreateFlag(FALSE);
		SetCreateGraphChannelBitDepth(0);
	}
	// �[�x��`�悷�邽�߂̃V�F�[�_�[��ǂݍ���
	Depth_NormalMeshVS = LoadVertexShader("Depth_NormalMeshVS.vso");
	Depth_SkinMeshVS = LoadVertexShader("Depth_SkinMeshVS.vso");
	DepthPS = LoadPixelShader("DepthPS.pso");
	// �[�x�����Đ��ʂ�`�悷��V�F�[�_�[�̓ǂݍ���
	VolumeWaterVS = LoadVertexShader("VolumeWaterVS.vso");
	VolumeWaterPS = LoadPixelShader("VolumeWaterPS.pso");
	//model
	MV1::Load("NormalBox.mqo", &ModelHandle, false);
	MV1::Load("Stage.mqo", &StageModel, false);
	MV1::Load("DxChara.x", &CharaModel, false);
	//cam
	CameraAngle = 325;	// �J�����̊p�x��������
	CameraEyePosition = VGet(-2605.0f, 570.0f, -2561.0f);	// �J�����̍��W���Z�b�g
	SetLightDirection(VGet(1.0f, -1.0f, -1.0f));	// ���C�g�̌������Z�b�g
	//
	FLOAT4 Param = { 0.0f,0.0f,0.0f,0.0f };
	int OldTime = GetNowCount();
	//
	while (ProcessMessage() == 0) {
		//
		{
			auto Time = GetNowCount();	// ���݂̎��Ԃ𓾂�
			Param.x += float(Time - OldTime) / 1000.0f;
			OldTime = Time;				// ���݂̎��Ԃ�ۑ�
		}

		//cam
		{
			// ���E�L�[�������ꂽ��J��������]����
			if (CheckHitKey(KEY_INPUT_LEFT)) {
				CameraAngle += 3;
				if (CameraAngle > 360) {
					CameraAngle -= 360;
				}
			}
			if (CheckHitKey(KEY_INPUT_RIGHT)) {
				CameraAngle -= 3;
				if (CameraAngle < 0) {
					CameraAngle += 360;
				}
			}

			// �J�����̌����Ă���������Z�o
			CameraDirection.x = cosf(CameraAngle * 3.14159f / 180.0f);
			CameraDirection.y = 0.0f;
			CameraDirection.z = sinf(CameraAngle * 3.14159f / 180.0f);

			// �㉺�L�[�������ꂽ��J������O�i�E��i������
			if (CheckHitKey(KEY_INPUT_UP)) {
				CameraEyePosition = VAdd(CameraEyePosition, VScale(CameraDirection, CAMERA_SPEED));
			}
			if (CheckHitKey(KEY_INPUT_DOWN)) {
				CameraEyePosition = VSub(CameraEyePosition, VScale(CameraDirection, CAMERA_SPEED));
			}
			if (CheckHitKey(KEY_INPUT_R)) {
				CameraEyePosition = VAdd(CameraEyePosition, VScale(VGet(0, 1.f, 0), CAMERA_SPEED));
			}
			if (CheckHitKey(KEY_INPUT_F)) {
				CameraEyePosition = VSub(CameraEyePosition, VScale(VGet(0, 1.f, 0), CAMERA_SPEED));
			}

			// �J�����̒����_���W���Z�o
			CameraTargetPosition = VAdd(CameraEyePosition, CameraDirection);
			CameraTargetPosition.y -= 0.3f;
		}

		CharaModel.SetPosition(VGet(0, -100, -2000));
		// �[�x��`��
		DepthScreen.SetDraw_Screen(CameraEyePosition, CameraTargetPosition, VGet(0, 1.f, 0), deg2rad(45), 100.f, 50000.f);
		{
			// �[�x�`��̏ꍇ�͐[�x�`��p�̒��_�V�F�[�_�[���Z�b�g����
			SetUsePixelShader(DepthPS);
			SetUseTextureToShader(0, -1);
			// �X�e�[�W���f���̕`��
			SetUseVertexShader(Depth_NormalMeshVS);
			MV1SetUseOrigShader(TRUE);
			StageModel.DrawModel();
			MV1SetUseOrigShader(FALSE);
			// �L�����N�^�[���f���̕`��
			SetUseVertexShader(Depth_SkinMeshVS);
			MV1SetUseOrigShader(TRUE);
			CharaModel.DrawModel();
			MV1SetUseOrigShader(FALSE);
		}
		// �ʏ탂�f���̕`��
		GraphHandle::SetDraw_Screen(int(DX_SCREEN_BACK), CameraEyePosition, CameraTargetPosition, VGet(0, 1.f, 0), deg2rad(45), 100.f, 50000.f);
		{
			MV1SetUseOrigShader(FALSE);
			StageModel.DrawModel();				// �X�e�[�W���f���̕`��
			CharaModel.DrawModel();				// �L�����N�^�[���f���̕`��
			SetUseVertexShader(VolumeWaterVS);	//				SetUseVertexShader(VertexShaderHandle);	// �g�p���钸�_�V�F�[�_�[���Z�b�g
			SetUsePixelShader(VolumeWaterPS);	//				SetUsePixelShader(PixelShaderHandle);	// �g�p����s�N�Z���V�F�[�_�[���Z�b�g
			{
				SetVSConstF(0, Param);
				SetPSConstF(0, Param);
				SetUseTextureToShader(2, DepthScreen.get());
			}
			MV1SetUseOrigShader(TRUE);
			ModelHandle.DrawModel();
			MV1SetUseOrigShader(FALSE);

			DrawFormatString(0, 0, GetColor(255, 0, 0), "campos : %5.2f,%5.2f,%5.2f", CameraEyePosition.x, CameraEyePosition.y, CameraEyePosition.z);
		}
		ScreenFlip();
	}
	DxLib_End();
	return 0;
}