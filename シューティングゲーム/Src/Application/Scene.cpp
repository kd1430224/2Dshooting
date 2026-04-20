#include "main.h"
#include "Scene.h"

void Scene::Draw2D()
{
	//背景(１枚目)
	SHADER.m_spriteShader.SetMatrix(backMat1);
	SHADER.m_spriteShader.DrawTex(&backTex, Math::Rectangle{ 0,0,1280,720 }, 1.0f);
	//背景(２枚目)
	SHADER.m_spriteShader.SetMatrix(backMat2);
	SHADER.m_spriteShader.DrawTex(&backTex, Math::Rectangle{ 0,0,1280,720 }, 1.0f);

	//自機の描画
	if (playerFlg == 1)
	{
		//①移動行列をセット
		SHADER.m_spriteShader.SetMatrix(charaMat);
		//②描画
		SHADER.m_spriteShader.DrawTex(&charaTex, Math::Rectangle{ (int)playerAnimCnt * 64,0,64,64 }, 1.0f);
	}

	//ボス
	if (bossFlg == 1)
	{
		SHADER.m_spriteShader.SetMatrix(bossMat);
		SHADER.m_spriteShader.DrawTex(&enemyTex, Math::Rectangle{ 0,0,64,64 }, 1.0f);
	}

	//敵
	for (int e = 0; e < enemyNum; e++)
	{
		if (enemyFlg[e] == 1)
		{
			SHADER.m_spriteShader.SetMatrix(enemyMat[e]);
			SHADER.m_spriteShader.DrawTex(&enemyTex, Math::Rectangle{ 0,0,64,64 }, 1.0f);
		}
	}

	//弾
	for (int bu = 0; bu < bulletNum; bu++)
	{
		if (bulletFlg[bu] == 1)
		{
			SHADER.m_spriteShader.SetMatrix(bulletMat[bu]);
			SHADER.m_spriteShader.DrawTex(&bulletTex, Math::Rectangle{ 0,0,16,16 }, 1.0f);
		}
	}

	//爆発
	for (int ex = 0; ex < expNum; ex++)
	{
		if (expFlg[ex] == 1)
		{
			SHADER.m_spriteShader.SetMatrix(expMat[ex]);              // ↓ 一時的にint型として扱う
			SHADER.m_spriteShader.DrawTex(&expTex, Math::Rectangle{ (int)expAnimCnt[ex] * 64,0,64,64 }, 1.0f);
		}
	}
	//==========この下にDrawStringを書く==========

	// スコア表示
	if (playerFlg == 1)
	{
		char text[200];   //①文字列格納用の配列作成
		sprintf_s(text, sizeof(text), "score:%d", score);  //② " "内の文字列がtextに格納される
		SHADER.m_spriteShader.DrawString(-640, 360, text, Math::Vector4(1, 1, 0, 1));  //③描画
	}
	
	//ゲームオーバー表示
	if (playerFlg == 0)
	{
		SHADER.m_spriteShader.DrawString(-100, 100, "GAME OVER", Math::Vector4(1, 0, 0, 1));

		char text[200];
		sprintf_s(text, sizeof(text), "score:%d", score);
		SHADER.m_spriteShader.DrawString(0, -100, text, Math::Vector4(1, 1, 0, 1)); 
	}
}

void Scene::Update() 
{
	if (playerFlg == 1)
	{
		//自機の移動処理
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			playerX += 5;  //playerX = playerX + 1　と同じ意味
		}
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			playerX -= 5;
		}
		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			playerY += 5;
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			playerY -= 5;
		}

		//両端判定
		if (playerX > 640 - 32)       //自機は64pxなので半径は32px
		{
			playerX = 640 - 32;
		}
		if (playerX < -640 + 32)
		{
			playerX = -640 + 32;
		}

		//スペースで弾発射
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			//待機時間が０の場合
			if (shotWait == 0)
			{
				for (int bu = 0; bu < bulletNum; bu++)
				{
					if (bulletFlg[bu] == 0)
					{
						bulletFlg[bu] = 1;
						bulletX[bu] = playerX;
						bulletY[bu] = playerY;

						//待機時間10フレーム
						shotWait = 10;
						break;  //1発「発射状態」にしたので
					}
				}
			}
		}
		//自機のアニメーション
		playerAnimCnt += 0.2f;
		if (playerAnimCnt >= 4)
		{
			playerAnimCnt = 0;
		}
	}

	//ボス出現
	if (score >= 500 && bossHP > 0)
	{
		bossFlg = 1;
	}
	if (bossFlg == 1)
	{
		bossY -= 2;
		if (bossY <= 0)
		{
			bossY = 0;
		}
		//回転
		bossAngle += 1;

		if (bossAngle >= 360)
		{
			// 1周超えたので-360する
			bossAngle = -360;
		}

		//HPの確認
		if (bossHP <= 0)
		{
			bossFlg = 0;
			Explosion(bossX, bossY);
			score += 500;
		}

	}

	//敵の動き
	for (int e = 0; e < enemyNum; e++)
	{
		if (enemyFlg[e] == 1)  //敵が生きていたら
		{
			enemyY[e] -= 3;

			//敵が下端まで到達したら上から再出現
			if (enemyY[e] < -360 - 32)
			{
				enemyY[e] = 360 + 32;
			}

			if (playerFlg == 1)
			{
				//自機との当たり判定
				float a = enemyX[e] - playerX;    //底辺(X座標の差)
				float b = enemyY[e] - playerY;    //高さ(Y座標の差)
				float c = sqrt(a * a + b * b); //斜辺(距離)

				if (c < 32 + 32)  //衝突してしたら (斜辺 < 半径 + 半径)
				{
					//敵を倒す
					enemyFlg[e] = 0;
					//自機死亡
					playerFlg = 0;
					//爆発
					Explosion(playerX, playerY);
					Explosion(enemyX[e], enemyY[e]);
				}
			}
		}
	}
	//2％の確率で敵を1体復活させる
	if ((rand() % 100 + 1) <= 2)//2%の確率
	{
		for (int e = 0; e < enemyNum; e++)//敵
		{
			if (enemyFlg[e] == 0)//死んでる敵がいたら
			{
				//1体だけ復活させる
				enemyFlg[e] = 1;
				enemyX[e] = rand() % (1281 - 64) - (640 - 32);
				enemyY[e] = 360 + 32;

				break;
			}
		}
	}

	//弾発射後の処理
	for (int bu = 0; bu < bulletNum; bu++)
	{
		if (bulletFlg[bu] == 1)
		{
			bulletY[bu] += 15;

			//弾が画面外に出たら未発射状態に戻す
			if (bulletY[bu] >= 360 + 8)
			{
				bulletFlg[bu] = 0;
			}
			//敵との当たり判定
			for (int e = 0; e < enemyNum; e++)
			{
				if (enemyFlg[e] == 1)  //敵が生きているか
				{
					float a = enemyX[e] - bulletX[bu];
					float b = enemyY[e] - bulletY[bu];
					float c = sqrt(a * a + b * b);
					if (c < 32 + 8)  //敵と弾が衝突したら
					{
						//敵を倒す
						enemyFlg[e] = 0;
						//弾を未発射に
						bulletFlg[bu] = 0;

						//爆発発生   ↓値渡し
						Explosion(enemyX[e], enemyY[e]);

						//スコア加算
						score += 100;

						//敵と弾が衝突したら、弾が未発射状態になったので敵の繰り返しを抜ける
						break;
					}
				}
			}

			//ボスとの当たり判定
			if (bossFlg == 1)
			{
				//距離判定
				float a = bossX - bulletX[bu];
				float b = bossY - bulletY[bu];
				float c = sqrt(a * a + b * b);
				if (c < bossRadius + 8)
				{
					bossHP -= 1;
					Explosion(bulletX[bu], bulletY[bu]);
					bulletFlg[bu] = 0;
				}
			}

		}
	}
	//弾の待機時間
	shotWait--;
	if (shotWait < 0)
	{
		shotWait = 0;
	}

	//アニメーション
	for (int ex = 0; ex < expNum; ex++)
	{
		if (expFlg[ex] == 1)
		{
			expAnimCnt[ex] += 0.2f;  //アニメーションを次のコマへ進める

			//終了チェック
			if (expAnimCnt[ex] >= 8)
			{
				expFlg[ex] = 0;
			}
		}
	}

	//リセット
	RESET();

	//背景スクロール
	backY -= 3;

	if (backY < -720)
	{
		backY = 0;
	}


	//↓Updateの最後に行列作成↓↓
	//自機
	charaMat = Math::Matrix::CreateTranslation(playerX,playerY,0);

	//背景
	backMat1 = Math::Matrix::CreateTranslation(0, backY, 0);
	backMat2 = Math::Matrix::CreateTranslation(0, backY+720, 0);

	//ボス
	
	Math::Matrix scale = Math::Matrix::CreateScale(bossSize, bossSize, 0);   //拡縮行列
	Math::Matrix trans = Math::Matrix::CreateTranslation(bossX, bossY, 0);   //移動行列
	                                                           //↓ラジアン単位に変換している
	Math::Matrix rotate = Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(bossAngle));  //回転行列
	bossMat = scale * rotate * trans;  //拡縮 * 縮小 の順番
	
	//敵
	for (int e = 0; e < enemyNum; e++)
	{
		enemyMat[e] = Math::Matrix::CreateTranslation(enemyX[e], enemyY[e], 0);
	}

	//弾
	for (int bu = 0; bu < bulletNum; bu++)
	{
		bulletMat[bu] = Math::Matrix::CreateTranslation(bulletX[bu], bulletY[bu], 0);
	}
	//爆発
	for (int ex = 0; ex < expNum; ex++)
	{
		expMat[ex] = Math::Matrix::CreateTranslation(expX[ex], expY[ex], 0);
	}
}

void Scene::Init()
{
	//乱数の初期化 (※必ず Init に書く)
	srand(time(0));

	// 画像の読み込み処理
	charaTex.Load("Texture/player.png");
	backTex.Load("Texture/back.png");
	enemyTex.Load("Texture/enemy.png");     //Release()も書く
	bulletTex.Load("Texture/bullet.png");
	expTex.Load("Texture/explosion.png");

	//自機の初期化処理
	playerX = 0;
	playerY = -200;
	playerFlg = 1;
	playerAnimCnt = 0;

	//ボスの初期化処理
	bossX = 0;
	bossY = 360 + 32;
	bossFlg = 0;
	bossSize = 1.5;
	bossHP = 10;
	bossRadius = 32 * bossSize;   //拡大率を考慮した半径
	bossAngle = 0;

	//敵の初期化処理
	for (int e = 0; e < enemyNum; e++)
	{
		//X座標は画面のランダムな位置に
		enemyX[e] = rand() % (1281 - 64) - (640 - 32);
		enemyY[e] = 360 + 32;
		enemyFlg[e] = 1;
	}

	//弾の初期化処理
	for (int bu = 0; bu < bulletNum; bu++)
	{
		bulletX[bu] = 0;
		bulletY[bu] = 0;
		bulletFlg[bu] = 0;
	}
	shotWait = 0;

	//爆発の初期化処理
	for (int ex = 0; ex < expNum; ex++)
	{
		expX[ex] = 0;
		expY[ex] = 0;
		expFlg[ex] = 0;
		expAnimCnt[ex] = 0;
	}

	//スコア
	score = 0;
}

void Scene::Release()
{
	// 画像の解放処理
	charaTex.Release();
	backTex.Release();
	enemyTex.Release();      //忘れないように
	bulletTex.Release();
	expTex.Release();
}

void Scene::ImGuiUpdate()
{
	return;

	ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_Once);

	// デバッグウィンドウ
	if (ImGui::Begin("Debug Window"))
	{
		ImGui::Text("FPS : %d", APP.m_fps);
	}
	ImGui::End();
}

void Scene::RESET()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		//自機
		playerFlg = 1;
		playerX = 0;
		playerY = -200;
		//敵
		for (int e = 0; e < enemyNum; e++)
		{
			//画面内のランダムな位置に敵を配置する
		    //※幅1280,高さ720(画面の中心が X = 0, Y = 0)
			enemyFlg[e] = 1;
			enemyX[e] = rand() % (1281 - 64) - (640 - 32);
			enemyY[e] = 360 + 32;
		}
		score = 0;
	}
}

void Scene::Explosion(float x, float y)
{
	for (int ex = 0; ex < expNum; ex++)
	{
		if (expFlg[ex] == 0) //「未生成」状態であれば
		{
			expFlg[ex] = 1;
			expX[ex] = x;
			expY[ex] = y;
			expAnimCnt[ex] = 0;   //１コマ目から
			//爆発は一つだけ生成
			break;
		}
	}
}