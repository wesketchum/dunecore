////////////////////////////////////////////////////////////////////////
// Class:       CheckDPhaseGeometry
// Module Type: analyzer
// File:        CheckDPhaseGeometry_module.cc
//
// Generated at Fri Jan 15 14:32:57 2016 by Vyacheslav Galymov using artmod
// from cetpkgsupport v1_10_01.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "larcore/Geometry/Geometry.h"

#include "TFile.h"
#include "TCanvas.h"
#include "TBox.h"
#include "TH2F.h"
#include "TLine.h"

#include <iostream>
#include <iomanip>

class CheckDPhaseGeometry;

class CheckDPhaseGeometry : public art::EDAnalyzer {
public:
  explicit CheckDPhaseGeometry(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  CheckDPhaseGeometry(CheckDPhaseGeometry const &) = delete;
  CheckDPhaseGeometry(CheckDPhaseGeometry &&) = delete;
  CheckDPhaseGeometry & operator = (CheckDPhaseGeometry const &) = delete;
  CheckDPhaseGeometry & operator = (CheckDPhaseGeometry &&) = delete;

  // Required functions.
  void analyze(art::Event const & e) override;


private:

  // Declare member data here.
  const double dpwpitch = 0.3125; //cm

};


CheckDPhaseGeometry::CheckDPhaseGeometry(fhicl::ParameterSet const & p)
  :
  EDAnalyzer(p)  // ,
 // More initializers here.
{;}

void CheckDPhaseGeometry::analyze(art::Event const & e)
{
  //TCanvas *can = new TCanvas("c1","c1");
  //can->cd();
  std::vector<TBox*> TPCBox;
  std::vector<TLine*> Wires;

  double minx = 1e9;
  double maxx = -1e9;
  double miny = 1e9;
  double maxy = -1e9;
  double minz = 1e9;
  double maxz = -1e9;
  
  int nwires = 0;

  // get geometry
  art::ServiceHandle<geo::Geometry> geo;
  
  std::cout<<"Total number of TPC "<<geo->NTPC()<<std::endl;

  for (geo::TPCID const& tpcid: geo->IterateTPCIDs()) {
    size_t const t = tpcid.TPC;
    //if (t%2==0) continue;
    double local[3] = {0.,0.,0.};
    double world[3] = {0.,0.,0.};
    const geo::TPCGeo &tpc = geo->TPC(t);
    tpc.LocalToWorld(local,world);
    if (minx>world[0]-tpc.ActiveHalfWidth())
      minx = world[0]-tpc.ActiveHalfWidth();
    if (maxx<world[0]+tpc.ActiveHalfWidth())
      maxx = world[0]+tpc.ActiveHalfWidth();
    if (miny>world[1]-tpc.ActiveHalfHeight())
      miny = world[1]-tpc.ActiveHalfHeight();
    if (maxy<world[1]+tpc.ActiveHalfHeight())
      maxy = world[1]+tpc.ActiveHalfHeight();
    if (minz>world[2]-tpc.ActiveLength()/2.)
      minz = world[2]-tpc.ActiveLength()/2.;
    if (maxz<world[2]+tpc.ActiveLength()/2.)
      maxz = world[2]+tpc.ActiveLength()/2.;
    
    
    TPCBox.push_back(new TBox(world[2]-tpc.ActiveLength()/2.,
			      world[1]-tpc.ActiveHalfHeight(),
			      world[2]+tpc.ActiveLength()/2.,
			      world[1]+tpc.ActiveHalfHeight()));
    TPCBox.back()->SetFillStyle(0);
    TPCBox.back()->SetLineStyle(2);
    TPCBox.back()->SetLineWidth(2);
    TPCBox.back()->SetLineColor(16);

    std::cout<<"TPC "<<t<<" has found "<<geo->Nplanes(t)<<" planes"<<std::endl;
    std::cout<<"TPC coordinates : "<<world[0]<<" "<<world[1]<<" "<<world[2]<<std::endl;
    std::cout<<"Drift direction : ";
    
    if(tpc.DriftDirection() == geo::kPosX) std::cout<<"geo::kPosX"<<std::endl;
    else if(tpc.DriftDirection() == geo::kNegX) std::cout<<"geo::kNegX"<<std::endl;
    else std::cout<<" Uknown drift direction"<<std::endl;

    std::cout<<"Drift distance  : "<<tpc.DriftDistance()<<std::endl;
    
    // scan the planes
    for (size_t p = 0; p<geo->Nplanes(t);++p)
      {
	geo::PlaneID planeID(tpcid, p);
	const geo::PlaneGeo &vPlane = tpc.Plane( planeID );
	if( vPlane.View() == geo::kU )
	  std::cout<<"  View type geo::kU"<<std::endl;
	if( vPlane.View() == geo::kV )
	  std::cout<<"  View type geo::kV"<<std::endl;
	if( vPlane.View() == geo::kZ )
	  std::cout<<"  View type geo::kZ"<<std::endl;
	if( geo->SignalType(planeID) == geo::kCollection )
	  std::cout<<"  View is geo::kCollection"<<std::endl;
	if( geo->SignalType(planeID) == geo::kInduction )
	  std::cout<<"  View is geo::kInduction"<<std::endl;
	std::cout<<"  Number of wires : "<<vPlane.Nwires()<<std::endl;
	std::cout<<"  Wire pitch      : "<<vPlane.WirePitch()<<std::endl;
	std::cout<<"  Theta Z         : "<<vPlane.ThetaZ()<<std::endl;

	double prval = 0;
	for (size_t w = 0; w<geo->Nwires(p,t); ++w){
	  ++nwires;
	  //++nwires_tpc[t];
	  double xyz0[3];
	  double xyz1[3];
	  unsigned int c = 0;

	  if (true)
	    {
	      geo->WireEndPoints(c,t,p,w,xyz0,xyz1);
	      Wires.push_back(new TLine(xyz0[2],xyz0[1],xyz1[2],xyz1[1]));
	      
	      double pitch = 0;
	      if( p == 0) {pitch = xyz0[1] - prval; prval = xyz0[1];}
	      else if( p == 1) {pitch = xyz0[2] - prval; prval = xyz0[2];}
	      
	      if(w > 0 && fabs(pitch - dpwpitch) > 0.00001)
		{
		  std::cerr<<" Bad pitch : "<<t<<" "<<p<<" "<<w<<" "<<w-1<<" "<<pitch<<std::endl;
		  //<<std::setprecision(15)<<xyz0[0]<<" "<<xyz0[1]<<" "<<xyz0[2]<<std::endl;
		}
	    }
	  //std::cout<<t<<" "<<p<<" "<<w<<" "<<xyz0[0]<<" "<<xyz0[1]<<" "<<xyz0[2]<<std::endl;
	}
	std::cout<<std::endl;
      }
    //break;
  }

  //TH2F *frame = new TH2F("frame",";z (cm);y (cm)",3000,minz,maxz,3000,miny,maxy);
  //frame->SetStats(0);
  //frame->Draw();
  //for (auto box: TPCBox) box->Draw();
  //for (auto wire: Wires) wire->Draw();
  //can->Print("wires.pdf");
  //can->Print("wires.C");
  //std::cout<<"N wires = "<<nwires<<std::endl;
  std::cout<<"Total number of channel wires = "<<nwires<<std::endl;
  //for (int i = 0; i<8; ++i)
  //{
  //std::cout<<"TPC "<<i<<" has "<<nwires_tpc[i]<<" wires"<<std::endl;
  //}
}

DEFINE_ART_MODULE(CheckDPhaseGeometry)
