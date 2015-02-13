


#include "stdafx.h"
#include "CETransaction.h"

CETransaction::CETransaction(shared_ptr<CEConnection> conn, long _IsolationLevel)
{
	open = false;
	SetIsolationLevel(_IsolationLevel);
	txn = NULL;
	connection = conn;
}
CETransaction::~CETransaction()
{
	Rollback();
}

void CETransaction::Connection(shared_ptr<CEConnection> _conn)
{
	connection = _conn;
}
void CETransaction::SetIsolationLevel(long _iso)
{
	if( _iso == IsolationLevel::READ_COMMITTED )
		_isolation = ISOLATIONLEVEL_READCOMMITTED;
	else if( _iso == IsolationLevel::READ_UNCOMMITTED )
		_isolation = ISOLATIONLEVEL_READUNCOMMITTED;
	else if( _iso == IsolationLevel::REPEATABLE_READ )
		_isolation = ISOLATIONLEVEL_REPEATABLEREAD;
	else if( _iso == IsolationLevel::SERIALIZABLE )
		_isolation = ISOLATIONLEVEL_SERIALIZABLE;
	else if( _iso == IsolationLevel::NONE )
		_isolation = ISOLATIONLEVEL_UNSPECIFIED;

}
long CETransaction::GetIsolationLevel()
{
	return _isolation;
}
bool CETransaction::Start()
{
	//HRESULT hr = connection->GetSession()->CreateSession(NULL, IID_ITransactionLocal, (IUnknown**) &txn);
	//if( !FAILED(hr) )
	//{
	//	hr = txn->StartTransaction(_isolation, 0, NULL, NULL);
	//	if(!FAILED(hr))
	//		return true;
	//	else
	//		txn->Release();
	//}
	return false;
}
void CETransaction::Commit()
{
	//if( !open )
	//	return;

	//txn->Commit(FALSE, XACTTC_SYNC, 0);
	//txn->Release();
	//open = false;
}
void CETransaction::Rollback()
{
	//if( !open )
	//	return;

	//HRESULT hr = txn->Abort( NULL, FALSE, FALSE);
	//open = false;
	//txn->Release();
}
bool CETransaction::IsStarted()
{
	return open;
}


DbTransaction::DbTransaction()
{
}
DbTransaction::~DbTransaction()
{
}