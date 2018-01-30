#include "ex_sqlite.h"
#include <functional>
#include <sstream>
#include "common/util/StrUtil.h"
namespace jw {
	namespace db {

		bool JJH_DB_open::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			if (arguments.size() < 1 || !arguments[0]->HasValue("dbpath")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"用法：JWebTop.db.open({dbpath:\"数据库路径，可以是绝对路径或相对于运行目录的相对路径\"});"
					L"\r\n返回值：{msg:\"出错时返回错误提示，否则为{}对象\"}"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  dbpath = arguments[0]->GetValue("dbpath");
			if (!dbpath->IsString()) {
				retval->SetValue("msg", CefV8Value::CreateString(L"dbpath参数必须是字符串"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			sqlite3 *db;
			int rc;
			// 打开数据库
			rc = sqlite3_open_v2(dbpath->GetStringValue().ToString().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
			if (rc) {
				string err = "打开数据库失败：";
				err.append(sqlite3_errmsg(db));
				retval->SetValue("msg", CefV8Value::CreateString(jw::str::s2w(err)), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_close(db);
				return true;
			}
			retval->SetValue("db", CefV8Value::CreateInt((int)db), V8_PROPERTY_ATTRIBUTE_NONE);
			return true;
		}

		// 获取传入参数中的db
		inline CefRefPtr<CefV8Value> getParam_db(const CefV8ValueList& arguments, const CefRefPtr<CefV8Value>& retval) {
			CefRefPtr<CefV8Value>  db = arguments[0]->GetValue("db");
			if (!db->IsInt()) {// 因为db参数是必须的，之前已经检测过是否存在，所以这里不进行null检查
				retval->SetValue("msg", CefV8Value::CreateString(L"db参数必须是数字"), V8_PROPERTY_ATTRIBUTE_NONE);
				db = nullptr;
				return nullptr;
			}
			return db;
		}
		bool JJH_DB_close::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			if (arguments.size() < 1 || !arguments[0]->HasValue("db")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"用法：JWebTop.db.close({db:数据库句柄})"
					L"\r\n返回值：{msg:\"出错时返回错误提示，否则为{}对象\"}"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  db = getParam_db(arguments, retval);
			if (db == nullptr)return true;
			sqlite3_close((sqlite3 *)db->GetIntValue());
			return true;
		}

		// 获取传入参数中的sql，获取成功函数返回true
		inline CefRefPtr<CefV8Value> getParam_sql(const CefV8ValueList& arguments, const CefRefPtr<CefV8Value>& retval) {
			CefRefPtr<CefV8Value>  sql = arguments[0]->GetValue("sql");
			if (!sql->IsString()) {// 因为sql参数是必须的，之前已经检测过是否存在，所以这里不进行null检查
				retval->SetValue("msg", CefV8Value::CreateString(L"sql参数必须是字符串"), V8_PROPERTY_ATTRIBUTE_NONE);
				sql = nullptr;
				return nullptr;
			}
			return sql;
		}

		// 执行sql语句，不支持参数传入
		bool JJH_DB_exec::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			if (arguments.size() < 1 || !arguments[0]->HasValue("db") || !arguments[0]->HasValue("sql")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"用法：JWebTop.db.exec({db:数据库句柄,sql:\"待执行的SQL\"})"
					L"\r\n返回值：{msg:\"出错时返回错误提示，否则为{}对象\"}"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  db=getParam_db(arguments, retval);
			if (db == nullptr)return true;
			CefRefPtr<CefV8Value>  sql = getParam_sql(arguments, retval);
			if (sql == nullptr)return true;

			// 函数回调：不支持callback了，因为和下面的query类似，没有必要搞一遍
			char *zErrMsg = 0;
			int rc = sqlite3_exec((sqlite3 *)db->GetIntValue(), sql->GetStringValue().ToString().c_str(), NULL, this, &zErrMsg);
			if (rc != SQLITE_OK) {
				retval->SetValue("msg", CefV8Value::CreateString(zErrMsg), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_free(zErrMsg);
			}
			return true;
		}

		// 为query函数设置参数
		inline bool setQueryParams(const CefRefPtr<CefV8Value> params, const CefRefPtr<CefV8Value>  retval, sqlite3_stmt *pStmt) {
			int len = params->GetArrayLength();
			CefRefPtr<CefV8Value>   tmp, typeJO; string str;
			for (int paramIdx = 0; paramIdx < len; paramIdx++) {
				tmp = params->GetValue(paramIdx);
				if (!tmp->IsArray() || tmp->GetArrayLength() < 2)continue;
				switch (tmp->GetValue(0)->GetIntValue()) {
				case SQLITE_INTEGER:						/*处理整型*/
					sqlite3_bind_int(pStmt, paramIdx + 1, tmp->GetValue(1)->GetIntValue());
					break;
				case SQLITE_FLOAT:							/*处理浮点数*/
					sqlite3_bind_double(pStmt, paramIdx + 1, tmp->GetValue(1)->GetDoubleValue());
					break;
				case SQLITE_TEXT:							/*处理字符串*/
					str = tmp->GetValue(1)->GetStringValue();
					sqlite3_bind_text(pStmt, paramIdx + 1, str.c_str(), str.length(), SQLITE_TRANSIENT);
					break;
					//case SQLITE_BLOB:							/*处理二进制*/
					//case SQLITE_NULL:							/*处理空*/
				default:
					wstringstream err;
					err << L"不支持[" << typeJO->GetIntValue() << L"]类型的参数";
					retval->SetValue("msg", CefV8Value::CreateString(err.str()), V8_PROPERTY_ATTRIBUTE_NONE);
					return false;
				}
			}
			return true;
		}

		bool SQLiteQueryBase::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			// 进行数据库操作之前检查各项参数是否正确
			if (!checkParams(object, arguments, retval))return true;
			CefRefPtr<CefV8Value>  db = getParam_db(arguments, retval);
			if (db == nullptr)return true;
			CefRefPtr<CefV8Value>  sql = getParam_sql(arguments, retval);
			if (sql == nullptr)return true;		
			CefRefPtr<CefV8Value>  params = arguments[0]->GetValue("params");
			if (params != NULL && !params->IsNull()) {
				params = arguments[0]->GetValue("params");
				if (!params->IsArray()) {
					retval->SetValue("msg", CefV8Value::CreateString(L"params参数必须是二维数组，比如：[[JWebTop.db.type.SQLITE_INTEGER,1],[JWebTop.db.type.SQLITE_FLOAT,1.234],[JWebTop.db.type.SQLITE_TEXT,\"string\"]]"), V8_PROPERTY_ATTRIBUTE_NONE);
					return true;
				}
			}
			// 开始进行数据操作
			sqlite3 * pDb = (sqlite3 *)db->GetIntValue();
			sqlite3_stmt *pStmt;
			int rc = sqlite3_prepare_v2(pDb, sql->GetStringValue().ToString().c_str(), -1, &pStmt, NULL);
			if (rc != SQLITE_OK) {
				wstringstream ss; ss << L"打开数据库失败：" << CefString(sqlite3_errmsg(pDb)).ToWString();
				retval->SetValue("msg", CefV8Value::CreateString(ss.str()), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_finalize(pStmt);
				return true;
			}
			if (params != NULL && !params->IsNull()) {// 绑定参数
				if (!setQueryParams(params, retval, pStmt)) {
					sqlite3_finalize(pStmt);
					return true;
				}
			}
			int ret = sqlite3_step(pStmt);
			if (ret != SQLITE_ROW) {// 对于DDL和DML语句而言，sqlite3_step执行正确的返回值只有SQLITE_DONE
				wstringstream ss; ss << L"执行【" << sql->GetStringValue().ToWString() << "】失败：" << CefString(sqlite3_errmsg(pDb)).ToWString();
				retval->SetValue("msg", CefV8Value::CreateString(ss.str()), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_finalize(pStmt);
				this->pStmt = NULL;
				return true;
			}
			//获取列数目
			this->n_columns = sqlite3_column_count(pStmt);
			this->row = -1;
			this->col = 0;
			// 先取一行数据，把类型建立起来，避免后面每次都去判断类型，因为一个sql中的所有类型都是一致的			
			this->colFuns = new outfun[n_columns];
			this->pStmt = pStmt;
			// 设置返回的表格列名称
			CefRefPtr<CefV8Value> columnNames = CefV8Value::CreateArray(n_columns);
			for (col = 0; col < n_columns; col++) {
				columnNames->SetValue(col, CefV8Value::CreateString(sqlite3_column_name(pStmt, col)));
			}
			onSQLExecuted(retval, columnNames);

			// 处理结果集
			if (ret == SQLITE_ROW) {
				onRowStart();
				// 处理每一列，为没列设置一个读取函数
				for (col = 0; col < n_columns; col++) {
					switch (sqlite3_column_type(pStmt, col)) {
					case SQLITE_INTEGER:						/*处理整型*/
						this->colFuns[col] = &SQLiteQueryBase::intOutFun;
						break;
					case SQLITE_FLOAT:							/*处理浮点数*/
						this->colFuns[col] = &SQLiteQueryBase::doubleOutFun;
						break;
					case SQLITE_TEXT:							/*处理字符串*/
						this->colFuns[col] = &SQLiteQueryBase::txtOutFun;
						break;
						//case SQLITE_BLOB:							/*处理二进制*/	sqlite3_column_blob(stmt, col));	break;
						//case SQLITE_NULL:							/*处理空*/				break;
						//default:break;
					}
					onColumnValue();
				}
			}
			do {
				ret = sqlite3_step(pStmt);
				if (ret == SQLITE_ROW) {// 还有需要处理的结果数据
					onRowStart();
					for (col = 0; col < n_columns; col++) {				//处理每一列		
						onColumnValue();
					}
				} else if (ret == SQLITE_DONE) //结束
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
				retval->SetValue("msg", CefV8Value::CreateString(L"用法：JWebTop.db.queryCallbcak({" 
					L"db:数据库句柄,"
					L"sql:\"待执行的SQL语句，可以有?等参数\","
					L"params:'（可选）[[JWebTop.db.type.SQLITE_TEXT,\"params参数必须是二维数组，按顺序设置到sql的每个 ? 上，按类型进行绑定\"],[JWebTop.db.type.SQLITE_INTEGER,1],[JWebTop.db.type.SQLITE_FLOAT,1.234]]',"
					L"names: printColumnNames(colNameArray){/* 用于接收结果集列名数组的函数 */},"
					L"values:printValues(rowNumber,colNumber,value){/* 用于接收结果集的函数 */}  "
					L"\r\n返回值：{msg:\"出错时返回错误提示，否则为{}对象\"}"
					L"})"), V8_PROPERTY_ATTRIBUTE_NONE);
				return false;
			}
			this->jsObject = object;// JS执行环境
			// 函数回调：输出查询元信息			
			this->jsColNamesFun = arguments[0]->GetValue("names");
			if (!this->jsColNamesFun->IsFunction()) {
				retval->SetValue("msg", CefV8Value::CreateString(L"names参数必须是function printColumnNames(colNameArray)签名的JS函数"), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_finalize(pStmt);
				return false;
			}
			// 函数回调：用于输出结果集
			this->jsPrintQueryValues = arguments[0]->GetValue("values");
			if (!this->jsPrintQueryValues->IsFunction()) {
				retval->SetValue("msg", CefV8Value::CreateString(L"values参数必须是function printValues(rowNumber,colNumber,value)签名的JS函数"), V8_PROPERTY_ATTRIBUTE_NONE);
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
				retval->SetValue("msg", CefV8Value::CreateString(L"用法：JWebTop.db.queryDataSet({"
					L"db:数据库句柄,"
					L"sql:\"待执行的SQL语句，可以有?等参数\","
					L"params:'（可选）[[JWebTop.db.type.SQLITE_TEXT,\"params参数必须是二维数组，按顺序设置到sql的每个 ? 上，按类型进行绑定\"],[JWebTop.db.type.SQLITE_INTEGER,1],[JWebTop.db.type.SQLITE_FLOAT,1.234]]',"
					L"\r\n返回值：{msg:\"出错时返回错误提示，否则无此属性\",columnNames:[/*结果集列名称数组*/],dataset:[ [], [], [/*结果集*/] ]}"
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

		// 批量执行SQL语句，不返回任何结果集
		bool JJH_DB_batch::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			if (arguments.size() < 1 || !arguments[0]->HasValue("db") || !arguments[0]->HasValue("sql")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"用法：JWebTop.db.batch({"
					L"db: 数据库句柄,"
					L"sql: \"待执行的SQL语句，可以有?等参数\","
					L"paramsTypes: '（可选）paramsTypes参数必须是数组，比如：[JWebTop.db.type.SQLITE_INTEGER,JWebTop.db.type.SQLITE_FLOAT,JWebTop.db.type.SQLITE_TEXT]',"
					L"paramValues: 'paramValues参数必须是二维数组，比如：[[1,1.234,\"string1\"]  ,[2,2.234,\"string2\"] ]' },"
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
					retval->SetValue("msg", CefV8Value::CreateString(L"paramsTypes参数必须是数组，比如：[JWebTop.db.type.SQLITE_INTEGER,JWebTop.db.type.SQLITE_FLOAT,JWebTop.db.type.SQLITE_TEXT]"), V8_PROPERTY_ATTRIBUTE_NONE);
					return true;
				} else {
					int typeLen = paramsTypes->GetArrayLength();
					CefRefPtr<CefV8Value>  tmpColType;
					for (int col = 0; col < typeLen; col++) {// 首先检查所有传入类型是否正确
						tmpColType = paramsTypes->GetValue(col);
						switch (tmpColType->GetIntValue()) {
						case SQLITE_TEXT:							/*处理字符串*/
							break;
						case SQLITE_INTEGER:						/*处理整型*/
							break;
						case SQLITE_FLOAT:							/*处理浮点数*/
							break;
							//case SQLITE_BLOB:							/*处理二进制*/
							//case SQLITE_NULL:							/*处理空*/
						default:
							ss << L"不支持[" << tmpColType->GetIntValue() << L"]类型的参数";
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
					retval->SetValue("msg", CefV8Value::CreateString(L"paramValues参数必须是二维数组，比如：[[1,1.234,\"string1\"]  ,[2,2.234,\"string2\"] ] "), V8_PROPERTY_ATTRIBUTE_NONE);
					return true;
				}
			}
			sqlite3 * pDb = (sqlite3 *)db->GetIntValue();
			// 批量插入数据时，开启事务有助于提高速度
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
				ss << L"初始化【" << sql->GetStringValue().ToWString() << "】失败：" << CefString(sqlite3_errmsg(pDb)).ToWString();
				retval->SetValue("msg", CefV8Value::CreateString(ss.str()), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_finalize(pStmt);
				return true;
			}
			bool success = true;
			if (paramValues != NULL) {// 绑定参数
				int batchCount = paramValues->GetArrayLength();
				int typeLen = paramsTypes->GetArrayLength(), rowParamsLen = 0;
				CefRefPtr<CefV8Value> rowParams , tmpColType;				
				string str;
				for (int row = 0; row < batchCount; row++) {
					rowParams = paramValues->GetValue(row);
					rowParamsLen = rowParams->GetArrayLength();
					if (rowParamsLen > typeLen)rowParamsLen = typeLen;// 按最小的数组长度来设置值
					for (int col = 0; col < rowParamsLen; col++) {
						tmpColType = paramsTypes->GetValue(col);
						switch (tmpColType->GetIntValue()) {
						case SQLITE_TEXT:							/*处理字符串*/
							str = rowParams->GetValue(col)->GetStringValue();
							sqlite3_bind_text(pStmt, col + 1, str.c_str(), str.length(), SQLITE_TRANSIENT);
							break;
						case SQLITE_INTEGER:						/*处理整型*/
							sqlite3_bind_int(pStmt, col + 1, rowParams->GetValue(col)->GetIntValue());
							break;
						case SQLITE_FLOAT:							/*处理浮点数*/
							sqlite3_bind_double(pStmt, col + 1, rowParams->GetValue(col)->GetDoubleValue());
							break;
							//case SQLITE_BLOB:							/*处理二进制*/
							//case SQLITE_NULL:							/*处理空*/
						}
					}
					int ret = sqlite3_step(pStmt);
					if (ret != SQLITE_DONE) {// 出错了？？？
						success = false;
						ss << L"执行【" << sql->GetStringValue().ToWString() << "】失败：" << CefString(sqlite3_errmsg(pDb)).ToWString() << L"   row=" << row;
						break;
					}
					sqlite3_reset(pStmt);// 重新初始化该sqlite3_stmt对象绑定的变量。
				}
			} else {
				int ret = sqlite3_step(pStmt);
				if (ret != SQLITE_DONE) {
					success = false;
					ss << L"执行【" << sql->GetStringValue().ToWString() << "】失败：" << CefString(sqlite3_errmsg(pDb)).ToWString();
				}
			}
			sqlite3_finalize(pStmt);
			if (success) {// 之前执行没有出错，批量提交之前的事务
				rc = sqlite3_exec(pDb, "COMMIT", NULL, this, &zErrMsg);
				if (rc != SQLITE_OK) {
					ss << L"提交事务失败：" << zErrMsg;
					sqlite3_free(zErrMsg);
					rc = sqlite3_exec(pDb, "ROLLBACK", NULL, this, &zErrMsg);// 需要回滚事务
					if (rc != SQLITE_OK) {
						ss << L"  事务回滚失败：" << zErrMsg;
					} else {
						ss << L"  事务已回滚。";
					}
					sqlite3_free(zErrMsg);
					retval->SetValue("msg", CefV8Value::CreateString(ss.str()), V8_PROPERTY_ATTRIBUTE_NONE);
				}
			} else {// 之前执行出错，进行回滚
				rc = sqlite3_exec(pDb, "ROLLBACK", NULL, this, &zErrMsg);// 需要回滚事务
				if (rc != SQLITE_OK) {
					ss << L"  事务回滚失败：" << zErrMsg;
				} else {
					ss << L"  事务已回滚。";
				}
				sqlite3_free(zErrMsg);
				retval->SetValue("msg", CefV8Value::CreateString(ss.str()), V8_PROPERTY_ATTRIBUTE_NONE);
			}
			return true;
		}
	}// End namespace:db
}// End namespace:jw