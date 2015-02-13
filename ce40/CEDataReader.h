


#ifndef CEDATAREADER_H
#define CEDATAREADER_H

#include "DataProvider.h"
#include "CColumnsRowset.h"
#include <time.h>

class CECommand;




class CEDataReader : public DbDataReader
{
protected:
	int fieldCount;
	bool fetch;
	bool lastrow;

public:
	shared_ptr<CECommand> command;

	
	//Internal Methods
	void HandleColumns(DataTable& table, DBCOLUMNDESC* pColDesc, unsigned long nColumns);
	void HandleConstraints(DataTable& table, DBCONSTRAINTDESC* pConstraints, unsigned long nConstraints);
	void HandleIndexes();
	void FreeDBID(DBID &dbid);


	unsigned int FieldCount();
	bool Read();
	void Close();

	bool GetBoolean(unsigned int index);
	double GetDouble(unsigned int index);
	float GetFloat(unsigned int index);
	short GetInt16(unsigned int index);
	int GetInt32(unsigned int index);
	long GetInt64(unsigned int index);
	std::tm GetDate(unsigned int index);
	wstring GetString(unsigned int index);

	short GetInt16(const wstring& name);
	int	GetInt32(const wstring& name);
	long GetInt64(const wstring& name);
	bool GetBoolean(const wstring& name);
	float GetFloat(const wstring& name);
	double GetDouble(const wstring& name);
	std::tm GetDate(const wstring& name);
	wstring GetString(const wstring& name);

	//Some Metadata
	wstring GetName(unsigned int index);//Sql Name
	unsigned int GetFieldType(unsigned int index);
	unsigned int GetFieldType(DBTYPE type);
	unsigned int GetOrdinal(const wstring& name);
	bool IsDBNull(unsigned int index);

	DataTable GetSchemaTable();

	wstring operator[](unsigned int index);
	wstring operator[](wstring name);

	CEDataReader();
	virtual ~CEDataReader();


};

#endif//CEDATAREADER_H