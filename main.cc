#include <iostream>

/*
If libraryes are not read paste into the terminal:
    source ~/geant4-11.4.0-install/bin/geant4.sh
*/

#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "PhysicsList.hh"

#include "DetectorContruction.hh"

#include "ActionInitialization.hh"

#include "ObjectMovement.hh"

int main(int argc, char** argv){

    //Just to acrivate the visual interface
    //G4UIExecutive *ui = new G4UIExecutive(argc, argv);
    G4UIExecutive *ui = nullptr;

    #ifdef G4MULTITHREADED
        G4MTRunManager *runManager = new G4MTRunManager;
    #else
        G4RunManager *runManager = new G4RunManager;
    #endif

    //Physics List
    runManager->SetUserInitialization(new PhysicsList());

    //Detector Construction
    DetectorConstruction* detector = new DetectorConstruction();
    runManager->SetUserInitialization(detector);

    //Detector Action Initialization
    runManager->SetUserInitialization(new ActionInitialization());

    //Personalized command definition
    ObjectMovement* myMessenger = new ObjectMovement(detector);

    if (argc == 1) {
        //Define UI session for interactive mode
        ui = new G4UIExecutive(argc, argv);
    }

    G4VisManager *visMenager = new G4VisExecutive();
    visMenager->Initialise();

    G4UImanager *UImanager = G4UImanager::GetUIpointer();

    if(ui){
        //To initialize the visualization
        UImanager->ApplyCommand("/control/execute vis.mac");

        ui->SessionStart();
    } 
    else{
        //Batch mode
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command+fileName);
    }

    return 0;
}