#include "SensitiveDetector.hh"

SensitiveDetector::SensitiveDetector(G4String name): G4VSensitiveDetector(name)
{
    //Create and fill the PMT efficiency vector (linear interpolation is used for wavelenght not measured)
    Eff = new G4PhysicsFreeVector();
    std::ifstream datafile;
    datafile.open("eff.dat");
    if (!datafile.is_open()) {
        std::cerr << "Error reading eff.dat!" << "\n";
    }
    G4double fWlen, fEff;
    while(datafile >> fWlen >> fEff){
        Eff->InsertValues(fWlen, fEff/100.);
    }

    datafile.close();
}


SensitiveDetector::~SensitiveDetector()
{
    delete Eff;
}

void SensitiveDetector::Initialize(G4HCofThisEvent *)
{
}

G4bool SensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *)
{   
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    //Check if the photon is detectable (not optical photon can not be detected by PMT)
    G4Track *track = aStep->GetTrack();
    if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
        return false; 
    }

    //To take into account the efficiency of the detector the data are saved only if a random value is smaller than the efficiency measured (rejection method)
    //GetPreStepPoint is useb because the relevant property are the one of the detector incoming in the PMT
    G4StepPoint *preStepPoint = aStep->GetPreStepPoint();
    G4double photonEnergy = preStepPoint->GetTotalEnergy();
    G4double fWlen = (h_Planck*c_light)/photonEnergy/CLHEP::nm;
    //Sensor ID is saved since in the real experiment is impossible to measure the actual photon position but only the detector that lights up (and hence position)
    G4int detectorID = aStep->GetPreStepPoint()->GetTouchable()->GetCopyNumber();
    if (G4UniformRand() < Eff->Value(fWlen)){
        pmtHits[detectorID]++;
        //To check for time adjacency the time is detected
        G4double fGlobalTime = preStepPoint->GetGlobalTime();

        //Since in real life experiment the timestamp of any PMT is a flag triggered usually by the rising of the signal and hence correspond to the start of the scintillation process (that has by itself a duratio of 230ns as set in DetectorConstruction.cc) the saved time is the one correspondind to the first detected signal
        if (pmtTimes.find(detectorID)==pmtTimes.end() || fGlobalTime<pmtTimes[detectorID]) {
            pmtTimes[detectorID] = fGlobalTime;
        }
    }

    //Since the history of the particle after interaction is not needed
    track->SetTrackStatus(fStopAndKill);

    return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent *event)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    G4int evtID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    
    //Iterate over the map: for each PMT that has recorded at least one photon
    for (const auto& hit : pmtHits) {
        G4int pmtId = hit.first;
        G4int count = hit.second;
        G4double pmtTime = pmtTimes[pmtId]/CLHEP::ns;
        
        //Fill the coloumn
        analysisManager->FillNtupleIColumn(0, evtID);
        analysisManager->FillNtupleIColumn(1, pmtId);
        analysisManager->FillNtupleIColumn(2, count);
        analysisManager->FillNtupleDColumn(3, pmtTime);
        
        //Add the row to the file
        analysisManager->AddNtupleRow();
    }
    
    //Reset maps for the next event
    pmtHits.clear();
    pmtTimes.clear();
}