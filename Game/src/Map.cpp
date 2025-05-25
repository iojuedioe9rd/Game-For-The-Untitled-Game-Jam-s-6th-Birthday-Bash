
#include "Map.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <algorithm>
#include "Engine/Core/Application.h"
#include <Engine/Core/Components.h>
#include "Components.h"
#include "feline_load.h"
#include "Engine/Renderer/Texture.h"

#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE0 0x84C0
#define DATA_WALL 1
#define DATA_PLAYER 2
#define DATA_BAD 3
#define DATA_TEXT1 4
#define DATA_BAD_CAT 5
#define DATA_GAME_OVER_TRIGGER 6
#define DATA_DOOR 7

Engine::Ref<Map> Map::Create(const std::string& path)
{
    File file = feline_load_file(path.c_str(), true);
    return Engine::CreateRef<Map>((const char*)file.buffer);
}

std::vector<Map::MapChunk> Map::GetChunks(const char* mapData, int& width, int& height)
{
    std::vector<Map::MapChunk> chunks;
    nlohmann::json jsonData = nlohmann::json::parse(mapData);

    width = jsonData["width"];
    height = jsonData["height"];

    for (const auto& layer : jsonData["layers"])
    {
        if (layer["type"] != "tilelayer") continue;

        for (const auto& chunk : layer["chunks"])
        {
            Map::MapChunk mapChunk;
            mapChunk.X = chunk["x"];
            mapChunk.Y = chunk["y"];
            mapChunk.Width = chunk["width"];
            mapChunk.Height = chunk["height"];
            mapChunk.Data = chunk["data"].get<std::vector<uint8_t>>();

            chunks.push_back(std::move(mapChunk));
        }
    }

    return chunks;
}

std::vector<uint8_t> Map::ReadChunks(const std::vector<MapChunk>& chunks, int& outWidth, int& outHeight, int& offsetX, int& offsetY)
{
    if (chunks.empty()) return {};

    int minX = chunks[0].X;
    int minY = chunks[0].Y;
    int maxX = chunks[0].X + chunks[0].Width;
    int maxY = chunks[0].Y + chunks[0].Height;

    for (const auto& chunk : chunks)
    {
        minX = std::min(minX, chunk.X);
        minY = std::min(minY, chunk.Y);
        maxX = std::max(maxX, chunk.X + chunk.Width);
        maxY = std::max(maxY, chunk.Y + chunk.Height);
    }

    outWidth = maxX - minX;
    outHeight = maxY - minY;
    offsetX = minX;
    offsetY = minY;

    std::vector<uint8_t> result(outWidth * outHeight, 0);

    for (const auto& chunk : chunks)
    {
        for (int row = 0; row < chunk.Height; ++row)
        {
            for (int col = 0; col < chunk.Width; ++col)
            {
                int localIndex = row * chunk.Width + col;
                int globalX = chunk.X + col - minX;
                int globalY = chunk.Y + row - minY;
                int globalIndex = globalY * outWidth + globalX;

                if (globalIndex >= 0 && globalIndex < (int)result.size())
                    result[globalIndex] = chunk.Data[localIndex];
            }
        }
    }

    return result;
}

Map::Map(const char* mapData)
{
    int jsonWidth = 0, jsonHeight = 0;
    auto chunks = GetChunks(mapData, jsonWidth, jsonHeight);

    int dataWidth = 0, dataHeight = 0, offsetX = 0, offsetY = 0;
    auto data = ReadChunks(chunks, dataWidth, dataHeight, offsetX, offsetY);

    for (int y = 0; y < dataHeight; ++y)
    {
        for (int x = 0; x < dataWidth; ++x)
        {
            int index = y * dataWidth + x;
            if (index >= 0 && index < (int)data.size())
            {
                // Flip Y
                int flippedY = (dataHeight - 1 - y);
                HandleData(x + offsetX, flippedY + offsetY, dataWidth, dataHeight, data[index]);
            }
        }
    }
}

void Map::HandleData(int x, int y, int width, int height, uint8_t data)
{
    switch (data)
    {
    case DATA_WALL:    HandleWall(x, y, width, height); break;
    case DATA_PLAYER:  HandlePlayer(x, y, width, height); break;
	case DATA_BAD:     HandleBadGuy(x, y, width, height); break;
	case DATA_TEXT1:   HandleText(x, y, width, height, 1); break;
	case DATA_BAD_CAT: HandleBadCat(x, y, width, height); break;
	case DATA_GAME_OVER_TRIGGER: HandleGameOverTrigger(x, y, width, height); break;
	case DATA_DOOR:    HandleDoor(x, y, width, height); break;
    default: break;
    }
}

void Map::HandleWall(int x, int y, int width, int height)
{


	Engine::Application& app = Engine::Application::Get();
	ecs::Manager& manager = app.GetManager();
	ecs::Entity wall = manager.CreateEntity();
	app.NewCommponent<Engine::Components::TransformComponent>(wall, glm::vec3(x * 2, y * 2, 0), glm::vec3(0), glm::vec3(2, 2, 1));
	app.NewCommponent<Engine::Components::Renderer>(wall, glm::vec4(1, 1, 1, 1), Engine::Texture::Create("assets/textures/wall.png", GL_TEXTURE_2D, GL_TEXTURE0));
	app.NewCommponent<Engine::Components::Rigidbody2DComponent>(wall, Engine::Components::Rigidbody2DComponent::BodyType::Static);
	Engine::Components::BoxCollider2DComponent::BoxCollider2DSettings settings = {};
	settings.Density = 1.0f;
	settings.Friction = 0.5f;
	settings.Restitution = 0.0f;
	settings.RestitutionThreshold = 0.5f;
	app.NewCommponent<Engine::Components::BoxCollider2DComponent>(wall, settings);

}

void Map::HandleText(int x, int y, int width, int height, int id)
{
    Engine::Application& app = Engine::Application::Get();
    ecs::Manager& manager = app.GetManager();
    switch (id)
    {
    case 1:
    {
        Engine::Application& app = Engine::Application::Get();
        ecs::Manager& manager = app.GetManager();
        ecs::Entity wall = manager.CreateEntity();
        float size_x = (1715.0 / 64.0) * 2.0;
		float size_y = (35.0 / 64.0) * 2.0;
        app.NewCommponent<Engine::Components::TransformComponent>(wall, glm::vec3(x * 3, y * 2, 0), glm::vec3(0), glm::vec3(size_x, size_y, 1));
        app.NewCommponent<Engine::Components::Renderer>(wall, glm::vec4(1, 1, 1, 1), Engine::Texture::Create("assets/textures/text1.png", GL_TEXTURE_2D, GL_TEXTURE0));
        break;
    }
    default:
        break;
    }
}

void Map::HandlePlayer(int x, int y, int width, int height)
{
    std::cout << "Player at (" << x << ", " << y << ")\n";

    Engine::Application& app = Engine::Application::Get();
    ecs::Manager& manager = app.GetManager();
    ecs::Entity player = manager.CreateEntity();
    app.NewCommponent<Engine::Components::TransformComponent>(player, glm::vec3(x * 2, y * 2, 0), glm::vec3(0), glm::vec3(2, 3.9f, 1));
    app.NewCommponent<Engine::Components::Renderer>(player, glm::vec4(1, 1, 1, 1), Engine::Texture::Create("assets/textures/player.png", GL_TEXTURE_2D, GL_TEXTURE0));
    app.NewCommponent<Engine::Components::Rigidbody2DComponent>(player, Engine::Components::Rigidbody2DComponent::BodyType::Dynamic, true);
    Engine::Components::BoxCollider2DComponent::BoxCollider2DSettings settings = {};
    settings.Density = 1.0f;
    settings.Friction = 0.5f;
    settings.Restitution = 0.0f;
    settings.RestitutionThreshold = 0.5f;
    app.NewCommponent<Engine::Components::BoxCollider2DComponent>(player, settings);
	Player& player_commp = app.NewCommponent<Player>(player);
    player_commp.jumpSound = Engine::Audio::Create("assets/audio/jump.wav");
	player_commp.swap = Engine::Audio::Create("assets/audio/swap.wav");
}

static void KillBadGuy(float newHealth, float health, float maxHealth, void* userData)
{
    Engine::Components::Rigidbody2DComponent& badGuy = *(Engine::Components::Rigidbody2DComponent*)userData;

	Engine::Application& app = Engine::Application::Get();
	ecs::Manager& manager = app.GetManager();

	
	{
		app.m_RigidBodiesToRemove.emplace_back(badGuy);
		
	}
	
}

void Map::HandleBadGuy(int x, int y, int width, int height)
{
	std::cout << "Bad Guy at (" << x << ", " << y << ")\n";
	Engine::Application& app = Engine::Application::Get();
	ecs::Manager& manager = app.GetManager();
	ecs::Entity badGuy = manager.CreateEntity();
	app.NewCommponent<Engine::Components::TransformComponent>(badGuy, glm::vec3(x * 2, y * 2, 0), glm::vec3(0), glm::vec3(2, 2, 1));
	app.NewCommponent<Engine::Components::Renderer>(badGuy, glm::vec4(1, 1, 1, 1), Engine::Texture::Create("assets/textures/bad.png", GL_TEXTURE_2D, GL_TEXTURE0));
    Engine::Components::Rigidbody2DComponent& rb2d = app.NewCommponent<Engine::Components::Rigidbody2DComponent>(badGuy, Engine::Components::Rigidbody2DComponent::BodyType::Dynamic);
	Engine::Components::BoxCollider2DComponent::BoxCollider2DSettings settings = {};
	settings.Density = 1.0f;
	settings.Friction = 0.5f;
	settings.Restitution = 0.0f;
	settings.RestitutionThreshold = 0.5f;
	app.NewCommponent<Engine::Components::BoxCollider2DComponent>(badGuy, settings);
    HealthSystem<float>& healthSystem = app.NewCommponent<HealthSystem<float>>(badGuy, 100.0f);
    healthSystem.userData = &rb2d;
	healthSystem.onDeath = KillBadGuy;
	app.NewCommponent<BadGuy>(badGuy);
}

void Map::HandleBadCat(int x, int y, int width, int height)
{
    //std::cout << "Bad Guy at (" << x << ", " << y << ")\n";
    Engine::Application& app = Engine::Application::Get();
    ecs::Manager& manager = app.GetManager();
    ecs::Entity badGuy = manager.CreateEntity();
    app.NewCommponent<Engine::Components::TransformComponent>(badGuy, glm::vec3(x * 2, y * 2, 0), glm::vec3(0), glm::vec3(2, 4, 1));
    app.NewCommponent<Engine::Components::Renderer>(badGuy, glm::vec4(1, 1, 1, 1), Engine::Texture::Create("assets/textures/bad_cat.png", GL_TEXTURE_2D, GL_TEXTURE0));
    Engine::Components::Rigidbody2DComponent& rb2d = app.NewCommponent<Engine::Components::Rigidbody2DComponent>(badGuy, Engine::Components::Rigidbody2DComponent::BodyType::Dynamic, true);
    Engine::Components::BoxCollider2DComponent::BoxCollider2DSettings settings = {};
    settings.Density = 1.0f;
    settings.Friction = 0.5f;
    settings.Restitution = 0.0f;
    settings.RestitutionThreshold = 0.5f;
    app.NewCommponent<Engine::Components::BoxCollider2DComponent>(badGuy, settings);
    HealthSystem<float>& healthSystem = app.NewCommponent<HealthSystem<float>>(badGuy, 100.0f);
    healthSystem.userData = &rb2d;
    healthSystem.onDeath = KillBadGuy;
    app.NewCommponent<BadGuy>(badGuy, 9, .5f);

	app.NewCommponent<BadCat>(badGuy);
}

void Map::HandleGameOverTrigger(int x, int y, int width, int height)
{
    Engine::Application& app = Engine::Application::Get();
    ecs::Manager& manager = app.GetManager();
    ecs::Entity GameOver = manager.CreateEntity();

	app.NewCommponent<Engine::Components::TransformComponent>(GameOver, glm::vec3(x * 2, y * 2, 0), glm::vec3(0), glm::vec3(2, 2, 1));
    app.NewCommponent< GameOverTrigger>(GameOver);
}

void Map::HandleDoor(int x, int y, int width, int height)
{
	Engine::Application& app = Engine::Application::Get();
	ecs::Manager& manager = app.GetManager();
	ecs::Entity door = manager.CreateEntity();
	app.NewCommponent<Engine::Components::TransformComponent>(door, glm::vec3(x * 2, y * 2, 0), glm::vec3(0), glm::vec3(2, 4, 1));
	app.NewCommponent<Engine::Components::Renderer>(door, glm::vec4(1, 1, 1, 1), Engine::Texture::Create("assets/textures/door.png", GL_TEXTURE_2D, GL_TEXTURE0));
	app.NewCommponent<GameWinTrigger>(door);
}

Map::~Map()
{
}
