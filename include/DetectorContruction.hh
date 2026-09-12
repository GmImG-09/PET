#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Ellipsoid.hh"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"

#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

//To better visualize the sensitive detector
#include "G4VisAttributes.hh"
#include "G4Color.hh"
#include "G4VisAttributes.hh"

#include "G4SDManager.hh"
#include "SensitiveDetector.hh"

#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "Randomize.hh"

#pragma once

#include <fstream>
#include <string>

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  DetectorConstruction();
  //It must to be virtual because the destructor is already defined in the base class G4VUserDetectorConstruction
  virtual ~DetectorConstruction();

  virtual G4VPhysicalVolume  *Construct();
  virtual void ConstructSDandField() override;

  void UpdateHelicalPosition(G4double theta);

private:
  G4LogicalVolume *logicPMT;
  G4LogicalVolume *logicScintillator;

  G4VPhysicalVolume* physSource;
  G4double fPitch;
};

#endif