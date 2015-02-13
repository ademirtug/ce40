


#ifndef DATA_PROVIDER_H
#define DATA_PROVIDER_H

#include "DataTable.h"
#include <memory>
#include <time.h>


using namespace std::tr1;

struct ConnectionState
{
	enum __connectionstates
	{
		Closed = 0,
		Open = 1,
		Connecting = 2,
		Executing = 3,
		Fetching = 4,
		Broken = 5
	};
};


#define ST_SHORT	1
#define ST_INT		2
#define ST_LONG		3
#define ST_FLOAT	4
#define ST_DOUBLE	5
#define ST_INT64	6

#define ST_BOOL		9
#define ST_CHAR		10
#define ST_VARCHAR	11
#define ST_NCHAR	12
#define ST_NVARCHAR 13

#define ST_DATE		20
#define ST_TIME		21
#define ST_DATETIME	22
#define ST_BLOB		23
#define ST_CURRENCY 24

#define ST_NULL		40
#define ST_UNKNOWN	50


struct IsolationLevel
{
	enum{
		NONE = 0,
		READ_COMMITTED	= 1,
		READ_UNCOMMITTED = 2,
		REPEATABLE_READ	= 3,
		SERIALIZABLE = 4
	};
};


class DbConnection;
class DbDataReader;
class DbTransaction;

class  DbConnection
{
public:
	virtual void ConnectionString(wstring cnn_str) = 0;
	virtual wstring ConnectionString() = 0;

	virtual int State() = 0;

	virtual void Open() = 0;
	virtual void Close() = 0;

	virtual shared_ptr<DbTransaction> BeginTransaction(int _IsolationLevel = IsolationLevel::READ_COMMITTED) = 0;

	DbConnection();
	DbConnection(wstring cnn_str);
	virtual ~DbConnection();
};


class DbTransaction
{
protected:
	DbTransaction();
public:
	virtual void Commit() = 0;
	virtual void Rollback() = 0;
	virtual bool IsStarted() = 0;

	virtual ~DbTransaction();
};

class DbDataReader;

class DbCommand
{
public:
	DbCommand(wstring cmdText = L"");
	DbCommand(wstring cmdText, shared_ptr<DbConnection> _connection);
	DbCommand(wstring cmdText, shared_ptr<DbConnection> _connection, shared_ptr<DbTransaction> _transaction);
	
	virtual void Prepare() = 0;

	virtual int ExecuteNonQuery() = 0;
	virtual shared_ptr<DbDataReader> ExecuteReader() = 0;
	virtual wstring ExecuteScalar() = 0;
	
	virtual void CommandText(wstring cmdText) = 0;
	virtual wstring CommandText() = 0;

	virtual void Connection(shared_ptr<DbConnection> _conn) = 0;
	virtual shared_ptr<DbConnection> Connection() = 0;

	virtual void Transaction(shared_ptr<DbTransaction> _trans) = 0;
	virtual shared_ptr<DbTransaction> Transaction() = 0;
	
	virtual ~DbCommand();
};
class  DbDataAdapter
{
protected:
	DbDataAdapter();
public:
	virtual int Fill(DataTable& table) = 0;
	virtual void FillSchema(DataTable& table) = 0;

	virtual int Update(DataTable& table) = 0;
	

	DbDataAdapter(DbCommand* command); 
	virtual ~DbDataAdapter();
};

class  DbDataReader
{
protected:
	DbDataReader();
public:
	virtual unsigned int FieldCount() = 0;
	virtual bool Read() = 0;
	virtual void Close() = 0;

	virtual bool GetBoolean(unsigned int index) = 0;
	virtual double GetDouble(unsigned int index) = 0;
	virtual float GetFloat(unsigned int index) = 0;
	virtual short GetInt16(unsigned int index) = 0;
	virtual int GetInt32(unsigned int index) = 0;
	virtual long GetInt64(unsigned int index) = 0;
	virtual std::tm GetDate(unsigned int index) = 0;
	virtual wstring GetString(unsigned int index) = 0;

	virtual short GetInt16(const wstring& name) = 0;
	virtual int	GetInt32(const wstring& name) = 0;
	virtual long GetInt64(const wstring& name) = 0;
	virtual bool GetBoolean(const wstring& name) = 0;
	virtual float GetFloat(const wstring& name) = 0;
	virtual double GetDouble(const wstring& name) = 0;
	virtual std::tm GetDate(const wstring& name) = 0;
	virtual wstring GetString(const wstring& name) = 0;

	//Some Metadata
	virtual wstring GetName(unsigned int index) = 0;//Sql Name
	virtual unsigned int GetFieldType(unsigned int index) = 0;
	virtual unsigned int GetOrdinal(const wstring& name) = 0;
	virtual bool IsDBNull(unsigned int index) = 0;

	virtual DataTable GetSchemaTable() = 0;

	virtual wstring operator[](unsigned int index) = 0;
	virtual wstring operator[](wstring name) = 0;

	virtual ~DbDataReader();
};



//Data Provider Base Class
class  DataProvider
{
public:
	//Database Object Generators
	virtual shared_ptr<DbConnection> GetConnection(const wstring& _cnn_str = L"" ) = 0;

	virtual shared_ptr<DbCommand> GetCommand(wstring sql) = 0; 
	virtual shared_ptr<DbCommand> GetCommand(wstring sql, shared_ptr<DbConnection> _cnn) = 0;
	virtual shared_ptr<DbCommand> GetCommand(wstring sql, shared_ptr<DbConnection> _cnn, shared_ptr<DbTransaction> _tr) = 0;

	virtual shared_ptr<DbTransaction> GetTransaction(shared_ptr<DbConnection> _cnn, int isolation = 1) = 0;

	virtual shared_ptr<DbDataAdapter> GetAdapter(shared_ptr<DbCommand> _cmd) = 0;

	DataProvider(){};
	virtual ~DataProvider(){};
};

#endif//DATA_PROVIDER_H