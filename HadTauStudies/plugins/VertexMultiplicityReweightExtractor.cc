#include "LLRAnalysis/HadTauStudies/plugins/VertexMultiplicityReweightExtractor.h"

#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "TauAnalysis/CandidateTools/interface/generalAuxFunctions.h"

#include <TMath.h>

#include <vector>

//-------------------------------------------------------------------------------
// CV: code to computed genPileUp reweights
//     copied & pasted from SimGeneral/MixingModule/python/mix_E7TeV_FlatDist10_2011EarlyData_inTimeOnly_cfi.py
//     cf. https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupReweighting
//    (extended up to 36 pile-up interactions by just adding zeros at the end)
namespace 
{
  std::vector<float> gen_pileup_summer11mc()
  {
    const int gen_pu_max = 36;
    const double npu_probs[gen_pu_max] = {
      0.0698146584, 0.0698146584, 0.0698146584, 0.0698146584, 0.0698146584,
      0.0698146584, 0.0698146584, 0.0698146584, 0.0698146584, 0.0698146584, 0.0698146584 /* <-- 10*/,
      0.0630151648, 0.0526654164, 0.0402754482, 0.0292988928, 0.0194384503, 0.0122016783, 0.007207042, 0.004003637, 0.0020278322,
      0.0010739954, 0.0004595759, 0.0002229748, 0.0001028162, 4.58337152809607E-05, /* <-- 24 */
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000
    };
    std::vector<float> retVal(gen_pu_max);
    for ( int i = 0; i < gen_pu_max; ++i ) {
      retVal[i] = npu_probs[i];
    }
    return retVal;
  }
  std::vector<float> gen_pileup_fall11mc()
  {
    const int gen_pu_max = 50;
    const double npu_probs[gen_pu_max] = {
        0.003388501, 0.010357558, 0.024724258, 0.042348605, 0.058279812,
        0.068851751, 0.072914824, 0.071579609, 0.066811668, 0.060672356,
        0.054528356, 0.04919354,  0.044886042, 0.041341896, 0.0384679,
        0.035871463, 0.03341952,  0.030915649, 0.028395374, 0.025798107,
        0.023237445, 0.020602754, 0.0180688,   0.015559693, 0.013211063,
        0.010964293, 0.008920993, 0.007080504, 0.005499239, 0.004187022,
        0.003096474, 0.002237361, 0.001566428, 0.001074149, 0.000721755,
        0.000470838, 0.00030268,  0.000184665, 0.000112883, 6.74043E-05,
        3.82178E-05, 2.22847E-05, 1.20933E-05, 6.96173E-06, 3.4689E-06,
        1.96172E-06, 8.49283E-07, 5.02393E-07, 2.15311E-07, 9.56938E-08
    };
    std::vector<float> retVal(gen_pu_max);
    for ( int i = 0; i < gen_pu_max; ++i ) {
      retVal[i] = npu_probs[i];
    }
    return retVal;
  }
  std::vector<float> gen_pileup_summer12mc_S07()
  {
    const int gen_pu_max = 60;
    const double npu_probs[gen_pu_max] = {
      2.344E-05, 2.344E-05, 2.344E-05, 2.344E-05, 4.687E-04,
      4.687E-04, 7.032E-04, 9.414E-04, 1.234E-03, 1.603E-03,
      2.464E-03, 3.250E-03, 5.021E-03, 6.644E-03, 8.502E-03,
      1.121E-02, 1.518E-02, 2.033E-02, 2.608E-02, 3.171E-02,
      3.667E-02, 4.060E-02, 4.338E-02, 4.520E-02, 4.641E-02,
      4.735E-02, 4.816E-02, 4.881E-02, 4.917E-02, 4.909E-02,
      4.842E-02, 4.707E-02, 4.501E-02, 4.228E-02, 3.896E-02,
      3.521E-02, 3.118E-02, 2.702E-02, 2.287E-02, 1.885E-02,
      1.508E-02, 1.166E-02, 8.673E-03, 6.190E-03, 4.222E-03,
      2.746E-03, 1.698E-03, 9.971E-04, 5.549E-04, 2.924E-04,
      1.457E-04, 6.864E-05, 3.054E-05, 1.282E-05, 5.081E-06,
      1.898E-06, 6.688E-07, 2.221E-07, 6.947E-08, 2.047E-08
    };
    std::vector<float> retVal(gen_pu_max);
    for ( int i = 0; i < gen_pu_max; ++i ) {
      retVal[i] = npu_probs[i];
    }
    return retVal;
  }

  std::vector<float> gen_pileup_summer12mc_S10()
  {
    const int gen_pu_max = 60;
    const double npu_probs[gen_pu_max] = {
      2.560E-06, 5.239E-06, 1.420E-05, 5.005E-05, 1.001E-04,
      2.705E-04, 1.999E-03, 6.097E-03, 1.046E-02, 1.383E-02,
      1.685E-02, 2.055E-02, 2.572E-02, 3.262E-02, 4.121E-02,
      4.977E-02, 5.539E-02, 5.725E-02, 5.607E-02, 5.312E-02,
      5.008E-02, 4.763E-02, 4.558E-02, 4.363E-02, 4.159E-02,
      3.933E-02, 3.681E-02, 3.406E-02, 3.116E-02, 2.818E-02,
      2.519E-02, 2.226E-02, 1.946E-02, 1.682E-02, 1.437E-02,
      1.215E-02, 1.016E-02, 8.400E-03, 6.873E-03, 5.564E-03,
      4.457E-03, 3.533E-03, 2.772E-03, 2.154E-03, 1.656E-03,
      1.261E-03, 9.513E-04, 7.107E-04, 5.259E-04, 3.856E-04,
      2.801E-04, 2.017E-04, 1.439E-04, 1.017E-04, 7.126E-05,
      4.948E-05, 3.405E-05, 2.322E-05, 1.570E-05, 5.005E-06
    };
    std::vector<float> retVal(gen_pu_max);
    for ( int i = 0; i < gen_pu_max; ++i ) {
      retVal[i] = npu_probs[i];
    }
    return retVal;
  }

  int getBin(TAxis* axis, double x)
  {
    int bin = axis->FindFixBin(x);
    int nBins = axis->GetNbins();
    if ( bin < 1     ) bin = 1;
    if ( bin > nBins ) bin = nBins;
    return bin;
  }
}
//-------------------------------------------------------------------------------

VertexMultiplicityReweightExtractor::VertexMultiplicityReweightExtractor(const edm::ParameterSet& cfg)
  : src_(cfg.getParameter<edm::InputTag>("src")),
    inputFile_(0),
    genLumiReweight_(0),
    genLumiReweight3d_(0),
    recVtxMultiplicityReweight_(0),
    type_(kUndefined)
{
  edm::FileInPath inputFileName = cfg.getParameter<edm::FileInPath>("inputFileName");
  if ( !inputFileName.isLocal()) throw cms::Exception("VertexMultiplicityReweightExtractor") 
    << " Failed to find File = " << inputFileName << " !!\n";
  std::string lutName = cfg.getParameter<std::string>("lutName");
  std::string type_string = cfg.getParameter<std::string>("type");
  if      ( type_string == "gen"   ) type_ = kGenLevel;
  else if ( type_string == "gen3d" ) type_ = kGenLevel3d;
  else if ( type_string == "rec"   ) type_ = kRecLevel;
  else throw cms::Exception("VertexMultiplicityReweightExtractor") 
    << " Invalid Configuration Parameter 'type' = " << type_string << " !!\n";

  if ( type_ == kGenLevel || type_ == kGenLevel3d ) {
    // get MC production cycle
    std::string mcPeriod = cfg.getParameter<std::string>("mcPeriod");
    std::vector<float> gen_pileup_mc;
    if      ( mcPeriod.compare("Summer11") == 0 ) {
      gen_pileup_mc = gen_pileup_summer11mc();
      bxPrevious_ = -1;
      bxNext_ = +1;
    } else if ( mcPeriod.compare("Fall11")   == 0 ) {
      gen_pileup_mc = gen_pileup_fall11mc();
      bxPrevious_ = -1;
      bxNext_ = +1;
    } else if ( mcPeriod.compare("Summer12_S07") == 0 ) {
      gen_pileup_mc = gen_pileup_summer12mc_S07();
      bxPrevious_ = -1;
      bxNext_ = +1;
    } else if ( mcPeriod.compare("Summer12_S10") == 0 ) {
      gen_pileup_mc = gen_pileup_summer12mc_S10();
      bxPrevious_ = -1;
      bxNext_ = +1;
    } else throw cms::Exception("VertexMultiplicityReweightExtractor")
	<< " Configuration parameter 'mcPeriod' must be 'Summer11', 'Fall11', 'Summer12_S07' or 'Summer12_S10'.\n"; 
    //std::cout << " gen_pileup_mc(" << mcPeriod << ") = " << format_vfloat(gen_pileup_mc) << std::endl;

    inputFile_ = new TFile(inputFileName.fullPath().data());
    puDist_data_ = dynamic_cast<TH1*>(inputFile_->Get(lutName.data()));
    if ( !puDist_data_ ) 
      throw cms::Exception("VertexMultiplicityReweightExtractor") 
	<< " Failed to load LUT = " << lutName.data() << " from file = " << inputFileName.fullPath().data() << " !!\n";
    
    TH1* puDist_mc = new TH1D("MC_distr", "MC_distr", gen_pileup_mc.size(), 0., gen_pileup_mc.size());
    int numBins = puDist_mc->GetNbinsX();
    for ( int iBin = 1; iBin <= numBins; ++iBin ) {
      double binCenter = puDist_mc->GetBinCenter(iBin);
      int idx = TMath::FloorNint(binCenter);
      double binContent = ( idx >= 0 && idx < (int)gen_pileup_mc.size() ) ?
	gen_pileup_mc[idx] : 0.;
      puDist_mc->SetBinContent(iBin, binContent);
      puDist_mc->SetBinError(iBin, 0.);
    }
    std::string puFileName_mc = "puDist_mc.root";
    TFile* puFile_mc = new TFile(puFileName_mc.data(), "RECREATE");
    puDist_mc->Write();
    delete puFile_mc;
    
    if ( type_ == kGenLevel ) {
      genLumiReweight_ = new edm::LumiReWeighting(puFileName_mc.data(), inputFileName.fullPath().data(), "MC_distr", lutName.data());
    } else if ( type_ == kGenLevel3d ) {
      std::string tmpStr("MC_distr");
      genLumiReweight3d_ = new edm::Lumi3DReWeighting(puFileName_mc.data(), inputFileName.fullPath().data(), "MC_distr", lutName.data(), "");
      // CV: For 2011 data use pp inelastic cross-section of 73.5mb measured by TOTEM
      //     instead of CMS measurement of 68mb (default in Lumi3DReWeighting).
      //     For 2012 data use pp inelastic cross-section measured by CMS
      //     scaled by ratio 1.02 of min. bias cross-sections @ 8 TeV/7 TeV, computed by PYTHIA,
      //     cf. https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/1692/1/1.html
      //
      if ( mcPeriod.find("11") != std::string::npos ) genLumiReweight3d_->weight3D_init(73.5/68.);
      else genLumiReweight3d_->weight3D_init(69.4/69.4);
    }
  } else {
    inputFile_ = new TFile(inputFileName.fullPath().data());
    recVtxMultiplicityReweight_ = dynamic_cast<TH1*>(inputFile_->Get(lutName.data()));
    if ( !recVtxMultiplicityReweight_ ) 
      throw cms::Exception("VertexMultiplicityReweightExtractor") 
	<< " Failed to load LUT = " << lutName.data() << " from file = " << inputFileName.fullPath().data() << " !!\n";
  }
}

VertexMultiplicityReweightExtractor::~VertexMultiplicityReweightExtractor() 
{
  delete inputFile_;
  delete genLumiReweight_;
  delete genLumiReweight3d_;
  delete recVtxMultiplicityReweight_;
}

double VertexMultiplicityReweightExtractor::operator()(const edm::Event& evt) const
{
  double weight = 1.;
  
  if ( type_ == kGenLevel || type_ == kGenLevel3d ) {
    typedef std::vector<PileupSummaryInfo> PileupSummaryInfoCollection;
    edm::Handle<PileupSummaryInfoCollection> genPileUpInfos;
    evt.getByLabel(src_, genPileUpInfos);

    int numPileUp_bxPrevious = -1;
    int numPileUp_inTime     = -1;
    int numPileUp_bxNext     = -1;
    for ( PileupSummaryInfoCollection::const_iterator genPileUpInfo = genPileUpInfos->begin();
	  genPileUpInfo != genPileUpInfos->end(); ++genPileUpInfo ) {
      // CV: in-time PU is stored in getBunchCrossing = 0, 
      //    cf. https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupInformation
      int bx = genPileUpInfo->getBunchCrossing();
      //std::cout << "bx = " << bx << ": numPileUpInteractions = " << genPileUpInfo->getPU_NumInteractions() << " (true = " << genPileUpInfo->getTrueNumInteractions() << ")" << std::endl;
      int numPileUp = genPileUpInfo->getPU_NumInteractions();
      if      ( bx == bxPrevious_ ) numPileUp_bxPrevious = numPileUp;
      else if ( bx ==  0          ) numPileUp_inTime     = numPileUp;
      else if ( bx == bxNext_     ) numPileUp_bxNext     = numPileUp;
    }    
    if ( numPileUp_bxPrevious == -1 || numPileUp_inTime == -1 || numPileUp_bxNext == -1 ) 
      throw cms::Exception("VertexMultiplicityReweightExtractor") 
	<< " Failed to decode in-time and/or out-of-time Pile-up information stored in PileupSummaryInfo object" 
	<< " (numPileUp = {" << numPileUp_bxPrevious << ", " << numPileUp_inTime << ", " << numPileUp_bxNext << "}) !!\n";
    if      ( type_ == kGenLevel   ) weight = genLumiReweight_->weight(numPileUp_inTime);
    else if ( type_ == kGenLevel3d ) weight = genLumiReweight3d_->weight3D(numPileUp_bxPrevious, numPileUp_inTime, numPileUp_bxNext);
    else assert(0);
    //std::cout << " numPileUp = {" << numPileUp_bxPrevious << ", " << numPileUp_inTime << ", " << numPileUp_bxNext << "}:" 
    //	        << " weight = " << weight << std::endl;
  } else if ( type_ == kRecLevel ) {
    edm::Handle<reco::VertexCollection> vertices;
    evt.getByLabel(src_, vertices);
    int binIdx = getBin(recVtxMultiplicityReweight_->GetXaxis(), vertices->size());
    weight = recVtxMultiplicityReweight_->GetBinContent(binIdx);
    //std::cout << " numVertices = " << vertices->size() << ":" 
    //	        << " weight = " << weight << std::endl;
  }

  return weight;
}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_EDM_PLUGIN(ObjValExtractorPluginFactory, VertexMultiplicityReweightExtractor, "VertexMultiplicityReweightExtractor");

