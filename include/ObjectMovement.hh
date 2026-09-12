#ifndef OBJECTMOVEMENT_HH
#define OBJECTMOVEMENT_HH

#pragma once

#include "G4UImessenger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIdirectory.hh"

#include "DetectorContruction.hh"

class ObjectMovement : public G4UImessenger {
public:
    ObjectMovement(DetectorConstruction* detector);
    virtual ~ObjectMovement();
    
    virtual void SetNewValue(G4UIcommand* command, G4String newValue);

private:
    DetectorConstruction* physSource;
    G4UIdirectory* fPetDir;
    G4UIcmdWithADouble* fThetaCmd;
};

#endif