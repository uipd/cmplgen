#pragma interface

#ifndef __TGLC_SYMTABLE_H__
#define __TGLC_SYMTABLE_H__

using namespace std;

#include <string>
#include <map>
#include "code.h"
#include "const.h"

// 記号表エントリの種別  SymVar: 変数, SymProc: 手続き
enum SymClass { SymVar, SymProc };

// 変数の種別  GlobalVar: 大域変数, LocalVar: 局所変数，Param: 仮引数
enum VarClass { GlobalVar, LocalVar, Param };

// 記号表エントリ
class SymbolEntry  {
    SymClass _class;	// 記号表エントリの種別
    string _name;		// 識別子名（エラー出力用）
    Type _type;		// 型
public:
    // コンストラクタ
    SymbolEntry(SymClass c, string name, Type type)
    // これ以降のコンストラクタの定義を書き換えること
            : _class(c), _name(name), _type(type)  { }
    // 記号表エントリの種別を調べるための述語
    bool isVariable() { return _class == SymVar; }
    bool isProcedure() { return _class == SymProc; }
    // 識別子名にアクセスするためのメンバ関数
    string getName() { return _name; }
    // 識別子の型にアクセスするためのメンバ関数
    Type getType() { return _type; }
};

// 記号表の型名の定義
typedef map<string, SymbolEntry *> SymbolTable;


// 変数エントリ（記号表エントリのサブクラス）
class VarEntry : public SymbolEntry {
    VarClass _vclass;	// 変数の種別
    int _location;	// 大域変数のとき: 変数の静的データ領域内の番地
    // それ以外のとき: 変数のフレーム内相対番地
    bool _array;		// 配列なら true、単純変数なら false
    int _size;		// 配列のサイズ
public:
    // コンストラクタ
    VarEntry(VarClass vc, string name, Type type, bool array, int size)
    // これ以降のコンストラクタの定義を書き換えること
            : SymbolEntry(SymVar,name,type), _vclass(vc), _array(array), _size(size) { }
    // 変数の種別を調べるための述語
    bool isGlobalVariable() { return _vclass == GlobalVar; }
    bool isLocalVariable() { return _vclass == LocalVar; }
    bool isParameter() { return _vclass == Param; }
    // 配列かどうかを調べるためのメンバ関数
    bool isArray() { return _array; }
    // 配列のサイズを得るためのメンバ関数
    int getArraySize() { return _size; }
    // 変数の番地（配列を含む大域変数の場合は静的データ領域中の番地、
    // 局所変数と仮引数の場合はフレームポインタからの相対位置） を格納する。
    void setLocation(int loc) { _location = loc; }
    // 変数の番地を返す。
    int getLocation(void) { return _location; }
};

// 仮引数の型(型と引数名のペア)
typedef pair<Type,string> ParamType;

// 仮引数リストの型(仮引数の型のベクタ)
typedef vector<ParamType> ParamList;

// 手続きエントリ（記号表エントリのサブクラス）
class ProcEntry : public SymbolEntry {
  ParamList *_params;	// 仮引数リストへのポインタ(仮引数リストが空のときはNULL)
  Code *_code;		// 手続き本体の中間コードへのポインタ
  SystemProc _sysProc;	// システム手続きでなければNULL
  bool _defined;	// 定義済フラグ(定義済ならtrue、未定義ならfalse)
 public:
  // コンストラクタ
  ProcEntry(Type type, string name, ParamList *params)
    : SymbolEntry(SymProc,name,type), _defined(false), _code(new Code()), _sysProc(NULL) {
      // paramsが空でなければ仮引数へのポインタ
      _params = params.size() > 0 ? params : NULL;
  }
  // 引数個数へのアクセス
  int getParamNumber() { return params.size(); }
  // 引数の型リストのポインタへのアクセス
  ParamList *getParamList() { return _params; }
  // 本体のコードのポインタへのアクセス
  Code *getCode() { return _code; }
  // 定義済フラグへのアクセス
  bool isDefined() { return _defined; }	// 定義済か？
  void define() { _defined = true; }	// 定義済にする
  SystemProc getSystemProcedure() { return _sysProc; }
  void setSystemProcedure(SystemProc sysProc) { _sysProc = sysProc; }
  // 手続き本体の中間コードに構文木へのポインタを追加する
  void push_back(STree *tree) { _code->push_back(tree); }
};

/* symboltable.cc 中の関数プロトタイプ宣言 */

void initializeSymbolTable(void);
void checkProgram(void);
Code *getMainCode(void);
VarEntry *addGlobalVariable(string name, Type type);
VarEntry *addArray(string name, Type type, int size);
VarEntry *addLocalVariable(string name, Type type);
VarEntry *addParameter(string name, Type type);
ProcEntry *addProcedure(string name, Type type, ParamList *params);
ProcEntry *defineProcedure(string name, Type type);
VarEntry *findVariable(string name);
ProcEntry *findProcedure(string name);
void checkParamList(ParamList *params, ProcEntry *proc);

#endif
