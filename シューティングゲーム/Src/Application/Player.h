#pragma once

struct Player
{
	KdTexture tex;
	Math::Matrix mat;
	bool flg;
	int x;
	int y;
	float animCnt;
};

class C_Player
{
public:

	C_Player();
	~C_Player();

	void PlayerInit();
	void PlayerUpdate();
	void PlayerDraw();

private:

	static const int maxAnimCnt = 4;
	Player player;

};
