#pragma once
#include <Engine/Renderer/Texture.h>
#include <Engine/Core/Audio.h>

struct Player
{
	float speed = 5.0f;
	float jump = 10.0f;
	float jumpTimer = 0.0f;
	float tenportTimer = 0.0f;
	Engine::Ref<Engine::Audio> swap = Engine::Ref<Engine::Audio>();
	Engine::Ref<Engine::Audio> jumpSound = Engine::Ref<Engine::Audio>();

};

struct BadGuy
{
	float speed = 5.0f;
	float fireRate = 1.f;
	float fireTimer = 0.0f;

	BadGuy()
		: speed(5.0f), fireRate(1.0f), fireTimer(0.0f) {
	}

	BadGuy(float speed, float fireRate)
		: speed(speed), fireRate(fireRate), fireTimer(0.0f) {
	}
};

struct BadCat
{
	float swapTimer = 0.0f;
	float swapRate = 1.5f;
	Engine::Ref<Engine::Audio> swapSound = Engine::Ref<Engine::Audio>();
	float meowTimer = 0.0f;
	float meowRate = 5.0f;
	Engine::Ref<Engine::Audio> meowSound = Engine::Ref<Engine::Audio>();

	BadCat()
		: swapSound(Engine::Audio::Create("assets/audio/swap.wav")),
		meowSound(Engine::Audio::Create("assets/audio/meow.wav")) {
	}


};


template<typename T>
struct HealthSystem
{
	typedef std::function<void(T newHealth, T health, T maxHealth, void* userData)> CallbackFunc;

	void* userData = nullptr;
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
			onDamage(health - damage, health, maxHealth, userData);
		}
		health -= damage;
		if (health < 0) {
			if (onDeath)
				onDeath(health, health, maxHealth, userData);
			health = 0;
		}
	}

	void Heal(T amount) {
		onHeal(health + amount, health, maxHealth, userData);
		health += amount;
		
		if (health > maxHealth) {
			if (onHealthChange)
				onHealthChange(health, health, maxHealth, userData);
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

struct Bullet
{
	Engine::Ref<Engine::Audio> bulletAudio;
	float speed = 10.0f;
	float damage = 10.0f;
	glm::vec2 direction = glm::vec2(0.0f);;

	Bullet(float speed, float damage, glm::vec2 direction)
		: speed(speed), damage(damage), direction(direction) {
		bulletAudio = Engine::Audio::Create("assets/audio/bullet.wav");
	}
};

struct GameOverTrigger
{
	float radius = 5.0f;
};

struct GameWinTrigger
{
	float radius = 5.0f;
	Engine::Ref<Engine::Audio> winSound = Engine::Audio::Create("assets/audio/win.wav");
	GameWinTrigger()
		: winSound(Engine::Audio::Create("assets/audio/win.wav")) {
	}
};