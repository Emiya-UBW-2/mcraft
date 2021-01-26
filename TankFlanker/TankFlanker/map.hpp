#pragma once

// �v���C���[�֌W�̒�`
#define PLAYER_ENUM_DEFAULT_SIZE	1.8f	// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
#define PLAYER_HIT_WIDTH			0.15f	// �����蔻��J�v�Z���̔��a
#define PLAYER_HIT_HEIGHT			1.8f	// �����蔻��J�v�Z���̍���
#define PLAYER_HIT_TRYNUM			16		// �ǉ����o�������̍ő厎�s��
#define PLAYER_HIT_SLIDE_LENGTH		0.015f	// ��x�̕ǉ����o�������ŃX���C�h�����鋗��

class Mapclass :Mainclass {
private:
	MV1 map, map_col;	//�n��
	MV1 sky;			//��
	SoundHandle envi;	//

	std::vector<VECTOR_ref> way_point;
	std::vector<VECTOR_ref> lean_point_q;
	std::vector<VECTOR_ref> lean_point_e;
	std::vector<VECTOR_ref> spawn_point;

	GraphHandle minmap;

	int x_size = 0;
	int y_size = 0;

	//�_
	int grasss = 600;				/*grass�̐�*/
	std::vector<VERTEX3D> grassver; /*grass*/
	std::vector<DWORD> grassind;    /*grass*/
	int VerBuf, IndexBuf;			/*grass*/
	MV1 grass;						/*grass���f��*/
	GraphHandle grass_pic;				/*�摜�n���h��*/
	int IndexNum, VerNum;			/*grass*/
	int vnum, pnum;					/*grass*/
	MV1_REF_POLYGONLIST RefMesh;	/*grass*/
public:


	std::vector<VECTOR_ref>& get_waypoint() {
		return way_point;
	}
	std::vector<VECTOR_ref>& get_leanpoint_q() {
		return lean_point_q;
	}
	std::vector<VECTOR_ref>& get_leanpoint_e() {
		return lean_point_e;
	}
	std::vector<VECTOR_ref>& get_spawn_point() {
		return spawn_point;
	}
	GraphHandle& get_minmap() {
		return minmap;
	}
	int& get_x_size() {
		return x_size;
	}
	int& get_y_size() {
		return y_size;
	}

	Mapclass() {
	}
	~Mapclass() {

	}
	void Ready_map(std::string dir) {
		MV1::Load(dir + "/model.mv1", &map, true);		   //map
		MV1::Load(dir + "/col.mv1", &map_col, true);		   //map�R���W����
		MV1::Load(dir + "/sky/model.mv1", &sky, true);	 //��
		SetUseASyncLoadFlag(TRUE);
		envi = SoundHandle::Load(dir + "/envi.wav");
		minmap = GraphHandle::Load(dir + "/minimap.png");
		SetUseASyncLoadFlag(FALSE);

		SetUseASyncLoadFlag(TRUE);
		grass_pic = GraphHandle::Load("data/model/grass/grass.png");		 /*grass*/
		SetUseASyncLoadFlag(FALSE);
		MV1::Load("data/model/grass/model.mv1", &grass, true);		/*grass*/
	}
	void Set_map(
		const char* item_txt, std::vector<Items>& item_data, 
		std::vector<Guns>& gun_data,
		std::vector<Mags>& mag_data,
		std::vector<Meds>& med_data,
		const char* buf) {
		//map.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
		VECTOR_ref size;
		{
			VECTOR_ref sizetmp = map_col.mesh_maxpos(0) - map_col.mesh_minpos(0);
			if (size.x() < sizetmp.x()) {
				size.x(sizetmp.x());
			}
			if (size.y() < sizetmp.y()) {
				size.y(sizetmp.y());
			}
			if (size.z() < sizetmp.z()) {
				size.z(sizetmp.z());
			}
		}
		map_col.SetupCollInfo(int(size.x() / 5.f), int(size.y() / 5.f), int(size.z() / 5.f), 0, 0);

		{
			MV1SetupReferenceMesh(map_col.get(), 0, FALSE);
			auto p = MV1GetReferenceMesh(map_col.get(), 0, FALSE);
			for (int i = 0; i < p.PolygonNum; i++) {
				if (p.Polygons[i].MaterialIndex == 1) {
					way_point.resize(way_point.size() + 1);
					way_point.back() = (VECTOR_ref(p.Vertexs[p.Polygons[i].VIndex[0]].Position) + p.Vertexs[p.Polygons[i].VIndex[1]].Position + p.Vertexs[p.Polygons[i].VIndex[2]].Position) * (1.f / 3.f);
					auto pt = map_col.CollCheck_Line(way_point.back() + VGet(0, 10.f, 0.f), way_point.back() + VGet(0, -10.f, 0.f), 0, 0);
					if (pt.HitFlag) { way_point.back() = pt.HitPosition; }
				}
				else if (p.Polygons[i].MaterialIndex == 2) {
					lean_point_e.resize(lean_point_e.size() + 1);
					lean_point_e.back() = (VECTOR_ref(p.Vertexs[p.Polygons[i].VIndex[0]].Position) + p.Vertexs[p.Polygons[i].VIndex[1]].Position + p.Vertexs[p.Polygons[i].VIndex[2]].Position) * (1.f / 3.f);
					auto pt = map_col.CollCheck_Line(lean_point_e.back() + VGet(0, 10.f, 0.f), lean_point_e.back() + VGet(0, -10.f, 0.f), 0, 0);
					if (pt.HitFlag) { lean_point_e.back() = pt.HitPosition; }
				}
				else if (p.Polygons[i].MaterialIndex == 3) {
					lean_point_q.resize(lean_point_q.size() + 1);
					lean_point_q.back() = (VECTOR_ref(p.Vertexs[p.Polygons[i].VIndex[0]].Position) + p.Vertexs[p.Polygons[i].VIndex[1]].Position + p.Vertexs[p.Polygons[i].VIndex[2]].Position) * (1.f / 3.f);
					auto pt = map_col.CollCheck_Line(lean_point_q.back() + VGet(0, 10.f, 0.f), lean_point_q.back() + VGet(0, -10.f, 0.f), 0, 0);
					if (pt.HitFlag) { lean_point_q.back() = pt.HitPosition; }
				}
				else if (p.Polygons[i].MaterialIndex == 4) {
					spawn_point.resize(spawn_point.size() + 1);
					spawn_point.back() = (VECTOR_ref(p.Vertexs[p.Polygons[i].VIndex[0]].Position) + p.Vertexs[p.Polygons[i].VIndex[1]].Position + p.Vertexs[p.Polygons[i].VIndex[2]].Position) * (1.f / 3.f);
					auto pt = map_col.CollCheck_Line(spawn_point.back() + VGet(0, 10.f, 0.f), spawn_point.back() + VGet(0, -10.f, 0.f), 0, 0);
					if (pt.HitFlag) { spawn_point.back() = pt.HitPosition; }
				}
			}
		}

		SetFogStartEnd(40.0f-15.f, 40.f);
		SetFogColor(12, 6, 0);

		item_data.clear();
		{
			int mdata = FileRead_open(item_txt, FALSE);
			//item_data magitem
			while (true) {
				auto p = getparams::_str(mdata);
				if (getparams::getright(p.c_str()).find("end") == std::string::npos) {
					size_t p1 = 0;
					float p2 = 0.f, p3 = 0.f, p4 = 0.f;
					for (auto& g : mag_data) {
						if (p.find(g.mod.name) != std::string::npos) {
							p1 = g.id;
							break;
						}
					}
					p2 = getparams::_float(mdata);
					p3 = getparams::_float(mdata);
					p4 = getparams::_float(mdata);

					item_data.resize(item_data.size() + 1);
					item_data.back().id = item_data.size() - 1;
					item_data.back().Set_item(&mag_data[p1], VGet(p2, p3, p4), MGetIdent());
					if (item_data.back().ptr_mag != nullptr) {
						item_data.back().magazine.cap = int(item_data.back().ptr_mag->cap);
					}
				}
				else {
					break;
				}
			}
			//item_data meditem
			while (true) {
				auto p = getparams::_str(mdata);
				if (getparams::getright(p.c_str()).find("end") == std::string::npos) {
					size_t p1 = 0;
					float p2 = 0.f, p3 = 0.f, p4 = 0.f;
					for (auto& g : med_data) {
						if (p.find(g.mod.name) != std::string::npos) {
							p1 = g.id;
							break;
						}
					}
					p2 = getparams::_float(mdata);
					p3 = getparams::_float(mdata);
					p4 = getparams::_float(mdata);

					item_data.resize(item_data.size() + 1);
					item_data.back().id = item_data.size() - 1;
					item_data.back().Set_item(&med_data[p1], VGet(p2, p3, p4), MGetIdent());
					/*
					if (item_data.back().ptr_mag != nullptr) {
						item_data.back().magazine.cap = int(item_data.back().ptr_mag->cap);
					}
					*/
				}
				else {
					break;
				}
			}
			FileRead_close(mdata);
		}

		minmap.GetSize(&x_size, &y_size);


		{
			/*grass*/
			MV1SetupReferenceMesh(grass.get(), -1, TRUE); /*�Q�Ɨp���b�V���̍쐬*/
			RefMesh = MV1GetReferenceMesh(grass.get(), -1, TRUE); /*�Q�Ɨp���b�V���̎擾*/
			IndexNum = RefMesh.PolygonNum * 3 * grasss; /*�C���f�b�N�X�̐����擾*/
			VerNum = RefMesh.VertexNum * grasss;	/*���_�̐����擾*/
			grassver.resize(VerNum);   /*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/
			grassind.resize(IndexNum); /*���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[���郁�����̈�̊m��*/

			vnum = 0;
			pnum = 0;

			int grass_pos = LoadSoftImage(buf);
			int xs = 0, ys = 0;
			GetSoftImageSize(grass_pos, &xs, &ys);

			float x_max = map_col.mesh_maxpos(0).x();
			float z_max = map_col.mesh_maxpos(0).z();
			float x_min = map_col.mesh_minpos(0).x();
			float z_min = map_col.mesh_minpos(0).z();

			for (int i = 0; i < grasss; ++i) {

				float x_t = (float)(GetRand(int((x_max - x_min)) * 100) - int(x_max - x_min) * 50) / 100.0f;
				float z_t = (float)(GetRand(int((z_max - z_min)) * 100) - int(z_max - z_min) * 50) / 100.0f;
				int _r_, _g_, _b_, _a_;
				while (true) {
					GetPixelSoftImage(grass_pos, int((x_t - x_min) / (x_max - x_min)*float(xs)), int((z_t - z_min) / (z_max - z_min)*float(ys)), &_r_, &_g_, &_b_, &_a_);
					if (_r_ <= 128) {
						break;
					}
					else {
						x_t = (float)(GetRand(int((x_max - x_min)) * 100) - int(x_max - x_min) * 50) / 100.0f;
						z_t = (float)(GetRand(int((z_max - z_min)) * 100) - int(z_max - z_min) * 50) / 100.0f;
					}
				}
				VECTOR_ref tmpvect2 = VGet(x_t, -5.f, z_t);
				VECTOR_ref tmpvect = VGet(x_t, 5.f, z_t);
				map_col_nearest(tmpvect2, &tmpvect);
				//
				MV1SetMatrix(grass.get(), MMult(MMult(MGetRotY(deg2rad(GetRand(90))), MGetScale(VGet(1.f, float(100-50+GetRand(50*2))/100.f, 1.f))), MGetTranslate(tmpvect.get())));
				//���
				MV1RefreshReferenceMesh(grass.get(), -1, TRUE);       /*�Q�Ɨp���b�V���̍X�V*/
				RefMesh = MV1GetReferenceMesh(grass.get(), -1, TRUE); /*�Q�Ɨp���b�V���̎擾*/
				for (int j = 0; j < RefMesh.VertexNum; ++j) {
					auto& g = grassver[j + vnum];
					g.pos = RefMesh.Vertexs[j].Position;
					g.norm = RefMesh.Vertexs[j].Normal;
					g.dif = RefMesh.Vertexs[j].DiffuseColor;
					g.spc = RefMesh.Vertexs[j].SpecularColor;
					g.u = RefMesh.Vertexs[j].TexCoord[0].u;
					g.v = RefMesh.Vertexs[j].TexCoord[0].v;
					g.su = RefMesh.Vertexs[j].TexCoord[1].u;
					g.sv = RefMesh.Vertexs[j].TexCoord[1].v;
				}
				for (size_t j = 0; j < size_t(RefMesh.PolygonNum); ++j) {
					for (size_t k = 0; k < std::size(RefMesh.Polygons[j].VIndex); ++k)
						grassind[j * 3 + k + pnum] = WORD(RefMesh.Polygons[j].VIndex[k] + vnum);
				}
				vnum += RefMesh.VertexNum;
				pnum += RefMesh.PolygonNum * 3;
			}
			DeleteSoftImage(grass_pos);

			VerBuf = CreateVertexBuffer(VerNum, DX_VERTEX_TYPE_NORMAL_3D);
			IndexBuf = CreateIndexBuffer(IndexNum, DX_INDEX_TYPE_32BIT);
			SetVertexBufferData(0, grassver.data(), VerNum, VerBuf);
			SetIndexBufferData(0, grassind.data(), IndexNum, IndexBuf);
		}
	}
	void Start_map() {
		envi.play(DX_PLAYTYPE_LOOP, TRUE);
	}
	void Delete_map() {
		map.Dispose();		   //map
		map_col.Dispose();		   //map�R���W����
		sky.Dispose();	 //��
		envi.Dispose();
		way_point.clear();
		lean_point_q.clear();
		lean_point_e.clear();
		spawn_point.clear();
		minmap.Dispose();

		grass_pic.Dispose();
		grass.Dispose();
		grassver.clear();
		grassind.clear();
	}
	auto& map_get() { return map; }
	auto& map_col_get() { return map_col; }
	auto map_col_line(const VECTOR_ref& startpos, const VECTOR_ref& endpos) {
		return map_col.CollCheck_Line(startpos, endpos, 0, 0);
	}

	void map_col_nearest(const VECTOR_ref& startpos, VECTOR_ref* endpos) {
		while (true) {
			auto p = this->map_col_line(startpos, *endpos);
			if (p.HitFlag) {
				if (*endpos == p.HitPosition) {
					break;
				}
				*endpos = p.HitPosition;
			}
			else {
				break;
			}
		}
	}
	void map_col_wall(const VECTOR_ref& OldPos, VECTOR_ref* NowPos) {
		auto MoveVector = *NowPos - OldPos;
		// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����( ���o����͈͈͂ړ��������l������ )
		auto HitDim = map_col.CollCheck_Sphere(OldPos, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size(), 0, 0);
		std::vector<MV1_COLL_RESULT_POLY*> kabe_;// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă���
		// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
		for (int i = 0; i < HitDim.HitNum; i++) {
			auto& h_d = HitDim.Dim[i];
			//�ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{0.1f��荂���|���S���̂ݓ����蔻����s��
			if (
				(abs(atan2f(h_d.Normal.y, std::hypotf(h_d.Normal.x, h_d.Normal.z)))<=deg2rad(30))
				&& (h_d.Position[0].y > OldPos.y() + 0.1f || h_d.Position[1].y > OldPos.y() + 0.1f || h_d.Position[2].y > OldPos.y() + 0.1f)
				&& (h_d.Position[0].y < OldPos.y() + 1.6f || h_d.Position[1].y < OldPos.y() + 1.6f || h_d.Position[2].y< OldPos.y() + 1.6f)
				) {
				kabe_.emplace_back(&h_d);// �|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
			}
		}
		// �ǃ|���S���Ƃ̓����蔻�菈��
		if (kabe_.size() > 0) {
			bool HitFlag = false;
			for (auto& k_ : kabe_) {
				if (Hit_Capsule_Tri(*NowPos, *NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, k_->Position[0], k_->Position[1], k_->Position[2])) {				// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
					HitFlag = true;// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
					if (MoveVector.size() >= 0.0001f) {	// x����y�������� 0.0001f �ȏ�ړ������ꍇ�͈ړ������Ɣ���
						// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
						*NowPos = VECTOR_ref(k_->Normal).cross(MoveVector.cross(k_->Normal))+ OldPos;
						bool j = false;
						for (auto& b_ : kabe_) {
							if (Hit_Capsule_Tri(*NowPos, *NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {
								j = true;
								break;// �������Ă����烋�[�v���甲����
							}
						}
						if (!j) {
							HitFlag = false;
							break;//�ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂ŕǂɓ��������t���O��|������Ń��[�v���甲����
						}
					}
					else {
						break;
					}
				}
			}
			//*
			if (
				HitFlag
				) {		// �ǂɓ������Ă�����ǂ��牟���o���������s��
				for (int k = 0; k < PLAYER_HIT_TRYNUM; k++) {			// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
					bool HitF = false;
					for (auto& k_ : kabe_) {
						if (Hit_Capsule_Tri(*NowPos, *NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, k_->Position[0], k_->Position[1], k_->Position[2])) {// �v���C���[�Ɠ������Ă��邩�𔻒�
							*NowPos += VECTOR_ref(k_->Normal) * PLAYER_HIT_SLIDE_LENGTH;					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
							bool j = false;
							for (auto& b_ : kabe_) {
								if (Hit_Capsule_Tri(*NowPos, *NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {// �������Ă����烋�[�v�𔲂���
									j = true;
									break;
								}
							}
							if (!j) {// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
								break;
							}
							HitF = true;
						}
					}
					if (!HitF) {//�S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
						break;
					}
				}
			}
			//*/
			kabe_.clear();
		}
		MV1CollResultPolyDimTerminate(HitDim);	// ���o�����v���C���[�̎��͂̃|���S�������J������
	}

	//��`��
	void sky_draw(void) {
		{
			SetFogEnable(FALSE);
			SetUseLighting(FALSE);
			sky.DrawModel();
			SetUseLighting(TRUE);
			SetFogEnable(TRUE);
		}
		return;
	}
	void grass_draw(void) {
		SetFogStartEnd(0.0f, 500.f);
		SetFogColor(192, 192, 192);

		SetDrawAlphaTest(DX_CMP_GREATER, 128);
		SetUseLighting(FALSE);
		{
			DrawPolygonIndexed3D_UseVertexBuffer(VerBuf, IndexBuf, grass_pic.get(), TRUE);
		}
		SetUseLighting(TRUE);
		SetDrawAlphaTest(-1, 0);
	}

};
class Minimapclass :Mainclass {
private:
	int x_size = 0;
	int y_size = 0;
	int x_pos = 0;
	int y_pos = 0;

	GraphHandle UI_player;			//�`��X�N���[��
	GraphHandle UI_minimap;			//�`��X�N���[��
	float xcam = 1.f;
	int MaskHandle;
	bool loadmasks = true;
public:
	void load() {
		SetUseASyncLoadFlag(FALSE);
		CreateMaskScreen();
		MaskHandle = LoadMask("data/UI/testMask_.bmp");
		GetMaskSize(&x_size, &y_size, MaskHandle);
		UI_minimap = GraphHandle::Make(x_size, y_size, true);
		UI_player = GraphHandle::Load("data/UI/player.bmp");
		loadmasks = true;
	}

	void set(std::vector<Chara>&chara, Mainclass::Chara& cc,std::unique_ptr<Mapclass, std::default_delete<Mapclass>>& mapparts) {
		UI_minimap.SetDraw_Screen(true);
		{
			DrawBox(0, 0, x_size, y_size, GetColor(0, 128, 0), TRUE);
			int xp = x_size / 2;
			int yp = y_size / 2;
			float radp = 0.f;
			{
				easing_set(&xcam, 1.f + (cc.add_vec_real.size() / ((cc.running ? 6.f : ((cc.ads.first ? 2.f : 4.f)*(cc.squat.first ? 0.4f : 1.f))) / GetFPS())) * 0.3f, 0.9f);

				auto t = cc.body.GetMatrix().pos();
				VECTOR_ref vec_z = cc.body.GetFrameLocalWorldMatrix(cc.frame_s.head_f.first).zvec()*-1.f;
				radp = -atan2f(vec_z.x(), vec_z.z());
				auto x_2 = t.x() / mapparts->map_col_get().mesh_maxpos(0).x() *(mapparts->get_x_size() / 2)*xcam;
				auto y_2 = -t.z() / mapparts->map_col_get().mesh_maxpos(0).z() *(mapparts->get_y_size() / 2)*xcam;
				xp -= int(x_2*cos(radp) - y_2 * sin(radp));
				yp -= int(y_2*cos(radp) + x_2 * sin(radp));
			}

			mapparts->get_minmap().DrawRotaGraph(xp, yp, xcam, radp, true);
			for (auto& c : chara) {
				auto t = (c.pos + c.pos_HMD - c.rec_HMD);
				VECTOR_ref vec_z = c.body.GetFrameLocalWorldMatrix(c.frame_s.head_f.first).zvec()*-1.f;
				auto rad = atan2f(vec_z.x(), vec_z.z());
				auto x_2 = t.x() / mapparts->map_col_get().mesh_maxpos(0).x() *(mapparts->get_x_size() / 2)*xcam;
				auto y_2 = -t.z() / mapparts->map_col_get().mesh_maxpos(0).z() *(mapparts->get_y_size() / 2)*xcam;

				int xt = xp + int(x_2*cos(radp) - y_2 * sin(radp));
				int yt = yp + int(y_2*cos(radp) + x_2 * sin(radp));
				UI_player.DrawRotaGraph(xt, yt, xcam, rad + radp, true);
			}
		}
	}

	void draw(const int& xpos, const int& ypos) {
		if (x_pos != xpos || y_pos != ypos) {
			loadmasks = true;
		}
		if (loadmasks) {
			FillMaskScreen(0);
			DrawMask(xpos, ypos, MaskHandle, DX_MASKTRANS_BLACK);
			loadmasks = false;
		}
		x_pos = xpos;
		y_pos = ypos;

		SetUseMaskScreenFlag(TRUE);
		UI_minimap.DrawGraph(xpos, ypos, true);
		SetUseMaskScreenFlag(FALSE);
	}
};