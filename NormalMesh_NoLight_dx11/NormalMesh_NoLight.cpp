#include "DxLib.h"
#include<cmath>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	int ModelHandle;
	int PixelShaderHandle;
	int VertexShaderHandle;
	int vscbhandle;
	int pscbhandle;


	SetOutApplicationLogValidFlag(false ? TRUE : FALSE);				/*log*/
	SetMainWindowText("");												/*タイトル*/
	ChangeWindowMode(TRUE);												/*窓表示*/
	SetUseDirect3DVersion(DX_DIRECT3D_11);								/*directX ver*/
	SetUseDirectInputFlag(TRUE);										/**/
	SetDirectInputMouseMode(TRUE);										/**/
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);						/*ウインドウサイズを手動不可、ウインドウサイズに合わせて拡大もしないようにする*/
	SetFullSceneAntiAliasingMode(4, 2);									/*アンチエイリアス*/
	SetUsePixelLighting(TRUE);
	SetEnableXAudioFlag(TRUE);											/**/
	Set3DSoundOneMetre(1.0f);											/**/
	DxLib_Init();														/**/
	SetSysCommandOffFlag(TRUE);											/**/
	SetAlwaysRunFlag(TRUE);												/*background*/
	SetUseZBuffer3D(TRUE);												/*zbufuse*/
	SetWriteZBuffer3D(TRUE);											/*zbufwrite*/

	vscbhandle = CreateShaderConstantBuffer(sizeof(float) * 4);
	VertexShaderHandle = LoadVertexShader("NormalMesh_DirLight_PhongVS.vso");	// 頂点シェーダーを読み込む
	pscbhandle = CreateShaderConstantBuffer(sizeof(float) * 4);
	PixelShaderHandle = LoadPixelShader("NormalMesh_DirLight_PhongPS.pso");	// ピクセルシェーダーを読み込む
	ModelHandle = MV1LoadModel("NormalBox.mqo");	// 剛体メッシュモデルを読み込む


	SetUseVertexShader(VertexShaderHandle);	// 使用する頂点シェーダーをセット
	SetUsePixelShader(PixelShaderHandle);	// 使用するピクセルシェーダーをセット
	FLOAT4 *f4;
	float g_fTime = 0.f;
	int GameTime, OldTime = GetNowCount();
	float tt = 0.f;
	// ESCキーが押されるまでループ
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
		SetLightDirection(VGet(0.5f*sin(tt), -0.5f, 0.5f*cos(tt)));
		if (CheckHitKey(KEY_INPUT_SPACE) != 0) {
			tt += DX_PI_F / GetFPS();
		}

		{
			auto Time = GetNowCount();	// 現在の時間を得る
			GameTime = Time - OldTime;	// 今フレームで経過した時間を得る
			OldTime = Time;				// 現在の時間を保存
			//
			g_fTime += float(GameTime) / 1000.0f;
			//
			f4 = (FLOAT4 *)GetBufferShaderConstantBuffer(vscbhandle);			// 頂点シェーダー用の定数バッファのアドレスを取得
			f4->x = g_fTime;
			UpdateShaderConstantBuffer(vscbhandle);								// 頂点シェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(vscbhandle, DX_SHADERTYPE_VERTEX, 4);		// 頂点シェーダーの定数バッファを定数バッファレジスタ４にセット
			//
			f4 = (FLOAT4 *)GetBufferShaderConstantBuffer(pscbhandle);			// ピクセルシェーダー用の定数バッファのアドレスを取得
			f4->x = g_fTime;
			UpdateShaderConstantBuffer(pscbhandle);								// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(pscbhandle, DX_SHADERTYPE_PIXEL, 3);		// ピクセルシェーダー用の定数バッファを定数バッファレジスタ３にセット
		}
		//MV1SetRotationXYZ(ModelHandle, VGet(0.0f, 0.f, 0.0f));			// モデルの回転値をモデルにセット

		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		SetCameraPositionAndTarget_UpVecY(VGet(-0.f, 150.f, 150.f), VGet(0, 0, 0));
		{
			MV1SetUseOrigShader(TRUE);
			MV1SetPosition(ModelHandle, VGet(-64.f, 0.f, -64.f));
			MV1DrawModel(ModelHandle);			// モデルを描画
			MV1SetUseOrigShader(FALSE);

			MV1SetPosition(ModelHandle, VGet(64.f, 0.0f, -64.0f));
			MV1DrawModel(ModelHandle);			// モデルを描画
		}
		ScreenFlip();
	}

	DeleteShader(VertexShaderHandle);		// 読み込んだ頂点シェーダーの削除
	DeleteShaderConstantBuffer(vscbhandle);	// 頂点シェーダー用定数バッファを削除

	DeleteShader(PixelShaderHandle);		// 読み込んだピクセルシェーダーの削除
	MV1DeleteModel(ModelHandle);			// 読み込んだモデルの削除
	DxLib_End();
	return 0;
}
