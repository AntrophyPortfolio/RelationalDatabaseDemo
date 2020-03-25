#include "Person.h"
#include<iostream>
#include<time.h>
#include<iomanip>
#include <chrono>
#include <thread>
#include <vector>
#define NUMBER_OF_RANDOM_NAMES 12

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

std::string getCurrentEnemies(Person* enemies, int size)
{
	std::string currentEnemies = "";
	for (size_t i = 0; i < size; i++)
	{
		currentEnemies += enemies[i].getName() + " " + "(" + std::to_string(enemies[i].getCurrentHealth()) + " HP)" + " Damage: " + std::to_string(enemies[i].getBaseDamage()) + "\n";
	}
	return currentEnemies;
};

Person& findEnemy(std::string name, Person* enemies, int size)
{
	for (size_t i = 0; i < size; i++)
	{
		if (enemies[i].getName() == name)
		{
			return enemies[i];
		}
	}
	throw std::exception("The enemy name doesn't exist.");
};

void playGame() {
	std::string nameArray[NUMBER_OF_RANDOM_NAMES]{ "Eva", "Milan", "Peter", "Karel", "Tereza", "Jakub", "Franta", "Jan", "Dominik", "Lukas", "Emil", "Ivan" };
	std::string command = "del /Q ";
	std::string path = "*.txt";
	system(command.append(path).c_str());
	int id = 0;
	srand(time(NULL));
	std::string username;
	int difficulty;
	std::cout << "Welcome to the CPP FIGHTER!" << std::endl;
	std::cout << "Please insert your username: ";
	std::cin >> username;

	Person* player = new Person(username, 1000, 40, id++);
	do
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		std::cout << username << ", how many opponents do you want to fight against? (2 - easy, 3 - medium, 4 - hard): ";
		std::cin >> difficulty;
	} while (difficulty < 2 && difficulty > 4);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	Person* enemies = new Person[difficulty];
	for (size_t i = 0; i < difficulty; i++)
	{
		Person enemy = Person(nameArray[i], 100, 40, id++);
		enemies[i] = enemy;

	}
	int attackChoice;
	bool win = false;
	std::string targetName;
	do
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "-----------------------------------------" << std::endl;
		std::cout << "Your current HP is: " << player->getCurrentHealth() << std::endl << std::endl;
		std::cout << "-----------------------------------------" << std::endl;
		std::cout << "The stats of your current enemies are: " << std::endl << std::endl << getCurrentEnemies(enemies, difficulty);
		std::cout << "-----------------------------------------" << std::endl;
		std::cout << "Attack a single enemy or all enemies at once (reduced damage)? (1 - single target, 2 - all targets)" << std::endl;
		std::cin >> attackChoice;

		if (attackChoice == 1)
		{
			std::cout << "Who do you want to attack? (insert name): ";
			std::cin >> targetName;
			Person target = findEnemy(targetName, enemies, difficulty);
			std::cout << target.getName() << " was damaged for " << target.damageTaken(player->damageDealt(player->getBaseDamage())) << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		if (attackChoice == 2)
		{
			for (size_t i = 0; i < difficulty; i++)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				std::cout << enemies[i].getName() << " was damaged for " << enemies[i].damageTaken(player->damageDealt(player->getBaseDamage() / difficulty)) << std::endl;
			}
		}
		for (size_t i = 0; i < difficulty; i++)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			if (!enemies[i].isDead())
			{
				std::cout << enemies[i].getName() << " hit you for " << player->damageTaken(enemies[i].damageDealt(enemies[i].getBaseDamage())) << std::endl;

			}
			else
			{
				std::cout << enemies[i].getName() << " died." << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));

				for (int j = i; j < difficulty - 1; ++j)
				{
					enemies[j] = enemies[j + 1];
				}
				difficulty--;
				Person* tmp = new Person[difficulty];
				for (size_t j = 0; j < difficulty; j++)
				{
					tmp[j] = enemies[j];
				}
				delete[] enemies;
				enemies = tmp;
				if (difficulty == 0)
				{
					win = true;
					break;
				}
				i--;
			}

		}
		if (player->isDead())
		{
			break;
		}
	} while (win != true);
	if (win == false)
	{
		std::cout << "You lost!";
	}
	else
	{
		std::cout << "You won the game!";
	}
	delete player;
	delete[] enemies;
};
int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	playGame();
	_CrtDumpMemoryLeaks();
	return 0;
}