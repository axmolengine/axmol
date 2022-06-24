/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismJson.hpp"
#include <stdlib.h>
#include "Type/csmString.hpp"
#include "CubismDebug.hpp"

using namespace std; // for strtof

//------------ LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace Utils {

//StaticInitializeNotForClientCall()で初期化する
Boolean* Boolean::TrueValue = NULL;
Boolean* Boolean::FalseValue = NULL;
Value* Value::ErrorValue = NULL;
Value* Value::NullValue = NULL;
csmVector<csmString>* Value::s_dummyKeys = NULL;

void Value::StaticReleaseNotForClientCall()
{
    CSM_DELETE(Boolean::TrueValue);
    CSM_DELETE(Boolean::FalseValue);
    CSM_DELETE(Value::ErrorValue);
    CSM_DELETE(Value::NullValue);
    CSM_DELETE(Value::s_dummyKeys);

    Boolean::TrueValue = NULL;
    Boolean::FalseValue = NULL;
    Value::ErrorValue = NULL;
    Value::NullValue = NULL;
    Value::s_dummyKeys = NULL;
}

void Value::StaticInitializeNotForClientCall()
{
    Boolean::TrueValue = CSM_NEW Boolean(true);
    Boolean::FalseValue = CSM_NEW Boolean(false);

    Value::ErrorValue = CSM_NEW Error("ERROR", true);
    Value::NullValue = CSM_NEW Utils::NullValue();

    Value::s_dummyKeys = CSM_NEW csmVector<csmString>();
}

CubismJson::CubismJson()
    : _error(NULL)
    , _lineCount(0)
    , _root(NULL)
{ }

CubismJson::CubismJson(const csmByte* buffer, csmInt32 length)
    : _error(NULL)
    , _lineCount(0)
    , _root(NULL)
{
    ParseBytes(buffer, length);
}

CubismJson::~CubismJson()
{
    if (_root && !_root->IsStatic())
    {
        CSM_DELETE(_root);
    }

    _root = NULL;
}

void CubismJson::Delete(CubismJson* instance)
{
    CSM_DELETE_SELF(CubismJson, instance);
}


CubismJson* CubismJson::Create(const csmByte* buffer, csmSizeInt size)
{
    CubismJson* json = CSM_NEW CubismJson();
    const csmBool succeeded = json->ParseBytes(buffer, size);

    if (!succeeded)
    {
        CubismJson::Delete(json);
        return NULL;
    }
    else
    {
        return json;
    }
}


Value& CubismJson::GetRoot() const
{
    return *_root;
}


csmBool CubismJson::ParseBytes(const csmByte* buffer, csmInt32 size)
{
    csmInt32 endPos;
    _root = ParseValue(reinterpret_cast<const csmChar*>(buffer), size, 0, &endPos);

    if (_error)
    {
#if defined(CSM_TARGET_WIN_GL) || defined(_MSC_VER)
        csmChar strbuf[256] = {'\0'};
        _snprintf_s(strbuf, 256, 256, "Json parse error : @line %d\n", (_lineCount + 1));
        _root = CSM_NEW String(strbuf);
#else
        csmChar strbuf[256] = { '\0' };
        snprintf(strbuf, 256, "Json parse error : @line %d\n", (_lineCount + 1));
        _root = CSM_NEW String(strbuf);
#endif
        CubismLogInfo("%s", _root->GetRawString());
        return false;
    }
    else if (_root == NULL)
    {
        _root = CSM_NEW Error(_error, false); //rootは開放されるのでエラーオブジェクトを別途作る
        return false;
    }
    return true;
}


csmString CubismJson::ParseString(const csmChar* string, csmInt32 length, csmInt32 begin, csmInt32* outEndPos)
{
    if (_error) return NULL;
    csmInt32 i = begin;
    csmChar c, c2;
    csmString ret;
    csmInt32 buf_start = begin; //sbufに登録されていない文字の開始位置

    for (; i < length; i++)
    {
        c = static_cast<csmChar>(string[i] & 0xFF);

        switch (c)
        {
        case '\"': {//終端の”, エスケープ文字は別に処理されるのでここにはこない
            *outEndPos = i + 1; // ”の次の文字
            ret.Append(static_cast<const csmChar*>(string + buf_start), (i - buf_start));
            return ret;
        }
        case '\\': {//エスケープの場合
            i++; //２文字をセットで扱う

            if (i - 1 > buf_start) ret.Append(static_cast<const csmChar*>(string + buf_start), (i - buf_start - 1)); //前の文字までを登録する
            buf_start = i + 1; //エスケープ（２文字）の次の文字から

            if (i < length)
            {
                c2 = static_cast<csmChar>(string[i] & 0xFF);
                switch (c2)
                {
                case '\\': ret.Append(1, '\\');
                    break;
                case '\"': ret.Append(1, '\"');
                    break;
                case '/': ret.Append(1, '/');
                    break;

                case 'b': ret.Append(1, '\b');
                    break;
                case 'f': ret.Append(1, '\f');
                    break;
                case 'n': ret.Append(1, '\n');
                    break;
                case 'r': ret.Append(1, '\r');
                    break;
                case 't': ret.Append(1, '\t');
                    break;
                case 'u':
                    _error = "parse string/unicode escape not supported";
                default:
                    break;
                }
            }
            else
            {
                _error = "parse string/escape error";
            }
            break;
        }
        default: {
            break;
        }
        }
    }
    _error = "parse string/illegal end";
    return NULL;
}


Value* CubismJson::ParseObject(const csmChar* buffer, csmInt32 length, csmInt32 begin, csmInt32* outEndPos)
{
    if (_error) return NULL;
    Map* ret = CSM_NEW Map();

    //key : value ,
    csmString key;
    csmInt32 i = begin;
    csmChar c;
    csmInt32 local_ret_endpos2[1];
    csmBool ok = false;

    // , が続く限りループ
    for (; i < length; i++)
    {
        for (; i < length; i++)
        {
            c = static_cast<csmChar>(buffer[i] & 0xFF);

            switch (c)
            {
            case '\"':
                key = ParseString(buffer, length, i + 1, local_ret_endpos2);
                if (_error) return NULL;
                i = local_ret_endpos2[0];
                ok = true;
                goto BREAK_LOOP1; //-- loopから出る
            case '}': //閉じカッコ
                *outEndPos = i + 1;
                return ret; //空
            case ':':
                _error = "illegal ':' position";
                break;
            case '\n': _lineCount++;
            default: break; //スキップする文字
            }
        }
    BREAK_LOOP1:
        if (!ok)
        {
            _error = "key not found";
            return NULL;
        }

        ok = false;

        // : をチェック
        for (; i < length; i++)
        {
            c = static_cast<csmChar>(buffer[i] & 0xFF);

            switch (c)
            {
            case ':': ok = true;
                i++;
                goto BREAK_LOOP2; //-- loopから出る

            case '}':
                _error = "illegal '}' position";
                break;
            case '\n': _lineCount++;
                //case ' ': case '\t': case '\r':
            default: break; //スキップする文字
            }
        }
    BREAK_LOOP2:

        if (!ok)
        {
            _error = "':' not found";
            return NULL;
        }

        // 値をチェック
        Value* value = ParseValue(buffer, length, i, local_ret_endpos2);
        if (_error) return NULL;
        i = local_ret_endpos2[0];
        // ret.put( key , value ) ;
        ret->Put(key, value);

        for (; i < length; i++)
        {
            c = static_cast<csmChar>(buffer[i] & 0xFF);

            switch (c)
            {
            case ',':
                goto BREAK_LOOP3;
            case '}':
                *outEndPos = i + 1;
                return ret; // << [] 正常終了 >>
            case '\n': _lineCount++;
                //case ' ': case '\t': case '\r':
            default: break; //スキップ
            }
        }
    BREAK_LOOP3:
        ; //dummy
    }

    _error = "illegal end of parseObject";
    return NULL;
}


Value* CubismJson::ParseArray(const csmChar* buffer, csmInt32 length, csmInt32 begin, csmInt32* outEndPos)
{
    if (_error) return NULL;
    Array* ret = CSM_NEW Array();

    //key : value ,
    csmInt32 i = begin;
    csmChar c;
    csmInt32 local_ret_endpos2[1];

    // , が続く限りループ
    for (; i < length; i++)
    {
        // : をチェック
        Value* value = ParseValue(buffer, length, i, local_ret_endpos2);
        if (_error) return NULL;
        i = local_ret_endpos2[0];
        if (value)
        {
            ret->Add(value);
        }

        //FOR_LOOP3:
        //bool breakflag = false;
        for (; i < length; i++)
        {
            c = static_cast<csmChar>(buffer[i] & 0xFF);

            switch (c)
            {
            case ',':
                // breakflag = true;
                // break;//次のKEY,VALUEへ
                goto BREAK_LOOP3;
            case ']':
                *outEndPos = i + 1;
                return ret; //終了
            case '\n': ++_lineCount;
                //case ' ': case '\t': case '\r':
            default: break; //スキップ
            }
        }
    BREAK_LOOP3:
        ; //dummy
    }

    CSM_DELETE(ret);
    _error = "illegal end of parseObject";
    return NULL;
}


Value* CubismJson::ParseValue(const csmChar* buffer, csmInt32 length, csmInt32 begin, csmInt32* outEndPos)
{
    if (_error) return NULL;

    Value* o = NULL;
    csmInt32 i = begin;
    csmFloat32 f;
    csmString s1; //デバッグ用に使っている

    for (; i < length; i++)
    {
        csmChar c = static_cast<csmChar>(buffer[i] & 0xFF);

        switch (c)
        {
        case '-': case '.':
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9': {
            char* ret_ptr;
            f = strtof(const_cast<csmChar*>(buffer + i), &ret_ptr);
            *outEndPos = static_cast<csmInt32>(ret_ptr - buffer);
            return CSM_NEW Float(f);
        }
        case '\"':
            return CSM_NEW String(ParseString(buffer, length, i + 1, outEndPos)); //\"の次の文字から
        case '[':
            o = ParseArray(buffer, length, i + 1, outEndPos);
            return o;
        case '{':
            o = ParseObject(buffer, length, i + 1, outEndPos);
            return o;
        case 'n': //null以外にない
            if (i + 3 < length)
            {
                o = CSM_NEW NullValue(); //開放できるようにする
                *outEndPos = i + 4;
            }
            else _error = "parse null";
            return o;
        case 't': //true以外にない
            if (i + 3 < length)
            {
                o = Boolean::TrueValue;
                *outEndPos = i + 4;
            }
            else _error = "parse true";
            return o;
        case 'f': //false以外にない
            if (i + 4 < length)
            {
                o = Boolean::FalseValue;
                *outEndPos = i + 5;
            }
            else _error = "parse false";
            return o;
        case ',': //Array separator
            _error = "illegal ',' position";
            return NULL;
        case ']': //不正な}だがスキップする。配列の最後に不要な , があると思われる
            *outEndPos = i; //同じ文字を再処理
            return NULL;
        case '\n': _lineCount++;
        case ' ': case '\t': case '\r':
        default: //スキップ
            break;
        }
    }

    _error = "illegal end of value";
    return NULL;
}


Map::~Map()
{
    csmMap<csmString, Value*>::const_iterator ite = _map.Begin();
    while (ite != _map.End())
    {
        Value* v = (*ite).Second;
        if (v && !v->IsStatic()) CSM_DELETE(v);
        ++ite;
    }

    if (_keys) CSM_DELETE(_keys);
}


Array::~Array()
{
    csmVector<Value*>::iterator ite = _array.Begin();
    for (; ite != _array.End(); ++ite)
    {
        Value* v = (*ite);
        if (v && !v->IsStatic()) CSM_DELETE(v);
    }
}
}}}}
//------------ LIVE2D NAMESPACE ------------
