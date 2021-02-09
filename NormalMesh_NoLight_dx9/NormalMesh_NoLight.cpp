#include "DxLib.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ){
	int ModelHandle ;
	int PixelShaderHandle ;
	int VertexShaderHandle ;
	float RotateAngle ;

	SetOutApplicationLogValidFlag(false ? TRUE : FALSE);				/*log*/
	SetMainWindowText("");												/*タイトル*/
	ChangeWindowMode(TRUE);												/*窓表示*/
	SetUseDirect3DVersion(DX_DIRECT3D_9);								/*directX ver*/
	SetUseDirectInputFlag(TRUE);										/**/
	SetDirectInputMouseMode(TRUE);										/**/
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);						/*ウインドウサイズを手動不可、ウインドウサイズに合わせて拡大もしないようにする*/
	SetFullSceneAntiAliasingMode(4, 2);									/*アンチエイリアス*/
	SetEnableXAudioFlag(TRUE);											/**/
	Set3DSoundOneMetre(1.0f);											/**/
	DxLib_Init();														/**/
	SetSysCommandOffFlag(TRUE);											/**/
	SetAlwaysRunFlag(TRUE);												/*background*/
	SetUseZBuffer3D(TRUE);												/*zbufuse*/
	SetWriteZBuffer3D(TRUE);											/*zbufwrite*/

	// 頂点シェーダーを読み込む
	VertexShaderHandle = LoadVertexShader( "NormalMesh_NoLightVS.vso" ) ;

	// ピクセルシェーダーを読み込む
	PixelShaderHandle = LoadPixelShader( "NormalMesh_NoLightPS.pso" ) ;

	// 剛体メッシュモデルを読み込む
	ModelHandle = MV1LoadModel( "NormalBox.mqo" ) ;

	// モデルを回転される値を初期化
	RotateAngle = 0.0f ;

	// モデルの描画にオリジナルシェーダーを使用する設定をＯＮにする
	MV1SetUseOrigShader( TRUE ) ;

	SetUseVertexShader(VertexShaderHandle);	// 使用する頂点シェーダーをセット
	SetUsePixelShader(PixelShaderHandle);	// 使用するピクセルシェーダーをセット
	// ESCキーが押されるまでループ
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
		RotateAngle += 0.02f;			// モデルを回転値を加算
		MV1SetRotationXYZ(ModelHandle, VGet(0.0f, RotateAngle, 0.0f));			// モデルの回転値をモデルにセット

		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		{
			MV1SetUseOrigShader(TRUE);
			MV1SetPosition(ModelHandle, VGet(320.0f, 240.0f, 0.0f));
			MV1DrawModel(ModelHandle);			// モデルを描画
			MV1SetUseOrigShader(FALSE);
			MV1SetPosition(ModelHandle, VGet(320.0f + 120.f, 240.0f, -1.0f));
			MV1DrawModel(ModelHandle);			// モデルを描画
		}
		ScreenFlip();
	}

	DeleteShader(VertexShaderHandle);	// 読み込んだ頂点シェーダーの削除
	DeleteShader(PixelShaderHandle);	// 読み込んだピクセルシェーダーの削除
	MV1DeleteModel(ModelHandle);	// 読み込んだモデルの削除
	DxLib_End();
	return 0;
}

