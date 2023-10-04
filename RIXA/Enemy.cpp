#include "Enemy.h"


Enemy::Enemy(const std::string name, std::string description) {
	name_ = name; 
	description_ = description; 
}


// getters 
inline std::string Enemy::getName() {
	return name_; 
}

inline std::string Enemy::getDescription() {
	return name_;
}

//setters 

inline void Enemy::setName(const std::string name) {
	name_ = name;
}


inline void Enemy::setDescription(const std::string description) {
	description_ = description; 
}


