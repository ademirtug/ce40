


#ifndef CECONNECTION_IMPL_H
#define CECONNECTION_IMPL_H

#include <map>
#include <string>
using namespace std;

#include "DataProvider.h"

class CETransaction;

class CEConnection : public DbConnection, public std::tr1::enable_shared_from_this<CEConnection>
{
protected:
	int _state;
	wstring _cs;
	wstring _database;
	wstring _password;

	void parse();
public:
	CDataSource source;
	CSession session;

	void ConnectionString(wstring cnn_str);
	wstring ConnectionString();

	CSession GetSession();

	int State();
	void Open();
	void Close();


	shared_ptr<DbTransaction> BeginTransaction(int _IsolationLevel = IsolationLevel::READ_COMMITTED);

	CEConnection();
	CEConnection(wstring cnn_str);
	virtual ~CEConnection();
};


#endif//CECONNECTION_IMPL_H