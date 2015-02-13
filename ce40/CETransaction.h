

#ifndef CETRANSACTION_H
#define CETRANSACTION_H

#include "CEConnection.h"
#include "DataProvider.h"

class CETransaction : public DbTransaction
{
protected:
	ISOLATIONLEVEL _isolation;


	shared_ptr<CEConnection> connection;
public:
	ITransactionLocal	*txn;
	bool open;

	void Commit();
	void Rollback();
	bool IsStarted();
	bool Start();

	void Connection(shared_ptr<CEConnection> _conn);

	void SetIsolationLevel(long _iso);
	long GetIsolationLevel();

	CETransaction(shared_ptr<CEConnection> conn, long _IsolationLevel);
	virtual ~CETransaction();
};

#endif//CETRANSACTION_H