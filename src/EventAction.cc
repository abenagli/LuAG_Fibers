// Martin Goettlich @ DESY
//
 
#include "EventAction.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "G4SDManager.hh"
#include "MyMaterials.hh"
#include "CreateTree.hh"


EventAction::EventAction()
{
	mppcCollID = -1;
}
 
EventAction::~EventAction()
{
}

void EventAction::BeginOfEventAction(const G4Event* evt)
{
  G4int evtNb = evt->GetEventID();
  if(evtNb%100 == 0 && evtNb!=0 ) 
  {
    G4cout << "---> Begin of Event: " << evtNb << G4endl;
  }

  // -------------------- INSTANCE RUN/EVENT IN TREE ---------------------- //
  Int_t run = CreateTree::Instance() -> Run;

  CreateTree::Instance() -> Clear();
  CreateTree::Instance() -> Run = run;
  CreateTree::Instance() -> Event = evt -> GetEventID();
  
}

void EventAction::EndOfEventAction(const G4Event* evt)
{
  if (CreateTree::Instance()->TotalMeV > 0.00005) CreateTree::Instance()->Fill();	//richiesta che ci sia abb energia da produrre almeno un photone
}

