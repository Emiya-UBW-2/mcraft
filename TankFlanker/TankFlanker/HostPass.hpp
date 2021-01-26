#pragma once
#include "DXLib_ref/DXLib_ref.h"

class HostPassEffect {
private:
	GraphHandle FarScreen_;		//�`��X�N���[��
	GraphHandle NearFarScreen_;	//�`��X�N���[��
	GraphHandle NearScreen_;	//�`��X�N���[��
	GraphHandle GaussScreen_;	//�`��X�N���[��
	GraphHandle BufScreen;		//�`��X�N���[��
	GraphHandle BufScreen_;		//�`��X�N���[��
	GraphHandle SkyScreen;		//��`��
	GraphHandle MAIN_Screen;	//�`��X�N���[��

	int EXTEND = 4;
	bool dof_flag = true;
	bool bloom_flag = true;
	int disp_x = deskx;
	int disp_y = desky;
public:
	int input_low = 25;
	int input_high = 255;
	float gamma = 1.8f;
	int output_low = 0;
	int output_high = 255;

	HostPassEffect(const bool& dof_, const bool& bloom_, const int& xd, const int& yd) {
		disp_x = xd;
		disp_y = yd;
		//DoF�p
		dof_flag = dof_;
		SkyScreen = GraphHandle::Make(disp_x, disp_y, false);						//��`��
		FarScreen_ = GraphHandle::Make(disp_x, disp_y, true);						//�`��X�N���[��
		NearFarScreen_ = GraphHandle::Make(disp_x, disp_y, true);					//�`��X�N���[��
		NearScreen_ = GraphHandle::Make(disp_x, disp_y, true);						//�`��X�N���[��
		//�u���[���p
		bloom_flag = bloom_;
		GaussScreen_ = GraphHandle::Make(disp_x / EXTEND, disp_y / EXTEND, true);	//�`��X�N���[��
		BufScreen_ = GraphHandle::Make(disp_x, disp_y, true);						//�`��X�N���[��
		//�ŏI�`��p
		BufScreen = GraphHandle::Make(disp_x, disp_y, true);						//�`��X�N���[��
		MAIN_Screen = GraphHandle::Make(disp_x, disp_y, true);						//�`��X�N���[��
	}
	~HostPassEffect() {
	}
private:
	//�u���[���G�t�F�N�g
	void buf_bloom(const int& level = 255) {
		if (bloom_flag) {
			GraphFilterBlt(BufScreen.get(), BufScreen_.get(), DX_GRAPH_FILTER_TWO_COLOR, 250, GetColor(0, 0, 0), 255, GetColor(128, 128, 128), 255);
			GraphFilterBlt(BufScreen_.get(), GaussScreen_.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			GraphFilter(GaussScreen_.get(), DX_GRAPH_FILTER_GAUSS, 16, 1000);
		}
		BufScreen.SetDraw_Screen(false);
		if (bloom_flag) {
			SetDrawMode(DX_DRAWMODE_BILINEAR);
			SetDrawBlendMode(DX_BLENDMODE_ADD, level);
			GaussScreen_.DrawExtendGraph(0, 0, disp_x, disp_y, true);
			GaussScreen_.DrawExtendGraph(0, 0, disp_x, disp_y, true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	}
	//���x���␳
	void buf_levelcorrect() {
		if (true) {
			GraphFilterBlt(BufScreen.get(), BufScreen_.get(), DX_GRAPH_FILTER_LEVEL, input_low, input_high, int(gamma * 100), output_low, output_high);
		}
		BufScreen.SetDraw_Screen(false);
		if (true) {
			BufScreen_.DrawExtendGraph(0, 0, disp_x, disp_y, true);
		}
	}

	//��ʑ̐[�x�`��
	template <typename T1, typename T2>
	void near_dof(T1 sky_doing, T2 doing, DXDraw::cam_info& cams, bool update_effekseer = true) {
		if (dof_flag) {
			//��
			SkyScreen.SetDraw_Screen(cams.campos - cams.camvec, VGet(0, 0, 0), cams.camup, cams.fov, 1000.0f, 5000.0f);
			{
				sky_doing();
			}
			//������
			FarScreen_.SetDraw_Screen(cams.campos, cams.camvec, cams.camup, cams.fov, cams.far_, 1000000.f);
			{
				SkyScreen.DrawGraph(0, 0, FALSE);
				doing();
			}
			//����
			NearFarScreen_.SetDraw_Screen(cams.campos, cams.camvec, cams.camup, cams.fov, cams.near_, cams.far_);
			{
				Effekseer_Sync3DSetting();
				GraphFilter(FarScreen_.get(), DX_GRAPH_FILTER_GAUSS, 16, 200);
				FarScreen_.DrawGraph(0, 0, false);
				if (update_effekseer) {
					UpdateEffekseer3D();
				}
				doing();
				DrawEffekseer3D();
			}
			//����
			NearScreen_.SetDraw_Screen(cams.campos, cams.camvec, cams.camup, cams.fov, 0.1f, 0.1f + cams.near_);
			{
				NearFarScreen_.DrawGraph(0, 0, false);
				doing();
			}
		}
	}
	//���ɉ��������`��
	template <typename T1, typename T2>
	void near_nomal(T1 sky_doing, T2 doing, DXDraw::cam_info& cams, bool update_effekseer = true) {
		//��
		SkyScreen.SetDraw_Screen(cams.campos - cams.camvec, VGet(0, 0, 0), cams.camup, cams.fov, 1000.0f, 5000.0f);
		{
			sky_doing();
		}
		NearScreen_.SetDraw_Screen(cams.campos, cams.camvec, cams.camup, cams.fov, 0.1f, cams.far_);
		{
			Effekseer_Sync3DSetting();
			SkyScreen.DrawGraph(0, 0, FALSE);
			if (update_effekseer) {
				UpdateEffekseer3D();
			}
			doing();
			DrawEffekseer3D();
		}
	}
public:
	template <typename T1 ,typename T2>
	void BUF_draw(T1 sky_doing, T2 doing, DXDraw::cam_info& cams, bool update_effekseer = true) {
		//near�ɕ`��
		if (dof_flag) {
			near_dof(sky_doing, doing, cams, update_effekseer);
		}
		else {
			near_nomal(sky_doing, doing, cams, update_effekseer);
		}
		//���ʕ`��
		BufScreen.SetDraw_Screen();
		{
			NearScreen_.DrawGraph(0, 0, false);
		}
	}
	//
	void MAIN_draw() {
		//buf�ɕ`��
		buf_bloom(255);//�u���[��
		buf_levelcorrect();
		//���ʕ`��
		MAIN_Screen.SetDraw_Screen();
		{
			BufScreen.DrawGraph(0, 0, false);
		}
	}
	//
	GraphHandle& get_main() {
		return MAIN_Screen;
	}
};
