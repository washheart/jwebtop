#include "ex_sqlite.h"
#include <functional>
#include <sstream>
#include "common/util/StrUtil.h"
namespace jw {
	namespace db {

		bool JJH_DB_open::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			if (arguments.size() < 1 || !arguments[0]->HasValue("dbpath")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"�÷���JWebTop.db.open({dbpath:\"���ݿ�·���������Ǿ���·�������������Ŀ¼�����·��\"});"
					L"\r\n����ֵ��{msg:\"����ʱ���ش�����ʾ������Ϊ{}����\"}"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  dbpath = arguments[0]->GetValue("dbpath");
			if (!dbpath->IsString()) {
				retval->SetValue("msg", CefV8Value::CreateString(L"dbpath�����������ַ���"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			sqlite3 *db;
			int rc;
			// �����ݿ�
			rc = sqlite3_open_v2(dbpath->GetStringValue().ToString().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
			if (rc) {
				string err = "�����ݿ�ʧ�ܣ�";
				err.append(sqlite3_errmsg(db));
				retval->SetValue("msg", CefV8Value::CreateString(jw::str::s2w(err)), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_close(db);
				return true;
			}
			retval->SetValue("db", CefV8Value::CreateInt((int)db), V8_PROPERTY_ATTRIBUTE_NONE);
			return true;
		}

		// ��ȡ��������е�db
		inline CefRefPtr<CefV8Value> getParam_db(const CefV8ValueList& arguments, const CefRefPtr<CefV8Value>& retval) {
			CefRefPtr<CefV8Value>  db = arguments[0]->GetValue("db");
			if (!db->IsInt()) {// ��Ϊdb�����Ǳ���ģ�֮ǰ�Ѿ������Ƿ���ڣ��������ﲻ����null���
				retval->SetValue("msg", CefV8Value::CreateString(L"db��������������"), V8_PROPERTY_ATTRIBUTE_NONE);
				db = nullptr;
				return nullptr;
			}
			return db;
		}
		bool JJH_DB_close::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			if (arguments.size() < 1 || !arguments[0]->HasValue("db")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"�÷���JWebTop.db.close({db:���ݿ���})"
					L"\r\n����ֵ��{msg:\"����ʱ���ش�����ʾ������Ϊ{}����\"}"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  db = getParam_db(arguments, retval);
			if (db == nullptr)return true;
			sqlite3_close((sqlite3 *)db->GetIntValue());
			return true;
		}

		// ��ȡ��������е�sql����ȡ�ɹ���������true
		inline CefRefPtr<CefV8Value> getParam_sql(const CefV8ValueList& arguments, const CefRefPtr<CefV8Value>& retval) {
			CefRefPtr<CefV8Value>  sql = arguments[0]->GetValue("sql");
			if (!sql->IsString()) {// ��Ϊsql�����Ǳ���ģ�֮ǰ�Ѿ������Ƿ���ڣ��������ﲻ����null���
				retval->SetValue("msg", CefV8Value::CreateString(L"sql�����������ַ���"), V8_PROPERTY_ATTRIBUTE_NONE);
				sql = nullptr;
				return nullptr;
			}
			return sql;
		}

		// ִ��sql��䣬��֧�ֲ�������
		bool JJH_DB_exec::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			if (arguments.size() < 1 || !arguments[0]->HasValue("db") || !arguments[0]->HasValue("sql")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"�÷���JWebTop.db.exec({db:���ݿ���,sql:\"��ִ�е�SQL\"})"
					L"\r\n����ֵ��{msg:\"����ʱ���ش�����ʾ������Ϊ{}����\"}"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  db=getParam_db(arguments, retval);
			if (db == nullptr)return true;
			CefRefPtr<CefV8Value>  sql = getParam_sql(arguments, retval);
			if (sql == nullptr)return true;

			// �����ص�����֧��callback�ˣ���Ϊ�������query���ƣ�û�б�Ҫ��һ��
			char *zErrMsg = 0;
			int rc = sqlite3_exec((sqlite3 *)db->GetIntValue(), sql->GetStringValue().ToString().c_str(), NULL, this, &zErrMsg);
			if (rc != SQLITE_OK) {
				retval->SetValue("msg", CefV8Value::CreateString(zErrMsg), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_free(zErrMsg);
			}
			return true;
		}

		// Ϊquery�������ò���
		inline bool setQueryParams(const CefRefPtr<CefV8Value> params, const CefRefPtr<CefV8Value>  retval, sqlite3_stmt *pStmt) {
			int len = params->GetArrayLength();
			CefRefPtr<CefV8Value>   tmp, typeJO; string str;
			for (int paramIdx = 0; paramIdx < len; paramIdx++) {
				tmp = params->GetValue(paramIdx);
				if (!tmp->IsArray() || tmp->GetArrayLength() < 2)continue;
				switch (tmp->GetValue(0)->GetIntValue()) {
				case SQLITE_INTEGER:						/*��������*/
					sqlite3_bind_int(pStmt, paramIdx + 1, tmp->GetValue(1)->GetIntValue());
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
					return false;
				}
			}
			return true;
		}

		bool SQLiteQueryBase::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			// �������ݿ����֮ǰ����������Ƿ���ȷ
			if (!checkParams(object, arguments, retval))return true;
			CefRefPtr<CefV8Value>  db = getParam_db(arguments, retval);
			if (db == nullptr)return true;
			CefRefPtr<CefV8Value>  sql = getParam_sql(arguments, retval);
			if (sql == nullptr)return true;		
			CefRefPtr<CefV8Value>  params = arguments[0]->GetValue("params");
			if (params != NULL && !params->IsNull()) {
				params = arguments[0]->GetValue("params");
				if (!params->IsArray()) {
					retval->SetValue("msg", CefV8Value::CreateString(L"params���������Ƕ�ά���飬���磺[[JWebTop.db.type.SQLITE_INTEGER,1],[JWebTop.db.type.SQLITE_FLOAT,1.234],[JWebTop.db.type.SQLITE_TEXT,\"string\"]]"), V8_PROPERTY_ATTRIBUTE_NONE);
					return true;
				}
			}
			// ��ʼ�������ݲ���
			sqlite3 * pDb = (sqlite3 *)db->GetIntValue();
			sqlite3_stmt *pStmt;
			int rc = sqlite3_prepare_v2(pDb, sql->GetStringValue().ToString().c_str(), -1, &pStmt, NULL);
			if (rc != SQLITE_OK) {
				wstringstream ss; ss << L"�����ݿ�ʧ�ܣ�" << CefString(sqlite3_errmsg(pDb)).ToWString();
				retval->SetValue("msg", CefV8Value::CreateString(ss.str()), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_finalize(pStmt);
				return true;
			}
			if (params != NULL && !params->IsNull()) {// �󶨲���
				if (!setQueryParams(params, retval, pStmt)) {
					sqlite3_finalize(pStmt);
					return true;
				}
			}
			int ret = sqlite3_step(pStmt);
			if (ret != SQLITE_ROW) {// ����DDL��DML�����ԣ�sqlite3_stepִ����ȷ�ķ���ֵֻ��SQLITE_DONE
				wstringstream ss; ss << L"ִ�С�" << sql->GetStringValue().ToWString() << "��ʧ�ܣ�" << CefString(sqlite3_errmsg(pDb)).ToWString();
				retval->SetValue("msg", CefV8Value::CreateString(ss.str()), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_finalize(pStmt);
				this->pStmt = NULL;
				return true;
			}
			//��ȡ����Ŀ
			this->n_columns = sqlite3_column_count(pStmt);
			this->row = -1;
			this->col = 0;
			// ��ȡһ�����ݣ������ͽ����������������ÿ�ζ�ȥ�ж����ͣ���Ϊһ��sql�е��������Ͷ���һ�µ�			
			this->colFuns = new outfun[n_columns];
			this->pStmt = pStmt;
			// ���÷��صı��������
			CefRefPtr<CefV8Value> columnNames = CefV8Value::CreateArray(n_columns);
			for (col = 0; col < n_columns; col++) {
				columnNames->SetValue(col, CefV8Value::CreateString(sqlite3_column_name(pStmt, col)));
			}
			onSQLExecuted(retval, columnNames);

			// ��������
			if (ret == SQLITE_ROW) {
				onRowStart();
				// ����ÿһ�У�Ϊû������һ����ȡ����
				for (col = 0; col < n_columns; col++) {
					switch (sqlite3_column_type(pStmt, col)) {
					case SQLITE_INTEGER:						/*��������*/
						this->colFuns[col] = &SQLiteQueryBase::intOutFun;
						break;
					case SQLITE_FLOAT:							/*��������*/
						this->colFuns[col] = &SQLiteQueryBase::doubleOutFun;
						break;
					case SQLITE_TEXT:							/*�����ַ���*/
						this->colFuns[col] = &SQLiteQueryBase::txtOutFun;
						break;
						//case SQLITE_BLOB:							/*���������*/	sqlite3_column_blob(stmt, col));	break;
						//case SQLITE_NULL:							/*�����*/				break;
						//default:break;
					}
					onColumnValue();
				}
			}
			do {
				ret = sqlite3_step(pStmt);
				if (ret == SQLITE_ROW) {// ������Ҫ����Ľ������
					onRowStart();
					for (col = 0; col < n_columns; col++) {				//����ÿһ��		
						onColumnValue();
					}
				} else if (ret == SQLITE_DONE) //����
				{
					break;
				}
			} while (true);
			delete[]colFuns;
			sqlite3_finalize(pStmt);
			this->pStmt = NULL;
			return true;
		}
		/*------------------------------query callback---------------------------*/
		bool JJH_DB_queryCallbcak::checkParams(const CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, const CefRefPtr<CefV8Value>& retval) {
			if (arguments.size() < 1 || !arguments[0]->HasValue("db") || !arguments[0]->HasValue("sql")
				|| !arguments[0]->HasValue("names") || !arguments[0]->HasValue("values")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"�÷���JWebTop.db.queryCallbcak({" 
					L"db:���ݿ���,"
					L"sql:\"��ִ�е�SQL��䣬������?�Ȳ���\","
					L"params:'����ѡ��[[JWebTop.db.type.SQLITE_TEXT,\"params���������Ƕ�ά���飬��˳�����õ�sql��ÿ�� ? �ϣ������ͽ��а�\"],[JWebTop.db.type.SQLITE_INTEGER,1],[JWebTop.db.type.SQLITE_FLOAT,1.234]]',"
					L"names: printColumnNames(colNameArray){/* ���ڽ��ս������������ĺ��� */},"
					L"values:printValues(rowNumber,colNumber,value){/* ���ڽ��ս�����ĺ��� */}  "
					L"\r\n����ֵ��{msg:\"����ʱ���ش�����ʾ������Ϊ{}����\"}"
					L"})"), V8_PROPERTY_ATTRIBUTE_NONE);
				return false;
			}
			this->jsObject = object;// JSִ�л���
			// �����ص��������ѯԪ��Ϣ			
			this->jsColNamesFun = arguments[0]->GetValue("names");
			if (!this->jsColNamesFun->IsFunction()) {
				retval->SetValue("msg", CefV8Value::CreateString(L"names����������function printColumnNames(colNameArray)ǩ����JS����"), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_finalize(pStmt);
				return false;
			}
			// �����ص���������������
			this->jsPrintQueryValues = arguments[0]->GetValue("values");
			if (!this->jsPrintQueryValues->IsFunction()) {
				retval->SetValue("msg", CefV8Value::CreateString(L"values����������function printValues(rowNumber,colNumber,value)ǩ����JS����"), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_finalize(pStmt);
				return false;
			}
			return true;
		};
		void JJH_DB_queryCallbcak::onSQLExecuted(const CefRefPtr<CefV8Value>  retval, const CefRefPtr<CefV8Value> columnNames) {
			CefV8ValueList args;
			args.push_back(columnNames);
			jsColNamesFun->ExecuteFunction(this->jsObject, args);
		}
		void JJH_DB_queryCallbcak::onRowStart() {
			this->row++;
		}
		void JJH_DB_queryCallbcak::onColumnValue() {
			CefV8ValueList args;
			args.push_back(CefV8Value::CreateInt(this->row));
			args.push_back(CefV8Value::CreateInt(col));
			args.push_back((this->*colFuns[this->col])());
			this->jsPrintQueryValues->ExecuteFunction(this->jsObject, args);
		}

		/*------------------------------query dataset---------------------------*/
		bool JJH_DB_queryDataSet::checkParams(const CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, const CefRefPtr<CefV8Value>& retval) {
			if (arguments.size() < 1 || !arguments[0]->HasValue("db") || !arguments[0]->HasValue("sql")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"�÷���JWebTop.db.queryDataSet({"
					L"db:���ݿ���,"
					L"sql:\"��ִ�е�SQL��䣬������?�Ȳ���\","
					L"params:'����ѡ��[[JWebTop.db.type.SQLITE_TEXT,\"params���������Ƕ�ά���飬��˳�����õ�sql��ÿ�� ? �ϣ������ͽ��а�\"],[JWebTop.db.type.SQLITE_INTEGER,1],[JWebTop.db.type.SQLITE_FLOAT,1.234]]',"
					L"\r\n����ֵ��{msg:\"����ʱ���ش�����ʾ�������޴�����\",columnNames:[/*���������������*/],dataset:[ [], [], [/*�����*/] ]}"
					L"})"), V8_PROPERTY_ATTRIBUTE_NONE);
				return false;
			}
			return true;
		};
		void JJH_DB_queryDataSet::onSQLExecuted(const CefRefPtr<CefV8Value>  retval, const CefRefPtr<CefV8Value> columnNames) {
			retval->SetValue("columnNames", columnNames, V8_PROPERTY_ATTRIBUTE_NONE);
			this->dataset = CefV8Value::CreateArray(this->n_columns);
			retval->SetValue("dataset", dataset, V8_PROPERTY_ATTRIBUTE_NONE);
		}
		void JJH_DB_queryDataSet::onRowStart() {
			this->curRowArr = CefV8Value::CreateArray(this->n_columns);
			this->row++;
			this->dataset->SetValue(this->row, this->curRowArr);
		}
		void JJH_DB_queryDataSet::onColumnValue() {
			this->curRowArr->SetValue(this->col, (this->*colFuns[this->col])());
		}

		// ����ִ��SQL��䣬�������κν����
		bool JJH_DB_batch::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			if (arguments.size() < 1 || !arguments[0]->HasValue("db") || !arguments[0]->HasValue("sql")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"�÷���JWebTop.db.batch({"
					L"db: ���ݿ���,"
					L"sql: \"��ִ�е�SQL��䣬������?�Ȳ���\","
					L"paramsTypes: '����ѡ��paramsTypes�������������飬���磺[JWebTop.db.type.SQLITE_INTEGER,JWebTop.db.type.SQLITE_FLOAT,JWebTop.db.type.SQLITE_TEXT]',"
					L"paramValues: 'paramValues���������Ƕ�ά���飬���磺[[1,1.234,\"string1\"]  ,[2,2.234,\"string2\"] ]' },"
					L"})"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  db = getParam_db(arguments, retval);
			if (db == nullptr)return true;
			CefRefPtr<CefV8Value>  sql = getParam_sql(arguments, retval);
			if (sql == nullptr)return true;
			CefRefPtr<CefV8Value>  paramsTypes;
			wstringstream ss;
			if (arguments[0]->HasValue("paramsTypes")) {
				paramsTypes = arguments[0]->GetValue("paramsTypes");
				if (!paramsTypes->IsArray()) {
					retval->SetValue("msg", CefV8Value::CreateString(L"paramsTypes�������������飬���磺[JWebTop.db.type.SQLITE_INTEGER,JWebTop.db.type.SQLITE_FLOAT,JWebTop.db.type.SQLITE_TEXT]"), V8_PROPERTY_ATTRIBUTE_NONE);
					return true;
				} else {
					int typeLen = paramsTypes->GetArrayLength();
					CefRefPtr<CefV8Value>  tmpColType;
					for (int col = 0; col < typeLen; col++) {// ���ȼ�����д��������Ƿ���ȷ
						tmpColType = paramsTypes->GetValue(col);
						switch (tmpColType->GetIntValue()) {
						case SQLITE_TEXT:							/*�����ַ���*/
							break;
						case SQLITE_INTEGER:						/*��������*/
							break;
						case SQLITE_FLOAT:							/*��������*/
							break;
							//case SQLITE_BLOB:							/*���������*/
							//case SQLITE_NULL:							/*�����*/
						default:
							ss << L"��֧��[" << tmpColType->GetIntValue() << L"]���͵Ĳ���";
							retval->SetValue("msg", CefV8Value::CreateString(ss.str()), V8_PROPERTY_ATTRIBUTE_NONE);
							return true;
						}
					}
				}
			}
			CefRefPtr<CefV8Value>  paramValues;
			if (arguments[0]->HasValue("paramValues")) {
				paramValues = arguments[0]->GetValue("paramValues");
				if (!paramValues->IsArray()) {
					retval->SetValue("msg", CefV8Value::CreateString(L"paramValues���������Ƕ�ά���飬���磺[[1,1.234,\"string1\"]  ,[2,2.234,\"string2\"] ] "), V8_PROPERTY_ATTRIBUTE_NONE);
					return true;
				}
			}
			sqlite3 * pDb = (sqlite3 *)db->GetIntValue();
			// ������������ʱ��������������������ٶ�
			char *zErrMsg = 0;
			int rc = sqlite3_exec(pDb, "BEGIN TRANSACTION", NULL, this, &zErrMsg);
			if (rc != SQLITE_OK) {
				retval->SetValue("msg", CefV8Value::CreateString(zErrMsg), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_free(zErrMsg); 
				return true;
			}

			sqlite3_stmt *pStmt;
			rc = sqlite3_prepare_v2(pDb, sql->GetStringValue().ToString().c_str(), -1, &pStmt, NULL);
			if (rc != SQLITE_OK) {
				ss << L"��ʼ����" << sql->GetStringValue().ToWString() << "��ʧ�ܣ�" << CefString(sqlite3_errmsg(pDb)).ToWString();
				retval->SetValue("msg", CefV8Value::CreateString(ss.str()), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_finalize(pStmt);
				return true;
			}
			bool success = true;
			if (paramValues != NULL) {// �󶨲���
				int batchCount = paramValues->GetArrayLength();
				int typeLen = paramsTypes->GetArrayLength(), rowParamsLen = 0;
				CefRefPtr<CefV8Value> rowParams , tmpColType;				
				string str;
				for (int row = 0; row < batchCount; row++) {
					rowParams = paramValues->GetValue(row);
					rowParamsLen = rowParams->GetArrayLength();
					if (rowParamsLen > typeLen)rowParamsLen = typeLen;// ����С�����鳤��������ֵ
					for (int col = 0; col < rowParamsLen; col++) {
						tmpColType = paramsTypes->GetValue(col);
						switch (tmpColType->GetIntValue()) {
						case SQLITE_TEXT:							/*�����ַ���*/
							str = rowParams->GetValue(col)->GetStringValue();
							sqlite3_bind_text(pStmt, col + 1, str.c_str(), str.length(), SQLITE_TRANSIENT);
							break;
						case SQLITE_INTEGER:						/*��������*/
							sqlite3_bind_int(pStmt, col + 1, rowParams->GetValue(col)->GetIntValue());
							break;
						case SQLITE_FLOAT:							/*��������*/
							sqlite3_bind_double(pStmt, col + 1, rowParams->GetValue(col)->GetDoubleValue());
							break;
							//case SQLITE_BLOB:							/*���������*/
							//case SQLITE_NULL:							/*�����*/
						}
					}
					int ret = sqlite3_step(pStmt);
					if (ret != SQLITE_DONE) {// �����ˣ�����
						success = false;
						ss << L"ִ�С�" << sql->GetStringValue().ToWString() << "��ʧ�ܣ�" << CefString(sqlite3_errmsg(pDb)).ToWString() << L"   row=" << row;
						break;
					}
					sqlite3_reset(pStmt);// ���³�ʼ����sqlite3_stmt����󶨵ı�����
				}
			} else {
				int ret = sqlite3_step(pStmt);
				if (ret != SQLITE_DONE) {
					success = false;
					ss << L"ִ�С�" << sql->GetStringValue().ToWString() << "��ʧ�ܣ�" << CefString(sqlite3_errmsg(pDb)).ToWString();
				}
			}
			sqlite3_finalize(pStmt);
			if (success) {// ֮ǰִ��û�г��������ύ֮ǰ������
				rc = sqlite3_exec(pDb, "COMMIT", NULL, this, &zErrMsg);
				if (rc != SQLITE_OK) {
					ss << L"�ύ����ʧ�ܣ�" << zErrMsg;
					sqlite3_free(zErrMsg);
					rc = sqlite3_exec(pDb, "ROLLBACK", NULL, this, &zErrMsg);// ��Ҫ�ع�����
					if (rc != SQLITE_OK) {
						ss << L"  ����ع�ʧ�ܣ�" << zErrMsg;
					} else {
						ss << L"  �����ѻع���";
					}
					sqlite3_free(zErrMsg);
					retval->SetValue("msg", CefV8Value::CreateString(ss.str()), V8_PROPERTY_ATTRIBUTE_NONE);
				}
			} else {// ֮ǰִ�г������лع�
				rc = sqlite3_exec(pDb, "ROLLBACK", NULL, this, &zErrMsg);// ��Ҫ�ع�����
				if (rc != SQLITE_OK) {
					ss << L"  ����ع�ʧ�ܣ�" << zErrMsg;
				} else {
					ss << L"  �����ѻع���";
				}
				sqlite3_free(zErrMsg);
				retval->SetValue("msg", CefV8Value::CreateString(ss.str()), V8_PROPERTY_ATTRIBUTE_NONE);
			}
			return true;
		}
	}// End namespace:db
}// End namespace:jw