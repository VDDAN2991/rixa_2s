#pragma once
#include <string>
class Enemy
{

public: 
	Enemy(const std::string name, const std::string description);

	// getters 
	std::string getName(); 
	std::string getDescription(); 

	//setters 
	void setName(const std::string name); 
	void setDescription(const std::string description); 
private:
	std::string name_; 
	std::string description_; 
};

