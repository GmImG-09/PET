#ifndef ACTIONINITIALIZATION_HH
#define ACTIONINITIALIZATION_HH

#include "G4VUserActionInitialization.hh"

#include "PrimaryGenerator.hh"
#include "RunAction.hh"

class ActionInitialization : public G4VUserActionInitialization
{
public:
    ActionInitialization();
    virtual ~ActionInitialization();
    
    //BuildForMaster() and Build() methods are used to define the actions for the master thread and worker threads, respectively.
    virtual void BuildForMaster() const;
    virtual void Build() const;
};

#endif