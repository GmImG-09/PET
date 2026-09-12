#include "DetectorContruction.hh"

DetectorConstruction::DetectorConstruction()
{
}

DetectorConstruction::~DetectorConstruction()
{
}

//Construct
G4VPhysicalVolume *DetectorConstruction::Construct()
{   
    fPitch = 5*mm;
    //In geant4 volumes can't overlap each other, so we need to check it
    G4bool chechOverlaps = true;

    //Define materials
    G4NistManager *nist = G4NistManager::Instance();

    //Scintillator material
    G4Material *scintillatorMat = nist->FindOrBuildMaterial("G4_SODIUM_IODIDE");
    //Scintillator optical properties
    G4MaterialPropertiesTable *mptNaI = new G4MaterialPropertiesTable();
    //Energies of the typical emitted photon (in NaI)
    G4double photonEnergy[] = {2.0*eV, 2.98*eV, 3.5*eV};
    //Scintillator refractive index (assumed fixed in the energy range)
    G4double rIndex[] = {1.85, 1.85, 1.85};
    mptNaI->AddProperty("RINDEX", photonEnergy, rIndex, 3);
    //Scintillation spectrum
    G4double scintEmission[] = {0.1, 1.0, 0.1};
    mptNaI->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy, scintEmission, 3);
    //Trasparency
    G4double absLength[] = {50.*cm, 50.*cm, 50.*cm};
    mptNaI->AddProperty("ABSLENGTH", photonEnergy, absLength, 3);
    //Scintillation efficiency (3.8E+4 photons emitted for each MeV of the incident photon energy) standard poissonian distribution is assumed
    mptNaI->AddConstProperty("SCINTILLATIONYIELD", 38./keV);
    mptNaI->AddConstProperty("RESOLUTIONSCALE", 1.0);
    //Time constant of the scintillation process (is assumed the same for all emitions)
    mptNaI->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 230.*ns);
    mptNaI->AddConstProperty("SCINTILLATIONYIELD1", 1.0);
    scintillatorMat->SetMaterialPropertiesTable(mptNaI);

    //Photomultiplier material
    G4Material *glassMat = nist->FindOrBuildMaterial("G4_GLASS_PLATE");
    G4MaterialPropertiesTable *mptGlass = new G4MaterialPropertiesTable();
    //Scintillator refractive index (assumed fixed in the energy range)
    G4double rIndexGlass[] = {1.5, 1.5, 1.5};
    mptGlass->AddProperty("RINDEX", photonEnergy, rIndexGlass, 3);
    glassMat->SetMaterialPropertiesTable(mptGlass);

    //Mirror material
    G4OpticalSurface *mirrorSurface = new G4OpticalSurface("mirrorSurface");
    mirrorSurface->SetType(dielectric_metal);
    mirrorSurface->SetFinish(ground);
    mirrorSurface->SetModel(unified);
    G4MaterialPropertiesTable *mptMirror = new G4MaterialPropertiesTable();
    G4double reflettivity[] = {1., 1., 1.};
    mptMirror->AddProperty("REFLECTIVITY", photonEnergy, reflettivity, 3);

    //Source material (fluorine-18)
    G4Isotope *F18 = new G4Isotope("F18", 9, 18, 18.000938*g/mole);
    G4Element *elF18 = new G4Element("Fluorine-18", "F18", 1);
    elF18->AddIsotope(F18, 100.*perCent);
    //To attribute a material to the source we need to define a material with the isotope we just defined, so we can use it in the simulation, the III arg is the number of elements in the material, in this case is 1 because we have only one isotope
    G4Material *matF18 = new G4Material("matF18", 1.51*g/cm3, 1);
    matF18->AddElement(elF18, 100.*perCent);
    G4Material *water = nist->FindOrBuildMaterial("G4_WATER");

    G4Material *matF18_01 = new G4Material("matF18_01", 1.0*g/cm3, 2);
    matF18_01->AddMaterial(water, 99.9*perCent);
    matF18_01->AddMaterial(matF18, 0.1*perCent);

    G4MaterialPropertiesTable *mpt01 = new G4MaterialPropertiesTable();
    mpt01->AddConstProperty("ACTIVITY", 1.0, true); //Base ACTIVITY
    matF18_01->SetMaterialPropertiesTable(mpt01);

    //Hotspot - 5 times more concentrated
    G4Material *matF18_05 = new G4Material("matF18_05", 1.0*g/cm3, 2);
    matF18_05->AddMaterial(water, 99.5*perCent);
    matF18_05->AddMaterial(matF18, 0.5*perCent);

    G4MaterialPropertiesTable *mpt05 = new G4MaterialPropertiesTable();
    mpt05->AddConstProperty("ACTIVITY", 5.0, true); // Concentrazione 5x
    matF18_05->SetMaterialPropertiesTable(mpt05);

    //World material
    G4Material *worldMat = nist->FindOrBuildMaterial("G4_AIR");
    G4MaterialPropertiesTable *mptAir = new G4MaterialPropertiesTable();
    G4double rIndexAir[] = { 1.0, 1.0, 1.0 };
    mptAir->AddProperty("RINDEX", photonEnergy, rIndexAir, 3);
    worldMat->SetMaterialPropertiesTable(mptAir);

    //Other materials that we will use in the simulation
    G4Material *leadMat = nist->FindOrBuildMaterial("G4_Pb");

    //Define the geometry of the world volume
    G4double xWorld = 1.0*m;
    G4double yWorld = 1.0*m;
    G4double zWorld = 1.0*m;
    //Box is the function that defines parallelepipeds
    //Is good custom to use, as name of the solid, the name of the object itself so, in this case "solidWorld"
    //The 0.5 is because the box is defined from the center to the edges, so we need to divide by 2 the dimensions of the box
    G4Box *solidWorld = new G4Box("solidWorld", .5*xWorld, .5*yWorld, .5*zWorld); 

    //Is not enough to define the solid, we need to define the logical volume, that is the solid with a material
    G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");

    //Finally we need to define the physical volume, that is the logical volume with a position and a rotation in the world
    /*
    - I parameter: is the rotation, in this case we don't want to rotate the world, so we put 0
    - II parameter: is the position, in this case we want to put the world in the origin, so we put G4ThreeVector(0, 0, 0)
    - III parameter: is the logical volume that we want to place, in this case the logical volume of the world
    - IV parameter: is the name of the physical volume, in this case we put "physWorld"
    - V parameter: is the mother volume, in this case we don't have a mother volume (is needed if we want to place the volume inside another volume), so we put 0
    - VI parameter: is a boolean that indicates if we want to check for overlaps, in this case we put false because we don't need to check for overlaps in the world volume
    - VII parameter: is the copy number, in this case we put 0 because we don't need to copy the world volume
    - VIII parameter: is a boolean that indicates if we want to check for overlaps
    */
    G4VPhysicalVolume *physWorld = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logicWorld, "physWorld", 0, false, 0, chechOverlaps);

    //Flourine source
    G4double ax = 6.*cm; // Semi-asse X
    G4double ay = 4.*cm; // Semi-asse Y
    G4double az = 10.*cm; // Semi-asse Z
    /*
    The argument are:
    - I parameter: is the name of the solid, in this case we put "solidSource"
    - II parameter: is the inner radius, in this case we put 0 because we want a solid sphere
    - III parameter: is the outer radius, in this case we put the radius of the source
    - IV parameter: is the starting phi angle, in this case we put 0 because we want a full sphere
    - V parameter: is the delta phi angle, in this case we put 2*pi because we want a full sphere
    - VI parameter: is the starting theta angle, in this case we put 0 because we want a full sphere
    - VII parameter: is the delta theta angle, in this case we put pi because we want a full sphere
    */
    //G4Sphere *solidSource = new G4Sphere("solidSource", 0., sourceRadius, 0., 360*deg, 0., 180.*deg);
    G4Ellipsoid *solidSource = new G4Ellipsoid("solidSource", ax, ay, az);
    G4LogicalVolume *logicSource = new G4LogicalVolume(solidSource, matF18_01, "logicSource");
    physSource = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicSource, "physSource", logicWorld, false, 0, chechOverlaps);
    
    //Random higher density spot
    G4double spotRadius = 1*cm; // Raggio delle zone iper-dense
    G4Sphere *solidSpot = new G4Sphere("solidSpot", 0., spotRadius, 0., 360*deg, 0., 180.*deg);
    G4LogicalVolume *logicSpot = new G4LogicalVolume(solidSpot, matF18_05, "logicSpot");
    
    G4VisAttributes *spotVisAttributes = new G4VisAttributes(G4Colour(0., 1., 0., 0.9));
    spotVisAttributes->SetForceSolid(true);
    logicSpot->SetVisAttributes(spotVisAttributes);

    //Generate two spots within the ellipsoid.
    for (int i = 0; i < 2; i++) {
        G4ThreeVector spotPos;
        bool validPosition = false;
        
        while (!validPosition) {
            //Random sampling within a box-shaped volume matching the dimensions of the ellipsoid
            G4double x = (2.0 * G4UniformRand() - 1.0) * ax;
            G4double y = (2.0 * G4UniformRand() - 1.0) * ay;
            G4double z = (2.0 * G4UniformRand() - 1.0) * az;
            
            //To ensure that the spheres are inside the original volume
            G4double safeX = ax - spotRadius;
            G4double safeY = ay - spotRadius;
            G4double safeZ = az - spotRadius;
            if ((x*x)/(safeX*safeX) + (y*y)/(safeY*safeY) + (z*z)/(safeZ*safeZ) <= 1.0) {
                spotPos = G4ThreeVector(x, y, z);
                validPosition = true;
            }
        }
         
        //Using 'logicSource' as mother logis volume the two element are ancored
        new G4PVPlacement(0, spotPos, logicSpot, "physSpot", logicSource, false, i, chechOverlaps);
    }

    G4VisAttributes *sourceVisAttributes = new G4VisAttributes(G4Colour(1., 0., 1., .5));
    sourceVisAttributes->SetForceSolid(true);
    logicSource->SetVisAttributes(sourceVisAttributes);

    //Define the geometry of the scintillator volume
    G4double scintillatorSize = 5.*cm;

    G4Box *solidScintillator = new G4Box("solidScintillator", .5*scintillatorSize, .5*scintillatorSize, .5*scintillatorSize);
    //The type is not needed because it's a private variable, it's so because I want to access it in order to save data (for example the energy deposited in the detector)
    logicScintillator = new G4LogicalVolume(solidScintillator, scintillatorMat, "logicScintillator");

    //Define the geometry of the photon multiplier
    G4double pmtThickness = 1.*mm;
    G4double pmtSize = .9*scintillatorSize;
    G4Box *solidPMT = new G4Box("solidPMT", .5*pmtSize, .5*pmtSize, .5*pmtThickness);
    logicPMT = new G4LogicalVolume(solidPMT, glassMat, "logicPMT");

    //To have the PET configuration
    G4int k_rings = 5; //n. of ring                      
    G4int n_crystals_per_ring = 32; //n. of sensors for each ring
    G4double R = 1.1*scintillatorSize*(1/(2.*std::tan(CLHEP::pi/(double)n_crystals_per_ring)) + 1/2.); //ring radious
    G4double z_spacing = 1.1*scintillatorSize;//z spacing between rings
    G4double z_start = -(k_rings - 1) / 2.0 * z_spacing; //to center the ring z position in z=0
    G4double R_pmt = R + 0.5 * scintillatorSize + 0.5 * pmtThickness;

    G4int copyN = 0; //n. of copy counter

    

    for (G4int k = 0; k < k_rings; k++) {
        G4double z_pos = z_start+k*z_spacing; //z coordinate of k-th ring
        for (G4int i = 0; i < n_crystals_per_ring; i++) {
            //For each crystal the occupied angle is 2*pi/n_crystals_per_ring
            G4double phi = (i*2.+(k%2))*CLHEP::pi/(double)n_crystals_per_ring;
            G4ThreeVector posScint(
                R * std::cos(phi), 
                R * std::sin(phi), 
                z_pos);
            //Since PMT is radially external to the crystal 
            G4ThreeVector posPMT(
                R_pmt*std::cos(phi), 
                R_pmt*std::sin(phi), 
                z_pos);
            G4RotationMatrix* rot = new G4RotationMatrix();
            rot->rotateZ(-phi); 

            //Scintillator
            G4VPhysicalVolume *physScintillator = new G4PVPlacement(
                rot, 
                posScint, 
                logicScintillator, 
                "physScintillator", 
                logicWorld, 
                false, 
                copyN, 
                chechOverlaps
            );

            //PMT
            rot->rotateY(CLHEP::pi / 2.0);
            G4VPhysicalVolume *physPMT = new G4PVPlacement(
                rot, 
                posPMT, 
                logicPMT, 
                "physPMT", 
                logicWorld, 
                false, 
                copyN, 
                chechOverlaps
            );

            //Optical isolator 
            G4String borderName = "ScintWorldBorder_" + std::to_string(copyN);
            G4LogicalBorderSurface *LogicMirrorSurface = new G4LogicalBorderSurface(
                borderName, 
                physScintillator, 
                physWorld, 
                mirrorSurface
            );

            copyN++;
        }
    }

    G4VisAttributes *scintillatorVisAttributes = new G4VisAttributes(G4Colour(1., 1., 0., .5));
    scintillatorVisAttributes->SetForceSolid(true);
    logicScintillator->SetVisAttributes(scintillatorVisAttributes);

    G4VisAttributes *pmtVisAttributes = new G4VisAttributes(G4Colour(0., 0., 1., .5));
    pmtVisAttributes->SetForceSolid(true);
    logicPMT->SetVisAttributes(pmtVisAttributes);

    return physWorld;
}

void DetectorConstruction::ConstructSDandField()
{
    SensitiveDetector *sensDet = new SensitiveDetector("sensDet");

    logicPMT->SetSensitiveDetector(sensDet);

    //Get a pointer for the SDManager, that is the class that manages all the sensitive detectors in the simulation (so the function defined in SensitiveDetector can be called)
    G4SDManager::GetSDMpointer()->AddNewDetector(sensDet);
}

//To implemet the helical motion
void DetectorConstruction::UpdateHelicalPosition(G4double theta) {
    if (!physSource) {
        G4cout << "Errore: fPhysDetector non e' stato inizializzato!" << G4endl;
        return;
    }

    //Helix coordinate calculation
    G4double z = (fPitch / (2.0 * CLHEP::pi)) * theta*deg;
    G4ThreeVector newTranslation(0, 0, z);
    G4RotationMatrix* newRot = new G4RotationMatrix();
    newRot->rotateZ(theta*deg);

    //Application to the physical volume
    physSource->SetTranslation(newTranslation);
    physSource->SetRotation(newRot);

    //notification to the RunManager
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
    
    static G4int runIndex = 0; //Keeps track of the runs executed by the macro (static ensure that this line in sexecuted just the first time the function is called)
    
    //Name of the file containing "dat" and the run number (e.g., data_run_0.dat)
    std::string fileName = "/home/giorgiogrlj/geant4_code/Prova1/output_data/spiral_data" + std::to_string(runIndex) + ".dat";
    std::ofstream outFile(fileName);

    G4cout << "-> Creating spiral position file " << fileName << G4endl;
    
    if (outFile.is_open()) {
        outFile << "Theta_deg: " << theta << "\n";
        outFile << "PosZ: " << z << "\n";
        outFile.close();
        G4cout << "-> File saved successfully: " << fileName << G4endl;
    } else {
        G4cout << "-> Error: unable to create the file " << fileName << G4endl;
    }
    runIndex++;
}