/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"
#include "csmString.hpp"
#include "Utils/CubismDebug.hpp"

#ifndef NULL
#   define  NULL 0
#endif

//--------- LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework {

//========================テンプレートの宣言==============================

/**
 * @brief   Key-Valueのペアを定義するクラス。<br>
 *          csmMapクラスの内部データで使用する。
 */
template<class _KeyT, class _ValT>
class csmPair
{
public:

    /**
     * @brief   コンストラクタ
     */
    csmPair() : First()
              , Second() {}

    /**
     * @brief   引数付きコンストラクタ
     *
     * @param[in]   key ->  Keyとしてセットする値
     */
    csmPair(const _KeyT& key) : First(key)
                              , Second() {}

    /**
     * @brief   引数付きコンストラクタ
     *
     * @param[in]   key ->  Keyとしてセットする値
     * @param[in]   value   ->  Valueとしてセットする値
     */
    csmPair(const _KeyT& key, const _ValT& value): First(key)
                                                 , Second(value) {}

    /**
     * @brief   デストラクタ
     */
    virtual ~csmPair() {}

    _KeyT First;    ///< Keyとして用いる変数
    _ValT Second;   ///< Valueとして用いる変数
};

/**
 *@brief    マップ型<br>
 *           コンシューマゲーム機等でSTLの組み込みを避けるための実装。std::map の簡易版
 */
template<class _KeyT, class _ValT>
class csmMap
{
public:

    /**
     * @brief    コンストラクタ
     */
    csmMap();

    /**
     * @brief   引数付きコンストラクタ
     *
     * @param[in]   size    ->  初期化時点で確保するサイズ
     */
    csmMap(csmInt32 size);

    /**
     * @brief   デストラクタ
     *
     */
    virtual ~csmMap();

    /**
     * @brief   キーを追加する
     *
     * @param[in]   key ->  新たに追加するキー
     */
    void AppendKey(_KeyT& key)
    {
        // 新しくKey/Valueのペアを作る
        PrepareCapacity(_size + 1, false); //１つ以上入る隙間を作る
        // 新しいkey/valueのインデックスは _size

        void* addr = &_keyValues[_size];
        CSM_PLACEMENT_NEW(addr) csmPair<_KeyT, _ValT>(key); //placement new

        _size += 1;
    }

    /**
     * @brief   添字演算子[key]のオーバーロード
     *
     * @return  添字から特定されるValue値
     */
    _ValT& operator[](_KeyT key)
    {
        csmInt32 found = -1;
        for (csmInt32 i = 0; i < _size; i++)
        {
            if (_keyValues[i].First == key)
            {
                found = i;
                break;
            }
        }
        if (found >= 0)
        {
            return _keyValues[found].Second;
        }
        else
        {
            AppendKey(key); // 新規キーを追加
            return _keyValues[_size - 1].Second;
        }
    }

    /**
     * @brief   添字演算子[key]のオーバーロード(const)
     *
     * @return  添字から特定されるValue値
     */
    const _ValT& operator[](_KeyT key) const
    {
        csmInt32 found = -1;
        for (csmInt32 i = 0; i < _size; i++)
        {
            if (_keyValues[i].First == key)
            {
                found = i;
                break;
            }
        }
        if (found >= 0)
        {
            return _keyValues[found].Second;
        }
        else
        {
            if (!_dummyValuePtr) _dummyValuePtr = CSM_NEW _ValT();
            return *_dummyValuePtr;
        }
    }

    /**
     * @brief   引数で渡したKeyを持つ要素が存在するか
     *
     * @retval  true    ->  引数で渡したKeyを持つ要素が存在する
     * @retval  false   ->  引数で渡したKeyを持つ要素が存在しない
     */
    csmBool IsExist(_KeyT key)
    {
        for (csmInt32 i = 0; i < _size; i++)
        {
            if (_keyValues[i].First == key)
            {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief   Key-Valueのポインタを全て解放する
     */
    void Clear();

    /**
     * @brief   コンテナのサイズを取得する
     *
     * @return  コンテナのサイズ
     */
    csmInt32 GetSize() const { return _size; }

    /**
     * @brief   コンテナのキャパシティを確保する
     *
     * @param[in]   newSize     -> 新たなキャパシティ。引数の値が現在のサイズ未満の場合は何もしない。
     * @param[in]   fitToSize   ->  trueなら指定したサイズに合わせる。falseならサイズを2倍確保しておく。
     */
    void PrepareCapacity(csmInt32 newSize, csmBool fitToSize);

    /**
     * @brief   csmMap<T>のイテレータ
     */
    class iterator
    {
        // csmMap<T>をフレンドクラスとする
        friend class csmMap;

    public:
        /**
         * @brief   コンストラクタ
         *
         */
        iterator() : _index(0)
                   , _map(NULL) {}

        /**
         * @brief   引数付きコンストラクタ
         *
         * @param[in]   v   ->  csmMap<T>のオブジェクト
         *
         */
        iterator(csmMap<_KeyT, _ValT>* v) : _index(0)
                                          , _map(v) {}

        /**
         * @brief   引数付きコンストラクタ
         *
         * @param[in]   v   ->  csmMap<T>のオブジェクト
         * @param[in]   idx ->  コンテナから参照するインデックス値
         */
        iterator(csmMap<_KeyT, _ValT>* v, int idx) : _index(idx)
                                                   , _map(v) {}

        /**
         * @brief   =演算子のオーバーロード
         *
         */
        iterator& operator=(const iterator& ite)
        {
            this->_index = ite._index;
            this->_map = ite._map;
            return *this;
        }

        /**
         * @brief   前置++演算子のオーバーロード
         *
         */
        iterator& operator++()
        {
            // 前置
            this->_index++;
            return *this;
        }

        /**
         * @brief   前置--演算子のオーバーロード
         *
         */
        iterator& operator--()
        {
            // 前置
            this->_index--;
            return *this;
        }

        /**
         * @brief   後置++演算子のオーバーロード(intは後置用のダミー引数)
         *
         */
        iterator operator++(csmInt32)
        {
            // intは後置のためのダミー引数
            iterator iteold(this->_map, this->_index++); // 古い値を保存
            return iteold; // 古い値を返す
        }

        /**
         * @brief   後置--演算子のオーバーロード(intは後置用のダミー引数)
         *
         */
        iterator operator--(csmInt32)
        {
            // intは後置のためのダミー引数
            iterator iteold(this->_map, this->_index--); // 古い値を保存
            return iteold;
        }

        /**
         * @brief    *演算子のオーバーロード
         *
         */
        csmPair<_KeyT, _ValT>& operator*() const
        {
            return this->_map->_keyValues[this->_index];
        }

        /**
         * @brief   !=演算子のオーバーロード
         *
         */
        csmBool operator!=(const iterator& ite) const
        {
            return (this->_index != ite._index) || (this->_map != ite._map);
        }

    private:
        csmInt32 _index;                ///< コンテナのインデックス値
        csmMap<_KeyT, _ValT>* _map;     ///< コンテナのポインタ
    };

    /**
     * @brief   csmMap<T>のイテレータ(const)
     */
    class const_iterator
    {
        // csmMap<T>をフレンドクラスとする
        friend class csmMap;

    public:
        /**
         * @brief   コンストラクタ
         *
         */
        const_iterator() : _index(0)
                         , _map(NULL) {}

        /**
         * @brief   引数付きコンストラクタ
         *
         * @param[in]   v   ->  csmVector<T>のオブジェクト
         *
         */
        const_iterator(const csmMap<_KeyT, _ValT>* v) : _index(0)
                                                      , _map(v) {}

        /**
         * @brief   引数付きコンストラクタ
         *
         * @param[in]   v   ->  csmVector<T>のオブジェクト
         * @param[in]   idx ->  コンテナから参照するインデックス値
         */
        const_iterator(const csmMap<_KeyT, _ValT>* v, csmInt32 idx) : _index(idx)
                                                               , _map(v) {}

        /**
         * @brief   =演算子のオーバーロード
         *
         */
        const_iterator& operator=(const const_iterator& ite)
        {
            this->_index = ite._index;
            this->_map = ite._map;
            return *this;
        }

        /**
         * @brief   前置++演算子のオーバーロード
         *
         */
        const_iterator& operator++()
        {
            // 前置
            ++this->_index;
            return *this;
        }

        /**
         * @brief   前置--演算子のオーバーロード
         *
         */
        const_iterator& operator--()
        {
            // 前置
            --this->_index;
            return *this;
        }

        /**
         * @brief   後置++演算子のオーバーロード(intは後置用のダミー引数)
         *
         */
        const_iterator operator++(csmInt32)
        {
            // intは後置のためのダミー引数
            const_iterator iteold(this->_map, this->_index++); // 古い値を保存
            return iteold; // 古い値を返す
        }

        /**
         * @brief   後置--演算子のオーバーロード(intは後置用のダミー引数)
         *
         */
        const_iterator operator--(csmInt32)
        {
            // intは後置のためのダミー引数
            const_iterator iteold(this->_map, this->_index--); // 古い値を保存
            return iteold;
        }

        /**
         * @brief   ->演算子のオーバーロード
         *
         */
        csmPair<_KeyT, _ValT>* operator->() const
        {
            return &this->_map->_keyValues[this->_index];
        }

        /**
         * @brief    *演算子のオーバーロード
         *
         */
        csmPair<_KeyT, _ValT>& operator*() const
        {
            return this->_map->_keyValues[this->_index];
        }

        /**
         * @brief   !=演算子のオーバーロード
         *
         */
        csmBool operator!=(const const_iterator& ite) const
        {
            return (this->_index != ite._index) || (this->_map != ite._map);
        }

    private:
        csmInt32 _index;                    ///< コンテナのインデックス値
        const csmMap<_KeyT, _ValT>* _map;   ///< コンテナのポインタ(const)
    };

    /**
     * @brief   コンテナの先頭要素を返す
     *
     */
    const const_iterator Begin() const
    {
        const_iterator ite(this, 0);
        return ite;
    }

    /**
     * @bief    コンテナの終端要素を返す
     *
     */
    const const_iterator End() const
    {
        const_iterator ite(this, _size); // 終了
        return ite;
    }

    /**
     * @brief   コンテナから要素を削除する
     *
     * @param[in]   ite ->  削除する要素
     *
     */
    const iterator Erase(const iterator& ite)
    {
        int index = ite._index;
        if (index < 0 || _size <= index) return ite; // 削除範囲外

        // 削除(メモリをシフトする)、最後の一つを削除する場合はmove不要
        if (index < _size - 1)
            memmove(&(_keyValues[index]), &(_keyValues[index + 1]), sizeof(csmPair<_KeyT, _ValT>) * (_size - index - 1));
        --_size;

        iterator ite2(this, index); // 終了
        return ite2;
    }

    /**
     * @brief   コンテナから要素を削除する
     *
     * @param[in]   ite ->  削除する要素
     *
     */
    const const_iterator Erase(const const_iterator& ite)
    {
        csmInt32 index = ite._index;
        if (index < 0 || _size <= index) return ite; // 削除範囲外

        // 削除(メモリをシフトする)、最後の一つを削除する場合はmove不要
        if (index < _size - 1)
            memmove(&(_keyValues[index]), &(_keyValues[index + 1]), sizeof(csmPair<_KeyT, _ValT>) * (_size - index - 1));
        --_size;

        const_iterator ite2(this, index); // 終了
        return ite2;
    }

    /**
     * @brief   コンテナの値を32ビット符号付き整数型でダンプする
     *
     */
    void DumpAsInt()
    {
        for (csmInt32 i = 0; i < _size; i++) CubismLogDebug("%d ,", _keyValues[i]);
        CubismLogDebug("\n");
    }

private:
    static const csmInt32 DefaultSize = 10;  ///< コンテナ初期化のデフォルトサイズ

    csmPair<_KeyT, _ValT>* _keyValues;      ///< Key-Valueペアの配列
    _ValT* _dummyValuePtr;                  ///< 空の値を返すためのダミー(staticのtemplteを回避するためメンバとする）
    csmInt32 _size;                         ///< コンテナの要素数（サイズ）
    csmInt32 _capacity;                     ///< コンテナのキャパシティ
};


//========================テンプレートの定義==============================

template<class _KeyT, class _ValT>
csmMap<_KeyT, _ValT>::csmMap()
    : _keyValues(NULL)
    , _dummyValuePtr(NULL)
    , _size(0)
    , _capacity(0)
{ }

template<class _KeyT, class _ValT>
csmMap<_KeyT, _ValT>::csmMap(csmInt32 size)
    : _dummyValuePtr(NULL)
{
    if (size < 1)
    {
        _keyValues = NULL;
        _capacity = 0;
        _size = 0;
    }
    else
    {
        _keyValues = static_cast<csmPair<_KeyT, _ValT> *>(CSM_MALLOC(size * sizeof(csmPair<_KeyT, _ValT>)));

        CSM_ASSERT(_keyValues != NULL);

        // ここだけ calloc により、確保したバイトを0で埋める
        memset(_keyValues, 0, size * sizeof(csmPair<_KeyT, _ValT>));

        _capacity = size;
        _size = size;
    }
}

template<class _KeyT, class _ValT>
csmMap<_KeyT, _ValT>::~csmMap()
{
    Clear();
}

template<class _KeyT, class _ValT>
void csmMap<_KeyT, _ValT>::PrepareCapacity(csmInt32 newSize, csmBool fitToSize)
{
    if (newSize > _capacity)
    {
        if (_capacity == 0)
        {
            if (!fitToSize && newSize < DefaultSize) newSize = DefaultSize;

            _keyValues = static_cast<csmPair<_KeyT, _ValT> *>(CSM_MALLOC(sizeof(csmPair<_KeyT, _ValT>) * newSize));

            CSM_ASSERT(_keyValues != NULL);

            _capacity = newSize;
        }
        else
        {
            if (!fitToSize && newSize < _capacity * 2) newSize = _capacity * 2; // 指定サイズに合わせる必要がない場合は、２倍に広げる

            csmInt32 tmp_capacity = newSize;
            csmPair<_KeyT, _ValT>* tmp = static_cast<csmPair<_KeyT, _ValT> *>(CSM_MALLOC(sizeof(csmPair<_KeyT, _ValT>) * tmp_capacity));

            CSM_ASSERT(tmp != NULL);

            // 通常のMALLOCになったためコピーする
            memcpy(static_cast<void*>(tmp), static_cast<void*>(_keyValues), sizeof(csmPair<_KeyT, _ValT>) * _capacity);
            CSM_FREE(_keyValues);

            _keyValues = tmp; // そのまま
            _capacity = newSize;
        }
    }
    //  _size = newsize ;
}

template<class _KeyT, class _ValT>
void csmMap<_KeyT, _ValT>::Clear()
{
    if (_dummyValuePtr) CSM_DELETE(_dummyValuePtr);
    for (csmInt32 i = 0; i < _size; i++)
    {
        _keyValues[i].~csmPair<_KeyT, _ValT>();
    }

    CSM_FREE(_keyValues);

    _keyValues = NULL;

    _size = 0;
    _capacity = 0;
}
}}}

//------------------------- LIVE2D NAMESPACE ------------
