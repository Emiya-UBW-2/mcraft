#pragma once
class main_c : Mainclass {
	//�J����
	cam_info camera_main, camera_TPS;
	//�`��X�N���[��
	GraphHandle outScreen2,UI_Screen;			
	//����
	switchs TPS;
	float xr_cam = 0.f,yr_cam = 0.f;
	//�I�u�W�F
	std::vector<Chara> chara;		//�L����
	//��
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
		//�ݒ�ǂݍ���
		OPTION::Create();
		auto* OptionParts = OPTION::Instance();
		OptionParts->Load();							//�ݒ�ǂݍ���

		DXDraw::Create("FPS_n2", FRAME_RATE);	//�ėp

		DeBuG::Create(FRAME_RATE);
		auto DebugParts = DeBuG::Instance();					//�f�o�b�O

		OptionParts->Set_useVR(DXDraw::Instance()->use_vr);
		//�V�F�[�_�[
		PostPassEffect::Create();

		auto* DrawParts = DXDraw::Instance();

		auto UIparts = std::make_unique<UI>();																		//UI
		auto* PostPassParts = PostPassEffect::Instance();

		UI_Screen = GraphHandle::Make(DrawParts->disp_x, DrawParts->disp_y, true);									//VR�A�t���X�N���[�����pUI
		//auto Hostpass2parts = std::make_unique<PostPassEffect>(dof_e, bloom_e, deskx, desky);						//�z�X�g�p�X�G�t�F�N�g(�t���X�N���[������)
		outScreen2 = GraphHandle::Make(deskx, desky, true);															//TPS�p�`��X�N���[��
		auto mapparts = std::make_unique<Mapclass>();																//MAP
		//model
		//auto font = FontHandle::Create(18);
		UIparts->load_window("�I�u�W�F�N�g");	//���[�h���1
		do {
			//�}�b�v�ǂݍ���
			mapparts->Ready("data/map_new2");
			UIparts->load_window("�}�b�v");
			mapparts->Set();
			//�L�����ݒ�
			chara.resize(1);
			auto& mine = chara[0];
			{
				//���@�Z�b�g
				mine.set();
				mine.spawn(VGet(0,0,0),MGetIdent());
			}
			//���C�e�B���O
			DrawParts->Set_Light_Shadow(
				VGet(mapparts->cube_size_x*mapparts->size_x, mapparts->cube_size_y*mapparts->size_y, mapparts->cube_size_z*mapparts->size_z),
				VGet(-mapparts->cube_size_x*mapparts->size_x, -mapparts->cube_size_y*mapparts->size_y, -mapparts->cube_size_z*mapparts->size_z),
				VGet(0.5f, -0.5f, 0.5f), [&] { mapparts->map_draw_all(); });
			//�`�悷����̂��w�肷��(��)
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
			//�J�n
			{
				//��
				mapparts->Start();
				//�v���C���[����ϐ��Q
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
						//�v���C���[����
						{
							//chara
							if (DrawParts->use_vr) {
								mine.operation_VR(false, &oldv_1_1);
							}
							else {
								mine.operation(false);
								mine.pos_HMD.y(1.8f);//��
							}
							//common
							mine.operation_2();
						}
						//pos
						for (auto& c : chara) {
							//����
							{
								//�ǂ��̑��̔���
								{
									VECTOR_ref pos_t3 = VGet(c.pos_HMD.x(), 0.f, c.pos_HMD.z());
									//VR�p
									{
										VECTOR_ref pos_t2 = c.pos + VGet(c.pos_HMD_old.x(), 0.f, c.pos_HMD_old.z());
										VECTOR_ref pos_t = c.pos + pos_t3;
										//��
										/*
										mapparts->map_col_wall(pos_t2, &pos_t);
										//*/
										c.pos = pos_t - pos_t3;
									}
									//����
									{
										VECTOR_ref pos_t2 = c.pos + pos_t3;
										VECTOR_ref pos_t = pos_t2 + c.add_vec;
										//��
										/*
										mapparts->map_col_wall(pos_t2, &pos_t);
										//*/
										//����
										{
											auto pp = mapparts->getcol_line_floor(pos_t);
											if (c.add_ypos <= 0.f && pp.HitFlag == TRUE) {
												pos_t = pp.HitPosition;
												c.add_ypos = 0.f;
											}
											else {
												pos_t.yadd(c.add_ypos);
												c.add_ypos += M_GR / std::powf(fps_, 2.f);
												//���A
												if (pos_t.y() <= -15.f) {
													pos_t.y(0.f);
													c.add_ypos = 0.f;
												}
											}
										}
										//���f
										c.pos = pos_t - pos_t3;
									}
								}
							}
							auto pos_ = (c.pos + c.pos_HMD) + c.mat.zvec()*(DrawParts->use_vr ? 1.f : -1.f)*3.f;
							//�u���b�N�u��
							if (c.shot.trigger()) {
								mapparts->put_block(int(pos_.x() / mapparts->cube_size_x), int(pos_.y() / mapparts->cube_size_y), int(pos_.z() / mapparts->cube_size_z), &mapparts->mods.back());
								DrawParts->Update_far_Shadow([&] { mapparts->map_draw_all(); });
							}
							//�u���b�N����
							if (c.shot_R.trigger()) {
								mapparts->pop_block(int(pos_.x() / mapparts->cube_size_x), int(pos_.y() / mapparts->cube_size_y), int(pos_.z() / mapparts->cube_size_z));
								DrawParts->Update_far_Shadow([&] { mapparts->map_draw_all(); });
							}
						}
						//campos,camvec,camup�̎w��
						{
							auto& ct = mine;
							camera_main.set_cam_pos(ct.pos + ct.pos_HMD, (ct.pos + ct.pos_HMD) + ct.mat.zvec()*(DrawParts->use_vr ? 1.f : -1.f), ct.mat.yvec());
						}
						Set3DSoundListenerPosAndFrontPosAndUpVec(camera_main.campos.get(), camera_main.camvec.get(), camera_main.camup.get());
						//VR��ԂɓK�p
						DrawParts->Move_Player();
						//1P�`��
						{
							//�e�p��
							DrawParts->Ready_Shadow(
								camera_main.campos,
								[&] { for (auto& c : this->chara) { c.Draw_chara(); } },
								[&] { for (auto& c : this->chara) { c.Draw_chara(); } },
								VGet(2.f, 2.5f, 2.f), VGet(5.f, 2.5f, 5.f));
							//��������
							{
								this->UI_Screen.SetDraw_Screen();
								{
									UIparts->set_draw(DrawParts->use_vr);
								}
							}
							//VR�Ɉڂ�
							DrawParts->Draw_VR([&] {
								auto tmp = GetDrawScreen();
								auto tmp_cam = camera_main;
								tmp_cam.campos = GetCameraPosition();
								tmp_cam.camvec = GetCameraTarget();
								{
									//��ʑ̐[�x�`��
									PostPassParts->BUF_Draw([&]() { mapparts->sky_draw(); }, draw_by_shadow, tmp_cam);
									//�ŏI�`��
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
						//�f�B�X�v���C�`��
						{
							//TPS���_
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
								//�e�p��
								DrawParts->Ready_Shadow(camera_TPS.campos,
									[&] { for (auto& c : this->chara) { c.Draw_chara(); } },
									[&] { for (auto& c : this->chara) { c.Draw_chara(); } },
									VGet(2.f, 2.5f, 2.f), VGet(5.f, 2.5f, 5.f));
								//��������
								{
									//��ʑ̐[�x�`��
									//Hostpass2parts->BUF_Draw([&]() { mapparts->sky_draw(); }, draw_by_shadow, camera_TPS);
									//�ŏI�`��
									//Hostpass2parts->Set_MAIN_Draw();
								}
								GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), false);
								{
									//Hostpass2parts->MAIN_Draw();
								}
							}
							else {//FPS���_
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
							//�f�o�b�O
							DebugParts->end_way();
							DebugParts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
						}
					}

					mapparts->noise_.DrawRotaGraph(DrawParts->disp_x / 6, DrawParts->disp_y / 6, 2.f, 0.f, false);
					//��ʂ̔��f
					DrawParts->Screen_Flip();
					//�I������
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
			//���
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