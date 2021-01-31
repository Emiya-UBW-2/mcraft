#pragma once

#include <random>

// プレイヤー関係の定義
#define PLAYER_ENUM_DEFAULT_SIZE	1.8f	// 周囲のポリゴン検出に使用する球の初期サイズ
#define PLAYER_HIT_WIDTH			0.15f	// 当たり判定カプセルの半径
#define PLAYER_HIT_HEIGHT			1.8f	// 当たり判定カプセルの高さ
#define PLAYER_HIT_TRYNUM			16		// 壁押し出し処理の最大試行回数
#define PLAYER_HIT_SLIDE_LENGTH		0.015f	// 一度の壁押し出し処理でスライドさせる距離

class PerlinNoise {
private:
	//メンバ変数
	std::array<uint_fast8_t, 512> p{ 0 };

	constexpr float getFade(const float t_) const noexcept {
		return t_ * t_ * t_ * (t_ * (t_ * 6 - 15) + 10);
	}
	constexpr float getLerp(const float t_, const float a_, const float b_) const noexcept {
		return a_ + t_ * (b_ - a_);
	}
	constexpr float makeGrad(const uint_fast8_t hash_, const float u_, const float v_) const noexcept {
		return (((hash_ & 1) == 0) ? u_ : -u_) + (((hash_ & 2) == 0) ? v_ : -v_);
	}
	constexpr float makeGrad(const uint_fast8_t hash_, const float x_, const float y_, const float z_) const noexcept {
		return this->makeGrad(hash_, hash_ < 8 ? x_ : y_, hash_ < 4 ? y_ : hash_ == 12 || hash_ == 14 ? x_ : z_);
	}
	constexpr float getGrad(const uint_fast8_t hash_, const float x_, const float y_, const float z_) const noexcept {
		return this->makeGrad(hash_ & 15, x_, y_, z_);
	}

	float setNoise(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
		const std::size_t x_int{ static_cast<std::size_t>(static_cast<std::size_t>(std::floor(x_)) & 255) };
		const std::size_t y_int{ static_cast<std::size_t>(static_cast<std::size_t>(std::floor(y_)) & 255) };
		const std::size_t z_int{ static_cast<std::size_t>(static_cast<std::size_t>(std::floor(z_)) & 255) };
		x_ -= std::floor(x_);
		y_ -= std::floor(y_);
		z_ -= std::floor(z_);
		const float u{ this->getFade(x_) };
		const float v{ this->getFade(y_) };
		const float w{ this->getFade(z_) };
		const std::size_t a0{ static_cast<std::size_t>(this->p[x_int] + y_int) };
		const std::size_t a1{ static_cast<std::size_t>(this->p[a0] + z_int) };
		const std::size_t a2{ static_cast<std::size_t>(this->p[a0 + 1] + z_int) };
		const std::size_t b0{ static_cast<std::size_t>(this->p[x_int + 1] + y_int) };
		const std::size_t b1{ static_cast<std::size_t>(this->p[b0] + z_int) };
		const std::size_t b2{ static_cast<std::size_t>(this->p[b0 + 1] + z_int) };

		return this->getLerp(w,
			this->getLerp(v,
				this->getLerp(u, this->getGrad(this->p[a1], x_, y_, z_), this->getGrad(this->p[b1], x_ - 1, y_, z_)),
				this->getLerp(u, this->getGrad(this->p[a2], x_, y_ - 1, z_), this->getGrad(this->p[b2], x_ - 1, y_ - 1, z_))),
			this->getLerp(v,
				this->getLerp(u, this->getGrad(this->p[a1 + 1], x_, y_, z_ - 1), this->getGrad(this->p[b1 + 1], x_ - 1, y_, z_ - 1)),
				this->getLerp(u, this->getGrad(this->p[a2 + 1], x_, y_ - 1, z_ - 1), this->getGrad(this->p[b2 + 1], x_ - 1, y_ - 1, z_ - 1))));
	}
	float setOctaveNoise(const std::size_t octaves_, float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
		float noise_value = 0.f;
		float amp{ 1.0 };
		for (std::size_t i = 0; i < octaves_; ++i) {
			noise_value += this->setNoise(x_, y_, z_) * amp;
			x_ *= 2.0;
			y_ *= 2.0;
			z_ *= 2.0;
			amp *= 0.5;
		}
		return noise_value;
	}
public:
	constexpr PerlinNoise() = default;
	explicit PerlinNoise(const std::uint_fast32_t seed_) {
		this->setSeed(seed_);
	}

	//SEED値を設定する
	void setSeed(const std::uint_fast32_t seed_) {
		for (std::size_t i = 0; i < 256; ++i)
			this->p[i] = static_cast<uint_fast8_t>(i);
		std::shuffle(this->p.begin(), this->p.begin() + 256, std::default_random_engine(seed_));
		for (std::size_t i = 0; i < 256; ++i) {
			this->p[256 + i] = this->p[i];
		}
	}
	//オクターブ無しノイズを取得する
	float noise(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
		return this->setNoise(x_, y_, z_) * 0.5f + 0.5f;
	}
	//オクターブ有りノイズを取得する
	float octaveNoise(const std::size_t octaves_, float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) const noexcept {
		return this->setOctaveNoise(octaves_, x_, y_, z_) * 0.5 + 0.5;
	}
};
class Mapclass :Mainclass {
public:
	float cube_size_x = 1.f;
	float cube_size_y = 0.25f;
	float cube_size_z = 1.f;
	int size_x = 100;
	int size_y = 100;
	int size_z = 100;

	GraphHandle noise_;
private:
	class block_mod {
	private:
	public:
		MV1 model;
		MV1 model_1_2;
		MV1 model_1_6;
		MV1 model_5_6;

		void set() {
			MV1::Load("data/block/b_1/model.mv1", &model, false);
			MV1::Load("data/block/b_1_2/model.mv1", &model_1_2, false);
			MV1::Load("data/block/b_1_3/model.mv1", &model_1_6, false);
			MV1::Load("data/block/b_1_4/model.mv1", &model_5_6, false);
		}
	};

	class block_obj {
	private:
	public:
		block_mod* ptr = nullptr;
		block_mod obj;
		size_t id = 0;
		int x = 0;
		int y = 0;
		int z = 0;
		std::array<block_obj*, 6> near_{ nullptr };
	};

	MV1 sky;			//空
public:
	std::vector<block_mod> mods;
	std::vector<block_obj> objs;
	std::vector<block_obj*> objs_canlook;

	Mapclass() {
		noise_ = GraphHandle::Make(size_x, size_z, false);
		PerlinNoise ns(10000);

		mods.resize(mods.size() + 1);
		mods.back().set();

		for (int x = -size_x / 2; x <= size_x / 2; x++) {
			for (int y = -size_y / 2; y <= size_y / 2; y++) {
				for (int z = -size_z / 2; z <= size_z / 2; z++) {
					objs.resize(objs.size() + 1);
					objs.back().ptr = nullptr;
					objs.back().x = x;
					objs.back().y = y;
					objs.back().z = z;
				}
			}
		}

		for (auto& m : objs) {
			set_block(m);
		}

		{
			noise_.SetDraw_Screen(true);
			int siz = 4;
			for (int z1 = 0; z1 < size_z * siz; z1++) {
				for (int x1 = 0; x1 < size_x * siz; x1++) {
					auto y = int(ns.octaveNoise(2,float(x1) / size_x, float(z1) / size_z) * 255);
					DrawPixel(x1 / siz, z1 / siz, GetColor(y, y, y));
					if (x1%siz == 0 && z1%siz == 0) {
						put_block_begin(x1 / siz - size_x / 2, -20 + 20 * y / 255, z1 / siz - size_z / 2, &mods.back());
					}
				}
			}
			/*
			for (int z1 = 0; z1 < size_z; z1++) {
				for (int x1 = 0; x1 < size_x; x1++) {
						put_block(x1 - size_x / 2, 0, z1 - size_z / 2, &mods.back());
				}
			}
			//*/
		}


	}
	~Mapclass() {
	}

	void set_block(block_obj& m) {
		for (auto& n : m.near_) {
			n = nullptr;
		}

		{
			auto id = (m.x - 1 + size_x / 2)*(size_y + 1) * (size_z + 1) + (m.y + size_y / 2) * (size_z + 1) + (m.z + size_z / 2);
			if (id >= 0 && id <= objs.size() - 1 && abs(m.x) != size_x / 2) {
				m.near_[0] = &objs[id];
			}
		}
		{
			auto id = (m.x + 1 + size_x / 2)*(size_y + 1) * (size_z + 1) + (m.y + size_y / 2) * (size_z + 1) + (m.z + size_z / 2);
			if (id >= 0 && id <= objs.size() - 1 && abs(m.x) != size_x / 2) {
				m.near_[1] = &objs[id];
			}
		}
		{
			auto id = (m.x + size_x / 2)*(size_y + 1) * (size_z + 1) + (m.y - 1 + size_y / 2) * (size_z + 1) + (m.z + size_z / 2);
			if (id >= 0 && id <= objs.size() - 1 && abs(m.y) != size_y / 2) {
				m.near_[2] = &objs[id];
			}
		}
		{
			auto id = (m.x + size_x / 2)*(size_y + 1) * (size_z + 1) + (m.y + 1 + size_y / 2) * (size_z + 1) + (m.z + size_z / 2);
			if (id >= 0 && id <= objs.size() - 1 && abs(m.y) != size_y / 2) {
				m.near_[3] = &objs[id];
			}
		}
		{
			auto id = (m.x + size_x / 2)*(size_y + 1) * (size_z + 1) + (m.y + size_y / 2) * (size_z + 1) + (m.z - 1 + size_z / 2);
			if (id >= 0 && id <= objs.size() - 1 && abs(m.z) != size_z / 2) {
				m.near_[4] = &objs[id];
			}
		}
		{
			auto id = (m.x + size_x / 2)*(size_y + 1) * (size_z + 1) + (m.y + size_y / 2) * (size_z + 1) + (m.z + 1 + size_z / 2);
			if (id >= 0 && id <= objs.size() - 1 && abs(m.z) != size_z / 2) {
				m.near_[5] = &objs[id];
			}
		}
	}
	void push_look(block_obj& m) {
		m.obj.model.Dispose();
		if (m.near_[0] != nullptr && m.near_[1] != nullptr && m.near_[4] != nullptr && m.near_[5] != nullptr) {//x-1
			if (m.near_[0]->ptr == nullptr && m.near_[1]->ptr != nullptr && m.near_[4]->ptr != nullptr && m.near_[5]->ptr != nullptr) {//x-1
				m.id = 0x11;
			}
			else if (m.near_[0]->ptr != nullptr&&m.near_[1]->ptr == nullptr &&  m.near_[4]->ptr != nullptr && m.near_[5]->ptr != nullptr) {//x+1
				m.id = 0x12;
			}
			else if (m.near_[0]->ptr != nullptr && m.near_[1]->ptr != nullptr &&m.near_[4]->ptr == nullptr &&  m.near_[5]->ptr != nullptr) {//z-1
				m.id = 0x14;
			}
			else if (m.near_[0]->ptr != nullptr && m.near_[1]->ptr != nullptr && m.near_[4]->ptr != nullptr&&m.near_[5]->ptr == nullptr) {//z+1
				m.id = 0x18;
			}
			else if (m.near_[0]->ptr == nullptr && m.near_[1]->ptr != nullptr && m.near_[4]->ptr == nullptr && m.near_[5]->ptr == nullptr) {//x-1
				m.id = 0x21;
			}
			else if (m.near_[0]->ptr != nullptr&&m.near_[1]->ptr == nullptr &&  m.near_[4]->ptr == nullptr && m.near_[5]->ptr == nullptr) {//x+1
				m.id = 0x22;
			}
			else if (m.near_[0]->ptr == nullptr && m.near_[1]->ptr == nullptr &&m.near_[4]->ptr == nullptr &&  m.near_[5]->ptr != nullptr) {//z-1
				m.id = 0x24;
			}
			else if (m.near_[0]->ptr == nullptr && m.near_[1]->ptr == nullptr && m.near_[4]->ptr != nullptr&&m.near_[5]->ptr == nullptr) {//z+1
				m.id = 0x28;
			}
			else if (m.near_[0]->ptr == nullptr && m.near_[1]->ptr != nullptr && m.near_[4]->ptr != nullptr&&m.near_[5]->ptr == nullptr) {//z+1
				m.id = 0x41;
			}
			else if (m.near_[0]->ptr != nullptr && m.near_[1]->ptr == nullptr &&m.near_[4]->ptr == nullptr &&  m.near_[5]->ptr != nullptr) {//z-1
				m.id = 0x42;
			}
			else if (m.near_[0]->ptr == nullptr && m.near_[1]->ptr != nullptr && m.near_[4]->ptr == nullptr && m.near_[5]->ptr != nullptr) {//x-1
				m.id = 0x44;
			}
			else if (m.near_[0]->ptr != nullptr&&m.near_[1]->ptr == nullptr &&  m.near_[4]->ptr != nullptr && m.near_[5]->ptr == nullptr) {//x+1
				m.id = 0x48;
			}
			else {
				if (
					(m.near_[0]->id == 0x14 || m.near_[0]->id == 0x24 || m.near_[0]->id == 0x41)
					&&
					(m.near_[1]->id == 0x00 || m.near_[1]->id == 0x12 || m.near_[1]->id == 0x22 || m.near_[1]->id == 0x82 || m.near_[1]->id == 0x84)
					&&
					(m.near_[4]->id == 0x00 || m.near_[4]->id == 0x18 || m.near_[4]->id == 0x28 || m.near_[4]->id == 0x82 || m.near_[4]->id == 0x88)
					&&
					(m.near_[5]->id == 0x11 || m.near_[5]->id == 0x21 || m.near_[5]->id == 0x41)
					) {//z+1
					m.id = 0x81;
				}
				else if (
					(m.near_[0]->id == 0x00 || m.near_[0]->id == 0x11 || m.near_[0]->id == 0x21 || m.near_[0]->id == 0x81 || m.near_[0]->id == 0x88) 
					&&
					(m.near_[1]->id == 0x18 || m.near_[1]->id == 0x28 || m.near_[1]->id == 0x42)
					&&
					(m.near_[4]->id == 0x12 || m.near_[4]->id == 0x22 || m.near_[4]->id == 0x42)
					&&
					(m.near_[5]->id == 0x00 || m.near_[5]->id == 0x18 || m.near_[5]->id == 0x24 || m.near_[5]->id == 0x81 || m.near_[5]->id == 0x84)
					) {//x+1
					m.id = 0x82;
				}
				else if (
					(m.near_[0]->id == 0x00 || m.near_[0]->id == 0x11 || m.near_[0]->id == 0x21 || m.near_[0]->id == 0x81 || m.near_[0]->id == 0x88)
					&&
					(m.near_[1]->id == 0x14 || m.near_[1]->id == 0x24 || m.near_[1]->id == 0x48)
					&&
					(m.near_[4]->id == 0x00 || m.near_[4]->id == 0x18 || m.near_[4]->id == 0x28 || m.near_[4]->id == 0x82 || m.near_[4]->id == 0x88)
					&&
					(m.near_[5]->id == 0x12 || m.near_[5]->id == 0x22 || m.near_[5]->id == 0x48)
					) {//z-1
					m.id = 0x88;
				}
				else if (
					(m.near_[0]->id == 0x18 || m.near_[0]->id == 0x28 || m.near_[0]->id == 0x44)
					&&
					(m.near_[1]->id == 0x00 || m.near_[1]->id == 0x12 || m.near_[1]->id == 0x22 || m.near_[1]->id == 0x82 || m.near_[1]->id == 0x84)
					&&
					(m.near_[4]->id == 0x11 || m.near_[4]->id == 0x21 || m.near_[4]->id == 0x44)
					&&
					(m.near_[5]->id == 0x00 || m.near_[5]->id == 0x18 || m.near_[5]->id == 0x24 || m.near_[5]->id == 0x81 || m.near_[5]->id == 0x84)
					) {//x-1
					m.id = 0x84;
				}
				else {
					m.obj.model = m.ptr->model.Duplicate();
					m.id = 0x00;
				}
			}
			//
			if (m.id & 0x10) {
				m.obj.model = m.ptr->model_1_2.Duplicate();
			}
			if (m.id & 0x20) {
				m.obj.model = m.ptr->model_1_2.Duplicate();
			}
			if (m.id & 0x40) {
				m.obj.model = m.ptr->model_1_6.Duplicate();
			}
			if (m.id & 0x80) {
				m.obj.model = m.ptr->model_5_6.Duplicate();
			}
			//
			if (m.id & 0x01) {
				m.obj.model.SetRotationZYAxis(VGet(0.f, 0, -1.f), VGet(0, 1.f, 0), 0.f);//→
			}
			if (m.id & 0x02) {
				m.obj.model.SetRotationZYAxis(VGet(0.f, 0, 1.f), VGet(0, 1.f, 0), 0.f);//←
			}
			if (m.id & 0x04) {
				m.obj.model.SetRotationZYAxis(VGet(1.f, 0, 0.f), VGet(0, 1.f, 0), 0.f);//↓
			}
			if (m.id & 0x08) {
				m.obj.model.SetRotationZYAxis(VGet(-1.f, 0, 0.f), VGet(0, 1.f, 0), 0.f);//↑
			}
			//
		}
		else {
			m.obj.model = m.ptr->model.Duplicate();
			m.id = 0x00;
		}
		m.obj.model.SetPosition(VGet(float(m.x)*cube_size_x, float(m.y)*cube_size_y, float(m.z)*cube_size_z));
	}
	void push_canlook(block_obj& m) {
		if (m.ptr != nullptr) {
			push_look(m);
			for (auto& n : m.near_) {
				if (n != nullptr) {
					if (n->ptr != nullptr) {
						push_look(*n);
					}
				}
			}
			/*
			int cnt = 0;
			for (auto& n : m.near_) {
				if (n != nullptr) {
					cnt++;
				}
			}
			*/
			objs_canlook.emplace_back(&m);
		}

	}

	void push_canlook_begin(block_obj& m) {
		if (m.ptr != nullptr) {
			m.obj.model = m.ptr->model.Duplicate();
			m.obj.model.SetPosition(VGet(float(m.x)*cube_size_x, float(m.y)*cube_size_y, float(m.z)*cube_size_z));
			/*
			int cnt = 0;
			for (auto& n : m.near_) {
				if (n != nullptr) {
					cnt++;
				}
			}
			*/
			objs_canlook.emplace_back(&m);
		}

	}

	void put_block(int x, int y, int z, block_mod* mod) {
		if (abs(x) > size_x / 2 || abs(y) > size_y / 2 || abs(z) > size_z / 2) {
			return;
		}
		auto id = (x + size_x / 2)*(size_y + 1) * (size_z + 1) + (y + size_y / 2) * (size_z + 1) + (z + size_z / 2);
		if (objs[id].ptr != nullptr) {
			return;
		}
		objs[id].ptr = mod;
		objs[id].x = x;
		objs[id].y = y;
		objs[id].z = z;
		push_canlook(objs[id]);
	}
	void put_block_begin(int x, int y, int z, block_mod* mod) {
		if (abs(x) > size_x / 2 || abs(y) > size_y / 2 || abs(z) > size_z / 2) {
			return;
		}
		auto id = (x + size_x / 2)*(size_y + 1) * (size_z + 1) + (y + size_y / 2) * (size_z + 1) + (z + size_z / 2);
		if (objs[id].ptr != nullptr) {
			return;
		}
		objs[id].ptr = mod;
		objs[id].x = x;
		objs[id].y = y;
		objs[id].z = z;
		push_canlook_begin(objs[id]);
	}

	void pop_block(int x, int y, int z) {
		auto id = (x + size_x / 2)*(size_y + 1) * (size_z + 1) + (y + size_y / 2) * (size_z + 1) + (z + size_z / 2);
		objs[id].ptr = nullptr;
		objs[id].obj.model.Dispose();
		objs[id].x = x;
		objs[id].y = y;
		objs[id].z = z;
		{
			size_t ids = SIZE_MAX;
			for (auto& m : objs_canlook) {
				if (&objs[id] == m) {
					ids = &m - &objs_canlook[0];
					break;
				}
			}
			if (ids != SIZE_MAX) {
				objs_canlook.erase(objs_canlook.begin() + ids);
			}
		}

		for (auto& n : objs[id].near_) {
			if (n != nullptr) {
				bool p = true;
				for (auto& o : objs_canlook) {
					if (n == o) {
						p = false;
						break;
					}
				}
				if (p) {
					push_canlook(*n);
				}
			}
		}
	}

	void Ready_map(std::string dir) {
		SetUseASyncLoadFlag(TRUE);
		MV1::Load("data/sky/model.mv1", &sky, true);	 //空
		SetUseASyncLoadFlag(FALSE);


		//ノイズ生成
		{

		}
	}
	void Set_map() {
		SetFogStartEnd(0.f, 50.f);
		SetFogColor(0, 0, 0);
	}
	void Start_map() {
	}
	void Delete_map() {
		sky.Dispose();	 //空
	}

	//空描画
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
	//
	void map_draw() {
		int p = 0;
		for (auto& m : objs_canlook) {
			if (
				CheckCameraViewClip_Box(
					(m->obj.model.GetPosition() + VGet(-cube_size_x / 2, -cube_size_y / 2, -cube_size_z / 2)).get(),
					(m->obj.model.GetPosition() + VGet(cube_size_x / 2, cube_size_y / 2, cube_size_z / 2)).get()
				)
				) {
				continue;
			}
			m->obj.model.DrawModel();
			/*
			if (
				CheckCameraViewClip_Box(
					VGet(float(m->x)*cube_size_x - cube_size_x / 2, float(m->y)*cube_size_y - cube_size_y / 2, float(m->z)*cube_size_z - cube_size_z / 2),
					VGet(float(m->x)*cube_size_x + cube_size_x / 2, float(m->y)*cube_size_y + cube_size_y / 2, float(m->z)*cube_size_z + cube_size_z / 2)
				)
				) {
				continue;
			}
			DrawCube3D(
				VGet(float(m->x)*cube_size_x - cube_size_x / 2, float(m->y)*cube_size_y - cube_size_y / 2, float(m->z)*cube_size_z - cube_size_z / 2),
				VGet(float(m->x)*cube_size_x + cube_size_x / 2, float(m->y)*cube_size_y + cube_size_y / 2, float(m->z)*cube_size_z + cube_size_z / 2),
				GetColor(0, 255, 0),
				GetColor(255, 255, 255),
				TRUE
			);
			//*/
		}
		return;
	}
};
