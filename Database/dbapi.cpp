#include "dbapi.h"
#include <iostream>
#include <iomanip>
#include <fstream>

FieldObject::~FieldObject()
{
}

bool FieldObject::isType(FieldType type) const
{
	if (this->getType() == type)
	{
		return true;
	}
	return false;
}

Table::~Table()
{

}

void Table::insert(Object** row)
{
	Object*** tmpArray = new Object * *[_rowCount + 1];
	// extend the array by 1 new row
	for (size_t i = 0; i < _rowCount + 1; i++)
	{
		tmpArray[i] = new Object * [_fieldCount];
	}

	for (size_t i = 0; i < _rowCount; i++)
	{
		for (size_t j = 0; j < _fieldCount; j++)
		{
			tmpArray[i][j] = _dataValues[i][j];
		}
	}
	// check if the new row contains correct values for table
	for (size_t i = 0; i < _fieldCount; i++)
	{
		if (StringObject* v = dynamic_cast<StringObject*>(row[i]))
		{
			if (v->isType(_fields[i]->getType()))
			{
				continue;
			}
		}
		if (IntObject* v = dynamic_cast<IntObject*>(row[i]))
		{
			if (v->isType(_fields[i]->getType()))
			{
				continue;
			}
		}
		if (DoubleObject* v = dynamic_cast<DoubleObject*>(row[i]))
		{
			if (v->isType(_fields[i]->getType()))
			{
				continue;
			}
		}
		throw std::invalid_argument("The fields of this table don't match with the data you want to enter.");
	}

	for (size_t i = 0; i < _rowCount; i++)
	{
		delete[] _dataValues[i];
	}
	delete[] _dataValues;

	for (size_t i = 0; i < _fieldCount; i++)
	{
		tmpArray[_rowCount][i] = row[i];
	}
	_rowCount++;
	_dataValues = tmpArray;
}

void Table::remove(int rowid)
{
	for (size_t i = 0; i < _rowCount; i++)
	{
		if (_dataValues[i][0]->getInt() == rowid)
		{
			for (size_t j = 0; j < _fieldCount; j++)
			{
				delete _dataValues[i][j];
			}
			delete[] _dataValues[i];
			for (size_t j = i; j < _rowCount - 1; j++)
			{
				_dataValues[j] = _dataValues[j + 1];
			}
			break;
		}
	}
	_rowCount--;
	Object*** newTable = new Object * *[_rowCount];
	for (size_t i = 0; i < _rowCount; i++)
	{
		newTable[i] = _dataValues[i];
	}
	delete[] _dataValues;
	_dataValues = newTable;
}

Iterator* Table::select()
{
	_iterator = new TableIterator(this);
	return _iterator;
}

void Table::commit()
{
	std::remove((_nameOfTable + "_scheme.txt").c_str());
	std::remove((_nameOfTable + "values.txt").c_str());
	std::ofstream fieldsFile;
	fieldsFile.open(_nameOfTable + "_scheme.txt");
	if (fieldsFile.is_open())
	{
			for (size_t i = 0; i < _fieldCount; i++)
			{
				fieldsFile << _fields[i]->getName() << "<" << _fields[i]->getType() << ">";
				fieldsFile << '\t';
			}
		fieldsFile.close();
		std::ofstream valuesFile;
		valuesFile.open(_nameOfTable + "_values.txt");

		auto it = this->select();
		while (it->moveNext())
		{
			for (size_t i = 0; i < _fieldCount; i++)
			{
				if (it->getRow()[i]->isType(FieldType::Integer))
				{
					valuesFile << it->getRow()[i]->getInt() << "<FieldType::Integer>";
				}
				if (it->getRow()[i]->isType(FieldType::Double))
				{
					valuesFile << it->getRow()[i]->getDouble() << "<FieldType::Double>";
				}
				if (it->getRow()[i]->isType(FieldType::String))
				{
					valuesFile << it->getRow()[i]->getString() << "<FieldType::String>";
				}
				if (i == _fieldCount - 1)
				{
					valuesFile << ";" << std::endl;
				}
				else
				{
					valuesFile << std::setw(10);
				}
			}
		}
		it->close();
		valuesFile.close();
	}
	else
	{
		throw std::exception("The file could not be opened");
	}
}

void Table::close()
{
	for (size_t i = 0; i < _rowCount; i++)
	{
		for (size_t j = 0; j < _fieldCount; j++)
		{
			delete _dataValues[i][j];
		}
		delete[] _dataValues[i];
	}
	for (size_t i = 0; i < _fieldCount; i++)
	{
		delete _fields[i];
	}
	delete[] _dataValues;
	delete[] _fields;
}

FieldObject** Table::getFields() const
{
	return _fields;
}

Db* Db::open(std::string name)
{
	Db* dbInstance = new Db();
	dbInstance->_nameOfDatabase = name;
	return dbInstance;
}
void Db::close()
{
	for (int i = 0; i < _tables.size(); i++)
	{
		delete _tables.at(i);
	}
	delete this;
}

Table* Db::createTable(std::string name, int fieldsCount, FieldObject** fields)
{
	Table* newTable = new Table();
	newTable->setName(name);
	newTable->setFields(fields);
	newTable->setFieldCount(fieldsCount);
	_tables.push_back(newTable);
	return newTable;
}
FieldType getFieldType(std::string line)
{
	if (line == "FieldType::String")
	{
		return FieldType::String;
	}
	if (line == "FieldType::Integer")
	{
		return FieldType::Integer;
	}
	else
	{
		return FieldType::Double;
	}
}
std::pair <std::string, FieldType> getValuesFromString(std::string line)
{
	std::string value;
	std::string fieldTypeString;

	value = line.substr(0, line.find("<"));
	size_t startString = line.find("<");
	size_t endString = line.find(">", startString);
	fieldTypeString = line.substr(startString + 1, endString - startString - 1);

	std::pair <std::string, FieldType> pair;
	pair.first = value;
	pair.second = getFieldType(fieldTypeString);

	return pair;
}

Object* getObjectFromPair(std::pair <std::string, FieldType> pair)
{
	switch (pair.second)
	{
	case FieldType::Double:
		return Db::Double(std::stod(pair.first));
	case FieldType::Integer:
		return Db::Int(std::stoi(pair.first));
	case FieldType::String:
		return Db::String(pair.first);
	}
	return nullptr;
}

Table* Db::openTable(std::string name)
{

	Table* openedTable = new Table();
	std::ifstream fieldsFile;
	fieldsFile.open(name + "_scheme.txt");
	int fieldCount = 0;
	if (fieldsFile.is_open())
	{
		std::string line;
		while (fieldsFile >> line)
		{
			fieldCount++;
		}
		FieldObject** fields = new FieldObject * [fieldCount];
		int index = 0;
		fieldsFile.clear();
		fieldsFile.seekg(0, std::ios::beg);
		while (fieldsFile >> line)
		{
			auto fieldPair = getValuesFromString(line);
			FieldObject* field = Db::Field(fieldPair.first, fieldPair.second);
			fields[index] = field;
			index++;
		}
		fieldsFile.close();
		openedTable->setFields(fields);
		openedTable->setFieldCount(fieldCount);
	}
	else
	{
		throw std::exception("The file with table schema could not be opened.");
	}

	std::ifstream valuesFile;
	valuesFile.open(name + "_values.txt");
	if (valuesFile.is_open())
	{
		std::string line;
		int rowCount = 0;
		while (valuesFile >> line)
		{
			if (line.at(line.length() - 1) == ';')
			{
				rowCount++;
			}
		}
		valuesFile.clear();
		valuesFile.seekg(0);
		Object*** values = new Object * *[rowCount];
		Object** row = nullptr;
		int indexRow = 0;
		int indexColumn = 0;
		while (valuesFile >> line)
		{
			if (indexColumn == 0)
			{
				row = new Object * [fieldCount];
			}
			auto pair = getValuesFromString(line);
			Object* readObject = getObjectFromPair(pair);
			row[indexColumn] = readObject;
			indexColumn++;
			if (line.at(line.length() - 1) == ';')
			{
				values[indexRow] = row;
				indexRow++;
				indexColumn = 0;
			}
		}
		openedTable->setRowCount(rowCount);
		openedTable->setDataValues(values);
	}
	else
	{
		throw std::exception("The file with table values could not be opened.");
	}
	openedTable->setName(name);
	valuesFile.close();
	_tables.push_back(openedTable);
	return openedTable;
}



Table* Db::openOrCreateTable(std::string name, int fieldsCount, FieldObject** fields)
{
	Table* table;
	std::ifstream wantedTable(name + "_scheme.txt");
	if (wantedTable.is_open())
	{
		wantedTable.close();
		table = openTable(name);
		for (size_t i = 0; i < fieldsCount; i++)
		{
			delete fields[i];
		}
		delete[] fields;
	}
	else
	{
		table = createTable(name, fieldsCount, fields);
	}
	return table;
}

Object* Db::Int(int value)
{
	return new IntObject(value);
}

Object* Db::Double(double value)
{
	return new DoubleObject(value);
}

Object* Db::String(std::string value)
{
	return new StringObject(value);
}

FieldObject* Db::Field(std::string name, FieldType type)
{
	return new FieldObject(name, type);
}
IntObject::~IntObject()
{
}
bool IntObject::isType(FieldType type) const
{
	if (type == FieldType::Integer)
	{
		return true;
	}
	return false;
}

DoubleObject::~DoubleObject()
{
}

bool DoubleObject::isType(FieldType type) const
{
	if (type == FieldType::Double)
	{
		return true;
	}
	return false;
}

StringObject::~StringObject()
{
}

bool StringObject::isType(FieldType type) const
{
	if (type == FieldType::String)
	{
		return true;
	}
	return false;
}

Table::TableIterator::TableIterator(Table* t) : _currentTable(_currentTable)
{
	_currentTable = t;
	_currentIndex = 0;
}


bool Table::TableIterator::moveNext()
{
	if (_currentIndex < _currentTable->getRowCount())
	{
		_currentIndex++;
		return true;
	}
	return false;
}

Object** Table::TableIterator::getRow()
{
	return _currentTable->getDataValues()[_currentIndex - 1];
}

int Table::TableIterator::getRowId()
{
	return _currentTable->getDataValues()[_currentIndex - 1][0]->getInt();
}

void Table::TableIterator::close()
{
	delete _currentTable->_iterator;
}

std::ostream& operator<<(std::ostream& os, const FieldType& dt)
{
	switch (dt)
	{
	case FieldType::Double:
		os << "FieldType::Double";
		break;
	case FieldType::Integer:
		os << "FieldType::Integer";
		break;
	case FieldType::String:
		os << "FieldType::String";
		break;
	}
	return os;
}
