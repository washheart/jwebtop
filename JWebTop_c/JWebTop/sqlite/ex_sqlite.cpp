#include "ex_sqlite.h"
#include <functional>
#include <sstream>
#include "common/util/StrUtil.h"
namespace jw {
	namespace db {

		bool JJH_db_open::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			if (arguments.size() < 1 || !arguments[0]->HasValue("dbpath")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"�÷���JWebTop.db.open({dbpath:\"���ݿ�·���������Ǿ���·�������������Ŀ¼�����·��\"});"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  dbpath = arguments[0]->GetValue("dbpath");
			if (!dbpath->IsString()) {
				retval->SetValue("msg", CefV8Value::CreateString(L"dbpath�����������ַ���"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			sqlite3 *db;
			char *zErrMsg = 0;
			int rc;
			// �����ݿ�
			rc = sqlite3_open_v2(dbpath->GetStringValue().ToString().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
			if (rc) {
				string err = "�����ݿ�ʧ�ܣ�";
				err.append(sqlite3_errmsg(db));
				retval->SetValue("msg", CefV8Value::CreateString(jw::s2w(err)), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_close(db);
				return true;
			}
			retval->SetValue("db", CefV8Value::CreateInt((int)db), V8_PROPERTY_ATTRIBUTE_NONE);
			return true;
		}

		bool JJH_db_close::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			if (arguments.size() < 1 || !arguments[0]->HasValue("db")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"�÷���JWebTop.db.close({db:���ݿ���})"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  db = arguments[0]->GetValue("db");
			if (!db->IsInt()) {
				retval->SetValue("msg", CefV8Value::CreateString(L"db��������������"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			sqlite3_close((sqlite3 *)db->GetIntValue());
			return true;
		}
		bool JJH_DB_exec::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			if (arguments.size() < 1 || !arguments[0]->HasValue("db") || !arguments[0]->HasValue("sql")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"�÷���JWebTop.db.exex({db:���ݿ���,sql:\"��ִ�е�SQL\"})"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  db = arguments[0]->GetValue("db");
			if (!db->IsInt()) {
				retval->SetValue("msg", CefV8Value::CreateString(L"db��������������"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  sql = arguments[0]->GetValue("sql");
			if (!sql->IsString()) {
				retval->SetValue("msg", CefV8Value::CreateString(L"sql�����������ַ���"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			//// �����ص�����֧��callback�ˣ���Ϊ�������query���ƣ�û�б�Ҫ��һ��
			//if (arguments[0]->HasValue("callback")) {
			//	this->jsCallback = arguments[0]->GetValue("callback");
			//	if (!this->jsCallback->IsFunction()) {
			//		retval->SetValue("msg", CefV8Value::CreateString(L"callback����������function callback(rowNumber,colNumber,colName,colValue)ǩ����JS����"), V8_PROPERTY_ATTRIBUTE_NONE);
			//		return true;
			//	}
			//	this->jsObject = object;
			//}
			char *zErrMsg = 0;
			int rc = sqlite3_exec((sqlite3 *)db->GetIntValue(), sql->GetStringValue().ToString().c_str(), NULL, this, &zErrMsg);
			if (rc != SQLITE_OK) {
				retval->SetValue("msg", CefV8Value::CreateString(zErrMsg), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_free(zErrMsg);
			}
			return true;
		}
		//int JJH_DB_exec::callback(void *data, int ncols, char **values, char **azColName) {
		//	JJH_DB_exec * t = (JJH_DB_exec *)data;
		//	return 0;
		//}

		bool JJH_DB_query::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			if (arguments.size() < 1 || !arguments[0]->HasValue("db") || !arguments[0]->HasValue("sql")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"�÷���JWebTop.db.exex({db:���ݿ���,sql:\"��ִ�е�SQL\"})"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  db = arguments[0]->GetValue("db");
			if (!db->IsInt()) {
				retval->SetValue("msg", CefV8Value::CreateString(L"db��������������"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  sql = arguments[0]->GetValue("sql");
			if (!sql->IsString()) {
				retval->SetValue("msg", CefV8Value::CreateString(L"sql�����������ַ���"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  params;
			if (arguments[0]->HasValue("params")) {
				params = arguments[0]->GetValue("params");
				if (!params->IsArray()) {
					retval->SetValue("msg", CefV8Value::CreateString(L"params���������Ƕ�ά���飬���磺[[JWebTop.db.type.SQLITE_INTEGER,1],[JWebTop.db.type.SQLITE_FLOAT,1.234],[JWebTop.db.type.SQLITE_TEXT,\"string\"]]"), V8_PROPERTY_ATTRIBUTE_NONE);
					return true;
				}
			}		
			sqlite3 * pDb = (sqlite3 *)db->GetIntValue();
			sqlite3_stmt *pStmt;
			int rc = sqlite3_prepare_v2(pDb, sql->GetStringValue().ToString().c_str(), -1, &pStmt, NULL);
			if (rc != SQLITE_OK) {
				wstringstream ss; ss << L"�����ݿ�ʧ�ܣ�" << sqlite3_errmsg(pDb);
				retval->SetValue("msg", CefV8Value::CreateString(ss.str()), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_finalize(pStmt);
				return true;
			}
			if (params != NULL) {// �󶨲���
				int len = params->GetArrayLength();
				int type = 0;
				CefRefPtr<CefV8Value>   tmp, typeJO; string str;
				for (int paramIdx = 0; paramIdx < len; paramIdx++) {
					tmp = params->GetValue(paramIdx);
					if (!tmp->IsArray() || tmp->GetArrayLength() < 2)continue;
					switch (tmp->GetValue(0)->GetIntValue()) {
					case SQLITE_INTEGER:						/*��������*/
						sqlite3_bind_int(pStmt, paramIdx+1, tmp->GetValue(1)->GetIntValue());
						break;
					case SQLITE_FLOAT:							/*��������*/
						sqlite3_bind_double(pStmt, paramIdx + 1, tmp->GetValue(1)->GetDoubleValue());
						break;
					case SQLITE_TEXT:							/*�����ַ���*/
						str = tmp->GetValue(1)->GetStringValue();
						sqlite3_bind_text(pStmt, paramIdx + 1, str.c_str(), str.length(), SQLITE_TRANSIENT);
						break;
						//case SQLITE_BLOB:							/*���������*/
						//case SQLITE_NULL:							/*�����*/
					default:
						wstringstream err;
						err << L"��֧��[" << typeJO->GetIntValue() << L"]���͵Ĳ���";
						retval->SetValue("msg", CefV8Value::CreateString(err.str()), V8_PROPERTY_ATTRIBUTE_NONE);
						sqlite3_finalize(pStmt);
						return true;
					}
				}
			}
			int ret = sqlite3_step(pStmt);
			wstringstream ss;
			ss << L"appendOutput('ret=" << ret << L"  SQLITE_DONE=" << SQLITE_DONE << L"')\r\n";
			if (ret == SQLITE_DONE) {// ����DDL��DML�����ԣ�sqlite3_stepִ����ȷ�ķ���ֵֻ��SQLITE_DONE
				sqlite3_finalize(pStmt);
				return true;
			}
			// �����ص��������ѯԪ��Ϣ��������������������
			CefRefPtr<CefV8Value> jsColNamesFun;
			if (arguments[0]->HasValue("names")) {
				jsColNamesFun = arguments[0]->GetValue("names");
				if (!jsColNamesFun->IsFunction()) {
					retval->SetValue("msg", CefV8Value::CreateString(L"names����������function printColumnNames(colNameArray)ǩ����JS����"), V8_PROPERTY_ATTRIBUTE_NONE);
					sqlite3_finalize(pStmt); 
					return true;
				}
				this->jsObject = object;
			}
			// �����ص�
			if (arguments[0]->HasValue("values")) {
				this->jsPrintQueryValues = arguments[0]->GetValue("values");
				if (!this->jsPrintQueryValues->IsFunction()) {
					retval->SetValue("msg", CefV8Value::CreateString(L"values����������function printValues(rowNumber,colNumber,value)ǩ����JS����"), V8_PROPERTY_ATTRIBUTE_NONE);
					sqlite3_finalize(pStmt); 
					return true;
				}
				this->jsObject = object;
			}
			if (jsColNamesFun == NULL&&this->jsPrintQueryValues == NULL) {// ���û�лص�����������Ϊ����Ҫ���ؽ��������ֱ�ӰѺ���������
				sqlite3_finalize(pStmt);
				return true;
			}
			//��ȡ����Ŀ
			int n_columns = sqlite3_column_count(pStmt);
			this->row = 0;
			this->col = 0;
			// ��ȡһ�����ݣ������ͽ����������������ÿ�ζ�ȥ�ж����ͣ���Ϊһ��sql�е��������Ͷ���һ�µ�			
			this->colFuns = new outfun[n_columns];
			this->pStmt = pStmt;
			if (ret == SQLITE_ROW) {
				if (jsColNamesFun != NULL) {
					CefV8ValueList args;
					CefRefPtr<CefV8Value> arr = CefV8Value::CreateArray(n_columns);
					for (col = 0; col < n_columns; col++) {						;
						arr->SetValue(col, CefV8Value::CreateString(sqlite3_column_name(pStmt, col)));
					}
					args.push_back(arr);
					jsColNamesFun->ExecuteFunction(this->jsObject, args);
				}
				// ����ÿһ��
				for (col = 0; col < n_columns; col++) {
					 switch (sqlite3_column_type(pStmt, col)) {
					case SQLITE_INTEGER:						/*��������*/
						this->colFuns[col] = &JJH_DB_query::intOutFun;
						break;
					case SQLITE_FLOAT:							/*��������*/
						this->colFuns[col] = &JJH_DB_query::doubleOutFun;
						break;
					case SQLITE_TEXT:							/*�����ַ���*/
						this->colFuns[col] = &JJH_DB_query::txtOutFun;
						break;
						//case SQLITE_BLOB:							/*���������*/	sqlite3_column_blob(stmt, col));	break;
						//case SQLITE_NULL:							/*�����*/				break;
					//default:break;
					}
					outputOneRowCol(this->colFuns[col]);
				}				
			}
			do {
				ret = sqlite3_step(pStmt);
				if (ret == SQLITE_ROW) {// ������Ҫ����Ľ������				
					for (col = 0; col < n_columns; col++) {				//����ÿһ��		
						outputOneRowCol(this->colFuns[col]);
					}
				} else if (ret == SQLITE_DONE) //����
				{
					break;
				}
			} while (true);
			delete []colFuns;
			sqlite3_finalize(pStmt);
			this->pStmt = NULL;
			return true;
		}
		inline bool JJH_DB_query::outputOneRowCol(CefRefPtr<CefV8Value>(JJH_DB_query::* outfun)(CefV8ValueList)) {
			CefV8ValueList args;
			args.push_back(CefV8Value::CreateInt(this->row));
			args.push_back(CefV8Value::CreateInt(col));
			args.push_back((this->*outfun)(args));
			this->jsPrintQueryValues->ExecuteFunction(this->jsObject, args);
			this->row++;
			return true;
		}
	}// End namespace:db
}// End namespace:jw