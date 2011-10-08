#include "TTree.h"
#include "TFile.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TGraphAsymmErrors.h"
#include "TGraphErrors.h"
#include "THStack.h"
#include "TBenchmark.h"
#include "TGraph.h"
#include "TVectorT.h"
#include "TMultiGraph.h"
#include "TBranch.h"
#include "TSystem.h"
#include "ratioEfficiency.C"
#include "RecoilCorrector.C"
#include "TLorentzVector.h"

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"

#include <vector>
#include <utility>
#include <map>
#include <algorithm>

#define SAVE   true
#define MINPt1 20.0 //20
#define MINPt2 20.0 //15

using namespace ROOT::Math;
using namespace std;

typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;


float* computeZeta(LV leg1, LV leg2, LV MEt){

  float* result = new float[2];

  float cos1 = leg1.Px()/leg1.Pt();
  float cos2 = leg2.Px()/leg2.Pt();
  float sen1 = leg1.Py()/leg1.Pt();
  float sen2 = leg2.Py()/leg2.Pt();

  float bisecX = cos1 + cos2;
  float bisecY = sen1 + sen2;

  float norm = TMath::Sqrt(bisecX*bisecX + bisecY*bisecY);

  if(norm>0.){
    bisecX /= norm;
    bisecY /= norm;
  }

  float pZetaVis = (leg1+leg2).Px()*bisecX + (leg1+leg2).Py()*bisecY;
  float pZeta    = (leg1+leg2+MEt).Px()*bisecX + (leg1+leg2+MEt).Py()*bisecY; 

  result[0] = pZetaVis;
  result[1] = pZeta;

  return result;
}


void makeTrees_ElecTauStream(int index = 4){

  gSystem->Load("ratioEfficiency_C.so");
  ratioEfficiency* ratioEff = new ratioEfficiency();

  gSystem->Load("RecoilCorrector_C.so");
  RecoilCorrector* recoilCorr = new RecoilCorrector();

  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;

  std::vector<std::string> samples;
  std::vector<float> crossSec;

  // samples & x-sections
  samples.push_back("Run2011-Elec");            crossSec.push_back( 0  );
  samples.push_back("DYJets-50-madgraph-PUS1"); crossSec.push_back( 3048  );
  //samples.push_back("DYToEE-20-PUS1");          crossSec.push_back( 1666  );
  //samples.push_back("DYToTauTau-20-PUS1");      crossSec.push_back( 1666  );
  samples.push_back("TTJets-madgraph-PUS1");    crossSec.push_back( 157.5 );
  samples.push_back("Top");                     crossSec.push_back( -1 );
  samples.push_back("SingleTop");               crossSec.push_back( -1 );
  samples.push_back("WJets-madgraph-PUS1");     crossSec.push_back( 31314.0);
  samples.push_back("ElectroWeak");             crossSec.push_back( -1);
  samples.push_back("DiBoson");                 crossSec.push_back( -1 );
  samples.push_back("QCD");                     crossSec.push_back( -1 );
  samples.push_back("G1Jet");                   crossSec.push_back( -1 );
  samples.push_back("VBFH115-powheg-PUS1");     crossSec.push_back( 0.1012);
  samples.push_back("GGFH115-powheg-PUS1");     crossSec.push_back( 7.65e-02 * 18.13 );
  //samples.push_back("VBFH125-powheg-PUS1");     crossSec.push_back( 6.37e-02 *  1.332 );
  //samples.push_back("GGFH120-powheg-PUS1");     crossSec.push_back( 7.11e-02 * 16.63  );
  //samples.push_back("VBFH135-powheg-PUS1");     crossSec.push_back( 4.52e-02 *  1.100 );
  //samples.push_back("GGFH130-powheg-PUS1");     crossSec.push_back( 5.49e-02 * 14.12 );


  // normalization Lumi
  Float_t Lumi=1000;

  TString sample(samples[index]);

  TString outName = "nTuple"+sample+"_Open_ElecTauStream.root";
  TFile *outFile = new TFile(outName,"RECREATE");
  TTree* outTreePtOrd     = new TTree("outTreePtOrd","tree jets pT-ord");
 
  float pt1,pt2,eta1,eta2,Deta,Mjj,Dphi;
  float pt1_v2,pt2_v2,eta1_v2,eta2_v2,Deta_v2,Mjj_v2,Dphi_v2;
  float diTauSVFitMass,diTauVisPt,diTauVisEta,diTauCAPt,diTauCAEta,diTauSVFitPt,diTauSVFitEta;
  float diTauVisMass;
  float ptL1,ptL2,etaL1,etaL2,leadTrackPt;
  float diTauCharge_,MtLeg1_,pZeta_,pZetaVis_,MEt,pZetaCorr_,pZetaVisCorr_,MEtCorr;
  float numPV_; 
  float sampleWeight,puWeight;
  float combRelIsoLeg1Raw,combRelIsoLeg1,combRelIsoLeg1Beta,combRelIsoLeg1DBeta,combRelIsoLeg1Rho;
  float combIsoLeg2;
  int tightestHPSWP_;
  float jetsBtagHE1,jetsBtagHE2;
  float ptVeto, chFracPV1, chFracPV2;
  float HLTx,HLTmatch,HLTweight;
  int isTauLegMatched_,elecFlag_;
  float event_,run_,lumi_;

  outTreePtOrd->Branch("pt1",  &pt1,"pt1/F");
  outTreePtOrd->Branch("pt2",  &pt2,"pt2/F");
  outTreePtOrd->Branch("eta1", &eta1,"eta1/F");
  outTreePtOrd->Branch("eta2", &eta2,"eta2/F");
  outTreePtOrd->Branch("Deta", &Deta,"Deta/F");
  outTreePtOrd->Branch("Mjj",  &Mjj,"Mjj/F");

  outTreePtOrd->Branch("pt1_v2",  &pt1_v2,"pt1_v2/F");
  outTreePtOrd->Branch("pt2_v2",  &pt2_v2,"pt2_v2/F");
  outTreePtOrd->Branch("eta1_v2", &eta1_v2,"eta1_v2/F");
  outTreePtOrd->Branch("eta2_v2", &eta2_v2,"eta2_v2/F");
  outTreePtOrd->Branch("Deta_v2", &Deta_v2,"Deta_v2/F");
  outTreePtOrd->Branch("Mjj_v2",  &Mjj_v2,"Mjj_v2/F");
 
  outTreePtOrd->Branch("ptVeto", &ptVeto,"ptVeto/F");
  outTreePtOrd->Branch("diTauVisMass",    &diTauVisMass,"diTauVisMass/F");
  outTreePtOrd->Branch("diTauVisPt",  &diTauVisPt,"diTauVisPt/F");
  outTreePtOrd->Branch("diTauVisEta",   &diTauVisEta,"diTauVisEta/F");
  outTreePtOrd->Branch("diTauSVFitPt",  &diTauSVFitPt,"diTauSVFitPt/F");
  outTreePtOrd->Branch("diTauSVFitMass",  &diTauSVFitMass,"diTauSVFitMass/F");
  outTreePtOrd->Branch("diTauSVFitEta", &diTauSVFitEta,"diTauSVFitEta/F");
  outTreePtOrd->Branch("etaL1", &etaL1,"etaL1/F");
  outTreePtOrd->Branch("etaL2", &etaL2,"etaL2/F");
  outTreePtOrd->Branch("ptL1",  &ptL1,"ptL1/F");
  outTreePtOrd->Branch("ptL2",  &ptL2,"ptL2/F");
  outTreePtOrd->Branch("Dphi", &Dphi,"Dphi/F");
  outTreePtOrd->Branch("diTauCharge",  &diTauCharge_,"diTauCharge/F");
  outTreePtOrd->Branch("MtLeg1",  &MtLeg1_,"MtLeg1/F");
  outTreePtOrd->Branch("pZeta",  &pZeta_,"pZeta/F");
  outTreePtOrd->Branch("pZetaVis",  &pZetaVis_,"pZetaVis/F");
  outTreePtOrd->Branch("MEt",  &MEt,"MEt/F");
  outTreePtOrd->Branch("pZetaCorr",  &pZetaCorr_,"pZetaCorr/F");
  outTreePtOrd->Branch("pZetaVisCorr",  &pZetaVisCorr_,"pZetaVisCorr/F");
  outTreePtOrd->Branch("MEtCorr",  &MEtCorr,"MEtCorr/F");
  outTreePtOrd->Branch("numPV",  &numPV_,"numPV/F");
  outTreePtOrd->Branch("sampleWeight",  &sampleWeight,"sampleWeight/F"); 
  outTreePtOrd->Branch("puWeight",  &puWeight,"puWeight/F");
  outTreePtOrd->Branch("chFracPV1",  &chFracPV1,"chFracPV1/F"); 
  outTreePtOrd->Branch("chFracPV2",  &chFracPV2,"chFracPV2/F"); 
  //outTreePtOrd->Branch("combRelIsoLeg1Raw",  &combRelIsoLeg1Raw,"combRelIsoLeg1Raw/F");
  //outTreePtOrd->Branch("combRelIsoLeg1",  &combRelIsoLeg1,"combRelIsoLeg1/F");
  //outTreePtOrd->Branch("combRelIsoLeg1Beta",  &combRelIsoLeg1Beta,"combRelIsoLeg1Beta/F");
  outTreePtOrd->Branch("combRelIsoLeg1DBeta",  &combRelIsoLeg1DBeta,"combRelIsoLeg1DBeta/F");
  outTreePtOrd->Branch("combIsoLeg2",  &combIsoLeg2,"combIsoLeg2/F");
  //outTreePtOrd->Branch("combRelIsoLeg1Rho",  &combRelIsoLeg1Rho, "combRelIsoLeg1Rho/F");
 
  outTreePtOrd->Branch("tightestHPSWP",  &tightestHPSWP_,"tightestHPSWP/I");
  outTreePtOrd->Branch("HLTx", &HLTx,"HLTx/F");
  outTreePtOrd->Branch("HLTmatch", &HLTmatch,"HLTmatch/F");
  outTreePtOrd->Branch("HLTweight", &HLTweight,"HLTweight/F");
  outTreePtOrd->Branch("isTauLegMatched", &isTauLegMatched_,"isTauLegMatched/I");
  outTreePtOrd->Branch("elecFlag", &elecFlag_,"elecFlag/I"); 
  outTreePtOrd->Branch("event", &event_,"event/F");
  outTreePtOrd->Branch("run", &run_,"run/F");
  outTreePtOrd->Branch("lumi", &lumi_,"lumi/F");
  outTreePtOrd->Branch("leadTrackPt",&leadTrackPt, "leadTrackPt/F");
 
  outTreePtOrd->Branch("jetsBtagHE1",  &jetsBtagHE1,"jetsBtagHE1/F");
  outTreePtOrd->Branch("jetsBtagHE2",  &jetsBtagHE2,"jetsBtagHE2/F");

 
  TString inName = "/data_CMS/cms/lbianchini/ElecTauStream2011_iter3//treeElecTauStream_"+sample+".root";
  TFile* file   = new TFile(inName,"READ");
  if(file->IsZombie()){
    cout << "No such file!" << endl;
    return;
  }
  TTree* currentTree = (TTree*)file->Get("elecTauStreamAnalyzer/tree");
  int nEntries = currentTree->GetEntries() ;

  TH1F* allEvents = (TH1F*)file->Get("allEventsFilter/totalEvents");
  float totalEvents = allEvents->GetBinContent(1);
  float scaleFactor = (crossSec[index] != 0) ? ((crossSec[index])*Lumi) / totalEvents : 1;

  currentTree->SetBranchStatus("diTauVisP4",1);
  currentTree->SetBranchStatus("diTauCAP4",0);
  currentTree->SetBranchStatus("diTauICAP4",0);
  currentTree->SetBranchStatus("diTauSVfit1P4",0);
  currentTree->SetBranchStatus("diTauSVfit2P4",0);
  currentTree->SetBranchStatus("diTauSVfit3P4",1);
  currentTree->SetBranchStatus("diTauLegsP4",1);
  currentTree->SetBranchStatus("genDiTauLegsP4",0);
  currentTree->SetBranchStatus("METP4",1);
  currentTree->SetBranchStatus("genMETP4",0);
  currentTree->SetBranchStatus("jetsP4",0);
  currentTree->SetBranchStatus("jetsIDP4",1);
  currentTree->SetBranchStatus("jetsIDL1OffsetP4",1);
  currentTree->SetBranchStatus("genJetsIDP4",0);
  currentTree->SetBranchStatus("jetsBtagHE",1);
  currentTree->SetBranchStatus("jetsBtagHP",0);
  currentTree->SetBranchStatus("jetMoments",0);
  currentTree->SetBranchStatus("jetsChEfraction",1);
  currentTree->SetBranchStatus("jetsChNfraction",0);
  currentTree->SetBranchStatus("sumEt",0);
  currentTree->SetBranchStatus("MtLeg1",  1);
  currentTree->SetBranchStatus("pZeta",   1);
  currentTree->SetBranchStatus("pZetaVis",1);
  currentTree->SetBranchStatus("chIsoLeg1v1",0);
  currentTree->SetBranchStatus("nhIsoLeg1v1",0);
  currentTree->SetBranchStatus("phIsoLeg1v1",0);
  currentTree->SetBranchStatus("chIsoPULeg1v1",0);
  currentTree->SetBranchStatus("nhIsoPULeg1v1",0);
  currentTree->SetBranchStatus("phIsoPULeg1v1",0);
  currentTree->SetBranchStatus("chIsoLeg1v2",1);
  currentTree->SetBranchStatus("nhIsoLeg1v2",1);
  currentTree->SetBranchStatus("phIsoLeg1v2",1);
  currentTree->SetBranchStatus("chIsoPULeg1v2",1);
  currentTree->SetBranchStatus("nhIsoPULeg1v2",1);
  currentTree->SetBranchStatus("phIsoPULeg1v2",1);
  currentTree->SetBranchStatus("rhoFastJet",1);
  currentTree->SetBranchStatus("chIsoLeg2",1);
  currentTree->SetBranchStatus("nhIsoLeg2",0);
  currentTree->SetBranchStatus("phIsoLeg2",1);
  currentTree->SetBranchStatus("dxy1",0);
  currentTree->SetBranchStatus("dxy2",0);
  currentTree->SetBranchStatus("dz1",0);
  currentTree->SetBranchStatus("dz2",0);
  currentTree->SetBranchStatus("run",1);
  currentTree->SetBranchStatus("event",1);
  currentTree->SetBranchStatus("lumi",1);
  currentTree->SetBranchStatus("mcPUweight",1);
  currentTree->SetBranchStatus("numPV",1);
  currentTree->SetBranchStatus("numOfDiTaus",0);
  currentTree->SetBranchStatus("decayMode",0);
  currentTree->SetBranchStatus("tightestHPSWP",1);
  currentTree->SetBranchStatus("visibleTauMass",0);
  currentTree->SetBranchStatus("isTauLegMatched",1);
  currentTree->SetBranchStatus("isElecLegMatched",0);
  currentTree->SetBranchStatus("elecFlag",1);
  currentTree->SetBranchStatus("diTauCharge",1);
  currentTree->SetBranchStatus("tauXTriggers",1);
  currentTree->SetBranchStatus("triggerBits",1);
  currentTree->SetBranchStatus("leadPFChargedHadrCandTrackPt",1);

  //std::vector<double>* jetsBtagHE;
 
  float chIsoLeg1,nhIsoLeg1,phIsoLeg1; 
  float chIsoPULeg1,nhIsoPULeg1,phIsoPULeg1; 
  float chIsoLeg2,phIsoLeg2; 
  int tightestHPSWP,elecFlag;  
  float  diTauCharge, MtLeg1,pZeta,pZetaVis,leadPFChargedHadrCandTrackPt; 
  float numPV, mcPUweight,rhoFastJet;
  float weight;
  int isTauLegMatched;
  float event,run,lumi;

  std::vector< LV >* jets        = new std::vector< LV >();
  currentTree->SetBranchAddress("jetsIDP4",   &jets);
  std::vector< LV >* jets_v2        = new std::vector< LV >();
  currentTree->SetBranchAddress("jetsIDL1OffsetP4",   &jets_v2);
  std::vector< LV >* diTauLegsP4 = new std::vector< LV >();
  currentTree->SetBranchAddress("diTauLegsP4",&diTauLegsP4);
  std::vector< LV >* diTauVisP4  =  new std::vector< LV >();
  currentTree->SetBranchAddress("diTauVisP4",&diTauVisP4);
  //std::vector< LV >* diTauICAP4 =  new std::vector< LV >();
  //currentTree->SetBranchAddress("diTauICAP4",&diTauICAP4);
  std::vector< LV >* diTauSVfit3P4 =  new std::vector< LV >();
  currentTree->SetBranchAddress("diTauSVfit3P4",&diTauSVfit3P4);
  std::vector< LV >* METP4 =  new std::vector< LV >();
  currentTree->SetBranchAddress("METP4",&METP4);

  std::vector< int >* tauXTriggers =  new std::vector< int >();
  currentTree->SetBranchAddress("tauXTriggers",&tauXTriggers);
  std::vector< int >* triggerBits =  new std::vector< int >();
  currentTree->SetBranchAddress("triggerBits",&triggerBits);
  std::vector< double >* jetsBtagHE =  new std::vector< double >();
  currentTree->SetBranchAddress("jetsBtagHE",&jetsBtagHE);

  std::vector< float >* jetsChNfraction =  new std::vector< float >();
  currentTree->SetBranchAddress("jetsChEfraction",&jetsChNfraction);

  currentTree->SetBranchAddress("chIsoLeg2",&chIsoLeg2);
  currentTree->SetBranchAddress("phIsoLeg2",&phIsoLeg2);
  currentTree->SetBranchAddress("chIsoLeg1v2",&chIsoLeg1);
  currentTree->SetBranchAddress("nhIsoLeg1v2",&nhIsoLeg1);
  currentTree->SetBranchAddress("phIsoLeg1v2",&phIsoLeg1);
  currentTree->SetBranchAddress("chIsoPULeg1v2",&chIsoPULeg1);
  currentTree->SetBranchAddress("nhIsoPULeg1v2",&nhIsoPULeg1);
  currentTree->SetBranchAddress("phIsoPULeg1v2",&phIsoPULeg1);
  currentTree->SetBranchAddress("rhoFastJet",&rhoFastJet);
  currentTree->SetBranchAddress("tightestHPSWP",&tightestHPSWP);
  currentTree->SetBranchAddress("diTauCharge",&diTauCharge);
  currentTree->SetBranchAddress("MtLeg1",&MtLeg1);
  currentTree->SetBranchAddress("pZeta",&pZeta);
  currentTree->SetBranchAddress("pZetaVis",&pZetaVis);
  currentTree->SetBranchAddress("numPV",&numPV);
  currentTree->SetBranchAddress("mcPUweight",&mcPUweight);
  currentTree->SetBranchAddress("event",&event);
  currentTree->SetBranchAddress("run",&run);
  currentTree->SetBranchAddress("lumi",&lumi);
  currentTree->SetBranchAddress("elecFlag",&elecFlag);
  currentTree->SetBranchAddress("isTauLegMatched",&isTauLegMatched);
  currentTree->SetBranchAddress("leadPFChargedHadrCandTrackPt",&leadPFChargedHadrCandTrackPt);

  if(crossSec[index]<0) currentTree->SetBranchAddress("weight",&weight);


  for (int n = 0; n < nEntries ; n++) {

    currentTree->GetEntry(n);
    if(n%1000==0) cout << n << endl;

    pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;
    pt1_v2=-99;pt2_v2=-99;eta1_v2=-99,eta2_v2=-99;Deta_v2=-99;Dphi_v2=-99;Mjj_v2=-99;

    diTauVisPt=-99;diTauVisEta=-99;diTauCAPt=-99;diTauCAEta=-99;
    diTauSVFitMass = -99;diTauSVFitPt=-99;diTauSVFitEta=-99;diTauVisMass=-99;
    ptL1=-99;ptL2=-99;etaL1=-99;etaL2=-99;leadTrackPt=-99;
    diTauCharge_=-99;MtLeg1_=-99;pZeta_=-99;pZetaVis_=-99;MEt=-99;
    numPV_=-99;sampleWeight=-99;puWeight=-99;
    combRelIsoLeg1DBeta=-99; combIsoLeg2 = -99;
    tightestHPSWP_=-99;ptVeto=-99;chFracPV1=-99; chFracPV2=-99;
    HLTx=-99;HLTmatch=-99;isTauLegMatched_=-99;elecFlag_=-99;
    event_=-99;run_=-99;lumi_=-99;
    jetsBtagHE1 = -99;
    jetsBtagHE2 = -99;

    //2 jet preslection
    if(jets->size()>1 && (*jets)[0].Pt()>MINPt1 && (*jets)[1].Pt()>MINPt2){

      pt1  = (*jets)[0].Pt();
      pt2  = (*jets)[1].Pt();
      eta1 = (*jets)[0].Eta();
      eta2 = (*jets)[1].Eta();
      Deta = abs(eta1-eta2);
      Dphi = abs((*jets)[0].Phi()-(*jets)[1].Phi()) > TMath::Pi() ? 
	-abs( (*jets)[0].Phi()-(*jets)[1].Phi() ) + 2*TMath::Pi()  :
	abs( (*jets)[0].Phi()-(*jets)[1].Phi() ) ;
      Mjj  = ((*jets)[0]+(*jets)[1]).M();

      pt1_v2  = (*jets_v2)[0].Pt();
      pt2_v2  = (*jets_v2)[1].Pt();
      eta1_v2 = (*jets_v2)[0].Eta();
      eta2_v2 = (*jets_v2)[1].Eta();
      Deta_v2 = abs(eta1-eta2);
      Dphi_v2 = abs((*jets_v2)[0].Phi()-(*jets_v2)[1].Phi()) > TMath::Pi() ? 
	-abs( (*jets_v2)[0].Phi()-(*jets_v2)[1].Phi() ) + 2*TMath::Pi()  :
	abs( (*jets_v2)[0].Phi()-(*jets_v2)[1].Phi() ) ;
      Mjj_v2  = ((*jets_v2)[0]+(*jets_v2)[1]).M();

      for(unsigned k=0; k < jets->size(); k++){
	if(k>1 && 
	   (  ((*jets)[k].Eta()>(*jets)[1].Eta()+0.5 &&  (*jets)[k].Eta()<(*jets)[0].Eta()-0.5) || 
	      ((*jets)[k].Eta()>(*jets)[0].Eta()+0.5 &&  (*jets)[k].Eta()<(*jets)[1].Eta()-0.5) ) && (*jets)[k].Pt()>ptVeto) ptVeto=(*jets)[k].Et();  
      }
      if(ptVeto>0) ptVeto = (*jets)[2].Pt();

      jetsBtagHE1 = (*jetsBtagHE)[0];
      jetsBtagHE2 = (*jetsBtagHE)[1];

      chFracPV1 = (*jetsChNfraction)[0]; 
      chFracPV2 = (*jetsChNfraction)[1]; 
    }


    diTauVisMass  = (*diTauVisP4)[0].M();
    diTauVisPt  = (*diTauVisP4)[0].Pt();
    diTauVisEta = (*diTauVisP4)[0].Eta();
    diTauSVFitPt  = (*diTauSVfit3P4)[0].Pt(); 
    diTauSVFitMass = (*diTauSVfit3P4)[0].M();
    diTauSVFitEta  = (*diTauSVfit3P4)[0].Eta() ;
    //diTauCAPt  = (*diTauICAP4)[0].Pt();
    //diTauCAEta = (*diTauICAP4)[0].Eta();
    
    ptL1  = (*diTauLegsP4)[0].Pt();
    ptL2  = (*diTauLegsP4)[1].Pt();
    etaL1 = (*diTauLegsP4)[0].Eta();
    etaL2 = (*diTauLegsP4)[1].Eta();
    diTauCharge_ = diTauCharge;
    float scalarSumPt = (*diTauLegsP4)[0].Pt() + (*METP4)[0].Pt();
    float vectorSumPt = ((*diTauLegsP4)[0] + (*METP4)[0]).Pt() ;
    MtLeg1_ = TMath::Sqrt( scalarSumPt*scalarSumPt - vectorSumPt*vectorSumPt ) ;
    //MtLeg1_ = MtLeg1;

    TLorentzVector corrMET_tmp;
    LV corrMET(1,0,0,1);
    double corrPt = (*METP4)[0].Et(); double corrPhi = (*METP4)[0].Phi();
    recoilCorr->Correct(corrPt,corrPhi, 10, TMath::Pi(), ((*diTauLegsP4)[0]+(*diTauLegsP4)[1]).Pt(),((*diTauLegsP4)[0]+(*diTauLegsP4)[1]).Phi()  );
    corrMET_tmp.SetPtEtaPhiM(corrPt,0,corrPhi,0);
    corrMET.SetPx(corrMET_tmp.Px());
    corrMET.SetPy(corrMET_tmp.Py());
    corrMET.SetPz(corrMET_tmp.Pz());
    corrMET.SetE(corrMET_tmp.E());



    pZetaVis_ = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[0];
    pZeta_    = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[1];
    pZetaVisCorr_ = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], corrMET))[0];
    pZetaCorr_    = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], corrMET))[1];

    MEt     = (*METP4)[0].Et();
    MEtCorr = corrMET.Et();
    tightestHPSWP_ = tightestHPSWP;
    numPV_ = numPV;
  
    //combRelIsoLeg1Raw = (chIsoLeg1+chIsoPULeg1+nhIsoLeg1+phIsoLeg1)/(*diTauLegsP4)[0].Pt();
    //combRelIsoLeg1 = (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diTauLegsP4)[0].Pt();

    //float PUtoPVratio = (chIsoLeg1+chIsoPULeg1)>0 ? chIsoPULeg1/(chIsoLeg1+chIsoPULeg1) : 0.0;
    //float scaled_nhIsoLeg1 = std::max( nhIsoLeg1*(1-PUtoPVratio), float(0.0));
    //float scaled_phIsoLeg1 = std::max( phIsoLeg1*(1-PUtoPVratio), float(0.0));
    //combRelIsoLeg1Beta = (chIsoLeg1+scaled_nhIsoLeg1+scaled_phIsoLeg1)/(*diTauLegsP4)[0].Pt();
    combRelIsoLeg1DBeta =(chIsoLeg1+ std::max( nhIsoLeg1+phIsoLeg1-0.5*0.5*(nhIsoPULeg1+phIsoPULeg1),double(0.0)))/(*diTauLegsP4)[0].Pt();
    combIsoLeg2 =  ( chIsoLeg2 + std::max( phIsoLeg2 - rhoFastJet*TMath::Pi()*0.5*0.5, 0.0) ) ;
    //cout << chIsoLeg2 << " --- " << phIsoLeg2 << " --- " << combIsoLeg2 << endl;
    //float EffArea = TMath::Pi()*0.4*0.4;
    //EffArea = 0.27/0.79; // from fit to MC: ( <iso>/#vtx ) / ( <rho>/#vtx )
    //combRelIsoLeg1Rho = std::max(((chIsoLeg1+chIsoPULeg1+nhIsoLeg1+phIsoLeg1) - rhoFastJet*EffArea),float(0.0))/(*diTauLegsP4)[0].Pt();
    
    sampleWeight = (scaleFactor>=0) ? scaleFactor : weight;
    puWeight = (std::string(sample.Data())).find("Run2011")!=string::npos ? 1.0 : mcPUweight;

    if((std::string(sample.Data())).find("Run2011")!=string::npos){
      
      if(run>=160404 && run<=161176)
	HLTx = 	float((*triggerBits)[0]); //HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau15_v1
      else if(run>=161216 && run<=163261)
	HLTx = 	float((*triggerBits)[1]); //HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau15_v2
      else if(run>=163269)
	HLTx = 	float((*triggerBits)[2]); //HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau15_v4
    
      HLTmatch = float((*tauXTriggers)[0]);
      
    } else{
      HLTx = 	float((*triggerBits)[0]);
      HLTmatch = float((*tauXTriggers)[0]);
    }
    
    HLTweight = (std::string(sample.Data())).find("Run2011")==string::npos ? 
      ratioEff->dataEfficiency((*diTauLegsP4)[0].Pt(), bool(fabs((*diTauLegsP4)[0].Eta())<1.5) ) : 1;

    isTauLegMatched_ = isTauLegMatched;
    elecFlag_ = elecFlag;
    event_=event;
    run_=run;
    lumi_=lumi;
    leadTrackPt = leadPFChargedHadrCandTrackPt;
    

    outTreePtOrd->Fill();
  }


 file->Close();

 if(SAVE) outFile->Write();
 outFile->Close();

 delete jets; delete jets_v2; delete diTauLegsP4; delete diTauVisP4; delete diTauSVfit3P4; delete tauXTriggers; delete triggerBits;
 delete METP4; delete jetsBtagHE; delete jetsChNfraction;
 delete ratioEff; delete recoilCorr;
}

void makeTrees_MuTauStream(int index = 4){


  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;

  std::vector<std::string> samples;
  std::vector<float> crossSec;

  // samples & x-sections
  samples.push_back("Run2011-Mu");            crossSec.push_back( 0  );
  samples.push_back("DYJets-Mu-50-madgraph-PUS1"); crossSec.push_back( 3048  );
  //samples.push_back("DYToEE-20-PUS1");          crossSec.push_back( 1666  );
  //samples.push_back("DYToTauTau-20-PUS1");      crossSec.push_back( 1666  );
  samples.push_back("TTJets-Mu-madgraph-PUS1");    crossSec.push_back( 157.5 );
  samples.push_back("Top");                     crossSec.push_back( -1 );
  samples.push_back("SingleTop");               crossSec.push_back( -1 );
  samples.push_back("WJets-Mu-madgraph-PUS1");     crossSec.push_back( 31314.0);
  samples.push_back("ElectroWeak");             crossSec.push_back( -1);
  samples.push_back("DiBoson");                 crossSec.push_back( -1 );
  samples.push_back("QCDmu-pythia-20-15-PUS1"); crossSec.push_back( 84679 );
  samples.push_back("VBFH115-Mu-powheg-PUS1");     crossSec.push_back( 0.1012);
  samples.push_back("GGFH115-Mu-powheg-PUS1");     crossSec.push_back( 7.65e-02 * 18.13 );
  //samples.push_back("VBFH125-powheg-PUS1");     crossSec.push_back( 6.37e-02 *  1.332 );
  //samples.push_back("GGFH120-powheg-PUS1");     crossSec.push_back( 7.11e-02 * 16.63  );
  //samples.push_back("VBFH135-powheg-PUS1");     crossSec.push_back( 4.52e-02 *  1.100 );
  //samples.push_back("GGFH130-powheg-PUS1");     crossSec.push_back( 5.49e-02 * 14.12 );


  // normalization Lumi
  Float_t Lumi=1000;

  TString sample(samples[index]);

  TString outName = "nTuple"+sample+"_Open_MuTauStream.root";
  TFile *outFile = new TFile(outName,"RECREATE");
  TTree* outTreePtOrd     = new TTree("outTreePtOrd","tree jets pT-ord");
 
  float pt1,pt2,eta1,eta2,Deta,Mjj,Dphi;
  float pt1_v2,pt2_v2,eta1_v2,eta2_v2,Deta_v2,Mjj_v2,Dphi_v2;
  float diTauSVFitMass,diTauVisPt,diTauVisEta,diTauCAPt,diTauCAEta,diTauSVFitPt,diTauSVFitEta;
  float diTauVisMass;
  float ptL1,ptL2,etaL1,etaL2,leadTrackPt;
  float diTauCharge_,MtLeg1_,pZeta_,pZetaVis_,MEt;
  float numPV_; 
  float sampleWeight,puWeight;
  float combRelIsoLeg1Raw,combRelIsoLeg1,combRelIsoLeg1Beta,combRelIsoLeg1DBeta,combRelIsoLeg1Rho;
  float combIsoLeg2;
  int tightestHPSWP_;
  float jetsBtagHE1,jetsBtagHE2;
  float ptVeto, chFracPV1, chFracPV2;
  float HLTmu,HLTx,HLTmatch;
  int isTauLegMatched_,muFlag_;
  float event_,run_,lumi_;

  outTreePtOrd->Branch("pt1",  &pt1,"pt1/F");
  outTreePtOrd->Branch("pt2",  &pt2,"pt2/F");
  outTreePtOrd->Branch("eta1", &eta1,"eta1/F");
  outTreePtOrd->Branch("eta2", &eta2,"eta2/F");
  outTreePtOrd->Branch("Deta", &Deta,"Deta/F");
  outTreePtOrd->Branch("Mjj",  &Mjj,"Mjj/F");

  outTreePtOrd->Branch("pt1_v2",  &pt1_v2,"pt1_v2/F");
  outTreePtOrd->Branch("pt2_v2",  &pt2_v2,"pt2_v2/F");
  outTreePtOrd->Branch("eta1_v2", &eta1_v2,"eta1_v2/F");
  outTreePtOrd->Branch("eta2_v2", &eta2_v2,"eta2_v2/F");
  outTreePtOrd->Branch("Deta_v2", &Deta_v2,"Deta_v2/F");
  outTreePtOrd->Branch("Mjj_v2",  &Mjj_v2,"Mjj_v2/F");
 
  outTreePtOrd->Branch("ptVeto", &ptVeto,"ptVeto/F");
  outTreePtOrd->Branch("diTauVisMass",    &diTauVisMass,"diTauVisMass/F");
  outTreePtOrd->Branch("diTauVisPt",  &diTauVisPt,"diTauVisPt/F");
  outTreePtOrd->Branch("diTauVisEta",   &diTauVisEta,"diTauVisEta/F");
  outTreePtOrd->Branch("diTauSVFitPt",  &diTauSVFitPt,"diTauSVFitPt/F");
  outTreePtOrd->Branch("diTauSVFitMass",  &diTauSVFitMass,"diTauSVFitMass/F");
  outTreePtOrd->Branch("diTauSVFitEta", &diTauSVFitEta,"diTauSVFitEta/F");
  outTreePtOrd->Branch("etaL1", &etaL1,"etaL1/F");
  outTreePtOrd->Branch("etaL2", &etaL2,"etaL2/F");
  outTreePtOrd->Branch("ptL1",  &ptL1,"ptL1/F");
  outTreePtOrd->Branch("ptL2",  &ptL2,"ptL2/F");
  outTreePtOrd->Branch("Dphi", &Dphi,"Dphi/F");
  outTreePtOrd->Branch("diTauCharge",  &diTauCharge_,"diTauCharge/F");
  outTreePtOrd->Branch("MtLeg1",  &MtLeg1_,"MtLeg1/F");
  outTreePtOrd->Branch("pZeta",  &pZeta_,"pZeta/F");
  outTreePtOrd->Branch("pZetaVis",  &pZetaVis_,"pZetaVis/F");
  outTreePtOrd->Branch("MEt",  &MEt,"MEt/F");
  outTreePtOrd->Branch("numPV",  &numPV_,"numPV/F");
  outTreePtOrd->Branch("sampleWeight",  &sampleWeight,"sampleWeight/F"); 
  outTreePtOrd->Branch("puWeight",  &puWeight,"puWeight/F");
  outTreePtOrd->Branch("chFracPV1",  &chFracPV1,"chFracPV1/F"); 
  outTreePtOrd->Branch("chFracPV2",  &chFracPV2,"chFracPV2/F"); 
  //outTreePtOrd->Branch("combRelIsoLeg1Raw",  &combRelIsoLeg1Raw,"combRelIsoLeg1Raw/F");
  //outTreePtOrd->Branch("combRelIsoLeg1",  &combRelIsoLeg1,"combRelIsoLeg1/F");
  //outTreePtOrd->Branch("combRelIsoLeg1Beta",  &combRelIsoLeg1Beta,"combRelIsoLeg1Beta/F");
  outTreePtOrd->Branch("combRelIsoLeg1DBeta",  &combRelIsoLeg1DBeta,"combRelIsoLeg1DBeta/F");
  outTreePtOrd->Branch("combIsoLeg2",  &combIsoLeg2,"combIsoLeg2/F");
  //outTreePtOrd->Branch("combRelIsoLeg1Rho",  &combRelIsoLeg1Rho, "combRelIsoLeg1Rho/F");
 
  outTreePtOrd->Branch("tightestHPSWP",  &tightestHPSWP_,"tightestHPSWP/I");
  outTreePtOrd->Branch("HLTmu", &HLTmu,"HLTmu/F");
  outTreePtOrd->Branch("HLTx", &HLTx,"HLTx/F");
  outTreePtOrd->Branch("HLTmatch", &HLTmatch,"HLTmatch/F");
  outTreePtOrd->Branch("isTauLegMatched", &isTauLegMatched_,"isTauLegMatched/I");
  outTreePtOrd->Branch("muFlag", &muFlag_,"muFlag/I"); 
  outTreePtOrd->Branch("event", &event_,"event/F");
  outTreePtOrd->Branch("run", &run_,"run/F");
  outTreePtOrd->Branch("lumi", &lumi_,"lumi/F");
  //outTreePtOrd->Branch("leadTrackPt",&leadTrackPt, "leadTrackPt/F");
 
  outTreePtOrd->Branch("jetsBtagHE1",  &jetsBtagHE1,"jetsBtagHE1/F");
  outTreePtOrd->Branch("jetsBtagHE2",  &jetsBtagHE2,"jetsBtagHE2/F");

 
  TString inName = "/data_CMS/cms/lbianchini/MuTauStream2011_iter3//treeMuTauStream_"+sample+".root";
  TFile* file   = new TFile(inName,"READ");
  if(file->IsZombie()){
    cout << "No such file!" << endl;
    return;
  }
  TTree* currentTree = (TTree*)file->Get("muTauStreamAnalyzer/tree");
  int nEntries = currentTree->GetEntries() ;

  TH1F* allEvents = (TH1F*)file->Get("allEventsFilter/totalEvents");
  float totalEvents = allEvents->GetBinContent(1);
  float scaleFactor = (crossSec[index] != 0) ? ((crossSec[index])*Lumi) / totalEvents : 1;

  currentTree->SetBranchStatus("diTauVisP4",1);
  currentTree->SetBranchStatus("diTauCAP4",0);
  currentTree->SetBranchStatus("diTauICAP4",0);
  currentTree->SetBranchStatus("diTauSVfit1P4",0);
  currentTree->SetBranchStatus("diTauSVfit2P4",0);
  currentTree->SetBranchStatus("diTauSVfit3P4",1);
  currentTree->SetBranchStatus("diTauLegsP4",1);
  currentTree->SetBranchStatus("genDiTauLegsP4",0);
  currentTree->SetBranchStatus("METP4",1);
  currentTree->SetBranchStatus("genMETP4",0);
  currentTree->SetBranchStatus("jetsP4",0);
  currentTree->SetBranchStatus("jetsIDP4",1);
  currentTree->SetBranchStatus("jetsIDL1OffsetP4",1);
  currentTree->SetBranchStatus("genJetsIDP4",0);
  currentTree->SetBranchStatus("jetsBtagHE",1);
  currentTree->SetBranchStatus("jetsBtagHP",0);
  currentTree->SetBranchStatus("jetMoments",0);
  currentTree->SetBranchStatus("jetsChEfraction",1);
  currentTree->SetBranchStatus("jetsChNfraction",0);
  currentTree->SetBranchStatus("sumEt",0);
  currentTree->SetBranchStatus("MtLeg1",  1);
  currentTree->SetBranchStatus("pZeta",   1);
  currentTree->SetBranchStatus("pZetaVis",1);
  currentTree->SetBranchStatus("chIsoLeg1v1",0);
  currentTree->SetBranchStatus("nhIsoLeg1v1",0);
  currentTree->SetBranchStatus("phIsoLeg1v1",0);
  currentTree->SetBranchStatus("chIsoPULeg1v1",0);
  currentTree->SetBranchStatus("nhIsoPULeg1v1",0);
  currentTree->SetBranchStatus("phIsoPULeg1v1",0);
  currentTree->SetBranchStatus("chIsoLeg1v2",1);
  currentTree->SetBranchStatus("nhIsoLeg1v2",1);
  currentTree->SetBranchStatus("phIsoLeg1v2",1);
  currentTree->SetBranchStatus("chIsoPULeg1v2",1);
  currentTree->SetBranchStatus("nhIsoPULeg1v2",1);
  currentTree->SetBranchStatus("phIsoPULeg1v2",1);
  currentTree->SetBranchStatus("rhoFastJet",1);
  currentTree->SetBranchStatus("chIsoLeg2",1);
  currentTree->SetBranchStatus("nhIsoLeg2",0);
  currentTree->SetBranchStatus("phIsoLeg2",1);
  currentTree->SetBranchStatus("dxy1",0);
  currentTree->SetBranchStatus("dxy2",0);
  currentTree->SetBranchStatus("dz1",0);
  currentTree->SetBranchStatus("dz2",0);
  currentTree->SetBranchStatus("run",1);
  currentTree->SetBranchStatus("event",1);
  currentTree->SetBranchStatus("lumi",1);
  currentTree->SetBranchStatus("mcPUweight",1);
  currentTree->SetBranchStatus("numPV",1);
  currentTree->SetBranchStatus("numOfDiTaus",0);
  currentTree->SetBranchStatus("decayMode",0);
  currentTree->SetBranchStatus("tightestHPSWP",1);
  currentTree->SetBranchStatus("visibleTauMass",0);
  currentTree->SetBranchStatus("isTauLegMatched",1);
  currentTree->SetBranchStatus("isMuLegMatched",0);
  currentTree->SetBranchStatus("muFlag",1);
  currentTree->SetBranchStatus("diTauCharge",1);
  currentTree->SetBranchStatus("tauXTriggers",1);
  currentTree->SetBranchStatus("triggerBits",1);
  currentTree->SetBranchStatus("leadPFChargedHadrCandTrackPt",0);

  //std::vector<double>* jetsBtagHE;
 
  float chIsoLeg2, phIsoLeg2;
  float chIsoLeg1,nhIsoLeg1,phIsoLeg1; 
  float chIsoPULeg1,nhIsoPULeg1,phIsoPULeg1; 
  int tightestHPSWP,muFlag;  
  float  diTauCharge, MtLeg1,pZeta,pZetaVis,leadPFChargedHadrCandTrackPt; 
  float numPV, mcPUweight,rhoFastJet;
  float weight;
  int isTauLegMatched;
  float event,run,lumi;

  std::vector< LV >* jets        = new std::vector< LV >();
  currentTree->SetBranchAddress("jetsIDP4",   &jets);
  std::vector< LV >* jets_v2        = new std::vector< LV >();
  currentTree->SetBranchAddress("jetsIDL1OffsetP4",   &jets_v2);
  std::vector< LV >* diTauLegsP4 = new std::vector< LV >();
  currentTree->SetBranchAddress("diTauLegsP4",&diTauLegsP4);
  std::vector< LV >* diTauVisP4  =  new std::vector< LV >();
  currentTree->SetBranchAddress("diTauVisP4",&diTauVisP4);
  //std::vector< LV >* diTauICAP4 =  new std::vector< LV >();
  //currentTree->SetBranchAddress("diTauICAP4",&diTauICAP4);
  std::vector< LV >* diTauSVfit3P4 =  new std::vector< LV >();
  currentTree->SetBranchAddress("diTauSVfit3P4",&diTauSVfit3P4);
  std::vector< LV >* METP4 =  new std::vector< LV >();
  currentTree->SetBranchAddress("METP4",&METP4);

  std::vector< int >* tauXTriggers =  new std::vector< int >();
  currentTree->SetBranchAddress("tauXTriggers",&tauXTriggers);
  std::vector< int >* triggerBits =  new std::vector< int >();
  currentTree->SetBranchAddress("triggerBits",&triggerBits);
  std::vector< double >* jetsBtagHE =  new std::vector< double >();
  currentTree->SetBranchAddress("jetsBtagHE",&jetsBtagHE);

  std::vector< float >* jetsChNfraction =  new std::vector< float >();
  currentTree->SetBranchAddress("jetsChEfraction",&jetsChNfraction);

  currentTree->SetBranchAddress("chIsoLeg2",&chIsoLeg2);
  currentTree->SetBranchAddress("phIsoLeg2",&phIsoLeg2);
  currentTree->SetBranchAddress("chIsoLeg1v2",&chIsoLeg1);
  currentTree->SetBranchAddress("nhIsoLeg1v2",&nhIsoLeg1);
  currentTree->SetBranchAddress("phIsoLeg1v2",&phIsoLeg1);
  currentTree->SetBranchAddress("chIsoPULeg1v2",&chIsoPULeg1);
  currentTree->SetBranchAddress("nhIsoPULeg1v2",&nhIsoPULeg1);
  currentTree->SetBranchAddress("phIsoPULeg1v2",&phIsoPULeg1);
  currentTree->SetBranchAddress("rhoFastJet",&rhoFastJet);
  currentTree->SetBranchAddress("tightestHPSWP",&tightestHPSWP);
  currentTree->SetBranchAddress("diTauCharge",&diTauCharge);
  currentTree->SetBranchAddress("MtLeg1",&MtLeg1);
  currentTree->SetBranchAddress("pZeta",&pZeta);
  currentTree->SetBranchAddress("pZetaVis",&pZetaVis);
  currentTree->SetBranchAddress("numPV",&numPV);
  currentTree->SetBranchAddress("mcPUweight",&mcPUweight);
  currentTree->SetBranchAddress("event",&event);
  currentTree->SetBranchAddress("run",&run);
  currentTree->SetBranchAddress("lumi",&lumi);
  currentTree->SetBranchAddress("muFlag",&muFlag);
  currentTree->SetBranchAddress("isTauLegMatched",&isTauLegMatched);
  //currentTree->SetBranchAddress("leadPFChargedHadrCandTrackPt",&leadPFChargedHadrCandTrackPt);

  if(crossSec[index]<0) currentTree->SetBranchAddress("weight",&weight);


  for (int n = 0; n < nEntries ; n++) {
    currentTree->GetEntry(n);
    if(n%1000==0) cout << n << endl;

    pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;
    pt1_v2=-99;pt2_v2=-99;eta1_v2=-99,eta2_v2=-99;Deta_v2=-99;Dphi_v2=-99;Mjj_v2=-99;

    diTauVisPt=-99;diTauVisEta=-99;diTauCAPt=-99;diTauCAEta=-99;
    diTauSVFitMass = -99;diTauSVFitPt=-99;diTauSVFitEta=-99;diTauVisMass=-99;
    ptL1=-99;ptL2=-99;etaL1=-99;etaL2=-99;leadTrackPt=-99;
    diTauCharge_=-99;MtLeg1_=-99;pZeta_=-99;pZetaVis_=-99;MEt=-99;
    numPV_=-99;sampleWeight=-99;puWeight=-99;
    combRelIsoLeg1DBeta=-99;combIsoLeg2=-99;
    tightestHPSWP_=-99;ptVeto=-99;chFracPV1=-99;chFracPV2=-99;
    HLTmu=-99; HLTx=-99;HLTmatch=-99;isTauLegMatched_=-99;muFlag_=-99;
    event_=-99;run_=-99;lumi_=-99;
    jetsBtagHE1 = -99;
    jetsBtagHE2 = -99;

    //2 jet preslection
    if(jets->size()>1 && (*jets)[0].Pt()>MINPt1 && (*jets)[1].Pt()>MINPt2){

      pt1  = (*jets)[0].Pt();
      pt2  = (*jets)[1].Pt();
      eta1 = (*jets)[0].Eta();
      eta2 = (*jets)[1].Eta();
      Deta = abs(eta1-eta2);
      Dphi = abs((*jets)[0].Phi()-(*jets)[1].Phi()) > TMath::Pi() ? 
	-abs( (*jets)[0].Phi()-(*jets)[1].Phi() ) + 2*TMath::Pi()  :
	abs( (*jets)[0].Phi()-(*jets)[1].Phi() ) ;
      Mjj  = ((*jets)[0]+(*jets)[1]).M();

      pt1_v2  = (*jets_v2)[0].Pt();
      pt2_v2  = (*jets_v2)[1].Pt();
      eta1_v2 = (*jets_v2)[0].Eta();
      eta2_v2 = (*jets_v2)[1].Eta();
      Deta_v2 = abs(eta1-eta2);
      Dphi_v2 = abs((*jets_v2)[0].Phi()-(*jets_v2)[1].Phi()) > TMath::Pi() ? 
	-abs( (*jets_v2)[0].Phi()-(*jets_v2)[1].Phi() ) + 2*TMath::Pi()  :
	abs( (*jets_v2)[0].Phi()-(*jets_v2)[1].Phi() ) ;
      Mjj_v2  = ((*jets_v2)[0]+(*jets_v2)[1]).M();

      for(unsigned k=0; k < jets->size(); k++){
	if(k>1 && 
	   (  ((*jets)[k].Eta()>(*jets)[1].Eta()+0.5 &&  (*jets)[k].Eta()<(*jets)[0].Eta()-0.5) || 
	      ((*jets)[k].Eta()>(*jets)[0].Eta()+0.5 &&  (*jets)[k].Eta()<(*jets)[1].Eta()-0.5) ) && (*jets)[k].Pt()>ptVeto) ptVeto=(*jets)[k].Pt();  
      }
      if(ptVeto>0) ptVeto = (*jets)[2].Pt();

      jetsBtagHE1 = (*jetsBtagHE)[0];
      jetsBtagHE2 = (*jetsBtagHE)[1];

      chFracPV1 = (*jetsChNfraction)[0]; 
      chFracPV2 = (*jetsChNfraction)[1]; 
    }


    diTauVisMass  = (*diTauVisP4)[0].M();
    diTauVisPt  = (*diTauVisP4)[0].Pt();
    diTauVisEta = (*diTauVisP4)[0].Eta();
    diTauSVFitPt  = (*diTauSVfit3P4)[0].Pt(); 
    diTauSVFitMass = (*diTauSVfit3P4)[0].M();
    diTauSVFitEta  = (*diTauSVfit3P4)[0].Eta() ;
    //diTauCAPt  = (*diTauICAP4)[0].Pt();
    //diTauCAEta = (*diTauICAP4)[0].Eta();
    
    ptL1  = (*diTauLegsP4)[0].Pt();
    ptL2  = (*diTauLegsP4)[1].Pt();
    etaL1 = (*diTauLegsP4)[0].Eta();
    etaL2 = (*diTauLegsP4)[1].Eta();
    diTauCharge_ = diTauCharge;
    float scalarSumPt = (*diTauLegsP4)[0].Pt() + (*METP4)[0].Pt();
    float vectorSumPt = ((*diTauLegsP4)[0] + (*METP4)[0]).Pt() ;
    MtLeg1_ = TMath::Sqrt( scalarSumPt*scalarSumPt - vectorSumPt*vectorSumPt ) ;
    //MtLeg1_ = MtLeg1;

    pZetaVis_ = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[0];
    pZeta_    = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[1];

    MEt = (*METP4)[0].Et();
    tightestHPSWP_ = tightestHPSWP;
    numPV_ = numPV;
  
    //combRelIsoLeg1Raw = (chIsoLeg1+chIsoPULeg1+nhIsoLeg1+phIsoLeg1)/(*diTauLegsP4)[0].Pt();
    //combRelIsoLeg1 = (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diTauLegsP4)[0].Pt();

    //float PUtoPVratio = (chIsoLeg1+chIsoPULeg1)>0 ? chIsoPULeg1/(chIsoLeg1+chIsoPULeg1) : 0.0;
    //float scaled_nhIsoLeg1 = std::max( nhIsoLeg1*(1-PUtoPVratio), float(0.0));
    //float scaled_phIsoLeg1 = std::max( phIsoLeg1*(1-PUtoPVratio), float(0.0));
    //combRelIsoLeg1Beta = (chIsoLeg1+scaled_nhIsoLeg1+scaled_phIsoLeg1)/(*diTauLegsP4)[0].Pt();
    combRelIsoLeg1DBeta =(chIsoLeg1+ std::max( nhIsoLeg1+phIsoLeg1-0.5*0.5*(nhIsoPULeg1+phIsoPULeg1),double(0.0)))/(*diTauLegsP4)[0].Pt();
    combIsoLeg2 =  ( chIsoLeg2 + std::max( phIsoLeg2 - rhoFastJet*TMath::Pi()*0.5*0.5, 0.0) ) ;
    //cout << chIsoLeg2 << " --- " << phIsoLeg2 << " --- " << combIsoLeg2 << endl;
    //float EffArea = TMath::Pi()*0.4*0.4;
    //EffArea = 0.27/0.79; // from fit to MC: ( <iso>/#vtx ) / ( <rho>/#vtx )
    //combRelIsoLeg1Rho = std::max(((chIsoLeg1+chIsoPULeg1+nhIsoLeg1+phIsoLeg1) - rhoFastJet*EffArea),float(0.0))/(*diTauLegsP4)[0].Pt();
    
    sampleWeight = (scaleFactor>=0) ? scaleFactor : weight;
    puWeight = (std::string(sample.Data())).find("Run2011")!=string::npos ? 1.0 : mcPUweight;

    if((std::string(sample.Data())).find("Run2011")!=string::npos){
      if(run>=160404 && run<=163261)
	HLTmu = float((*triggerBits)[8]);   //HLT_IsoMu12_v1
      else if (run>=163269)
	HLTmu = float((*triggerBits)[9]);   //HLT_IsoMu12_v2
      
      if(run>=160404 && run<=161176)
	HLTx = 	float((*triggerBits)[0]); //HLT_IsoMu12_LooseIsoPFTau10_v1
      else if(run>=161216 && run<=163261)
	HLTx = 	float((*triggerBits)[1]); //HLT_IsoMu12_LooseIsoPFTau10_v2
      else if(run>=163269)
	HLTx = 	float((*triggerBits)[2]); //HLT_IsoMu12_LooseIsoPFTau10_v4
      
      if(run<163269)
	HLTmatch = float((*tauXTriggers)[3]);
      else if(run>=163269)
	HLTmatch = float((*tauXTriggers)[0]);
    } else{
      HLTmu = float((*triggerBits)[3]);
      HLTx = 	float((*triggerBits)[1]);
      HLTmatch = float((*tauXTriggers)[1]);
    }
    
    isTauLegMatched_ = isTauLegMatched;
    muFlag_ = muFlag;
    event_=event;
    run_=run;
    lumi_=lumi;
    //leadTrackPt = leadPFChargedHadrCandTrackPt;
    

    outTreePtOrd->Fill();
  }


 file->Close();

 if(SAVE) outFile->Write();
 outFile->Close();

 delete jets; delete jets_v2; delete diTauLegsP4; delete diTauVisP4; delete diTauSVfit3P4; delete tauXTriggers; delete triggerBits;
 delete METP4; delete jetsBtagHE; delete jetsChNfraction;

}



void doAllSamplesElec(){
 
  for( unsigned int k = 0; k < 12 ; k++)  makeTrees_ElecTauStream(k);

  return;

}

void doAllSamplesMu(){
 
  for( unsigned int k = 0; k < 11 ; k++)  makeTrees_MuTauStream(k);

  return;

}
