//
// Created by yu on 12/7/2021.
//

#include "QueryResult.h"

static uint32 SizeForType(MYSQL_FIELD* field)
{
	switch (field->type)
	{
	case MYSQL_TYPE_NULL:
		return 0;
	case MYSQL_TYPE_TINY:
		return 1;
	case MYSQL_TYPE_YEAR:
	case MYSQL_TYPE_SHORT:
		return 2;
	case MYSQL_TYPE_INT24:
	case MYSQL_TYPE_LONG:
	case MYSQL_TYPE_FLOAT:
		return 4;
	case MYSQL_TYPE_DOUBLE:
	case MYSQL_TYPE_LONGLONG:
	case MYSQL_TYPE_BIT:
		return 8;

	case MYSQL_TYPE_TIMESTAMP:
	case MYSQL_TYPE_DATE:
	case MYSQL_TYPE_TIME:
	case MYSQL_TYPE_DATETIME:
		return sizeof(MYSQL_TIME);

	case MYSQL_TYPE_TINY_BLOB:
	case MYSQL_TYPE_MEDIUM_BLOB:
	case MYSQL_TYPE_LONG_BLOB:
	case MYSQL_TYPE_BLOB:
	case MYSQL_TYPE_STRING:
	case MYSQL_TYPE_VAR_STRING:
		return field->max_length + 1;

	case MYSQL_TYPE_DECIMAL:
	case MYSQL_TYPE_NEWDECIMAL:
		return 64;

	case MYSQL_TYPE_GEOMETRY:
		/*
		Following types are not sent over the wire:
		MYSQL_TYPE_ENUM:
		MYSQL_TYPE_SET:
		*/
	default:
		LOG_WARN("SQL::SizeForType(): invalid field type %u", uint32(field->type));
		return 0;
	}
}

DatabaseFieldTypes MysqlTypeToFieldType(enum_field_types type)
{
	switch (type)
	{
	case MYSQL_TYPE_NULL:
		return DatabaseFieldTypes::Null;
	case MYSQL_TYPE_TINY:
		return DatabaseFieldTypes::Int8;
	case MYSQL_TYPE_YEAR:
	case MYSQL_TYPE_SHORT:
		return DatabaseFieldTypes::Int16;
	case MYSQL_TYPE_INT24:
	case MYSQL_TYPE_LONG:
		return DatabaseFieldTypes::Int32;
	case MYSQL_TYPE_LONGLONG:
	case MYSQL_TYPE_BIT:
		return DatabaseFieldTypes::Int64;
	case MYSQL_TYPE_FLOAT:
		return DatabaseFieldTypes::Float;
	case MYSQL_TYPE_DOUBLE:
		return DatabaseFieldTypes::Double;
	case MYSQL_TYPE_DECIMAL:
	case MYSQL_TYPE_NEWDECIMAL:
		return DatabaseFieldTypes::Decimal;
	case MYSQL_TYPE_TIMESTAMP:
	case MYSQL_TYPE_DATE:
	case MYSQL_TYPE_TIME:
	case MYSQL_TYPE_DATETIME:
		return DatabaseFieldTypes::Date;
	case MYSQL_TYPE_TINY_BLOB:
	case MYSQL_TYPE_MEDIUM_BLOB:
	case MYSQL_TYPE_LONG_BLOB:
	case MYSQL_TYPE_BLOB:
	case MYSQL_TYPE_STRING:
	case MYSQL_TYPE_VAR_STRING:
		return DatabaseFieldTypes::Binary;
	default:
		LOG_WARN("MysqlTypeToFieldType(): invalid field type %u", uint32(type));
		break;
	}

	return DatabaseFieldTypes::Null;
}

static char const* FieldTypeToString(enum_field_types type)
{
	switch (type)
	{
	case MYSQL_TYPE_BIT:         return "BIT";
	case MYSQL_TYPE_BLOB:        return "BLOB";
	case MYSQL_TYPE_DATE:        return "DATE";
	case MYSQL_TYPE_DATETIME:    return "DATETIME";
	case MYSQL_TYPE_NEWDECIMAL:  return "NEWDECIMAL";
	case MYSQL_TYPE_DECIMAL:     return "DECIMAL";
	case MYSQL_TYPE_DOUBLE:      return "DOUBLE";
	case MYSQL_TYPE_ENUM:        return "ENUM";
	case MYSQL_TYPE_FLOAT:       return "FLOAT";
	case MYSQL_TYPE_GEOMETRY:    return "GEOMETRY";
	case MYSQL_TYPE_INT24:       return "INT24";
	case MYSQL_TYPE_LONG:        return "LONG";
	case MYSQL_TYPE_LONGLONG:    return "LONGLONG";
	case MYSQL_TYPE_LONG_BLOB:   return "LONG_BLOB";
	case MYSQL_TYPE_MEDIUM_BLOB: return "MEDIUM_BLOB";
	case MYSQL_TYPE_NEWDATE:     return "NEWDATE";
	case MYSQL_TYPE_NULL:        return "NULL";
	case MYSQL_TYPE_SET:         return "SET";
	case MYSQL_TYPE_SHORT:       return "SHORT";
	case MYSQL_TYPE_STRING:      return "STRING";
	case MYSQL_TYPE_TIME:        return "TIME";
	case MYSQL_TYPE_TIMESTAMP:   return "TIMESTAMP";
	case MYSQL_TYPE_TINY:        return "TINY";
	case MYSQL_TYPE_TINY_BLOB:   return "TINY_BLOB";
	case MYSQL_TYPE_VAR_STRING:  return "VAR_STRING";
	case MYSQL_TYPE_YEAR:        return "YEAR";
	default:                     return "-Unknown-";
	}
}
/*保存列的元数据*/
void InitializeDatabaseFieldMetadata(QueryResultFieldMetadata* meta, const MYSQL_FIELD* field, uint32 fieldIndex)
{
	meta->TableName = field->org_table;
	meta->TableAlias = field->table;
	meta->Name = field->org_name;
	meta->Alias = field->name;
	meta->TypeName = FieldTypeToString(field->type);
	meta->Index = fieldIndex;
	meta->Type = MysqlTypeToFieldType(field->type);
}

ResultSet::ResultSet(MYSQL_RES* result, MYSQL_FIELD* fields, uint64 rowCount, uint32 fieldCount) :
_rowCount(rowCount), _fieldCount(fieldCount), _result(result), _fields(fields)
{
	_fieldMetadata.resize(_fieldCount);
	_currentRow = new Field[_fieldCount];
	for (uint32 i = 0; i < _fieldCount; i++)
	{
		InitializeDatabaseFieldMetadata(&_fieldMetadata[i], &_fields[i], i);
		_currentRow[i].SetMetadata(&_fieldMetadata[i]);
	}
}

ResultSet::~ResultSet()
{
	CleanUp();
};

bool ResultSet::NextRow()
{
	MYSQL_ROW row;

	if (!_result)
		return false;

	row = mysql_fetch_row(_result);
	if (!row)
	{
		CleanUp();
		return false;
	}

	unsigned long* lengths = mysql_fetch_lengths(_result);
	if (!lengths)
	{
		LOG_WARN("%s:mysql_fetch_lengths, cannot retrieve value lengths. Error %s.", __FUNCTION__, mysql_error(_result->handle));
		CleanUp();
		return false;
	}

	for (uint32 i = 0; i < _fieldCount; i++)
	{
		_currentRow[i].SetStructuredValue(row[i], lengths[i]);
	}
	return true;
}

char* ResultSet::GetFieldName(uint32 index) const
{
	assert(index < _fieldCount);
	return _fields[index].name;
}

void ResultSet::CleanUp()
{
	if (_currentRow)
	{
		delete[] _currentRow;
		_currentRow = nullptr;
	}

	if (_result)
	{
		mysql_free_result(_result);
		_result = nullptr;
	}
}

const Field& ResultSet::operator[](std::size_t index) const
{
	assert(index < _fieldCount);
	return _currentRow[index];
}
