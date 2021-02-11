#include "DxLib.h"
#include <math.h>
// 画面の解像度
#define SCREEN_W			(1920)
#define SCREEN_H			(1080)
// カメラの速度
#define CAMERA_SPEED		(32.0f)

int ModelHandle;
//int PixelShaderHandle;
//int VertexShaderHandle;
// キャラクターモデルとステージモデル
int CharaModel;
int StageModel;
// 深度の描画に使用するシェーダー
int Depth_NormalMeshVS;
int Depth_SkinMeshVS;
int DepthPS;
// 深度を見て水面を描画するシェーダー
int VolumeWaterVS;
int VolumeWaterPS;
// 深度を取得するスクリーン
int DepthScreen;

// 水面のワールド座標
VECTOR WaterWorldPos[4] ={
	{ -80000.0f, -1.0f,  80000.0f },
	{  80000.0f, -1.0f,  80000.0f },
	{ -80000.0f, -1.0f, -80000.0f },
	{  80000.0f, -1.0f, -80000.0f },
};

// 水面の Diffuse Color
COLOR_U8 MirrorDiffuseColor ={255, 255,   0, 255,};

// WinMain関数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	int CameraAngle;		// カメラの水平角度
	VECTOR CameraEyePosition;	// カメラの座標
	VECTOR CameraDirection;	// カメラの向いている方向
	VECTOR CameraTargetPosition;	// カメラの注視点
	int i;

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

	// 深度を描画するためのシェーダーを読み込む
	Depth_NormalMeshVS = LoadVertexShader("Depth_NormalMeshVS.vso");
	Depth_SkinMeshVS = LoadVertexShader("Depth_SkinMeshVS.vso");
	DepthPS = LoadPixelShader("DepthPS.pso");

	// 深度を描画するテクスチャの作成( １チャンネル浮動小数点１６ビットテクスチャ )
	SetCreateDrawValidGraphChannelNum(1);
	SetDrawValidFloatTypeGraphCreateFlag(TRUE);
	SetCreateGraphChannelBitDepth(16);
	DepthScreen = MakeScreen(SCREEN_W, SCREEN_H, FALSE);
	SetCreateDrawValidGraphChannelNum(0);
	SetDrawValidFloatTypeGraphCreateFlag(FALSE);
	SetCreateGraphChannelBitDepth(0);

	// 深度を見て水面を描画するシェーダーの読み込み
	VolumeWaterVS = LoadVertexShader("VolumeWaterVS.vso");
	VolumeWaterPS = LoadPixelShader("VolumeWaterPS.pso");
	//
	//VertexShaderHandle = LoadVertexShader("NormalMesh_NoLightVS.vso");	// 頂点シェーダーを読み込む
	//PixelShaderHandle = LoadPixelShader("NormalMesh_NoLightPS.pso");	// ピクセルシェーダーを読み込む
	ModelHandle = MV1LoadModel("NormalBox.mqo");	// 剛体メッシュモデルを読み込む
	//model
	CharaModel = MV1LoadModel("DxChara.x");	// キャラクターモデルの読み込み
	StageModel = MV1LoadModel("Stage.mqo");	// ステージモデルの読み込み
	MV1SetPosition(StageModel, VGet(0.0f, 0.0f, 0.0f));	// ステージモデルの配置
	MV1SetPosition(CharaModel, VGet(0.0f, 0.0f, 0.0f));	// キャラクターモデルの設置
	MV1AttachAnim(CharaModel, 7);	// キャラクターモデルの設置
	//cam
	CameraAngle = 325;	// カメラの角度を初期化
	CameraEyePosition = VGet(-2605.0f, 570.0f, -2561.0f);	// カメラの座標をセット
	SetLightDirection(VGet(1.0f, -1.0f, -1.0f));	// ライトの向きをセット
	//
	VERTEX3DSHADER Vertex[6];
	FLOAT4 Param;
	// 頂点データの準備
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
			// 左右キーが押されたらカメラを回転する
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

			// カメラの向いている方向を算出
			CameraDirection.x = cosf(CameraAngle * 3.14159f / 180.0f);
			CameraDirection.y = 0.0f;
			CameraDirection.z = sinf(CameraAngle * 3.14159f / 180.0f);

			// 上下キーが押されたらカメラを前進・後進させる
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

			// カメラの注視点座標を算出
			CameraTargetPosition = VAdd(CameraEyePosition, CameraDirection);
			CameraTargetPosition.y -= 0.3f;
		}
		// 深度を描画
		{
			// 描画先を深度スクリーンに変更
			SetDrawScreen(DepthScreen);
			ClearDrawScreen();
			SetCameraPositionAndTarget_UpVecY(CameraEyePosition, CameraTargetPosition);
			SetCameraNearFar(100.f, 50000.f);
			{
				// 深度描画の場合は深度描画用の頂点シェーダーをセットする
				MV1SetUseOrigShader(TRUE);
				SetUsePixelShader(DepthPS);
				SetUseTextureToShader(0, -1);
				// ステージモデルの描画
				SetUseVertexShader(Depth_NormalMeshVS);
				MV1DrawModel(StageModel);

				// キャラクターモデルの描画
				SetUseVertexShader(Depth_SkinMeshVS);
				MV1DrawModel(CharaModel);
			}
		}

		// 通常モデルの描画
		{
			SetDrawScreen(DX_SCREEN_BACK);
			ClearDrawScreen();
			SetCameraPositionAndTarget_UpVecY(CameraEyePosition, CameraTargetPosition);
			SetCameraNearFar(100.f, 50000.f);
			{
				MV1SetUseOrigShader(FALSE);
				// ステージモデルの描画
				MV1DrawModel(StageModel);
				// キャラクターモデルの描画
				MV1DrawModel(CharaModel);

				SetUseVertexShader(VolumeWaterVS);
				SetUsePixelShader(VolumeWaterPS);

//				SetUseVertexShader(VertexShaderHandle);	// 使用する頂点シェーダーをセット
//				SetUsePixelShader(PixelShaderHandle);	// 使用するピクセルシェーダーをセット
				{
					SetUseTextureToShader(2, DepthScreen);
					auto Time = GetNowCount();	// 現在の時間を得る
					GameTime = Time - OldTime;	// 今フレームで経過した時間を得る
					OldTime = Time;				// 現在の時間を保存
					//
					g_fTime.x += float(GameTime) / 1000.0f;
					SetVSConstF(0, g_fTime);
					SetPSConstF(0, g_fTime);
				}
				MV1SetUseOrigShader(TRUE);
				MV1DrawModel(ModelHandle);			// モデルを描画
				MV1SetUseOrigShader(FALSE);

				DrawFormatString(0, 0, GetColor(255, 0, 0), "campos : %5.2f,%5.2f,%5.2f", CameraEyePosition.x, CameraEyePosition.y, CameraEyePosition.z);
			}
		}

		ScreenFlip();
	}
	DxLib_End();
	return 0;
}
