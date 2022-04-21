//
// Created by yu on 12/7/2021.
//

#pragma once
#include "common.h"

enum class DatabaseFieldTypes : uint8		//enum class 强类型枚举，不允许和int转换
{
	Null,
	Int8,
	Int16,
	Int32,
	Int64,
	Float,
	Double,
	Decimal,
	Date,
	Binary
};

struct QueryResultFieldMetadata
{
	char const* TableName = nullptr;
	char const* TableAlias = nullptr;
	char const* Name = nullptr;
	char const* Alias = nullptr;
	char const* TypeName = nullptr;
	uint32 Index = 0;
	DatabaseFieldTypes Type = DatabaseFieldTypes::Null;
};

class Field
{
	friend class ResultSet;

public:
	Field();
	~Field() = default;

	bool GetBool() const { return GetUInt8() == 1 ? true : false; }
	uint8 GetUInt8() const;
	int8 GetInt8() const;
	uint16 GetUInt16() const;
	int16 GetInt16() const;
	uint32 GetUInt32() const;
	int32 GetInt32() const;
	uint64 GetUInt64() const;
	int64 GetInt64() const;
	float GetFloat() const;
	double GetDouble() const;
	char const* GetCString() const;
	std::string GetString() const;
	std::string_view GetStringView() const;
	std::vector<uint8> GetBinary() const;

	template <size_t S>
	std::array<uint8, S> GetBinary() const
	{
		std::array<uint8, S> buf;
		GetBinarySizeChecked(buf.data(), S);
		return buf;
	}

	bool IsNull() const
	{
		return data.value == nullptr;
	}

	DatabaseFieldTypes GetType() { return meta->Type; }

protected:
	struct
	{
		char const* value;          // Actual data in memory
		uint32 length;              // Length
		bool raw;                   // Raw bytes? (Prepared statement or ad hoc)
	} data{};

	void SetByteValue(char const* newValue, uint32 length);
	void SetStructuredValue(char const* newValue, uint32 length);

	bool IsType(DatabaseFieldTypes type) const;

	bool IsNumeric() const;

private:
	QueryResultFieldMetadata const* meta;
	void LogWrongType(char const* getter) const;
	void SetMetadata(QueryResultFieldMetadata const* fieldMeta);

	void GetBinarySizeChecked(uint8* buf, size_t size) const;
};
