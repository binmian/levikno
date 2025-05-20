#include "levikno/levikno.h"
#include <levikno/lvn_ecs.h>

#include <string>
#include <vector>

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
    // [Entities]
    // creating entities
    // - Note that entities are only 64-bit unsigned integers underneath
    LvnEntity e1 = lvn::createEntity();
    LvnEntity e2 = lvn::createEntity();
    LvnEntity e3 = lvn::createEntity();

    printf("e1: %zu\n", e1);
    printf("e2: %zu\n", e2);
    printf("e3: %zu\n", e3);


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

    // lvn::entityAddComponent(e3, { -2.0f, 13.0f });             // will not compile, type of component not clearly defined


    // multiple components can be added at the same time
    lvn::entityAddComponent(e1, VelocityComponent{ 2.1f, 0.0f }, AcceleractionComponent{ -0.1f, -9.8f }, NameComponent{ "entity1" });
    lvn::entityAddComponent(e2, VelocityComponent{ 0.6f, 3.0f }, AcceleractionComponent{ 2.5f, -0.8f }, NameComponent{ "entity2" });


    // components can be retrieved individually
    // - make sure that a component is retrieved by reference or else you wont be able to update the component
    PosComponent& posComp1 = lvn::entityGetComponent<PosComponent>(e1);
    NameComponent nameComp1 = lvn::entityGetComponent<NameComponent>(e1); // NOTE: not taken by reference, making a copy instead

    printf("\"%s\" pos comp: { x = %f, y = %f }\n", nameComp1.name.c_str(), posComp1.x, posComp1.y);

    // change values of components
    posComp1 = { 10.5f, 22.3f };
    nameComp1.name = "block1"; // NameComponent was not taken by reference, entity e1 will not change name

    // get components again
    PosComponent& posCompAgain1 = lvn::entityGetComponent<PosComponent>(e1);
    NameComponent& nameCompAgain1 = lvn::entityGetComponent<NameComponent>(e1);

    // NOTE: position values should change, but name stays the same
    printf("\"%s\" pos comp: { x = %f, y = %f }\n", nameCompAgain1.name.c_str(), posCompAgain1.x, posCompAgain1.y);


    printf("\n");

    // [Systems]
    // using update systems are straighforward to use in levikno.
    // entities can be updated through a callback function that updates the entities' components through the callback's parameter
    // - to update multiple entities at once, pass in an array of entities and the number of entities in the array
    // - pass in a function pointer that updates the entities with the component types taken by reference in the paramter
    // - Note that every entity passed in must have all the components that are present in the function's parameter
    // Ex:
    lvn::entityUpdateSystem(&e1, 1, updatePosition);


    PosComponent& posCompUpdate = lvn::entityGetComponent<PosComponent>(e1);
    VelocityComponent& velocityCompUpdate = lvn::entityGetComponent<VelocityComponent>(e1);

    printf("First System update:\n");
    printf("pos comp: { x = %f, y = %f }\n", posCompUpdate.x, posCompUpdate.y);
    printf("velocity comp: { x = %f, y = %f }\n", velocityCompUpdate.x, velocityCompUpdate.y);


    // lambdas can be passed in instead
    // some c++ compilers may interpret lambdas as a struct with an overload operator, make sure to tell the compiler to treat lambdas as function pointers
    // the '+' symbol in front of the lambda will tell the compiler to convert the lambda to a function pointer
    lvn::entityUpdateSystem(&e1, 1, +[](PosComponent& pos, VelocityComponent& velocity, AcceleractionComponent& acceleration)
    {
        velocity.x += acceleration.x;
        velocity.y += acceleration.y;

        pos.x += velocity.x;
        pos.y += velocity.y;
    });

    printf("Second System update:\n");
    printf("pos comp: { x = %f, y = %f }\n", posCompUpdate.x, posCompUpdate.y);
    printf("velocity comp: { x = %f, y = %f }\n", velocityCompUpdate.x, velocityCompUpdate.y);


    // multiple entities with the same components can be updated at the same time
    // make sure each entity has all the components present in the parameter of the update function

    std::vector<LvnEntity> entities = { e1, e2 };

    lvn::entityUpdateSystem(entities.data(), entities.size(), +[](PosComponent& pos, VelocityComponent& velocity, AcceleractionComponent& acceleration)
    {
        velocity.x += acceleration.x;
        velocity.y += acceleration.y;

        pos.x += velocity.x;
        pos.y += velocity.y;
    });

    printf("Third System update:\n");
    printf("pos comp: { x = %f, y = %f }\n", posCompUpdate.x, posCompUpdate.y);
    printf("velocity comp: { x = %f, y = %f }\n", velocityCompUpdate.x, velocityCompUpdate.y);

    entities.push_back(e3);

    // will not work, entities now includes e3 which does not have a velocity or acceleration component to update in the system

    // lvn::entityUpdateSystem(entities.data(), entities.size(), +[](PosComponent& pos, VelocityComponent& velocity, AcceleractionComponent& acceleration)
    // {
    //      velocity.x += acceleration.x;
    //      velocity.y += acceleration.y;

    //      pos.x += velocity.x;
    //      pos.y += velocity.y;
    // });


    // when an entity is no longer needed, it can be destroyed
    lvn::destroyEntity(e3);

    // components can be removed from an entity when no longer needed
    lvn::entityRemoveComponent<PosComponent>(e2);

    // multiple components can be removed at the same time
    lvn::entityRemoveComponent<PosComponent, VelocityComponent, AcceleractionComponent>(e1);


    // optionally if you want to restart the entire ecs system, just call lvn::ecsRestart()
    // this will clear all components added to the components manager and reset the entity id count.
    // doing this will make all created entities and added components prior invalid so only do this
    // when you want to start a completely new ecs session
    lvn::ecsRestart();

    return 0;
}
