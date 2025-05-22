
#include "Map.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <algorithm>
#include "Engine/Core/Application.h"
#include <Engine/Core/Components.h>
#include "Components.h"
#include "feline_load.h"

#define DATA_WALL 1
#define DATA_PLAYER 2
#define DATA_BAD 3

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
    case DATA_WALL:   HandleWall(x, y, width, height); break;
    case DATA_PLAYER: HandlePlayer(x, y, width, height); break;
	case DATA_BAD:    HandleBadGuy(x, y, width, height); break;
    default: break;
    }
}

void Map::HandleWall(int x, int y, int width, int height)
{
    std::cout << "Wall at (" << x << ", " << y << ")\n";


	Engine::Application& app = Engine::Application::Get();
	ecs::Manager& manager = app.GetManager();
	ecs::Entity wall = manager.CreateEntity();
	app.NewCommponent<Engine::Components::TransformComponent>(wall, glm::vec3(x, y, 0), glm::vec3(0), glm::vec3(1, 1, 1));
	app.NewCommponent<Engine::Components::Renderer>(wall, glm::vec4(1, 1, 1, 1));
	app.NewCommponent<Engine::Components::Rigidbody2DComponent>(wall, Engine::Components::Rigidbody2DComponent::BodyType::Static);
	Engine::Components::BoxCollider2DComponent::BoxCollider2DSettings settings = {};
	settings.Density = 1.0f;
	settings.Friction = 0.5f;
	settings.Restitution = 0.0f;
	settings.RestitutionThreshold = 0.5f;
	app.NewCommponent<Engine::Components::BoxCollider2DComponent>(wall, settings);

}

void Map::HandlePlayer(int x, int y, int width, int height)
{
    std::cout << "Player at (" << x << ", " << y << ")\n";

    Engine::Application& app = Engine::Application::Get();
    ecs::Manager& manager = app.GetManager();
    ecs::Entity player = manager.CreateEntity();
    app.NewCommponent<Engine::Components::TransformComponent>(player, glm::vec3(x, y, 0), glm::vec3(0), glm::vec3(1, 1, 1));
    app.NewCommponent<Engine::Components::Renderer>(player, glm::vec4(1, 1, 1, 1));
    app.NewCommponent<Engine::Components::Rigidbody2DComponent>(player, Engine::Components::Rigidbody2DComponent::BodyType::Dynamic);
    Engine::Components::BoxCollider2DComponent::BoxCollider2DSettings settings = {};
    settings.Density = 1.0f;
    settings.Friction = 0.5f;
    settings.Restitution = 0.0f;
    settings.RestitutionThreshold = 0.5f;
    app.NewCommponent<Engine::Components::BoxCollider2DComponent>(player, settings);
	app.NewCommponent<Player>(player);
}

void Map::HandleBadGuy(int x, int y, int width, int height)
{
	std::cout << "Bad Guy at (" << x << ", " << y << ")\n";
	Engine::Application& app = Engine::Application::Get();
	ecs::Manager& manager = app.GetManager();
	ecs::Entity badGuy = manager.CreateEntity();
	app.NewCommponent<Engine::Components::TransformComponent>(badGuy, glm::vec3(x, y, 0), glm::vec3(0), glm::vec3(1, 1, 1));
	app.NewCommponent<Engine::Components::Renderer>(badGuy, glm::vec4(1, 0, 0, 1));
	app.NewCommponent<Engine::Components::Rigidbody2DComponent>(badGuy, Engine::Components::Rigidbody2DComponent::BodyType::Dynamic);
	Engine::Components::BoxCollider2DComponent::BoxCollider2DSettings settings = {};
	settings.Density = 1.0f;
	settings.Friction = 0.5f;
	settings.Restitution = 0.0f;
	settings.RestitutionThreshold = 0.5f;
	app.NewCommponent<Engine::Components::BoxCollider2DComponent>(badGuy, settings);
	app.NewCommponent<HealthSystem<float>>(badGuy, 100.0f);
	app.NewCommponent<BadGuy>(badGuy);
}

Map::~Map()
{
}
