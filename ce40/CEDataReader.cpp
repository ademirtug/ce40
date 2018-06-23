

#include "stdafx.h"
#include "CEDataReader.h"
#include "CECommand.h"

CEDataReader::CEDataReader()
{
	fieldCount = 0;
	fetch = false;
}

CEDataReader::~CEDataReader()
{
	Close();
}


unsigned int CEDataReader::FieldCount()
{
	return fieldCount;
}
bool CEDataReader::Read()
{
	HRESULT hr = S_OK;

	if( !fetch )
	{
		hr = command->command.MoveFirst();
		fetch = true;

		if( FAILED(hr) || hr == DB_S_ENDOFROWSET )
			return false;

		fieldCount = command->command.GetColumnCount();
		return true;
	}
	
	hr = command->command.MoveNext();
	
	if( FAILED(hr) || hr == DB_S_ENDOFROWSET)
		return false;

	return true;
}
void CEDataReader::Close()
{
	
}

bool CEDataReader::GetBoolean(unsigned int index)
{
	BOOL b;
	command->command.GetValue( ++index, &b );

	if( b == 0 )
		return false;
	return false;
}
double CEDataReader::GetDouble(unsigned int index)
{
	double d = 0;
	command->command.GetValue( ++index, &d );
	return d;
}
float CEDataReader::GetFloat(unsigned int index)
{
	return *(float*)command->command.GetValue( ++index );
}
short CEDataReader::GetInt16(unsigned int index)
{
	return *(short*)command->command.GetValue( ++index );
}
int CEDataReader::GetInt32(unsigned int index)
{
	return *(int*)command->command.GetValue( ++index );
}
long CEDataReader::GetInt64(unsigned int index)
{
	return *(long*)command->command.GetValue( ++index );
}
std::tm CEDataReader::GetDate(unsigned int index)
{
	DBTIMESTAMP* pts = (DBTIMESTAMP*)command->command.GetValue( ++index );
	tm ct;
	ct.tm_year = pts->year;
	ct.tm_mon = pts->month;
	ct.tm_mday = pts->day;
	ct.tm_hour = pts->hour;
	ct.tm_min = pts->minute;
	ct.tm_sec = pts->second;

	return ct;
}
wstring CEDataReader::GetString(unsigned int index)
{
	DBTYPE type = 0;
	wstring data;
	//++index;

	if( !command->command.GetColumnType( index, &type ) )
		return L"";
	
	switch(type)
	{
	case DBTYPE_WSTR:
		{
			data += (WCHAR*)command->command.GetValue( index );
		}
		break;
	case DBTYPE_STR:
		{
			USES_CONVERSION;
			data += A2W( (CHAR*)command->command.GetValue( index ) );
		}
		break;
	case DBTYPE_BOOL:
		{
			if( (BOOL*)command->command.GetValue( index ) )
				data = L"1";
			else
				data = L"0";
		}
		break;
	case DBTYPE_CY:
		{
			USES_CONVERSION;
			char* out = 0;
			CURRENCY cy = *(CURRENCY*)command->command.GetValue( index );
			VarI1FromCy(cy, out);
			
			data += A2W( out );
			delete[] out;
		}
		break;
	case DBTYPE_DATE:
		{
			DBTIMESTAMP* pts = (DBTIMESTAMP*)command->command.GetValue( index );
			tm ct;
			ct.tm_year = pts->year;
			ct.tm_mon = pts->month;
			ct.tm_mday = pts->day;
			ct.tm_hour = pts->hour;
			ct.tm_min = pts->minute;
			ct.tm_sec = pts->second;

			wchar_t buffer[50];
			wcsftime(buffer, 50, L"%Y.%m.%d %H:%M:%S", &ct);

			data += wstring(buffer, 50);

		}
		break;
	case DBTYPE_DBTIMESTAMP:
		{
			DBTIMESTAMP* pts = (DBTIMESTAMP*)command->command.GetValue( index );
			tm ct;
			ct.tm_year = pts->year;
			ct.tm_mon = pts->month;
			ct.tm_mday = pts->day;
			ct.tm_hour = pts->hour;
			ct.tm_min = pts->minute;
			ct.tm_sec = pts->second;

			wchar_t buffer[50];
			wcsftime(buffer, 50, L"%Y.%m.%d %H:%M:%S", &ct);

			data += wstring(buffer, 50);
		}
		break;
	case DBTYPE_R4:
		{
			wchar_t buff[100];
			memset(&buff, 0, 100);
			float f = *(FLOAT*)command->command.GetValue( index );
			swprintf( buff, 100, L"%f", f);
			data += buff;
		}
		break;
	case DBTYPE_R8:
		{
			wchar_t buff[100];
			memset(&buff, 0, 100);
			double d = *(DOUBLE*)command->command.GetValue( index );
			swprintf( buff, 100, L"%f", d);
			data += buff;
		}
		break;
	case DBTYPE_I2:
		{
			wchar_t buff[100];
			memset( &buff, 0, 100);
			short s = *(SHORT*)command->command.GetValue( index );
			swprintf(buff, 100, L"%i", s);
			data += buff;
		}
		break;
	case DBTYPE_I4:
		{
			wchar_t buff[100];
			memset( &buff, 0, 100);
			int i = *(INT*)command->command.GetValue( index );
			swprintf(buff, 100, L"%i", i);
			data += buff;
		}
		break;
	case DBTYPE_I8:
		{
			wchar_t buff[100];
			memset( &buff, 0, 100);
			__int64 ll = *(LONGLONG*)command->command.GetValue( index );
			swprintf(buff, 100, L"%lli", ll);
			data += buff;
		}
		break;
	case DBTYPE_GUID:
		{
			GUID g = *(GUID*)command->command.GetValue( index );
			unsigned long ul = g.Data1;
			unsigned short us = g.Data2;
			unsigned short us2 = g.Data3;
			unsigned char uc[9];
			uc[0] = g.Data4[0];
			uc[1] = g.Data4[1];
			uc[2] = g.Data4[2];
			uc[3] = g.Data4[3];
			uc[4] = g.Data4[4];
			uc[5] = g.Data4[5];
			uc[6] = g.Data4[6];
			uc[7] = g.Data4[7];
			uc[8] = 0;
		}
		break;

	default:
		break;
	}

	return data;
}

short CEDataReader::GetInt16(const wstring& name)
{
	return *(short*)command->command.GetValue( GetOrdinal(name) );
}
int	CEDataReader::GetInt32(const wstring& name)
{
	return *(int*)command->command.GetValue( GetOrdinal(name) );
}
long CEDataReader::GetInt64(const wstring& name)
{
	return *(long*)command->command.GetValue( GetOrdinal(name) );
}
bool CEDataReader::GetBoolean(const wstring& name)
{
	if( *(BOOL*)command->command.GetValue( GetOrdinal(name) ) )
		return true;
	return false;
}
float CEDataReader::GetFloat(const wstring& name)
{
	return *(float*)command->command.GetValue( GetOrdinal(name) );
}
double CEDataReader::GetDouble(const wstring& name)
{
	return *(double*)command->command.GetValue( GetOrdinal(name) );
}
std::tm CEDataReader::GetDate(const wstring& name)
{
	DBTIMESTAMP* pts = (DBTIMESTAMP*)command->command.GetValue( GetOrdinal(name) );

	tm ct;
	ct.tm_year = pts->year;
	ct.tm_mon = pts->month;
	ct.tm_mday = pts->day;
	ct.tm_hour = pts->hour;
	ct.tm_min = pts->minute;
	ct.tm_sec = pts->second;

	return ct;
}
wstring CEDataReader::GetString(const wstring& name)
{
	return GetString( GetOrdinal( name ) );
}

//Some Metadata
wstring CEDataReader::GetName(unsigned int index)
{
	return command->command.GetColumnName( index );
}
unsigned int CEDataReader::GetFieldType(unsigned int index)
{
	DBTYPE type;
	command->command.GetColumnType( index, &type );

	return GetFieldType(type);
}
unsigned int CEDataReader::GetFieldType(DBTYPE type)
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

unsigned int CEDataReader::GetOrdinal(const wstring& name)
{
	DBORDINAL ord = 0;
	command->command.GetOrdinal( name.c_str(), &ord );
	return ord;
}
bool CEDataReader::IsDBNull(unsigned int index)
{
	return false;
}

DataTable CEDataReader::GetSchemaTable()
{
	DataTable table;
	HRESULT hr;
	CColumnsRowset<CDynamicAccessor, CRowset> schema;
	schema.SetBlobHandling( DBBLOBHANDLING_SKIP );
	
	command->Open();

	hr = schema.Open(command->command.m_spRowset);

	if( FAILED(hr) )
		throw CEException(L"GetSchemaTable() - CColumnsRowset<CDynamicAccessor, CRowset>::Open Failed, ",true);	

	hr = schema.MoveFirst();

	if( FAILED(hr)  )
		throw CEException(L"GetSchemaTable() - CColumnsRowset<CDynamicAccessor, CRowset>::MoveFirst Failed, ",true);

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
		throw CEException(L"GetSchemaTable() - QueryInterface(IID_ITableCreation) Failed, ",true);	

	idTable.eKind          = DBKIND_NAME;
	idTable.uName.pwszName = (LPOLESTR)table.TableName.c_str();

	hr = spTableCreation->GetTableDefinition(&idTable,&nColumns,&pColDesc, NULL, NULL,
		&nConstraints, &pConstraints, &pStrings);

	if( FAILED(hr)  )
		throw CEException(L"GetSchemaTable() - GetTableDefinition Failed, ",true);

	HandleColumns(table, pColDesc, nColumns);
	HandleConstraints(table, pConstraints, nConstraints);
	

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


	return table;
}
void CEDataReader::FreeDBID(DBID &dbid)
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
void CEDataReader::HandleColumns(DataTable& table, DBCOLUMNDESC* pColDesc, unsigned long nColumns)
{
	//Kolon Özellikleri
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
							case DBPROP_COL_DEFAULT:
								{
									int efg = 5;
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
void CEDataReader::HandleConstraints(DataTable& table, DBCONSTRAINTDESC* pConstraints, unsigned long nConstraints)
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
void CEDataReader::HandleIndexes()
{
}
wstring CEDataReader::operator[](unsigned int index)
{
	return GetString(index);
}
wstring CEDataReader::operator[](wstring name)
{
	return GetString( GetOrdinal( name ) );
}

DbDataReader::DbDataReader()
{
}
DbDataReader::~DbDataReader()
{
}