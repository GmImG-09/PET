#include "PhysicsList.hh"

PhysicsList::PhysicsList(){
    //EM physics
    RegisterPhysics(new G4EmStandardPhysics());

    //Radioactive decay physics
    RegisterPhysics(new G4RadioactiveDecayPhysics());

    //Decay physics
    RegisterPhysics(new G4DecayPhysics());

    //Optical physics
    RegisterPhysics(new G4OpticalPhysics());
}

PhysicsList::~PhysicsList(){
    
}