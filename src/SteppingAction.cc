#include "SteppingAction.hh"

#include "G4SteppingManager.hh"
#include "G4SDManager.hh"
#include "G4EventManager.hh"
#include "G4ProcessManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4UnitsTable.hh"
#include <iostream>
#include <fstream>
#include "CreateTree.hh"

using namespace std;

SteppingAction::SteppingAction()
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step * theStep)
{
  
  G4StepPoint* thePrePoint = theStep->GetPreStepPoint();
  G4VPhysicalVolume* thePrePV = thePrePoint->GetPhysicalVolume();
  G4StepPoint* thePostPoint = theStep->GetPostStepPoint();
  G4VPhysicalVolume* thePostPV = thePostPoint->GetPhysicalVolume();
  
  if (thePostPV != NULL) {	// check post step volume is always in the mother volume to avoid crashing
    
  G4Track* theTrack = theStep->GetTrack();
  G4ParticleDefinition* particleType = theTrack->GetDefinition();
  
  //if (theStep->GetTotalEnergyDeposit()!=0) cout << "energy dep = " << theStep->GetTotalEnergyDeposit() << endl;
  //if (theStep->GetTotalEnergyDeposit() > 0.0006) {	// minimum energy deposition TRIGGER for storing events (corresponds to 10 photons produced with 15000 ph lightyield)

	//cout << " trigger passed! " << endl;
  
  // ------------------------ STORING PARTICLE AND PROCESSES ------------------------- //
  
  // Storing time, energy and position of optical photons absorbed
  if(particleType == G4OpticalPhoton::OpticalPhotonDefinition())
  {
    if(thePostPoint->GetProcessDefinedStep()->GetProcessName()=="OpAbsorption")
    {
      CreateTree::Instance()->NumOptPhotonsAbsorbed++;
    }
  }

  
  // ------------------------ Energy deposition info ------------------------- //
  if(CreateTree::Instance() -> Deposition())
  {
    if(theStep->GetTotalEnergyDeposit()!=0)
    { 
//      CreateTree::Instance()-> depositionProcess.push_back(thePostPoint-> GetProcessDefinedStep()-> GetProcessName());
    //  CreateTree::Instance()-> energyDeposited.push_back(theStep-> GetTotalEnergyDeposit());
      CreateTree::Instance()->TotalMeV += theStep-> GetTotalEnergyDeposit();
      
  //    CreateTree::Instance()-> depositionX.push_back(thePostPoint-> GetPosition().x());
//      CreateTree::Instance()-> depositionY.push_back(thePostPoint-> GetPosition().y());
//      CreateTree::Instance()-> depositionZ.push_back(thePostPoint-> GetPosition().z());
    } 
  }
  
  if(thePostPV && particleType==G4Gamma::GammaDefinition()  )
  { 
    if(CreateTree::Instance() -> Crystal() && CreateTree::Instance() -> Control())
    {
      if((thePrePV->GetName()=="Air_opposite_front" || thePrePV->GetName()=="Air_opposite_rear" || thePrePV->GetName()=="Air_side") &&  thePostPV->GetName()=="Crystal")
      {
        CreateTree::Instance()-> NumGammaEnter = 1;    
      }
    }
  }
  
  
  // ---------- INFO AT CONTROL VOLUME ---------- //
/*  if(CreateTree::Instance() -> Crystal() && CreateTree::Instance() -> Control())
  {
    if(particleType==G4OpticalPhoton::OpticalPhotonDefinition())
    {
    
//     if (thePostPV->GetName() != "World")      cout << " thePostPV = " << thePostPV->GetName() << endl;
         
      if ((thePrePV->GetName()=="Air_opposite_front" || thePrePV->GetName()=="Air_opposite_rear" || thePrePV->GetName()=="Air_side") && 
      (thePostPV->GetName()=="World" || thePostPV->GetName()=="Detector_rear" || thePostPV->GetName()=="Detector_front"))
      { 	
      
//      cout << "control volume at work ... " << endl;

     
        //CreateTree::Instance()-> Time.push_back(theStep-> GetTrack()-> GetGlobalTime());
        //CreateTree::Instance()-> Wglth_ex.push_back(theStep -> GetTrack() ->GetTotalEnergy());
	
        if (thePrePV->GetName()=="Air_opposite_front")  	CreateTree::Instance()-> Extraction.push_back(1);
        else if(thePrePV->GetName()=="Air_opposite_rear")	CreateTree::Instance()-> Extraction.push_back(2);
        else if(thePrePV->GetName()=="Air_side")			CreateTree::Instance()-> Extraction.push_back(3);
	
        if(theStep-> GetTrack()-> GetCreatorProcess() 
           && theStep-> GetTrack()-> GetCreatorProcess()-> GetProcessName()=="Cerenkov") 
        {
          CreateTree::Instance()->Parent.push_back(1);
        }     
        else if(theStep-> GetTrack()-> GetCreatorProcess() 
           && theStep-> GetTrack()-> GetCreatorProcess()-> GetProcessName()=="Scintillation")
        { 
          CreateTree::Instance()-> Parent.push_back(2);
        }
        else
        { 
          CreateTree::Instance()-> Parent.push_back(3);
	      cout << "boh" <<endl;
        }
      }
    }
  }*/
  
  
  
  // ---------- INFO AT DETECTOR ---------- //
  if( CreateTree::Instance() -> Hits())
  {
    if(particleType==G4OpticalPhoton::OpticalPhotonDefinition() )
    {  
    

     if ((thePrePV->GetName()=="World" || thePrePV->GetName()=="Air_opposite_front" || thePrePV->GetName()=="Air_opposite_rear") &&   
      (thePostPV->GetName()=="Detector_front" || thePostPV->GetName()=="Detector_rear"))
      {
//      cout << " detected photon ... " << endl; 	 
      
//     cout << " thePostPV = " << thePostPV->GetName() << endl;
	
		if (thePostPV->GetName()=="Detector_front") CreateTree::Instance()->Detector_type.push_back(0);
		else if (thePostPV->GetName()=="Detector_rear" ) CreateTree::Instance()->Detector_type.push_back(1);
 
        CreateTree::Instance()-> Time_det.push_back(theStep-> GetTrack()-> GetLocalTime());
        CreateTree::Instance()-> Wglth_ex_det.push_back(theStep -> GetTrack() ->GetTotalEnergy());
//        CreateTree::Instance()-> TrackLength.push_back(theStep -> GetTrack() ->GetTrackLength());
        CreateTree::Instance()-> TotalPhotonsSum ++;

        
		
        if(theStep-> GetTrack()-> GetCreatorProcess() && theStep-> GetTrack()-> GetCreatorProcess()-> GetProcessName()=="Cerenkov") 
        {
          CreateTree::Instance()->Parent_det.push_back(1);          
        }     
        
        else if(theStep-> GetTrack()-> GetCreatorProcess() && theStep-> GetTrack()-> GetCreatorProcess()-> GetProcessName()=="Scintillation")
        { 
          	CreateTree::Instance()-> Parent_det.push_back(2);         
        }
        
        else
        { 
       		CreateTree::Instance()-> Parent_det.push_back(3);
	  		cout << "Anomaly (production process)" <<endl;
        }
        
        
      }
    }  
  }

  //} //loop for minimum TRIGGER
 } // loop for physical volume inside world volume
  
}

