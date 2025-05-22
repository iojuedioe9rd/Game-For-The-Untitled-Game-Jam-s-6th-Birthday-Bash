#pragma once

#include "Engine/Core/Base.h"
#include <vector>



class Map
{
public:

	Map(const char* mapData);

	struct MapChunk {
		int X, Y;
		int Width, Height;
		std::vector<uint8_t> Data;
	};

	static Engine::Ref<Map> Create(const std::string& path);
	~Map();

private:
	static std::vector<MapChunk> GetChunks(const char* mapData, int& width, int& height);
	static std::vector<uint8_t> ReadChunks(const std::vector<MapChunk>& chunks, int& outWidth, int& outHeight, int& offsetX, int& offsetY);

	void HandleData(int x, int y, int width, int height, uint8_t data);
	void HandleWall(int x, int y, int width, int height);
	void HandlePlayer(int x, int y, int width, int height);
	void HandleBadGuy(int x, int y, int width, int height);
};