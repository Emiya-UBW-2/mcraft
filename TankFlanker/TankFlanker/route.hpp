#pragma once
class main_c : Mainclass {
	//設定
	bool dof_e = false;
	bool bloom_e = false;
	bool shadow_e = false;
	bool useVR_e = true;
	float fov_pc = 45.f;
	//カメラ
	cam_info camera_main, camera_TPS;
	//描画スクリーン
	GraphHandle outScreen2;
	GraphHandle UI_Screen,UI_Screen2;			
	//操作
	switchs TPS;
	float xr_cam = 0.f;
	float yr_cam = 0.f;
	int sel_cam = 0;
	//データ
	//オブジェ
	std::vector<Chara> chara;		//キャラ
	//仮
	bool oldv_1_1 = false;
	bool oldv_1_2 = false;
	bool oldv_2_1 = false;
	bool oldv_2_2 = false;
	bool oldv_3_1 = false;
	bool oldv_3_2 = false;
	//
	bool ending = true;

	int light = 0;
public:
	main_c() {
		//設定読み込み
		{
			SetOutApplicationLogValidFlag(FALSE);
			int mdata = FileRead_open("data/setting.txt", FALSE);
			dof_e = getparams::_bool(mdata);
			bloom_e = getparams::_bool(mdata);
			shadow_e = getparams::_bool(mdata);
			useVR_e = getparams::_bool(mdata);
			fov_pc = getparams::_float(mdata);
			FileRead_close(mdata);
			SetOutApplicationLogValidFlag(TRUE);
		}
		auto Drawparts = std::make_unique<DXDraw>("FPS_n2", FRAME_RATE, useVR_e, shadow_e);							//汎用
		auto UIparts = std::make_unique<UI>();																		//UI
		auto Debugparts = std::make_unique<DeBuG>(FRAME_RATE);														//デバッグ
		auto Hostpassparts = std::make_unique<HostPassEffect>(dof_e, bloom_e, Drawparts->disp_x, Drawparts->disp_y);//ホストパスエフェクト(VR、フルスクリーン共用)
		UI_Screen = GraphHandle::Make(Drawparts->disp_x, Drawparts->disp_y, true);									//VR、フルスクリーン共用UI
		auto Hostpass2parts = std::make_unique<HostPassEffect>(dof_e, bloom_e, deskx, desky);						//ホストパスエフェクト(フルスクリーン向け)
		outScreen2 = GraphHandle::Make(deskx, desky, true);															//TPS用描画スクリーン
		UI_Screen2 = GraphHandle::Make(deskx, desky, true);															//フルスクリーン向けUI
		auto mapparts = std::make_unique<Mapclass>();																//MAP
		//model
		//auto font = FontHandle::Create(18);
		UIparts->load_window("アイテムオブジェクト");	//ロード画面1
		SetBackgroundColor(255, 0, 0);
		UIparts->load_window("アイテムデータ");	//ロード画面2

		//*
		light = CreateSpotLightHandle(
			VGet(3, 3, 3),
			VGet(0, 0, -1.f),
			DX_PI_F / 4.0f,
			DX_PI_F / 8.0f,
			15.0f,
			0.0f,
			0.1f,
			0.0f
		);
		//*/
		do {
			//マップ読み込み
			mapparts->Ready_map("data/map_new2");
			UIparts->load_window("マップ");
			mapparts->Set_map();
			//キャラ設定
			size_t sel_g = 0;
			chara.resize(1);
			auto& mine = chara[0];
			{
				//自機セット
				mine.set();
				mine.spawn(VGet(0,0,0),MGetIdent());
			}
			//ライティング
			Drawparts->Set_Light_Shadow(VGet(10.5f, 10.5f, 10.5f), VGet(-10.5f, -10.5f, -10.5f), VGet(0.5f, -0.5f, 0.5f), [&] {
				//mapparts->map_draw();
			});


			//描画するものを指定する(仮)
			auto draw_by_shadow = [&] {
				Drawparts->Draw_by_Shadow(
					[&] {
					mapparts->map_draw();
					for (auto& c : this->chara) {
						c.Draw_chara();
					}
				});
			};
			//開始
			{
				//環境
				mapparts->Start_map();
				//プレイヤー操作変数群
				this->TPS.ready(false);
				oldv_1_1 = true;
				oldv_1_2 = true;
				oldv_2_1 = true;
				oldv_2_2 = true;
				oldv_3_1 = true;
				oldv_3_2 = true;
				for (auto& c : chara) {
					c.start();
				}
				SetMousePoint(deskx / 2, desky / 2);
				//1P
				camera_main.set_cam_info(deg2rad(Drawparts->use_vr ? 90 : fov_pc), 0.1f, 100.f);
				//TPS
				camera_TPS.campos = VGet(0, 1.8f, -10);
				camera_TPS.set_cam_info(deg2rad(fov_pc), 0.1f, 100.f);
				//
				while (ProcessMessage() == 0) {
					const auto fps_ = GetFPS();
					const auto waits = GetNowHiPerformanceCount();
					Debugparts->put_way();
					{
						//プレイヤー操作
						{
							//chara
							if (Drawparts->use_vr) {
								mine.operation_VR(Drawparts, false, &oldv_1_1);
							}
							else {
								mine.operation(false);
								//common
								mine.operation_2();
							}
						}
						//
						for (auto& c : chara) {
							//判定
							//pos
						}
						//campos,camvec,camupの指定
						{
							auto& ct = mine;
							camera_main.set_cam_pos(ct.pos + ct.pos_HMD - ct.rec_HMD, (ct.pos + ct.pos_HMD - ct.rec_HMD) + ct.mat.zvec()*(Drawparts->use_vr ? 1.f : -1.f), ct.mat.yvec());
						}
						Set3DSoundListenerPosAndFrontPosAndUpVec(camera_main.campos.get(), camera_main.camvec.get(), camera_main.camup.get());
						UpdateEffekseer3D();
						//VR空間に適用
						Drawparts->Move_Player();
						//1P描画
						{
							//影用意
							Drawparts->Ready_Shadow(camera_main.campos,
								[&] {
								for (auto& c : this->chara) { c.Draw_chara(); }
							},

								VGet(2.f, 2.5f, 2.f),
								VGet(5.f, 2.5f, 5.f)
								);
							//書き込み
							{
								this->UI_Screen.SetDraw_Screen();
								{
									UIparts->set_draw(Drawparts->use_vr);
								}
							}
							{
								if (this->TPS.first) {
									GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), camera_TPS.campos, camera_TPS.camvec, camera_TPS.camup, camera_TPS.fov, camera_TPS.near_, camera_TPS.far_);
								}
								else {
									GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), camera_main.campos, camera_main.camvec, camera_main.camup, camera_main.fov, camera_main.near_, camera_main.far_);
								}
							}
							//VRに移す
							Drawparts->draw_VR([&] {
								auto tmp = GetDrawScreen();
								auto tmp_cam = camera_main;
								tmp_cam.campos = GetCameraPosition();
								tmp_cam.camvec = GetCameraTarget();
								{
									//被写体深度描画
									Hostpassparts->BUF_draw([&]() { mapparts->sky_draw(); }, draw_by_shadow, tmp_cam);
									//最終描画
									Hostpassparts->MAIN_draw();
								}
								GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_);
								{
									//main
									Hostpassparts->get_main().DrawGraph(0, 0, true);
									//UI
									if (Drawparts->use_vr) {
										SetCameraNearFar(0.01f, 2.f);
										SetUseZBuffer3D(FALSE);												//zbufuse
										SetWriteZBuffer3D(FALSE);											//zbufwrite
										{
											DrawBillboard3D((camera_main.campos + (camera_main.camvec - camera_main.campos).Norm()*1.0f).get(), 0.5f, 0.5f, 1.8f, 0.f, this->UI_Screen.get(), TRUE);
										}
										SetUseZBuffer3D(TRUE);												//zbufuse
										SetWriteZBuffer3D(TRUE);											//zbufwrite
									}
									else {
										this->UI_Screen.DrawGraph(0, 0, TRUE);
									}
									//UI2
									UIparts->item_draw(camera_main.campos, Drawparts->use_vr);
								}
							}, camera_main);
						}
						//ディスプレイ描画
						{
							//TPS視点
							this->TPS.get_in(CheckHitKey(KEY_INPUT_LCONTROL) != 0);
							if (this->TPS.first) {
								{
									//cam
									for (int i = 0; i < std::min<size_t>(chara.size(), 10); i++) {
										if (CheckHitKey(KEY_INPUT_1 + i) != 0) {
											sel_cam = i;
										}
									}
									//pos
									{
										if (CheckHitKey(KEY_INPUT_LEFT) != 0) {
											camera_TPS.campos.x(camera_TPS.campos.x() - 10.f / fps_ * cos(yr_cam));
											camera_TPS.campos.z(camera_TPS.campos.z() + 10.f / fps_ * sin(yr_cam));
										}
										if (CheckHitKey(KEY_INPUT_RIGHT) != 0) {
											camera_TPS.campos.x(camera_TPS.campos.x() + 10.f / fps_ * cos(yr_cam));
											camera_TPS.campos.z(camera_TPS.campos.z() - 10.f / fps_ * sin(yr_cam));
										}
										if (CheckHitKey(KEY_INPUT_UP) != 0) {
											camera_TPS.campos.z(camera_TPS.campos.z() + 10.f / fps_ * cos(yr_cam));
											camera_TPS.campos.x(camera_TPS.campos.x() + 10.f / fps_ * sin(yr_cam));
										}
										if (CheckHitKey(KEY_INPUT_DOWN) != 0) {
											camera_TPS.campos.z(camera_TPS.campos.z() - 10.f / fps_ * cos(yr_cam));
											camera_TPS.campos.x(camera_TPS.campos.x() - 10.f / fps_ * sin(yr_cam));
										}
									}
									//rad
									{
										VECTOR_ref vec_2 = (camera_TPS.camvec - camera_TPS.campos).Norm();
										VECTOR_ref vec_z = (camera_TPS.camvec - camera_TPS.campos).Norm();
										VECTOR_ref vec_x = vec_z.cross(camera_TPS.camup);

										xr_cam -= deg2rad(int(vec_z.cross(vec_2).dot(vec_x) * 50))*0.1f;
										yr_cam -= deg2rad(int(vec_x.dot(vec_2) * 50))*0.1f;
										xr_cam = std::clamp(xr_cam, deg2rad(-89), deg2rad(89));
									}
									//
									camera_TPS.camvec = camera_TPS.campos + MATRIX_ref::Vtrans(VGet(0, 0, 1), MATRIX_ref::RotX(xr_cam)*MATRIX_ref::RotY(yr_cam));
									camera_TPS.camup = VGet(0, 1.f, 0);
								}

								SetLightPositionHandle(light, camera_TPS.campos.get());
								SetLightDirectionHandle(light, (camera_TPS.camvec - camera_TPS.campos).get());

								//影用意
								Drawparts->Ready_Shadow(camera_TPS.campos,
									[&] {
									for (auto& c : this->chara) { c.Draw_chara(); }
								},
									VGet(2.f, 2.5f, 2.f),
									VGet(5.f, 2.5f, 5.f)
									);
								//書き込み
								{
									//被写体深度描画
									Hostpass2parts->BUF_draw([&]() { mapparts->sky_draw(); }, draw_by_shadow, camera_TPS);
									//最終描画
									Hostpass2parts->MAIN_draw();
								}
								GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), false);
								{
									Hostpass2parts->get_main().DrawGraph(0, 0, true);
								}
							}
							else {//FPS視点
								GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), false);
								{
									if (Drawparts->use_vr) {
										outScreen2.DrawGraph(0, 0, false);
									}
									else {
										Drawparts->outScreen[0].DrawGraph(0, 0, false);
									}
								}
							}
							//デバッグ
							Debugparts->end_way();
							Debugparts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
						}
					}
					//画面の反映
					Drawparts->Screen_Flip(waits);
					//終了判定
					if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
						this->ending = false;
						break;
					}
					if (CheckHitKey(KEY_INPUT_O) != 0) {
						break;
					}
					//
				}
			}
			//解放
			{
				for (auto& c : chara) {
					c.Delete_chara();
				}
				chara.clear();
				mapparts->Delete_map();
				Drawparts->Delete_Shadow();
			}
			//
		} while (ProcessMessage() == 0 && this->ending);
	}
};