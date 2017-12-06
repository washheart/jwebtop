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
		 * ��;����ָ�������ݿ⣬�������ݿ�������򿪴����ԭ��
		 * �÷���JWebTop.db.open({dbpath:"���ݿ�·���������Ǿ���·�������������Ŀ¼�����·��"},[handler]);
		 * ����ֵ��{db:�򿪵����ݿ���(null��ʾ��ʧ��),msg:"��ʧ��ʱ���ַ�������"}
		 */
		class JJH_DB_open : public CefV8Handler {
		public:
			bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);
		private:
			IMPLEMENT_REFCOUNTING(JJH_DB_open);
		};

		/*
		 * ��;���ر�ָ�������ݿ⡣�رճ���ʱ�����ش����ԭ��
		 * �÷���JWebTop.db.close({db:���ݿ���},[handler]);
		 * ����ֵ��{msg:"�رճ����ԭ�򣬻���null"}
		 */
		class JJH_DB_close : public CefV8Handler {
		public:
			bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);
		private:
			IMPLEMENT_REFCOUNTING(JJH_DB_close);
		};
	
		/*
		 * ��;��ִ��ָ����sql���˷��������ؽ����JS�ˣ���Ҫ���ʱ�����query
		 * �÷���JWebTop.db.exec({db:(����)���ݿ���,sql:"(����)��ִ�е�SQL���");
		 * ����ֵ��{msg:"ִ��SQL�����ԭ�򣬻���null"}
		 */
		class JJH_DB_exec : public CefV8Handler {
		public:
			bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);
		private:
			IMPLEMENT_REFCOUNTING(JJH_DB_exec);
		};

	   /*
		* ��;��ִ��ָ����sql������callback��ʽ�ѽ�����ص�JS��
		* �÷���JWebTop.db.query({
		*         db:(����)���ݿ���,
		*		  sql:"(����)��ִ�е�SQL��䣬������?�Ȳ���"
		*		  params:[[JWebTop.db.type.SQLITE_TEXT,"params���������Ƕ�ά���飬��˳�����õ�sql��ÿ��?�ϣ������ͽ��а�"],[JWebTop.db.type.SQLITE_INTEGER,1],[JWebTop.db.type.SQLITE_FLOAT,1.234]],
		*		  names: printColumnNames(colNameArray){}
		*		  values:printValues(rowNumber,colNumber,value){}
		*		);
		* ����ֵ��{msg:"ִ��SQL�����ԭ�򣬻���null"}
		*/
		class JJH_DB_queryCallbcak : public CefV8Handler {
		public:
			bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);
		private:
			typedef CefRefPtr<CefV8Value>(JJH_DB_queryCallbcak::* outfun)(CefV8ValueList args);// ����һ������ָ�룬�û�������ݿ��ĳ�����ݵ�js

			int row = 0, col = 0;                      // ��¼�����������������
			CefRefPtr<CefV8Value> jsObject;            // JSִ�л���
			CefRefPtr<CefV8Value> jsPrintQueryValues;  // �ص���js���� 
			outfun *colFuns;                           // �����ÿ�е����ݶ�ȡ����
			sqlite3_stmt *pStmt;                       // ��ѯ��Ľ����
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
			typedef CefRefPtr<CefV8Value>(JJH_DB_query::* outfun)();// ����һ������ָ�룬�û�������ݿ��ĳ�����ݵ�js

			int row = 0, col = 0;                      // ��¼�����������������

			outfun *colFuns;                           // �����ÿ�е����ݶ�ȡ����
			sqlite3_stmt *pStmt;                       // ��ѯ��Ľ����
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
		* ��;��ִ��ָ����sql������callback��ʽ�ѽ�����ص�JS��
		* �÷���JWebTop.db.batch({
		*         db:(����)���ݿ���,
		*		  sql:"(����)��ִ�е�SQL��䣬������?�Ȳ���"
		*		  params:[[JWebTop.db.type.SQLITE_TEXT,"params���������Ƕ�ά���飬��˳�����õ�sql��ÿ��?�ϣ������ͽ��а�"],[JWebTop.db.type.SQLITE_INTEGER,1],[JWebTop.db.type.SQLITE_FLOAT,1.234]],
		*		  paramsTypes: printColumnNames(colNameArray){}
		*		  paramsValues:printValues(rowNumber,colNumber,value){}
		*		);
		* ����ֵ��{msg:"ִ��SQL�����ԭ�򣬻���null"}
		*/
		class JJH_DB_batch : public CefV8Handler {
		public:
			bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);
		private:
			typedef CefRefPtr<CefV8Value>(JJH_DB_batch::* outfun)(CefV8ValueList args);// ����һ������ָ�룬�û�������ݿ��ĳ�����ݵ�js
			IMPLEMENT_REFCOUNTING(JJH_DB_batch);
		};
	}
}
#endif