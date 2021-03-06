////////////////////////////////////////////////////////////////////////
// Class:       MCTruthT0Matching
// Module Type: producer
// File:        MCTruthT0Matching_module.cc
//
// Generated at Wed Mar 25 13:54:28 2015 by Thomas Warburton using artmod
// from cetpkgsupport v1_08_04.
////////////////////////////////////////////////////////////////////////

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Core/FindManyP.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Event.h" 
#include "art/Persistency/Common/Ptr.h" 
#include "art/Persistency/Common/PtrVector.h" 
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Services/Optional/TFileService.h" 
#include "art/Framework/Services/Optional/TFileDirectory.h"

#include "art/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <memory>
#include <iostream>
#include <map>
#include <iterator>

// LArSoft
#include "Geometry/Geometry.h"
#include "Geometry/PlaneGeo.h"
#include "Geometry/WireGeo.h"
#include "AnalysisBase/T0.h"
#include "RecoBase/Hit.h"
#include "RecoBase/SpacePoint.h"
#include "RecoBase/Track.h"
#include "Utilities/AssociationUtil.h"
#include "Utilities/LArProperties.h"
#include "Utilities/DetectorProperties.h"
#include "SimulationBase/MCParticle.h"
#include "SimulationBase/MCTruth.h"
#include "MCCheater/BackTracker.h"
#include "RawData/ExternalTrigger.h"
#include "SimpleTypesAndConstants/PhysicalConstants.h"
#include "AnalysisBase/ParticleID.h"

// ROOT
#include "TTree.h"
#include "TFile.h"

namespace lbne {
  class MCTruthT0Matching;
}

class lbne::MCTruthT0Matching : public art::EDProducer {
public:
  explicit MCTruthT0Matching(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  MCTruthT0Matching(MCTruthT0Matching const &) = delete;
  MCTruthT0Matching(MCTruthT0Matching &&) = delete;
  MCTruthT0Matching & operator = (MCTruthT0Matching const &) = delete; 
 MCTruthT0Matching & operator = (MCTruthT0Matching &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;

  // Selected optional functions.
  void beginJob() override;
  void reconfigure(fhicl::ParameterSet const & p) override;


private:
  
  // Params got from fcl file
  std::string fTrackModuleLabel;

  // Variable in TFS branches
  TTree* fTree;
  int    TrueTrackID  = 0;
  int    TrueTrackPDG = 0;
  int    TrueTriggerType = 0;
  double TrueTrackT0  = 0;  
};


lbne::MCTruthT0Matching::MCTruthT0Matching(fhicl::ParameterSet const & p)
// :
// Initialize member data here, if know don't want to reconfigure on the fly
{
  // Call appropriate produces<>() functions here.
  produces< std::vector<anab::T0>             >();
  produces< art::Assns<recob::Track, anab::T0> >();
  reconfigure(p);
}

void lbne::MCTruthT0Matching::reconfigure(fhicl::ParameterSet const & p)
{
  // Implementation of optional member function here.
  fTrackModuleLabel = (p.get< std::string > ("TrackModuleLabel") );
}

void lbne::MCTruthT0Matching::beginJob()
{
  // Implementation of optional member function here.
  art::ServiceHandle<art::TFileService> tfs;
  fTree = tfs->make<TTree>("MCTruthT0Matching","MCTruthT0");
  fTree->Branch("TrueTrackID" ,&TrueTrackID ,"TrueTrackID/I");
  fTree->Branch("TrueTrackT0" ,&TrueTrackT0 ,"TrueTrackT0/D");
  fTree->Branch("TrueTrackPDG",&TrueTrackPDG,"TrueTrackPDG/D");

}

void lbne::MCTruthT0Matching::produce(art::Event & evt)
{
  // Access art services...
  art::ServiceHandle<geo::Geometry> geom;
  art::ServiceHandle<util::LArProperties> larprop;
  art::ServiceHandle<util::DetectorProperties> detprop;
  art::ServiceHandle<cheat::BackTracker> bt;

  //TrackList handle
  art::Handle< std::vector<recob::Track> > trackListHandle;
  std::vector<art::Ptr<recob::Track> > tracklist;
  if (evt.getByLabel(fTrackModuleLabel,trackListHandle))
    art::fill_ptr_vector(tracklist, trackListHandle); 

  //Access tracks and hits
  art::FindManyP<recob::Hit>       fmht(trackListHandle, evt, fTrackModuleLabel);
  
  // Create anab::T0 objects and make association with recob::Track
  
  std::unique_ptr< std::vector<anab::T0> > T0col( new std::vector<anab::T0>);
  std::unique_ptr< art::Assns<recob::Track, anab::T0> > assn( new art::Assns<recob::Track, anab::T0>);

  size_t NTracks = tracklist.size();
  
  // Now to access MCTruth for each track... 
  for(size_t iTrk=0; iTrk < NTracks; ++iTrk) { 

    std::vector< art::Ptr<recob::Hit> > allHits = fmht.at(iTrk);
      
    std::map<int,double> trkide;
    for(size_t h = 0; h < allHits.size(); ++h){
      art::Ptr<recob::Hit> hit = allHits[h];
      std::vector<sim::IDE> ides;
      std::vector<sim::TrackIDE> TrackIDs = bt->HitToTrackID(hit);

      for(size_t e = 0; e < TrackIDs.size(); ++e){
	trkide[TrackIDs[e].trackID] += TrackIDs[e].energy;
      }
    }
    // Work out which IDE despoited the most charge in the hit if there was more than one.
    double maxe = -1;
    double tote = 0;
    for (std::map<int,double>::iterator ii = trkide.begin(); ii!=trkide.end(); ++ii){
      tote += ii->second;
      if ((ii->second)>maxe){
	maxe = ii->second;
	TrueTrackID = ii->first;
      }
    }

    // Now have trackID, so get PdG code and T0 etc.
    const simb::MCParticle *particle = bt->TrackIDToParticle(TrueTrackID);
    TrueTrackT0  = particle->T();
    TrueTrackPDG = particle->PdgCode();
    
    TrueTriggerType = 2; // Using MCTruth as trigger, so tigger type is 2.

    std::cout << "Filling T0col with " << TrueTrackT0 << " " << TrueTriggerType << " " << TrueTrackPDG << " " << (*T0col).size() << std::endl;
    
    T0col->push_back(anab::T0(TrueTrackT0,
			      TrueTriggerType,
			      TrueTrackPDG,
			      (*T0col).size()
			      ));
    util::CreateAssn(*this, evt, *T0col, tracklist[iTrk], *assn);    
    fTree -> Fill();  
  } // Loop over tracks   

  evt.put(std::move(T0col));
  evt.put(std::move(assn));

} // Produce

DEFINE_ART_MODULE(lbne::MCTruthT0Matching)
    
