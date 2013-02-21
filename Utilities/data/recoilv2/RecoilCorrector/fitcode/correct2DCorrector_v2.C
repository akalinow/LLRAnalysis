#include <vector>
#include <sstream>
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TProfile.h"
#include "TF1.h"
#include "TMath.h"
#include "TRandom1.h"
#include "EmuData.hh"
#include "/home/pharris/Analysis/W/condor/run/CMSSW_3_8_4/src/MitWlnu/NYStyle/test/NYStyle.h"
#include "RecoilCorrector.hh"

TFile *fZMCFile = new TFile("ztt-mad_select.root");//dy_select.root");
TTree *fZMCTree = (TTree*) fZMCFile->FindObjectAny("Events");

double mT(double iPt1,double iPt2,double iPhi1,double iPhi2) {
  double lPx = iPt1*cos(iPhi1)+iPt2*cos(iPhi2);
  double lPy = iPt1*sin(iPhi1)+iPt2*sin(iPhi2);
  return sqrt((iPt1+iPt2)*(iPt1+iPt2)-lPx*lPx-lPy*lPy);
}

double pZeta(double iPhi1,double iPhi2,double iMet,double iMPhi) { 
  double lDPhi = fabs(iPhi1-iPhi2); if(lDPhi > 2*TMath::Pi()-lDPhi) lDPhi = 2*TMath::Pi()-lDPhi;
  double projPhi = 0;
    if(iPhi1>iPhi2)
      if(iPhi1-iPhi2 < TMath::Pi()) projPhi = iPhi1 - lDPhi/2;
      else                          projPhi = iPhi1 + lDPhi/2;
    else
      if(iPhi2-iPhi1 < TMath::Pi()) projPhi = iPhi2 - lDPhi/2;
      else        	            projPhi = iPhi2 + lDPhi/2;
    Double_t projX  =  cos(projPhi);
    Double_t projY  =  sin(projPhi);
    double pMProj  = iMet*cos(iMPhi)*projX + iMet*sin(iMPhi)*projY;
    return pMProj;
}
  
void correct2DCorrector_v2(int iNJet=-1,std::string iNameZDat = "recoilfits/recoilfit_higgs_inc.root",//"recoilfits/recoilfit_datamm_njet.root",
			               std::string iNameZMC  = "recoilfits/recoilfit_zmm42X_njet.root") {
  Prep();
  //TFile *lWFile = new TFile("Weight.root");                 //NNLO Reweighting file
  //TH1F  *lWH    = (TH1F*) lWFile->FindObjectAny("Weight");
  RecoilCorrector corrector1(iNameZDat);        // Replace this with iWMC for W events
  corrector1.addMCFile      (iNameZMC);
  //corrector1.addDataFile    (iNameZDat);

  EmuData lEvent; double lMet = 0; double lMPhi = 0;
  //float lGenPt = 0;  float lGenPhi  = 0; float lLepPt = 0;   float lLepPhi = 0; float lMet = 0; float lMPhi = 0; int lNJet = 0; 
  //float lWeight = 0; float lLepEta  = 0; float lLepPhi1 = 0; float lLepPhi2 = 0; float lLepPt1 = 0; float lLepPt2 = 0; 
  //float lMass   = 0; float lSVFMass = 0; float lPVis    = 0;
  //fZMCTree->SetBranchAddress("Events.vpt"         ,&lGenPt); 
  //fZMCTree->SetBranchAddress("Events.vphi"        ,&lGenPhi);
  //fZMCTree->SetBranchAddress("Events.pt"          ,&lLepPt);
  //fZMCTree->SetBranchAddress("Events.phi"         ,&lLepPhi);
  //fZMCTree->SetBranchAddress("Events.phi1"        ,&lLepPhi1);
  //fZMCTree->SetBranchAddress("Events.phi2"        ,&lLepPhi2);
  //fZMCTree->SetBranchAddress("Events.metphi"      ,&lMPhi);
  //fZMCTree->SetBranchAddress("Events.weight"      ,&lWeight);
  //fZMCTree->SetBranchAddress("Events.mass"        ,&lMass);
  //fZMCTree->SetBranchAddress("Events.svfmass"     ,&lSVFMass);
  //fZMCTree->SetBranchAddress("Events.njet"        ,&lNJet);
  //fZMCTree->SetBranchAddress("Events.pvis"        ,&lPVis);
  //fZMCTree->SetBranchAddress("rawMet"      ,&lMet);
  //fZMCTree->SetBranchAddress("rawMetPhi"   ,&lMPhi);
  fZMCTree->SetBranchAddress("Events"      ,&lEvent);


  TFile *lOFile = new TFile("dy_select_corr.root","RECREATE");
  TTree *lTree = new TTree("Events","Events");
  float lFGenPt = 0;  float lFGenPhi  = 0; float lFLepPt   = 0; float lFLepPhi  = 0; int lFNJet = 0; 
  float lFWeight = 0; float lFLepEta  = 0; float lFLepPhi1 = 0; float lFLepPhi2 = 0; float lFLepPt1 = 0; float lFLepPt2 = 0; 
  float lFMass   = 0; float lFSVFMass = 0; float lFPVis    = 0; 
  float lFPMet   = 0;  float lFPMet0  = 0; float lFPMet1   = 0; float lFPMet2   = 0; float lFPMet3  = 0;
  float lFMet    = 0;  float lFMet0   = 0; float lFMet1    = 0; float lFMet2    = 0; float lFMet3   = 0;
  float lFMt     = 0;  float lFMt0    = 0; float lFMt1     = 0; float lFMt2     = 0; float lFMt3    = 0;
  float lFMPhi   = 0;  float lFMPhi0  = 0; float lFMPhi1   = 0; float lFMPhi2   = 0; float lFMPhi3  = 0; float lFU1 = 0; float lFU2 = 0;
  lTree->Branch("vpt"         ,&lFGenPt,  "lFGenPt/F");
  lTree->Branch("vphi"        ,&lFGenPhi, "lFGenPhi/F");
  lTree->Branch("lpt1"        ,&lFLepPt1, "lFLepPt1/F");
  lTree->Branch("lpt2"        ,&lFLepPt2, "lFLepPt2/F");
  lTree->Branch("pt"          ,&lFLepPt,  "lFLepPt/F");
  lTree->Branch("phi"         ,&lFLepPhi, "lFLepPhi/F");
  lTree->Branch("phi1"        ,&lFLepPhi1,"lFLepPhi1/F");
  lTree->Branch("phi2"        ,&lFLepPhi2,"lFLepPhi2/F");
  lTree->Branch("met"         ,&lFMet0,   "lFMet/F");
  lTree->Branch("met0"        ,&lFMet1,   "lFMet0/F");
  lTree->Branch("met1"        ,&lFMet1,   "lFMet1/F");
  lTree->Branch("met2"        ,&lFMet2,   "lFMet2/F");
  lTree->Branch("met3"        ,&lFMet3,   "lFMet3/F");
  lTree->Branch("metphi"      ,&lFMPhi,   "lFMPhi/F");
  lTree->Branch("weight"      ,&lFWeight, "lFWeight/F");
  lTree->Branch("mass"        ,&lFMass,   "lFMass/F");
  lTree->Branch("svfmass"     ,&lFSVFMass,"lFSVFMass/F");
  lTree->Branch("njets"       ,&lFNJet,   "lFNJet/I");
  lTree->Branch("pvis"        ,&lFPVis,   "lFPVis/F");
  lTree->Branch("mt"          ,&lFMt,     "lFMt/F");
  lTree->Branch("mt0"         ,&lFMt0,    "lFMt0/F");
  lTree->Branch("mt1"         ,&lFMt1,    "lFMt1/F");
  lTree->Branch("mt2"         ,&lFMt2,    "lFMt2/F");
  lTree->Branch("mt3"         ,&lFMt3,    "lFMt3/F");
  lTree->Branch("pmet"        ,&lFPMet,   "lFPMet/F");
  lTree->Branch("pmet0"       ,&lFPMet0,  "lFPMet0/F");
  lTree->Branch("pmet1"       ,&lFPMet1,  "lFPMet1/F");
  lTree->Branch("pmet2"       ,&lFPMet2,  "lFPMet2/F");
  lTree->Branch("pmet3"       ,&lFPMet3,  "lFPMet3/F");
  lTree->Branch("u1"          ,&lFU1,    "lFU1/F");
  lTree->Branch("u2"          ,&lFU2,    "lFU2/F");

  TH1F* lMMet0 = new TH1F("mMET0","mMET0",75,0,100); lMMet0->Sumw2(); lMMet0->SetLineColor(kRed); lMMet0->SetLineWidth(1);
  TH1F* lMMet1 = new TH1F("mMET1","mMET1",75,0,100); lMMet1->Sumw2(); lMMet1->SetLineColor(kBlue); lMMet1->SetLineWidth(1);
  TH1F* lMMet2 = new TH1F("mMET2","mMET2",75,0,100); lMMet2->Sumw2(); lMMet2->SetLineColor(kOrange); lMMet2->SetLineWidth(1);
  TH1F* lMMet3 = new TH1F("mMET3","mMET3",75,0,100); lMMet3->Sumw2(); lMMet3->SetLineColor(kViolet); lMMet3->SetLineWidth(1);
  int lN = 1;  //Number of iterations ===> To increase stats (be careful with this)
  int lCount = 0;
  for(int i0 = 0; i0 < fZMCTree->GetEntries(); i0++) {
    fZMCTree->GetEntry(i0);
    lMPhi = lEvent.rawmetphi;
    lMet  = lEvent.rawmet;
    double pPFMPhi  = lMPhi; 
    double pPFMPhi0 = lMPhi; 
    double pPFMPhi1 = lMPhi; 
    double pPFMPhi2 = lMPhi; 
    double pMet     = lMet;
    double pPFMet0  = lMet;   
    double pPFMet1  = lMet;  
    double pPFMet2  = lMet;  
    double pPFMet3  = lMet;  
    lFGenPt   = lEvent.vpt;
    lFGenPhi  = lEvent.vphi;
    lFLepPt   = lEvent.pt;  
    lFLepPhi  = lEvent.phi;
    lFLepPt1  = lEvent.lpt1;  
    lFLepPt2  = lEvent.lpt2;  
    lFWeight  = lEvent.weight;
    lFMass    = lEvent.mass;
    lFSVFMass = lEvent.svfmass;
    lFNJet    = lEvent.njets;
    lFPVis    = lEvent.pvis;
    if(iNJet != lFNJet && iNJet != -1) continue;
    lMMet0->Fill(pMet,lFWeight);
    for(int i1 = 0; i1 < lN; i1++) { 
      double pU1 = 0; double pU2 = 0; double pU21 = 0; double pU22 = 0; double pU11 = 0; double pU12 = 0;
      corrector1.CorrectAll  (pPFMet0,pPFMPhi0,lFGenPt,lFGenPhi,lFLepPt,lFLepPhi, pU1, pU2, 0,lFNJet);
      corrector1.CorrectType1(pPFMet1,pPFMPhi1,lFGenPt,lFGenPhi,lFLepPt,lFLepPhi,0,lFNJet);              //Tau Group
      corrector1.CorrectType2(pPFMet2,pPFMPhi2,lFGenPt,lFGenPhi,lFLepPt,lFLepPhi,0,lFNJet);              //Tau Group
      lFMet     = lEvent.met;
      lFMPhi    = lEvent.metphi;
      lFMet0    = pPFMet0;
      lFU1      = pU1;
      lFU2      = pU2;
      lFMPhi0   = pPFMPhi0;
      lFMet1    = pPFMet1;
      lFMPhi1   = pPFMPhi1;
      lFMet2    = pPFMet2;
      lFMPhi2   = pPFMPhi2;
      lFMet3    = lEvent.rawmet;
      lFMPhi3   = lEvent.rawmetphi;
      lFMt      = mT(lEvent.lpt1,lFMet ,lEvent.lphi1,lFMPhi);
      lFMt0     = mT(lEvent.lpt1,lFMet0,lEvent.lphi1,lFMPhi0);
      lFMt1     = mT(lEvent.lpt1,lFMet1,lEvent.lphi1,lFMPhi1);
      lFMt2     = mT(lEvent.lpt1,lFMet2,lEvent.lphi1,lFMPhi2);
      lFMt3     = mT(lEvent.lpt1,lFMet3,lEvent.lphi1,lFMPhi3);
      lFPMet    = pZeta(lEvent.lphi1,lEvent.lphi2,lFMet, lFMPhi);
      lFPMet0   = pZeta(lEvent.lphi1,lEvent.lphi2,lFMet0,lFMPhi0);
      lFPMet1   = pZeta(lEvent.lphi1,lEvent.lphi2,lFMet1,lFMPhi1);
      lFPMet2   = pZeta(lEvent.lphi1,lEvent.lphi2,lFMet2,lFMPhi2);
      lFPMet3   = pZeta(lEvent.lphi1,lEvent.lphi2,lFMet3,lFMPhi3);
      lMMet1->Fill(pPFMet0,lFWeight);
      lMMet2->Fill(pPFMet1,lFWeight);
      lMMet3->Fill(pPFMet2,lFWeight);
      lTree->Fill();
    }
  }
  lTree->Write();

  TCanvas *lC0 = new TCanvas("c0","c0",800,600); lC0->cd(); lC0->SetLogy();
  TLegend *lL = new TLegend(0.6,0.6,0.9,0.9); lL->SetFillColor(0); lL->SetBorderSize(0);
  lL->AddEntry(lMMet0,"MC","l");       lMMet0->SetMarkerStyle(kFullCircle); lMMet0->SetMarkerColor(kRed);
  lL->AddEntry(lMMet1,"MC corrected","lp"); 
  lL->AddEntry(lMMet2,"Correction Type1","lp"); 
  lL->AddEntry(lMMet3,"Correction Type2","lp"); 
  std::cout << "=====Int0 ===> " << lMMet0->Integral() << std::endl;
  std::cout << "=====Int1 ===> " << lMMet1->Integral() << std::endl;
  std::cout << "=====Int2 ===> " << lMMet2->Integral() << std::endl;
  std::cout << "=====Int3 ===> " << lMMet3->Integral() << std::endl;

  lMMet0->Scale(1./lMMet0->Integral());
  lMMet1->Scale(1./lMMet1->Integral());
  lMMet2->Scale(1./lMMet2->Integral());
  lMMet3->Scale(1./lMMet3->Integral());

  lMMet0->Draw(" hist");
  lMMet1->Draw(" hist sames");
  lMMet2->Draw(" hist sames");
  lMMet3->Draw(" hist sames");
  lL->Draw();
}
