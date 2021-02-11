#include "DxLib.h"
#include <math.h>
// ��ʂ̉𑜓x
#define SCREEN_W			(1920)
#define SCREEN_H			(1080)
// �J�����̑��x
#define CAMERA_SPEED		(32.0f)

int ModelHandle;
//int PixelShaderHandle;
//int VertexShaderHandle;
// �L�����N�^�[���f���ƃX�e�[�W���f��
int CharaModel;
int StageModel;
// �[�x�̕`��Ɏg�p����V�F�[�_�[
int Depth_NormalMeshVS;
int Depth_SkinMeshVS;
int DepthPS;
// �[�x�����Đ��ʂ�`�悷��V�F�[�_�[
int VolumeWaterVS;
int VolumeWaterPS;
// �[�x���擾����X�N���[��
int DepthScreen;

// ���ʂ̃��[���h���W
VECTOR WaterWorldPos[4] ={
	{ -80000.0f, -1.0f,  80000.0f },
	{  80000.0f, -1.0f,  80000.0f },
	{ -80000.0f, -1.0f, -80000.0f },
	{  80000.0f, -1.0f, -80000.0f },
};

// ���ʂ� Diffuse Color
COLOR_U8 MirrorDiffuseColor ={255, 255,   0, 255,};

// WinMain�֐�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	int CameraAngle;		// �J�����̐����p�x
	VECTOR CameraEyePosition;	// �J�����̍��W
	VECTOR CameraDirection;	// �J�����̌����Ă������
	VECTOR CameraTargetPosition;	// �J�����̒����_
	int i;

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

	// �[�x��`�悷�邽�߂̃V�F�[�_�[��ǂݍ���
	Depth_NormalMeshVS = LoadVertexShader("Depth_NormalMeshVS.vso");
	Depth_SkinMeshVS = LoadVertexShader("Depth_SkinMeshVS.vso");
	DepthPS = LoadPixelShader("DepthPS.pso");

	// �[�x��`�悷��e�N�X�`���̍쐬( �P�`�����l�����������_�P�U�r�b�g�e�N�X�`�� )
	SetCreateDrawValidGraphChannelNum(1);
	SetDrawValidFloatTypeGraphCreateFlag(TRUE);
	SetCreateGraphChannelBitDepth(16);
	DepthScreen = MakeScreen(SCREEN_W, SCREEN_H, FALSE);
	SetCreateDrawValidGraphChannelNum(0);
	SetDrawValidFloatTypeGraphCreateFlag(FALSE);
	SetCreateGraphChannelBitDepth(0);

	// �[�x�����Đ��ʂ�`�悷��V�F�[�_�[�̓ǂݍ���
	VolumeWaterVS = LoadVertexShader("VolumeWaterVS.vso");
	VolumeWaterPS = LoadPixelShader("VolumeWaterPS.pso");
	//
	//VertexShaderHandle = LoadVertexShader("NormalMesh_NoLightVS.vso");	// ���_�V�F�[�_�[��ǂݍ���
	//PixelShaderHandle = LoadPixelShader("NormalMesh_NoLightPS.pso");	// �s�N�Z���V�F�[�_�[��ǂݍ���
	ModelHandle = MV1LoadModel("NormalBox.mqo");	// ���̃��b�V�����f����ǂݍ���
	//model
	CharaModel = MV1LoadModel("DxChara.x");	// �L�����N�^�[���f���̓ǂݍ���
	StageModel = MV1LoadModel("Stage.mqo");	// �X�e�[�W���f���̓ǂݍ���
	MV1SetPosition(StageModel, VGet(0.0f, 0.0f, 0.0f));	// �X�e�[�W���f���̔z�u
	MV1SetPosition(CharaModel, VGet(0.0f, 0.0f, 0.0f));	// �L�����N�^�[���f���̐ݒu
	MV1AttachAnim(CharaModel, 7);	// �L�����N�^�[���f���̐ݒu
	//cam
	CameraAngle = 325;	// �J�����̊p�x��������
	CameraEyePosition = VGet(-2605.0f, 570.0f, -2561.0f);	// �J�����̍��W���Z�b�g
	SetLightDirection(VGet(1.0f, -1.0f, -1.0f));	// ���C�g�̌������Z�b�g
	//
	VERTEX3DSHADER Vertex[6];
	FLOAT4 Param;
	// ���_�f�[�^�̏���
	for (i = 0; i < 4; i++) {
		Vertex[i].pos = WaterWorldPos[i];
		Vertex[i].dif = MirrorDiffuseColor;
	}
	Vertex[4] = Vertex[2];
	Vertex[5] = Vertex[1];
	//
	FLOAT4 g_fTime = { 0.0f,0.0f,0.0f,0.0f };
	int GameTime, OldTime = GetNowCount();
	//
	while (ProcessMessage() == 0){
		//cam
		{
			// ���E�L�[�������ꂽ��J��������]����
			if (CheckHitKey(KEY_INPUT_LEFT))
			{
				CameraAngle += 3;
				if (CameraAngle > 360)
				{
					CameraAngle -= 360;
				}
			}
			if (CheckHitKey(KEY_INPUT_RIGHT))
			{
				CameraAngle -= 3;
				if (CameraAngle < 0)
				{
					CameraAngle += 360;
				}
			}

			// �J�����̌����Ă���������Z�o
			CameraDirection.x = cosf(CameraAngle * 3.14159f / 180.0f);
			CameraDirection.y = 0.0f;
			CameraDirection.z = sinf(CameraAngle * 3.14159f / 180.0f);

			// �㉺�L�[�������ꂽ��J������O�i�E��i������
			if (CheckHitKey(KEY_INPUT_UP))			{
				CameraEyePosition = VAdd(CameraEyePosition, VScale(CameraDirection, CAMERA_SPEED));
			}
			if (CheckHitKey(KEY_INPUT_DOWN))			{
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
		// �[�x��`��
		{
			// �`����[�x�X�N���[���ɕύX
			SetDrawScreen(DepthScreen);
			ClearDrawScreen();
			SetCameraPositionAndTarget_UpVecY(CameraEyePosition, CameraTargetPosition);
			SetCameraNearFar(100.f, 50000.f);
			{
				// �[�x�`��̏ꍇ�͐[�x�`��p�̒��_�V�F�[�_�[���Z�b�g����
				MV1SetUseOrigShader(TRUE);
				SetUsePixelShader(DepthPS);
				SetUseTextureToShader(0, -1);
				// �X�e�[�W���f���̕`��
				SetUseVertexShader(Depth_NormalMeshVS);
				MV1DrawModel(StageModel);

				// �L�����N�^�[���f���̕`��
				SetUseVertexShader(Depth_SkinMeshVS);
				MV1DrawModel(CharaModel);
			}
		}

		// �ʏ탂�f���̕`��
		{
			SetDrawScreen(DX_SCREEN_BACK);
			ClearDrawScreen();
			SetCameraPositionAndTarget_UpVecY(CameraEyePosition, CameraTargetPosition);
			SetCameraNearFar(100.f, 50000.f);
			{
				MV1SetUseOrigShader(FALSE);
				// �X�e�[�W���f���̕`��
				MV1DrawModel(StageModel);
				// �L�����N�^�[���f���̕`��
				MV1DrawModel(CharaModel);

				SetUseVertexShader(VolumeWaterVS);
				SetUsePixelShader(VolumeWaterPS);

//				SetUseVertexShader(VertexShaderHandle);	// �g�p���钸�_�V�F�[�_�[���Z�b�g
//				SetUsePixelShader(PixelShaderHandle);	// �g�p����s�N�Z���V�F�[�_�[���Z�b�g
				{
					SetUseTextureToShader(2, DepthScreen);
					auto Time = GetNowCount();	// ���݂̎��Ԃ𓾂�
					GameTime = Time - OldTime;	// ���t���[���Ōo�߂������Ԃ𓾂�
					OldTime = Time;				// ���݂̎��Ԃ�ۑ�
					//
					g_fTime.x += float(GameTime) / 1000.0f;
					SetVSConstF(0, g_fTime);
					SetPSConstF(0, g_fTime);
				}
				MV1SetUseOrigShader(TRUE);
				MV1DrawModel(ModelHandle);			// ���f����`��
				MV1SetUseOrigShader(FALSE);

				DrawFormatString(0, 0, GetColor(255, 0, 0), "campos : %5.2f,%5.2f,%5.2f", CameraEyePosition.x, CameraEyePosition.y, CameraEyePosition.z);
			}
		}

		ScreenFlip();
	}
	DxLib_End();
	return 0;
}
