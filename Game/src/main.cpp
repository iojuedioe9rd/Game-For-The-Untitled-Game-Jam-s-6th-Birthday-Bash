#include <cstdlib>
#include <cstdio>
#include "../../Engine/src/Engine/main.h"
#include "../../Engine/src/Engine/Core/Application.h"
#include "Engine/Core/Input.h"
#include "box2d/b2_body.h"

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <Engine/Core/Components.h>
#include "Map.h"
#include "Components.h"

#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE0 0x84C0

class Game : public Engine::Application
{
public:
    Game() {}
    ~Game() {}
};

static Game* s_Game = nullptr;
static Engine::Ref<Map> s_map;

namespace Engine
{
    Application* CreateApplication()
    {
        return new Game();
    }
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");

    s_Game = static_cast<Game*>(Engine::CreateApplication());

    s_Game->AddUpdateFunc([&](float dt, ecs::Manager& m) {
        for (auto [e, player, tr, rb2d] : m.EntitiesWith<Player, Engine::Components::TransformComponent, Engine::Components::Rigidbody2DComponent>())
        {
            b2Body* body = static_cast<b2Body*>(rb2d.RuntimeBody.object);
            b2Vec2 velocity = body->GetLinearVelocity();

            player.tenportTimer += dt;

            glm::vec2 direction(0.0f);
            s_Game->Camera.Position.x = tr.Position.x;
            s_Game->Camera.Position.y = tr.Position.y;

            if (Engine::Input::IsPressed('a')) direction.x -= 1.0f;
            if (Engine::Input::IsPressed('d')) direction.x += 1.0f;

            if (Engine::Input::IsPressed(' ') && velocity.y >= -1.0f)
            {
                player.jumpTimer += dt * 3.0f;
            }

            if (!Engine::Input::IsPressed(' ') && player.jumpTimer >= 0.0f)
            {
                body->ApplyLinearImpulseToCenter(b2Vec2(0, player.jumpTimer * player.jump * dt * 3.9f), true);
                player.jumpTimer = -1.0f;
            }

            if (direction == glm::vec2(0.0f)) continue;
            direction = glm::normalize(direction);
            tr.Position += glm::vec3(direction.x, direction.y, 0.0f) * player.speed * dt * 2.0f;
        }
        });

    s_Game->AddUpdateFunc([&](float dt, ecs::Manager& m) {
        glm::vec3 player_pos(0.0f);
		float player_tenportTimer = 0.0f;

        for (auto [e, player, tr] : m.EntitiesWith<Player, Engine::Components::TransformComponent>())
        {
            player_pos = tr.Position;
			player_tenportTimer = player.tenportTimer;
        }

        // Mouse position conversion
        float w = static_cast<float>(Engine::Application::GetWindow().GetWidth());
        float h = static_cast<float>(Engine::Application::GetWindow().GetHeight());

        float pix_x = 0, pix_y = 0;
        auto mouseState = SDL_GetMouseState(&pix_x, &pix_y);

        float x = (pix_x / w) * 2.0f - 1.0f;
        float y = 1.0f - (pix_y / h) * 2.0f; // Flip Y

        glm::mat4 viewProj = s_Game->Camera.Matrix(90, 0.1f, 100.0f); // Ensure P * V
        glm::mat4 invViewProj = glm::inverse(viewProj);

        glm::vec4 mouseNDC(x, y, 0.0f, 1.0f); // Z at 0.0f
        glm::vec4 worldPos = invViewProj * mouseNDC;
        glm::vec3 mouse_pos = glm::vec3(worldPos) / worldPos.w;

        for (auto [e, badGuy, tr, rb2d] : m.EntitiesWith<BadGuy, Engine::Components::TransformComponent, Engine::Components::Rigidbody2DComponent>())
        {
			badGuy.fireTimer += dt;

            float dis = glm::distance(tr.Position, player_pos);
            float dis2 = glm::distance(tr.Position, mouse_pos);
            glm::vec2 direction = glm::normalize(player_pos - tr.Position);

            if (dis < 10.0f)
            {
                
                tr.Position += glm::vec3(direction.x, direction.y, 0.0f) * badGuy.speed * dt;
            }

            if (dis < 20.0f)
            {
                if (badGuy.fireTimer >= badGuy.fireRate)
                {
                    ecs::Entity bullet = m.CreateEntity();
                    bullet.Add<Engine::Components::TransformComponent>(tr.Position, glm::vec3(0), glm::vec3(1));
                    bullet.Add<Engine::Components::Renderer>(glm::vec4(1, 1, 1, 1), Engine::Texture::Create("assets/textures/bullet.png", GL_TEXTURE_2D, GL_TEXTURE0));
                    bullet.Add<Bullet>(5.0f, 10, direction);
                    Engine::Components::Rigidbody2DComponent bul_rb2d = s_Game->NewCommponent<Engine::Components::Rigidbody2DComponent>(bullet, Engine::Components::Rigidbody2DComponent::BodyType::Dynamic);
                    Engine::Components::BoxCollider2DComponent::BoxCollider2DSettings settings = {};
                    settings.Density = 1.0f;
                    settings.Friction = 0.5f;
                    settings.Restitution = 0.0f;
                    settings.RestitutionThreshold = 0.5f;
                    ((b2Body*)bul_rb2d.RuntimeBody.get())->SetBullet(true);
					((b2Body*)bul_rb2d.RuntimeBody.get())->SetGravityScale(0.0f);
					
                    s_Game->NewCommponent<Engine::Components::BoxCollider2DComponent>(bullet, settings);

					badGuy.fireTimer = 0.0f;
                }
            }

            if (player_tenportTimer >= 2.1f && dis2 < 20.0f && (mouseState & SDL_BUTTON_LMASK))
            {
                glm::vec3 old_pos = tr.Position;
                tr.Position = player_pos;
                player_pos = old_pos;
				player_tenportTimer = 0.0f;
            }
        }

        for (auto [e, player, tr] : m.EntitiesWith<Player, Engine::Components::TransformComponent>())
        {
            tr.Position = player_pos;
			player.tenportTimer = player_tenportTimer;
        }
        });
    /*
    s_Game->AddUpdateFunc([&](float dt, ecs::Manager& m) {

        for (auto [e, healthSystem, tr] : m.EntitiesWith<HealthSystem<float>, Engine::Components::TransformComponent>())
        {
            for (auto [e2, bullet, bu_tr, rb2d] : m.EntitiesWith<Bullet, Engine::Components::TransformComponent, Engine::Components::Rigidbody2DComponent>())
            {
				float dis = glm::distance(tr.Position, bu_tr.Position);

                if(dis <= 5.f)
				{
					healthSystem.TakeDamage(bullet.damage);
					
                    s_Game->m_RigidBodiesToRemove.emplace_back(rb2d);

                    e2.Destroy();
				}
            }
        }

    });
    */



	s_Game->AddUpdateFunc([&](float dt, ecs::Manager& m) {
		for (auto [e, bullet, tr, rb2d] : m.EntitiesWith<Bullet, Engine::Components::TransformComponent, Engine::Components::Rigidbody2DComponent>())
		{
			tr.Position += glm::vec3(bullet.direction * bullet.speed * dt, 0);


            tr.Scale.x -= dt / 10;
			tr.Scale.y -= dt / 10;

			if (tr.Scale.x <= 0.0f || tr.Scale.y <= 0.0f)
			{
				s_Game->m_RigidBodiesToRemove.emplace_back(rb2d);
				e.Destroy();
			}
		}
	});

    s_map = Map::Create("assets/Tiled/map.tmj");

    ecs::Entity floor = s_Game->GetManager().CreateEntity();
    floor.Add<Engine::Components::TransformComponent>(glm::vec3(-5), glm::vec3(0), glm::vec3(100, 1, 1));
    floor.Add<Engine::Components::Renderer>(glm::vec4(1, 1, 1, 1));
    s_Game->NewCommponent<Engine::Components::Rigidbody2DComponent>(floor, Engine::Components::Rigidbody2DComponent::BodyType::Static);

    Engine::Components::BoxCollider2DComponent::BoxCollider2DSettings settings = {};
    settings.Density = 1.0f;
    settings.Friction = 0.5f;
    settings.Restitution = 0.0f;
    settings.RestitutionThreshold = 0.5f;

    s_Game->NewCommponent<Engine::Components::BoxCollider2DComponent>(floor, settings);

    s_Game->GetManager().Refresh();
    s_Game->Run();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    if (event->type == SDL_EVENT_KEY_DOWN)
        Engine::Input::SetPressed(event->key.key, true);

    if (event->type == SDL_EVENT_KEY_UP)
        Engine::Input::SetPressed(event->key.key, false);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    s_Game->Update();
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    // Optional cleanup
}
