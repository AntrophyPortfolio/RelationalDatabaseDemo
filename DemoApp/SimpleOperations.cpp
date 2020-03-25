#include "SimpleOperations.h"

int findPersonId(std::string whereName)
{
	Db* db = Db::open("database");
	Table* people = db->openTable("people");
	auto iteratorPeople = people->select();
	int idIndex;
	for (size_t i = 0; i < people->getFieldCount(); i++)
	{
		if (people->getFields()[i]->getName() == "Name")
		{
			idIndex = i;
		}
	}
	while (iteratorPeople->moveNext())
	{
		Object** row = iteratorPeople->getRow();
		std::string name = row[idIndex]->getString();
		if (name == whereName)
		{
			int rowId = iteratorPeople->getRowId();
			people->close();
			iteratorPeople->close();
			db->close();
			return rowId;
		}
	}
	people->close();
	iteratorPeople->close();
	db->close();
	throw std::exception("No user was found");
}
double select(std::string columnName, std::string nameOfPerson)
{
	int personId = findPersonId(nameOfPerson);
	Db* db = Db::open("database");
	Table* people = db->openTable("people");
	Table* stats = db->openTable("statistics");

	auto itStats = stats->select();

	while (itStats->moveNext())
	{
		if (personId == itStats->getRowId())
		{
			for (size_t i = 0; i < stats->getFieldCount(); i++)
			{
				if (stats->getFields()[i]->getName() == columnName)
				{
					double result = itStats->getRow()[i]->getDouble();
					people->close();
					stats->close();
					itStats->close();
					db->close();
					return result;
				}
			}
		}
	}
	people->close();
	stats->close();
	itStats->close();
	db->close();
	throw std::exception("The user doesn't exist.");

}

void update(std::string columnName, std::string nameOfPerson, double value)
{
	int personId = findPersonId(nameOfPerson);
	Db* db = Db::open("database");
	Table* stats = db->openTable("statistics");
	auto itStats = stats->select();

	while (itStats->moveNext())
	{
		if (itStats->getRowId() == personId)
		{
			for (size_t i = 0; i < stats->getFieldCount(); i++)
			{
				if (stats->getFields()[i]->getName() == columnName)
				{
					Object* obj = itStats->getRow()[i];
					delete obj;
					itStats->getRow()[i] = db->Double(value);
					stats->commit();
					delete itStats;
					stats->close();
					db->close();
					return;
				}
			}
		}
	}
	delete itStats;
	stats->close();
	db->close();
	throw std::exception("The person doesn't exist.");
}

std::string deletePerson(std::string name)
{
	int personId = findPersonId(name);
	Db* db = Db::open("database");
	Table* people = db->openTable("people");
	Table* stats = db->openTable("statistics");
	people->remove(personId);
	stats->remove(personId);
	people->commit();
	stats->commit();

	people->close();
	stats->close();
	db->close();

	return name;
}

