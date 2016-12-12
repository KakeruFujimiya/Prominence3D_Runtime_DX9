//-----------------------------------------------------------------------------
// Prominence3D Runtime for DirectX9
//
// Copyright(C) 藤宮翔流 All Rights Reserved.
// https://github.com/KakeruFujimiya/Prominence3D_Runtime_DX9
//-----------------------------------------------------------------------------
//
// P3DEnginr.h
//

//-----------------------------------------------------------------------------
// インクルード
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
	//クラスの前方宣言
	class P3DEffect;		//表示中のエフェクトを扱うクラス
	class P3DParticle;		//表示中のパーティクルを扱うクラス
	class P3DNode;			//ロードされたノードを扱うクラス
	class P3DPrimitive;		//円柱や半球などの形状を扱うクラス


	///////////////////////////////////　【ライブラリ初期化用構造体】　///////////////////////////////////
	//P3DEngineのオブジェクトを作成するときは、この構造体に必要な情報をまとめて渡します。
	struct P3DInitData
	{
		//Direct3Dデバイスオブジェクトのアドレス
		LPDIRECT3DDEVICE9	pDevice;	

		//円柱や球体を使ったときの滑らかさ
		//省略した場合は16になります（円柱が16角柱になる）
		//大きくすれば滑らかになりますが、当然描画にかかる時間が長くなります
		//最小値は3、最大値は64です。
		DWORD	polyDivisionNum;



		/*** この設定項目は今後のバージョンアップで増える予定 ***/


		//デフォルト値
		P3DInitData()
		{
			pDevice = nullptr;
			polyDivisionNum = 16;
		}
	};



	///////////////////////////////////　【メインのクラス】　///////////////////////////////////
	class P3DDLL_API P3DEngine
	{
	public:
		/**
		* コンストラクタ
		* @param  initData      初期化情報をまとめた構造体
		*/
		P3DEngine(P3DInitData &initData);


		/**
		* デストラクタ
		*/
		~P3DEngine();


		/**
		* エフェクトファイルの読み込み
		* @param  fileName       ファイル名（*.p3b）
		* @return ロードできればS_OK、失敗ならE_FAIL
		*/
		HRESULT Load(LPCSTR fileName);


		/**
		* エフェクトを画面に追加
		* @param  worldPosition	エフェクトを発生させたい位置
		* @param  isLoop		ループさせたい場合はTRUE
		* @return P3DEffect型のアドレス。後から変形させたり、再生中か判断するときに使用する。
		*/
		P3DEffect* Play(LPCSTR fileName, D3DXVECTOR3 &worldPosition, BOOL isLoop = FALSE);


		/**
		* エフェクトの位置を変更
		* @param  effect	変更したいエフェクトのアドレス
		* @param  vec		位置
		* @return 成功すればS_OK、失敗（すでに再生が終わってるなど）の場合はE_FAIL
		*/
		HRESULT SetTranslation(P3DEffect* effect, D3DXVECTOR3& vec);


		/**
		* エフェクトを回転
		* @param  effect	回転させたいエフェクトのアドレス
		* @param  vec		Ｘ，Ｙ，Ｚ軸での角度（ラジアン）
		* @return 成功すればS_OK、失敗（すでに再生が終わってるなど）の場合はE_FAIL
		*/
		HRESULT SetRotation(P3DEffect* effect, D3DXVECTOR3& vec);


		/**
		* エフェクトのサイズを変更
		* @param  effect	変更したいエフェクトのアドレス
		* @param  vec		Ｘ，Ｙ，Ｚ方向の拡大率
		* @return 成功すればS_OK、失敗（すでに再生が終わってるなど）の場合はE_FAIL
		*/
		HRESULT SetScaling(P3DEffect* effect, D3DXVECTOR3& vec);


		/**
		* 追加された全てのエフェクトを描画
		* @return 成功すればS_OK、問題があればE_FAIL
		*/
		HRESULT Draw();


		/**
		* エフェクトが表示中か調べる
		* @param  effect	調べたいエフェクトのアドレス
		* @return 表示中ならTRUE、それ以外はFALSE
		*/
		BOOL IsPlayng(P3DEffect* effect);


		/**
		* 再生中のエフェクトを止める（消す）
		* @param  effect	止めたいエフェクトのアドレス
		* @return 成功すればS_OK、再生中でなければE_FAIL
		*/
		HRESULT Stop(P3DEffect* effect);


	private:
		std::map<LPCTSTR, P3DNode*>	_nodeMap;			//ロードしたファイル
		std::list<P3DEffect*>		_particleManager;	//画面に表示されたエフェクト
		std::vector<P3DPrimitive*>	_primitives;		//四角形や円柱などの基本図形

		D3DXMATRIX	_matrixDir[7];	//6方向(+ランダム)分の回転行列	
		LPD3DXEFFECT	_effect;	//シェーダー
	};
}

using namespace Prominence3D_SDK;