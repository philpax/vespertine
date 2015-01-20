#include <iostream>
#include <memory>
#include <map>
#include <functional>
#include <string>

// Macro that declares an entity, creates a factory for it, and then starts the
// complete declaration
#define ENTITY_CLASS( entity ) \
  class entity; \
  static EntityFactoryImplementation<entity> entity##Factory( #entity ); \
  class entity : public Entity \

// Entity base class
class Entity
{
public:
  Entity()
  {
  }

  void Init( std::string const& name )
  {
    _name = name;
  }

  virtual void Hello() = 0;
    
protected:
  std::string _name;
};

// Declare EntityFactory so we can use it in EntityFactoryMap
class EntityFactory;

// Creates entities
class EntityFactoryMap
{
public:
  template <typename Ty>
  std::shared_ptr<Ty> Create( std::string const& name )
  {
    return std::static_pointer_cast<Ty>( CreateEntity( name ) );
  }

  void AddFactory( std::string const& name, EntityFactory* factory )
  {
    _map[ name ] = factory;
  }

private:
  // Needs to be broken up so that we have the full declaration
  std::shared_ptr<Entity> CreateEntity( std::string const& name );

  std::map<std::string, EntityFactory*> _map;
};

static EntityFactoryMap EntityFactoryMap_;

// Entity Factory interface
class EntityFactory
{
public:
  virtual std::shared_ptr<Entity> Create( std::string const& name ) = 0;
};

// Entity Factory implementation
template <typename Ty>
class EntityFactoryImplementation : public EntityFactory
{
public:
  EntityFactoryImplementation( std::string const& name )
  {
    EntityFactoryMap_.AddFactory( name, this );
  }

  std::shared_ptr<Entity> Create( std::string const& name )
  {
    std::shared_ptr<Entity> ent = std::make_shared<Ty>();
    ent->Init( name );
    return ent;
  }
};

std::shared_ptr<Entity> EntityFactoryMap::CreateEntity( std::string const& name )
{
  EntityFactory* factory = _map[ name ];
  return factory->Create( name );
}

// Example classes
ENTITY_CLASS( Player )
{
public:
  void Hello()
  {
    std::cout << "Hello from " << _name << "!" << std::endl;
  }
};

ENTITY_CLASS( NPC )
{
public:
  void Hello()
  {
    std::cout << "Bonjour from " << _name << "!" << std::endl;
  }
};

int main()
{
  std::shared_ptr<Player> player = EntityFactoryMap_.Create<Player>( "Player" );
  std::shared_ptr<NPC> npc = EntityFactoryMap_.Create<NPC>( "NPC" );

  player->Hello();
  npc->Hello();
}