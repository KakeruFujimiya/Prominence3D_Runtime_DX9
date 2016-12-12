//-----------------------------------------------------------------------------
// Prominence3D Runtime Sample
//
// Copyright(C) 藤宮翔流 All Rights Reserved.
// https://github.com/KakeruFujimiya/Prominence3D_Runtime_DX9
//-----------------------------------------------------------------------------
//
// Sample.cpp
//

/******************************************************************************

	Prominence3Dで作成したエフェクトをDirectX9で表示するサンプルです。

	実行すると炎のエフェクト表示され続け、キーボードのA・S・Z・Xキーを押すと
	それぞれ異なったエフェクトが表示されます。

	Zを押したときに表示される炎エフェクトは、
	もう一度Zを押すまで表示され続けます。

	Aを押したときに表示されるビームエフェクトは、
	再生が終わるまで次のビームは発射できないようになっており、
	さらにグルグル回転するようになっています。

	Prominence3Dに関係する部分には ※印 を付けてあります。

******************************************************************************/

//-----------------------------------------------------------------------------
// インクルード
//-----------------------------------------------------------------------------
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#include "P3DEngine.h"					//=================================================== ※


//-----------------------------------------------------------------------------
// ライブラリのロード
//-----------------------------------------------------------------------------
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "P3DEngine.lib")	//=================================================== ※


//-----------------------------------------------------------------------------
// グローバル変数
//-----------------------------------------------------------------------------
LPDIRECT3D9             pD3D = NULL;
LPDIRECT3DDEVICE9       pDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 pVB = NULL;
LPDIRECT3DTEXTURE9		pTexture = NULL;

P3DEngine*				pP3D = NULL;			//Prominence3Dランタイム本体						============ ※
P3DEffect*				pEffectBeam = NULL;		//発射したビームエフェクトのアドレスを入れる変数	============ ※
P3DEffect*				pEffectFire = NULL;		//発射した炎エフェクトのアドレスを入れる変数		============ ※


//-----------------------------------------------------------------------------
// 頂点バッファ用構造体
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
	FLOAT x, y, z;	//位置
	FLOAT u, v;		//ＵＶ
};



//-----------------------------------------------------------------------------
// 定数
//-----------------------------------------------------------------------------
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

//使用するエフェクトのファイル名				=================================================== ※
#define EFFECT_FIRE		"assets/炎.p3b"
#define EFFECT_BOMB		"assets/爆発.p3b"
#define EFFECT_MAGIC	"assets/魔方陣.p3b"
#define EFFECT_BEAM		"assets/ビーム.p3b"




/////////////////////////////////////////////////////////////////////////////////////
// ここから関数
/////////////////////////////////////////////////////////////////////////////////////

/**
* Direct3Dの初期化
* @param  hWnd	ウィンドウハンドル
* @return 成功すればS_OK
*/
HRESULT InitD3D(HWND hWnd)
{

	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION)))	return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDevice)))
	{
		return E_FAIL;
	}
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	//ビュー行列
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(0.0f, 20.0f, -20.0f), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
	pDevice->SetTransform(D3DTS_VIEW, &matView);

	//プロジェクション行列
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f);
	pDevice->SetTransform(D3DTS_PROJECTION, &matProj);


	return S_OK;
}




/**
* 床の作成
* @return P3DEffect型のアドレス。後から変形させたり、再生中か判断するときに使用する。
* @return 成功すればS_OK
*/
HRESULT InitGeometry()
{
	//頂点情報
	CUSTOMVERTEX g_Vertices[] =
	{
		{ -7.0f, 0.0f, 7.0f, 0, 0, },
		{ 7.0f, 0.0f, 7.0f, 1, 0, },
		{ -7.0f, 0.0f, -7.0f, 0, 1, },
		{ 7.0f, 0.0f, -7.0f, 1, 1, },
	};


	//頂点バッファの作成
	if (FAILED(pDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX,	D3DPOOL_DEFAULT, &pVB, NULL)))
		return E_FAIL;
	VOID* pVertices;
	if (FAILED(pVB->Lock(0, sizeof(g_Vertices), (void**)&pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, g_Vertices, sizeof(g_Vertices));
	pVB->Unlock();


	//テクスチャ
	D3DXCreateTextureFromFileEx(pDevice, "Assets/floor.png", 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT,
		D3DX_FILTER_NONE, D3DX_DEFAULT, NULL, NULL, NULL, &pTexture);

	return S_OK;
}




/**
* 開放処理
*/
VOID Cleanup()
{
	//開放					===================================================================== ※
	if (pP3D != NULL)
		delete pP3D;

	if (pVB != NULL)
		pVB->Release();

	if (pDevice != NULL)
		pDevice->Release();

	if (pD3D != NULL)
		pD3D->Release();
}



/**
* 描画
*/
VOID Render()
{
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 100), 1.0f, 0);


	if (SUCCEEDED(pDevice->BeginScene()))
	{
		//ワールド行列
		D3DXMATRIXA16 matWorld;
		D3DXMatrixIdentity(&matWorld);
		pDevice->SetTransform(D3DTS_WORLD, &matWorld);

		//床を描画
		pDevice->SetStreamSource(0, pVB, 0, sizeof(CUSTOMVERTEX));
		pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		pDevice->SetTexture(0, pTexture);
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);


		//全エフェクトの描画		===================================================================== ※
		pP3D->Draw();

		//ビームエフェクトを回転	===================================================================== ※
		pP3D->SetRotation(pEffectBeam, D3DXVECTOR3(0, D3DXToRadian(timeGetTime() / 10), 0));



		pDevice->EndScene();
	}

	pDevice->Present(NULL, NULL, NULL, NULL);
}




/**
* ウィンドウプロシージャ
*/
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		//キー押下
	case WM_CHAR:
		switch (wParam)
		{
		case 's':
			//魔方陣エフェクトを（5,0,5）の位置に発生	============================================== ※
			pP3D->Play(EFFECT_MAGIC, D3DXVECTOR3(5, 0, 5));
			break;

		case 'x':
			//爆発エフェクトを（5,0,-5）の位置に発生	============================================== ※
			pP3D->Play(EFFECT_BOMB, D3DXVECTOR3(5, 0, -5));
			break;

		case 'a':
			//ビームが発射中じゃなければ				============================================== ※
			if (!pP3D->IsPlayng(pEffectBeam))
			{
				//ビームを（-5,0,5）の位置に発生		============================================== ※
				pEffectBeam = pP3D->Play(EFFECT_BEAM, D3DXVECTOR3(-5, 0, 5));
			}
			break;

		case 'z':
			//炎が表示中なら							============================================== ※
			if (pP3D->IsPlayng(pEffectFire))
			{
				//炎エフェクトを止める（削除）
				pP3D->Stop(pEffectFire);
			}

			//炎が表示中じゃなければ
			else
			{
				//炎エフェクトを(-5,0,-5)の位置に発生（第３引数をTRUEにしているのでずっと表示される）	========== ※
				pEffectFire = pP3D->Play(EFFECT_FIRE, D3DXVECTOR3(-5, 0, -5), TRUE);
			}
			break;
		}
		break;

	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}




/**
* エントリーポイント
*/
INT WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, INT)
{

	//ウィンドウクラス
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(NULL), 
		NULL, NULL, NULL, NULL,	"Prominence3D Sample", NULL
	};
	RegisterClassEx(&wc);

	//ウィンドウ作成
	HWND hWnd = CreateWindow("Prominence3D Sample", "Prominence3D Runtime Sample",
		WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, NULL, NULL, wc.hInstance, NULL);

	//Direct3D初期化
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		//床作成
		if (SUCCEEDED(InitGeometry()))
		{
			//ウィンドウ表示
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);



			//Prominence3Dランタイム初期化用構造体	========================================================== ※
			P3DInitData data;
			data.pDevice = pDevice;			//Direct3Dデバイスオブジェクト
			data.polyDivisionNum = 64;

			//Prominence3Dランタイム本体を生成		========================================================== ※
			pP3D = new P3DEngine(data);

			//使用するエフェクトファイルをすべて読み込む	================================================== ※
			pP3D->Load(EFFECT_FIRE);
			pP3D->Load(EFFECT_BOMB);
			pP3D->Load(EFFECT_MAGIC);
			pP3D->Load(EFFECT_BEAM);






			//メッセージループ
			MSG msg;
			ZeroMemory(&msg, sizeof(msg));
			while (msg.message != WM_QUIT)
			{
				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					//描画処理
					Render();
				}
			}
		}
	}

	return 0;
}