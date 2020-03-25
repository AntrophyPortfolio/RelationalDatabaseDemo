#include "Person.h"
#include <time.h>

Person::Person()
{
}

Person::Person(std::string name, double currentHealth, double damage, int personId)
{
	_name = name;
	Db* db = Db::open("database");

	// first table - people
	auto idFieldPeople = db->Field("id", FieldType::Integer);
	auto nameField = db->Field("Name", FieldType::String);
	auto peopleFields = combineToDefinition(idFieldPeople, nameField);

	Table* people = db->openOrCreateTable("people", 2, peopleFields);

	auto idPeople = db->Int(personId);
	auto namePeople = db->String(name);
	auto personRow = combineToRow(idPeople, namePeople);
	people->insert(personRow);

	// second table - statistics of people
	auto idFieldStats = db->Field("id", FieldType::Integer);
	auto currHpField = db->Field("CurrentHealth", FieldType::Double);
	auto damageField = db->Field("Damage", FieldType::Double);
	auto statsFields = combineToDefinition(idFieldStats, currHpField, damageField);

	Table* statistics = db->openOrCreateTable("statistics", 3, statsFields);

	auto idStats = db->Int(personId);
	auto currHpStats = db->Double(currentHealth);
	auto damageStats = db->Double(damage);
	auto statsRow = combineToRow(idStats, currHpStats, damageStats);
	statistics->insert(statsRow);

	statistics->commit();
	people->commit();
	people->close();
	statistics->close();
	db->close();
	delete statsRow;
	delete personRow;
}

double Person::damageTaken(double value)
{
	double currHealth = select("CurrentHealth", _name);
	update("CurrentHealth", _name, currHealth - value);
	if (currHealth - value <= 0.0)
	{
		deletePerson(_name);
	}
	return value;
}

double Person::damageDealt(double value)
{
	srand(time(NULL));
	double random = rand() % 20 + 1;
	double damage = value + (rand() % 20 + 1) - 10;
	return damage;
}

bool Person::isDead() const
{
	try
	{
		select("CurrentHealth", _name);
	}
	catch (const std::exception&)
	{
		return true;
	}
	return false;
}