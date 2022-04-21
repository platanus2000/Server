//
// Created by yu on 12/7/2021.
//

#pragma once
#include <mysql/mysql.h>
#include "common.h"
#include "Field.h"

class ResultSet : public nocopyable
{
public:
	ResultSet(MYSQL_RES* result, MYSQL_FIELD* fields, uint64 rowCount, uint32 fieldCount);
	~ResultSet();

	bool NextRow();
	uint64 GetRowCount() const { return _rowCount; }
	uint32 GetFieldCount() const { return _fieldCount; }
	char* GetFieldName(uint32 index) const;

	Field* Fetch() const { return _currentRow; }
	const Field& operator[](std::size_t index) const;

protected:
	std::vector<QueryResultFieldMetadata> _fieldMetadata;
	uint64 _rowCount;
	Field* _currentRow;
	uint32 _fieldCount;

private:
	void CleanUp();
	MYSQL_RES* _result{};
	MYSQL_FIELD* _fields{};
};
