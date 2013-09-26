// Martin Goettlich
//

#ifndef StackingAction_H
#define StackingAction_H 1

#include "globals.hh"
#include "G4UserStackingAction.hh"

class StackingAction : public G4UserStackingAction
{
  public:
    StackingAction();
   ~StackingAction();

  public:
    G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);

};


#endif
