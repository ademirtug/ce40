


#ifndef CEDATAADAPTER_H
#define CEDATAADAPTER_H

#include "DataProvider.h"
#include "CECommand.h"

class CEDataAdapter : public DbDataAdapter
{
protected:
	shared_ptr<CECommand> command;


	wstring GenerateInsertCommand(Row& row);
	wstring GenerateUpdateCommand(Row& row);
	wstring GenerateDeleteCommand(Row& row);
	wstring GetUniqueStatement(Row& row);

public:
	void FreeDBID(DBID &dbid);
	void HandleColumns(DataTable& table, DBCOLUMNDESC* pColDesc, unsigned long nColumns);
	void HandleConstraints(DataTable& table, DBCONSTRAINTDESC* pConstraints, unsigned long nConstraints);
	void HandleIndexes();

	int Fill(DataTable& table);
	void FillSchema(DataTable& table);

	int Update(DataTable& table);
	unsigned int GetFieldType(DBTYPE type);

	CEDataAdapter(shared_ptr<DbCommand> _command); 
	virtual ~CEDataAdapter();
};


#endif//CEDATAADAPTER_H