#pragma once

// プレイヤー関係の定義
#define PLAYER_ENUM_DEFAULT_SIZE	1.8f	// 周囲のポリゴン検出に使用する球の初期サイズ
#define PLAYER_HIT_WIDTH			0.15f	// 当たり判定カプセルの半径
#define PLAYER_HIT_HEIGHT			1.8f	// 当たり判定カプセルの高さ
#define PLAYER_HIT_TRYNUM			16		// 壁押し出し処理の最大試行回数
#define PLAYER_HIT_SLIDE_LENGTH		0.015f	// 一度の壁押し出し処理でスライドさせる距離

class Mapclass :Mainclass {
public:
	float cube_size_x = 1.f;
	float cube_size_y = 0.25f;
	float cube_size_z = 1.f;
private:
	int size_x = 100;
	int size_y = 100;
	int size_z = 100;
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
		MV1::Load("data/sky/model.mv1", &sky, true);	 //空


		mods.resize(mods.size() + 1);
		mods.back();

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
			push_canlook(m);
		}

		for (int x = -size_x / 2; x <= size_x / 2; x++) {
			for (int z = -size_z / 2; z <= size_z / 2; z++) {
				put_block(x, -1, z, &mods.back());
			}
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
	void push_canlook(block_obj& m) {
		if (m.ptr!=nullptr) {
			int cnt = 0;
			for (auto& n : m.near_) {
				if (n != nullptr) {
					cnt++;
				}
			}
			if (cnt != m.near_.size()) {
				objs_canlook.emplace_back(&m);
				return;
			}
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
	void pop_block(int x, int y, int z) {
		auto id = (x + size_x / 2)*(size_y + 1) * (size_z + 1) + (y + size_y / 2) * (size_z + 1) + (z + size_z / 2);
		objs[id].ptr = nullptr;
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
		SetUseASyncLoadFlag(FALSE);
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
			int x = m->x;
			int y = m->y;
			int z = m->z;

			if (
				CheckCameraViewClip_Box(
					VGet(float(x)*cube_size_x - cube_size_x / 2, float(y)*cube_size_y - cube_size_y / 2, float(z)*cube_size_z - cube_size_z / 2),
					VGet(float(x)*cube_size_x + cube_size_x / 2, float(y)*cube_size_y + cube_size_y / 2, float(z)*cube_size_z + cube_size_z / 2)
				)
				) {
				continue;
			}
			DrawCube3D(
				VGet(float(x)*cube_size_x - cube_size_x / 2, float(y)*cube_size_y - cube_size_y / 2, float(z)*cube_size_z - cube_size_z / 2),
				VGet(float(x)*cube_size_x + cube_size_x / 2, float(y)*cube_size_y + cube_size_y / 2, float(z)*cube_size_z + cube_size_z / 2),
				GetColor(0, 255, 0),
				GetColor(255, 255, 255),
				TRUE
			);
		}
		return;
	}
};
