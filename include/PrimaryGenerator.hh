#ifndef PRIMARYGENERATOR_HH
#define PRIMARYGENERATOR_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include "G4ParticleGun.hh"

#include "G4SystemOfUnits.hh"

#include "G4IonTable.hh"

#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "Randomize.hh"
#include "G4AffineTransform.hh"
#include "G4Navigator.hh"

#include "G4TransportationManager.hh"
#include "G4Material.hh"


class PrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGenerator();
    ~PrimaryGenerator();

    //
    virtual void GeneratePrimaries(G4Event *);

private:
    G4ParticleGun *fParticleGun;
};

#endif