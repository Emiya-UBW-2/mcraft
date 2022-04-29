#pragma once
class main_c : Mainclass {
	//カメラ
	cam_info camera_main, camera_TPS;
	//描画スクリーン
	GraphHandle outScreen2,UI_Screen;			
	//操作
	switchs TPS;
	float xr_cam = 0.f,yr_cam = 0.f;
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
public:
	main_c() {
		//設定読み込み
		OPTION::Create();
		auto* OptionParts = OPTION::Instance();
		OptionParts->Load();							//設定読み込み

		DXDraw::Create("FPS_n2", FRAME_RATE);	//汎用

		DeBuG::Create(FRAME_RATE);
		auto DebugParts = DeBuG::Instance();					//デバッグ

		OptionParts->Set_useVR(DXDraw::Instance()->use_vr);
		//シェーダー
		PostPassEffect::Create();

		auto* DrawParts = DXDraw::Instance();

		auto UIparts = std::make_unique<UI>();																		//UI
		auto* PostPassParts = PostPassEffect::Instance();

		UI_Screen = GraphHandle::Make(DrawParts->disp_x, DrawParts->disp_y, true);									//VR、フルスクリーン共用UI
		//auto Hostpass2parts = std::make_unique<PostPassEffect>(dof_e, bloom_e, deskx, desky);						//ホストパスエフェクト(フルスクリーン向け)
		outScreen2 = GraphHandle::Make(deskx, desky, true);															//TPS用描画スクリーン
		auto mapparts = std::make_unique<Mapclass>();																//MAP
		//model
		//auto font = FontHandle::Create(18);
		UIparts->load_window("オブジェクト");	//ロード画面1
		do {
			//マップ読み込み
			mapparts->Ready("data/map_new2");
			UIparts->load_window("マップ");
			mapparts->Set();
			//キャラ設定
			chara.resize(1);
			auto& mine = chara[0];
			{
				//自機セット
				mine.set();
				mine.spawn(VGet(0,0,0),MGetIdent());
			}
			//ライティング
			DrawParts->Set_Light_Shadow(
				VGet(mapparts->cube_size_x*mapparts->size_x, mapparts->cube_size_y*mapparts->size_y, mapparts->cube_size_z*mapparts->size_z),
				VGet(-mapparts->cube_size_x*mapparts->size_x, -mapparts->cube_size_y*mapparts->size_y, -mapparts->cube_size_z*mapparts->size_z),
				VGet(0.5f, -0.5f, 0.5f), [&] { mapparts->map_draw_all(); });
			//描画するものを指定する(仮)
			auto draw_by_shadow = [&] {
				DrawParts->Draw_by_Shadow(
					[&] {
					mapparts->map_draw();
					for (auto& c : this->chara) {
						c.Draw_chara();
						//
						SetUseZBuffer3D(FALSE);
						SetUseLighting(FALSE);
						auto pos_ = (c.pos + c.pos_HMD) + c.mat.zvec()*(DrawParts->use_vr ? 1.f : -1.f)*3.f;
						DrawCube3D(
							VGet(
								float(int(pos_.x() / mapparts->cube_size_x)) * mapparts->cube_size_x - mapparts->cube_size_x / 2,
								float(int(pos_.y() / mapparts->cube_size_y)) * mapparts->cube_size_y - mapparts->cube_size_y / 2,
								float(int(pos_.z() / mapparts->cube_size_z)) * mapparts->cube_size_z - mapparts->cube_size_z / 2),
							VGet(
								float(int(pos_.x() / mapparts->cube_size_x)) * mapparts->cube_size_x + mapparts->cube_size_x / 2,
								float(int(pos_.y() / mapparts->cube_size_y)) * mapparts->cube_size_y + mapparts->cube_size_y / 2,
								float(int(pos_.z() / mapparts->cube_size_z)) * mapparts->cube_size_z + mapparts->cube_size_z / 2),
							GetColor(255, 255, 255), GetColor(255, 255, 255), FALSE);
						SetUseZBuffer3D(TRUE);
						SetUseLighting(TRUE);
						//
					}
				});
			};
			//開始
			{
				//環境
				mapparts->Start();
				//プレイヤー操作変数群
				this->TPS.Init(false);
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
				camera_main.set_cam_info(deg2rad(DrawParts->use_vr ? 90 : OptionParts->Get_Fov()), 0.1f, 100.f);
				//TPS
				camera_TPS.campos = VGet(0, 1.8f, -10);
				camera_TPS.set_cam_info(deg2rad(OptionParts->Get_Fov()), 0.1f, 100.f);
				//
				while (ProcessMessage() == 0) {
					const auto fps_ = GetFPS();
					const auto waits = GetNowHiPerformanceCount();
					DebugParts->put_way();
					{
						//プレイヤー操作
						{
							//chara
							if (DrawParts->use_vr) {
								mine.operation_VR(false, &oldv_1_1);
							}
							else {
								mine.operation(false);
								mine.pos_HMD.y(1.8f);//仮
							}
							//common
							mine.operation_2();
						}
						//pos
						for (auto& c : chara) {
							//判定
							{
								//壁その他の判定
								{
									VECTOR_ref pos_t3 = VGet(c.pos_HMD.x(), 0.f, c.pos_HMD.z());
									//VR用
									{
										VECTOR_ref pos_t2 = c.pos + VGet(c.pos_HMD_old.x(), 0.f, c.pos_HMD_old.z());
										VECTOR_ref pos_t = c.pos + pos_t3;
										//壁
										/*
										mapparts->map_col_wall(pos_t2, &pos_t);
										//*/
										c.pos = pos_t - pos_t3;
									}
									//共通
									{
										VECTOR_ref pos_t2 = c.pos + pos_t3;
										VECTOR_ref pos_t = pos_t2 + c.add_vec;
										//壁
										/*
										mapparts->map_col_wall(pos_t2, &pos_t);
										//*/
										//落下
										{
											auto pp = mapparts->getcol_line_floor(pos_t);
											if (c.add_ypos <= 0.f && pp.HitFlag == TRUE) {
												pos_t = pp.HitPosition;
												c.add_ypos = 0.f;
											}
											else {
												pos_t.yadd(c.add_ypos);
												c.add_ypos += M_GR / std::powf(fps_, 2.f);
												//復帰
												if (pos_t.y() <= -15.f) {
													pos_t.y(0.f);
													c.add_ypos = 0.f;
												}
											}
										}
										//反映
										c.pos = pos_t - pos_t3;
									}
								}
							}
							auto pos_ = (c.pos + c.pos_HMD) + c.mat.zvec()*(DrawParts->use_vr ? 1.f : -1.f)*3.f;
							//ブロック置く
							if (c.shot.trigger()) {
								mapparts->put_block(int(pos_.x() / mapparts->cube_size_x), int(pos_.y() / mapparts->cube_size_y), int(pos_.z() / mapparts->cube_size_z), &mapparts->mods.back());
								DrawParts->Update_far_Shadow([&] { mapparts->map_draw_all(); });
							}
							//ブロック消す
							if (c.shot_R.trigger()) {
								mapparts->pop_block(int(pos_.x() / mapparts->cube_size_x), int(pos_.y() / mapparts->cube_size_y), int(pos_.z() / mapparts->cube_size_z));
								DrawParts->Update_far_Shadow([&] { mapparts->map_draw_all(); });
							}
						}
						//campos,camvec,camupの指定
						{
							auto& ct = mine;
							camera_main.set_cam_pos(ct.pos + ct.pos_HMD, (ct.pos + ct.pos_HMD) + ct.mat.zvec()*(DrawParts->use_vr ? 1.f : -1.f), ct.mat.yvec());
						}
						Set3DSoundListenerPosAndFrontPosAndUpVec(camera_main.campos.get(), camera_main.camvec.get(), camera_main.camup.get());
						//VR空間に適用
						DrawParts->Move_Player();
						//1P描画
						{
							//影用意
							DrawParts->Ready_Shadow(
								camera_main.campos,
								[&] { for (auto& c : this->chara) { c.Draw_chara(); } },
								[&] { for (auto& c : this->chara) { c.Draw_chara(); } },
								VGet(2.f, 2.5f, 2.f), VGet(5.f, 2.5f, 5.f));
							//書き込み
							{
								this->UI_Screen.SetDraw_Screen();
								{
									UIparts->set_draw(DrawParts->use_vr);
								}
							}
							//VRに移す
							DrawParts->Draw_VR([&] {
								auto tmp = GetDrawScreen();
								auto tmp_cam = camera_main;
								tmp_cam.campos = GetCameraPosition();
								tmp_cam.camvec = GetCameraTarget();
								{
									//被写体深度描画
									PostPassParts->BUF_Draw([&]() { mapparts->sky_draw(); }, draw_by_shadow, tmp_cam);
									//最終描画
									PostPassParts->Set_MAIN_Draw();
								}
								GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_);
								{
									//main
									PostPassParts->MAIN_Draw();
									//UI
									if (DrawParts->use_vr) {
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
								}
							}, camera_main);
						}
						//ディスプレイ描画
						{
							//TPS視点
							this->TPS.GetInput(CheckHitKey(KEY_INPUT_LCONTROL) != 0);
							if (this->TPS.first) {
								{
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
								//影用意
								DrawParts->Ready_Shadow(camera_TPS.campos,
									[&] { for (auto& c : this->chara) { c.Draw_chara(); } },
									[&] { for (auto& c : this->chara) { c.Draw_chara(); } },
									VGet(2.f, 2.5f, 2.f), VGet(5.f, 2.5f, 5.f));
								//書き込み
								{
									//被写体深度描画
									//Hostpass2parts->BUF_Draw([&]() { mapparts->sky_draw(); }, draw_by_shadow, camera_TPS);
									//最終描画
									//Hostpass2parts->Set_MAIN_Draw();
								}
								GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), false);
								{
									//Hostpass2parts->MAIN_Draw();
								}
							}
							else {//FPS視点
								GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), false);
								{
									if (DrawParts->use_vr) {
										outScreen2.DrawGraph(0, 0, false);
									}
									else {
										DrawParts->outScreen[0].DrawGraph(0, 0, false);
									}
								}
							}
							//デバッグ
							DebugParts->end_way();
							DebugParts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
						}
					}

					mapparts->noise_.DrawRotaGraph(DrawParts->disp_x / 6, DrawParts->disp_y / 6, 2.f, 0.f, false);
					//画面の反映
					DrawParts->Screen_Flip();
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
				mapparts->Delete();
				DrawParts->Delete_Shadow();
			}
			//
		} while (ProcessMessage() == 0 && this->ending);
	}
};