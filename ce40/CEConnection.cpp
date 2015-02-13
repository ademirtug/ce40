


#include "stdafx.h"
#include "CETransaction.h"
#include "CEConnection.h"


#define _WIN32_DCOM

CEConnection::CEConnection()
{
	_state = ConnectionState::Closed;
}
CEConnection::CEConnection(wstring cnn_str)
{
	_state = ConnectionState::Closed;
	ConnectionString(cnn_str);
}

CEConnection::~CEConnection()
{
	Close();
}

void CEConnection::ConnectionString(wstring cnn_str)
{
	if( _state != ConnectionState::Closed )
		throw exception("You cant change connection string before you close the connection properly");
	
	_cs = cnn_str;
	parse();
}
wstring CEConnection::ConnectionString()
{
	return _cs;
}
int CEConnection::State()
{
	return _state;
}
void CEConnection::Open()
{
	if( _state == ConnectionState::Open )
		throw exception("connection is already open");

	if( _database.size() < 1 )
		throw exception("You must supply a database name!");

	HRESULT	hr = NULL;
	CDBPropSet propset[2];//[2] = {DBPROPSET_DBINIT, DBPROPSET_SSCE_DBINIT};

	propset[0].SetGUID( DBPROPSET_DBINIT );
	propset[0].AddProperty( DBPROP_INIT_DATASOURCE, _database.c_str() );

	propset[1].SetGUID( DBPROPSET_SSCE_DBINIT );
	propset[1].AddProperty( DBPROP_SSCE_ENCRYPTIONMODE, DBPROPVAL_SSCE_EM_PLATFORM_DEFAULT );
	propset[1].AddProperty( DBPROP_SSCE_DBPASSWORD, _password.c_str() );

	hr = source.Open(CLSID_SQLSERVERCE, propset, 2);
	
	if( FAILED(hr) )
		throw CEException( L"CeConnection::Open Failed.", true );

	_state = ConnectionState::Open;
	hr = session.Open(source);

	return;
}
void CEConnection::Close()
{
	if( _state == ConnectionState::Closed )
		return;

	session.Close();
	source.Close();
	
	_state = ConnectionState::Closed;
}

CSession CEConnection::GetSession()
{
	return session;
}
void CEConnection::parse()
{
	_database.clear();
	_password.clear();

	//Find parts
	size_t last_pos = 0;
	for( size_t t = 0; t < _cs.size(); t++ )
	{
		if( _cs[t] == L';' )
		{
			wstring part = _cs.substr( last_pos, t - last_pos );

			for( size_t n = 0; n < part.size(); ++n)
			{
				if( part[n] == L'=' )
				{
					wstring key, value;
					
					key = part.substr( 0, n );
					value = part.substr( n+1, part.size() - (n + 1));
					
					for(size_t k = 0; k < key.size(); ++k)
						key[k] = towlower(key[k]);

					if( wcscmp(key.c_str(), L"database") == 0)
						_database = value;
					else if( wcscmp(key.c_str(), L"password") == 0 )
						_password = value;
				}
			}
			last_pos = t;
		}
		if( t + 1 >= _cs.size() && t > last_pos )
		{
			wstring part = _cs.substr( last_pos+1, t - last_pos+1 );

			for( size_t n = 0; n < part.size(); ++n)
			{
				if( part[n] == L'=' )
				{
					wstring key, value;
					
					key = part.substr( 0, n );
					value = part.substr( n+1, part.size() - (n + 1));
					
					for(size_t k = 0; k < key.size(); ++k)
						key[k] = towlower(key[k]);

					if( wcscmp(key.c_str(), L"database") == 0)
						_database = value;
					else if( wcscmp(key.c_str(), L"password") == 0 )
						_password = value;
				}
			}
		}

	}
}



shared_ptr<DbTransaction> CEConnection::BeginTransaction(int _IsolationLevel)
{
	if( _state != ConnectionState::Open )
		throw exception("CEConnection::BeginTransaction requires a valid and active connection!");

	shared_ptr<CETransaction> tr( new CETransaction(this->shared_from_this(), _IsolationLevel));
	return dynamic_pointer_cast<DbTransaction>(tr);
}


DbConnection::DbConnection()
{
}
DbConnection::DbConnection(std::wstring cnn_str)
{
}
DbConnection::~DbConnection()
{
}