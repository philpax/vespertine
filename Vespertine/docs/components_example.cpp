#include <iostream>
#include <map>
#include <memory>
#include <typeinfo>
#include <chrono>
#include <cstdint>

// Implementation notes:
// C++ RTTI is used, but not ideal: in real situation, would use custom RTTI
// solution using auto-generated hashes
// Message broadcasting uses strings - also not ideal, would also use hashes

class Entity;
bool game_running = true;

// Base declaration
namespace components
{
	class Base
	{
	public:
		Base()
		{
		}

		// Inheriting constructors? This is C++. We don't 'inherit' constructors.
		void PostConstruct( std::shared_ptr<Entity> entity )
		{
			_entity = entity;
		}

		virtual void Update( float dt )
		{
		}

		virtual void Receive( std::string const& msg )
		{
		}

	protected:
		std::shared_ptr<Entity> _entity;
	};
}

class Entity : public std::enable_shared_from_this<Entity>
{
public:
	Entity( uint32_t index )
		: _index( index )
	{
		_last_update = std::chrono::high_resolution_clock::now();
	}

	void Update()
	{
		namespace ch = std::chrono;

		ch::high_resolution_clock::time_point now = 
			ch::high_resolution_clock::now();

		float dt = 
			ch::duration_cast<ch::duration<float>>( now - _last_update ).count();

		for (auto component: _components)
			component.second->Update( dt );

		_last_update = now;
	}

	template <typename Ty>
	void AddComponent()
	{
		std::shared_ptr<Ty> component = std::make_shared<Ty>();
		component->PostConstruct( shared_from_this() );

		_components[typeid(Ty).hash_code()] = component;
	}

	template <typename Ty>
	std::shared_ptr<Ty> GetComponent()
	{
		size_t hash = typeid(Ty).hash_code();

		if (_components.find( hash ) == _components.end())
			return std::shared_ptr<Ty>();

		return std::static_pointer_cast<Ty>( _components[hash] );
	}

	void BroadcastToComponents( std::string const& msg )
	{
		for (auto component: _components)
			component.second->Receive( msg );
	}

	uint32_t GetIndex()
	{
		return _index;
	}

private:
	std::chrono::high_resolution_clock::time_point _last_update;
	std::map<size_t, std::shared_ptr<components::Base>> _components;
	uint32_t _index;
};

// Derived declaration
namespace components
{
	class Position : public Base
	{
	public:
		Position()
			: y(0.0f)
		{
		}

		float y;
	};

	class Physics : public Base
	{
	public:
		Physics()
			: velocity(0.0f), acceleration(-9.81f)
		{
		}

		void Update( float dt )
		{
			std::shared_ptr<Position> position = 
				_entity->GetComponent<Position>();

			// If the parent entity has no position component, we have nothing
			// to update
			if (!position)
				return;

			velocity += acceleration * dt;
			position->y += velocity * dt;

			if (position->y <= 0.0f)
			{
				position->y = 0.0f;
				_entity->BroadcastToComponents( "GroundHit" );
				velocity = 1.0f;
			}
		}

		float velocity;
		float acceleration;
	};

	class Sound : public Base
	{
		void Receive( std::string const& msg )
		{
			if (msg == "GroundHit")
			{
				std::cout << "[Sound] GroundHit from " << _entity->GetIndex() << std::endl;
			}
		}
	};

	class GameEndOnGroundHit : public Base
	{
		void Receive( std::string const& msg )
		{
			if (msg == "GroundHit")
			{
				std::cout << "[GameEndOnGroundHit] GroundHit from " << _entity->GetIndex() << std::endl;
				game_running = false;
			}
		}
	};

	class Group : public Base
	{
	public:
		void Update( float dt )
		{
			for (auto component: _components)
				component->Update( dt );
		}

		void Receive( std::string const& msg )
		{
			for (auto component: _components)
				component->Receive( msg );
		}

		template <typename Ty>
		void AddComponent()
		{
			std::shared_ptr<Ty> component = std::make_shared<Ty>();
			component->PostConstruct( _entity );

			_components.push_back( component );
		}

	private:
		std::vector<std::shared_ptr<Base>> _components;
	};
}

int main()
{
	std::shared_ptr<Entity> ent1 = std::make_shared<Entity>( 1 );
	std::shared_ptr<Entity> ent2 = std::make_shared<Entity>( 2 );

	ent1->AddComponent<components::Position>();
	ent1->AddComponent<components::Physics>();
	ent1->AddComponent<components::Sound>();

	ent2->AddComponent<components::Position>();
	ent2->AddComponent<components::Physics>();
	ent2->AddComponent<components::GameEndOnGroundHit>();

	ent1->GetComponent<components::Position>()->y = 1.0f;
	ent2->GetComponent<components::Position>()->y = 2.0f;

	std::cout << "Starting game" << std::endl;

	while (game_running)
	{
		ent1->Update();
		ent2->Update();
	}

	std::cout << "Ending game" << std::endl;
}