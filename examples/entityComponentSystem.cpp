#include <levikno/levikno.h>

// INFO: this program demonstrates the entity component system (ECS) utilities from the library


// components examples
struct PosComponent
{
	float x, y;
};

struct VelocityComponent
{
	float x, y;
};

struct AcceleractionComponent
{
	float x, y;
};

struct NameComponent
{
	std::string name;
};


// example of system update function
void updatePosition(PosComponent& pos, VelocityComponent& velocity, AcceleractionComponent& acceleration)
{
	velocity.x += acceleration.x;
	velocity.y += acceleration.y;

	pos.x += velocity.x;
	pos.y += velocity.y;
}

int main(int argc, char** argv)
{
	// [Create Context]
	// create the context to load the library
	LvnContextCreateInfo lvnCreateInfo{};
	lvnCreateInfo.logging.enableLogging = true;
	lvnCreateInfo.logging.disableCoreLogging = true;

	lvn::createContext(&lvnCreateInfo);


	// [Entities]
	// creating entities
	// - Note that entities are only 64-bit unsigned integers underneath
	LvnEntity e1 = lvn::createEntity();
	LvnEntity e2 = lvn::createEntity();
	LvnEntity e3 = lvn::createEntity();

	LVN_TRACE("e1: %zu", e1);
	LVN_TRACE("e2: %zu", e2);
	LVN_TRACE("e3: %zu", e3);


	PosComponent posComp{};
	posComp.x = 10.0f;
	posComp.y = 25.0f;

	printf("\n");

	// [Components]
	// adding components to entities
	// - there are many ways to add a component to an entity
	// - make sure the type is clearly defined
	//
	lvn::entityAddComponent(e1, posComp);                         // from external struct
	lvn::entityAddComponent(e2, PosComponent{ -2.0f, 13.0f });    // within function
	lvn::entityAddComponent(e3, PosComponent());                  // add component with default constructor/default values

	// lvn::entityAddComponent(e3, { -2.0f, 13.0f });             // will not compile, type of component not defined


	// multiple components can be added at the same time
	lvn::entityAddComponent(e1, VelocityComponent{ 2.1f, 0.0f }, AcceleractionComponent{ -0.1f, -9.8f }, NameComponent{ "entity1" });
	lvn::entityAddComponent(e2, VelocityComponent{ 0.6f, 3.0f }, AcceleractionComponent{ 2.5f, -0.8f }, NameComponent{ "entity2" });


	// components can be retrieved individually
	// - make sure to pass it into a reference variable to change the values
	PosComponent& posComp1 = lvn::entityGetComponent<PosComponent>(e1);
	NameComponent nameComp1 = lvn::entityGetComponent<NameComponent>(e1); // NOTE: not taken by reference, making a copy instead

	LVN_TRACE("\"%s\" pos comp: { x = %f, y = %f }", nameComp1.name.c_str(), posComp1.x, posComp1.y);

	// change values of components
	posComp1 = { 10.5f, 22.3f };
	nameComp1.name = "block1"; // since NameComponent was not taken by reference, the component does not change value tied with the entity

	// get components again
	PosComponent& posCompAgain1 = lvn::entityGetComponent<PosComponent>(e1);
	NameComponent& nameCompAgain1 = lvn::entityGetComponent<NameComponent>(e1);

	// NOTE: position values should change, but name stays the same
	LVN_TRACE("\"%s\" pos comp: { x = %f, y = %f }", nameCompAgain1.name.c_str(), posCompAgain1.x, posCompAgain1.y);


	printf("\n");

	// [Systems]
	// creating and updating systems are straighforward and simple to use in levikno.
	// To update multiple entities through a system, levikno requires the type of components to be used
	// and the system update function to update the components
	// - pass in the entities and the number of entities that need to be updated
	// - pass in a function pointer with the components types taken by reference in the function paramter
	// - Note that every entity passed in must have all the components that are passed in as arguments for the function parameter
	// Ex:
	lvn::entityUpdateSystem(&e1, 1, updatePosition);


	PosComponent& posCompUpdate = lvn::entityGetComponent<PosComponent>(e1);
	VelocityComponent& velocityCompUpdate = lvn::entityGetComponent<VelocityComponent>(e1);

	LVN_TRACE("pos comp: { x = %f, y = %f }", posCompUpdate.x, posCompUpdate.y);
	LVN_TRACE("velocity comp: { x = %f, y = %f }", velocityCompUpdate.x, velocityCompUpdate.y);


	// you can also pass lambdas into the update function
	// make sure to include the '+' symbol in front of the lambda to convert it to a function pointer
	lvn::entityUpdateSystem(&e1, 1, +[](PosComponent& pos, VelocityComponent& velocity, AcceleractionComponent& acceleration)
	{
		velocity.x += acceleration.x;
		velocity.y += acceleration.y;

		pos.x += velocity.x;
		pos.y += velocity.y;
	});

	LVN_TRACE("pos comp: { x = %f, y = %f }", posCompUpdate.x, posCompUpdate.y);
	LVN_TRACE("velocity comp: { x = %f, y = %f }", velocityCompUpdate.x, velocityCompUpdate.y);


	// multiple entities with the same components can be updated at the same time
	// make sure all entities have the components passed into the parameters of the system update function

	std::vector<LvnEntity> entities = { e1, e2 };

	lvn::entityUpdateSystem(entities.data(), entities.size(), +[](PosComponent& pos, VelocityComponent& velocity, AcceleractionComponent& acceleration)
	{
		velocity.x += acceleration.x;
		velocity.y += acceleration.y;

		pos.x += velocity.x;
		pos.y += velocity.y;
	});

	entities.push_back(e3);

	// will not work, entities now includes e3 which does not have a velocity or acceleration component
	// matching with the function parameter in the system update function

	// lvn::entityUpdateSystem(entities.data(), entities.size(), +[](PosComponent& pos, VelocityComponent& velocity, AcceleractionComponent& acceleration)
	// {
	//      velocity.x += acceleration.x;
	//      velocity.y += acceleration.y;

	//      pos.x += velocity.x;
	//      pos.y += velocity.y;
	// });


	// entities can be desctroyed when no longer needed
	lvn::destroyEntity(e3);

	// components can be removed from an entity when no longer needed
	lvn::entityRemoveComponent<PosComponent>(e2);

	// multiple components can be removed at the same time
	lvn::entityRemoveComponent<PosComponent, VelocityComponent, AcceleractionComponent>(e1);

	// terminate context
	lvn::terminateContext();

	return 0;
}
