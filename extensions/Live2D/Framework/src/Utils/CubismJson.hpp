/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once
#include <stdio.h>
#include "CubismFramework.hpp"
#include "Type/csmVector.hpp"
#include "Type/csmMap.hpp"
#include "Type/csmString.hpp"

//------------ LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace Utils {
class Value;
class Error;
class NullValue;

#define CSM_JSON_ERROR_TYPE_MISMATCH            "Error:type mismatch"
#define CSM_JSON_ERROR_INDEX_OUT_OF_BOUNDS      "Error:index out of bounds"


/**
 * @brief  パースしたJSONエレメントの要素の基底クラス
 *
 */
class Value
{
    friend class Array;
    friend class Live2D::Cubism::Framework::CubismFramework;

public:
    static Value* ErrorValue; ///< 一時的な返り値として返すエラー。CubismFramework::Dispose()するまではCSM_DELETEしない。
    static Value* NullValue;  ///< 一時的な返り値として返すNULL。CubismFramework::Dispose()するまではCSM_DELETEしない。

    /**
     * @brief   コンストラクタ
     *
     */
    Value() {}

    /**
     * @brief   デストラクタ
     *
     */
    virtual ~Value() {}

    /**
     * @brief   要素を文字列で返す(csmString型)
     *
     */
    virtual const csmString& GetString(const csmString& defaultValue = "", const csmString& indent = "") = 0;

    /**
     * @brief   要素を文字列で返す(csmChar*)
     *
     */
    virtual const csmChar* GetRawString(const csmString& defaultValue = "", const csmString& indent = "")
    {
        return this->GetString(defaultValue, indent).GetRawString();
    }

    /**
     * @brief   要素を数値型で返す(csmInt32)
     *
     */
    virtual csmInt32 ToInt(csmInt32 defaultValue = 0) { return defaultValue; }

    /**
     * @brief   要素を数値型で返す(csmFloat32)
     *
     */
    virtual csmFloat32 ToFloat(csmFloat32 defaultValue = 0.0f) { return defaultValue; }

    /**
     * @brief   要素を真偽値で返す(csmBool)
     *
     */
    virtual csmBool ToBoolean(csmBool defaultValue = false) { return defaultValue; }

    /**
     * @brief   要素を真偽値で返す(csmBool)
     *
     */
    virtual csmInt32 GetSize() { return 0; }

    /**
     * @brief   要素をコンテナで返す(csmVector<Value*>)
     *
     */
    virtual csmVector<Value*>* GetVector(csmVector<Value*>* defaultValue = NULL) {
        if (!defaultValue)
        {
            defaultValue = new csmVector<Value*>;
        }
        return defaultValue;
    }

    /**
     * @brief   要素をマップで返す(csmMap<csmString, Value*>)
     *
     */
    virtual csmMap<csmString, Value*>* GetMap(csmMap<csmString, Value*>* defaultValue = NULL) { return defaultValue; }

    /**
     * @brief   添字演算子[csmInt32]
     *
     */
    virtual Value& operator[](csmInt32 index)
    {
        return *(ErrorValue->SetErrorNotForClientCall(CSM_JSON_ERROR_TYPE_MISMATCH));
    }

    /**
     * @brief   添字演算子[csmString]
     *
     */
    virtual Value& operator[](const csmString& string)
    {
        return *(NullValue->SetErrorNotForClientCall(CSM_JSON_ERROR_TYPE_MISMATCH));
    }

    /**
     * @brief   添字演算子[csmChar*]
     *
     */
    virtual Value& operator[](const csmChar* s)
    {
        return *(NullValue->SetErrorNotForClientCall(CSM_JSON_ERROR_TYPE_MISMATCH));
    }

    /**
     * @brief   マップのキー一覧をコンテナで返す
     *
     * @return  マップのキー一覧
     */
    virtual csmVector<csmString>& GetKeys()
    {
        return *s_dummyKeys;
    }

    /**
     *@brief Valueの種類がエラー値ならtrue。
     */
    virtual csmBool IsError() { return false; }

    /**
     *@brief Valueの種類がNULL値ならtrue。
     */
    virtual csmBool IsNull() { return false; }

    /**
     *@brief Valueの種類が真偽値ならtrue。
     */
    virtual csmBool IsBool() { return false; }

    /**
     *@brief Valueの種類が数値型ならtrue。
     */
    virtual csmBool IsFloat() { return false; }

    /**
     *@brief Valueの種類が文字列ならtrue。
     */
    virtual csmBool IsString() { return false; }

    /**
     *@brief Valueの種類が配列ならtrue。
     */
    virtual csmBool IsArray() { return false; }

    /**
     *@brief Valueの種類がマップ型ならtrue。
     */
    virtual csmBool IsMap() { return false; }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(const csmString& value) { return false; }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(const csmChar* value) { return false; }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(csmInt32 value) { return false; }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(csmFloat32 value) { return false; }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(csmBool value) { return false; }

    /**
     *@brief Valueの値が静的ならtrue. 静的なら解放しない
     */
    virtual csmBool IsStatic() { return false; }

    /**
     *@brief Valueにエラー値をセットする
     */
    virtual Value* SetErrorNotForClientCall(const csmChar* errorStr) {
        this->_stringBuffer = errorStr;
        return NullValue;
    }

protected:
    csmString _stringBuffer;        ///< 文字列バッファ

private:
    static csmVector<csmString>* s_dummyKeys;    ///< ダミーキー

    /**
    * @brief    初期化用メソッド
    */
    static void StaticInitializeNotForClientCall();

    /**
    * @brief   リリース用メソッド
    */
    static void StaticReleaseNotForClientCall();


};

/**
 * @brief   Ascii文字のみ対応した最小限の軽量JSONパーサ。<br>
 *           仕様はJSONのサブセットとなる。<br>
 *           設定ファイル(model3.json)などのロード用<br>
 *           <br>
 *           [未対応項目]<br>
 *           ・日本語などの非ASCII文字<br>
 *           ・e による指数表現
 */
class CubismJson
{
public:
    /**
     * @brief  バイトデータから直接ロードしてパースする<br>
     *          引数 buffer は外部で管理（破棄）する必要がある
     *
     * @param   buffer  ->  バイトデータのバッファ
     * @param   size    ->  バッファサイズ
     * @return  CubismJsonクラスのインスタンス。失敗したらNULL。
     */
    static CubismJson* Create(const csmByte* buffer, csmSizeInt size);

    /**
    * @brief   パースしたJSONオブジェクトの解放処理
    *
    * @param[in] instance CubismJsonクラスのインスタンス
    *
    */
    static void Delete(CubismJson* instance);

    /**
     * @brief   パースしたJSONのルート要素のポインタを返す
     *
     */
    Value& GetRoot() const;

    /**
     * @brief   パース時のエラー値を返す
     *
     */
    const csmChar* GetParseError() const { return _error; }

    /**
     * @brief   ルート要素の次の要素がファイルの終端だったらtrueを返す
     *
     */
    csmBool CheckEndOfFile() const { return (*_root)[1].Equals("EOF"); }

protected:
    /**
     * @brief JSONのパースを実行する
     *
     * @param[in]   buffer  ->  パース対象のデータバイト
     * @param[in]   size    ->  データバイトのサイズ
     * @retval      true    ->  成功
     * @retval      false   ->  失敗
     */
    csmBool ParseBytes(const csmByte* buffer, csmInt32 size);

    /**
     * @brief   次の「"」までの文字列をパースする。文字列は外部で解放する必要がある。
     *
     * @param[in]   string  ->  パース対象の文字列
     * @param[in]   length  ->  パースする長さ
     * @param[in]   begin   ->  パースを開始する位置
     * @param[out]  outEndPos   ->  パース終了時の位置
     * @return      パースした文字列要素
     */
    csmString ParseString(const csmChar* string, csmInt32 length, csmInt32 begin, csmInt32* outEndPos);

    /**
     * @brief   数値をパースする。ロケール設定にかかわらず、小数点の区切り文字を . としてパースする。
     *
     * @param[in]   buffer  ->  JSONエレメントのバッファ
     * @param[in]   length  ->  パースする長さ
     * @param[in]   begin   ->  パースを開始する位置
     * @param[out]  outEndPos   ->  パース終了時の位置
     * @return      パースから取得したValueオブジェクト
     */
    Value* ParseNumeric(const csmChar* buffer, csmInt32 length, csmInt32 begin, csmInt32* outEndPos);

    /**
     * @brief   JSONのオブジェクトエレメントをパースしてValueオブジェクトを返す
     *
     * @param[in]   buffer  ->  JSONエレメントのバッファ
     * @param[in]   length  ->  パースする長さ
     * @param[in]   begin   ->  パースを開始する位置
     * @param[out]  outEndPos   ->  パース終了時の位置
     * @return      パースから取得したValueオブジェクト
     */
    Value* ParseObject(const csmChar* buffer, csmInt32 length, csmInt32 begin, csmInt32* outEndPos);

    /**
     * @brief   JSONの配列エレメントをパースしてValueオブジェクトを返す
     *
     * @param[in]   buffer  ->  JSONエレメントのバッファ
     * @param[in]   length  ->  パースする長さ
     * @param[in]   begin   ->  パースを開始する位置
     * @param[out]  outEndPos   ->  パース終了時の位置
     * @return      パースから取得したValueオブジェクト
     */
    Value* ParseArray(const csmChar* buffer, csmInt32 length, csmInt32 begin, csmInt32* outEndPos);

    /**
     * @brief   JSONエレメントからValue(float,String,Value*,Array,null,true,false)をパースする<br>
     *           エレメントの書式に応じて内部でParseString(), ParseObject(), ParseArray()を呼ぶ<br>
     *
     * @param[in]   buffer  ->  JSONエレメントのバッファ
     * @param[in]   length  ->  パースする長さ
     * @param[in]   begin   ->  パースを開始する位置
     * @param[out]  outEndPos   ->  パース終了時の位置
     * @return      パースから取得したValueオブジェクト
     */
    Value* ParseValue(const csmChar* buffer, csmInt32 length, csmInt32 begin, csmInt32* outEndPos);

private:
    /**
    * @brief   コンストラクタ
    *
    */
    CubismJson();

    /**
    * @brief   引数付きコンストラクタ
    * @param[in]   buffer  ->  JSONエレメントのバッファ
    * @param[in]   length  ->  パースする長さ
    */
    CubismJson(const csmByte* buffer, csmInt32 length);

    /**
    * @brief   デストラクタ
    *
    */
    virtual ~CubismJson();

    const csmChar*  _error;         ///< パース時のエラー
    csmInt32        _lineCount;     ///< エラー報告に用いる行数カウント
    Value*          _root;          ///< パースされたルート要素
};


/**
 *@brief パースしたJSONの要素をDouble値として扱う
 */
class Float : public Value
{
public:
    /**
     * @brief   コンストラクタ
     */
    Float(csmFloat32 v) : Value()
    {
        this->_value = v;
    }

    /**
     * @brief   デストラクタ
     */
    virtual ~Float() {}

    /**
     *@brief Valueの種類が数値型ならtrue。
     */
    virtual csmBool IsFloat() { return true; }

    /**
     * @brief   要素を文字列で返す(csmString型)
     *
     */
    virtual const csmString& GetString(const csmString& defaultValue = "", const csmString& indent = "")
    {
#if defined(CSM_TARGET_WIN_GL) || defined(_MSC_VER)
        csmChar strbuf[32] = {'\0'};
        _snprintf_s(strbuf, 32, 32, "%f", this->_value);
        _stringBuffer = csmString(strbuf);
        return _stringBuffer;
#else
        // string stream 未対応
        csmChar strbuf[32] = { '\0' };
        snprintf(strbuf, 32, "%f", this->_value);
        _stringBuffer = csmString(strbuf);
        return _stringBuffer;
#endif
    }

    /**
     * @brief   要素を数値型で返す(csmInt32)
     */
    virtual csmInt32 ToInt(csmInt32 defaultValue = 0) { return static_cast<csmInt32>(this->_value); }

    /**
     * @brief   要素を数値型で返す(csmFloat32)
     */
    virtual csmFloat32 ToFloat(csmFloat32 defaultValue = 0) { return this->_value; }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(csmFloat32 v) { return v == this->_value; }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(const csmString& v) { return false; }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(const csmChar* v) { return false; }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(csmInt32 v) { return false; }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(csmBool v) { return false; }

private:
    csmFloat32 _value;  ///< JSON要素の値
};


/**
 * @brief   パースしたJSONの要素を真偽値として扱う
 *
 */
class Boolean : public Value
{
    friend class Value;

public:
    static Boolean* TrueValue;  ///< true
    static Boolean* FalseValue; ///< false

    /**
     * @brief   デストラクタ
     *
     */
    virtual ~Boolean() {}

    /**
     *@brief Valueの種類が真偽値ならtrue。
     */
    virtual csmBool IsBool() { return true; }

    /**
     * @brief   要素を真偽値で返す(csmBool)
     */
    virtual csmBool ToBoolean(csmBool defaultValue = false) { return _boolValue; }

    /**
     * @brief   要素を文字列で返す(csmString型)
     */
    virtual const csmString& GetString(const csmString& defaultValue = "", const csmString& indent = "")
    {
        _stringBuffer = csmString(_boolValue ? "true" : "false");
        return _stringBuffer;
    }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(csmBool v) { return v == _boolValue; }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(const csmString& v) { return false; }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(const csmChar* v) { return false; }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(csmInt32 v) { return false; }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(csmFloat32 v) { return false; }

    /**
     *@brief Valueの値が静的ならtrue. 静的なら解放しない
     */
    virtual csmBool IsStatic() { return true; }

private:
    /**
     * @brief   引数付きコンストラクタ
     */
    Boolean(csmBool v) : Value() { this->_boolValue = v; }
    csmBool _boolValue;     ///< JSON要素の値
};


/**
 * @brief   パースしたJSONの要素を文字列として扱う
 *
 */
class String : public Value
{
public:
    /**
     * @brief   引数付きコンストラクタ
     */
    String(const csmString& s) : Value() { this->_stringBuffer = s; }

    /**
     * @brief   引数付きコンストラクタ
     */
    String(const csmChar* s) : Value() { this->_stringBuffer = s; }

    /**
     * @brief   デストラクタ
     */
    virtual ~String() {}

    /**
     *@brief Valueの種類が文字列ならtrue。
     */
    virtual csmBool IsString() { return true; }

    /**
     * @brief   要素を文字列で返す(csmString型)
     */
    virtual const csmString& GetString(const csmString& defaultValue = "", const csmString& indent = "")
    {
        return _stringBuffer;
    }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(const csmString& v) { return (_stringBuffer == v); }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(const csmChar* v) { return (_stringBuffer == v); }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(csmInt32 v) { return false; }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(csmFloat32 v) { return false; }

    /**
     *@brief 引数の値と等しければtrue。
     */
    virtual csmBool Equals(csmBool v) { return false; }
};


/**
 * @brief  JSONパース時のエラー結果。文字列型のように振る舞う
 *
 */
class Error : public String
{
    friend class Value; //
    friend class Array; //
    friend class CubismJson; //

public:
    /**
     *@brief Valueの値が静的ならtrue. 静的なら解放しない
     */
    virtual csmBool IsStatic() { return _isStatic; }

    /**
    * @brief    エラー情報をセットする
    */
    virtual Value* SetErrorNotForClientCall(const csmChar* s)
    {
        this->_stringBuffer = s;
        return this;
    }

protected:
    /**
     * @brief   引数付きコンストラクタ
     */
    Error(const csmString& s, csmBool isStatic) : String(s)
                                                , _isStatic(isStatic) {}

    /**
     * @brief   デストラクタ
     */
    virtual ~Error() {}

    /**
     *@brief Valueの種類がエラー値ならtrue。
     */
    virtual csmBool IsError() { return true; }

    csmBool _isStatic; ///< 静的なValueかどうか
};


/**
 * @brief   パースしたJSONの要素をNull値として持つ
 */
class NullValue : public Value
{
    friend class Value; //
    friend class CubismJson; //

public:
    /**
     * @brief   デストラクタ
     */
    virtual ~NullValue() {}

    /**
     *@brief Valueの種類がNULL値ならtrue。
     */
    virtual csmBool IsNull() { return true; }

    /**
     * @brief   要素を文字列で返す(csmString型)
     */
    virtual const csmString& GetString(const csmString& defaultValue = "", const csmString& indent = "")
    {
        return _stringBuffer;
    }

    /**
     *@brief Valueの値が静的ならtrue. 静的なら解放しない
     */
    virtual csmBool IsStatic() { return true; }

private:
    /**
     * @brief    コンストラクタ
     */
    NullValue() : Value() { _stringBuffer = "NullValue"; }
};


/**
 * @brief   パースしたJSONの要素を配列として持つ
 *
 */
class Array : public Value
{
public:
    /**
     * @brief    コンストラクタ
     */
    Array() : Value()
            , _array() {}

    /**
     * @brief   デストラクタ
     */
    virtual ~Array();

    /**
     *@brief Valueの種類が配列ならtrue。
     */
    virtual csmBool IsArray() { return true; }

    /**
     * @brief   添字演算子[csmInt32]
     *
     */
    virtual Value& operator[](csmInt32 index)
    {
        if (index < 0 || (static_cast<csmInt32>(_array.GetSize()) <= index))
            return *(ErrorValue->SetErrorNotForClientCall(CSM_JSON_ERROR_INDEX_OUT_OF_BOUNDS));
        Value* v = _array[index];

        if (v == NULL) return *Value::NullValue;
        return *v;
    }

    /**
     * @brief   添字演算子[csmString]
     *
     */
    virtual Value& operator[](const csmString& string)
    {
        return *(ErrorValue->SetErrorNotForClientCall(CSM_JSON_ERROR_TYPE_MISMATCH));
    }

    /**
     * @brief   添字演算子[csmChar*]
     *
     */
    virtual Value& operator[](const csmChar* s)
    {
        return *(ErrorValue->SetErrorNotForClientCall(CSM_JSON_ERROR_TYPE_MISMATCH));
    }

    /**
     * @brief   要素を文字列で返す(csmString型)
     *
     */
    virtual const csmString& GetString(const csmString& defaultValue = "", const csmString& indent = "")
    {
        _stringBuffer = indent + "[\n";
        csmVector<Value*>::iterator ite = _array.Begin();
        for (; ite != _array.End(); ite++)
        {
            Value* v = (*ite);
            _stringBuffer += indent + "	" + v->GetString(indent + "	") + "\n";
        }
        _stringBuffer += indent + "]\n";

        return _stringBuffer;
    }

    /**
     * @brief   配列要素を追加する
     */
    void Add(Value* v) { _array.PushBack(v, false); }

    /**
     * @brief   要素をコンテナで返す(csmVector<Value*>)
     *
     */
    virtual csmVector<Value*>* GetVector(csmVector<Value*>* defaultValue = NULL) { return &_array; }

    /**
     * @brief   要素の数を返す
     *
     */
    virtual csmInt32 GetSize() { return static_cast<csmInt32>(_array.GetSize()); }

private:
    csmVector<Value*> _array; ///< JSON要素の値
};


/**
 * @brief   パースしたJSONの要素をマップとして持つ
 *
 */
class Map : public Value
{
public:
    /**
     * @brief    コンストラクタ
     */
    Map() : Value()
          , _keys(NULL) {}

    /**
     * @brief    デストラクタ
     */
    virtual ~Map();

    /**
     * @brief    Valueの値がMap型ならtrue
     */
    virtual csmBool IsMap() { return true; }

    /**
     * @brief    添字演算子[csmString]
     */
    virtual Value& operator[](const csmString& s)
    {
        Value* ret = _map[s];
        if (ret == NULL)
        {
            return *Value::NullValue;
        }
        return *ret;
    }

    /**
     * @brief   添字演算子[csmChar*]
     *
     */
    virtual Value& operator[](const csmChar* s)
    {
        for (csmMap<csmString, Value*>::const_iterator iter = _map.Begin(); iter != _map.End(); ++iter)
        {
            if( strcmp(iter->First.GetRawString(), s) == 0 )
            {
                if(iter->Second==NULL)
                {
                    return *Value::NullValue;
                }
                return *iter->Second;
            }
        }

        return *Value::NullValue;
    }

    /**
     * @brief    添字演算子[csmInt32]
     */
    virtual Value& operator[](csmInt32 index)
    {
        return *(ErrorValue->SetErrorNotForClientCall(CSM_JSON_ERROR_TYPE_MISMATCH));
    }

    virtual const csmString& GetString(const csmString& defaultValue = "", const csmString& indent = "")
    {
        _stringBuffer = indent + "{\n";
        csmMap<csmString, Value*>::const_iterator ite = _map.Begin();
        while (ite != _map.End())
        {
            const csmString& key = (*ite).First;
            Value* v = (*ite).Second;

            _stringBuffer += indent + "	" + key + " : " + v->GetString(indent + "	") + "\n";
            ++ite;
        }
        _stringBuffer += indent + "}\n";
        return _stringBuffer;
    }

    /**
     * @brief    要素をMap型で返す
     */
    virtual csmMap<csmString, Value*>* GetMap(csmMap<csmString, Value*>* defaultValue = NULL)
    {
        return &_map;
    }

    /**
     * @brief    Mapに要素を追加する
     */
    void Put(csmString& key, Value* v)
    {
        _map[key] = v;
    }

    /**
     * @brief    Mapからキーのリストを取得する
     */
    virtual csmVector<csmString>& GetKeys()
    {
        if (!_keys)
        {
            _keys = CSM_NEW csmVector<csmString>();
            csmMap<csmString, Value*>::const_iterator ite = _map.Begin();
            while (ite != _map.End())
            {
                const csmString& key = (*ite).First;
                _keys->PushBack(key, true);
                ++ite;
            }
        }
        return *_keys;
    }

    /**
     * @brief    Mapの要素数を取得する
     */
    virtual csmInt32 GetSize() { return static_cast<csmInt32>(_keys->GetSize()); }

private:
    csmMap<csmString, Value*> _map;     ///< JSON要素の値
    csmVector<csmString>* _keys;        ///< JSON要素の値
};
}}}}

//------------ LIVE2D NAMESPACE ------------
