#include "RunAction.hh"

RunAction::RunAction()
{
    //The analysisManager is one but we can create a different instance in each run action. This is useful if we want to have different analysis for different runs.
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    //To merge all the tuple created in the different cores
    analysisManager->SetNtupleMerging(true);

    //To save data
    analysisManager->CreateNtuple("PET_Data", "Photon_Count_PMT");
    analysisManager->CreateNtupleIColumn("EventID");
    analysisManager->CreateNtupleIColumn("PMT_ID");
    analysisManager->CreateNtupleIColumn("PhotonCount");
    analysisManager->CreateNtupleDColumn("Timestamp_ns");
    analysisManager->FinishNtuple();

}

RunAction::~RunAction()
{
}

void RunAction::BeginOfRunAction(const G4Run *run)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    G4int runID = run->GetRunID();

    //Interaction output data
    std::stringstream strRunID;
    strRunID << runID;
    analysisManager->OpenFile("../output_data/output" + strRunID.str() + ".root");

    //Geometry data
    std::stringstream fileNameStream;
    fileNameStream << "../output_data/pmt_positions_" << runID << ".dat";
    G4String fileName = fileNameStream.str();
    std::ofstream outFile(fileName);
    if (!outFile.is_open()){
        G4cerr << "Error creating geometry file" << fileName << G4endl;
        return;
    }

    //CSV file intestation with info about the singre run
    outFile << "copyN, x_mm, y_mm, z_mm\n";

    //Store of all physical volumes
    G4PhysicalVolumeStore* pvtStore = G4PhysicalVolumeStore::GetInstance();

    //From all the physical volume find "physPMT"
    for (size_t i = 0; i < pvtStore->size(); i++) {
        G4VPhysicalVolume* pv = (*pvtStore)[i];
        //Verify if the name is correct
        if (pv->GetName() == "physPMT") {
            G4int copyN = pv->GetCopyNo();
            G4ThreeVector posPMT = pv->GetObjectTranslation();

            outFile << copyN << ","
                    << std::fixed << std::setprecision(10)
                    << posPMT.x()/mm << ", "
                    << posPMT.y()/mm << ", "
                    << posPMT.z()/mm << "\n";
        }
    }

    outFile.close();
    //G4cout << "\n[INFO] Coordinate PMT salvate con successo in '" << fileName << "'\n" << G4endl;
}

void RunAction::EndOfRunAction(const G4Run *run)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();

    G4int runID = run->GetRunID();

    G4cout << "Run " << runID << " finished." << G4endl;
}