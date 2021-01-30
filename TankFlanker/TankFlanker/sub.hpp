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
public:
	//player
	class Chara {
	private:
	public:
		/*モデル*/
		MV1 body;
		/*プレイヤー操作*/
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
		bool wkey = false;
		bool skey = false;
		bool akey = false;
		bool dkey = false;
		bool running = false;										//走るか否か
		bool jamp = false;
		switchs shot;
		switchs shot_R;
		//頭部座標系
		VECTOR_ref pos_HMD, pos_HMD_old, rec_HMD;
		float add_ypos = 0.f;//垂直加速度
		float body_xrad = 0.f;//胴体角度
		float body_yrad = 0.f;//胴体角度
		//
		void set() {
			//身体
			//body_.DuplicateonAnime(&this->body);
		}
		void spawn(const VECTOR_ref& pos_, const MATRIX_ref& mat_H) {
			this->xrad_p = 0;

			this->pos = this->spawn_pos;
			this->mat = this->spawn_mat;
			this->running = false;
			this->add_ypos = 0.f;
			this->add_vec_buf.clear();

			this->body_xrad = 0.f;//胴体角度
			this->body_yrad = 0.f;//胴体角度
		}

		void start() {
		}

		void Draw_chara() {
			this->body.DrawModel();
			//this->col.DrawModel();
		}

		void Delete_chara() {
			this->body.Dispose();
		}

		void operation_2_1(const bool& cannotmove, int32_t x_m, int32_t y_m) {
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
			{
				//
				this->mat = MATRIX_ref::RotX(-this->xrad_p)*this->mat;
				this->xrad_p = std::clamp(this->xrad_p - deg2rad(y_m)*0.1f, deg2rad(-80), deg2rad(60));
				this->mat *= MATRIX_ref::RotY(deg2rad(x_m)*0.1f);//y軸回転
				this->mat = MATRIX_ref::RotX(this->xrad_p)*this->mat;//x軸回転
			}

			if (cannotmove) {
				this->wkey = false;
				this->skey = false;
				this->akey = false;
				this->dkey = false;
				this->running = false;
				this->jamp = false;
			}

			this->speed = (this->running ? 6.f : 4.f) / GetFPS();
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
				this->jamp = false;
				{
					if (Drawparts->get_hand1_num() != -1) {
						auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hand1_num()];
						if (ptr_.turn && ptr_.now) {
						}
					}
					if (Drawparts->get_hand2_num() != -1) {
						auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hand2_num()];
						if (ptr_.turn && ptr_.now) {
							//running
							this->running = (ptr_.on[0] & BUTTON_TOUCHPAD) != 0;
							//jamp
							this->jamp = (ptr_.on[0] & BUTTON_SIDE) != 0;
							//移動
							if ((ptr_.on[1] & BUTTON_TOUCHPAD) != 0) {
								this->speed = (this->running ? 6.f : 4.f) / GetFPS();

								easing_set(&this->add_vec_buf, (this->mat.zvec()*ptr_.touch.y() + this->mat.xvec()*ptr_.touch.x())*this->speed, 0.95f);
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
				if ((ptr_.turn && ptr_.now) != *oldv_1_1) {
					this->rec_HMD = VGet(this->pos_HMD.x(), 0.f, this->pos_HMD.z());
				}
				*oldv_1_1 = ptr_.turn && ptr_.now;
			}
		}
		void operation(const bool& cannotmove) {
			//HMD_mat
			int32_t x_m = 0, y_m = 0;
			{
				//操作
				this->wkey = (CheckHitKey(KEY_INPUT_W) != 0);
				this->skey = (CheckHitKey(KEY_INPUT_S) != 0);
				this->akey = (CheckHitKey(KEY_INPUT_A) != 0);
				this->dkey = (CheckHitKey(KEY_INPUT_D) != 0);
				this->running = (CheckHitKey(KEY_INPUT_LSHIFT) != 0);
				this->jamp = CheckHitKey(KEY_INPUT_SPACE) != 0;
				this->shot.get_in((GetMouseInput() & MOUSE_INPUT_LEFT) != 0);
				this->shot_R.get_in((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0);
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
			{
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
			}
			x_m = std::clamp(x_m, -120, 120);
			y_m = std::clamp(y_m, -120, 120);
			operation_2_1(cannotmove, x_m, y_m);
		}
		void operation_2() {
			//ジャンプ
			{
				if (this->add_ypos == 0.f) {
					if (this->jamp) {
						this->add_ypos = 0.06f*FRAME_RATE / GetFPS();
					}
					this->add_vec = this->add_vec_buf;
				}
				else {
					easing_set(&this->add_vec, VGet(0, 0, 0), 0.995f);
				}
			}
		}
	};
};
//