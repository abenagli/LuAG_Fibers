// Martin Goettlich @ DESY
//

#ifndef SteppingAction_H
#define SteppingACtion_H 1

#include "globals.hh"
#include "G4UserSteppingAction.hh"


//class SteppingMessenger;

class SteppingAction : public G4UserSteppingAction
{
public:

  SteppingAction();
  ~SteppingAction();
  virtual void UserSteppingAction(const G4Step*);
  

};

#endif
