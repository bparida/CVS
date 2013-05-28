#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/MakerMacros.h"


#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonCocktails.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"


/// Class declaration
class TunePMuonProducer : public edm::EDProducer {

public:
  explicit TunePMuonProducer(const edm::ParameterSet &);
  ~TunePMuonProducer(){};


private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup);
  virtual void beginJob() ;
  virtual void endJob() ;

  edm::InputTag muLabel_; 
  double muonMass;

};

/// Class definition
TunePMuonProducer::TunePMuonProducer(const edm::ParameterSet& iConfig) {

  muonMass = 0.1056583715; //GeV 
  if( iConfig.existsAs<std::string>("muLabel") )
     muLabel_=iConfig.getParameter<edm::InputTag>("muLabel");
  else muLabel_ = edm::InputTag("selectedPatMuons");

  produces < pat::MuonCollection >();

}

void TunePMuonProducer::beginJob(){}
                                                                                                         
void TunePMuonProducer::endJob(){}


void TunePMuonProducer::produce(edm::Event& iEvent,const edm::EventSetup& iSetup){
 
  //load muons
  edm::Handle<edm::View<pat::Muon> > muHandle;
  iEvent.getByLabel(muLabel_,muHandle);
  const edm::View<pat::Muon>& muons = *(muHandle.product());

  //prepare output collections
  std::auto_ptr<pat::MuonCollection> outMuons(new pat::MuonCollection());

  //clone muons, set pt to TuneP pt
  for(size_t muonNr=0;muonNr<muons.size();muonNr++){

    pat::Muon * recoMu = muons[muonNr].clone();

    if(recoMu->isTrackerMuon()) {
    
      // call to get the optimal muon track
      reco::TrackRef cktTrack = (muon::tevOptimized(recoMu->combinedMuon(),recoMu->innerTrack(),recoMu->tpfmsTrack(),recoMu->pickyTrack(),200, 17., 40., 0.25)).first ;
      double oPx = cktTrack->px();
      double oPy = cktTrack->py();
      double oPz = cktTrack->pz();
      double oE = sqrt((oPx*oPx) + (oPy*oPy) + (oPz*oPz) + (muonMass*muonMass));
      math::XYZTLorentzVector oP4(oPx,oPy,oPz,oE);

      std::cout<<" Old Px "<<recoMu->px()<<" Old py "<<recoMu->py()<<" Old pz "<<recoMu->pz()<<" Old E "<<recoMu->energy()<<std::endl;
      std::cout<<" New Px "<<oPx<<" New py "<<oPy<<" New pz "<<oPz<<" New E "<<oE<<std::endl;
      
      recoMu->setP4(oP4); /// This sets the new LorentzVector

      //recoMu->setTrack(cktTrack);
      
      /// We don't set the track anymore, because the TeV-refit tracks are NOT
      /// suitable for the muon ID cuts - those have to be separately done
      /// on the globalTrack and on the innerTrack. Instead, there's another
      /// producer which produces the sigmaPt/overPt, and we add it to the 
      /// pat::Muon as an userFloat.
    }
    
    outMuons->push_back(*(recoMu));
    delete recoMu;
  }

  /// Store the muons in the event
  iEvent.put(outMuons);
}


//define this as a plug-in
DEFINE_FWK_MODULE(TunePMuonProducer);
