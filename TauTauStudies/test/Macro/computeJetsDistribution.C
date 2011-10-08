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
#include "TChain.h"

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"

#include <vector>
#include <utility>
#include <map>
#include <algorithm>

#define SAVE   true
#define MINPt 30.0

using namespace ROOT::Math;
using namespace std;


void makePlot(TH1F* hMult=0, 
	      TH1F* hPt1=0, 
	      TH1F* hPt2=0, 
	      TH1F* hPt3=0, 
	      TH1F* hDeta=0,
	      TH1F* hMjj=0,  
	      TH1F* hDphi=0,
	      TH1F* hFrac1=0,    
	      TH1F* hFrac2=0,    
	      TTree* treeData=0, bool isMC=false, string analysis = "" ){

  // pu weights
  TFile fgen("pileUp_EPS2011.root");
  TFile fdat("/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions11/7TeV/PileUp/Pileup_2011_to_173692_LPLumiScale_68mb.root");

  TH1F *hgen = (TH1F*)fgen.Get("hMCOneBX");
  TH1F *hdat = (TH1F*)fdat.Get("pileup");

  TH1F *hgen2 = (TH1F*)hdat->Clone("hgen2");
  for(int k=1; k <= hgen2->GetNbinsX(); k++){
    if(k==hgen2->GetNbinsX()){
      hgen2->SetBinContent(k, 0);
      continue;
    }
    hgen2->SetBinContent(k, hgen->GetBinContent(k));
  }

  TH1F *hRatio = (TH1F*)hdat->Clone("hRatio");
  hRatio->Reset();

  hdat->Scale(1./hdat->Integral());
  hgen2->Scale(1./hgen2->Integral());
  hRatio->Divide(hdat,hgen2,1,1) ;

  float chIsoLeg1v2,nhIsoLeg1v2,phIsoLeg1v2,chIsoPULeg1v2,nhIsoPULeg1v2,phIsoPULeg1v2;
  float chIsoLeg2v2,nhIsoLeg2v2,phIsoLeg2v2,chIsoPULeg2v2,nhIsoPULeg2v2,phIsoPULeg2v2;
  float mcPUweight,rhoNeutralFastJet;
  float numPV;
  int nPUVertices;

  std::vector< double >* jetsBtagHE = new std::vector< double >();
  std::vector< double >* jetsBtagHP = new std::vector< double >();
  std::vector< float >* jetsChNfraction =  new std::vector< float >();
  
  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;

  std::vector< LV >* jets     = new std::vector< LV >();
  std::vector< LV >* muons    = new std::vector< LV >();
  std::vector< LV >* MET      = new std::vector< LV >();

  if(analysis.find("JetUp")!=string::npos)
    treeData->SetBranchAddress("jetsIDUpP4"  ,&jets);
  else if(analysis.find("JetDown")!=string::npos)
    treeData->SetBranchAddress("jetsIDDownP4",&jets);
  else
    treeData->SetBranchAddress("jetsIDP4"    ,&jets);

  treeData->SetBranchAddress("diMuonLegsP4",&muons);
  treeData->SetBranchAddress("METP4",&MET);
  treeData->SetBranchAddress("chIsoLeg1v2",&chIsoLeg1v2);
  treeData->SetBranchAddress("nhIsoLeg1v2",&nhIsoLeg1v2);
  treeData->SetBranchAddress("phIsoLeg1v2",&phIsoLeg1v2);
  treeData->SetBranchAddress("chIsoPULeg1v2",&chIsoPULeg1v2);
  treeData->SetBranchAddress("nhIsoPULeg1v2",&nhIsoPULeg1v2);
  treeData->SetBranchAddress("phIsoPULeg1v2",&phIsoPULeg1v2);
  treeData->SetBranchAddress("chIsoLeg2v2",&chIsoLeg2v2);
  treeData->SetBranchAddress("nhIsoLeg2v2",&nhIsoLeg2v2);
  treeData->SetBranchAddress("phIsoLeg2v2",&phIsoLeg2v2);
  treeData->SetBranchAddress("chIsoPULeg2v2",&chIsoPULeg2v2);
  treeData->SetBranchAddress("nhIsoPULeg2v2",&nhIsoPULeg2v2);
  treeData->SetBranchAddress("phIsoPULeg2v2",&phIsoPULeg2v2);
  treeData->SetBranchAddress("mcPUweight",&mcPUweight);
  treeData->SetBranchAddress("rhoNeutralFastJet",&rhoNeutralFastJet);
  treeData->SetBranchAddress("numPV",&numPV);
  treeData->SetBranchAddress("nPUVertices",&nPUVertices);
  treeData->SetBranchAddress("jetsBtagHE",      &jetsBtagHE);
  treeData->SetBranchAddress("jetsBtagHP",      &jetsBtagHP);
  treeData->SetBranchAddress("jetsChNfraction", &jetsChNfraction);

  TBranch *brjets          = treeData->GetBranch("jetsIDP4");
  TBranch *brjetsUp        = treeData->GetBranch("jetsIDUpP4");
  TBranch *brjetsDown      = treeData->GetBranch("jetsIDDownP4");
  TBranch *brmuons         = treeData->GetBranch("diMuonLegsP4");
  TBranch *brMET           = treeData->GetBranch("METP4");
  TBranch *brchIsoLeg1v2   = treeData->GetBranch("chIsoLeg1v2");
  TBranch *brnhIsoLeg1v2   = treeData->GetBranch("nhIsoLeg1v2");
  TBranch *brphIsoLeg1v2   = treeData->GetBranch("phIsoLeg1v2");
  TBranch *brchIsoPULeg1v2 = treeData->GetBranch("chIsoPULeg1v2");
  TBranch *brnhIsoPULeg1v2 = treeData->GetBranch("nhIsoPULeg1v2");
  TBranch *brphIsoPULeg1v2 = treeData->GetBranch("phIsoPULeg1v2");

  TBranch *brchIsoLeg2v2   = treeData->GetBranch("chIsoLeg2v2");
  TBranch *brnhIsoLeg2v2   = treeData->GetBranch("nhIsoLeg2v2");
  TBranch *brphIsoLeg2v2   = treeData->GetBranch("phIsoLeg2v2");
  TBranch *brchIsoPULeg2v2 = treeData->GetBranch("chIsoPULeg2v2");
  TBranch *brnhIsoPULeg2v2 = treeData->GetBranch("nhIsoPULeg2v2");
  TBranch *brphIsoPULeg2v2 = treeData->GetBranch("phIsoPULeg2v2");

  TBranch *brmcPUweight    = treeData->GetBranch("mcPUweight");
  TBranch *brrhoNeutralFastJet   = treeData->GetBranch("rhoNeutralFastJet");
  TBranch *brnumPV         = treeData->GetBranch("numPV");
  TBranch *brnPUVertices   = treeData->GetBranch("nPUVertices");
  TBranch *brjetsBtagHE    = treeData->GetBranch("jetsBtagHE");
  TBranch *brjetsBtagHP    = treeData->GetBranch("jetsBtagHP");
  TBranch *brjetsChNfraction     = treeData->GetBranch("jetsChNfraction");
  
 
  unsigned int nEntries = treeData->GetEntries() ;

  int total = 0;
  int vbf   = 0;

  cout << "Now doing " << analysis << endl;
  for(unsigned int n = 0 ; n<nEntries ; n++){

    if(n%100000==0) cout << "Processing event " << n << " ..." << endl;

    brjets->GetEntry(n);
    brjetsUp->GetEntry(n);
    brjetsUp->GetEntry(n);
    brjetsDown->GetEntry(n);
    brmuons->GetEntry(n);
    brMET->GetEntry(n);
    brchIsoLeg1v2->GetEntry(n);
    brnhIsoLeg1v2->GetEntry(n);
    brphIsoLeg1v2->GetEntry(n);
    brchIsoPULeg1v2->GetEntry(n);
    brnhIsoPULeg1v2->GetEntry(n);
    brphIsoPULeg1v2->GetEntry(n);
    brchIsoLeg2v2->GetEntry(n);
    brnhIsoLeg2v2->GetEntry(n);
    brphIsoLeg2v2->GetEntry(n);
    brchIsoPULeg2v2->GetEntry(n);
    brnhIsoPULeg2v2->GetEntry(n);
    brphIsoPULeg2v2->GetEntry(n);
    brmcPUweight->GetEntry(n);
    brrhoNeutralFastJet->GetEntry(n);
    brnumPV->GetEntry(n);
    brnPUVertices->GetEntry(n);
    brjetsBtagHE->GetEntry(n);
    brjetsBtagHP->GetEntry(n);
    brjetsChNfraction->GetEntry(n);
    //////////////////////////////////////////////////////////////////////////////////////

    float combRelIsoLeg1DBeta 
      = (chIsoLeg1v2+ std::max( nhIsoLeg1v2+phIsoLeg1v2-0.5*0.5*(nhIsoPULeg1v2+phIsoPULeg1v2),double(0.0)))/(*muons)[0].Pt();
    float combRelIsoLeg2DBeta 
      = (chIsoLeg2v2+ std::max( nhIsoLeg2v2+phIsoLeg2v2-0.5*0.5*(nhIsoPULeg2v2+phIsoPULeg2v2),double(0.0)))/(*muons)[1].Pt();
    
    float puWeight = isMC ? hRatio->GetBinContent( hRatio->FindBin(nPUVertices) ) : 1.0 ;

    if(!(combRelIsoLeg1DBeta<0.10 && combRelIsoLeg2DBeta<0.30 && 
	 (( (*muons)[0].Pt()>25 && (*muons)[1].Pt()>15 )  ||
	  ( (*muons)[1].Pt()>25 && (*muons)[0].Pt()>15 )) &&
	 ( (*muons)[0]+(*muons)[1] ).M()> 80 && ( (*muons)[0]+(*muons)[1] ).M() < 100
	 )   
       )
      continue;   

    int mult = 0;
    float pt1=-99; float eta1=-99; float phi1=-99; float jetsBtagHE1=-99; float jetsBtagHP1=-99; float chFracPV1=-99;
    float pt2=-99; float eta2=-99; float phi2=-99; float jetsBtagHE2=-99; float jetsBtagHP2=-99; float chFracPV2=-99;
    float pt3=-99; float  eta3=-99;
    float Deta=-99; float Dphi=-99; float Mjj=-99;

    for(unsigned int k = 0 ; k < jets->size() ; k++){
      if((*jets)[k].Pt()>MINPt && abs((*jets)[k].Eta())<4.5) mult++;
    }

    hMult->Fill(mult, puWeight);

    if(jets->size()>0){
      pt1  = (*jets)[0].Pt();
      eta1 = (*jets)[0].Eta();
      phi1 = (*jets)[0].Phi();
      jetsBtagHE1 = (*jetsBtagHE)[0];
      jetsBtagHP1 = (*jetsBtagHP)[0];
      chFracPV1 = (*jetsChNfraction)[0]; 
    }
    if(jets->size()>1 && (*jets)[0].Pt()>MINPt){
      pt2  = (*jets)[1].Pt();
      eta2 = (*jets)[1].Eta();
      phi2 = (*jets)[1].Phi();
      jetsBtagHE2 = (*jetsBtagHE)[1];
      jetsBtagHP2 = (*jetsBtagHP)[1];
      chFracPV2 = (*jetsChNfraction)[1];
    }
    if(jets->size()>2 && (*jets)[0].Pt()>MINPt && (*jets)[1].Pt()>MINPt){
      pt3  = (*jets)[2].Pt();
      eta3 = (*jets)[2].Eta();
    }
    if(jets->size()>1 && (*jets)[0].Pt()>MINPt && (*jets)[1].Pt()>MINPt){
      Deta = abs(eta1-eta2);
      Dphi = abs((*jets)[0].Phi()-(*jets)[1].Phi()) > TMath::Pi() ? 
	-abs( (*jets)[0].Phi()-(*jets)[1].Phi() ) + 2*TMath::Pi()  :
	abs( (*jets)[0].Phi()-(*jets)[1].Phi() ) ;
      Mjj  = ((*jets)[0]+(*jets)[1]).M();
    }


    for(int v = 0 ; v < muons->size() ; v++){

      LV MEt =  (*MET)[0]+(*muons)[v];
      
      for( int k = 0 ; k < jets->size() ; k++){
	int index0 = -99;
	int index1 = -99;
	int index2 = -99;
	int index3 = -99;
	if( (*jets)[k].Pt()>20 && fabs((*jets)[k].Eta())<2.4 ){
	  index0 = k;
	  for( int m = 0 ;  m < jets->size() ; m++){
	    if(m!= k && index1>=0 && index2>=0 && index3<0)  index3 = m;
	    if(m!= k && index1>=0 && index2<0  && index3<0)  index2 = m;
	    if(m!= k && index1<0  && index2<0  && index3<0)  index1 = m;
	  }
	  //cout << index0 << " - " << index1 << " - " << index2 << " - " << index3 << endl;
	}

	if(index0>0){
	  float cos1 = (*muons)[1-v].Px()/(*muons)[1-v].Pt();
	  float cos2 = (*jets)[index0].Px()/(*jets)[index0].Pt();
	  float sen1 = (*muons)[1-v].Py()/(*muons)[1-v].Pt();
	  float sen2 = (*jets)[index0].Py()/(*jets)[index0].Pt();
	  float bisecX = cos1 + cos2;
	  float bisecY = sen1 + sen2;
	  float norm = TMath::Sqrt(bisecX*bisecX + bisecY*bisecY);
	  if(norm>0.){
	    bisecX /= norm;
	    bisecY /= norm;
	  }
	  float pZetaVis = ((*muons)[1-v]+(*jets)[index0]).Px()*bisecX + ((*muons)[1-v]+(*jets)[index0]).Py()*bisecY;
	  float pZeta    = ((*muons)[1-v]+(*jets)[index0]+MEt).Px()*bisecX + ((*muons)[1-v]+(*jets)[index0]+MEt).Py()*bisecY; 

	  if((pZeta-1.5*pZetaVis) < -20) index0 = -99;
	}

	if(index0>=0) total+=puWeight;
	if(index0>=0 && index1>=0 && index2>=0 && (index3<0 || (*jets)[index3].Pt()<30)
	   && (*jets)[index1].Pt()>30 && (*jets)[index2].Pt()>30 && TMath::Abs((*jets)[index1].Eta()-(*jets)[index2].Eta())>3.5 &&
	   ((*jets)[index1]+(*jets)[index2]).M()>350 )
	  vbf+=puWeight;
      }
    }

    //total+=puWeight;
    //if(pt1>30 && pt2>30 && Deta>3.5 && Mjj>350 && pt3<30) vbf+=puWeight;


    if(hPt1)   hPt1->Fill(pt1,puWeight);
    if(hPt2)   hPt2->Fill(pt2,puWeight);
    if(hPt3)   hPt3->Fill(pt3,puWeight);
    if(hDeta)  hDeta->Fill(Deta,puWeight);
    if(hDphi)  hDphi->Fill(Dphi,puWeight);
    if(hMjj)   hMjj->Fill(Mjj,puWeight);
    if(hFrac1) hFrac1->Fill(chFracPV1,puWeight);
    if(hFrac2) hFrac2->Fill(chFracPV2,puWeight);

  }

   delete jets;
   delete muons ;
   delete MET ;
   delete jetsBtagHE;
   delete jetsBtagHP;
   delete jetsChNfraction;

   cout << "Efficiency for vbf cut " << vbf << "/" << total << " = " << float(vbf)/float(total) << endl;

}

void setUp(TH1F* hData=0, TH1F* hMC0=0, TH1F* hMCUp=0, TH1F* hMCDown=0){
  hData->SetMarkerColor(kBlack);
  hData->SetMarkerStyle(kFullCircle);
  hMC0->SetLineColor(kRed);
  hMCUp->SetLineColor(kRed);
  hMCUp->SetLineStyle(kDashed);
  hMCDown->SetLineColor(kRed);
  hMCDown->SetLineStyle(kDotted);
}

void plot(){

  TCanvas *c0 = new TCanvas("c0","",5,30,650,600);
  c0->SetGrid(0,0);
  c0->SetFillStyle(4000);
  c0->SetFillColor(10);
  c0->SetTicky();
  c0->SetObjectStat(0);
  c0->SetLogy(1);
  c0->Divide(3,3);

  TLegend* leg = new TLegend(0.55,0.68,0.85,0.88,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);

  TFile fdata("/data_CMS/cms/lbianchini/VbfJetsStudy/Background/MuTauStreamSummer11_fAna/treeZmumuPlusJets_Run2011-Mu-Zjets.root");
  TChain mc("zPlusJetsAnalyzer/tree");

  //mc.Add("/data_CMS/cms/lbianchini/VbfJetsStudy/Background/MuTauStreamSummer11_fAna/treeZmumuPlusJets_DYJets-50-madgraph-PUS4-Zjets.root");
  //mc.Add("/data_CMS/cms/lbianchini/VbfJetsStudy/Background/MuTauStreamSummer11_fAna/treeZmumuPlusJets_DYJets-50-madgraph-PUS4-Zjets_1.root");
  mc.Add("/data_CMS/cms/lbianchini/VbfJetsStudy/Background/MuTauStreamSummer11_fAna/treeZmumuPlusJets_DYJets-50-madgraph-PUS4-Zjets_2.root");
  //mc.Add("/data_CMS/cms/lbianchini/VbfJetsStudy/Background/MuTauStreamSummer11_fAna/treeZmumuPlusJets_DYJets-50-madgraph-PUS4-Zjets_3.root");

  TTree* treeData = (TTree*)(fdata.Get("zPlusJetsAnalyzer/tree"));
  mc.LoadTree(1);
  TTree* treeMc   = (TTree*)mc.GetTree();

  TH1F* hMultData = new TH1F("hMultData"," ; multiplicity of jets p_{T}>30 GeV ; units",10,0,10);
  TH1F* hMultMc   = new TH1F("hMultMc","",10,0,10);
  TH1F* hMultMcUp = new TH1F("hMultMcUp","",10,0,10);
  TH1F* hMultMcDown = new TH1F("hMultMcDown","",10,0,10);
  setUp(hMultData,hMultMc,hMultMcUp,hMultMcDown);

  TH1F* hPt1Data = new TH1F("hPt1Data"," ; p_{T} leading jet ; units",60,0,300);
  TH1F* hPt1Mc   = new TH1F("hPt1Mc","",60,0,300);
  TH1F* hPt1McUp = new TH1F("hPt1McUp","",60,0,300);
  TH1F* hPt1McDown = new TH1F("hPt1McDown","",60,0,300);
  setUp(hPt1Data,hPt1Mc,hPt1McUp,hPt1McDown);

  TH1F* hPt2Data = new TH1F("hPt2Data"," ; p_{T} trailing jet ; units",60,0,300);
  TH1F* hPt2Mc   = new TH1F("hPt2Mc","",60,0,300);
  TH1F* hPt2McUp = new TH1F("hPt2McUp","",60,0,300);
  TH1F* hPt2McDown = new TH1F("hPt2McDown","",60,0,300);
  setUp(hPt2Data,hPt2Mc,hPt2McUp,hPt2McDown);

  TH1F* hPt3Data = new TH1F("hPt3Data"," ; p_{T} veto jet ; units",60,0,300);
  TH1F* hPt3Mc   = new TH1F("hPt3Mc","",60,0,300);
  TH1F* hPt3McUp = new TH1F("hPt3McUp","",60,0,300);
  TH1F* hPt3McDown = new TH1F("hPt3McDown","",60,0,300);
  setUp(hPt3Data,hPt3Mc,hPt3McUp,hPt3McDown);

  TH1F* hDetaData = new TH1F("hDetaData"," ; |#Delta#eta| leading-trailing jet ; units",50,0,10);
  TH1F* hDetaMc   = new TH1F("hDetaMc","",50,0,10);
  TH1F* hDetaMcUp = new TH1F("hDetaMcUp","",50,0,10);
  TH1F* hDetaMcDown = new TH1F("hDetaMcDown","",50,0,10);
  setUp(hDetaData,hDetaMc,hDetaMcUp,hDetaMcDown);

  TH1F* hMjjData = new TH1F("hMjjData"," ; Mass leading-trailing jet pair; units",50,0,1000);
  TH1F* hMjjMc   = new TH1F("hMjjMc","",50,0,1000);
  TH1F* hMjjMcUp = new TH1F("hMjjMcUp","",50,0,1000);
  TH1F* hMjjMcDown = new TH1F("hMjjMcDown","",50,0,1000);
  setUp(hMjjData,hMjjMc,hMjjMcUp,hMjjMcDown);

  TH1F* hDphiData = new TH1F("hDphiData"," ; #Delta#phi leading-trailing jet pair; units",40,0,3.2);
  TH1F* hDphiMc   = new TH1F("hDphiMc","",40,0,3.2);
  TH1F* hDphiMcUp = new TH1F("hDphiMcUp","",40,0,3.2);
  TH1F* hDphiMcDown = new TH1F("hDphiMcDown","",40,0,3.2);
  setUp(hDphiData,hDphiMc,hDphiMcUp,hDphiMcDown);

  TH1F* hFrac1Data = new TH1F("hFrac1Data"," ; Fraction of charged energy from PV of leading jet; units",50,0,1);
  TH1F* hFrac1Mc   = new TH1F("hFrac1Mc","",50,0,1);
  TH1F* hFrac1McUp = new TH1F("hFrac1McUp","",50,0,1);
  TH1F* hFrac1McDown = new TH1F("hFrac1McDown","",50,0,1);
  setUp(hFrac1Data,hFrac1Mc,hFrac1McUp,hFrac1McDown);

  TH1F* hFrac2Data = new TH1F("hFrac2Data"," ; Fraction of charged energy from PV of trailing jet; units",50,0,1);
  TH1F* hFrac2Mc   = new TH1F("hFrac2Mc","",50,0,1);
  TH1F* hFrac2McUp = new TH1F("hFrac2McUp","",50,0,1);
  TH1F* hFrac2McDown = new TH1F("hFrac2McDown","",50,0,1);
  setUp(hFrac2Data,hFrac2Mc,hFrac2McUp,hFrac2McDown);

  ///////////////////////////////////////////////////////////////////
  makePlot( hMultData,   hPt1Data,  hPt2Data,  hPt3Data,  hDetaData,   hMjjData  , hDphiData   ,hFrac1Data,   hFrac2Data   ,  treeData, false,   "");
  makePlot( hMultMc,     hPt1Mc,    hPt2Mc,    hPt3Mc,    hDetaMc,     hMjjMc    , hDphiMc     ,hFrac1Mc,     hFrac2Mc     ,  treeMc,   true ,   "");
  makePlot( hMultMcUp,   hPt1McUp,  hPt2McUp,  hPt3McUp,  hDetaMcUp,   hMjjMcUp  , hDphiMcUp   ,hFrac1McUp,   hFrac2McUp   ,  treeMc,   true ,   "JetUp");
  makePlot( hMultMcDown, hPt1McDown,hPt2McDown,hPt3McDown,hDetaMcDown, hMjjMcDown, hDphiMcDown ,hFrac1McDown, hFrac2McDown ,  treeMc,   true ,   "JetDown");
  ///////////////////////////////////////////////////////////////////

  leg->AddEntry(hMultData,  "Observed","P");
  leg->AddEntry(hMultMc,    "Z+jets","L");
  leg->AddEntry(hMultMcUp,  "Z+jets (JES up)","L");
  leg->AddEntry(hMultMcDown,"Z+jets (JES down)","L");


  c0->cd(1);
  gPad->SetLogy();
  hMultData->Sumw2();
  hMultData->DrawNormalized("P");
  hMultMc->DrawNormalized("HISTSAME");
  hMultMcUp->DrawNormalized("HISTSAME");
  hMultMcDown->DrawNormalized("HISTSAME");
  leg->Draw();

  c0->cd(2);
  gPad->SetLogy();
  hPt1Data->Sumw2();
  hPt1Data->DrawNormalized("P");
  hPt1Mc->DrawNormalized("HISTSAME");
  hPt1McUp->DrawNormalized("HISTSAME");
  hPt1McDown->DrawNormalized("HISTSAME");
  leg->Draw();

  c0->cd(3);
  gPad->SetLogy();
  hPt2Data->Sumw2();
  hPt2Data->DrawNormalized("P");
  hPt2Mc->DrawNormalized("HISTSAME");
  hPt2McUp->DrawNormalized("HISTSAME");
  hPt2McDown->DrawNormalized("HISTSAME");
  leg->Draw();

  c0->cd(4);
  gPad->SetLogy();
  hPt3Data->Sumw2();
  hPt3Data->DrawNormalized("P");
  hPt3Mc->DrawNormalized("HISTSAME");
  hPt3McUp->DrawNormalized("HISTSAME");
  hPt3McDown->DrawNormalized("HISTSAME");
  leg->Draw();

  c0->cd(5);
  gPad->SetLogy();
  hDetaData->Sumw2();
  hDetaData->DrawNormalized("P");
  hDetaMc->DrawNormalized("HISTSAME");
  hDetaMcUp->DrawNormalized("HISTSAME");
  hDetaMcDown->DrawNormalized("HISTSAME");
  leg->Draw();

  c0->cd(6);
  gPad->SetLogy();
  hMjjData->Sumw2();
  hMjjData->DrawNormalized("P");
  hMjjMc->DrawNormalized("HISTSAME");
  hMjjMcUp->DrawNormalized("HISTSAME");
  hMjjMcDown->DrawNormalized("HISTSAME");
  leg->Draw();

  c0->cd(7);
  //gPad->SetLogy();
  hDphiData->Sumw2();
  hDphiData->DrawNormalized("P");
  hDphiMc->DrawNormalized("HISTSAME");
  hDphiMcUp->DrawNormalized("HISTSAME");
  hDphiMcDown->DrawNormalized("HISTSAME");
  leg->Draw();

  c0->cd(8);
  //gPad->SetLogy();
  hFrac1Data->Sumw2();
  hFrac1Data->DrawNormalized("P");
  hFrac1Mc->DrawNormalized("HISTSAME");
  hFrac1McUp->DrawNormalized("HISTSAME");
  hFrac1McDown->DrawNormalized("HISTSAME");
  leg->Draw();

  c0->cd(9);
  //gPad->SetLogy();
  hFrac2Data->Sumw2();
  hFrac2Data->DrawNormalized("P");
  hFrac2Mc->DrawNormalized("HISTSAME");
  hFrac2McUp->DrawNormalized("HISTSAME");
  hFrac2McDown->DrawNormalized("HISTSAME");
  leg->Draw();

  c0->Draw();

}



