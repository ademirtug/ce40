

#ifndef CCOLUMNSROWSET_H
#define CCOLUMNSROWSET_H


template <class TAccessor = CNoAccessor, template <typename T> class TRowset = CRowset >
class CColumnsRowset : public CAccessorRowset<TAccessor, TRowset>
{
protected:
    ULONG    m_cOptColumns;    // Number of optional columns
    DBID*    m_pOptColumns;    // Optional columns array
public:
    CColumnsRowset()
    :    m_cOptColumns(0),
        m_pOptColumns(NULL)
    {
    }

    ~CColumnsRowset()
    {
        if(m_pOptColumns)
            delete [] m_pOptColumns;
        Close();
    }
	bool AddOptionalColumn(const DBID *pColumn)
	{
		DBID* pNew = new DBID[m_cOptColumns+1];

		if(pNew)
		{
			if(m_pOptColumns && m_cOptColumns)
				memcpy(pNew, m_pOptColumns, sizeof(DBID) * m_cOptColumns);
			if(m_pOptColumns)
				delete [] m_pOptColumns;
			m_pOptColumns = pNew;
			m_pOptColumns[m_cOptColumns++] = *pColumn;
			return true;
		}
		return false;
	}

	bool AddOptionalColumn(LPOLESTR pszColumnName)
	{
		DBID dbid;

		dbid.eKind          = DBKIND_NAME;
		dbid.uName.pwszName = pszColumnName;

		return AddOptionalColumn(&dbid);
	}
	HRESULT Open(IUnknown* pUnk, DBPROPSET* pPropSet = NULL, ULONG ulPropSets = 0)
	{
		ATLASSERT(pUnk != NULL);

		CComPtr<IColumnsRowset> spColumnsRowset;
		HRESULT                 hr;

		hr = pUnk->QueryInterface(__uuidof(IColumnsRowset),
								  (void**)&spColumnsRowset);
		
		DBID pOptColumns2[5] = {DBCOLUMN_BASETABLENAME, DBCOLUMN_BASECOLUMNNAME, 
			DBCOLUMN_ISUNIQUE, DBCOLUMN_ISAUTOINCREMENT, DBCOLUMN_KEYCOLUMN};
		if(SUCCEEDED(hr))
		{
			hr = spColumnsRowset->GetColumnsRowset(NULL,
												   5,
												   pOptColumns2,
												   GetIID(),
												   0,
												   0,
												   (IUnknown**)GetInterfacePtr());
			if(SUCCEEDED(hr))
			{
				SetupOptionalRowsetInterfaces();

				// If we have output columns then bind
				if(_OutputColumnsClass::HasOutputColumns())
					hr = Bind();
			}
		}
		return hr;
	}
};


#endif//CCOLUMNSROWSET_H