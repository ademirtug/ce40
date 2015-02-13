

#ifndef COMPACTDOG_H
#define COMPACTDOG_H

#include "DataProvider.h"


class CompactDog : public DataProvider
{
protected:
	bool initialised;
public:

	//Database Object Generators
	shared_ptr<DbConnection> GetConnection(const wstring& _cnn_str = L"" );

	shared_ptr<DbCommand> GetCommand(wstring sql); 
	shared_ptr<DbCommand> GetCommand(wstring sql, shared_ptr<DbConnection> _cnn);
	shared_ptr<DbCommand> GetCommand(wstring sql, shared_ptr<DbConnection> _cnn, shared_ptr<DbTransaction> _tr);

	shared_ptr<DbTransaction> GetTransaction(shared_ptr<DbConnection> _cnn, int isolation = 1);

	shared_ptr<DbDataAdapter> GetAdapter(shared_ptr<DbCommand> _cmd);

	CompactDog();
	virtual ~CompactDog();
};



#endif//COMPACTDOG_H