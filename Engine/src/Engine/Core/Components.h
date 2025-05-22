#pragma once

#include "Engine/Core/Base.h"
#include <glm/glm.hpp>

namespace Engine::Components
{
	struct ENGINE_API TransformComponent
	{
		glm::vec3 Position;
		glm::vec3 Rotation;
		glm::vec3 Scale;
		TransformComponent(const glm::vec3& position = glm::vec3(0), const glm::vec3& rotation = glm::vec3(0), const glm::vec3& scale = glm::vec3(1))
			: Position(position), Rotation(rotation), Scale(scale) {
		}

		TransformComponent(const TransformComponent& other)
			: Position(other.Position), Rotation(other.Rotation), Scale(other.Scale) {
		}

		TransformComponent& operator=(const TransformComponent& other)
		{
			if (this != &other) {
				Position = other.Position;
				Rotation = other.Rotation;
				Scale = other.Scale;
			}
			return *this;
		}

		TransformComponent(TransformComponent&& other) noexcept
			: Position(std::move(other.Position)), Rotation(std::move(other.Rotation)), Scale(std::move(other.Scale)) {
		}

		TransformComponent& operator=(TransformComponent&& other) noexcept
		{
			if (this != &other) {
				Position = std::move(other.Position);
				Rotation = std::move(other.Rotation);
				Scale = std::move(other.Scale);
			}
			return *this;
		}

		TransformComponent& operator+=(const TransformComponent& other)
		{
			Position += other.Position;
			Rotation += other.Rotation;
			Scale += other.Scale;
			return *this;
		}

		TransformComponent& operator-=(const TransformComponent& other)
		{
			Position -= other.Position;
			Rotation -= other.Rotation;
			Scale -= other.Scale;
			return *this;
		}

		TransformComponent& operator*=(const TransformComponent& other)
		{
			Position *= other.Position;
			Rotation *= other.Rotation;
			Scale *= other.Scale;
			return *this;
		}

		TransformComponent& operator/=(const TransformComponent& other)
		{
			Position /= other.Position;
			Rotation /= other.Rotation;
			Scale /= other.Scale;
			return *this;
		}

		TransformComponent operator+(const TransformComponent& other) const
		{
			return TransformComponent(Position + other.Position, Rotation + other.Rotation, Scale + other.Scale);
		}

		TransformComponent operator-(const TransformComponent& other) const
		{
			return TransformComponent(Position - other.Position, Rotation - other.Rotation, Scale - other.Scale);
		}

		TransformComponent operator*(const TransformComponent& other) const
		{
			return TransformComponent(Position * other.Position, Rotation * other.Rotation, Scale * other.Scale);
		}

		TransformComponent operator/(const TransformComponent& other) const
		{
			return TransformComponent(Position / other.Position, Rotation / other.Rotation, Scale / other.Scale);
		}

		TransformComponent operator+(const glm::vec3& vec) const
		{
			return TransformComponent(Position + vec, Rotation, Scale);
		}

		TransformComponent operator-(const glm::vec3& vec) const
		{
			return TransformComponent(Position - vec, Rotation, Scale);
		}

		TransformComponent operator*(const glm::vec3& vec) const
		{
			return TransformComponent(Position * vec, Rotation, Scale);
		}

		TransformComponent operator/(const glm::vec3& vec) const
		{
			return TransformComponent(Position / vec, Rotation, Scale);
		}

		TransformComponent& operator+=(const glm::vec3& vec)
		{
			Position += vec;
			return *this;
		}

		TransformComponent& operator-=(const glm::vec3& vec)
		{
			Position -= vec;
			return *this;
		}

		TransformComponent& operator*=(const glm::vec3& vec)
		{
			Position *= vec;
			return *this;
		}

		TransformComponent& operator/=(const glm::vec3& vec)
		{
			Position /= vec;
			return *this;
		}

		TransformComponent operator+(float scalar) const
		{
			return TransformComponent(Position + glm::vec3(scalar), Rotation, Scale);
		}

		TransformComponent operator-(float scalar) const
		{
			return TransformComponent(Position - glm::vec3(scalar), Rotation, Scale);
		}

		TransformComponent operator*(float scalar) const
		{
			return TransformComponent(Position * glm::vec3(scalar), Rotation, Scale);
		}
	};
	struct ENGINE_API Renderer
	{
		glm::vec4 colour;
		Renderer(const glm::vec4& color = glm::vec4(1.0f))
			: colour(color) {
		}
	};

	struct ENGINE_API Rigidbody2DComponent
	{
		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
		// Kinematic Dynamic
		enum class BodyType { Static = 0, Kinematic, Dynamic};
		Rigidbody2DComponent(BodyType Type, bool FixedRotation = false)
			: Type(Type), FixedRotation(FixedRotation) {
		}
		
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		// Storage for runtime
		void* RuntimeBody = nullptr;
	};

	
	struct ENGINE_API BoxCollider2DComponent
	{
		struct ENGINE_API BoxCollider2DSettings
		{
			float Density = 1.0f;
			float Friction = 0.5f;
			float Restitution = 0.0f;
			float RestitutionThreshold = 0.5f;
		};

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;

		BoxCollider2DComponent(const BoxCollider2DSettings& settings)
			: Density(settings.Density), Friction(settings.Friction), Restitution(settings.Restitution), RestitutionThreshold(settings.RestitutionThreshold) {
		}

		BoxCollider2DComponent(const glm::vec2& offset, const glm::vec2& size)
			: Offset(offset), Size(size) {
		}
		BoxCollider2DComponent(const glm::vec2& offset, const glm::vec2& size, float density, float friction, float restitution, float restitutionThreshold)
			: Offset(offset), Size(size), Density(density), Friction(friction), Restitution(restitution), RestitutionThreshold(restitutionThreshold) {
		}
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.5f, 0.5f };

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;
	};
}