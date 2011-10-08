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
#include "ratioEfficiencyElec.C"
#include "ratioEfficiencyTau.C"
#include "RecoilCorrector.C"
#include "TLorentzVector.h"
#include "TRandom3.h"

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


void makeTrees_ElecTauStream(int index = 4, bool doExlusive = false){

  gSystem->Load("ratioEfficiencyElec_C.so");
  ratioEfficiencyElec* ratioEffElec = new ratioEfficiencyElec();

  gSystem->Load("ratioEfficiencyTau_C.so");
  ratioEfficiencyTau* ratioEffTau = new ratioEfficiencyTau();

  gSystem->Load("RecoilCorrector_C.so");
  RecoilCorrector* recoilCorr = new RecoilCorrector();

  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;

  TFile *fPU = new TFile("pileUp_EPS2011.root","READ");
  TH1F* hPU = (TH1F*)fPU->Get("hWOneBX");

  TRandom3* tRandom = new TRandom3();

  std::vector<std::string> samples;
  std::vector<float> crossSec;
  std::vector<int> readEvents;

  // samples & x-sections
  samples.push_back("Run2011-Elec_All");            crossSec.push_back( 0  );                              readEvents.push_back( 0  );
  samples.push_back("DYJets-50-madgraph-PUS4_run"); crossSec.push_back( 3048           * 0.0206749225649 );readEvents.push_back(550447);
  samples.push_back("TTJets-madgraph-PUS4_run");    crossSec.push_back( 157.5          * 0.121349924468);  readEvents.push_back(219845);
  
  samples.push_back("WJets-madgraph-PUS4_run");     crossSec.push_back( 31314.0        * 0.00813873);      readEvents.push_back(329597);

  samples.push_back("T-t-PUS4_run");                crossSec.push_back( 42.6  *  0.03529121);              readEvents.push_back(14726);
  samples.push_back("Tbar-t-PUS4_run");             crossSec.push_back( 22.0  *  0.0371653);               readEvents.push_back(69307);
  samples.push_back("WZIncl-pythia-PUS4_run");      crossSec.push_back( 18.2  *  0.02982);                 readEvents.push_back(1491);
  samples.push_back("ZZ-pythia-PUS4_run");          crossSec.push_back( 0.238 *  0.03224);                 readEvents.push_back(1612);
  samples.push_back("WW-pythia-PUS4_run");          crossSec.push_back( 4.51  *  0.05138);                 readEvents.push_back(2569);
 
  samples.push_back("VBFH120-powheg-PUS4_run");     crossSec.push_back( 7.10e-02 * 1.269  * 0.0763019);    readEvents.push_back(15156);

  samples.push_back("VBFH125-powheg-PUS4_run");     crossSec.push_back( 6.37e-02 * 1.211  * 0.07841419);   readEvents.push_back(15574);
  samples.push_back("GGFH125-powheg-PUS4_run");     crossSec.push_back( 6.37e-02 * 15.31  * 0.04875379);   readEvents.push_back(8775);
  samples.push_back("VBFH130-powheg-PUS4_run");     crossSec.push_back( 5.48e-02 * 1.154  * 0.081445 );    readEvents.push_back(16064);
  samples.push_back("GGFH130-powheg-PUS4_run");     crossSec.push_back( 5.48e-02 * 14.12  * 0.0514438 );   readEvents.push_back(10288);
  samples.push_back("VBFH135-powheg-PUS4_run");     crossSec.push_back( 4.52e-02 * 1.100  * 0.0844918 );   readEvents.push_back(16787);
  samples.push_back("GGFH135-powheg-PUS4_run");     crossSec.push_back( 4.52e-02 * 13.08  * 0.0552598);    readEvents.push_back(10973);
  samples.push_back("VBFH140-powheg-PUS4_run");     crossSec.push_back( 3.54e-02 * 1.052  * 0.0851500);    readEvents.push_back(16506);
  samples.push_back("GGFH140-powheg-PUS4_run");     crossSec.push_back( 3.54e-02 * 12.13  * 0.05694626);   readEvents.push_back(10908);

  // normalization Lumi
  Float_t Lumi=1000;

  TString sample(samples[index]);

  TString outName = "nTuple"+sample+"_Open_ElecTauStream.root";
  TFile *outFile = new TFile(outName,"RECREATE");
  TTree* outTreePtOrd     = new TTree("outTreePtOrd","tree jets pT-ord");
 
  float pt1,pt2,eta1,eta2,Deta,Mjj,Dphi;
  float pt1_v2,pt2_v2,eta1_v2,eta2_v2,Deta_v2,Mjj_v2,Dphi_v2;
  float diTauNSVfitMass_,diTauNSVfitMassErrUp_,diTauNSVfitMassErrDown_, diTauCAPt,diTauCAEta,diTauSVFitPt,diTauSVFitEta;
  float diTauVisMass,diTauVisPt,diTauVisEta;
  float ptL1,ptL2,etaL1,etaL2,leadTrackPt;
  float diTauCharge_,MtLeg1_,MtLeg1Corr_,pZeta_,pZetaVis_,MEt,pZetaCorr_,pZetaVisCorr_,MEtCorr;
  float numPV_;
  float sampleWeight,puWeight;
  float combRelIsoLeg1Raw,combRelIsoLeg1,combRelIsoLeg1Beta,combRelIsoLeg1DBeta,combRelIsoLeg1Rho;
  float combIsoLeg2;
  int tightestHPSWP_,tightestHPSDBWP_;
  float jetsBtagHE1,jetsBtagHE2;
  float ptVeto, chFracPV1, chFracPV2;
  float HLTx,HLTmatch,HLTweightElec,HLTweightTau;
  int isTauLegMatched_,elecFlag_,genDecay_;
  ULong64_t event_,run_,lumi_;

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
  outTreePtOrd->Branch("diTauNSVfitMass", &diTauNSVfitMass_,"diTauNSVfitMass/F");
  outTreePtOrd->Branch("diTauNSVfitMassErrUp",  &diTauNSVfitMassErrUp_, "diTauNSVfitMassErrUp/F");
  outTreePtOrd->Branch("diTauNSVfitMassErrDown",   &diTauNSVfitMassErrDown_,"diTauNSVfitMassErrDown/F");

  outTreePtOrd->Branch("etaL1", &etaL1,"etaL1/F");
  outTreePtOrd->Branch("etaL2", &etaL2,"etaL2/F");
  outTreePtOrd->Branch("ptL1",  &ptL1,"ptL1/F");
  outTreePtOrd->Branch("ptL2",  &ptL2,"ptL2/F");
  outTreePtOrd->Branch("Dphi",  &Dphi,"Dphi/F");
  outTreePtOrd->Branch("diTauCharge",  &diTauCharge_,"diTauCharge/F");
  outTreePtOrd->Branch("MtLeg1",  &MtLeg1_,"MtLeg1/F");
  outTreePtOrd->Branch("pZeta",  &pZeta_,"pZeta/F");
  outTreePtOrd->Branch("pZetaVis",  &pZetaVis_,"pZetaVis/F");
  outTreePtOrd->Branch("MEt",  &MEt,"MEt/F");
  outTreePtOrd->Branch("MtLeg1Corr",  &MtLeg1Corr_,"MtLeg1Corr/F");
  outTreePtOrd->Branch("pZetaCorr",  &pZetaCorr_,"pZetaCorr/F");
  outTreePtOrd->Branch("pZetaVisCorr",  &pZetaVisCorr_,"pZetaVisCorr/F");
  outTreePtOrd->Branch("MEtCorr",  &MEtCorr,"MEtCorr/F");
  outTreePtOrd->Branch("numPV",  &numPV_,"numPV/F");
  outTreePtOrd->Branch("sampleWeight",  &sampleWeight,"sampleWeight/F"); 
  outTreePtOrd->Branch("puWeight",  &puWeight,"puWeight/F");
  outTreePtOrd->Branch("chFracPV1",  &chFracPV1,"chFracPV1/F"); 
  outTreePtOrd->Branch("chFracPV2",  &chFracPV2,"chFracPV2/F"); 
  outTreePtOrd->Branch("combRelIsoLeg1DBeta",  &combRelIsoLeg1DBeta,"combRelIsoLeg1DBeta/F");
  outTreePtOrd->Branch("combIsoLeg2",  &combIsoLeg2,"combIsoLeg2/F");
  outTreePtOrd->Branch("tightestHPSWP",  &tightestHPSWP_,"tightestHPSWP/I");
  outTreePtOrd->Branch("tightestHPSDBWP",  &tightestHPSDBWP_,"tightestHPSDBWP/I");
  outTreePtOrd->Branch("HLTx", &HLTx,"HLTx/F");
  outTreePtOrd->Branch("HLTweightElec", &HLTweightElec,"HLTweightElec/F");
  outTreePtOrd->Branch("HLTweightTau", &HLTweightTau,"HLTweightTau/F");
  outTreePtOrd->Branch("HLTmatch", &HLTmatch,"HLTmatch/F");
  outTreePtOrd->Branch("isTauLegMatched", &isTauLegMatched_,"isTauLegMatched/I");
  outTreePtOrd->Branch("genDecay", &genDecay_,"genDecay/I");
  outTreePtOrd->Branch("elecFlag", &elecFlag_,"elecFlag/I"); 
  outTreePtOrd->Branch("event", &event_,"event/l");
  outTreePtOrd->Branch("run", &run_,"run/l");
  outTreePtOrd->Branch("lumi", &lumi_,"lumi/l");
 
  outTreePtOrd->Branch("jetsBtagHE1",  &jetsBtagHE1,"jetsBtagHE1/F");
  outTreePtOrd->Branch("jetsBtagHE2",  &jetsBtagHE2,"jetsBtagHE2/F");

 
  TString inName = "/data_CMS/cms/lbianchini/ElecTauStreamSummer11_iter3//treeElecTauStream_"+sample+".root";
  TFile* file   = new TFile(inName,"READ");
  if(file->IsZombie()){
    cout << "No such file!" << endl;
    return;
  }
  TString treeName = doExlusive ? "elecTauStreamAnalyzer2/tree" : "elecTauStreamAnalyzer/tree";
  TTree* currentTree = (TTree*)file->Get(treeName);
  int nEntries = currentTree->GetEntries() ;
  int nEventsRead = readEvents[index];//((TTree*)file->Get("elecTauStreamAnalyzer/tree"))->GetEntries() ;

  float scaleFactor = (crossSec[index] != 0) ? ((crossSec[index])*Lumi*(float(nEntries)/float(nEventsRead))) / nEntries : 1;

  
  currentTree->SetBranchStatus("diTauVisP4",1);
  currentTree->SetBranchStatus("diTauCAP4",0);
  currentTree->SetBranchStatus("diTauICAP4",0);
  currentTree->SetBranchStatus("diTauSVfitP4",0);
  currentTree->SetBranchStatus("diTauNSVfitMass",1);
  currentTree->SetBranchStatus("diTauNSVfitMassErrUp",1);
  currentTree->SetBranchStatus("diTauNSVfitMassErrDown",1);

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
  currentTree->SetBranchStatus("jetsChNfraction",1);
  currentTree->SetBranchStatus("jetsChEfraction",0);
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
  currentTree->SetBranchStatus("genDecay",1);
  currentTree->SetBranchStatus("genVP4",1);
  currentTree->SetBranchStatus("nPUVertices",1);
  currentTree->SetBranchStatus("numPV",1);
  currentTree->SetBranchStatus("numOfDiTaus",0);
  currentTree->SetBranchStatus("decayMode",0);
  currentTree->SetBranchStatus("tightestHPSWP",1);
  currentTree->SetBranchStatus("tightestHPSDBWP",1);
  currentTree->SetBranchStatus("visibleTauMass",0);
  currentTree->SetBranchStatus("isTauLegMatched",1);
  currentTree->SetBranchStatus("isElecLegMatched",0);
  currentTree->SetBranchStatus("elecFlag",1);
  currentTree->SetBranchStatus("diTauCharge",1);
  currentTree->SetBranchStatus("tauXTriggers",1);
  currentTree->SetBranchStatus("triggerBits",1);
  currentTree->SetBranchStatus("leadPFChargedHadrCandTrackPt",0);

  //std::vector<double>* jetsBtagHE;
 
  float chIsoLeg2, phIsoLeg2;
  float chIsoLeg1,nhIsoLeg1,phIsoLeg1; 
  float chIsoPULeg1,nhIsoPULeg1,phIsoPULeg1; 
  int tightestHPSWP,tightestHPSDBWP,elecFlag;  
  float  diTauCharge, MtLeg1,pZeta,pZetaVis,leadPFChargedHadrCandTrackPt; 
  float numPV, mcPUweight,rhoFastJet;
  float diTauNSVfitMass,diTauNSVfitMassErrUp,diTauNSVfitMassErrDown;
  float weight;
  int isTauLegMatched;
  int genDecay;
  int nPUVertices;
  ULong64_t event,run,lumi;

  std::vector< LV >* jets        = new std::vector< LV >();
  currentTree->SetBranchAddress("jetsIDP4",   &jets);
  std::vector< LV >* jets_v2        = new std::vector< LV >();
  currentTree->SetBranchAddress("jetsIDL1OffsetP4",   &jets_v2);
  std::vector< LV >* diTauLegsP4 = new std::vector< LV >();
  currentTree->SetBranchAddress("diTauLegsP4",&diTauLegsP4);
  std::vector< LV >* diTauVisP4  =  new std::vector< LV >();
  currentTree->SetBranchAddress("diTauVisP4",&diTauVisP4);
  std::vector< LV >* genVP4 =  new std::vector< LV >();
  currentTree->SetBranchAddress("genVP4",&genVP4);
  std::vector< LV >* METP4 =  new std::vector< LV >();
  currentTree->SetBranchAddress("METP4",&METP4);

  std::vector< int >* tauXTriggers =  new std::vector< int >();
  currentTree->SetBranchAddress("tauXTriggers",&tauXTriggers);
  std::vector< int >* triggerBits =  new std::vector< int >();
  currentTree->SetBranchAddress("triggerBits",&triggerBits);
  std::vector< double >* jetsBtagHE =  new std::vector< double >();
  currentTree->SetBranchAddress("jetsBtagHE",&jetsBtagHE);

  std::vector< float >* jetsChNfraction =  new std::vector< float >();
  currentTree->SetBranchAddress("jetsChNfraction",&jetsChNfraction);

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
  currentTree->SetBranchAddress("tightestHPSDBWP",&tightestHPSDBWP);
  currentTree->SetBranchAddress("diTauCharge",&diTauCharge);

  currentTree->SetBranchAddress("diTauNSVfitMass",&diTauNSVfitMass);
  currentTree->SetBranchAddress("diTauNSVfitMassErrUp",&diTauNSVfitMassErrUp);
  currentTree->SetBranchAddress("diTauNSVfitMassErrDown",&diTauNSVfitMassErrDown);

  currentTree->SetBranchAddress("MtLeg1",&MtLeg1);
  currentTree->SetBranchAddress("pZeta",&pZeta);
  currentTree->SetBranchAddress("pZetaVis",&pZetaVis);
  currentTree->SetBranchAddress("numPV",&numPV);
  currentTree->SetBranchAddress("mcPUweight",&mcPUweight);
  currentTree->SetBranchAddress("event",&event);
  currentTree->SetBranchAddress("run",&run);
  currentTree->SetBranchAddress("lumi",&lumi);
  currentTree->SetBranchAddress("nPUVertices",&nPUVertices);
  currentTree->SetBranchAddress("genDecay",&genDecay);
  currentTree->SetBranchAddress("elecFlag",&elecFlag);
  currentTree->SetBranchAddress("isTauLegMatched",&isTauLegMatched);
  //currentTree->SetBranchAddress("leadPFChargedHadrCandTrackPt",&leadPFChargedHadrCandTrackPt);

  if(crossSec[index]<0) currentTree->SetBranchAddress("weight",&weight);

  for (int n = 0; n < nEntries ; n++) {

    currentTree->GetEntry(n);
    if(n%1000==0) cout << n << endl;

    pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;
    pt1_v2=-99;pt2_v2=-99;eta1_v2=-99,eta2_v2=-99;Deta_v2=-99;Dphi_v2=-99;Mjj_v2=-99;

    diTauVisPt=-99;diTauVisEta=-99;diTauCAPt=-99;diTauCAEta=-99;
    diTauNSVfitMass_ = -99;diTauNSVfitMassErrUp_=-99;diTauNSVfitMassErrDown_=-99;diTauVisMass=-99;
    ptL1=-99;ptL2=-99;etaL1=-99;etaL2=-99;leadTrackPt=-99;
    diTauCharge_=-99;MtLeg1_=-99;pZeta_=-99;pZetaVis_=-99;MEt=-99;
    numPV_=-99;sampleWeight=-99;puWeight=-99;
    combRelIsoLeg1DBeta=-99;combIsoLeg2=-99;
    tightestHPSWP_=-99;tightestHPSDBWP_=-99;ptVeto=-99;chFracPV1=-99;chFracPV2=-99;
    HLTx=-99;HLTmatch=-99;isTauLegMatched_=-99;elecFlag_=-99;genDecay_ = -99;
    event_=0;run_=0;lumi_=0;
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
      //if(ptVeto>0) 
      ptVeto = (jets->size()>2) ? (*jets)[2].Pt() : -99;

      jetsBtagHE1 = (*jetsBtagHE)[0];
      jetsBtagHE2 = (*jetsBtagHE)[1];

      chFracPV1 = (*jetsChNfraction)[0]; 
      chFracPV2 = (*jetsChNfraction)[1]; 
    }


    diTauVisMass  = (*diTauVisP4)[0].M();
    diTauVisPt  = (*diTauVisP4)[0].Pt();
    diTauVisEta = (*diTauVisP4)[0].Eta();
    diTauNSVfitMass_ = diTauNSVfitMass;
    diTauNSVfitMassErrUp_ = diTauNSVfitMassErrUp;
    diTauNSVfitMassErrDown_ = diTauNSVfitMassErrDown;
    
    ptL1  = (*diTauLegsP4)[0].Pt();
    ptL2  = (*diTauLegsP4)[1].Pt();
    etaL1 = (*diTauLegsP4)[0].Eta();
    etaL2 = (*diTauLegsP4)[1].Eta();
    diTauCharge_ = diTauCharge;
  
    TLorentzVector corrMET_tmp;
    LV corrMET(1,0,0,1);
    double corrPt = (*METP4)[0].Et(); double corrPhi = (*METP4)[0].Phi();
    if(genVP4->size()){
      if(samples[index].find("WJets")  !=string::npos)      recoilCorr->Correct(corrPt,corrPhi,(*genVP4)[0].Pt() ,(*genVP4)[0].Phi() , ((*diTauLegsP4)[0]).Pt(),((*diTauLegsP4)[0]).Phi()  );
      else if(samples[index].find("DYJets")!=string::npos)  recoilCorr->Correct(corrPt,corrPhi,(*genVP4)[0].Pt() ,(*genVP4)[0].Phi() , ((*diTauLegsP4)[0]+(*diTauLegsP4)[1]).Pt(),((*diTauLegsP4)[0]+(*diTauLegsP4)[1]).Phi()  );
    }
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

    float scalarSumPt = (*diTauLegsP4)[0].Pt() + (*METP4)[0].Pt();
    float vectorSumPt = ((*diTauLegsP4)[0] + (*METP4)[0]).Pt() ;
    MtLeg1_ = TMath::Sqrt( scalarSumPt*scalarSumPt - vectorSumPt*vectorSumPt ) ;
    float scalarSumPtCorr = (*diTauLegsP4)[0].Pt() + corrMET.Pt();
    float vectorSumPtCorr = ((*diTauLegsP4)[0] + corrMET).Pt() ;
    MtLeg1Corr_ = TMath::Sqrt( scalarSumPtCorr*scalarSumPtCorr - vectorSumPtCorr*vectorSumPtCorr ) ;

    tightestHPSWP_   = tightestHPSWP;
    tightestHPSDBWP_ = tightestHPSDBWP;
    numPV_ = numPV;
  
    combRelIsoLeg1DBeta =(chIsoLeg1+ std::max( nhIsoLeg1+phIsoLeg1-0.5*0.5*(nhIsoPULeg1+phIsoPULeg1),double(0.0)))/(*diTauLegsP4)[0].Pt();
    combIsoLeg2 =  ( chIsoLeg2 + std::max( phIsoLeg2 - rhoFastJet*TMath::Pi()*0.5*0.5, 0.0) ) ;
   
    
    sampleWeight = (scaleFactor>=0) ? scaleFactor : weight;
    //puWeight = (std::string(sample.Data())).find("Run2011")!=string::npos ? 1.0 : mcPUweight;
    puWeight = (std::string(sample.Data())).find("Run2011")!=string::npos ? 1.0 : hPU->GetBinContent(nPUVertices+1) ;

    if((std::string(sample.Data())).find("Run2011")!=string::npos){
      
      if(run>=160404 && run<=161176)
	HLTx = 	float((*triggerBits)[0]); //HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau15_v1
      else if(run>=161216 && run<=163261)
	HLTx = 	float((*triggerBits)[1]); //HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau15_v2
      else if(run>=163269 && run<=163869)
	HLTx = 	float((*triggerBits)[2]); //HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau15_v4
      else if(run>=165088 && run<=165633)
	HLTx = 	float((*triggerBits)[3]); //HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau15_v6
      else if(run>=165970 && run<=166967)
	HLTx = 	float((*triggerBits)[4]); //HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau20_v8
      else if(run>=167039 && run<=167913)
	HLTx = 	float((*triggerBits)[5]); //HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau20_v9
      else if(run>=170452)
	HLTx = 	float((*triggerBits)[6]); //HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_TightIsoPFTau20_v2

      if(run>=160404 && run<=165633)
	HLTmatch = float((*tauXTriggers)[0]);
      else if(run>=165970 && run<=167913)
	HLTmatch = float((*tauXTriggers)[1]);

    } else{
      if(tRandom->Uniform(0,1)<0.3){
	HLTx =  float((*triggerBits)[0]);
	HLTmatch = float((*tauXTriggers)[0]);
      }
      else{
	HLTx =  float((*triggerBits)[1]);
	HLTmatch = float((*tauXTriggers)[1]);
      }
    }
    
    HLTweightElec = (std::string(sample.Data())).find("Run2011")==string::npos ? 
      ratioEffElec->ratio((*diTauLegsP4)[0].Pt(), bool(fabs((*diTauLegsP4)[0].Eta())<1.5) ) : 1;
    HLTweightTau = (std::string(sample.Data())).find("Run2011")==string::npos ? 
      ratioEffTau->ratio( (*diTauLegsP4)[1].Pt() ) : 1.0;

    isTauLegMatched_ = isTauLegMatched;
    elecFlag_ = elecFlag;
    genDecay_ = genDecay ;
    event_=event;
    run_=run;
    lumi_=lumi;

    outTreePtOrd->Fill();
  }


 file->Close();

 if(SAVE) outFile->Write();
 outFile->Close();

 delete jets; delete jets_v2; delete diTauLegsP4; delete diTauVisP4; delete tauXTriggers; delete triggerBits;
 delete METP4; delete jetsBtagHE; delete jetsChNfraction; delete genVP4;
 delete ratioEffElec; delete ratioEffTau;  delete recoilCorr; delete hPU; delete tRandom;
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


void makeTrees_MuTauStream(int index = 4, bool doExlusive = false){
  
  gSystem->Load("RecoilCorrector_C.so");
  RecoilCorrector* recoilCorr = new RecoilCorrector();

  gSystem->Load("ratioEfficiencyTau_C.so");
  ratioEfficiencyTau* ratioEffTau = new ratioEfficiencyTau();

  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;

  TFile *fPU = new TFile("pileUp_EPS2011.root","READ");
  TH1F* hPU = (TH1F*)fPU->Get("hWOneBX");

  std::vector<std::string> samples;
  std::vector<float> crossSec;
  std::vector<int> readEvents;

  // samples & x-sections
  samples.push_back("Run2011-Mu_All");                 crossSec.push_back( 0  );                          readEvents.push_back( 0  );
  samples.push_back("DYJets-Mu-50-madgraph-PUS4_run"); crossSec.push_back( 3048           * 0.02461861 ); readEvents.push_back(800630);
  samples.push_back("TTJets-Mu-madgraph-PUS4_run");    crossSec.push_back( 157.5          * 0.20574389);  readEvents.push_back(761653);
  
  samples.push_back("WJets-Mu-madgraph-PUS4_run");     crossSec.push_back( 31314.0        * 0.0124213);    readEvents.push_back(429416);

  samples.push_back("T-Mu-t-PUS4_run");                crossSec.push_back( 42.6  *  0.06856682);           readEvents.push_back(200028);
  samples.push_back("Tbar-Mu-t-PUS4_run");             crossSec.push_back( 22.0  *  0.071513338468);       readEvents.push_back(139081);
  samples.push_back("WZIncl-Mu-pythia-PUS4_run");      crossSec.push_back( 18.2  *  0.0453);               readEvents.push_back(2265);
  samples.push_back("ZZ-Mu-pythia-PUS4_run");          crossSec.push_back( 0.238 *  0.04114);              readEvents.push_back(2057);
  samples.push_back("WW-Mu-pythia-PUS4_run");          crossSec.push_back( 4.51  *  0.09366);              readEvents.push_back(4683);
 
  samples.push_back("VBFH115-Mu-powheg-PUS4_run");     crossSec.push_back( 7.65e-02 * 1.332  * 0.10683611);readEvents.push_back(21159);  
  samples.push_back("VBFH120-Mu-powheg-PUS4_run");     crossSec.push_back( 7.10e-02 * 1.269  * 0.1107091); readEvents.push_back(11599);

  samples.push_back("VBFH125-Mu-powheg-PUS4_run");     crossSec.push_back( 6.37e-02 * 1.211  * 0.11374);     readEvents.push_back(14574);
  samples.push_back("GGFH125-Mu-powheg-PUS4_run");     crossSec.push_back( 6.37e-02 * 15.31  * 0.07533527);  readEvents.push_back(15066);
  samples.push_back("VBFH130-Mu-powheg-PUS4_run");     crossSec.push_back( 5.48e-02 * 1.154  * 0.115987953); readEvents.push_back(22877);
  samples.push_back("GGFH130-Mu-powheg-PUS4_run");     crossSec.push_back( 5.48e-02 * 14.12  * 0.079845988 );readEvents.push_back(15968);
  samples.push_back("VBFH135-Mu-powheg-PUS4_run");     crossSec.push_back( 4.52e-02 * 1.100  * 0.119965573); readEvents.push_back(23835);
  samples.push_back("GGFH135-Mu-powheg-PUS4_run");     crossSec.push_back( 4.52e-02 * 13.08  * 0.083426);    readEvents.push_back(16566);
  samples.push_back("VBFH140-Mu-powheg-PUS4_run");     crossSec.push_back( 3.54e-02 * 1.052  * 0.12298938);  readEvents.push_back(23841);
  samples.push_back("GGFH140-Mu-powheg-PUS4_run");     crossSec.push_back( 3.54e-02 * 12.13  * 0.08600932);  readEvents.push_back(16475);
  
  // normalization Lumi
  Float_t Lumi=1000;

  TString sample(samples[index]);

  TString outName = "nTuple"+sample+"_Open_MuTauStream.root";
  TFile *outFile = new TFile(outName,"RECREATE");
  TTree* outTreePtOrd     = new TTree("outTreePtOrd","tree jets pT-ord");
 
  float pt1,pt2,eta1,eta2,Deta,Mjj,Dphi;
  float pt1_v2,pt2_v2,eta1_v2,eta2_v2,Deta_v2,Mjj_v2,Dphi_v2;
  float diTauNSVfitMass_,diTauNSVfitMassErrUp_,diTauNSVfitMassErrDown_, diTauCAPt,diTauCAEta,diTauSVFitPt,diTauSVFitEta;
  float diTauVisMass,diTauVisPt,diTauVisEta;
  float ptL1,ptL2,etaL1,etaL2,leadTrackPt;
  float diTauCharge_,MtLeg1_,pZeta_,pZetaVis_,MEt, MtLeg1Corr_,pZetaCorr_,pZetaVisCorr_,MEtCorr;
  float numPV_; 
  float sampleWeight,puWeight;
  float combRelIsoLeg1Raw,combRelIsoLeg1,combRelIsoLeg1Beta,combRelIsoLeg1DBeta,combRelIsoLeg1Rho;
  float combIsoLeg2;
  int tightestHPSWP_;
  int tightestHPSDBWP_;
  int genDecay_; int decayMode_; float visibleTauMass_;
  float jetsBtagHE1,jetsBtagHE2;
  float ptVeto, chFracPV1, chFracPV2;
  float HLTmu,HLTx,HLTmatch,HLTweightTau;
  int isTauLegMatched_,muFlag_;
  ULong64_t event_,run_,lumi_;

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
  outTreePtOrd->Branch("diTauNSVfitMass", &diTauNSVfitMass_,"diTauNSVfitMass/F");
  outTreePtOrd->Branch("diTauNSVfitMassErrUp",  &diTauNSVfitMassErrUp_, "diTauNSVfitMassErrUp/F");
  outTreePtOrd->Branch("diTauNSVfitMassErrDown",   &diTauNSVfitMassErrDown_,"diTauNSVfitMassErrDown/F");

  outTreePtOrd->Branch("etaL1", &etaL1,"etaL1/F");
  outTreePtOrd->Branch("etaL2", &etaL2,"etaL2/F");
  outTreePtOrd->Branch("ptL1",  &ptL1,"ptL1/F");
  outTreePtOrd->Branch("ptL2",  &ptL2,"ptL2/F");
  outTreePtOrd->Branch("Dphi",  &Dphi,"Dphi/F");
  outTreePtOrd->Branch("diTauCharge",  &diTauCharge_,"diTauCharge/F");
  outTreePtOrd->Branch("MtLeg1",  &MtLeg1_,"MtLeg1/F");
  outTreePtOrd->Branch("pZeta",  &pZeta_,"pZeta/F");
  outTreePtOrd->Branch("pZetaVis",  &pZetaVis_,"pZetaVis/F");
  outTreePtOrd->Branch("MEt",  &MEt,"MEt/F");
  outTreePtOrd->Branch("MtLeg1Corr",  &MtLeg1Corr_,"MtLeg1Corr/F");
  outTreePtOrd->Branch("pZetaCorr",  &pZetaCorr_,"pZetaCorr/F");
  outTreePtOrd->Branch("pZetaVisCorr",  &pZetaVisCorr_,"pZetaVisCorr/F");
  outTreePtOrd->Branch("MEtCorr",  &MEtCorr,"MEtCorr/F");
  outTreePtOrd->Branch("numPV",  &numPV_,"numPV/F");
  outTreePtOrd->Branch("sampleWeight",  &sampleWeight,"sampleWeight/F"); 
  outTreePtOrd->Branch("puWeight",  &puWeight,"puWeight/F");
  outTreePtOrd->Branch("chFracPV1",  &chFracPV1,"chFracPV1/F"); 
  outTreePtOrd->Branch("chFracPV2",  &chFracPV2,"chFracPV2/F"); 
  outTreePtOrd->Branch("combRelIsoLeg1DBeta",  &combRelIsoLeg1DBeta,"combRelIsoLeg1DBeta/F");
  outTreePtOrd->Branch("combIsoLeg2",  &combIsoLeg2,"combIsoLeg2/F");
  outTreePtOrd->Branch("tightestHPSWP",  &tightestHPSWP_,"tightestHPSWP/I");
  outTreePtOrd->Branch("tightestHPSDBWP",  &tightestHPSDBWP_,"tightestHPSDBWP/I");
  outTreePtOrd->Branch("HLTmu", &HLTmu,"HLTmu/F");
  outTreePtOrd->Branch("HLTx", &HLTx,"HLTx/F");
  outTreePtOrd->Branch("HLTweightTau", &HLTweightTau,"HLTweightTau/F");
  outTreePtOrd->Branch("HLTmatch", &HLTmatch,"HLTmatch/F");
  outTreePtOrd->Branch("isTauLegMatched", &isTauLegMatched_,"isTauLegMatched/I");
  outTreePtOrd->Branch("genDecay", &genDecay_,"genDecay/I");
  outTreePtOrd->Branch("muFlag", &muFlag_,"muFlag/I"); 
  outTreePtOrd->Branch("event", &event_,"event/l");
  outTreePtOrd->Branch("run", &run_,"run/l");
  outTreePtOrd->Branch("lumi", &lumi_,"lumi/l");

  outTreePtOrd->Branch("visibleTauMass", &visibleTauMass_,"visibleTauMass/F");
  outTreePtOrd->Branch("decayMode", &decayMode_,"decayMode/I"); 
    
 
  outTreePtOrd->Branch("jetsBtagHE1",  &jetsBtagHE1,"jetsBtagHE1/F");
  outTreePtOrd->Branch("jetsBtagHE2",  &jetsBtagHE2,"jetsBtagHE2/F");

 
  TString inName = "/data_CMS/cms/lbianchini/MuTauStreamSummer11_iter3//treeMuTauStream_"+sample+".root";
  TFile* file   = new TFile(inName,"READ");
  if(file->IsZombie()){
    cout << "No such file!" << endl;
    return;
  }
  TString treeName = doExlusive ? "muTauStreamAnalyzer2/tree" : "muTauStreamAnalyzer/tree";
  TTree* currentTree = (TTree*)file->Get(treeName);
  int nEntries = currentTree->GetEntries() ;
  int nEventsRead = readEvents[index];//((TTree*)file->Get("muTauStreamAnalyzer/tree"))->GetEntries() ;

  float scaleFactor = (crossSec[index] != 0) ? ((crossSec[index])*Lumi*(float(nEntries)/float(nEventsRead))) / nEntries : 1;

  currentTree->SetBranchStatus("diTauVisP4",1);
  currentTree->SetBranchStatus("diTauCAP4",0);
  currentTree->SetBranchStatus("diTauICAP4",0);
  currentTree->SetBranchStatus("diTauSVfitP4",0);
  currentTree->SetBranchStatus("diTauNSVfitMass",1);
  currentTree->SetBranchStatus("diTauNSVfitMassErrUp",1);
  currentTree->SetBranchStatus("diTauNSVfitMassErrDown",1);

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
  currentTree->SetBranchStatus("jetsChNfraction",1);
  currentTree->SetBranchStatus("jetsChEfraction",0);
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
  currentTree->SetBranchStatus("genDecay",1);
  currentTree->SetBranchStatus("genVP4",1);
  currentTree->SetBranchStatus("nPUVertices",1);
  currentTree->SetBranchStatus("numPV",1);
  currentTree->SetBranchStatus("numOfDiTaus",0);
  currentTree->SetBranchStatus("decayMode",1);
  currentTree->SetBranchStatus("tightestHPSWP",1);
  currentTree->SetBranchStatus("tightestHPSDBWP",1);
  currentTree->SetBranchStatus("visibleTauMass",1);
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
  int tightestHPSWP,tightestHPSDBWP,muFlag;  
  float  diTauCharge, MtLeg1,pZeta,pZetaVis,leadPFChargedHadrCandTrackPt; 
  float numPV, mcPUweight,rhoFastJet;
  float diTauNSVfitMass,diTauNSVfitMassErrUp,diTauNSVfitMassErrDown;
  float weight;
  int isTauLegMatched, decayMode;
  float visibleTauMass;
  int genDecay;
  int nPUVertices;
  ULong64_t event,run,lumi;

  std::vector< LV >* jets        = new std::vector< LV >();
  currentTree->SetBranchAddress("jetsIDP4",   &jets);
  std::vector< LV >* jets_v2        = new std::vector< LV >();
  currentTree->SetBranchAddress("jetsIDL1OffsetP4",   &jets_v2);
  std::vector< LV >* diTauLegsP4 = new std::vector< LV >();
  currentTree->SetBranchAddress("diTauLegsP4",&diTauLegsP4);
  std::vector< LV >* diTauVisP4  =  new std::vector< LV >();
  currentTree->SetBranchAddress("diTauVisP4",&diTauVisP4);
  std::vector< LV >* genVP4 =  new std::vector< LV >();
  currentTree->SetBranchAddress("genVP4",&genVP4);
  std::vector< LV >* METP4 =  new std::vector< LV >();
  currentTree->SetBranchAddress("METP4",&METP4);

  std::vector< int >* tauXTriggers =  new std::vector< int >();
  currentTree->SetBranchAddress("tauXTriggers",&tauXTriggers);
  std::vector< int >* triggerBits =  new std::vector< int >();
  currentTree->SetBranchAddress("triggerBits",&triggerBits);
  std::vector< double >* jetsBtagHE =  new std::vector< double >();
  currentTree->SetBranchAddress("jetsBtagHE",&jetsBtagHE);

  std::vector< float >* jetsChNfraction =  new std::vector< float >();
  currentTree->SetBranchAddress("jetsChNfraction",&jetsChNfraction);

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
  currentTree->SetBranchAddress("tightestHPSDBWP",&tightestHPSDBWP);
  currentTree->SetBranchAddress("diTauCharge",&diTauCharge);

  currentTree->SetBranchAddress("visibleTauMass",&visibleTauMass);
  currentTree->SetBranchAddress("decayMode",&decayMode);

  currentTree->SetBranchAddress("diTauNSVfitMass",&diTauNSVfitMass);
  currentTree->SetBranchAddress("diTauNSVfitMassErrUp",&diTauNSVfitMassErrUp);
  currentTree->SetBranchAddress("diTauNSVfitMassErrDown",&diTauNSVfitMassErrDown);

  currentTree->SetBranchAddress("MtLeg1",&MtLeg1);
  currentTree->SetBranchAddress("pZeta",&pZeta);
  currentTree->SetBranchAddress("pZetaVis",&pZetaVis);
  currentTree->SetBranchAddress("numPV",&numPV);
  currentTree->SetBranchAddress("mcPUweight",&mcPUweight);
  currentTree->SetBranchAddress("event",&event);
  currentTree->SetBranchAddress("run",&run);
  currentTree->SetBranchAddress("lumi",&lumi);
  currentTree->SetBranchAddress("nPUVertices",&nPUVertices);
  currentTree->SetBranchAddress("genDecay",&genDecay);
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
    diTauNSVfitMass_ = -99;diTauNSVfitMassErrUp_=-99;diTauNSVfitMassErrDown_=-99;diTauVisMass=-99;
    ptL1=-99;ptL2=-99;etaL1=-99;etaL2=-99;leadTrackPt=-99;
    diTauCharge_=-99;MtLeg1_=-99;pZeta_=-99;pZetaVis_=-99;MEt=-99;
    numPV_=-99;sampleWeight=-99;puWeight=-99;
    combRelIsoLeg1DBeta=-99;combIsoLeg2=-99;
    tightestHPSWP_=-99;tightestHPSDBWP_=-99;ptVeto=-99;chFracPV1=-99;chFracPV2=-99;
    HLTmu=-99; HLTx=-99;HLTmatch=-99;isTauLegMatched_=-99;muFlag_=-99;genDecay_ = -99;
    event_=0;run_=0;lumi_=0;
    jetsBtagHE1 = -99;decayMode_=-99;visibleTauMass_=-99;
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
      //if(ptVeto>0) 
      ptVeto = (jets->size()>2) ? (*jets)[2].Pt() : -99;

      jetsBtagHE1 = (*jetsBtagHE)[0];
      jetsBtagHE2 = (*jetsBtagHE)[1];

      chFracPV1 = (*jetsChNfraction)[0]; 
      chFracPV2 = (*jetsChNfraction)[1]; 
    }

    decayMode_ = decayMode;
    visibleTauMass_ = visibleTauMass;

    diTauVisMass  = (*diTauVisP4)[0].M();
    diTauVisPt  = (*diTauVisP4)[0].Pt();
    diTauVisEta = (*diTauVisP4)[0].Eta();
    diTauNSVfitMass_ = diTauNSVfitMass;
    diTauNSVfitMassErrUp_ = diTauNSVfitMassErrUp;
    diTauNSVfitMassErrDown_ = diTauNSVfitMassErrDown;
    
    ptL1  = (*diTauLegsP4)[0].Pt();
    ptL2  = (*diTauLegsP4)[1].Pt();
    etaL1 = (*diTauLegsP4)[0].Eta();
    etaL2 = (*diTauLegsP4)[1].Eta();
    diTauCharge_ = diTauCharge;

    TLorentzVector corrMET_tmp;
    LV corrMET(1,0,0,1);
    double corrPt = (*METP4)[0].Et(); double corrPhi = (*METP4)[0].Phi();
    if(genVP4->size()){
      if(samples[index].find("WJets")  !=string::npos)      recoilCorr->Correct(corrPt,corrPhi,(*genVP4)[0].Pt() ,(*genVP4)[0].Phi() , ((*diTauLegsP4)[0]).Pt(),((*diTauLegsP4)[0]).Phi()  );
      else if(samples[index].find("DYJets")!=string::npos)  recoilCorr->Correct(corrPt,corrPhi,(*genVP4)[0].Pt() ,(*genVP4)[0].Phi() , ((*diTauLegsP4)[0]+(*diTauLegsP4)[1]).Pt(),((*diTauLegsP4)[0]+(*diTauLegsP4)[1]).Phi()  );
    }
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

    float scalarSumPt = (*diTauLegsP4)[0].Pt() + (*METP4)[0].Pt();
    float vectorSumPt = ((*diTauLegsP4)[0] + (*METP4)[0]).Pt() ;
    MtLeg1_ = TMath::Sqrt( scalarSumPt*scalarSumPt - vectorSumPt*vectorSumPt ) ;
    float scalarSumPtCorr = (*diTauLegsP4)[0].Pt() + corrMET.Pt();
    float vectorSumPtCorr = ((*diTauLegsP4)[0] + corrMET).Pt() ;
    MtLeg1Corr_ = TMath::Sqrt( scalarSumPtCorr*scalarSumPtCorr - vectorSumPtCorr*vectorSumPtCorr ) ;

    tightestHPSWP_   = tightestHPSWP;
    tightestHPSDBWP_ = tightestHPSDBWP;
    numPV_ = numPV;
  
    combRelIsoLeg1DBeta =(chIsoLeg1+ std::max( nhIsoLeg1+phIsoLeg1-0.5*0.5*(nhIsoPULeg1+phIsoPULeg1),double(0.0)))/(*diTauLegsP4)[0].Pt();
    combIsoLeg2 =  ( chIsoLeg2 + std::max( phIsoLeg2 - rhoFastJet*TMath::Pi()*0.5*0.5, 0.0) ) ;
    
    sampleWeight = (scaleFactor>=0) ? scaleFactor : weight;
    //puWeight = (std::string(sample.Data())).find("Run2011")!=string::npos ? 1.0 : mcPUweight;
    puWeight = (std::string(sample.Data())).find("Run2011")!=string::npos ? 1.0 : hPU->GetBinContent(nPUVertices+1) ;

    if((std::string(sample.Data())).find("Run2011")!=string::npos){
      if(run>=160404 && run<=163261)
	HLTmu = float((*triggerBits)[9]);   //HLT_IsoMu12_v1
      else if (run>=163269)
	HLTmu = float((*triggerBits)[10]);   //HLT_IsoMu12_v2
      
      if(run>=160404 && run<=161176)
	HLTx = 	float((*triggerBits)[0]); //HLT_IsoMu12_LooseIsoPFTau10_v1
      else if(run>=161216 && run<=163261)
	HLTx = 	float((*triggerBits)[1]); //HLT_IsoMu12_LooseIsoPFTau10_v2
      else if(run>=163269 && run<=163869)
	HLTx = 	float((*triggerBits)[2]); //HLT_IsoMu12_LooseIsoPFTau10_v4
      else if(run>=165088 && run<=165633)
	HLTx = 	float((*triggerBits)[3]); //HLT_IsoMu15_LooseIsoPFTau15_v2
      else if(run>=165970 && run<=167043 && run!=166346)
	HLTx = 	float((*triggerBits)[4]); //HLT_IsoMu15_LooseIsoPFTau15_v4
      else if(run==166346)
	HLTx =  float((*triggerBits)[5]); //HLT_IsoMu15_LooseIsoPFTau15_v5      
      else if(run>=167078 && run<=167913)
	HLTx =  float((*triggerBits)[6]); //HLT_IsoMu15_LooseIsoPFTau15_v6      
      else if(run>=170249)
	HLTx =  float((*triggerBits)[7]); //HLT_IsoMu15_LooseIsoPFTau15_v8 

      if(run<163269)
	HLTmatch = float( ( (*tauXTriggers)[3]==1 || (*tauXTriggers)[3]==2 ) );
      else if(run>=163269 && run<=163869)
	HLTmatch = float((*tauXTriggers)[0]);
      else if(run>=165088)
	HLTmatch = float((*tauXTriggers)[1]);
    } else{
      HLTmu = float((*triggerBits)[3]);   //HLT_IsoMu12
      HLTx = 	float((*triggerBits)[1]); //HLT_IsoMu12_LooseIsoPFTau10_v2
      HLTmatch = float((*tauXTriggers)[1]);
    }
    
    HLTweightTau = (std::string(sample.Data())).find("Run2011")==string::npos ? 
      ratioEffTau->ratio( (*diTauLegsP4)[1].Pt() ) : 1.0;

    isTauLegMatched_ = isTauLegMatched;
    genDecay_ = genDecay ;
    muFlag_ = muFlag;
    event_=event;
    run_=run;
    lumi_=lumi;

    outTreePtOrd->Fill();
  }


 file->Close();

 if(SAVE) outFile->Write();
 outFile->Close();

 delete jets; delete jets_v2; delete diTauLegsP4; delete diTauVisP4; delete tauXTriggers; delete triggerBits;
 delete METP4; delete jetsBtagHE; delete jetsChNfraction; delete genVP4;delete recoilCorr; delete ratioEffTau;

}



void doAllSamplesElec(bool doExlusive = false){
 
  for( unsigned int k = 0; k < 18 ; k++)  makeTrees_ElecTauStream(k, doExlusive);

  return;

}

void doAllSamplesMu(bool doExlusive = false){
 
  for( unsigned int k = 0; k < 19 ; k++)  makeTrees_MuTauStream(k, doExlusive);

  return;

}
