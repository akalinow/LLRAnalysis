#include "LLRAnalysis/TauTauStudies/interface/ObjectRescalerProducer.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

typedef ObjectRescalerProducer<pat::Electron> ElectronRescalerProducer;
typedef ObjectRescalerProducer<pat::Muon>     MuonRescalerProducer;
typedef ObjectRescalerProducer<pat::Tau>      TauRescalerProducer;

template <>
math::XYZTLorentzVectorD ObjectRescalerProducer<pat::Tau>::CompScale(const pat::Tau& object, double shift)
{
  double scale = 1+shift;
  if((object.signalPFChargedHadrCands()).size()==1 && (object.signalPFGammaCands()).size()<=0) //1 prong
    {
      scale = sqrt( object.energy()*(1+shift)*object.energy()*(1+shift) - object.mass()*object.mass() )/object.p(); 
    }
  math::XYZTLorentzVectorD p4( object.px()*scale , object.py()*scale, object.pz()*scale, object.energy()*(1+shift) ); 

  //std::cout<<" Rescaling Pat::Tau of DecayMode "<<object.decayMode()<<" by "<<shift<<"% ==> scale = "<<scale<<std::endl;
  return p4; 
} 

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(ElectronRescalerProducer);
DEFINE_FWK_MODULE(MuonRescalerProducer);
DEFINE_FWK_MODULE(TauRescalerProducer);

