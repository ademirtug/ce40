

#include "stdafx.h"
#include "CECommand.h"
#include "CEDataReader.h"

CECommand::CECommand(wstring cmdText)
{
	open = false;
	cmd_text = cmdText;
	propset.SetGUID(DBPROPSET_ROWSET);
	propset.AddProperty(DBPROP_IColumnsRowset, true);
}
CECommand::CECommand(wstring cmdText, shared_ptr<CEConnection> _connection)
{
	open = false;
	cmd_text = cmdText;
	connection = _connection;
	propset.SetGUID(DBPROPSET_ROWSET);
	propset.AddProperty(DBPROP_IColumnsRowset, true);
}
CECommand::CECommand(wstring cmdText, shared_ptr<CEConnection> _connection, shared_ptr<CETransaction> _transaction)
{
	open = false;
	cmd_text = cmdText;
	connection = _connection;
	transaction = _transaction;
	propset.SetGUID(DBPROPSET_ROWSET);
	propset.AddProperty(DBPROP_IColumnsRowset, true);
}
CECommand::~CECommand()
{
	open = false;
	command.Close();
}

void CECommand::Open()
{
	if( !init() )
		return;

	if( open )
		return;

	HRESULT hr = S_OK;

	hr = command.Open(connection->GetSession(), cmd_text.c_str(), &propset, NULL, DBGUID_DEFAULT, true, 1);

	if( SUCCEEDED(hr) )
		open = true;
	else
		throw CEException(L"CECommand::Open Failed.", true); 
}
int CECommand::ExecuteNonQuery()
{
	init();
	
	HRESULT hr = S_OK;

	hr = command.Open(connection->GetSession(), cmd_text.c_str() );
	
	command.Close();

	if( FAILED(hr) ) 
		throw CEException(L"CECommand::ExecuteNonQuery Failed!", true);

	return 1;
}
shared_ptr<DbDataReader> CECommand::ExecuteReader()
{
	init();

	HRESULT hr = S_OK;
	shared_ptr<CEDataReader> rdr(new CEDataReader());
	
	Open();

	if( FAILED(hr) )
		throw CEException(L"CECommand::ExecuteReader Failed, can't instantiate CEDataReader", true);

	rdr->command = shared_from_this();
	return rdr;
}
void CECommand::Prepare()
{
}
wstring CECommand::ExecuteScalar()
{
	return L"";
}

bool CECommand::init()
{
	//if( open )
	//	throw std::exception("CECommand is already open");

	if( cmd_text.size() < 1 )
		throw std::exception("you have to specifiy a command");

	if( connection == NULL )
		throw std::exception("this function requires a connection");

	if( connection->State() != ConnectionState::Open )
		throw std::exception("this function requires open connection to the data source");

	return true;
}


void CECommand::CommandText(wstring cmdText)
{
	if( cmd_text.size() < 1 || open == false )
	{
		cmd_text = cmdText;
		return;
	}

	command.Close();
	open = false;
	cmd_text = cmdText;

}
wstring CECommand::CommandText()
{
	return cmd_text;
}

void CECommand::Connection(shared_ptr<DbConnection> _conn)
{
	connection = dynamic_pointer_cast<CEConnection>(_conn);
}
shared_ptr<DbConnection> CECommand::Connection()
{
	return connection;
}

void CECommand::Transaction(shared_ptr<DbTransaction> _trans)
{
	transaction = dynamic_pointer_cast<CETransaction>(_trans);
}
shared_ptr<DbTransaction> CECommand::Transaction()
{
	return transaction;
}





DbCommand::DbCommand(std::wstring cmdText)
{
}

DbCommand::DbCommand(std::wstring cmdText, std::tr1::shared_ptr<DbConnection> _connection)
{
}
DbCommand::DbCommand(std::wstring cmdText, std::tr1::shared_ptr<DbConnection> _connection, std::tr1::shared_ptr<DbTransaction> _transaction)
{
}
DbCommand::~DbCommand()
{
}