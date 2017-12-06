#ifndef CEF_JWEBTOP_EX_SQLITE_H_
#define CEF_JWEBTOP_EX_SQLITE_H_
#include <map>
#include <string>
#include "sqlite3.h"
#include "include/cef_app.h" 

namespace jw {
	namespace db {
		using namespace std;

		/*
		 * 用途：打开指定的数据库，返回数据库句柄，或打开错误的原因
		 * 用法：JWebTop.db.open({dbpath:"数据库路径，可以是绝对路径或相对于运行目录的相对路径"},[handler]);
		 * 返回值：{db:打开的数据库句柄(null表示打开失败),msg:"打开失败时的字符串描述"}
		 */
		class JJH_DB_open : public CefV8Handler {
		public:
			bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);
		private:
			IMPLEMENT_REFCOUNTING(JJH_DB_open);
		};

		/*
		 * 用途：关闭指定的数据库。关闭出错时，返回错误的原因
		 * 用法：JWebTop.db.close({db:数据库句柄},[handler]);
		 * 返回值：{msg:"关闭出错的原因，或者null"}
		 */
		class JJH_DB_close : public CefV8Handler {
		public:
			bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);
		private:
			IMPLEMENT_REFCOUNTING(JJH_DB_close);
		};
	
		/*
		 * 用途：执行指定的sql，此方法不返回结果到JS端，需要结果时请采用query
		 * 用法：JWebTop.db.exec({db:(必填)数据库句柄,sql:"(必填)待执行的SQL语句");
		 * 返回值：{msg:"执行SQL出错的原因，或者null"}
		 */
		class JJH_DB_exec : public CefV8Handler {
		public:
			bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);
		private:
			IMPLEMENT_REFCOUNTING(JJH_DB_exec);
		};

	   /*
		* 用途：执行指定的sql，并以callback方式把结果返回到JS端
		* 用法：JWebTop.db.query({
		*         db:(必填)数据库句柄,
		*		  sql:"(必填)待执行的SQL语句，可以有?等参数"
		*		  params:[[JWebTop.db.type.SQLITE_TEXT,"params参数必须是二维数组，按顺序设置到sql的每个?上，按类型进行绑定"],[JWebTop.db.type.SQLITE_INTEGER,1],[JWebTop.db.type.SQLITE_FLOAT,1.234]],
		*		  names: printColumnNames(colNameArray){}
		*		  values:printValues(rowNumber,colNumber,value){}
		*		);
		* 返回值：{msg:"执行SQL出错的原因，或者null"}
		*/
		class JJH_DB_queryCallbcak : public CefV8Handler {
		public:
			bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);
		private:
			typedef CefRefPtr<CefV8Value>(JJH_DB_queryCallbcak::* outfun)(CefV8ValueList args);// 定义一个函数指针，用户输出数据库的某列数据到js

			int row = 0, col = 0;                      // 记录结果集的行数、列数
			CefRefPtr<CefV8Value> jsObject;            // JS执行环境
			CefRefPtr<CefV8Value> jsPrintQueryValues;  // 回调的js函数 
			outfun *colFuns;                           // 结果集每列的数据读取函数
			sqlite3_stmt *pStmt;                       // 查询后的结果集
			inline bool outputOneRowCol(CefRefPtr<CefV8Value>(JJH_DB_queryCallbcak::* outfun)(CefV8ValueList));
			inline CefRefPtr<CefV8Value> intOutFun(CefV8ValueList args) {
				return CefV8Value::CreateInt(sqlite3_column_int(pStmt, col));
			}
			inline CefRefPtr<CefV8Value> doubleOutFun(CefV8ValueList args) {
				return (CefV8Value::CreateDouble(sqlite3_column_double(pStmt, col)));
			}
			inline CefRefPtr<CefV8Value> txtOutFun(CefV8ValueList args) {
				 const unsigned char *  txt=sqlite3_column_text(pStmt, col);
				 if (txt) {
					 //string str = ( char *)txt;
					 return CefV8Value::CreateString((char *)txt);
				 }
				 return CefV8Value::CreateNull();
			}
			IMPLEMENT_REFCOUNTING(JJH_DB_queryCallbcak);
		};
	
		class JJH_DB_query : public CefV8Handler {
		public:
			bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);
		private:
			typedef CefRefPtr<CefV8Value>(JJH_DB_query::* outfun)();// 定义一个函数指针，用户输出数据库的某列数据到js

			int row = 0, col = 0;                      // 记录结果集的行数、列数

			outfun *colFuns;                           // 结果集每列的数据读取函数
			sqlite3_stmt *pStmt;                       // 查询后的结果集
			//inline bool outputOneRowCol(CefRefPtr<CefV8Value>(JJH_DB_query::* outfun)(), CefRefPtr<CefV8Value> arr);
			inline CefRefPtr<CefV8Value> intOutFun() {
				return CefV8Value::CreateInt(sqlite3_column_int(pStmt, col));
			}
			inline CefRefPtr<CefV8Value> doubleOutFun() {
				return (CefV8Value::CreateDouble(sqlite3_column_double(pStmt, col)));
			}
			inline CefRefPtr<CefV8Value> txtOutFun() {
				const unsigned char *  txt = sqlite3_column_text(pStmt, col);
				if (txt) {
					//string str = ( char *)txt;
					return CefV8Value::CreateString((char *)txt);
				}
				return CefV8Value::CreateNull();
			}
			IMPLEMENT_REFCOUNTING(JJH_DB_query);
		};

		/*
		* 用途：执行指定的sql，并以callback方式把结果返回到JS端
		* 用法：JWebTop.db.batch({
		*         db:(必填)数据库句柄,
		*		  sql:"(必填)待执行的SQL语句，可以有?等参数"
		*		  params:[[JWebTop.db.type.SQLITE_TEXT,"params参数必须是二维数组，按顺序设置到sql的每个?上，按类型进行绑定"],[JWebTop.db.type.SQLITE_INTEGER,1],[JWebTop.db.type.SQLITE_FLOAT,1.234]],
		*		  paramsTypes: printColumnNames(colNameArray){}
		*		  paramsValues:printValues(rowNumber,colNumber,value){}
		*		);
		* 返回值：{msg:"执行SQL出错的原因，或者null"}
		*/
		class JJH_DB_batch : public CefV8Handler {
		public:
			bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);
		private:
			typedef CefRefPtr<CefV8Value>(JJH_DB_batch::* outfun)(CefV8ValueList args);// 定义一个函数指针，用户输出数据库的某列数据到js
			IMPLEMENT_REFCOUNTING(JJH_DB_batch);
		};
	}
}
#endif