#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4AnalysisManager.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"

#include <fstream>
#include <iomanip>
#include <sstream>

class RunAction: public G4UserRunAction{
    public:
        RunAction();
        ~RunAction();

        virtual void BeginOfRunAction(const G4Run *) override;
        virtual void EndOfRunAction(const G4Run *) override;
};

#endif