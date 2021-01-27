#pragma once

// プレイヤー関係の定義
#define PLAYER_ENUM_DEFAULT_SIZE	1.8f	// 周囲のポリゴン検出に使用する球の初期サイズ
#define PLAYER_HIT_WIDTH			0.15f	// 当たり判定カプセルの半径
#define PLAYER_HIT_HEIGHT			1.8f	// 当たり判定カプセルの高さ
#define PLAYER_HIT_TRYNUM			16		// 壁押し出し処理の最大試行回数
#define PLAYER_HIT_SLIDE_LENGTH		0.015f	// 一度の壁押し出し処理でスライドさせる距離

class Mapclass :Mainclass {
private:
	class block_mod {
	private:
	public:

	};

	class block_obj {
	private:
	public:
		block_mod* ptr = nullptr;

	};

	MV1 sky;			//空

public:
	std::vector<block_mod> mods;
	std::vector<block_obj> objs;


	Mapclass() {
		mods.resize(mods.size() + 1);
		mods.back();

		for (int x = -10; x <= 10; x++) {
			for (int y = -10; y <= 10; y++) {
				for (int z = -10; z <= 10; z++) {
					objs.resize(objs.size() + 1);
					if (GetRand(100) <= 25) {
						objs.back().ptr = &mods.back();
					}
				}
			}
		}
	}
	~Mapclass() {

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
		for (int x = -10; x <= 10; x++) {
			for (int y = -10; y <= 10; y++) {
				for (int z = -10; z <= 10; z++) {
					if (objs[p++].ptr == nullptr) {
						continue;
					}
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
			}
		}
		return;
	}
};
