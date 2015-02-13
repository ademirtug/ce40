


#ifndef CECOMMAND_H
#define CECOMMAND_H

#include "DataProvider.h"
#include "CEConnection.h"
#include "CETransaction.h"


class CECommand : public DbCommand, public std::tr1::enable_shared_from_this<CECommand>
{
	CDBPropSet propset;
	wstring cmd_text;
	bool open;

	bool init();
public:
	shared_ptr<CEConnection> connection;
	shared_ptr<CETransaction> transaction;
	CCommand<CDynamicAccessor, CRowset> command;


	void Prepare();
	void Open();

	int ExecuteNonQuery();
	shared_ptr<DbDataReader> ExecuteReader();
	wstring ExecuteScalar();
	
	void CommandText(wstring cmdText);
	wstring CommandText();

	void Connection(shared_ptr<DbConnection> _conn);
	shared_ptr<DbConnection> Connection();

	void Transaction(shared_ptr<DbTransaction> _trans);
	shared_ptr<DbTransaction> Transaction();
	
	CECommand(wstring cmdText = L"");
	CECommand(wstring cmdText, shared_ptr<CEConnection> _connection);
	CECommand(wstring cmdText, shared_ptr<CEConnection> _connection, shared_ptr<CETransaction> _transaction);
	virtual ~CECommand();
};

#endif//CECOMMAND_H