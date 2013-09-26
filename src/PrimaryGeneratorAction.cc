// Martin Goettlich @ DESY
//

#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "CreateTree.hh"
#include "G4GeneralParticleSource.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  G4GeneralParticleSource *gps = new G4GeneralParticleSource();
  gun = gps;
  CreateTree::Instance()-> SourcePosition.push_back(gps->GetParticlePosition().x());
  CreateTree::Instance()-> SourcePosition.push_back(gps->GetParticlePosition().y());
  CreateTree::Instance()-> SourcePosition.push_back(gps->GetParticlePosition().z());
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
  gun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete gun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

