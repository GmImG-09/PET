#ifndef SENSITIVEDETECTOR_HH
#define SENSITIVEDETECTOR_HH

#include "G4VSensitiveDetector.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"

#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4PhysicsFreeVector.hh"

#include "G4OpticalPhoton.hh"

class SensitiveDetector: public G4VSensitiveDetector{
    public:
        SensitiveDetector(G4String);
        ~SensitiveDetector();

    private:

        //HC stands for Hits Collection 
        virtual void Initialize(G4HCofThisEvent *) override;
        virtual void EndOfEvent(G4HCofThisEvent *) override;
        //Used to define what to do when a hit is detected
        virtual G4bool ProcessHits(G4Step *, G4TouchableHistory *);

        //Define the vector that include the efficency data of our detector
        G4PhysicsFreeVector *Eff;

        //key stands for PMT ID, content stands for photocount
        std::map<G4int, G4int> pmtHits;
        std::map<G4int, G4double> pmtTimes;
};

#endif