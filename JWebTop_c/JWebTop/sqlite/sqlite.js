function sqlitedb(){
	this.INT=JWebTop.db.type.SQLITE_INTEGER;
	this.FLOAT=JWebTop.db.type.SQLITE_FLOAT;
	this.TEXT=JWebTop.db.type.SQLITE_TEXT;

	this.dbHandle=null;
	this.open=function(dbpath){ 
		var db=JWebTop.db.open({dbpath:dbpath});
		this.dbHandle=db.db;
		return db;
	}
	this.close=function(){ 
		return JWebTop.db.close({db:this.dbHandle});
	}
	this.exec=function(sql){
		return JWebTop.db.exec({db:this.dbHandle , sql:sql });		
	}
	this.queryCallback=function(sql,params,namesFun,valuesFun){
		return JWebTop.db.queryCallback({db:this.dbHandle , sql:sql ,params:params ,names:namesFun,values:valuesFun});		
	}
	this.queryDataSet=function(sql,params){
		if(!params)params=[];
		return JWebTop.db.queryDataSet({db:this.dbHandle , sql:sql ,params:params});
	}
	this.batch=function(sql,paramsTypes,paramValues){
		return JWebTop.db.batch({db:this.dbHandle , sql:sql ,paramsTypes:paramsTypes ,paramValues:paramValues});
	}
}
