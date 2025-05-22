#pragma once


struct Player
{
	float speed = 5.0f;
	float jump = 500.0f;
	float jumpTimer = 0.0f;
};

struct BadGuy
{
	float speed = 5.0f;
};

template<typename T>
struct HealthSystem
{
	typedef std::function<void(T newHealth, T health, T maxHealth)> CallbackFunc;

	T health;
	T maxHealth;

	CallbackFunc onHealthChange;
	CallbackFunc onDeath;

	CallbackFunc onDamage;
	CallbackFunc onHeal;

	
	HealthSystem(T health)
		: health(health), maxHealth(health)
	{
	}

	HealthSystem(T health, T maxHealth)
		: health(health), maxHealth(maxHealth) {
	}

	void TakeDamage(T damage) {
		
		if (onDamage)
		{
			onDamage(health - damage, health, maxHealth);
		}
		health -= damage;
		if (health < 0) {
			onDeath(health, health, maxHealth);
			health = 0;
		}
	}

	void Heal(T amount) {
		onHeal(health + amount, health, maxHealth);
		health += amount;
		
		if (health > maxHealth) {
			onHealthChange(health, health, maxHealth);
			health = maxHealth;
		}
	}

	T GetHealth() const {
		return health;
	}

	T GetMaxHealth() const {
		return maxHealth;
	}

	T GetHealthPercentage() const {
		return (health / maxHealth);
	}

};