#ifndef LLRAnalysis_Utilities_PATTauPairSelector_h
#define LLRAnalysis_Utilities_PATTauPairSelector_h

#include "DataFormats/Common/interface/RefVector.h"

#include "CommonTools/UtilAlgos/interface/StringCutObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/SingleObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/ObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/SingleElementCollectionSelector.h"

#include "AnalysisDataFormats/TauAnalysis/interface/CompositePtrCandidateT1T2MEt.h"

#include <vector>

typedef SingleObjectSelector<
  std::vector<PATMuTauPair>,
  StringCutObjectSelector<PATMuTauPair> >  MuTauPairSelector;

typedef SingleObjectSelector<
  std::vector<PATMuTauPair>,
  StringCutObjectSelector<PATMuTauPair>,
  edm::RefVector<std::vector<PATMuTauPair>  > >  MuTauPairRefSelector;

typedef SingleObjectSelector<
  std::vector<PATElecTauPair>,
  StringCutObjectSelector<PATElecTauPair> >  ElecTauPairSelector;

typedef SingleObjectSelector<
  std::vector<PATElecTauPair>,
  StringCutObjectSelector<PATElecTauPair>,
  edm::RefVector<std::vector<PATElecTauPair> >  > ElecTauPairRefSelector;


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(ElecTauPairSelector);
DEFINE_FWK_MODULE(ElecTauPairRefSelector);

DEFINE_FWK_MODULE(MuTauPairSelector);
DEFINE_FWK_MODULE(MuTauPairRefSelector);

#endif
