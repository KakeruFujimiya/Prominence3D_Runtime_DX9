//-----------------------------------------------------------------------------
// Prominence3D Runtime for DirectX9
//
// Copyright(C) ���{�ė� All Rights Reserved.
// https://github.com/KakeruFujimiya/Prominence3D_Runtime_DX9
//-----------------------------------------------------------------------------
//
// P3DEnginr.h
//

//-----------------------------------------------------------------------------
// �C���N���[�h
//-----------------------------------------------------------------------------
#pragma once
#pragma warning(disable:4251)
#ifndef PROMINECE3D_SDK
#define P3DDLL_API __declspec(dllexport)
#else
#define P3DDLL_API __declspec(dllimport)
#endif

#include <windows.h>
#include <d3dx9.h>
#include <vector>
#include <list>
#include <map>



namespace Prominence3D_SDK
{
	//�N���X�̑O���錾
	class P3DEffect;		//�\�����̃G�t�F�N�g�������N���X
	class P3DParticle;		//�\�����̃p�[�e�B�N���������N���X
	class P3DNode;			//���[�h���ꂽ�m�[�h�������N���X
	class P3DPrimitive;		//�~���┼���Ȃǂ̌`��������N���X


	///////////////////////////////////�@�y���C�u�����������p�\���́z�@///////////////////////////////////
	//P3DEngine�̃I�u�W�F�N�g���쐬����Ƃ��́A���̍\���̂ɕK�v�ȏ����܂Ƃ߂ēn���܂��B
	struct P3DInitData
	{
		//Direct3D�f�o�C�X�I�u�W�F�N�g�̃A�h���X
		LPDIRECT3DDEVICE9	pDevice;	

		//�~���⋅�̂��g�����Ƃ��̊��炩��
		//�ȗ������ꍇ��16�ɂȂ�܂��i�~����16�p���ɂȂ�j
		//�傫������Ί��炩�ɂȂ�܂����A���R�`��ɂ����鎞�Ԃ������Ȃ�܂�
		//�ŏ��l��3�A�ő�l��64�ł��B
		DWORD	polyDivisionNum;



		/*** ���̐ݒ荀�ڂ͍���̃o�[�W�����A�b�v�ő�����\�� ***/


		//�f�t�H���g�l
		P3DInitData()
		{
			pDevice = nullptr;
			polyDivisionNum = 16;
		}
	};



	///////////////////////////////////�@�y���C���̃N���X�z�@///////////////////////////////////
	class P3DDLL_API P3DEngine
	{
	public:
		/**
		* �R���X�g���N�^
		* @param  initData      �����������܂Ƃ߂��\����
		*/
		P3DEngine(P3DInitData &initData);


		/**
		* �f�X�g���N�^
		*/
		~P3DEngine();


		/**
		* �G�t�F�N�g�t�@�C���̓ǂݍ���
		* @param  fileName       �t�@�C�����i*.p3b�j
		* @return ���[�h�ł����S_OK�A���s�Ȃ�E_FAIL
		*/
		HRESULT Load(LPCSTR fileName);


		/**
		* �G�t�F�N�g����ʂɒǉ�
		* @param  worldPosition	�G�t�F�N�g�𔭐����������ʒu
		* @param  isLoop		���[�v���������ꍇ��TRUE
		* @return P3DEffect�^�̃A�h���X�B�ォ��ό`��������A�Đ��������f����Ƃ��Ɏg�p����B
		*/
		P3DEffect* Play(LPCSTR fileName, D3DXVECTOR3 &worldPosition, BOOL isLoop = FALSE);


		/**
		* �G�t�F�N�g�̈ʒu��ύX
		* @param  effect	�ύX�������G�t�F�N�g�̃A�h���X
		* @param  vec		�ʒu
		* @return ���������S_OK�A���s�i���łɍĐ����I����Ă�Ȃǁj�̏ꍇ��E_FAIL
		*/
		HRESULT SetTranslation(P3DEffect* effect, D3DXVECTOR3& vec);


		/**
		* �G�t�F�N�g����]
		* @param  effect	��]���������G�t�F�N�g�̃A�h���X
		* @param  vec		�w�C�x�C�y���ł̊p�x�i���W�A���j
		* @return ���������S_OK�A���s�i���łɍĐ����I����Ă�Ȃǁj�̏ꍇ��E_FAIL
		*/
		HRESULT SetRotation(P3DEffect* effect, D3DXVECTOR3& vec);


		/**
		* �G�t�F�N�g�̃T�C�Y��ύX
		* @param  effect	�ύX�������G�t�F�N�g�̃A�h���X
		* @param  vec		�w�C�x�C�y�����̊g�嗦
		* @return ���������S_OK�A���s�i���łɍĐ����I����Ă�Ȃǁj�̏ꍇ��E_FAIL
		*/
		HRESULT SetScaling(P3DEffect* effect, D3DXVECTOR3& vec);


		/**
		* �ǉ����ꂽ�S�ẴG�t�F�N�g��`��
		* @return ���������S_OK�A��肪�����E_FAIL
		*/
		HRESULT Draw();


		/**
		* �G�t�F�N�g���\���������ׂ�
		* @param  effect	���ׂ����G�t�F�N�g�̃A�h���X
		* @return �\�����Ȃ�TRUE�A����ȊO��FALSE
		*/
		BOOL IsPlayng(P3DEffect* effect);


		/**
		* �Đ����̃G�t�F�N�g���~�߂�i�����j
		* @param  effect	�~�߂����G�t�F�N�g�̃A�h���X
		* @return ���������S_OK�A�Đ����łȂ����E_FAIL
		*/
		HRESULT Stop(P3DEffect* effect);


	private:
		std::map<LPCTSTR, P3DNode*>	_nodeMap;			//���[�h�����t�@�C��
		std::list<P3DEffect*>		_particleManager;	//��ʂɕ\�����ꂽ�G�t�F�N�g
		std::vector<P3DPrimitive*>	_primitives;		//�l�p�`��~���Ȃǂ̊�{�}�`

		D3DXMATRIX	_matrixDir[7];	//6����(+�����_��)���̉�]�s��	
		LPD3DXEFFECT	_effect;	//�V�F�[�_�[
	};
}

using namespace Prominence3D_SDK;