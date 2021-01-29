#pragma once

// プレイヤー関係の定義
#define PLAYER_ENUM_DEFAULT_SIZE	1.8f	// 周囲のポリゴン検出に使用する球の初期サイズ
#define PLAYER_HIT_WIDTH			0.15f	// 当たり判定カプセルの半径
#define PLAYER_HIT_HEIGHT			1.8f	// 当たり判定カプセルの高さ
#define PLAYER_HIT_TRYNUM			16		// 壁押し出し処理の最大試行回数
#define PLAYER_HIT_SLIDE_LENGTH		0.015f	// 一度の壁押し出し処理でスライドさせる距離

class Mapclass :Mainclass {
private:
	int size_ = 10;
	class block_mod {
	private:
	public:

	};

	class block_obj {
	private:
	public:
		block_mod* ptr = nullptr;
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
		mods.resize(mods.size() + 1);
		mods.back();

		for (int x = -size_ / 2; x <= size_ / 2; x++) {
			for (int y = -size_ / 2; y <= size_ / 2; y++) {
				for (int z = -size_ / 2; z <= size_ / 2; z++) {
					objs.resize(objs.size() + 1);
					objs.back().ptr = &mods.back();
					objs.back().x = x;
					objs.back().y = y;
					objs.back().z = z;
				}
			}
		}
		for (auto& m : objs) {
			set_block(m);
			push_canlook(m);
		}

	}
	~Mapclass() {
	}

	void set_block(block_obj& m) {
		for (auto& n : m.near_) {
			n = nullptr;
		}

		{
			auto id = (m.x - 1 + size_ / 2)*size_ * size_ + (m.y + size_ / 2) * size_ + (m.z + size_ / 2);
			if (id >= 0 && id <= objs.size() - 1 && abs(m.x) != size_ / 2) {
				m.near_[0] = &objs[id];
			}
		}
		{
			auto id = (m.x + 1 + size_ / 2)*size_ * size_ + (m.y + size_ / 2) * size_ + (m.z + size_ / 2);
			if (id >= 0 && id <= objs.size() - 1 && abs(m.x) != size_ / 2) {
				m.near_[1] = &objs[id];
			}
		}
		{
			auto id = (m.x + size_ / 2)*size_ * size_ + (m.y - 1 + size_ / 2) * size_ + (m.z + size_ / 2);
			if (id >= 0 && id <= objs.size() - 1 && abs(m.y) != size_ / 2) {
				m.near_[2] = &objs[id];
			}
		}
		{
			auto id = (m.x + size_ / 2)*size_ * size_ + (m.y + 1 + size_ / 2) * size_ + (m.z + size_ / 2);
			if (id >= 0 && id <= objs.size() - 1 && abs(m.y) != size_ / 2) {
				m.near_[3] = &objs[id];
			}
		}
		{
			auto id = (m.x + size_ / 2)*size_ * size_ + (m.y + size_ / 2) * size_ + (m.z - 1 + size_ / 2);
			if (id >= 0 && id <= objs.size() - 1 && abs(m.z) != size_ / 2) {
				m.near_[4] = &objs[id];
			}
		}
		{
			auto id = (m.x + size_ / 2)*size_ * size_ + (m.y + size_ / 2) * size_ + (m.z + 1 + size_ / 2);
			if (id >= 0 && id <= objs.size() - 1 && abs(m.z) != size_ / 2) {
				m.near_[5] = &objs[id];
			}
		}
	}
	void push_canlook(block_obj& m) {
		int cnt = 0;
		for (auto& n : m.near_) {
			if (n != nullptr) {
				cnt++;
			}
		}
		if (cnt != m.near_.size()) {
			objs_canlook.resize(objs_canlook.size() + 1);
			objs_canlook.back() = &m;
		}
	}

	void put_block(int x, int y, int z, block_mod* mod) {
		auto id = (x + size_ / 2)*size_ * size_ + (y + size_ / 2) * size_ + (z + size_ / 2);
		objs[id].ptr = mod;
		objs[id].x = x;
		objs[id].y = y;
		objs[id].z = z;
		push_canlook(objs[id]);
	}
	void pop_block(int x, int y, int z) {
		auto id = (x + size_ / 2)*size_ * size_ + (y + size_ / 2) * size_ + (z + size_ / 2);
		objs[id].ptr = nullptr;
		objs[id].x = x;
		objs[id].y = y;
		objs[id].z = z;
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
		for (auto& n : objs[id].near_) {
			if (n != nullptr) {
				push_canlook(*n);
			}
		}
	}

	void Ready_map(std::string dir) {
		SetUseASyncLoadFlag(TRUE);
		SetUseASyncLoadFlag(FALSE);
	}
	void Set_map() {
		SetFogStartEnd(40.0f-15.f, 40.f);
		SetFogColor(12, 6, 0);
	}
	void Start_map() {
	}
	void Delete_map() {
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
			int x = m->x;
			int y = m->y;
			int z = m->z;

			if (
				CheckCameraViewClip_Box(
					VGet(float(x), float(y), float(z)),
					VGet(float(x) + 1.f, float(y) + 1.f, float(z) + 1.f)
				)
				) {
				continue;
			}
			DrawCube3D(
				VGet(float(x), float(y), float(z)),
				VGet(float(x) + 1.f, float(y) + 1.f, float(z) + 1.f),
				GetColor(0, 255, 0),
				GetColor(255, 255, 255),
				TRUE
			);
		}
		return;
	}
};
