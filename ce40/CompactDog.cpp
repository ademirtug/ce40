

#include "stdafx.h"
#include "CompactDog.h"

#include "CEConnection.h"
#include "CECommand.h"
#include "CEDataAdapter.h"
#include "CEDataReader.h"

CompactDog::CompactDog() : initialised(false)
{

}
CompactDog::~CompactDog()
{

}

//Database Object Generators
shared_ptr<DbConnection> CompactDog::GetConnection(const wstring& _cnn_str)
{
	shared_ptr<DbConnection> conn(new CEConnection(_cnn_str) );
	return conn;
}

shared_ptr<DbCommand> CompactDog::GetCommand(wstring sql)
{
	shared_ptr<DbCommand> cmd(new CECommand(sql));
	return cmd;
} 
shared_ptr<DbCommand> CompactDog::GetCommand(wstring sql, shared_ptr<DbConnection> _cnn)
{
	shared_ptr<DbCommand> cmd(new CECommand(sql, dynamic_pointer_cast<CEConnection>(_cnn)));
	return cmd;
}
shared_ptr<DbCommand> CompactDog::GetCommand(wstring sql, shared_ptr<DbConnection> _cnn, shared_ptr<DbTransaction> _tr)
{
	shared_ptr<DbCommand> cmd(new CECommand(sql, dynamic_pointer_cast<CEConnection>(_cnn),
		dynamic_pointer_cast<CETransaction>(_tr)));
	return cmd;
}
shared_ptr<DbTransaction> CompactDog::GetTransaction(shared_ptr<DbConnection> _cnn, int isolation)
{
	shared_ptr<DbTransaction> tr(new CETransaction(dynamic_pointer_cast<CEConnection>(_cnn), isolation));
	return tr;
}
shared_ptr<DbDataAdapter> CompactDog::GetAdapter(shared_ptr<DbCommand> _cmd)
{
	shared_ptr<DbDataAdapter> adapter( new CEDataAdapter(dynamic_pointer_cast<CECommand>(_cmd)) );
	return adapter;
}

