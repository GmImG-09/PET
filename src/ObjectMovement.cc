#include "ObjectMovement.hh"

ObjectMovement::ObjectMovement(DetectorConstruction* detector): G4UImessenger(), physSource(detector) {
    
    //Create a custom command directory (e.g., /pet/)
    fPetDir = new G4UIdirectory("/pet/");
    fPetDir->SetGuidance(" custom command for PET simulation");

    //Defines the /pet/setTheta [value] command.
    fThetaCmd = new G4UIcmdWithADouble("/pet/setTheta", this);
    fThetaCmd->SetGuidance("Set the angle theta in degree for the helical movement of the detector.");
    fThetaCmd->SetParameterName("theta", false);
    fThetaCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

ObjectMovement::~ObjectMovement() {
    delete fThetaCmd;
    delete fPetDir;
}

void ObjectMovement::SetNewValue(G4UIcommand* command, G4String newValue) {
    if (command == fThetaCmd) {
        G4double theta = fThetaCmd->GetNewDoubleValue(newValue);
        physSource->UpdateHelicalPosition(theta); //Calls the function that moves the volume
    }
}