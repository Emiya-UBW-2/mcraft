#pragma once
#define FRAME_RATE 90.f
//
enum Effect {
	ef_fire, //発砲炎
	ef_reco, //小口径跳弾
	ef_smoke, //銃の軌跡
	ef_gndsmoke,//地面の軌跡
	effects, //読み込む
};
//
class Mainclass {
private:
	//銃用オーディオ
	class Audios {
	private:
	public:
		SoundHandle shot;
		SoundHandle slide;
		SoundHandle trigger;
		SoundHandle mag_down;
		SoundHandle mag_set;

		SoundHandle load_;
		SoundHandle sort_;

		void set(int mdata) {
			SetUseASyncLoadFlag(TRUE);
			SetCreate3DSoundFlag(TRUE);
			shot = SoundHandle::Load("data/audio/shot_" + getparams::_str(mdata) + ".wav");
			slide = SoundHandle::Load("data/audio/slide_" + getparams::_str(mdata) + ".wav");
			trigger = SoundHandle::Load("data/audio/trigger_" + getparams::_str(mdata) + ".wav");
			mag_down = SoundHandle::Load("data/audio/mag_down_" + getparams::_str(mdata) + ".wav");
			mag_set = SoundHandle::Load("data/audio/mag_set_" + getparams::_str(mdata) + ".wav");
			load_ = SoundHandle::Load("data/audio/load.wav");
			sort_ = SoundHandle::Load("data/audio/sort.wav");
			SetCreate3DSoundFlag(FALSE);
			SetUseASyncLoadFlag(FALSE);

		}

		void Duplicate(Audios& tgt) {
			SetCreate3DSoundFlag(TRUE);
			this->shot = tgt.shot.Duplicate();
			this->slide = tgt.slide.Duplicate();
			this->trigger = tgt.trigger.Duplicate();
			this->mag_down = tgt.mag_down.Duplicate();
			this->mag_set = tgt.mag_set.Duplicate();
			this->load_ = tgt.load_.Duplicate();
			this->sort_ = tgt.sort_.Duplicate();
			SetCreate3DSoundFlag(FALSE);
		}

		void Dispose() {
			this->shot.Dispose();
			this->slide.Dispose();
			this->trigger.Dispose();
			this->mag_down.Dispose();
			this->mag_set.Dispose();
			this->load_.Dispose();
			this->sort_.Dispose();
		}
	};
	//銃、マガジン共通モデル
	class Models {
	private:
	public:
		GraphHandle UIScreen;
		int xsize = 1;
		int ysize = 1;
		MV1 model;
		std::string name;
		//*
		void Ready(std::string path, std::string named) {
			name = named;
			MV1::Load(path + named + "/model.mv1", &model, true);
			UIScreen = GraphHandle::Load(path + named + "/pic.bmp");
		}
		//*/
		void set() {
			GetGraphSize(UIScreen.get(), &xsize, &ysize);
		}
	};
public:
	class Chara;
	//必要アイテム
	class need_attach {
	private:
		size_t type = 0;
		bool ok = false;
	public:
	};
	//装着可能アイテム
	class can_attach {
	private:
		std::string name;
	public:
	};
	//パーツオブジェ
	class Parts_info {
	private:
		size_t type = 0;
		std::vector<Parts_info> parts;
		std::vector<can_attach> cans;
		std::vector<need_attach> needs;
	public:

	};
	//弾オブジェ
	class Ammos_info {
	private:
		size_t type = 0;
		float caliber = 0.f;
		float speed = 100.f;//弾速
		float pene = 10.f;//貫通
		float damage = 10.f;//ダメージ
	public:

	};
	//マガジンオブジェ
	class Magazines_info {
	private:
		size_t type = 0;
		size_t type_gun = 0;
		float caliber = 0.f;
		float speed = 100.f;//弾速
		float pene = 10.f;//貫通
		float damage = 10.f;//ダメージ
		size_t capa = 1;
		std::vector<Ammos_info> ammo;
	public:
	};
	//銃オブジェ
	class Gun_info {
	private:
		size_t type_gun = 0;
		std::vector<Parts_info> parts;
		std::vector<can_attach> cans;
		std::vector<need_attach> needs;
	public:
	};
	//アイテム
	class Items_info {
	private:
		int cate = 0;		//種類
		Parts_info parts;
		Ammos_info ammos;
		Magazines_info magazines;
		Gun_info guns;
	public:
	};
public:
	//
	struct Hit {		      /**/
		bool flug{ false };   /*弾痕フラグ*/
		int use{ 0 };	      /*使用フレーム*/
		MV1 pic;	      /*弾痕モデル*/
		VECTOR_ref pos;	      /*座標*/
		MATRIX_ref mat;	      /**/
	};								      /**/
	//弾データ
	class Ammo_info {
	private:
	public:
		std::string name;
		float caliber = 0.f;
		float speed = 100.f;//弾速
		float pene = 10.f;//貫通
		int damage = 10;//ダメージ

		void set(void) {
			int mdata = FileRead_open(("data/ammo/" + this->name + "/data.txt").c_str(), FALSE);
			this->caliber = getparams::_float(mdata)*0.001f;//口径
			this->speed = getparams::_float(mdata);	//弾速
			this->pene = getparams::_float(mdata);	//貫通
			this->damage = getparams::_int(mdata);//ダメージ
			FileRead_close(mdata);
		}
	};
	//実際に発射される弾
	class Ammos {
	private:
	public:
		bool hit{ false };
		float hit_count = 0.f;
		float hit_r = 0.f;
		int hit_x = 0, hit_y = 0;

		bool flug{ false };
		float cnt = 0.f;
		unsigned int color = 0;
		Ammo_info* spec = nullptr;
		float yadd = 0.f;
		VECTOR_ref pos, repos;
		VECTOR_ref vec;
		void ready() {
			this->flug = false;
		}
		void set(Ammo_info* spec_t, const VECTOR_ref& pos_t, const VECTOR_ref& vec_t) {
			this->hit = false;
			this->flug = true;
			this->cnt = 0.f;
			this->color = GetColor(255, 255, 172);
			this->spec = spec_t;
			this->yadd = 0.f;

			this->pos = pos_t;
			this->repos = this->pos;
			this->vec = vec_t;
		}
		template<class Y, class D>
		void update(Chara* c, std::vector<Chara>* chara, std::unique_ptr<Y, D>& mapparts, std::vector<Hit>& hit_obj,size_t& hitbuf) {
			if (this->flug) {
				this->repos = this->pos;
				this->pos += this->vec * (this->spec->speed / GetFPS());
				//判定
				{
					auto p = mapparts->map_col_line(this->repos, this->pos);
					if (p.HitFlag) {
						this->pos = p.HitPosition;
					}
					//*
					for (auto& tgt : *chara) {
						if (tgt.HP == 0) {
							continue;
						}
						{
							auto q = tgt.col.CollCheck_Line(this->repos, this->pos, -1,0);
							if (q.HitFlag) {
								this->pos = q.HitPosition;
								//hit
								c->effcs[ef_reco].set(this->pos, q.Normal, 0.1f / 0.1f);
								//
								this->hit = true;
								this->flug = false;
								tgt.HP = std::clamp(tgt.HP - this->spec->damage * 3 / 5, 0, tgt.HP_full);
								if (tgt.HP == 0) {
									c->kill_f = true;
									c->kill_id = &tgt - &(*chara)[0];
									tgt.death_id = &(*c) - &(*chara)[0];
									if (c->kill_time != 0.f) {
										c->kill_streak++;
										c->score += std::clamp(25 - c->kill_streak * 5, 0, 25);
									}
									c->kill_time = 7.f;
									c->score += 50;
									c->kill_count++;
									tgt.death_count++;
								}
								break;
							}
						}
						{
							auto q = tgt.col.CollCheck_Line(this->repos, this->pos, -1,1);
							if (q.HitFlag) {
								this->pos = q.HitPosition;
								//hit
								c->effcs[ef_reco].set(this->pos, q.Normal, 0.1f / 0.1f);
								//
								this->hit = true;
								this->flug = false;
								tgt.HP = std::clamp(tgt.HP - this->spec->damage*3, 0, tgt.HP_full);
								if (tgt.HP == 0) {
									c->kill_f = true;
									c->kill_id = &tgt - &(*chara)[0];
									tgt.death_id = &(*c) - &(*chara)[0];
									if (c->kill_time != 0.f) {
										c->kill_streak++;
										c->score += std::clamp(25 - c->kill_streak * 5, 0, 25);
									}
									c->kill_time = 7.f;
									c->score += 70;
									c->kill_count++;
									tgt.death_count++;
								}
								break;
							}
						}
						{
							auto q = tgt.col.CollCheck_Line(this->repos, this->pos, -1,2);
							if (q.HitFlag) {
								this->pos = q.HitPosition;
								//hit
								c->effcs[ef_reco].set(this->pos, q.Normal, 0.1f / 0.1f);
								//
								this->hit = true;
								this->flug = false;
								tgt.HP = std::clamp(tgt.HP - this->spec->damage, 0, tgt.HP_full);
								if (tgt.HP == 0) {
									c->kill_f = true;
									c->kill_id = &tgt - &(*chara)[0];
									tgt.death_id = &(*c) - &(*chara)[0];
									if (c->kill_time != 0.f) {
										c->kill_streak++;
										c->score += std::clamp(25 - c->kill_streak * 5, 0, 25);
									}
									c->kill_time = 7.f;
									c->score += 100;
									c->kill_count++;
									tgt.death_count++;
								}
								break;
							}
						}
					}
					if (p.HitFlag && this->flug) {
						this->flug = false;
						c->effcs_gndhit[c->use_effcsgndhit].set(this->pos, p.Normal, 0.025f / 0.1f);
						++c->use_effcsgndhit %= c->effcs_gndhit.size();

						//弾痕のセット
						{
							float asize = this->spec->caliber*100.f*2.f;
							VECTOR_ref y_vec = p.Normal;
							VECTOR_ref z_vec = y_vec.cross(this->vec.Norm()).Norm();
							VECTOR_ref scale = VGet(asize / std::abs(this->vec.Norm().dot(y_vec)), asize, asize);

							hit_obj[hitbuf].use = 0;
							hit_obj[hitbuf].mat = MATRIX_ref::Scale(scale)*  MATRIX_ref::Axis1(y_vec.cross(z_vec), y_vec, z_vec);
							hit_obj[hitbuf].pos = this->pos + y_vec * 0.02f;
							hit_obj[hitbuf].flug = true;
							++hitbuf %= hit_obj.size();
						}
					}
					//*/
				}
				//消す(3秒たった、スピードが0以下、貫通が0以下)
				if (this->cnt >= 3.f || this->spec->speed < 0.f || this->spec->pene <= 0.f) {
					this->flug = false;
				}
				//
			}
		}
		void draw() {
			if (this->flug) {
				DXDraw::Capsule3D(this->pos, this->repos, ((this->spec->caliber - 0.00762f) * 0.1f + 0.00762f), this->color, GetColor(255, 255, 255));
			}
		}
	};
public:
	class Guns;
	class Mags;
	class Meds;
	//銃データ
	class Guns {
	private:
	public:
		size_t id = 0;
		Models mod;
		/**/
		//std::vector<frames> frame;
		class fs {
		public:
			frames magazine_f;
			frames magazine2_f;
			frames mazzule_f;
			frames left_f;
			frames site_f;
			frames right_f;

			void reset() {
				magazine_f.first = INT_MAX;
				magazine2_f.first = INT_MAX;
				mazzule_f.first = INT_MAX;
				left_f.first = INT_MAX;
				site_f.first = INT_MAX;
				right_f.first = INT_MAX;
			}

			void get_frame(MV1& obj_) {
				for (int i = 0; i < int(obj_.frame_num()); i++) {
					std::string p = obj_.frame_name(i);
					if (p.find("mag_fall") != std::string::npos) {
						this->magazine_f = { i, obj_.frame(i) };//マガジン
						this->magazine2_f = { i + 1, obj_.frame(i + 1) };//マガジン
					}
					else if (p.find("mazzule") != std::string::npos) {
						this->mazzule_f = { i, obj_.frame(i) };//マズル
					}
					else if (p.find("LEFT") != std::string::npos) {
						this->left_f = { i, obj_.frame(i) };//左手
					}
					else if (p.find("site") != std::string::npos) {
						this->site_f = { i, obj_.frame(i) };//アイアンサイト
					}
					else if (p.find("RIGHT") != std::string::npos) {
						this->right_f = { i, obj_.frame(i) };//左手
					}
				}
			}

		};
		fs frame_s;
		std::vector <uint8_t> select;//セレクター
		Audios audio;
		float recoil_xup = 0.f;
		float recoil_xdn = 0.f;
		float recoil_yup = 0.f;
		float recoil_ydn = 0.f;
		float reload_time = 1.f;//再装填時間
		float ammo_speed = 100.f;//弾速
		float ammo_pene = 10.f;//貫通
		float ammo_damege = 10.f;//ダメージ
		std::vector<Ammo_info> ammo;
		Mags* magazine = nullptr;
		/**/
		void set_data(std::vector<Mags>& mag_data, const size_t& id_) {
			this->id = id_;
			this->mod.model.SetMatrix(MGetIdent());
			{
				//フレーム
				this->frame_s.reset();
				this->frame_s.get_frame(this->mod.model);
				//テキスト
				{
					int mdata = FileRead_open(("data/gun/" + this->mod.name + "/data.txt").c_str(), FALSE);
					//セレクター設定
					while (true) {
						auto p = getparams::_str(mdata);
						if (getparams::getright(p.c_str()).find("end") != std::string::npos) {
							break;
						}
						else if (getparams::getright(p.c_str()).find("semi") != std::string::npos) {
							this->select.emplace_back(uint8_t(1));					//セミオート=1
						}
						else if (getparams::getright(p.c_str()).find("full") != std::string::npos) {
							this->select.emplace_back(uint8_t(2));					//フルオート=2
						}
						else if (getparams::getright(p.c_str()).find("3b") != std::string::npos) {
							this->select.emplace_back(uint8_t(3));					//3連バースト=3
						}
						else if (getparams::getright(p.c_str()).find("2b") != std::string::npos) {
							this->select.emplace_back(uint8_t(4));					//2連バースト=4
						}
						else {
							this->select.emplace_back(uint8_t(1));
						}
					}
					//サウンド
					this->audio.set(mdata);
					//弾データ
					while (true) {
						auto p = getparams::_str(mdata);
						if (getparams::getright(p.c_str()).find("end") != std::string::npos) {
							break;
						}
						else {
							this->ammo.resize(this->ammo.size() + 1);
							this->ammo.back().name = p;
						}
					}
					//マガジン
					auto tmp = getparams::_str(mdata);
					//反動
					this->recoil_xup = getparams::_float(mdata);
					this->recoil_xdn = getparams::_float(mdata);
					this->recoil_yup = getparams::_float(mdata);
					this->recoil_ydn = getparams::_float(mdata);
					//リロードタイム
					this->reload_time = getparams::_float(mdata);
					FileRead_close(mdata);
					//
					for (auto& a : this->ammo) {
						a.set();
					}
					for (auto& g : mag_data) {
						if (tmp == g.mod.name) {
							this->magazine = &g;
						}
					}
				}
			}
			this->mod.set();
		}
	};
	//マガジンデータ
	class Mags {
	private:
	public:
		size_t id = 0;
		Models mod;
		size_t cap = 1;
		std::vector<Ammo_info> ammo;
		void set_data(const size_t& id_) {
			this->id = id_;
			this->mod.model.SetMatrix(MGetIdent());
			{
				int mdata = FileRead_open(("data/mag/" + this->mod.name + "/data.txt").c_str(), FALSE);
				this->cap = getparams::_long(mdata);//口径
				//弾データ
				while (true) {
					auto p = getparams::_str(mdata);
					if (getparams::getright(p.c_str()).find("end") != std::string::npos) {
						break;
					}
					else {
						this->ammo.resize(this->ammo.size() + 1);
						this->ammo.back().name = p;
					}
				}
				FileRead_close(mdata);
			}
			this->mod.set();
		}
	};
	//マガジンデータ
	class Meds {
	private:
	public:
		size_t id = 0;
		Models mod;
		/**/
		int repair = 0;
		/**/
		void set_data(const size_t& id_) {
			this->id = id_;
			this->mod.model.SetMatrix(MGetIdent());
			{
				int mdata = FileRead_open(("data/medkit/" + this->mod.name + "/data.txt").c_str(), FALSE);
				this->repair = getparams::_long(mdata);//口径
				FileRead_close(mdata);
			}
			this->mod.set();
		}
	};
public:
	class Items;
	//player
	class Chara {
	private:
		struct ammo_state {//弾データ
			size_t ammo_in = 0;
		};
		struct mag_state {//マガジンデータ
			size_t ammo_in = 0;
		};
		struct gun_state {//銃のデータ
			size_t ammo_cnt = 0;		//現在の装弾数カウント
			size_t ammo_total = 0;		//総所持弾数
			std::vector<size_t> mag_in;	//マガジン内データ
			uint8_t select = 0;			//セレクター

			void init() {
				this->ammo_total = 0;
				//this->select = 0;
				this->mag_in.clear();
			}
			//マガジンを1つ追加(装填無し)
			void mag_plus(Mags* magazine) {
				this->ammo_total += magazine->cap;
				this->mag_in.insert(this->mag_in.end(), magazine->cap);
			}
			//マガジンを1つ追加(装填あり)
			void mag_insert(Mags* magazine) {
				mag_plus(magazine);
				if (this->mag_in.size() >= 1) {
					this->ammo_cnt = this->mag_in.front();//改良
				}
				else {
					this->ammo_cnt = std::clamp<size_t>(this->ammo_total, 0, magazine->cap);//改良
				}
			}
			//マガジンを落とす
			void mag_release() {
				size_t dnm = this->ammo_cnt - 1;
				//弾数
				if (this->ammo_cnt >= 1) {
					this->ammo_cnt = 1;
				}
				else {
					dnm = 0;
				}
				this->ammo_total -= dnm;
				this->mag_in.erase(this->mag_in.begin());
			}
			//マガジンを落とす
			void mag_release_end() {
				this->mag_in.pop_back();
			}
			//射撃する
			void mag_shot(const bool& reloadf) {
				this->ammo_cnt--;
				this->ammo_total--;
				if (!reloadf && this->mag_in.size() >= 1 && this->mag_in.front() > 0) {
					this->mag_in.front()--;
				}
			}
			//スライドを引いて装填
			void mag_slide() {
				if (this->ammo_cnt <= 1) {
					this->ammo_cnt += this->mag_in.front();
				}
			}
		};
		class fs {
		public:
			//
			frames head_f;
			//
			frames LEFTeye_f;
			frames RIGHTeye_f;
			//
			frames bodyg_f;
			frames bodyc_f;
			frames bodyb_f;
			frames body_f;
			//右手座標系
			frames RIGHThand2_f;
			frames RIGHThand_f;
			frames RIGHTarm2_f;
			frames RIGHTarm1_f;
			//左手座標系
			frames LEFThand2_f;
			frames LEFThand_f;
			frames LEFTarm2_f;
			frames LEFTarm1_f;

			//右手座標系
			frames RIGHTreg2_f;
			frames RIGHTreg_f;
			frames RIGHTfoot2_f;
			frames RIGHTfoot1_f;
			//左手座標系
			frames LEFTreg2_f;
			frames LEFTreg_f;
			frames LEFTfoot2_f;
			frames LEFTfoot1_f;

			void get_frame(MV1& obj_,float*head_hight) {
				for (int i = 0; i < int(obj_.frame_num()); i++) {
					std::string p = obj_.frame_name(i);
					if (p == std::string("グルーブ")) {
						this->bodyg_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p == std::string("下半身")) {
						this->bodyc_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}

					else if (p.find("左足") != std::string::npos && p.find("首") == std::string::npos && p.find("先") == std::string::npos && p.find("ＩＫ") == std::string::npos) {
						this->LEFTfoot1_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("左ひざ") != std::string::npos) {
						this->LEFTfoot2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("左足首") != std::string::npos && p.find("先") == std::string::npos) {
						this->LEFTreg_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("左足首先") != std::string::npos) {
						this->LEFTreg2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}

					else if (p.find("右足") != std::string::npos && p.find("首") == std::string::npos && p.find("先") == std::string::npos && p.find("ＩＫ") == std::string::npos) {
						this->RIGHTfoot1_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("右ひざ") != std::string::npos) {
						this->RIGHTfoot2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("右足首") != std::string::npos && p.find("先") == std::string::npos) {
						this->RIGHTreg_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("右足首先") != std::string::npos) {
						this->RIGHTreg2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("上半身") != std::string::npos && p.find("上半身2") == std::string::npos) {
						this->bodyb_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("上半身2") != std::string::npos) {
						this->body_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("頭") != std::string::npos && p.find("先") == std::string::npos) {
						this->head_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
						*head_hight = obj_.frame(this->head_f.first).y();
					}
					else if (p.find("右目先") != std::string::npos) {
						this->RIGHTeye_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("左目先") != std::string::npos) {
						this->LEFTeye_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}

					else if (p.find("右腕") != std::string::npos && p.find("捩") == std::string::npos) {
						this->RIGHTarm1_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("右ひじ") != std::string::npos) {
						this->RIGHTarm2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p == std::string("右手首")) {
						this->RIGHThand_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("右手首先") != std::string::npos) {
						this->RIGHThand2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}

					else if (p.find("左腕") != std::string::npos && p.find("捩") == std::string::npos) {
						this->LEFTarm1_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("左ひじ") != std::string::npos) {
						this->LEFTarm2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p == std::string("左手首")) {
						this->LEFThand_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
					else if (p.find("左手首先") != std::string::npos) {
						this->LEFThand2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),obj_.GetFrameLocalMatrix(i)) };
					}
				}
			}

			void copy_frame(MV1& obj_mine, fs& frame_tgt_,MV1* obj_tgt) {
				obj_tgt->SetMatrix(obj_mine.GetMatrix());
				//
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.head_f.first, obj_mine.GetFrameLocalMatrix(this->head_f.first));
				//
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFTeye_f.first, obj_mine.GetFrameLocalMatrix(this->LEFTeye_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTeye_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHTeye_f.first));
				//
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.bodyg_f.first, obj_mine.GetFrameLocalMatrix(this->bodyg_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.bodyc_f.first, obj_mine.GetFrameLocalMatrix(this->bodyc_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.bodyb_f.first, obj_mine.GetFrameLocalMatrix(this->bodyb_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.body_f.first, obj_mine.GetFrameLocalMatrix(this->body_f.first));
				//右手座標系
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHThand2_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHThand2_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHThand_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHThand_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTarm2_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHTarm2_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTarm1_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHTarm1_f.first));
				//左手座標系
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFThand2_f.first, obj_mine.GetFrameLocalMatrix(this->LEFThand2_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFThand_f.first, obj_mine.GetFrameLocalMatrix(this->LEFThand_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFTarm2_f.first, obj_mine.GetFrameLocalMatrix(this->LEFTarm2_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFTarm1_f.first, obj_mine.GetFrameLocalMatrix(this->LEFTarm1_f.first));
				//右手座標系
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTreg2_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHTreg2_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTreg_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHTreg_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTfoot2_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHTfoot2_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTfoot1_f.first, obj_mine.GetFrameLocalMatrix(this->RIGHTfoot1_f.first));
				//左手座標系
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFTreg2_f.first, obj_mine.GetFrameLocalMatrix(this->LEFTreg2_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFTreg_f.first, obj_mine.GetFrameLocalMatrix(this->LEFTreg_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFTfoot2_f.first, obj_mine.GetFrameLocalMatrix(this->LEFTfoot2_f.first));
				obj_tgt->SetFrameLocalMatrix(frame_tgt_.LEFTfoot1_f.first, obj_mine.GetFrameLocalMatrix(this->LEFTfoot1_f.first));

				for (int i = 0; i < 9; i++) {
					obj_tgt->get_anime(i).per = obj_mine.get_anime(i).per;
					obj_tgt->get_anime(i).time = obj_mine.get_anime(i).time;
				}
			}
		};
	public:
		/*エフェクト*/
		std::array<EffectS, effects> effcs;
		size_t use_effcsgun = 0;
		std::array<EffectS, 12> effcs_gndhit;
		size_t use_effcsgndhit = 0;
		/*確保する弾*/
		std::array<Ammos, 64> bullet;
		size_t use_bullet = 0;
		/*武器データ*/
		std::vector<gun_state> gun_stat;		//所持弾数などのデータ
		Guns* gun_ptr = nullptr;				//現在使用中の武装
		/*音声*/
		Audios audio;
		/*モデル*/
		MV1 obj_gun, obj_mag;
		MV1 body, col;
		/*anime*/
		//脚
		MV1::ani* anime_walk;
		MV1::ani* anime_swalk;
		MV1::ani* anime_run;
		//脚ポーズ
		MV1::ani* anime_sit;
		//死
		MV1::ani* anime_die_1;
		//リロード
		MV1::ani* anime_reload;
		//腕
		MV1::ani* anime_arm_run;
		MV1::ani* anime_arm_check;
		//手
		MV1::ani* anime_hand_nomal;//基本手
		MV1::ani* anime_hand_trigger;//引き金
		MV1::ani* anime_hand_trigger_pull;//引き金引く
		/*プレイヤー操作*/
		float reload_cnt = 0.f;//リロード開始までのカウント
		switchs trigger;//トリガー
		bool gunf = false;//射撃フラグ
		switchs selkey;//トリガー
		bool LEFT_hand = false;//左手を添えているか
		bool reloadf = false;
		bool down_mag = false;
		//プレイヤー座標系
		float speed = 0.f;
		VECTOR_ref pos;				//位置
		MATRIX_ref mat;				//位置
		MATRIX_ref mat_body;		//位置
		VECTOR_ref spawn_pos;		//spawn
		MATRIX_ref spawn_mat;		//spawn
		VECTOR_ref add_vec;			//移動ベクトル
		VECTOR_ref add_vec_buf;		//移動ベクトルバッファ
		VECTOR_ref add_vec_real;	//実際の移動ベクトル
		float xrad_p = 0.f;			//マウスエイム用変数確保
		switchs ads, squat;
		bool wkey = false;
		bool skey = false;
		bool akey = false;
		bool dkey = false;
		bool qkey = false;
		bool ekey = false;
		bool running = false;										//走るか否か
		bool jamp = false;
		bool aim = false;
		bool select = false;
		bool shoot = false;
		bool reload = false;
		bool get_ = false;
		bool delete_ = false;
		bool sort_ = false;
		bool sort_f = false;
		bool sort_ing = false;										//走るか否か
		switchs sortmag;
		VECTOR_ref gunpos;											//マウスエイム用銃座標
		switchs getmag;
		switchs delete_item;
		//AI用
		float ai_time_a = 0.f;
		float ai_time_d = 0.f;
		float ai_time_q = 0.f;
		float ai_time_e = 0.f;
		float ai_time_shoot = 0.f;
		bool ai_reload = false;
		int ai_phase = 0;

		std::array<int, 6> wayp_pre;
		//マガジン座標系
		VECTOR_ref pos_mag;
		MATRIX_ref mat_mag;
		//頭部座標系
		VECTOR_ref pos_HMD, pos_HMD_old, rec_HMD;
		float add_ypos = 0.f;//垂直加速度
		float body_xrad = 0.f;//胴体角度
		float body_yrad = 0.f;//胴体角度
		float body_zrad = 0.f;//胴体角度
		//フレーム
		fs frame_s;
		float head_hight = 0.f;
		float head_hight2 = 0.f;
		//右手座標系
		VECTOR_ref pos_RIGHTHAND;
		VECTOR_ref add_RIGHTHAND;
		MATRIX_ref mat_RIGHTHAND;
		VECTOR_ref vecadd_RIGHTHAND, vecadd_RIGHTHAND_p;//
		//左手座標系
		VECTOR_ref pos_LEFTHAND;
		MATRIX_ref mat_LEFTHAND;
		//右手座標系
		MATRIX_ref mat_RIGHTREG_rep;
		VECTOR_ref pos_RIGHTREG;
		MATRIX_ref mat_RIGHTREG;
		//左手座標系
		MATRIX_ref mat_LEFTREG_rep;
		VECTOR_ref pos_LEFTREG;
		MATRIX_ref mat_LEFTREG;
		//
		VECTOR_ref pos_WAIST_rep;
		MATRIX_ref mat_WAIST_rep;
		VECTOR_ref pos_WAIST;
		MATRIX_ref mat_WAIST;
		//
		fs colframe_;
		//
		size_t canget_id = 0;
		//
		bool canget_gunitem = false;
		std::string canget_gun;
		//
		bool canget_magitem = false;
		std::string canget_mag;
		//
		bool canget_meditem = false;
		std::string canget_med;
		//
		bool start_b = true;
		bool start_c = true;
		float HP_r = 100;
		int HP = 100;
		int HP_full = 100;
		bool kill_f = false;
		size_t kill_id = 0;
		float kill_time = 0.f;
		int kill_streak = 0;
		bool hit_f{ false };
		float hit_time = 0.f;
		int score = 0;
		int kill_count = 0;
		size_t death_id = 0;
		int death_count = 0;
		float death_timer = 0.f;
		//

		void set(std::vector<Guns>& gun_data, const size_t& itr, MV1& body_, MV1& col_) {
			this->gun_ptr = &gun_data[itr];
			//身体
			body_.DuplicateonAnime(&this->body);
			//
			anime_walk = &body.get_anime(1);
			anime_swalk = &body.get_anime(8);
			anime_run = &body.get_anime(2);
			//
			anime_sit = &body.get_anime(7);
			//
			anime_die_1 = &body.get_anime(4);
			//
			anime_reload = &body.get_anime(3);
			//
			anime_arm_run = &body.get_anime(6);
			anime_arm_check = &body.get_anime(10);
			//
			anime_hand_nomal = &body.get_anime(0);
			anime_hand_trigger = &body.get_anime(9);
			anime_hand_trigger_pull = &body.get_anime(5);
			//
			frame_s.get_frame(this->body, &this->head_hight);
			col_.DuplicateonAnime(&this->col);
			colframe_.get_frame(this->col,&this->head_hight2);
			for (int i = 0; i < col.mesh_num(); i++) {
				col.SetupCollInfo(8, 8, 8, -1, i);
			}

			this->gun_ptr->mod.model.DuplicateonAnime(&this->obj_gun);
			this->obj_mag = this->gun_ptr->magazine->mod.model.Duplicate();
			this->LEFT_hand = false;

			//マガジン
			this->gun_stat.resize(gun_data.size());
			std::for_each(this->gun_stat.begin(), this->gun_stat.end(), [](gun_state& i) {i.init(); });

			this->gunf = false;
			this->vecadd_RIGHTHAND = VGet(0, 0, 1.f);
			this->vecadd_RIGHTHAND_p = this->vecadd_RIGHTHAND;
			this->reloadf = false;
			this->down_mag = true;
			this->selkey.ready(false);
			for (auto& a : this->obj_gun.get_anime()) {
				a.reset();
			}
			for (auto& a : this->bullet) {
				a.ready();
			}
			this->audio.Duplicate(this->gun_ptr->audio);
			this->HP = this->HP_full;

			this->kill_f = false;
			this->kill_id = 0;
			this->kill_streak = 0;
			this->kill_time = 0.f;
			this->score = 0;
			this->kill_count = 0;
			this->death_id = 0;
			this->death_count = 0;
		}
		void spawn(const VECTOR_ref& pos_, const MATRIX_ref& mat_H) {
			this->ai_time_shoot = 0.f;
			this->ai_time_a = 0.f;
			this->ai_time_d = 0.f;
			this->ai_time_e = 0.f;

			this->xrad_p = 0;

			this->spawn_pos = pos_;
			this->spawn_mat = mat_H;

			this->pos = this->spawn_pos;
			this->mat = this->spawn_mat;

			this->ads.ready(false);
			this->running = false;
			this->squat.ready(false);

			this->add_ypos = 0.f;
			this->add_vec_buf.clear();
			this->start_b = true;

			this->HP = this->HP_full;

			this->kill_f = false;
			this->kill_id = 0;
			this->death_id = 0;
			this->kill_streak = 0;
			this->kill_time = 0.f;

			this->gun_stat[this->gun_ptr->id].init();
			this->gun_stat[this->gun_ptr->id].mag_insert(this->gun_ptr->magazine);			//マガジン+1(装填あり)
			this->gun_stat[this->gun_ptr->id].mag_plus(this->gun_ptr->magazine);			//マガジン+1(装填無し)
			this->gun_stat[this->gun_ptr->id].mag_plus(this->gun_ptr->magazine);			//マガジン+1(装填無し)
			this->gun_stat[this->gun_ptr->id].mag_plus(this->gun_ptr->magazine);			//マガジン+1(装填無し)
			this->gun_stat[this->gun_ptr->id].mag_plus(this->gun_ptr->magazine);			//マガジン+1(装填無し)
			this->gun_stat[this->gun_ptr->id].mag_plus(this->gun_ptr->magazine);			//マガジン+1(装填無し)

			this->body_xrad = 0.f;//胴体角度
			this->body_yrad = 0.f;//胴体角度
			this->body_zrad = 0.f;//胴体角度
			std::for_each(this->wayp_pre.begin(), this->wayp_pre.end(), [](int& i) {i = 0; });
			this->obj_gun.get_anime(0).reset();
			this->obj_gun.get_anime(1).reset();
			this->obj_gun.get_anime(2).reset();
			this->obj_gun.get_anime(3).reset();
			this->obj_gun.get_anime(4).reset();
			this->obj_gun.get_anime(5).reset();
		}

		void start() {
			this->start_b = false;
			this->start_c = true;
		}

		template<class Y, class D>
		void reset_waypoint(std::unique_ptr<Y, D>& mapparts) {
			int now = -1;
			auto poss = this->pos + this->pos_HMD - this->rec_HMD;
			auto tmp = VECTOR_ref(VGet(0, 100.f, 0));
			for (auto& w : mapparts->get_waypoint()) {
				auto id = &w - &mapparts->get_waypoint()[0];
				bool tt = true;
				for (auto& ww : this->wayp_pre) {
					if (id == ww) {
						tt = false;
					}
				}
				if (tt) {
					if (tmp.size() >= (w - poss).size()) {
						auto p = mapparts->map_col_line(w + VGet(0, 0.5f, 0), poss + VGet(0, 0.5f, 0));
						if (!p.HitFlag) {
							tmp = (w - poss);
							now = int(id);
						}
					}
				}
			}
			if (now != -1) {
				for (auto& w : this->wayp_pre) {
					w = now;
				}
			}
			this->ai_phase = 0;
		}

		template<class Y2, class D2>
		void set_alive(std::vector<Items>& item, std::unique_ptr<Y2, D2>& mapparts) {
			const auto fps_ = GetFPS();
			easing_set(&this->HP_r, float(this->HP), 0.95f);
			if (this->HP == 0) {
				easing_set(&this->anime_hand_nomal->per, 0.f, 0.9f);
				easing_set(&this->anime_walk->per, 0.f, 0.9f);
				easing_set(&this->anime_run->per, 0.f, 0.9f);
				easing_set(&this->anime_reload->per, 0.f, 0.9f);
				easing_set(&this->anime_hand_trigger_pull->per, 0.f, 0.9f);
				easing_set(&this->anime_arm_run->per, 0.f, 0.9f);
				easing_set(&this->anime_arm_check->per, 0.f, 0.9f);
				easing_set(&this->anime_sit->per, 0.f, 0.9f);
				easing_set(&this->anime_swalk->per, 0.f, 0.9f);
				easing_set(&this->anime_hand_trigger->per, 0.f, 0.9f);

				easing_set(&this->anime_die_1->per, 1.f, 0.9f);
				this->anime_die_1->update(false, 1.f);

				if (this->death_timer == 0.f) {
					this->body.frame_reset(this->frame_s.bodyg_f.first);
					this->body.frame_reset(this->frame_s.bodyb_f.first);
					this->body.frame_reset(this->frame_s.body_f.first);
					this->body.frame_reset(this->frame_s.head_f.first);
					this->body.frame_reset(this->frame_s.RIGHTarm1_f.first);
					this->body.frame_reset(this->frame_s.RIGHTarm1_f.first);
					this->body.frame_reset(this->frame_s.RIGHTarm2_f.first);
					this->body.frame_reset(this->frame_s.RIGHTarm2_f.first);
					this->body.frame_reset(this->frame_s.RIGHThand_f.first);
					this->body.frame_reset(this->frame_s.LEFTarm1_f.first);
					this->body.frame_reset(this->frame_s.LEFTarm1_f.first);
					this->body.frame_reset(this->frame_s.LEFTarm2_f.first);
					this->body.frame_reset(this->frame_s.LEFTarm2_f.first);
					this->body.frame_reset(this->frame_s.LEFThand_f.first);

					//マガジン排出
					if (this->gun_stat[this->gun_ptr->id].mag_in.size() >= 1) {
						//音
						this->audio.mag_down.play_3D(this->pos_RIGHTHAND, 15.f);
						//弾数
						auto dnm = (this->gun_stat[this->gun_ptr->id].ammo_cnt >= 1) ? this->gun_stat[this->gun_ptr->id].ammo_cnt - 1 : 0;
						while (true) {
							//マガジン排出
							this->gun_stat[this->gun_ptr->id].mag_release();
							//マガジン排出
							bool tt = false;
							for (auto& g : item) {
								if (g.ptr_gun == nullptr && g.ptr_mag == nullptr && g.ptr_med == nullptr) {
									tt = true;
									g.Set_item(this->gun_ptr->magazine, this->pos_mag, this->mat_mag);
									g.add = (this->obj_gun.frame(this->gun_ptr->frame_s.magazine2_f.first) - this->obj_gun.frame(this->gun_ptr->frame_s.magazine_f.first)).Norm()*-1.f / fps_;//排莢ベクトル
									g.magazine.cap = dnm;
									g.del_timer = 0.f;
									break;
								}
							}
							if (!tt) {
								item.resize(item.size() + 1);
								auto& g = item.back();
								g.id = item.size() - 1;
								g.Set_item(this->gun_ptr->magazine, this->pos_mag, this->mat_mag);
								g.add = VECTOR_ref(VGet(
									float(-10 + GetRand(10 * 2)) / 100.f,
									-1.f,
									float(-10 + GetRand(10 * 2)) / 100.f
								))*1.f / fps_;//排莢ベクトル
								g.magazine.cap = dnm;
								g.del_timer = 0.f;
							}
							//
							if (this->gun_stat[this->gun_ptr->id].mag_in.size() == 0) {
								break;
							}
							dnm = this->gun_stat[this->gun_ptr->id].mag_in.front();
						}
					}
				}
				this->death_timer += 1.f / fps_;
				if (this->death_timer >= 5.f) {
					this->death_timer = 0.f;
					this->anime_die_1->reset();
					this->spawn(this->spawn_pos, this->spawn_mat);
					reset_waypoint(mapparts);
				}

			}
			if (this->kill_f) {
				this->kill_time -= 1.f / fps_;
				if (this->kill_time <= 0.f) {
					this->kill_time = 0.f;
					this->kill_streak = 0;
					this->kill_f = false;
				}
			}

		}

		void Draw_chara() {
			this->body.DrawModel();
			this->obj_gun.DrawModel();
			//this->col.DrawModel();

			if ((!this->reloadf || this->down_mag) && this->gun_stat[this->gun_ptr->id].mag_in.size() >= 1) {
				this->obj_mag.DrawModel();
			}
		}
		void Draw_ammo() {
			for (auto& a : this->bullet) {
				a.draw();
			}
		}

		void Delete_chara() {
			obj_gun.Dispose();
			obj_mag.Dispose();
			body.Dispose();
			col.Dispose();

			this->gun_ptr = nullptr;
			this->body.Dispose();
			this->col.Dispose();
			this->obj_gun.Dispose();
			this->obj_mag.Dispose();
			this->audio.Dispose();
			for (auto& t : this->effcs) {
				t.handle.Dispose();
			}
			for (auto& t : this->effcs_gndhit) {
				t.handle.Dispose();
			}
			this->gun_stat.clear();
		}

		void operation_2_1(const bool& cannotmove, int32_t x_m, int32_t y_m) {
			if (this->running) {
				this->squat.first = false;
			}
			if (this->ads.first) {
				this->running = false;
			}
			if (!this->wkey) {
				this->running = false;
			}
			if (!this->wkey && !this->skey && !this->akey && !this->dkey) {
				this->running = false;
			}
			if (this->skey) {
				this->running = false;
			}

			if (this->running) {
				this->skey = false;
			}

			if (this->running) {
				this->qkey = false;
				this->ekey = false;
			}


			if (this->HP != 0) {
				this->mat *= MATRIX_ref::RotAxis(this->mat.zvec(), this->body_zrad).Inverse();
				if (this->qkey) {
					easing_set(&this->body_zrad, deg2rad(-40), 0.9f);
				}
				else if (this->ekey) {
					easing_set(&this->body_zrad, deg2rad(40), 0.9f);
				}
				else {
					easing_set(&this->body_zrad, 0.f, 0.9f);
				}
				this->mat *= MATRIX_ref::RotAxis(this->mat.zvec(), this->body_zrad);
				//
				this->mat = MATRIX_ref::RotX(-this->xrad_p)*this->mat;
				this->xrad_p = std::clamp(this->xrad_p - deg2rad(y_m)*0.1f, deg2rad(-80), deg2rad(60));
				this->mat *= MATRIX_ref::RotY(deg2rad(x_m)*0.1f);//y軸回転
				this->mat = MATRIX_ref::RotX(this->xrad_p)*this->mat;//x軸回転
			}

			if (cannotmove || this->HP == 0) {
				this->wkey = false;
				this->skey = false;
				this->akey = false;
				this->dkey = false;
				this->shoot = false;
				this->running = false;
				this->squat.first = false;
				this->qkey = false;
				this->ekey = false;
				this->aim = false;
				this->reload = false;
				this->get_ = false;
				this->sort_ = false;
				this->delete_ = false;
				this->select = false;
				this->down_mag = true;
				this->jamp = false;
			}

			this->speed = (this->running ? 6.f : ((this->ads.first ? 2.f : 4.f)*(this->squat.first ? 0.4f : 1.f))) / GetFPS();
			VECTOR_ref zv_t = this->mat.zvec();
			zv_t.y(0.f);
			zv_t = zv_t.Norm();

			VECTOR_ref xv_t = this->mat.xvec();
			xv_t.y(0.f);
			xv_t = xv_t.Norm();

			if (this->running) {
				xv_t = xv_t.Norm()*0.5f;
				easing_set(&this->add_vec_buf,
					VECTOR_ref(VGet(0, 0, 0))
					+ (this->wkey ? (zv_t*-this->speed) : VGet(0, 0, 0))
					+ (this->akey ? (xv_t*this->speed) : VGet(0, 0, 0))
					+ (this->dkey ? (xv_t*-this->speed) : VGet(0, 0, 0))
					, 0.95f);
				this->mat_body = MATRIX_ref::Axis1(this->mat.yvec().cross(this->add_vec_buf.Norm()*-1.f), this->mat.yvec(), this->add_vec_buf.Norm()*-1.f);
			}
			else {
				easing_set(&this->add_vec_buf,
					VECTOR_ref(VGet(0, 0, 0))
					+ (this->wkey ? (zv_t*-this->speed) : VGet(0, 0, 0))
					+ (this->skey ? (zv_t*this->speed) : VGet(0, 0, 0))
					+ (this->akey ? (xv_t*this->speed) : VGet(0, 0, 0))
					+ (this->dkey ? (xv_t*-this->speed) : VGet(0, 0, 0))
					, 0.95f);
				this->mat_body = this->mat;
			}
		}
		void operation_VR(std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& Drawparts, const bool& cannotmove, bool* oldv_1_1) {
			//操作
			{
				this->aim = false;
				this->reload = false;
				this->get_ = false;
				this->sort_ = false;
				this->delete_ = false;
				this->select = false;

				this->shoot = false;
				this->jamp = false;
				if (this->HP != 0) {
					if (Drawparts->get_hand1_num() != -1) {
						auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hand1_num()];
						if (ptr_.turn && ptr_.now) {
							//射撃
							this->shoot = (ptr_.on[0] & BUTTON_TRIGGER) != 0;
							//マグキャッチ
							this->reload = (ptr_.on[0] & BUTTON_SIDE) != 0;
							//セレクター
							this->select = ((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_.touch.x() > 0.5f && ptr_.touch.y() < 0.5f&&ptr_.touch.y() > -0.5f);
						}
					}
					if (Drawparts->get_hand2_num() != -1) {
						auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hand2_num()];
						if (ptr_.turn && ptr_.now) {
							//マガジン持つ
							this->down_mag |= (((ptr_.on[0] & BUTTON_TRIGGER) != 0) && (this->gun_stat[this->gun_ptr->id].mag_in.size() >= 1));
							//アイテム取得
							this->get_ = (ptr_.on[0] & BUTTON_TOPBUTTON_B) != 0;
							//
							this->sort_ = false;
							//running
							this->running = (ptr_.on[0] & BUTTON_TOUCHPAD) != 0;
							//jamp
							this->jamp = (ptr_.on[0] & BUTTON_SIDE) != 0;
							//移動
							if ((ptr_.on[1] & BUTTON_TOUCHPAD) != 0) {
								this->speed = (this->running ? 6.f : 4.f) / GetFPS();

								if (Drawparts->tracker_num.size() > 0) {
									auto p = this->body.GetFrameLocalWorldMatrix(this->frame_s.bodyb_f.first);
									easing_set(&this->add_vec_buf, (p.zvec()*-ptr_.touch.y() + p.xvec()*-ptr_.touch.x())*this->speed, 0.95f);
								}
								else {
									easing_set(&this->add_vec_buf, (this->mat.zvec()*ptr_.touch.y() + this->mat.xvec()*ptr_.touch.x())*this->speed, 0.95f);
								}
							}
							else {
								easing_set(&this->add_vec_buf, VGet(0, 0, 0), 0.95f);
							}
						}
					}
				}
				//
			}
			//HMD_mat
			{
				//+視点取得
				auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hmd_num()];
				this->pos_HMD_old = this->pos_HMD;
				Drawparts->GetDevicePositionVR(Drawparts->get_hmd_num(), &this->pos_HMD, &this->mat);
				if (this->start_c || (ptr_.turn && ptr_.now) != *oldv_1_1) {
					this->rec_HMD = VGet(this->pos_HMD.x(), 0.f, this->pos_HMD.z());
				}
				*oldv_1_1 = ptr_.turn && ptr_.now;
			}
		}
		void operation(const bool& cannotmove) {
			//HMD_mat
			int32_t x_m = 0, y_m = 0;
			if (this->HP != 0) {
				//操作
				this->wkey = (CheckHitKey(KEY_INPUT_W) != 0);
				this->skey = (CheckHitKey(KEY_INPUT_S) != 0);
				this->akey = (CheckHitKey(KEY_INPUT_A) != 0);
				this->dkey = (CheckHitKey(KEY_INPUT_D) != 0);
				this->running = (CheckHitKey(KEY_INPUT_LSHIFT) != 0);
				this->squat.get_in(CheckHitKey(KEY_INPUT_C) != 0);
				this->qkey = (CheckHitKey(KEY_INPUT_Q) != 0);
				this->ekey = (CheckHitKey(KEY_INPUT_E) != 0);
				this->aim = ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0);
				this->reload = CheckHitKey(KEY_INPUT_R) != 0;
				this->get_ = CheckHitKey(KEY_INPUT_F) != 0;
				this->sort_ = CheckHitKey(KEY_INPUT_Z) != 0;
				this->delete_ = CheckHitKey(KEY_INPUT_G) != 0;
				this->select = (GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0;
				this->down_mag = true;
				this->shoot = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
				this->jamp = CheckHitKey(KEY_INPUT_SPACE) != 0;
				//
				GetMousePoint(&x_m, &y_m);
				x_m -= deskx / 2;
				y_m -= desky / 2;
				SetMousePoint(deskx / 2, desky / 2);
				SetMouseDispFlag(FALSE);
			}
			//
			x_m = std::clamp(x_m, -120, 120);
			y_m = std::clamp(y_m, -120, 120);
			operation_2_1(cannotmove, x_m, y_m);
		}
		template<class Y3, class D3>
		void operation_NPC(std::unique_ptr<Y3, D3>& mapparts,std::vector<Chara>&chara, const bool& cannotmove) {
			const auto fps_ = GetFPS();
			//HMD_mat
			int32_t x_m = 0, y_m = 0;
			if (this->HP != 0) {
				bool pp = true;
				bool is_player = false;
				//操作
				this->aim = false;
				this->reload = false;
				this->get_ = false;
				this->sort_ = false;
				this->delete_ = false;
				this->select = false;
				this->down_mag = true;
				this->shoot = false;
				this->jamp = false;
				//AI
				VECTOR_ref vec_2 = chara[0].body.frame(chara[0].frame_s.bodyb_f.first) - this->obj_gun.frame(this->gun_ptr->frame_s.mazzule_f.first);
				if (this->ai_time_shoot < 0.f) {
					vec_2 = chara[0].body.frame(chara[0].frame_s.head_f.first) - this->obj_gun.frame(this->gun_ptr->frame_s.mazzule_f.first);
				}
				for (auto& tgt : chara) {
					if (this == &tgt || tgt.HP == 0) {
						continue;
					}
					auto tmp = tgt.body.frame(tgt.frame_s.bodyb_f.first) - this->obj_gun.frame(this->gun_ptr->frame_s.mazzule_f.first);
					if (this->ai_time_shoot < 0.f) {
						tmp = tgt.body.frame(tgt.frame_s.head_f.first) - this->obj_gun.frame(this->gun_ptr->frame_s.mazzule_f.first);
					}

					bool tt = true;
					{
						VECTOR_ref startpos = this->obj_gun.frame(this->gun_ptr->frame_s.mazzule_f.first);
						VECTOR_ref endpos = tgt.body.frame(tgt.frame_s.bodyb_f.first);
						if (this->ai_time_shoot < 0.f) {
							endpos = tgt.body.frame(tgt.frame_s.head_f.first);
						}
						auto p = mapparts->map_col_line(startpos, endpos);
						if (p.HitFlag) {
							tt = false;
						}
					}
					if (!tt) {
						continue;
					}
					if (vec_2.size() >= tmp.size()) {
						vec_2 = tmp;
						is_player = (&tgt == &chara[0]);
						pp = false;
					}
				}
				{
					VECTOR_ref vec_x = this->body.GetFrameLocalWorldMatrix(this->frame_s.head_f.first).xvec();
					VECTOR_ref vec_y = this->body.GetFrameLocalWorldMatrix(this->frame_s.head_f.first).yvec();
					VECTOR_ref vec_z = this->body.GetFrameLocalWorldMatrix(this->frame_s.head_f.first).zvec()*-1.f;
					auto ai_p_old = this->ai_phase;
					if (pp) {
						this->ai_phase = 0;
					}
					else {
						if (vec_z.dot(vec_2.Norm()) >= 0 && vec_2.size() <= 20.f) {
							this->ai_phase = 1;
						}
					}
					if (this->reloadf) {
						this->ai_phase = 2;
					}
					if ((ai_p_old == 1 && this->ai_phase != 1) || (this->add_vec_real.size() <= this->add_vec.size()*0.8f)) {
						int now = -1;
						auto poss = this->body.GetMatrix().pos();
						auto tmp = VECTOR_ref(VGet(0, 100.f, 0));
						for (auto& w : mapparts->get_waypoint()) {
							auto id = &w - &mapparts->get_waypoint()[0];
							bool tt = true;
							for (auto& ww : this->wayp_pre) {
								if (id == ww) {
									tt = false;
								}
							}
							if (tt) {
								if (tmp.size() >= (w - poss).size()) {
									auto p = mapparts->map_col_line(w + VGet(0, 0.5f, 0), poss + VGet(0, 0.5f, 0));
									if (!p.HitFlag) {
										tmp = (w - poss);
										now = int(id);
									}
								}
							}
						}
						if (now != -1) {
							for (int i = int(this->wayp_pre.size()) - 1; i >= 1; i--) {
								this->wayp_pre[i] = this->wayp_pre[i - 1];
							}
							this->wayp_pre[0] = now;
						}
					}

					switch (this->ai_phase) {
					case 0://通常フェイズ
					{
						this->running = false;
						this->ai_reload = false;
						this->wkey = true;
						this->skey = false;
						this->akey = false;
						this->dkey = false;
						this->squat.first = false;
						this->qkey = false;
						this->ekey = false;
						this->ai_time_shoot = 0.f;

						vec_2 = mapparts->get_waypoint()[this->wayp_pre[0]] - this->body.GetMatrix().pos();
						x_m = -int(vec_x.dot(vec_2) * 120);
						y_m = -int(vec_y.dot(vec_2) * 120);

						//到達時に判断
						if (vec_2.size() <= 0.5f) {
							int now = -1;
							auto poss = this->body.GetMatrix().pos();
							auto tmp = VECTOR_ref(VGet(0, 100.f, 0));
							for (auto& w : mapparts->get_waypoint()) {
								auto id = &w - &mapparts->get_waypoint()[0];
								bool tt = true;
								for (auto& ww : this->wayp_pre) {
									if (id == ww) {
										tt = false;
									}
								}
								if (tt) {
									if (tmp.size() >= (w - poss).size()) {
										auto p = mapparts->map_col_line(w + VGet(0, 0.5f, 0), poss + VGet(0, 0.5f, 0));
										if (!p.HitFlag) {
											tmp = (w - poss);
											now = int(id);
										}
									}
								}
							}
							if (now != -1) {
								for (int i = int(this->wayp_pre.size()) - 1; i >= 1; i--) {
									this->wayp_pre[i] = this->wayp_pre[i - 1];
								}
								this->wayp_pre[0] = now;
							}
						}
					}
					break;
					case 1://戦闘フェイズ
					{
						this->running = false;
						this->ai_reload = false;
						this->wkey = false;
						this->skey = false;
						if (this->ekey) {
							this->ai_time_e += 1.f / fps_;
							if (this->ai_time_e >= 2) {
								this->ekey = false;
								this->ai_time_e = 0.f;
							}
						}
						auto poss = this->body.GetMatrix().pos();

						{
							for (auto& w : mapparts->get_leanpoint_e()) {
								if ((w - poss).size() <= 0.5f) {
									this->ekey = true;
									this->qkey = false;
									this->ai_time_e = 0.f;
									break;
								}
							}
						}
						if (this->qkey) {
							this->ai_time_q += 1.f / fps_;
							if (this->ai_time_q >= 2) {
								this->qkey = false;
								this->ai_time_q = 0.f;
							}
						}
						{
							for (auto& w : mapparts->get_leanpoint_q()) {
								if ((w - poss).size() <= 0.5f) {
									this->ekey = false;
									this->qkey = true;
									this->ai_time_q = 0.f;
									break;
								}
							}
						}

						int range = int(500.f + 500.f*vec_2.size() / 20.f);
						x_m = -int(vec_x.dot(vec_2) * 25) + int(float(-range + GetRand(range * 2)) / 100.f);
						y_m = -int(vec_y.dot(vec_2) * 25) + int(float(GetRand(range * 2)) / 100.f);

						this->ai_time_shoot += 1.f / fps_;
						if (this->ai_time_shoot < 0.f) {
							this->akey = false;
							this->dkey = false;
							this->wkey = false;
							this->skey = false;

							if (this->ai_time_shoot >= -5) {
								if (is_player) {
									this->shoot = GetRand(100) <= 5;
								}
								else {
									this->shoot = GetRand(100) <= 20;
								}
								this->aim = true;
								this->squat.first = true;
							}
						}
						else {
							this->squat.first = false;
							if (is_player) {
								this->shoot = GetRand(100) <= 10;
							}
							else {
								this->shoot = GetRand(100) <= 30;
							}

							if (this->ai_time_shoot >= GetRand(20) + 5) {
								this->ai_time_shoot = float(-8);
							}
							if (this->ekey && !this->akey) {
								this->akey = true;
								this->dkey = false;
								this->ai_time_d = 0.f;
								this->ai_time_a = 0.f;
							}
							if (this->qkey && !this->dkey) {
								this->dkey = true;
								this->akey = false;
								this->ai_time_d = 0.f;
								this->ai_time_a = 0.f;
							}

							if (!this->akey) {
								this->ai_time_d += 1.f / fps_;
								if (this->ai_time_d > GetRand(3) + 1) {
									this->akey = true;
									this->ai_time_d = 0.f;
								}
							}
							else {
								this->ai_time_a += 1.f / fps_;
								if (this->ai_time_a > GetRand(3) + 1) {
									this->akey = false;
									this->ai_time_a = 0.f;
								}
							}
							this->dkey = !this->akey;
						}
						if (this->gun_stat[this->gun_ptr->id].ammo_cnt == 0 && !this->reloadf) {
							this->reload = true;
							this->ai_reload = true;
						}
					}
					break;
					case 2://リロードフェイズ
					{
						this->wkey = true;
						this->skey = false;
						this->akey = false;
						this->dkey = false;
						this->running = true;
						this->squat.first = false;
						this->qkey = false;
						this->ekey = false;

						if (this->ai_reload) {
							auto ppp = this->wayp_pre[1];
							for (int i = int(this->wayp_pre.size()) - 1; i >= 1; i--) {
								this->wayp_pre[i] = this->wayp_pre[0];
							}
							this->wayp_pre[0] = ppp;
							this->ai_reload = false;
						}
						auto poss = this->body.GetMatrix().pos();
						vec_2 = mapparts->get_waypoint()[this->wayp_pre[0]] - poss;
						x_m = -int(vec_x.dot(vec_2) * 40);
						y_m = -int(vec_y.dot(vec_2) * 40);

						//到達時に判断
						if (vec_2.size() <= 0.5f) {
							int now = -1;
							auto tmp = VECTOR_ref(VGet(0, 100.f, 0));
							for (auto& w : mapparts->get_waypoint()) {
								auto id = &w - &mapparts->get_waypoint()[0];
								bool tt = true;
								for (auto& ww : this->wayp_pre) {
									if (id == ww) {
										tt = false;
									}
								}
								if (tt) {
									if (tmp.size() >= (w - poss).size()) {
										auto p = mapparts->map_col_line(w + VGet(0, 0.5f, 0), poss + VGet(0, 0.5f, 0));
										if (!p.HitFlag) {
											tmp = (w - poss);
											now = int(id);
										}
									}
								}
							}
							if (now != -1) {
								for (int i = int(this->wayp_pre.size()) - 1; i >= 1; i--) {
									this->wayp_pre[i] = this->wayp_pre[i - 1];
								}
								this->wayp_pre[0] = now;
							}
						}
					}
					break;
					default:
						break;
					}
				}
			}
			x_m = std::clamp(x_m, -120, 120);
			y_m = std::clamp(y_m, -120, 120);
			operation_2_1(cannotmove, x_m, y_m);
		}
		void operation_2() {
			//ジャンプ
			{
				if (this->add_ypos == 0.f) {
					if (this->jamp && this->HP != 0) {
						this->add_ypos = 0.06f*FRAME_RATE / GetFPS();
					}
					this->add_vec = this->add_vec_buf;
				}
				else {
					easing_set(&this->add_vec, VGet(0, 0, 0), 0.995f);
				}
			}
			//操作
			{
				//引き金(左クリック)
				easing_set(&this->obj_gun.get_anime(2).per, float(this->shoot && !this->running), 0.5f);
				//ADS
				this->ads.first = this->aim && (!this->reloadf);
				//セレクター(中ボタン)
				this->selkey.get_in(this->select);
				//マグキャッチ(Rキー)
				easing_set(&this->obj_gun.get_anime(5).per, float(this->reload), 0.5f);
				//銃取得
				this->getmag.get_in(this->get_);
				//
				this->sortmag.get_in(this->sort_);
				//
				this->delete_item.get_in(this->delete_);
			}
		}
	};
	//アイテム
	class Items {
	private:
	public:
		//共通
		size_t id = 0;
		VECTOR_ref pos, add;
		MATRIX_ref mat;
		MV1 obj;
		//銃専用パラメーター
		Guns* ptr_gun = nullptr;
		Guns gun;
		//マガジン専用パラメーター
		Mags* ptr_mag = nullptr;
		Mags magazine;
		float del_timer = 0.f;
		//治療キット専用パラメーター
		Meds* ptr_med = nullptr;
		Meds medkit;
		//mag
		void Set_item(Mags*magdata, const VECTOR_ref& pos_, const MATRIX_ref& mat_) {
			this->pos = pos_;
			this->add.clear();
			this->mat = mat_;
			//
			this->ptr_mag = magdata;
			this->obj = this->ptr_mag->mod.model.Duplicate();
		}
		//item
		void Set_item(Meds*meddata, const VECTOR_ref& pos_, const MATRIX_ref& mat_) {
			this->pos = pos_;
			this->add.clear();
			this->mat = mat_;
			//
			this->ptr_med = meddata;
			this->obj = this->ptr_med->mod.model.Duplicate();
		}
		template<class Y, class D>
		void update(std::vector<Items>& item, std::unique_ptr<Y, D>& mapparts) {
			const auto fps_ = GetFPS();
			auto old = this->pos;

			old = this->pos;
			if (this->ptr_mag != nullptr || this->ptr_med != nullptr) {
				this->obj.SetMatrix(this->mat*MATRIX_ref::Mtrans(this->pos));
				this->pos += this->add;
				this->add.yadd(M_GR / powf(fps_, 2.f));
				for (auto& p : item) {
					if ((p.ptr_mag != nullptr || p.ptr_med != nullptr) && &p != &*this) {
						if ((p.pos - this->pos).size() <= 0.35f) {
							p.add.xadd((p.pos - this->pos).x()*5.f / fps_);
							p.add.zadd((p.pos - this->pos).z()*5.f / fps_);
							this->add.xadd((this->pos - p.pos).x()*5.f / fps_);
							this->add.zadd((this->pos - p.pos).z()*5.f / fps_);
						}
					}
				}
				auto pp = mapparts->map_col_line(old - VGet(0, 0.0025f, 0), this->pos - VGet(0, 0.0025f, 0));
				if (pp.HitFlag) {
					this->pos = VECTOR_ref(pp.HitPosition) + VECTOR_ref(pp.Normal)*0.005f;
					this->mat *= MATRIX_ref::RotVec2(this->mat.xvec(), VECTOR_ref(pp.Normal)*-1.f);
					this->add.clear();
					//easing_set(&this->add, VGet(0, 0, 0), 0.8f);
				}
				//
			}
		}
		template<class Y2, class D2>
		void Get_item_2( Chara& chara, std::unique_ptr<Y2, D2>& mapparts) {
			if (this->ptr_mag != nullptr) {
				VECTOR_ref startpos = chara.pos_LEFTHAND;
				VECTOR_ref endpos = startpos - chara.mat_LEFTHAND.zvec()*2.6f;
				auto p = mapparts->map_col_line(startpos, endpos);
				if (p.HitFlag) {
					endpos = p.HitPosition;
				}
				bool zz=false;
				{
					zz = (Segment_Point_MinLength(startpos.get(), endpos.get(), this->pos.get()) <= 0.2f);
					chara.canget_magitem |= zz;
					if (zz) {
						chara.canget_id = this->id;
						chara.canget_mag = this->ptr_mag->mod.name;
						if (chara.getmag.second == 1 && this->magazine.cap != 0) {
							chara.sort_f = false;
							chara.gun_stat[this->ptr_mag->id].mag_plus(&(this->magazine));
							if (chara.gun_stat[this->ptr_mag->id].mag_in.size() == 1) {
								chara.reloadf = true;
							}
							this->Delete_item();
						}
					}
				}
			}

			if (this->ptr_med != nullptr) {
				VECTOR_ref startpos = chara.pos_LEFTHAND;
				VECTOR_ref endpos = startpos - chara.mat_LEFTHAND.zvec()*2.6f;
				auto p = mapparts->map_col_line(startpos, endpos);
				if (p.HitFlag) {
					endpos = p.HitPosition;
				}
				bool zz = false;
				{
					zz = (Segment_Point_MinLength(startpos.get(), endpos.get(), this->pos.get()) <= 0.2f);
					chara.canget_meditem |= zz;
					if (zz) {
						chara.canget_id = this->id;
						chara.canget_med = this->ptr_med->mod.name;
						if (chara.getmag.second == 1) {
							chara.HP = std::clamp<int>(chara.HP + this->ptr_med->repair, 0, chara.HP_full);
							//ITEM
							this->Delete_item();
						}
					}
				}
			}
			//個別
			if (this->ptr_mag != nullptr && this->magazine.cap == 0) {
				this->del_timer += 1.f / GetFPS();
			}
		}
		void Draw_item() {
			if (this->ptr_gun != nullptr) {
				this->obj.DrawModel();
			}
			if (this->ptr_mag != nullptr) {
				this->obj.DrawModel();
			}
			if (this->ptr_med != nullptr) {
				this->obj.DrawModel();
			}
		}
		void Draw_item(Chara& chara) {
			if (this->ptr_gun != nullptr) {
				if (chara.canget_gunitem && chara.canget_id == this->id) {
					DrawLine3D(this->pos.get(), (this->pos + VGet(0, 0.1f, 0)).get(), GetColor(255, 0, 0));
					auto c = MV1GetDifColorScale(this->obj.get());
					MV1SetDifColorScale(this->obj.get(), GetColorF(0.f, 1.f, 0.f, 1.f));
					this->obj.DrawModel();
					MV1SetDifColorScale(this->obj.get(), c);
				}
				else {
					this->obj.DrawModel();
				}
			}
			if (this->ptr_mag != nullptr) {
				if (chara.canget_magitem && chara.canget_id == this->id) {
					DrawLine3D(this->pos.get(), (this->pos + VGet(0, 0.1f, 0)).get(), GetColor(255, 0, 0));
					auto c = MV1GetDifColorScale(this->obj.get());
					MV1SetDifColorScale(this->obj.get(), GetColorF(0.f, 1.f, 0.f, 1.f));
					this->obj.DrawModel();
					MV1SetDifColorScale(this->obj.get(), c);
				}
				else {
					this->obj.DrawModel();
				}
			}
			if (this->ptr_med != nullptr) {
				if (chara.canget_meditem && chara.canget_id == this->id) {
					DrawLine3D(this->pos.get(), (this->pos + VGet(0, 0.1f, 0)).get(), GetColor(255, 0, 0));
					auto c = MV1GetDifColorScale(this->obj.get());
					MV1SetDifColorScale(this->obj.get(), GetColorF(0.f, 1.f, 0.f, 1.f));
					this->obj.DrawModel();
					MV1SetDifColorScale(this->obj.get(), c);
				}
				else {
					this->obj.DrawModel();
				}
			}
		}
		void Delete_item() {
			this->ptr_gun = nullptr;
			this->ptr_mag = nullptr;
			this->ptr_med = nullptr;
			this->obj.Dispose();
		}
	};
	//ルール
	class rule {
	private:
		float ready = 0.f;
		float timer = 0.f;
	public:
		float gettimer() {
			return timer;
		}

		float getready() {
			return ready;
		}

		bool getstart() {
			return ready <= 0.f;
		}
		bool getend() {
			return timer <= 0.f;
		}

		void set() {
			ready = 0.1f;
			timer = 180.f;
		}

		void update() {
			if (getstart()) {
				timer -= 1.f / GetFPS();
			}
			else {
				ready -= 1.f / GetFPS();
			}
			if (getend()) {
				timer = 0.f;
			}
		}

	};
};
//