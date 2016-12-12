//-----------------------------------------------------------------------------
// Prominence3D Runtime Sample
//
// Copyright(C) ���{�ė� All Rights Reserved.
// https://github.com/KakeruFujimiya/Prominence3D_Runtime_DX9
//-----------------------------------------------------------------------------
//
// Sample.cpp
//

/******************************************************************************

	Prominence3D�ō쐬�����G�t�F�N�g��DirectX9�ŕ\������T���v���ł��B

	���s����Ɖ��̃G�t�F�N�g�\�����ꑱ���A�L�[�{�[�h��A�ES�EZ�EX�L�[��������
	���ꂼ��قȂ����G�t�F�N�g���\������܂��B

	Z���������Ƃ��ɕ\������鉊�G�t�F�N�g�́A
	������xZ�������܂ŕ\�����ꑱ���܂��B

	A���������Ƃ��ɕ\�������r�[���G�t�F�N�g�́A
	�Đ����I���܂Ŏ��̃r�[���͔��˂ł��Ȃ��悤�ɂȂ��Ă���A
	����ɃO���O����]����悤�ɂȂ��Ă��܂��B

	Prominence3D�Ɋ֌W���镔���ɂ� ���� ��t���Ă���܂��B

******************************************************************************/

//-----------------------------------------------------------------------------
// �C���N���[�h
//-----------------------------------------------------------------------------
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#include "P3DEngine.h"					//=================================================== ��


//-----------------------------------------------------------------------------
// ���C�u�����̃��[�h
//-----------------------------------------------------------------------------
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "P3DEngine.lib")	//=================================================== ��


//-----------------------------------------------------------------------------
// �O���[�o���ϐ�
//-----------------------------------------------------------------------------
LPDIRECT3D9             pD3D = NULL;
LPDIRECT3DDEVICE9       pDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 pVB = NULL;
LPDIRECT3DTEXTURE9		pTexture = NULL;

P3DEngine*				pP3D = NULL;			//Prominence3D�����^�C���{��						============ ��
P3DEffect*				pEffectBeam = NULL;		//���˂����r�[���G�t�F�N�g�̃A�h���X������ϐ�	============ ��
P3DEffect*				pEffectFire = NULL;		//���˂������G�t�F�N�g�̃A�h���X������ϐ�		============ ��


//-----------------------------------------------------------------------------
// ���_�o�b�t�@�p�\����
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
	FLOAT x, y, z;	//�ʒu
	FLOAT u, v;		//�t�u
};



//-----------------------------------------------------------------------------
// �萔
//-----------------------------------------------------------------------------
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

//�g�p����G�t�F�N�g�̃t�@�C����				=================================================== ��
#define EFFECT_FIRE		"assets/��.p3b"
#define EFFECT_BOMB		"assets/����.p3b"
#define EFFECT_MAGIC	"assets/�����w.p3b"
#define EFFECT_BEAM		"assets/�r�[��.p3b"




/////////////////////////////////////////////////////////////////////////////////////
// ��������֐�
/////////////////////////////////////////////////////////////////////////////////////

/**
* Direct3D�̏�����
* @param  hWnd	�E�B���h�E�n���h��
* @return ���������S_OK
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

	//�r���[�s��
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(0.0f, 20.0f, -20.0f), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
	pDevice->SetTransform(D3DTS_VIEW, &matView);

	//�v���W�F�N�V�����s��
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f);
	pDevice->SetTransform(D3DTS_PROJECTION, &matProj);


	return S_OK;
}




/**
* ���̍쐬
* @return P3DEffect�^�̃A�h���X�B�ォ��ό`��������A�Đ��������f����Ƃ��Ɏg�p����B
* @return ���������S_OK
*/
HRESULT InitGeometry()
{
	//���_���
	CUSTOMVERTEX g_Vertices[] =
	{
		{ -7.0f, 0.0f, 7.0f, 0, 0, },
		{ 7.0f, 0.0f, 7.0f, 1, 0, },
		{ -7.0f, 0.0f, -7.0f, 0, 1, },
		{ 7.0f, 0.0f, -7.0f, 1, 1, },
	};


	//���_�o�b�t�@�̍쐬
	if (FAILED(pDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX,	D3DPOOL_DEFAULT, &pVB, NULL)))
		return E_FAIL;
	VOID* pVertices;
	if (FAILED(pVB->Lock(0, sizeof(g_Vertices), (void**)&pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, g_Vertices, sizeof(g_Vertices));
	pVB->Unlock();


	//�e�N�X�`��
	D3DXCreateTextureFromFileEx(pDevice, "Assets/floor.png", 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT,
		D3DX_FILTER_NONE, D3DX_DEFAULT, NULL, NULL, NULL, &pTexture);

	return S_OK;
}




/**
* �J������
*/
VOID Cleanup()
{
	//�J��					===================================================================== ��
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
* �`��
*/
VOID Render()
{
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 100), 1.0f, 0);


	if (SUCCEEDED(pDevice->BeginScene()))
	{
		//���[���h�s��
		D3DXMATRIXA16 matWorld;
		D3DXMatrixIdentity(&matWorld);
		pDevice->SetTransform(D3DTS_WORLD, &matWorld);

		//����`��
		pDevice->SetStreamSource(0, pVB, 0, sizeof(CUSTOMVERTEX));
		pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		pDevice->SetTexture(0, pTexture);
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);


		//�S�G�t�F�N�g�̕`��		===================================================================== ��
		pP3D->Draw();

		//�r�[���G�t�F�N�g����]	===================================================================== ��
		pP3D->SetRotation(pEffectBeam, D3DXVECTOR3(0, D3DXToRadian(timeGetTime() / 10), 0));



		pDevice->EndScene();
	}

	pDevice->Present(NULL, NULL, NULL, NULL);
}




/**
* �E�B���h�E�v���V�[�W��
*/
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		//�L�[����
	case WM_CHAR:
		switch (wParam)
		{
		case 's':
			//�����w�G�t�F�N�g���i5,0,5�j�̈ʒu�ɔ���	============================================== ��
			pP3D->Play(EFFECT_MAGIC, D3DXVECTOR3(5, 0, 5));
			break;

		case 'x':
			//�����G�t�F�N�g���i5,0,-5�j�̈ʒu�ɔ���	============================================== ��
			pP3D->Play(EFFECT_BOMB, D3DXVECTOR3(5, 0, -5));
			break;

		case 'a':
			//�r�[�������˒�����Ȃ����				============================================== ��
			if (!pP3D->IsPlayng(pEffectBeam))
			{
				//�r�[�����i-5,0,5�j�̈ʒu�ɔ���		============================================== ��
				pEffectBeam = pP3D->Play(EFFECT_BEAM, D3DXVECTOR3(-5, 0, 5));
			}
			break;

		case 'z':
			//�����\�����Ȃ�							============================================== ��
			if (pP3D->IsPlayng(pEffectFire))
			{
				//���G�t�F�N�g���~�߂�i�폜�j
				pP3D->Stop(pEffectFire);
			}

			//�����\��������Ȃ����
			else
			{
				//���G�t�F�N�g��(-5,0,-5)�̈ʒu�ɔ����i��R������TRUE�ɂ��Ă���̂ł����ƕ\�������j	========== ��
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
* �G���g���[�|�C���g
*/
INT WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, INT)
{

	//�E�B���h�E�N���X
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(NULL), 
		NULL, NULL, NULL, NULL,	"Prominence3D Sample", NULL
	};
	RegisterClassEx(&wc);

	//�E�B���h�E�쐬
	HWND hWnd = CreateWindow("Prominence3D Sample", "Prominence3D Runtime Sample",
		WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, NULL, NULL, wc.hInstance, NULL);

	//Direct3D������
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		//���쐬
		if (SUCCEEDED(InitGeometry()))
		{
			//�E�B���h�E�\��
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);



			//Prominence3D�����^�C���������p�\����	========================================================== ��
			P3DInitData data;
			data.pDevice = pDevice;			//Direct3D�f�o�C�X�I�u�W�F�N�g
			data.polyDivisionNum = 64;

			//Prominence3D�����^�C���{�̂𐶐�		========================================================== ��
			pP3D = new P3DEngine(data);

			//�g�p����G�t�F�N�g�t�@�C�������ׂēǂݍ���	================================================== ��
			pP3D->Load(EFFECT_FIRE);
			pP3D->Load(EFFECT_BOMB);
			pP3D->Load(EFFECT_MAGIC);
			pP3D->Load(EFFECT_BEAM);






			//���b�Z�[�W���[�v
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
					//�`�揈��
					Render();
				}
			}
		}
	}

	return 0;
}