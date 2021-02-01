#pragma once

//リサイズ
#define x_r(p1) (int(p1) * deskx / 1920)
#define y_r(p1) (int(p1) * desky / 1080)

class UI : Mainclass {
private:
	//font
	FontHandle font72;
	FontHandle font48;
	FontHandle font36;
	FontHandle font24;
	FontHandle font18;
	FontHandle font12;
	//sound
	SoundHandle decision;
	SoundHandle cancel;
	SoundHandle cursor;

	unsigned int red;
	unsigned int green;
	unsigned int write;
	unsigned int yellow;
	unsigned int gray_1;
	unsigned int gray_2;
public:
	UI() {
		SetUseASyncLoadFlag(TRUE);

		font72 = FontHandle::Create(y_r(72), DX_FONTTYPE_EDGE);
		font48 = FontHandle::Create(y_r(48), DX_FONTTYPE_EDGE);
		font36 = FontHandle::Create(y_r(36), DX_FONTTYPE_EDGE);
		font24 = FontHandle::Create(y_r(24), DX_FONTTYPE_EDGE);
		font18 = FontHandle::Create(y_r(18), DX_FONTTYPE_EDGE);
		font12 = FontHandle::Create(y_r(12), DX_FONTTYPE_EDGE);

		decision = SoundHandle::Load("data/audio/shot_2.wav");//
		cancel = SoundHandle::Load("data/audio/cancel.wav");
		cursor = SoundHandle::Load("data/audio/cursor.wav");

		SetUseASyncLoadFlag(FALSE);

		red = GetColor(255, 0, 0);
		green = GetColor(0, 255, 0);
		write = GetColor(255, 255, 255);
		yellow = GetColor(255, 255, 0);
		gray_1 = GetColor(64, 64, 64);
		gray_2 = GetColor(128, 128, 128);
	}
	~UI() {
	}
	void load_window(const char* mes) {
		SetUseASyncLoadFlag(FALSE);
		float bar = 0.f;
		auto all = GetASyncLoadNum();
		while (ProcessMessage() == 0) {
			int t_disp_x = 1920;
			int t_disp_y = 1080;
			GetScreenState(&t_disp_x, &t_disp_y, nullptr);
			int tmp = all - GetASyncLoadNum();

			GraphHandle::SetDraw_Screen((int32_t)DX_SCREEN_BACK);
			{
				font18.DrawStringFormat(0, t_disp_y - y_r(70), green, " loading... : %04d/%04d  ", tmp, all);
				font18.DrawStringFormat_RIGHT(t_disp_x, t_disp_y - y_r(70), green, "%s 読み込み中 ", mes);
				DrawBox(0, t_disp_y - y_r(50), int(float(t_disp_x) * bar / float(all)), t_disp_y - y_r(40), green, TRUE);
			}
			ScreenFlip();

			if (int(bar * 100) >= (all * 100-1)) {
				break;
			}
			easing_set(&bar, float(tmp), 0.95f);
		}
		GraphHandle::SetDraw_Screen((int32_t)DX_SCREEN_BACK);
		ScreenFlip();
	}

	void set_draw(bool use_vr = true) {
		int t_disp_x = deskx;
		int t_disp_y = desky;
		if (use_vr) {
			GetScreenState(&t_disp_x, &t_disp_y, nullptr);
		}

		int xs = 0, ys = 0, xp = 0, yp = 0;
		FontHandle* font_large = (use_vr) ? &font72 : &font48;
		FontHandle* font_big = (use_vr) ? &font36 : &font24;
		FontHandle* font = (use_vr) ? &font24 : &font18;
		const int font_largehight = (use_vr) ? y_r(72) : y_r(48);
		const int font_bighight = (use_vr) ? y_r(36) : y_r(24);
		const int fonthight = (use_vr) ? y_r(24) : y_r(18);
		{
			//終わり
		}
	}
};
