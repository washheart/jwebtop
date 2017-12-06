#include "ex_sqlite.h"
#include <functional>
#include <sstream>
#include "common/util/StrUtil.h"
namespace jw {
	namespace db {

		bool JJH_DB_open::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			if (arguments.size() < 1 || !arguments[0]->HasValue("dbpath")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"用法：JWebTop.db.open({dbpath:\"数据库路径，可以是绝对路径或相对于运行目录的相对路径\"});"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  dbpath = arguments[0]->GetValue("dbpath");
			if (!dbpath->IsString()) {
				retval->SetValue("msg", CefV8Value::CreateString(L"dbpath参数必须是字符串"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			sqlite3 *db;
			char *zErrMsg = 0;
			int rc;
			// 打开数据库
			rc = sqlite3_open_v2(dbpath->GetStringValue().ToString().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
			if (rc) {
				string err = "打开数据库失败：";
				err.append(sqlite3_errmsg(db));
				retval->SetValue("msg", CefV8Value::CreateString(jw::s2w(err)), V8_PROPERTY_ATTRIBUTE_NONE);
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
				retval->SetValue("msg", CefV8Value::CreateString(L"用法：JWebTop.db.close({db:数据库句柄})"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  db = getParam_db(arguments, retval);
			if (db == nullptr)return true;
			sqlite3_close((sqlite3 *)db->GetIntValue());
			return true;
		}

		// 获取传入参数中的sql，获取成功函数返回true
		inline CefRefPtr<CefV8Value>   getParam_sql(const CefV8ValueList& arguments, const CefRefPtr<CefV8Value>& retval) {
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
				retval->SetValue("msg", CefV8Value::CreateString(L"用法：JWebTop.db.exec({db:数据库句柄,sql:\"待执行的SQL\"})"), V8_PROPERTY_ATTRIBUTE_NONE);
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
			int type = 0;
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

		// 执行查询，并通过回调函数返回结果集
		bool JJH_DB_queryCallbcak::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			if (arguments.size() < 1 || !arguments[0]->HasValue("db") || !arguments[0]->HasValue("sql")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"用法：JWebTop.db.queryCallbcak({db:数据库句柄,sql:\"待执行的SQL语句，可以有?等参数\",params:'（可选）[[JWebTop.db.type.SQLITE_TEXT,\"params参数必须是二维数组，按顺序设置到sql的每个 ? 上，按类型进行绑定\"],[JWebTop.db.type.SQLITE_INTEGER,1],[JWebTop.db.type.SQLITE_FLOAT,1.234]]',names: printColumnNames(colNameArray){/*（可选）用于接收结果集列名数组的函数*/},values:printValues(rowNumber,colNumber,value){/*（可选）用于接收结果集的函数*/}  })"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  db = getParam_db(arguments, retval);
			if (db == nullptr)return true;
			CefRefPtr<CefV8Value>  sql = getParam_sql(arguments, retval);
			if (sql == nullptr)return true;

			CefRefPtr<CefV8Value>  params;
			if (arguments[0]->HasValue("params")) {
				params = arguments[0]->GetValue("params");
				if (!params->IsArray()) {
					retval->SetValue("msg", CefV8Value::CreateString(L"params参数必须是二维数组，比如：[[JWebTop.db.type.SQLITE_INTEGER,1],[JWebTop.db.type.SQLITE_FLOAT,1.234],[JWebTop.db.type.SQLITE_TEXT,\"string\"]]"), V8_PROPERTY_ATTRIBUTE_NONE);
					return true;
				}
			}
			sqlite3 * pDb = (sqlite3 *)db->GetIntValue();
			sqlite3_stmt *pStmt;
			int rc = sqlite3_prepare_v2(pDb, sql->GetStringValue().ToString().c_str(), -1, &pStmt, NULL);
			if (rc != SQLITE_OK) {
				wstringstream ss; ss << L"打开数据库失败：" << sqlite3_errmsg(pDb);
				retval->SetValue("msg", CefV8Value::CreateString(ss.str()), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_finalize(pStmt);
				return true;
			}
			if (params != NULL) {// 绑定参数
				if (setQueryParams(params, retval, pStmt)) {
					sqlite3_finalize(pStmt);
					return true;
				}
			}
			int ret = sqlite3_step(pStmt);
			if (ret == SQLITE_DONE) {// 对于DDL和DML语句而言，sqlite3_step执行正确的返回值只有SQLITE_DONE
				sqlite3_finalize(pStmt);
				return true;
			}
			// 函数回调：输出查询元信息（行数、列数、列名）
			CefRefPtr<CefV8Value> jsColNamesFun;
			if (arguments[0]->HasValue("names")) {
				jsColNamesFun = arguments[0]->GetValue("names");
				if (!jsColNamesFun->IsFunction()) {
					retval->SetValue("msg", CefV8Value::CreateString(L"names参数必须是function printColumnNames(colNameArray)签名的JS函数"), V8_PROPERTY_ATTRIBUTE_NONE);
					sqlite3_finalize(pStmt);
					return true;
				}
				this->jsObject = object;
			}
			// 函数回调
			if (arguments[0]->HasValue("values")) {
				this->jsPrintQueryValues = arguments[0]->GetValue("values");
				if (!this->jsPrintQueryValues->IsFunction()) {
					retval->SetValue("msg", CefV8Value::CreateString(L"values参数必须是function printValues(rowNumber,colNumber,value)签名的JS函数"), V8_PROPERTY_ATTRIBUTE_NONE);
					sqlite3_finalize(pStmt);
					return true;
				}
				this->jsObject = object;
			}
			if (jsColNamesFun == NULL&&this->jsPrintQueryValues == NULL) {// 如果没有回调函数，则认为不需要返回结果，可以直接把函数结束了
				sqlite3_finalize(pStmt);
				return true;
			}
			//获取列数目
			int n_columns = sqlite3_column_count(pStmt);
			this->row = 0;
			this->col = 0;
			// 先取一行数据，把类型建立起来，避免后面每次都去判断类型，因为一个sql中的所有类型都是一致的			
			this->colFuns = new outfun[n_columns];
			this->pStmt = pStmt;
			if (ret == SQLITE_ROW) {
				if (jsColNamesFun != NULL) {
					CefV8ValueList args;
					CefRefPtr<CefV8Value> arr = CefV8Value::CreateArray(n_columns);
					for (col = 0; col < n_columns; col++) {				;
						arr->SetValue(col, CefV8Value::CreateString(sqlite3_column_name(pStmt, col)));
					}
					args.push_back(arr);
					jsColNamesFun->ExecuteFunction(this->jsObject, args);
				}
				// 处理每一列
				for (col = 0; col < n_columns; col++) {
					switch (sqlite3_column_type(pStmt, col)) {
					case SQLITE_INTEGER:						/*处理整型*/
						this->colFuns[col] = &JJH_DB_queryCallbcak::intOutFun;
						break;
					case SQLITE_FLOAT:							/*处理浮点数*/
						this->colFuns[col] = &JJH_DB_queryCallbcak::doubleOutFun;
						break;
					case SQLITE_TEXT:							/*处理字符串*/
						this->colFuns[col] = &JJH_DB_queryCallbcak::txtOutFun;
						break;
						//case SQLITE_BLOB:							/*处理二进制*/	sqlite3_column_blob(stmt, col));	break;
						//case SQLITE_NULL:							/*处理空*/				break;
						//default:break;
					}
					outputOneRowCol(this->colFuns[col]);
				}
				this->row++;
			}
			do {
				ret = sqlite3_step(pStmt);
				if (ret == SQLITE_ROW) {// 还有需要处理的结果数据				
					for (col = 0; col < n_columns; col++) {				//处理每一列		
						outputOneRowCol(this->colFuns[col]);
					}
					this->row++;
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
		inline bool JJH_DB_queryCallbcak::outputOneRowCol(CefRefPtr<CefV8Value>(JJH_DB_queryCallbcak::* outfun)(CefV8ValueList)) {
			CefV8ValueList args;
			args.push_back(CefV8Value::CreateInt(this->row));
			args.push_back(CefV8Value::CreateInt(col));
			args.push_back((this->*outfun)(args));
			this->jsPrintQueryValues->ExecuteFunction(this->jsObject, args);
			return true;
		}

		// 执行查询，并通过回调函数返回结果集
		bool JJH_DB_query::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			if (arguments.size() < 1 || !arguments[0]->HasValue("db") || !arguments[0]->HasValue("sql")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"用法：JWebTop.db.exex({db:数据库句柄,sql:\"待执行的SQL语句，可以有?等参数\",params:'（可选）[[JWebTop.db.type.SQLITE_TEXT,\"params参数必须是二维数组，按顺序设置到sql的每个 ? 上，按类型进行绑定\"],[JWebTop.db.type.SQLITE_INTEGER,1],[JWebTop.db.type.SQLITE_FLOAT,1.234]]',names: printColumnNames(colNameArray){/*（可选）用于接收结果集列名数组的函数*/},values:printValues(rowNumber,colNumber,value){/*（可选）用于接收结果集的函数*/}  })"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  db = getParam_db(arguments, retval);
			if (db == nullptr)return true;
			CefRefPtr<CefV8Value>  sql = getParam_sql(arguments, retval);
			if (sql == nullptr)return true;

			CefRefPtr<CefV8Value>  params;
			if (arguments[0]->HasValue("params")) {
				params = arguments[0]->GetValue("params");
				if (!params->IsArray()) {
					retval->SetValue("msg", CefV8Value::CreateString(L"params参数必须是二维数组，比如：[[JWebTop.db.type.SQLITE_INTEGER,1],[JWebTop.db.type.SQLITE_FLOAT,1.234],[JWebTop.db.type.SQLITE_TEXT,\"string\"]]"), V8_PROPERTY_ATTRIBUTE_NONE);
					return true;
				}
			}
			sqlite3 * pDb = (sqlite3 *)db->GetIntValue();
			sqlite3_stmt *pStmt;
			int rc = sqlite3_prepare_v2(pDb, sql->GetStringValue().ToString().c_str(), -1, &pStmt, NULL);
			if (rc != SQLITE_OK) {
				wstringstream ss; ss << L"打开数据库失败：" << sqlite3_errmsg(pDb);
				retval->SetValue("msg", CefV8Value::CreateString(ss.str()), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_finalize(pStmt);
				return true;
			}
			if (params != NULL) {// 绑定参数
				if (setQueryParams(params, retval, pStmt)) {
					sqlite3_finalize(pStmt);
					return true;
				}
			}
			int ret = sqlite3_step(pStmt);
			if (ret == SQLITE_DONE) {// 对于DDL和DML语句而言，sqlite3_step执行正确的返回值只有SQLITE_DONE
				sqlite3_finalize(pStmt);
				return true;
			}
			// 函数回调：输出查询元信息（行数、列数、列名）
			// 函数回调
			//获取列数目
			int n_columns = sqlite3_column_count(pStmt);
			this->row = 0;
			this->col = 0;
			// 先取一行数据，把类型建立起来，避免后面每次都去判断类型，因为一个sql中的所有类型都是一致的			
			this->colFuns = new outfun[n_columns];
			this->pStmt = pStmt;
			// 设置返回的表格列名称
			CefRefPtr<CefV8Value> columnNames = CefV8Value::CreateArray(n_columns);
			for (col = 0; col < n_columns; col++) {
				columnNames->SetValue(col, CefV8Value::CreateString(sqlite3_column_name(pStmt, col)));
			}
			retval->SetValue("columnNames", columnNames, V8_PROPERTY_ATTRIBUTE_NONE);

			// 处理结果集
			CefRefPtr<CefV8Value> dataset = CefV8Value::CreateArray(n_columns);
			retval->SetValue("dataset", dataset, V8_PROPERTY_ATTRIBUTE_NONE);
			if (ret == SQLITE_ROW) {
				CefRefPtr<CefV8Value> arr = CefV8Value::CreateArray(n_columns);
				// 处理每一列，为没列设置一个读取函数
				for (col = 0; col < n_columns; col++) {
					switch (sqlite3_column_type(pStmt, col)) {
					case SQLITE_INTEGER:						/*处理整型*/
						this->colFuns[col] = &JJH_DB_query::intOutFun;
						break;
					case SQLITE_FLOAT:							/*处理浮点数*/
						this->colFuns[col] = &JJH_DB_query::doubleOutFun;
						break;
					case SQLITE_TEXT:							/*处理字符串*/
						this->colFuns[col] = &JJH_DB_query::txtOutFun;
						break;
						//case SQLITE_BLOB:							/*处理二进制*/	sqlite3_column_blob(stmt, col));	break;
						//case SQLITE_NULL:							/*处理空*/				break;
						//default:break;
					}
					//outputOneRowCol(this->colFuns[col],arr);
					arr->SetValue(this->col, (this->*colFuns[col])());
				}
				dataset->SetValue(this->row++, arr);
			}
			do {
				ret = sqlite3_step(pStmt);
				if (ret == SQLITE_ROW) {// 还有需要处理的结果数据	
					CefRefPtr<CefV8Value> arr = CefV8Value::CreateArray(n_columns);
					for (col = 0; col < n_columns; col++) {				//处理每一列		
						//outputOneRowCol(this->colFuns[col],arr);
						arr->SetValue(this->col, (this->*colFuns[col])());
					}
					dataset->SetValue(this->row++, arr);
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
		//inline bool JJH_DB_query::outputOneRowCol(CefRefPtr<CefV8Value>(JJH_DB_query::* outfun)(), CefRefPtr<CefV8Value> arr) {
		//	arr->SetValue(this->col, (this->*outfun)());
		//	return true;
		//}

		// 批量执行SQL语句，不返回任何结果集
		bool JJH_DB_batch::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
			retval = CefV8Value::CreateObject(NULL);
			if (arguments.size() < 1 || !arguments[0]->HasValue("db") || !arguments[0]->HasValue("sql")) {
				retval->SetValue("msg", CefV8Value::CreateString(L"用法：JWebTop.db.batch({db:数据库句柄,sql:\"待执行的SQL语句，可以有?等参数\",params:'（可选）[[JWebTop.db.type.SQLITE_TEXT,\"params参数必须是二维数组，按顺序设置到sql的每个 ? 上，按类型进行绑定\"],[JWebTop.db.type.SQLITE_INTEGER,1],[JWebTop.db.type.SQLITE_FLOAT,1.234]]',names: printColumnNames(colNameArray){/*（可选）用于接收结果集列名数组的函数*/},values:printValues(rowNumber,colNumber,value){/*（可选）用于接收结果集的函数*/}  })"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
			CefRefPtr<CefV8Value>  db = getParam_db(arguments, retval);
			if (db == nullptr)return true;
			CefRefPtr<CefV8Value>  sql = getParam_sql(arguments, retval);
			if (sql == nullptr)return true;
			CefRefPtr<CefV8Value>  paramsTypes;
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
							wstringstream err;
							err << L"不支持[" << tmpColType->GetIntValue() << L"]类型的参数";
							retval->SetValue("msg", CefV8Value::CreateString(err.str()), V8_PROPERTY_ATTRIBUTE_NONE);
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
				wstringstream ss; ss << L"初始化【" << sql << "】失败：" << sqlite3_errmsg(pDb);
				retval->SetValue("msg", CefV8Value::CreateString(ss.str()), V8_PROPERTY_ATTRIBUTE_NONE);
				sqlite3_finalize(pStmt);
				return true;
			}
			bool success = true;
			wstringstream ss;
			if (paramValues != NULL) {// 绑定参数
				int batchCount = paramValues->GetArrayLength();
				int typeLen = paramsTypes->GetArrayLength(), minLen = 0, rowParamsLen = 0;
				CefRefPtr<CefV8Value> rowParams , tmpColType;				
				string str;
				for (int row = 0; row < batchCount; row++) {
					rowParams = paramValues->GetValue(row);
					rowParamsLen = rowParams->GetArrayLength();
					if (rowParamsLen > typeLen)rowParamsLen = typeLen;// 按最小的数组长度来设置值
					for (int col = 0; col < minLen; col++) {
						tmpColType = paramsTypes->GetValue(col);
						switch (tmpColType->GetIntValue()) {
						case SQLITE_TEXT:							/*处理字符串*/
							str = rowParams->GetValue(col)->GetValue(1)->GetStringValue();
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
						sqlite3_finalize(pStmt);
						ss << L"执行【" << sql << "】失败：" << sqlite3_errmsg(pDb) << L"   row=" << row;
					}				
				}
			} else {
				int ret = sqlite3_step(pStmt);
				if (ret != SQLITE_DONE) {
					success = false;
					ss << L"执行【" << sql << "】失败：" << sqlite3_errmsg(pDb);
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