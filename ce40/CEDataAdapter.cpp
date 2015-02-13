

#include "stdafx.h"
#include "CEDataAdapter.h"
#include "CColumnsRowset.h"
#include "CEDataReader.h"

	

CEDataAdapter::CEDataAdapter(shared_ptr<DbCommand> _command)
{
	command = dynamic_pointer_cast<CECommand>(_command);
}

CEDataAdapter::~CEDataAdapter()
{

}

int CEDataAdapter::Fill(DataTable& table)
{
	FillSchema(table);
	if( table.Columns.Count() < 1 )
		return 0;
	
	shared_ptr<CEDataReader> _rdr = dynamic_pointer_cast<CEDataReader>(command->ExecuteReader());
	if( _rdr == NULL )
		return 0;

	CEDataReader& rdr = *_rdr.get();

	if( !rdr.Read() )
		return 0;

	int aff = 0;
	do
	{
		Row row = table.NewRow();

		for(unsigned int i = 1; i <= rdr.FieldCount(); ++i)
			row[i-1] = rdr[i];

		table.Rows.Add(row);
		++aff;
	}while(rdr.Read());

	table.AcceptChanges();
	
	return 0;
}
void CEDataAdapter::FillSchema(DataTable& table)
{
	HRESULT hr;
	CColumnsRowset<CDynamicAccessor, CRowset> schema;
	schema.SetBlobHandling( DBBLOBHANDLING_SKIP );
	
	command->Open();

	hr = schema.Open(command->command.m_spRowset);

	if( FAILED(hr) )
		throw CEException(L"FillSchema() - CColumnsRowset<CDynamicAccessor, CRowset>::Open Baþarýsýz, ",true);	

	hr = schema.MoveFirst();

	if( FAILED(hr)  )
		throw CEException(L"FillSchema() - CColumnsRowset<CDynamicAccessor, CRowset>::MoveFirst Baþarýsýz, ",true);

	table.TableName += (WCHAR*)schema.GetValue( L"DBCOLUMN_BASETABLENAME" );
	map<int, Column> cols;
	do
	{	
		if( wcscmp(table.TableName.c_str(), (wchar_t*)schema.GetValue( L"DBCOLUMN_BASETABLENAME" )) != 0 )
			table.TableName.clear();

		Column c;
		c.DataType = GetFieldType(*(unsigned short*)schema.GetValue( L"DBCOLUMN_TYPE" ) );
		c.ColumnName += (WCHAR*)schema.GetValue( L"DBCOLUMN_NAME" );
		c.SqlName += (WCHAR*)schema.GetValue( L"DBCOLUMN_BASECOLUMNNAME" );
		c.MaxLength = *(unsigned int*)schema.GetValue( L"DBCOLUMN_COLUMNSIZE" );

		cols[*(unsigned int*)schema.GetValue( L"DBCOLUMN_NUMBER" )] = c;

	}while( schema.MoveNext() == S_OK );

	schema.Close();
	
	for( map<int, Column>::size_type t = 0; t < cols.size(); ++t)
		table.Columns.Add( cols[t+1] );
	
	DBID					idTable;
	ULONG					nColumns, nConstraints;
	DBCOLUMNDESC*			pColDesc;
	OLECHAR*				pStrings;
	DBCONSTRAINTDESC*		pConstraints;
	CComPtr<ITableCreation> spTableCreation;

	hr = command->connection->session.m_spOpenRowset->QueryInterface( IID_ITableCreation, (void**)&spTableCreation );
	
	if( FAILED(hr)  )
		throw CEException(L"FillSchema() - QueryInterface(IID_ITableCreation) Failed, ",true);	

	idTable.eKind          = DBKIND_NAME;
	idTable.uName.pwszName = (LPOLESTR)table.TableName.c_str();

	hr = spTableCreation->GetTableDefinition(&idTable,&nColumns,&pColDesc, NULL, NULL,
		&nConstraints, &pConstraints, &pStrings);

	if( FAILED(hr)  )
		throw CEException(L"FillSchema() - GetTableDefinition Failed, ",true);

	HandleColumns(table, pColDesc, nColumns);
	HandleConstraints(table, pConstraints, nConstraints);
	
	//ole db specification fails at some point....
	DBCOLUMNDESC* pColCur;
	unsigned long i;
	for(pColCur = pColDesc, i = 0; i < nColumns; ++i, ++pColCur)
	{
		ULONG      iSet;
		DBPROPSET* pPropSet = pColCur->rgPropertySets;

		for(iSet = 0; iSet < pColCur->cPropertySets; ++iSet, ++pPropSet)
		{
			ULONG iProp;

			for(iProp = 0; iProp < pPropSet->cProperties; ++iProp)
			{
				VariantClear(&pPropSet->rgProperties[iProp].vValue);
				FreeDBID(pPropSet->rgProperties[iProp].colid);
			}
			CoTaskMemFree(pPropSet->rgProperties);
		}

		if(pColCur->rgPropertySets != NULL)
			CoTaskMemFree(pColCur->rgPropertySets);

		if(pColCur->pTypeInfo != NULL)
			CoTaskMemFree(pColCur->pTypeInfo);

		FreeDBID(pColCur->dbcid);

		if(pColCur->pclsid != NULL)
			CoTaskMemFree(pColCur->pclsid);
	}

	CoTaskMemFree(pColDesc);
	CoTaskMemFree(pStrings);

	spTableCreation.Release();

}
void CEDataAdapter::FreeDBID(DBID &dbid)
{
    switch(dbid.eKind)
    {
    case DBKIND_GUID_NAME:
        if(dbid.uName.pwszName != NULL)
            CoTaskMemFree(dbid.uName.pwszName);
        break;

    case DBKIND_NAME:
        if(dbid.uName.pwszName != NULL)
            CoTaskMemFree(dbid.uName.pwszName);
        break;

    case DBKIND_PGUID_NAME:
        if(dbid.uName.pwszName != NULL)
            CoTaskMemFree(dbid.uName.pwszName);
        if(dbid.uGuid.pguid != NULL)
            CoTaskMemFree(dbid.uGuid.pguid);
        break;

    case DBKIND_PGUID_PROPID:
        if(dbid.uGuid.pguid != NULL)
            CoTaskMemFree(dbid.uGuid.pguid);
        break;
    }
}
void CEDataAdapter::HandleColumns(DataTable& table, DBCOLUMNDESC* pColDesc, unsigned long nColumns)
{
	//col prop
	for(unsigned long i = 0; i < nColumns; ++i)
	{
		DBPROPSET* pPropSet = pColDesc[i].rgPropertySets;
		wstring col_name = pColDesc[i].dbcid.uName.pwszName;

		for(unsigned long cs = 0; cs < pColDesc[i].cPropertySets; ++cs, ++pPropSet )
		{
			if(pPropSet->guidPropertySet == DBPROPSET_COLUMN)
            {
				for(unsigned int ic = 0; ic < table.Columns.Count(); ++ic)
				{
					if( table.Columns[ic].SqlName == col_name )
					{
						unsigned long cp;
						DBPROP* pProp = pPropSet->rgProperties;
						for(cp = 0; cp < pPropSet->cProperties; ++cp, ++pProp)
						{
							switch(pProp->dwPropertyID)
							{
							case DBPROP_COL_AUTOINCREMENT:
								{
									if(pProp->vValue.boolVal == VARIANT_TRUE)
										table.Columns[ic].AutoIncrement = true;
									else
										table.Columns[ic].AutoIncrement = false;
								}
							break;
							case DBPROP_COL_NULLABLE:
								{
									if(pProp->vValue.boolVal == VARIANT_TRUE)
										table.Columns[ic].AllowDBNull = true;
									else
										table.Columns[ic].AllowDBNull = false;
								}
							break;
							//case DBPROP_COL_SEED:
							//col.nSeed = pProp->vValue.intVal;
							//break;
							//case DBPROP_COL_INCREMENT:
							//col.nIncr = pProp->vValue.intVal;
							//break;
							//case DBPROP_COL_ISLONG:
							//if(pProp->vValue.boolVal == VARIANT_TRUE)
							//col.dwFlags |= DBCOLUMNFLAGS_ISLONG;
							//break;
							//case DBPROP_COL_FIXEDLENGTH:
							//if(pProp->vValue.boolVal == VARIANT_TRUE)
							//col.dwFlags |= DBCOLUMNFLAGS_ISFIXEDLENGTH;
							//break;
							}
						}						
					}
				}
			}
		}
		
	}
}
void CEDataAdapter::HandleConstraints(DataTable& table, DBCONSTRAINTDESC* pConstraints, unsigned long nConstraints)
{
	unsigned long iCon = 0;
	for(;iCon < nConstraints; ++iCon)
	{
		DBCONSTRAINTDESC* pCon = pConstraints + iCon;
		switch(pCon->ConstraintType)
		{
		case DBCONSTRAINTTYPE_PRIMARYKEY:
			{
				wstring column_name = pCon->rgColumnList->uName.pwszName;
				for(unsigned int ic = 0; ic < table.Columns.Count(); ++ic)
					if( table.Columns[ic].SqlName == column_name )
						table.PrimaryKey( column_name );
			}
			break;
		case DBCONSTRAINTTYPE_UNIQUE:
			{
				wstring column_name = pCon->rgColumnList->uName.pwszName;
				for(unsigned int ic = 0; ic < table.Columns.Count(); ++ic)
					if( table.Columns[ic].SqlName == column_name )
						table.Columns[ic].Unique = true;
			}
			break;
		}
	}	
}
void CEDataAdapter::HandleIndexes()
{
}
int CEDataAdapter::Update(DataTable& table)
{
	vector<wstring> statements;

	for( unsigned i = 0; i < table.Rows.Count(); ++i)
	{
		if( (table.Rows[i].State() & RS_DELETED) == RS_DELETED )
			statements.push_back(GenerateDeleteCommand(table.Rows[i]));
		else if( (table.Rows[i].State() & RS_ADDED) == RS_ADDED )
			statements.push_back(GenerateInsertCommand(table.Rows[i]));
		else if( (table.Rows[i].State() & RS_MODIFIED ) == RS_MODIFIED )
			statements.push_back( GenerateUpdateCommand(table.Rows[i]));
	}
	//Number of rows successfully updated
	for( size_t i = 0; i < statements.size(); ++i)
	{
		command->CommandText(statements[i]);
		command->ExecuteNonQuery();
	}
	return (int)statements.size();
}
wstring CEDataAdapter::GenerateInsertCommand(Row& row)
{
	wstring sql;
	
	sql += L"INSERT INTO ";
	sql += row.Table()->TableName;
	sql += L"( ";
	Column pri = row.Table()->PrimaryKey();

	for( unsigned int i = 0; i < row.Table()->Columns.Count(); ++i)
	{
		Column c = row.Table()->Columns[i];
		if( pri == c )
			if( row[i].size() < 1 )
				continue;

		sql += row.Table()->Columns[i].SqlName;
		if( i < (row.Table()->Columns.Count()-1) )
			sql += L", ";
	}
	sql += L" ) VALUES( ";
	
	

	for( unsigned int i = 0; i < row.Table()->Columns.Count(); ++i)
	{
		Column c = row.Table()->Columns[i];
		if( pri == c )
		{		
			if( row[i].size() > 0 )
			{
				sql += row[i].c_str();
				sql += L", ";
			}
			continue;

		}
		if( c.DataType == ST_DATE || c.DataType == ST_TIME || c.DataType == ST_CHAR || c.DataType == ST_VARCHAR
			|| c.DataType == ST_NCHAR || c.DataType == ST_NVARCHAR || c.DataType == ST_DATETIME 
			|| c.DataType == ST_UNKNOWN || c.DataType == ST_FLOAT || c.DataType == ST_DOUBLE)
			sql += L"'";
		
		sql += row[i].c_str();
		
		if( c.DataType == ST_DATE || c.DataType == ST_TIME || c.DataType == ST_CHAR || c.DataType == ST_VARCHAR
			|| c.DataType == ST_NCHAR || c.DataType == ST_NVARCHAR || c.DataType == ST_DATETIME 
			|| c.DataType == ST_UNKNOWN || c.DataType == ST_FLOAT || c.DataType == ST_DOUBLE)
			sql += L"' ";
		
		if( i < (row.Table()->Columns.Count()-1) )
			sql += L", ";

	}
	sql += L" )";
	return sql;
}
wstring CEDataAdapter::GenerateUpdateCommand(Row& row)
{
	wstring sql;
	bool suspend_comma = false;
	
	sql += L"UPDATE ";
	sql += row.Table()->TableName;
	sql += L" SET ";
	for( unsigned int i = 0; i < row.Table()->Columns.Count(); ++i)
	{
		Column c = row.Table()->Columns[i];
		
		if( c == c.Table->PrimaryKey() )
		{
			suspend_comma = true;
			continue;
		}
		if( i > 0 && !suspend_comma )
			sql += L", ";
		
		if( suspend_comma )
			suspend_comma = false;

		sql += c.SqlName;
		sql += L"=";
		
		if( c.DataType == ST_DATE || c.DataType == ST_TIME || c.DataType == ST_CHAR || c.DataType == ST_VARCHAR
			|| c.DataType == ST_NCHAR || c.DataType == ST_NVARCHAR || c.DataType == ST_DATETIME 
			|| c.DataType == ST_UNKNOWN || c.DataType == ST_FLOAT || c.DataType == ST_DOUBLE)
			sql += L"'";
		
		sql += row[i].c_str();
		
		if( c.DataType == ST_DATE || c.DataType == ST_TIME || c.DataType == ST_CHAR || c.DataType == ST_VARCHAR
			|| c.DataType == ST_NCHAR || c.DataType == ST_NVARCHAR || c.DataType == ST_DATETIME 
			|| c.DataType == ST_UNKNOWN || c.DataType == ST_FLOAT || c.DataType == ST_DOUBLE)
			sql += L"' ";
		
	}
	sql += L" WHERE ";
	
	wstring un = GetUniqueStatement(row);
	if( un.size() > 0 )
	{
		sql += un;
		return sql;
	}
	throw CEException("Update can not be done without specifing a primary or unique key.");
}
wstring CEDataAdapter::GenerateDeleteCommand(Row& row)
{
	wstring sql;
	sql += L"DELETE FROM ";
	sql += row.Table()->TableName;
	sql += L" WHERE ";
	
	wstring un = GetUniqueStatement(row);

	if( un.size() > 0 )
	{
		sql += un;
		return sql;
	}
	return L"";
}

wstring CEDataAdapter::GetUniqueStatement(Row& row)
{
	wstring sql;

	Column pri = row.Table()->PrimaryKey();
	if( pri.SqlName.size() > 0 )
	{
		//PrimaryKey var
		wstring prival;
		for( unsigned int x = 0; x < row.Table()->Columns.Count(); ++x)
			if( row.Table()->Columns[x].SqlName == pri.SqlName )
				prival = row[x].c_str();

		if( prival.size() > 0 )
		{
			//PrimaryKey Deðeri boþ deðil
			sql += pri.SqlName;
			sql += L"=";

			if( pri.DataType == ST_DATE || pri.DataType == ST_TIME 
				|| pri.DataType == ST_CHAR || pri.DataType == ST_VARCHAR
				|| pri.DataType == ST_NCHAR || pri.DataType == ST_NVARCHAR 
				|| pri.DataType == ST_DATETIME || pri.DataType == ST_UNKNOWN 
				|| pri.DataType == ST_FLOAT || pri.DataType == ST_DOUBLE)
				sql += L"'";

			sql += prival;
			
			if( pri.DataType == ST_DATE || pri.DataType == ST_TIME 
				|| pri.DataType == ST_CHAR || pri.DataType == ST_VARCHAR
				|| pri.DataType == ST_NCHAR || pri.DataType == ST_NVARCHAR 
				|| pri.DataType == ST_DATETIME || pri.DataType == ST_UNKNOWN 
				|| pri.DataType == ST_FLOAT || pri.DataType == ST_DOUBLE)
				sql += L"'";
			return sql;
		}
	}	

	//no Primary Key
	//look for Unique one

	vector<wstring> uniques;

	for( unsigned int i = 0; i < row.Table()->Columns.Count(); ++i)
	{
		if( row.Table()->Columns[i].Unique )
			uniques.push_back(row.Table()->Columns[i].SqlName);
	}

	if( uniques.size() < 1 )
		return L"";
	
	//Unique Key is exist
	wstring unival;
	for( unsigned int i = 0; i < uniques.size(); ++i)
	{
		Column uni;

		for( unsigned int x = 0; x < row.Table()->Columns.Count(); ++x)
			if( row.Table()->Columns[x].SqlName == uniques[i] )
			{
				unival = row[x].c_str();
				uni = row.Table()->Columns[x];
			}

		if( unival.size() )
		{
			sql += uniques[i];
			sql += L"=";

			if( uni.DataType == ST_DATE || uni.DataType == ST_TIME || 
				uni.DataType == ST_CHAR || uni.DataType == ST_VARCHAR
				|| uni.DataType == ST_DATETIME || uni.DataType == ST_UNKNOWN 
				|| uni.DataType == ST_FLOAT || uni.DataType == ST_DOUBLE)
				sql += L"'";

			sql += unival;
			
			if( uni.DataType == ST_DATE || uni.DataType == ST_TIME || 
				uni.DataType == ST_CHAR || uni.DataType == ST_VARCHAR
				|| uni.DataType == ST_DATETIME || uni.DataType == ST_UNKNOWN 
				|| uni.DataType == ST_FLOAT || uni.DataType == ST_DOUBLE)
				sql += L"'";
			return sql;
		}
	}
	return sql;
}
unsigned int CEDataAdapter::GetFieldType(DBTYPE type)
{
	unsigned int datatype = 0;
	
	switch(type)
	{
	case DBTYPE_WSTR:
		{
			return ST_NCHAR;
		}
		break;
	case DBTYPE_STR:
		{
			return ST_CHAR;
		}
		break;
	case DBTYPE_BOOL:
		{
			return ST_BOOL;
		}
		break;
	case DBTYPE_CY:
		{
			return ST_CURRENCY;
		}
		break;
	case DBTYPE_DATE:
		{
			return ST_DATE;
		}
		break;
	case DBTYPE_DBTIMESTAMP:
		{
			return ST_DATETIME;
		}
		break;	
	case DBTYPE_R4:
		{
			return ST_FLOAT;
		}
		break;
	case DBTYPE_R8:
		{
			return ST_DOUBLE;
		}
		break;
	case DBTYPE_I2:
		{
			return ST_SHORT;
		}
		break;
	case DBTYPE_I4:
		{
			return ST_INT;
		}
		break;
	case DBTYPE_I8:
		{
			return ST_INT64;
		}
		break;
	default:
		break;
	}
	return datatype;
}


DbDataAdapter::DbDataAdapter(){}
DbDataAdapter::~DbDataAdapter(){}