#include"DXLib_ref/DXLib_ref.h"
#include <math.h>
// 画面の解像度
#define SCREEN_W			(1920)
#define SCREEN_H			(1080)
// カメラの速度
#define CAMERA_SPEED		(32.0f)

// 深度の描画に使用するシェーダー
int Depth_NormalMeshVS;
int Depth_SkinMeshVS;
int DepthPS;
// 深度を見て水面を描画するシェーダー
int VolumeWaterVS;
int VolumeWaterPS;

// 水面のワールド座標
VECTOR WaterWorldPos[4] = {
	{ -80000.0f, -1.0f,  80000.0f },
	{  80000.0f, -1.0f,  80000.0f },
	{ -80000.0f, -1.0f, -80000.0f },
	{  80000.0f, -1.0f, -80000.0f },
};

// 水面の Diffuse Color
COLOR_U8 MirrorDiffuseColor = { 255, 255,   0, 255, };

// WinMain関数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	int CameraAngle;		// カメラの水平角度
	VECTOR CameraEyePosition;	// カメラの座標
	VECTOR CameraDirection;	// カメラの向いている方向
	VECTOR CameraTargetPosition;	// カメラの注視点

	SetOutApplicationLogValidFlag(false ? TRUE : FALSE);				/*log*/
	SetMainWindowText("");												/*タイトル*/
	ChangeWindowMode(TRUE);												/*窓表示*/
	SetUseDirect3DVersion(DX_DIRECT3D_9EX);								/*directX ver*/
	SetUseDirectInputFlag(TRUE);										/**/
	SetDirectInputMouseMode(TRUE);										/**/
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);						/*ウインドウサイズを手動不可、ウインドウサイズに合わせて拡大もしないようにする*/
	SetFullSceneAntiAliasingMode(4, 2);									/*アンチエイリアス*/
	SetGraphMode(SCREEN_W, SCREEN_H, 32);
	SetUsePixelLighting(TRUE);
	SetEnableXAudioFlag(TRUE);											/**/
	Set3DSoundOneMetre(1.0f);											/**/
	DxLib_Init();														/**/
	SetSysCommandOffFlag(TRUE);											/**/
	SetAlwaysRunFlag(TRUE);												/*background*/
	SetUseZBuffer3D(TRUE);												/*zbufuse*/
	SetWriteZBuffer3D(TRUE);											/*zbufwrite*/

	// 深度を取得するスクリーン
	GraphHandle DepthScreen;
	//model
	MV1 ModelHandle;
	MV1 StageModel;
	MV1 CharaModel;

	// 深度を描画するテクスチャの作成( １チャンネル浮動小数点１６ビットテクスチャ )
	{
		SetCreateDrawValidGraphChannelNum(1);
		SetDrawValidFloatTypeGraphCreateFlag(TRUE);
		SetCreateGraphChannelBitDepth(16);
		DepthScreen = GraphHandle::Make(SCREEN_W, SCREEN_H, false);
		SetCreateDrawValidGraphChannelNum(0);
		SetDrawValidFloatTypeGraphCreateFlag(FALSE);
		SetCreateGraphChannelBitDepth(0);
	}
	// 深度を描画するためのシェーダーを読み込む
	Depth_NormalMeshVS = LoadVertexShader("Depth_NormalMeshVS.vso");
	Depth_SkinMeshVS = LoadVertexShader("Depth_SkinMeshVS.vso");
	DepthPS = LoadPixelShader("DepthPS.pso");
	// 深度を見て水面を描画するシェーダーの読み込み
	VolumeWaterVS = LoadVertexShader("VolumeWaterVS.vso");
	VolumeWaterPS = LoadPixelShader("VolumeWaterPS.pso");
	//model
	MV1::Load("NormalBox.mqo", &ModelHandle, false);
	MV1::Load("Stage.mqo", &StageModel, false);
	MV1::Load("DxChara.x", &CharaModel, false);
	//cam
	CameraAngle = 325;	// カメラの角度を初期化
	CameraEyePosition = VGet(-2605.0f, 570.0f, -2561.0f);	// カメラの座標をセット
	SetLightDirection(VGet(1.0f, -1.0f, -1.0f));	// ライトの向きをセット
	//
	FLOAT4 Param = { 0.0f,0.0f,0.0f,0.0f };
	int OldTime = GetNowCount();
	//
	while (ProcessMessage() == 0) {
		//
		{
			auto Time = GetNowCount();	// 現在の時間を得る
			Param.x += float(Time - OldTime) / 1000.0f;
			OldTime = Time;				// 現在の時間を保存
		}

		//cam
		{
			// 左右キーが押されたらカメラを回転する
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

			// カメラの向いている方向を算出
			CameraDirection.x = cosf(CameraAngle * 3.14159f / 180.0f);
			CameraDirection.y = 0.0f;
			CameraDirection.z = sinf(CameraAngle * 3.14159f / 180.0f);

			// 上下キーが押されたらカメラを前進・後進させる
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

			// カメラの注視点座標を算出
			CameraTargetPosition = VAdd(CameraEyePosition, CameraDirection);
			CameraTargetPosition.y -= 0.3f;
		}

		CharaModel.SetPosition(VGet(0, -100, -2000));
		// 深度を描画
		DepthScreen.SetDraw_Screen(CameraEyePosition, CameraTargetPosition, VGet(0, 1.f, 0), deg2rad(45), 100.f, 50000.f);
		{
			// 深度描画の場合は深度描画用の頂点シェーダーをセットする
			SetUsePixelShader(DepthPS);
			SetUseTextureToShader(0, -1);
			// ステージモデルの描画
			SetUseVertexShader(Depth_NormalMeshVS);
			MV1SetUseOrigShader(TRUE);
			StageModel.DrawModel();
			MV1SetUseOrigShader(FALSE);
			// キャラクターモデルの描画
			SetUseVertexShader(Depth_SkinMeshVS);
			MV1SetUseOrigShader(TRUE);
			CharaModel.DrawModel();
			MV1SetUseOrigShader(FALSE);
		}
		// 通常モデルの描画
		GraphHandle::SetDraw_Screen(int(DX_SCREEN_BACK), CameraEyePosition, CameraTargetPosition, VGet(0, 1.f, 0), deg2rad(45), 100.f, 50000.f);
		{
			MV1SetUseOrigShader(FALSE);
			StageModel.DrawModel();				// ステージモデルの描画
			CharaModel.DrawModel();				// キャラクターモデルの描画
			SetUseVertexShader(VolumeWaterVS);	//				SetUseVertexShader(VertexShaderHandle);	// 使用する頂点シェーダーをセット
			SetUsePixelShader(VolumeWaterPS);	//				SetUsePixelShader(PixelShaderHandle);	// 使用するピクセルシェーダーをセット
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