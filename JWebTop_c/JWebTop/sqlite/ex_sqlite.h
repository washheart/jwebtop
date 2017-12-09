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
		 * ��;��ִ��ָ����sql���޴������Ҳ�޽�������أ������Ƿ�ִ�гɹ��Լ�����ʱ�Ĵ�����Ϣ
		 * �÷����μ�ʵ���ࡣ�������ʱ�����κβ�������ʾʹ��˵��
		 */
		class JJH_DB_exec : public CefV8Handler {
		public:
			bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);
		private:
			IMPLEMENT_REFCOUNTING(JJH_DB_exec);
		};
		
		// ��ѯ��Ļ��࣬����һЩ�����߼�
		class SQLiteQueryBase : public virtual CefV8Handler {
		protected:
			typedef CefRefPtr<CefV8Value>(SQLiteQueryBase::* outfun)();// ����һ������ָ�룬�û�������ݿ��ĳ�����ݵ�js
			int n_columns = 0;                         // ������
			int row = 0, col = 0;                      // ��¼�����������������
			outfun *colFuns;                           // �����ÿ�е����ݶ�ȡ����
			sqlite3_stmt *pStmt;                       // ��ѯ��Ľ����
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
			// ������ʵ�֣����д�������Ļ�����֤
			virtual bool checkParams(const CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, const CefRefPtr<CefV8Value>& retval) = 0;
			// ��ѯ�ѳɹ�ִ�У���ʱ���Դ����������Ƶ�����
			virtual void onSQLExecuted(const CefRefPtr<CefV8Value>  retval, const CefRefPtr<CefV8Value> columnNames) = 0;
			// ��ʼһ�������ݵĴ���
			virtual void onRowStart() = 0;
			// �����������е�ĳ��
			virtual void onColumnValue() = 0;
		public:
			bool Execute(const CefString& name,
				CefRefPtr<CefV8Value> object,
				const CefV8ValueList& arguments,
				CefRefPtr<CefV8Value>& retval,
				CefString& exception) ;
		};

		/*
		* ��;��ִ��ָ����sql�����Ѳ�ѯ����ŵ���������أ�ͬʱ�����Ƿ�ִ�гɹ��Լ�����ʱ�Ĵ�����Ϣ
		* �÷����μ�ʵ���ࡣ�������ʱ�����κβ�������ʾʹ��˵��
		*/
		class JJH_DB_queryDataSet : public virtual SQLiteQueryBase {
		protected:
			 bool checkParams(const CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, const CefRefPtr<CefV8Value>& retval);
			 void onSQLExecuted(const CefRefPtr<CefV8Value>  retval, const CefRefPtr<CefV8Value> columnNames);
			 void onRowStart();
			 void onColumnValue();
		private:
			CefRefPtr<CefV8Value>  dataset, curRowArr;	
			IMPLEMENT_REFCOUNTING(JJH_DB_queryDataSet);
		};
	   
	   /*
		* ��;��ִ��ָ����sql������ִ�й�������callback��ʽ�ѽ�����ص�JS�ˣ�����ִ����Ϻ󷵻��Ƿ�ִ�гɹ��Լ�����ʱ�Ĵ�����Ϣ
		* �÷����μ�ʵ���ࡣ�������ʱ�����κβ�������ʾʹ��˵��
		*/
		class JJH_DB_queryCallbcak : public virtual SQLiteQueryBase {
		protected:
			bool checkParams(const CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, const CefRefPtr<CefV8Value>& retval);
			void onSQLExecuted(const CefRefPtr<CefV8Value>  retval, const CefRefPtr<CefV8Value> columnNames);
			void onRowStart();
			void onColumnValue();
		private:
			CefRefPtr<CefV8Value> jsObject;            // JSִ�л���
			CefRefPtr<CefV8Value> jsColNamesFun;       // �ص���js����
			CefRefPtr<CefV8Value> jsPrintQueryValues;  // �ص���js���� 
			IMPLEMENT_REFCOUNTING(JJH_DB_queryCallbcak);
		};
	
		/*
		* ��;��ִ����������͸��£��޽�������أ������Ƿ�ִ�гɹ��Լ�����ʱ�Ĵ�����Ϣ
		* �÷����μ�ʵ���ࡣ�������ʱ�����κβ�������ʾʹ��˵��
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