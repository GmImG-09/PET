#include "PrimaryGenerator.hh"

PrimaryGenerator::PrimaryGenerator()
{
    //Create a particle gun (1 particle per event)
    fParticleGun = new G4ParticleGun(1);

    /*
    //Particle gun position
    G4double x = 0. * m;
    G4double y = 0. * m;
    //G4double z = 0. * m;
    G4double z = -1. * cm;
    G4ThreeVector pos(x, y, z);
    fParticleGun->SetParticlePosition(pos);
    */

    //Particle gun direction
    G4double px = 0.;
    G4double py = 0.;
    //G4double pz = 1.;
    G4double pz = 0.;
    G4ThreeVector mom(px, py, pz);
    fParticleGun->SetParticleMomentumDirection(mom);

    /*
    //FOR PARTICLE GUN
    //Particle type
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition *particle = particleTable->FindParticle("gamma");
    fParticleGun->SetParticleDefinition(particle);

    //Particle energy
    G4double energy = 1. * MeV;
    fParticleGun->SetParticleEnergy(energy);
    */
}

PrimaryGenerator::~PrimaryGenerator(){
    delete fParticleGun;
}

void PrimaryGenerator::GeneratePrimaries(G4Event *anEvent){
    G4VPhysicalVolume* physSource = G4PhysicalVolumeStore::GetInstance()->GetVolume("physSource");
    
    if (!physSource) {
        G4cerr << "Error: physSource not found in PhysicalVolumeStore!" << G4endl;
        return;
    }

    //Source physic volume
    G4LogicalVolume* logicSource = physSource->GetLogicalVolume();
    G4VSolid* solidSource = logicSource->GetSolid();

    //Box in which the source is inside
    G4ThreeVector pMin, pMax;
    solidSource->BoundingLimits(pMin, pMax);

    //Global traslation and rotation of the source
    G4ThreeVector localPos, globalPos;
    G4ThreeVector translation = physSource->GetTranslation();
    G4RotationMatrix* rotPtr = physSource->GetObjectRotation();
    G4RotationMatrix rotation = rotPtr ? *rotPtr : G4RotationMatrix();
    G4AffineTransform transform(rotation, translation);

    //Extract max activity
    static G4double maxActivity = -1.0; 

    //If maxActivity < 0 it means that is the first research and maxActivity must be found otherwise is in memory and isn't needed to search for it
    if (maxActivity < 0.0) {
        maxActivity = 0.0;
        
        //List of all material used in the simulation
        const G4MaterialTable* matTable = G4Material::GetMaterialTable();
        
        //Cicle over all material
        for (size_t i = 0; i < matTable->size(); ++i) {
            G4Material* mat = (*matTable)[i];
            G4MaterialPropertiesTable* mpt = mat->GetMaterialPropertiesTable();
            
            //Check if the material has he property calle "ACTIVITY"
            if (mpt && mpt->ConstPropertyExists("ACTIVITY")) {
                G4double activity = mpt->GetConstProperty("ACTIVITY");
                if (activity > maxActivity) {
                    maxActivity = activity; //update maximum
                }
            }
        }
        
        //If no material have the property "ACTIVITY"
        if (maxActivity == 0.0) {
            maxActivity = 1.0; 
            G4cout << "\n[WARNING] No material with property \"ACTIVITY\" were found. maxActivity set to 1.0\n" << G4endl;
        } else {
            G4cout << "\n[INFO] maxActivity automaticalli found: " << maxActivity << "\n" << G4endl;
        }
    }

    G4Navigator* navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    
    //Extract a point inside de source (rejection method)
    G4bool positionAccepted = false;
    do {
        G4double x = pMin.x() + G4UniformRand() * (pMax.x() - pMin.x());
        G4double y = pMin.y() + G4UniformRand() * (pMax.y() - pMin.y());
        G4double z = pMin.z() + G4UniformRand() * (pMax.z() - pMin.z());
        localPos = G4ThreeVector(x, y, z);
        //Verify if localPos is inside solidSource 
        if (solidSource->Inside(localPos) != kOutside) {
            //Convert in global coordinates
            globalPos = transform.TransformPoint(localPos);

            //To emit proportionally with the isotope density (rejection method)
            G4VPhysicalVolume* currentVolume = navigator->LocateGlobalPointAndSetup(globalPos);
            
            if (currentVolume) {
                G4Material* localMat = currentVolume->GetLogicalVolume()->GetMaterial();
                G4MaterialPropertiesTable* mpt = localMat->GetMaterialPropertiesTable();
                
                if (mpt && mpt->ConstPropertyExists("ACTIVITY")) {
                    G4double localActivity = mpt->GetConstProperty("ACTIVITY");
                    
                    if (G4UniformRand() * maxActivity <= localActivity) {
                        positionAccepted = true;
                    }
                }
            }
        }
    } while (!positionAccepted);

    fParticleGun->SetParticlePosition(globalPos);

    //FOR DECAY
    //it's a fluorine isotope (beta+ sources) but there is the annihilation of the positron with an electron that produces two 511 keV photons
    G4int Z = 9;
    G4int A = 18;

    G4double energy = 0.*keV; //Initial ion kinetic energy
    G4double charge = 0.*eplus;
    G4ParticleDefinition *ion = G4IonTable::GetIonTable()->GetIon(Z, A, energy);
    fParticleGun->SetParticleDefinition(ion);
    fParticleGun->SetParticleCharge(charge);

    //Create a primary vertex and add it to the event
    fParticleGun->GeneratePrimaryVertex(anEvent);
}