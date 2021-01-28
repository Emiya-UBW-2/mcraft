#pragma once
class main_c : Mainclass {
	//�ݒ�
	bool dof_e = false;
	bool bloom_e = false;
	bool shadow_e = false;
	bool useVR_e = true;
	float fov_pc = 45.f;
	//�J����
	cam_info camera_main, camera_TPS;
	//�`��X�N���[��
	GraphHandle outScreen2;
	GraphHandle UI_Screen,UI_Screen2;			
	//����
	switchs TPS;
	float xr_cam = 0.f;
	float yr_cam = 0.f;
	int sel_cam = 0;
	//�f�[�^
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

	int light = 0;
public:
	main_c() {
		//�ݒ�ǂݍ���
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
		auto Drawparts = std::make_unique<DXDraw>("FPS_n2", FRAME_RATE, useVR_e, shadow_e);							//�ėp
		auto UIparts = std::make_unique<UI>();																		//UI
		auto Debugparts = std::make_unique<DeBuG>(FRAME_RATE);														//�f�o�b�O
		auto Hostpassparts = std::make_unique<HostPassEffect>(dof_e, bloom_e, Drawparts->disp_x, Drawparts->disp_y);//�z�X�g�p�X�G�t�F�N�g(VR�A�t���X�N���[�����p)
		UI_Screen = GraphHandle::Make(Drawparts->disp_x, Drawparts->disp_y, true);									//VR�A�t���X�N���[�����pUI
		auto Hostpass2parts = std::make_unique<HostPassEffect>(dof_e, bloom_e, deskx, desky);						//�z�X�g�p�X�G�t�F�N�g(�t���X�N���[������)
		outScreen2 = GraphHandle::Make(deskx, desky, true);															//TPS�p�`��X�N���[��
		UI_Screen2 = GraphHandle::Make(deskx, desky, true);															//�t���X�N���[������UI
		auto mapparts = std::make_unique<Mapclass>();																//MAP
		//model
		//auto font = FontHandle::Create(18);
		UIparts->load_window("�A�C�e���I�u�W�F�N�g");	//���[�h���1
		SetBackgroundColor(255, 0, 0);
		UIparts->load_window("�A�C�e���f�[�^");	//���[�h���2

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
			//�}�b�v�ǂݍ���
			mapparts->Ready_map("data/map_new2");
			UIparts->load_window("�}�b�v");
			mapparts->Set_map();
			//�L�����ݒ�
			size_t sel_g = 0;
			chara.resize(1);
			auto& mine = chara[0];
			{
				//���@�Z�b�g
				mine.set();
				mine.spawn(VGet(0,0,0),MGetIdent());
			}
			//���C�e�B���O
			Drawparts->Set_Light_Shadow(VGet(10.5f, 10.5f, 10.5f), VGet(-10.5f, -10.5f, -10.5f), VGet(0.5f, -0.5f, 0.5f), [&] {
				//mapparts->map_draw();
			});


			//�`�悷����̂��w�肷��(��)
			auto draw_by_shadow = [&] {
				Drawparts->Draw_by_Shadow(
					[&] {
					mapparts->map_draw();
					for (auto& c : this->chara) {
						c.Draw_chara();
					}
				});
			};
			//�J�n
			{
				//��
				mapparts->Start_map();
				//�v���C���[����ϐ��Q
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
						//�v���C���[����
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
							//����
							//pos
						}
						//campos,camvec,camup�̎w��
						{
							auto& ct = mine;
							camera_main.set_cam_pos(ct.pos + ct.pos_HMD - ct.rec_HMD, (ct.pos + ct.pos_HMD - ct.rec_HMD) + ct.mat.zvec()*(Drawparts->use_vr ? 1.f : -1.f), ct.mat.yvec());
						}
						Set3DSoundListenerPosAndFrontPosAndUpVec(camera_main.campos.get(), camera_main.camvec.get(), camera_main.camup.get());
						UpdateEffekseer3D();
						//VR��ԂɓK�p
						Drawparts->Move_Player();
						//1P�`��
						{
							//�e�p��
							Drawparts->Ready_Shadow(camera_main.campos,
								[&] {
								for (auto& c : this->chara) { c.Draw_chara(); }
							},

								VGet(2.f, 2.5f, 2.f),
								VGet(5.f, 2.5f, 5.f)
								);
							//��������
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
							//VR�Ɉڂ�
							Drawparts->draw_VR([&] {
								auto tmp = GetDrawScreen();
								auto tmp_cam = camera_main;
								tmp_cam.campos = GetCameraPosition();
								tmp_cam.camvec = GetCameraTarget();
								{
									//��ʑ̐[�x�`��
									Hostpassparts->BUF_draw([&]() { mapparts->sky_draw(); }, draw_by_shadow, tmp_cam);
									//�ŏI�`��
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
						//�f�B�X�v���C�`��
						{
							//TPS���_
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

								//�e�p��
								Drawparts->Ready_Shadow(camera_TPS.campos,
									[&] {
									for (auto& c : this->chara) { c.Draw_chara(); }
								},
									VGet(2.f, 2.5f, 2.f),
									VGet(5.f, 2.5f, 5.f)
									);
								//��������
								{
									//��ʑ̐[�x�`��
									Hostpass2parts->BUF_draw([&]() { mapparts->sky_draw(); }, draw_by_shadow, camera_TPS);
									//�ŏI�`��
									Hostpass2parts->MAIN_draw();
								}
								GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), false);
								{
									Hostpass2parts->get_main().DrawGraph(0, 0, true);
								}
							}
							else {//FPS���_
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
							//�f�o�b�O
							Debugparts->end_way();
							Debugparts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
						}
					}
					//��ʂ̔��f
					Drawparts->Screen_Flip(waits);
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
				mapparts->Delete_map();
				Drawparts->Delete_Shadow();
			}
			//
		} while (ProcessMessage() == 0 && this->ending);
	}
};