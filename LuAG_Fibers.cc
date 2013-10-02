#include <time.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <vector>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

#include "G4ios.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include <sstream>
#include "PrimaryGeneratorAction.hh"
//#include "MyPrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "StackingAction.hh"
#include "SteppingVerbose.hh"
#include "SteppingAction.hh"

#include "Randomize.hh"

#include "ConfigFile.hh"

#include "TRandom1.h"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#include "CreateTree.hh"
#include "TString.h"

#include "G4UIExecutive.hh"


#include "TCint.h"
using namespace std;



long int CreateSeed();



int main(int argc,char** argv)
{
  	// -----------------------------------------
	// -----------------------------------------
 	// PARSE CONFIGURATION FILE
 	// -----------------------------------------
	// -----------------------------------------

	gInterpreter -> GenerateDictionary("vector<float>","vector");
	
	if (argc != 3 && argc != 2)
	{
	  cout << "Syntax for exec: crystal <configuration file> <output file>" << endl; 
	  cout << "Syntax for viz:  crystal <configuration file>" << endl; 
	  return 0;
	  
	}
	  
	string file;
	string filename;
	TFile* outfile = NULL;
	  
  	if(argc == 3) 
	{
     	  cout << "Starting exec mode..." << endl; 
  	  file = argv[2];
	  filename = file + ".root";
  	  G4cout << "Writing data to file '" << filename << "' ..." << G4endl;
	  	
	  outfile = new TFile((TString)filename,"RECREATE");
	  outfile -> cd();
	  	  
	}
  	
  	if (argc == 2)
	{
	  cout<<"Starting viz mode..."<<endl; 
	}
	
	
  	cout<<"\n"<<endl;
  	cout<<"###########################################################"<<endl;  
  	cout<<"#                                                         #"<<endl;
  	cout<<"#  GEANT4 simulation of a scintillator read-out           #"<<endl;  
  	cout<<"#  by a photo-detector (crystal fibers).                  #"<<endl;  
  	cout<<"#                                                         #"<<endl;  
  	cout<<"#  Author: Nicolas Di Vara & Marco Lucchini, CERN, 2013   #"<<endl;
  	cout<<"#                                                         #"<<endl;  
  	cout<<"###########################################################"<<endl;
  	cout<<"\n\n"<<endl;
  	cout<<"=====>   C O N F I G U R A T I O N   <====\n"<<endl;

  	G4cout << "Configuration file: '" << argv[1] << "'" << G4endl;
  	ConfigFile config(argv[1]);

  	G4long myseed = config.read<long int>("seed");
  	if(myseed == -1) 
	{
     		G4cout << "Creating random seed..." << G4endl;   
     		myseed = CreateSeed();
  	}
  	G4cout << "Random seed : " << myseed << G4endl;
  
	Bool_t HITS = (Bool_t) config.read<int>("hits");
        G4cout << "External detector : " << HITS << G4endl;
	
	Bool_t WINDOW = (Bool_t) config.read<int>("window");
        G4cout << "Window : " << WINDOW << G4endl;
	
	Bool_t CRYSTAL = (Bool_t) config.read<int>("crystal");
        G4cout << "Crystal present : " << CRYSTAL << G4endl;
	
	Bool_t CONTROL = (Bool_t) config.read<int>("control");
        G4cout << "Control volumes : " << CONTROL << G4endl;
	
	Bool_t DEPOSITION = (Bool_t) config.read<int>("deposition");
        G4cout << "Deposition data : " << DEPOSITION << G4endl;
	
	Bool_t PRODUCTION = (Bool_t) config.read<int>("production");
        G4cout << "Production data : " << PRODUCTION << G4endl;
	
  	// Crystal parameters
  	G4double FiberLength = config.read<double>("length");
  	G4cout << "Fiber length [mm]: " << FiberLength << G4endl;
  	G4double FiberRadius = config.read<double>("fiberr");
  	G4cout << "Fiber radius [mm]: " << FiberRadius << G4endl; 

	// Control volumes and detector
	G4double depth = config.read<double>("depth");
  	G4cout << "Depth control volume [mm]: " << depth << G4endl;
  	G4double det_mat = config.read<double>("det_mat");
  	G4cout << "Detector material: " << det_mat << G4endl; 
  	G4double det_d = config.read<double>("det_d");
  	G4cout << "Detector thickness [mm]: " << det_d << G4endl;
	G4double det_x = config.read<double>("det_x");
  	G4cout << "Detector x position [mm]: " << det_x << G4endl;
	G4double det_y = config.read<double>("det_y");
  	G4cout << "Detector y position [mm]: " << det_y << G4endl;
	G4double det_z = config.read<double>("det_z");
  	G4cout << "Detector z position [mm]: " << det_z << G4endl;
	
	// Scintillator characteristics  
  	G4cout << "\nScintillator properties: " << G4endl;
  	G4long material = config.read<int>("scmaterial");
  	if     (material == 1)  G4cout << "Scintillator material: LSO"     << G4endl;
  	else if(material == 2)  G4cout << "Scintillator material: LYSO"    << G4endl;
  	else if(material == 3)  G4cout << "Scintillator material: LuAG:Ce" << G4endl; 
  	else if(material == 4)  G4cout << "Scintillator material: LuAG:Pr" << G4endl;
  	else if(material == 5)  G4cout << "Scintillator material: PbWO"    << G4endl;
  	else if(material == 6)  G4cout << "Scintillator material: Air"     << G4endl;
	else if(material == 7)  G4cout << "Scintillator material: Quartz"  << G4endl;

  	else if(material > 7 || material <= 0) 
	{
    		G4cerr << "<main>: Invalid material specifier: " << material << G4endl;
    		exit(0);
  	}
  	G4double lightyield = config.read<double>("lightyield");
  	if(lightyield >= 0) 
	{
    		G4cout << "Light yield [1/MeV]: " << lightyield << G4endl;
  	} 
	else 
	{
    		G4cout << "Light yield [1/MeV]: material default" << G4endl;
  	}
  	
  	G4double risetime = config.read<double>("risetime");
  	if(risetime >= 0) 
	{
    		G4cout << "Scintillation rise time [ns]: " << risetime << G4endl;
  	} 
	else 
	{
    		G4cout << "Scintillation rise time [ns]: material default" << G4endl;
  	}
  	
  	G4double abslength = config.read<double>("abslength");
  	if(abslength >= 0) 
	{
    		G4cout << "Abslength [m]: " << abslength << G4endl;
  	} 
	else 
	{
    		G4cout << "Abslength [m]: material default" << G4endl;
  	}
  	
  	G4double ind_abslength = config.read<double>("ind_abslength");
  	if(ind_abslength >= 0) 
	{
    		G4cout << "Induced Abslength [m]: " << ind_abslength << G4endl;
  	} 
	else 
	{
    		G4cout << "Induced Abslength [m]: 0" << G4endl;
  	}
  	
  	G4double det_distance = config.read<double>("det_distance");
	G4cout << "Detector Distance [mm]: " << det_distance << G4endl;
  	
  	

//	TGraph * gAbsorption = new TGraph ();
	CreateTree* mytree = new CreateTree("FiberOptics",HITS, WINDOW, CRYSTAL, CONTROL, DEPOSITION, PRODUCTION);
	CreateTree::Instance() -> ScintillationYield = lightyield;
	CreateTree::Instance() -> RiseTime = risetime;
	CreateTree::Instance() -> FiberLength = FiberLength;
	CreateTree::Instance() -> FiberRadius = FiberRadius;
	CreateTree::Instance() -> ScMaterial = material;
//	CreateTree::Instance() -> AbsLength = abslength;
	CreateTree::Instance() -> InducedAbsLength = ind_abslength;
	CreateTree::Instance() -> DetDistance = det_distance;
	
//	CreateTree::Instance() -> AbsLength = abslength;


	
	// -----------------------------------------
	// -----------------------------------------
 	// END CONFIGURATION ----> SIMULATION
 	// -----------------------------------------
	// -----------------------------------------


  	cout<<"\n\n"<<endl;
  	cout<<"=====>   S I M U L A T I O N   <===="<<endl;
  
	CLHEP::HepRandom::setTheSeed(myseed);
  
  	// User Verbose output class
  	G4VSteppingVerbose* verbosity = new SteppingVerbose;
  	G4VSteppingVerbose::SetInstance(verbosity);

  	// Run manager
  	G4RunManager* runManager = new G4RunManager;
	
	// UserInitialization classes - mandatory / Detector
	G4VUserDetectorConstruction* detector = new DetectorConstruction();
  	runManager->SetUserInitialization(detector);

	// UserInitialization classes - mandatory / Physics list
  	G4VUserPhysicsList* physics = new PhysicsList;
  	runManager->SetUserInitialization(physics);

	// UserAction classes
  	G4UserRunAction* run_action = new RunAction;
  	runManager->SetUserAction(run_action);

	// Gen action
	G4VUserPrimaryGeneratorAction* gen_action = new PrimaryGeneratorAction;
 	runManager->SetUserAction(gen_action);

  	// Event action
  	G4UserEventAction* event_action = new EventAction;
  	runManager->SetUserAction(event_action);
  
	// Stacking action
  	G4UserStackingAction* stacking_action = new StackingAction;
  	runManager->SetUserAction(stacking_action);
  	
	// Stepping action
  	SteppingAction* stepping_action = new SteppingAction;
  	runManager->SetUserAction(stepping_action);

  
	// Initialize G4 kernel
	//
	//runManager->Initialize();
  
	// Get the pointer to the User Interface manager
	//

	if (argc == 2)   // Define UI session for interactive mode
	{   
	  
	  // Initialize G4 kernel
	  //
	  runManager -> Initialize();
	  
	  #ifdef G4VIS_USE
	  G4VisManager* visManager = new G4VisExecutive;
	  visManager -> Initialize();
	  #endif
    
	  G4UImanager* UImanager = G4UImanager::GetUIpointer(); 
	  #ifdef G4UI_USE
	  G4UIExecutive * ui = new G4UIExecutive(argc,argv);
	  #ifdef G4VIS_USE
	  UImanager -> ApplyCommand("/control/execute vis.mac");     
	  #endif
	  ui -> SessionStart();
	  delete ui;
	  #endif 
   
	  #ifdef G4VIS_USE
	  delete visManager;
	  #endif  
	}
	else
	{
	  G4UImanager* UImanager = G4UImanager::GetUIpointer(); 
	  UImanager -> ApplyCommand("/control/execute gps.mac");
	} 

	// Job termination
	// Free the store: user actions, physics_list and detector_description are
	// 		   owned and deleted by the run manager, so they should not
	//                be deleted in the main() program !
  
	delete runManager;
	delete verbosity;

	if(argc == 3) 
	{
	  G4cout << "Writing tree to file " << filename << " ..." << G4endl;
	  
	  mytree -> GetTree() -> Write();
//	  gAbsorption->Write();
//	  outfile -> Write();
	  outfile -> Close();
	}
    
	return 0;

  
}


long int CreateSeed()
{
  	TRandom1 rangen;
  	long int s = time(0);
  	cout<<"Time : "<<s<<endl;
  	s+=getpid();
  	cout<<"PID  : "<<getpid()<<endl;
  
  	FILE * fp = fopen ("/proc/uptime", "r");
  	int uptime,upsecs = 0;
  	if (fp != NULL)
    	{
      		char buf[BUFSIZ];
      		int res;
      		char *b = fgets (buf, BUFSIZ, fp);
      		if (b == buf)
        	{
          		/* The following sscanf must use the C locale.  */
          		setlocale (LC_NUMERIC, "C");
          		res = sscanf (buf, "%i", &upsecs);
          		setlocale (LC_NUMERIC, "");
          		if (res == 1) uptime = (time_t) upsecs;
        	}
      		fclose (fp);
    	}

  	cout<<"Uptime: "<<upsecs<<endl;
  	s+=upsecs;
  
  	cout<<"Seed for srand: "<<s<<endl;
  	srand(s);
  	rangen.SetSeed(rand());
  	long int seed = round(1000000*rangen.Uniform());
  	return seed;
}

