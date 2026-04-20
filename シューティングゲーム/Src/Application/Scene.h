#pragma once

class Scene
{
private:

	// テクスチャ ・・・ 画像データ
	KdTexture charaTex;
	KdTexture backTex;
	KdTexture enemyTex;
	KdTexture bulletTex;
	KdTexture expTex;

	//背景
	Math::Matrix backMat1;
	Math::Matrix backMat2;
	float backY;

	//プレイヤー(自機)の変数
	Math::Matrix charaMat; //行列...座標・回転・拡縮の管理
	float playerX;         //自機のX座標
	float playerY;         //自機のY座標
	int playerFlg;
	float playerAnimCnt;

	//ボス
	Math::Matrix bossMat;
	float bossX;
	float bossY;
	int bossFlg;
	float bossSize;   //拡大率
	float bossRadius; //半径
	int bossHP;
	float bossAngle;  //角度

	//敵
	static const int enemyNum = 10;   //敵の数(書き換え不可能)

	Math::Matrix enemyMat[enemyNum];
	float enemyX[enemyNum];
	float enemyY[enemyNum];
	int enemyFlg[enemyNum];   //敵の生存状態を管理(0,死亡　1,生存)

	//弾の追加
	static const int bulletNum = 30;

	Math::Matrix bulletMat[bulletNum];
	float bulletX[bulletNum];
	float bulletY[bulletNum];
	float bulletFlg[bulletNum];

	int shotWait;   //発射待機時間

	//爆発
	static const int expNum = 10;

	Math::Matrix expMat[expNum];
	float expX[expNum];
	float expY[expNum];
	int expFlg[expNum];
	float expAnimCnt[expNum];  //アニメーション用
	
	//スコア
	int score;

	int test = 0;

public:

	// 初期設定
	void Init();

	// 解放
	void Release();

	// 更新処理
	void Update();

	// 描画処理
	void Draw2D();

	// GUI処理
	void ImGuiUpdate();

	//リセット
	void RESET();

	//爆発
	void Explosion(float x, float y);

private:

	Scene() {}

public:
	static Scene& GetInstance()
	{
		static Scene instance;
		return instance;
	}
};

#define SCENE Scene::GetInstance()
