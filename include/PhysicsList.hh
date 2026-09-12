#ifndef PHYSICSLIST_HH
#define PHYSICSLIST_HH

#include "G4EmStandardPhysics.hh"
#include "G4VModularPhysicsList.hh"

#include "G4RadioactiveDecayPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4OpticalPhysics.hh"

class PhysicsList: public G4VModularPhysicsList{
    public:
        PhysicsList();
        ~PhysicsList();
};

#endif