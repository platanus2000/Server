//
// Created by yu on 12/7/2021.
//

#include "Field.h"

Field::Field()
{
	data.value = nullptr;
	data.length = 0;
	data.raw = false;
	meta = nullptr;
}

uint8 Field::GetUInt8() const
{
	if (!data.value)
		return 0;

	if (data.raw)
		return *reinterpret_cast<const uint8*>(data.value);
	return static_cast<uint8>(strtoul(data.value, nullptr, 10));
}

int8 Field::GetInt8() const
{
	if (!data.value)
		return 0;

	if (data.raw)
		return *reinterpret_cast<int8 const*>(data.value);
	return static_cast<int8>(strtol(data.value, nullptr, 10));
}

uint16 Field::GetUInt16() const
{
	if (!data.value)
		return 0;

	if (data.raw)
		return *reinterpret_cast<uint16 const*>(data.value);
	return static_cast<uint16>(strtoul(data.value, nullptr, 10));
}

int16 Field::GetInt16() const
{
	if (!data.value)
		return 0;

	if (data.raw)
		return *reinterpret_cast<int16 const*>(data.value);
	return static_cast<int16>(strtol(data.value, nullptr, 10));
}

uint32 Field::GetUInt32() const
{
	if (!data.value)
		return 0;

	if (data.raw)
		return *reinterpret_cast<uint32 const*>(data.value);
	return static_cast<uint32>(strtoul(data.value, nullptr, 10));
}

int32 Field::GetInt32() const
{
	if (!data.value)
		return 0;

	if (data.raw)
		return *reinterpret_cast<int32 const*>(data.value);
	return static_cast<int32>(strtol(data.value, nullptr, 10));
}

uint64 Field::GetUInt64() const
{
	if (!data.value)
		return 0;

	if (data.raw)
		return *reinterpret_cast<uint64 const*>(data.value);
	return static_cast<uint64>(strtoull(data.value, nullptr, 10));
}

int64 Field::GetInt64() const
{
	if (!data.value)
		return 0;

	if (data.raw)
		return *reinterpret_cast<int64 const*>(data.value);
	return static_cast<int64>(strtoll(data.value, nullptr, 10));
}

float Field::GetFloat() const
{
	if (!data.value)
		return 0.0f;

	if (data.raw)
		return *reinterpret_cast<float const*>(data.value);
	return static_cast<float>(std::strtod(data.value, nullptr));
}

double Field::GetDouble() const
{
	if (!data.value)
		return 0.0f;

	if (data.raw && !IsType(DatabaseFieldTypes::Decimal))
		return *reinterpret_cast<double const*>(data.value);
	return static_cast<double>(std::strtod(data.value, nullptr));
}

char const* Field::GetCString() const
{
	if (!data.value)
		return nullptr;

	return static_cast<const char*>(data.value);
}

std::string Field::GetString() const
{
	if (!data.value)
		return "";

	char const* string = GetCString();
	if (!string)
		return "";

	return { string, data.length };
}

std::string_view Field::GetStringView() const
{
	if (!data.value)
		return {};

	const char* const string = GetCString();
	if (!string)
		return {};

	return { string, data.length };
}

std::vector<uint8> Field::GetBinary() const
{
	std::vector<uint8> result;
	if (!data.value || !data.length)
		return result;

	result.resize(data.length);
	memcpy(result.data(), data.value, data.length);
	return result;
}

void Field::GetBinarySizeChecked(uint8* buf, size_t length) const
{
	//ASSERT(data.value && (data.length == length), "Expected %zu-byte binary blob, got %sdata (%u bytes) instead", length, data.value ? "" : "no ", data.length);
	assert(data.value && (data.length == length));
	memcpy(buf, data.value, length);
}

void Field::SetByteValue(char const* newValue, uint32 length)
{
	// This value stores raw bytes that have to be explicitly cast later
	data.value = newValue;
	data.length = length;
	data.raw = true;
}

void Field::SetStructuredValue(char const* newValue, uint32 length)
{
	// This value stores somewhat structured data that needs function style casting
	data.value = newValue;
	data.length = length;
	data.raw = false;
}

bool Field::IsType(DatabaseFieldTypes type) const
{
	return meta->Type == type;
}

bool Field::IsNumeric() const
{
	return (meta->Type == DatabaseFieldTypes::Int8 ||
		meta->Type == DatabaseFieldTypes::Int16 ||
		meta->Type == DatabaseFieldTypes::Int32 ||
		meta->Type == DatabaseFieldTypes::Int64 ||
		meta->Type == DatabaseFieldTypes::Float ||
		meta->Type == DatabaseFieldTypes::Double);
}

void Field::LogWrongType(char const* getter) const
{
	LOG_WARN("Warning: %s on %s field %s.%s (%s.%s) at index %u.",
		getter, meta->TypeName, meta->TableAlias, meta->Alias, meta->TableName, meta->Name, meta->Index);
}

void Field::SetMetadata(QueryResultFieldMetadata const* fieldMeta)
{
	meta = fieldMeta;
}