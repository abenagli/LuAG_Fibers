#include "DetectorConstruction.hh"
#include "CreateTree.hh"
#include "MyMaterials.hh"

#include "G4RunManager.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4PVPlacement.hh"

#include "G4OpBoundaryProcess.hh"
#include "G4SDManager.hh"
#include "G4Color.hh"
#include "G4VisAttributes.hh"
#include "G4GeometryManager.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4SubtractionSolid.hh"
#include "TGraph.h"


using namespace std;

DetectorConstruction::DetectorConstruction()
{
	// set defaults for geometry and material properties
	//detectorMessenger = new DetectorMessenger(this);
  
	expHall_x = expHall_y = expHall_z = 100*cm;
	crystal_abslength    = -1;
	updated = true;
}

DetectorConstruction::~DetectorConstruction(){;}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
	readConfigFile("crystal.cfg");
	initializeMaterials();

	//
	// C O M M O N   V O L U M E S 
	//
	
	/*-------EXPERIMENTAL HALL-------*/

  	G4Box* expHall_box = new G4Box("World",expHall_x,expHall_y,expHall_z);
  	G4LogicalVolume* expHall_log = new G4LogicalVolume(expHall_box,Air,"World",0,0,0);
  	G4VPhysicalVolume* expHall_phys = new G4PVPlacement(0,G4ThreeVector(0,0,0),expHall_log,"World",0,false,0);

	G4LogicalVolume*  Fiber_log    = NULL;
  	G4LogicalVolume* surface_log = NULL;
  	G4LogicalVolume* opp_log  = NULL;
  	G4LogicalVolume* det_log  = NULL;

  	G4VPhysicalVolume* Fiber_phys   = NULL;  	
  	G4VPhysicalVolume* opp_phys_front = NULL;
  	G4VPhysicalVolume* opp_phys_rear = NULL;
  	G4VPhysicalVolume* surface_phys = NULL;

  	
	G4VPhysicalVolume* det_phys_front = NULL;
	G4VPhysicalVolume* det_phys_rear = NULL;
	G4VPhysicalVolume* win_phys_front = NULL;
	G4VPhysicalVolume* win_phys_rear = NULL;	
  		  

  //******************************* VISUALIZATION ATTRIBUTES ******************************* //
  //  
    
  //G4Colour  white   ()              ;  // white
  G4Colour  white   (1.0, 1.0, 1.0) ;  // white
  G4Colour  gray    (0.5, 0.5, 0.5) ;  // gray
  G4Colour  black   (0.0, 0.0, 0.0) ;  // black
  G4Colour  red     (1.0, 0.0, 0.0) ;  // red
  G4Colour  green   (0.0, 1.0, 0.0) ;  // green
  G4Colour  blue    (0.0, 0.0, 1.0) ;  // blue
  G4Colour  cyan    (0.0, 1.0, 1.0) ;  // cyan
  G4Colour  magenta (1.0, 0.0, 1.0) ;  // magenta 
  G4Colour  yellow  (1.0, 1.0, 0.0) ;  // yellow
  
  //*********************************************************************************************//  
  
	/*-------CRYSTAL-------*/
	if(CreateTree::Instance()->Crystal())
	{   	
	 G4Tubs* Fiber_box = new G4Tubs("Crystal_sol", 0, fiber_r, 0.5*fiber_length, 0,360);
          Fiber_log = new G4LogicalVolume(Fiber_box, ScMaterial, "Crystal_log",0,0,0);
          Fiber_phys = new G4PVPlacement(0, G4ThreeVector(0,0,0), Fiber_log, "Crystal", expHall_log, false,0);
          
          G4VisAttributes* VisAttCrystalFiber = new G4VisAttributes(green);
		  VisAttCrystalFiber->SetVisibility(true);
	 	  Fiber_log->SetVisAttributes(VisAttCrystalFiber);	 	  
	}
	
		  /*-------TOP AIR LAYERS/DETECTOR-------*/
	if(CreateTree::Instance()->Control())
	  {
        G4Tubs* opp_box = new G4Tubs("Air_opposite", 0, fiber_r, depth, 0, 360);	
	    	 opp_log  = new G4LogicalVolume(opp_box, Air, "Air_opposite_log", 0, 0, 0);	//change with grease layer when det_distance is set to 0 to couple with grease OptGrease
	    	 opp_phys_front = new G4PVPlacement(0,G4ThreeVector(0., 0., - 0.5*fiber_length-0.5*depth), opp_log, "Air_opposite_front", expHall_log, false,0);
   	    	 opp_phys_rear = new G4PVPlacement(0,G4ThreeVector(0., 0., + 0.5*fiber_length+0.5*depth), opp_log, "Air_opposite_rear", expHall_log, false,0);
	
	    G4Tubs* surface_box = new G4Tubs("surface_box", fiber_r, fiber_r + depth, 0.5*fiber_length, 0, 360);
	    	surface_log  = new G4LogicalVolume(surface_box, Air, "Air_side", 0, 0, 0);
			surface_phys = new G4PVPlacement(0, G4ThreeVector(0,0,0), surface_log, "Air_side", expHall_log, false,0);
			
			G4VisAttributes* VisAttSideAir = new G4VisAttributes(blue);
			VisAttSideAir->SetVisibility(true);
			surface_log->SetVisAttributes(VisAttSideAir);

			G4VisAttributes* VisAttOppAir = new G4VisAttributes(red);  
			VisAttOppAir->SetVisibility(true);
			opp_log->SetVisAttributes(VisAttOppAir);
	}
	  
	if(CreateTree::Instance()->Hits())
	{
	  G4Box* det_box = new G4Box("Detector", 0.5*det_d, 0.5*det_d, 0.5*det_d);
	   det_log  = new G4LogicalVolume(det_box, Silicon, "Detector", 0,0,0);
	   det_phys_front = new G4PVPlacement(0,G4ThreeVector(0, 0, - 0.5*(fiber_length + det_d) - det_distance - depth ), det_log, "Detector_front", expHall_log, false,0);
   	   det_phys_rear = new G4PVPlacement(0,G4ThreeVector(0, 0, 0.5*(fiber_length + det_d) + det_distance + depth), det_log, "Detector_rear", expHall_log, false,0);
   	   
   	  G4Tubs* win_tub = new G4Tubs("Window", 0.0, 0.5*win_diam, 0.5*win_depth, 0, 360);
	  G4LogicalVolume* win_log  = new G4LogicalVolume(win_tub, WiMaterial, "Window", 0,0,0);
	  G4RotationMatrix* rot_win = new G4RotationMatrix();
	  rot_win->rotateX(rot_ang*deg);
	  win_phys_front = new G4PVPlacement(rot_win,G4ThreeVector(0, 0, - 0.5*fiber_length - det_distance + 0.5*win_depth ),win_log,"Window_front",expHall_log,false,0);
	  win_phys_rear = new G4PVPlacement(rot_win,G4ThreeVector(0, 0, 0.5*fiber_length + det_distance - 0.5*win_depth),win_log,"Window_rear",expHall_log,false,0);
	}

	
	//
	  // 

    

/*
  	*/
  
  //G4VisAttributes* VisAttHole = new G4VisAttributes(blue);
  //VisAttHole->SetVisibility(true);
  //VisAttCrystal->SetColour(green);
//  Brass_hole_log->SetVisAttributes(VisAttHole);
	//
 	expHall_log->SetVisAttributes(G4VisAttributes::Invisible);
  
	//always return the physical World
  	return expHall_phys;
}

//
// Update geometry
// 
void DetectorConstruction::UpdateGeometry()
{
  	G4cout<<"Updating detector ..."<<G4endl;
  	// clean-up previous geometry
  	G4GeometryManager::GetInstance()->OpenGeometry();
  	G4PhysicalVolumeStore::GetInstance()->Clean();
  	G4LogicalVolumeStore::GetInstance()->Clean();
  	G4SolidStore::GetInstance()->Clean();
  	G4LogicalSkinSurface::CleanSurfaceTable();
  	G4LogicalBorderSurface::CleanSurfaceTable();

  	//define new one
  	G4RunManager::GetRunManager()->DefineWorldVolume(this->Construct());
  	G4RunManager::GetRunManager()->GeometryHasBeenModified();

  	updated=false;
}

G4bool DetectorConstruction::GetUpdated() const { return updated; }


void DetectorConstruction::initializeMaterials(){

	//
  	// Materials
  	//
  	Air        = MyMaterials::Air();
  	Water      = MyMaterials::Water();  
  	Vacuum     = MyMaterials::Vacuum();
  	Silicon    = MyMaterials::Silicon();
  	OptGrease  = MyMaterials::OpticalGrease();
  	Quartz	   = MyMaterials::Quartz();
  	ScMaterial = NULL;
	WiMaterial = NULL;

  	if(crystal_material==1)        ScMaterial = MyMaterials::LSO();
  	else if(crystal_material==2)   ScMaterial = MyMaterials::LYSO();
  	else if(crystal_material==3)   ScMaterial = MyMaterials::LUAGCE();  
  	else if(crystal_material==4)   ScMaterial = MyMaterials::LUAGPR();
  	else if(crystal_material==5)   ScMaterial = MyMaterials::PbWO();
	else if(crystal_material==6)   ScMaterial = MyMaterials::Air();
	else if(crystal_material==7)   ScMaterial = MyMaterials::Quartz();
  	else if(crystal_material>7 || crystal_material<=0) 
	{
    	  G4cerr<<"<DetectorConstruction::Construct>: Invalid material specifier "<<crystal_material<<G4endl;
   	  exit(0);
  	}
  	G4cout << "Sc. material: "<< ScMaterial << G4endl;
	
	if(win_material == 7) WiMaterial = MyMaterials::Quartz();
  	else if(win_material > 7 || win_material < 7) 
	{
    	  G4cerr << "<DetectorConstruction::Construct>: Invalid material specifier " << win_material << G4endl;
   	  exit(0);
  	}
  	G4cout << "Window material: " << win_material << G4endl;
	
	//
  	// modify default properties of the scintillator
  	//
  	if(crystal_lightyield>=0) 
	{
    	  ScMaterial->GetMaterialPropertiesTable()->RemoveConstProperty("SCINTILLATIONYIELD");
    	  ScMaterial->GetMaterialPropertiesTable()->AddConstProperty("SCINTILLATIONYIELD",crystal_lightyield/MeV);  
  	} 
	else 
	{
    	  CreateTree::Instance()->ScintillationYield = ScMaterial->GetMaterialPropertiesTable()->GetConstProperty("SCINTILLATIONYIELD");
  	}
  	if(crystal_risetime>=0) 
	{
    	  ScMaterial->GetMaterialPropertiesTable()->RemoveConstProperty("FASTSCINTILLATIONRISETIME");
    	  ScMaterial->GetMaterialPropertiesTable()->AddConstProperty("FASTSCINTILLATIONRISETIME",crystal_risetime/ns);  
  	} 
	else 
	{
    	  CreateTree::Instance()->RiseTime = ScMaterial->GetMaterialPropertiesTable()->GetConstProperty("FASTSCINTILLATIONRISETIME");
  	}
  	
  	if(crystal_abslength>=0) 
	{
    	  ScMaterial->GetMaterialPropertiesTable()->RemoveProperty("ABSLENGTH");
    	  ScMaterial->GetMaterialPropertiesTable()->AddConstProperty("ABSLENGTH",crystal_abslength);  

    	  gAbsorption = new TGraph ();	
    	  gAbsorption->SetTitle("gAbsorption");
    	  gAbsorption->SetName("gAbsorption");	  
		  gAbsorption ->SetPoint (0, 1, crystal_abslength);   	
		  gAbsorption->Write();
////    	  CreateTree::Instance()->AbsLength.push_back(crystal_abslength);
   //   	  CreateTree::Instance()->AbsLengthEn.push_back(0);
  	} 
	else 
	{
	
	    gAbsorption = new TGraph ();
	    gAbsorption->SetTitle("gAbsorption");
  	    gAbsorption->SetName("gAbsorption");
		for (unsigned int j = 0; j < ScMaterial->GetMaterialPropertiesTable()->GetProperty("ABSLENGTH")->GetVectorLength(); j++){
			double energy = ScMaterial->GetMaterialPropertiesTable()->GetProperty("ABSLENGTH")->Energy(j);
        	gAbsorption ->SetPoint (j, energy, ScMaterial->GetMaterialPropertiesTable()->GetProperty("ABSLENGTH")->Value(energy)  );
       	}
       	gAbsorption->Write();
        	
		
    	//  CreateTree::Instance()->AbsLength = ScMaterial->GetMaterialPropertiesTable()->GetProperty("ABSLENGTH");
    	  
  	}
}

void DetectorConstruction::readConfigFile(string configFileName)
{	
	ConfigFile config(configFileName);
	config.readInto(fiber_length,"length");
	config.readInto(fiber_r,"fiberr");

	config.readInto(depth,"depth");
	config.readInto(det_mat,"det_mat");
    config.readInto(det_d,"det_d");
	config.readInto(det_x,"det_x");
	config.readInto(det_y,"det_y");
	config.readInto(det_z,"det_z");

	config.readInto(rot_ang,"rot_ang");
	config.readInto(win_diam, "win_diam");
	config.readInto(win_depth, "win_depth");
	config.readInto(win_x, "win_x");
	config.readInto(win_y, "win_y");
	config.readInto(win_z, "win_z");
	config.readInto(win_material, "win_material");
		
	config.readInto(crystal_material,"scmaterial");
	config.readInto(crystal_risetime,"risetime");
	config.readInto(crystal_induced_abslength,"ind_abslength");
	config.readInto(crystal_abslength,"abslength");

	config.readInto(crystal_lightyield,"lightyield");
	config.readInto(det_distance,"det_distance");
}










