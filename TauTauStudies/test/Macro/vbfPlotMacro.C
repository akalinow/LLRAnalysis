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

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"

#include <vector>
#include <utility>
#include <map>

#define SAVE true

using namespace ROOT::Math;
using namespace std;

namespace User{
  struct moreStruct {
    bool operator() (const double& lhs, const double& rhs) const
    {return lhs>rhs;}
  };
}

void plot(){

  typedef std::map<double, ROOT::Math::XYZTVector , User::moreStruct>::iterator CImap;

  TFile* file   = new TFile("./vbfTree.root","READ");

  TCanvas *c1 = new TCanvas("c1Mass","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);

  TPad* pad1 = new TPad("pad1","",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad("pad2","",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(1);

  TLegend* leg = new TLegend(0.55,0.45,0.85,0.75,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader("#splitline{POWHEG+PYTHIA qqH(115)#rightarrow#tau#tau}{jets matched to tag partons}");

  TTree* currentTree = (TTree*)file->Get("vbfJetAnalyzer/tree");
  int nEntries = currentTree->GetEntries() ;

  TH1F* h_TagMult = new TH1F("h_TagMult"," ; multiplicity ; a.u. ", 6,-0.5,5.5);

  TH1F* h_ptTag1  = new TH1F("h_ptTag1","; p_{T} (GeV/c); a.u. ", 60,0,300);
  TH1F* h_ptTag2  = new TH1F("h_ptTag2","; p_{T} (GeV/c); a.u. ", 60,0,300);
  TH1F* h_ptTag3  = new TH1F("h_ptTag3","; p_{T} (GeV/c); a.u. ", 60,0,300);

  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets;
  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* tagjets;
  currentTree->SetBranchAddress("jetsP4",   &jets);
  currentTree->SetBranchAddress("tagjetsP4",&tagjets);


  for (int n = 0; n < nEntries ; n++) {

    currentTree->GetEntry(n);

    std::map<double, ROOT::Math::XYZTVector , User::moreStruct> sortedTagJets;
    for(unsigned int i = 0; i < tagjets->size(); i++){
      sortedTagJets.insert( make_pair( (*tagjets)[i].Et(),(*tagjets)[i] )  ) ;
    }
    std::map<double, ROOT::Math::XYZTVector , User::moreStruct> sortedJets;
    for(unsigned int i = 0; i < jets->size(); i++){
      sortedJets.insert( make_pair( (*jets)[i].Et(),(*jets)[i] )  ) ;
    }

    h_TagMult->Fill(tagjets->size());

    int counter=0;
    for(CImap it = sortedTagJets.begin(); 
	it!=sortedTagJets.end(); it++){
      if( counter==0 ) h_ptTag1->Fill( (it->second).Et() );
      if( counter==1 ) h_ptTag2->Fill( (it->second).Et() );
      if( counter==2 ) h_ptTag3->Fill( (it->second).Et() );
      counter++;
    }
   
  }

  h_ptTag1->SetLineColor(kRed);
  h_ptTag1->SetLineStyle(kSolid);
  h_ptTag2->SetLineColor(kBlue);
  h_ptTag2->SetLineStyle(kDashed);
  h_ptTag2->SetLineColor(kMagenta);
  h_ptTag2->SetLineStyle(kDotted);

  cout << "1st jet " << h_ptTag1->GetEntries() << endl;
  cout << "2nd jet " << h_ptTag2->GetEntries() << endl;
  cout << "3rd jet " << h_ptTag3->GetEntries() << endl;

  h_ptTag3->Draw("HISTS");
  h_ptTag1->Draw("HISTSAME");
  h_ptTag2->Draw("HISTSAME");

  leg->AddEntry(h_ptTag1,"1st largest-p_{T} jet","L");
  leg->AddEntry(h_ptTag2,"2nd largest-p_{T} jet","L");
  leg->AddEntry(h_ptTag3,"3rd largest-p_{T} jet","L");

  leg->Draw();

  pad2->cd();
  TH1F* hLow = (TH1F*)h_TagMult->Clone();
  hLow->SetLabelSize(0.12,"X");
  hLow->SetLabelSize(0.10,"Y");
  hLow->SetTitleSize(0.12,"Y");
  hLow->SetTitleSize(0.12,"X");
  hLow->SetTitleOffset(0.36,"Y");
  hLow->SetTitleOffset(0.36,"X");
  hLow->Draw("HIST");

}



void plot2(){

  typedef std::map<double, ROOT::Math::XYZTVector , User::moreStruct>::iterator CImap;

  TFile* file   = new TFile("../vbfTree.root","READ");

  TCanvas *c1 = new TCanvas("c1Mass","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TLegend* leg = new TLegend(0.55,0.45,0.85,0.75,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader("#splitline{POWHEG+PYTHIA qqH(115)#rightarrow#tau#tau}{jets matched to tag partons}");

  TTree* currentTree = (TTree*)file->Get("vbfJetAnalyzer/tree");
  int nEntries = currentTree->GetEntries() ;

  TH2F* h2  = new TH2F("h2","; #Delta#eta_{j1,j2}; min_{i=1,2}#Delta#eta_{ji,j3} ", 80,0,8,40,0,8);
  
  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets;
  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* tagjets;
  currentTree->SetBranchAddress("jetsP4",   &jets);
  currentTree->SetBranchAddress("tagjetsP4",&tagjets);


  for (int n = 0; n < nEntries ; n++) {

    currentTree->GetEntry(n);

    std::map<double, ROOT::Math::XYZTVector , User::moreStruct> sortedTagJets;
    for(unsigned int i = 0; i < tagjets->size(); i++){
      sortedTagJets.insert( make_pair( (*tagjets)[i].Et(),(*tagjets)[i] )  ) ;
    }
    std::map<double, ROOT::Math::XYZTVector , User::moreStruct> sortedJets;
    for(unsigned int i = 0; i < jets->size(); i++){
      sortedJets.insert( make_pair( (*jets)[i].Et(),(*jets)[i] )  ) ;
    }

    if(tagjets->size()<3) continue;

    int counter = 0;
    float eta1,eta2,eta3;

    for(CImap it = sortedTagJets.begin(); 
	it!=sortedTagJets.end(); it++){
      if(counter==0) eta1 = (it->second).Eta();
      if(counter==1) eta2 = (it->second).Eta();
      if(counter==2) eta3 = (it->second).Eta();
      counter++;
    }

    float deta12 = abs(eta1-eta2);
    float detai3 = std::min( abs(eta1-eta3),
			     abs(eta2-eta3)  );
    h2->Fill(deta12,detai3);
   
  }

  h2->Draw("COLZ");


  leg->Draw();


}

/*

void plot3(){

  typedef std::map<double, ROOT::Math::XYZTVector , User::moreStruct>::iterator CImap;

  TFile* file   = new TFile("../vbfTree.root","READ");

  TCanvas *c1 = new TCanvas("c1Mass","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);

  TPad* pad1 = new TPad("pad1","",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad("pad2","",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(1);

  TLegend* leg = new TLegend(0.55,0.45,0.85,0.75,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader("#splitline{POWHEG+PYTHIA qqH(115)#rightarrow#tau#tau}{jets matched to tag partons}");

  TTree* currentTree = (TTree*)file->Get("vbfJetAnalyzer/tree");
  int nEntries = currentTree->GetEntries() ;

  TH1F* h_TagMult = new TH1F("h_TagMult"," ; multiplicity ; a.u. ", 6,-0.5,5.5);

  TH1F* h_ptTag1  = new TH1F("h_ptTag1","; p_{T} (GeV/c); a.u. ", 150,0,300);
  TH1F* h_ptTag2  = new TH1F("h_ptTag2","; p_{T} (GeV/c); a.u. ", 150,0,300);
  TH1F* h_ptTag3  = new TH1F("h_ptTag3","; p_{T} (GeV/c); a.u. ", 150,0,300);

  TH2F* h2  = new TH2F("h2","; p_{T}_{j1} (GeV/c);  p_{T}_{j2} (GeV/c) ", 150,0,300,150,0,300);

  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets;
  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* tagjets;
  currentTree->SetBranchAddress("jetsP4",   &jets);
  currentTree->SetBranchAddress("tagjetsP4",&tagjets);


  for (int n = 0; n < nEntries ; n++) {

    currentTree->GetEntry(n);

    std::map<double, ROOT::Math::XYZTVector , User::moreStruct> sortedTagJets;
    for(unsigned int i = 0; i < tagjets->size(); i++){
      sortedTagJets.insert( make_pair( (*tagjets)[i].Et(),(*tagjets)[i] )  ) ;
    }
    std::map<double, ROOT::Math::XYZTVector , User::moreStruct> sortedJets;
    for(unsigned int i = 0; i < jets->size(); i++){
      sortedJets.insert( make_pair( (*jets)[i].Et(),(*jets)[i] )  ) ;
    }

    h_TagMult->Fill(tagjets->size());

    int counter=0;
    float et1,et2;
    for(CImap it = sortedTagJets.begin(); 
	it!=sortedTagJets.end(); it++){
      if( counter==0 ){
	h_ptTag1->Fill( (it->second).Et() );
	et1 = (it->second).Et();
      }
      if( counter==1 ){
	h_ptTag2->Fill( (it->second).Et() );
	et2 = (it->second).Et();
      }
      if( counter==2 ) h_ptTag3->Fill( (it->second).Et() );
      counter++;
    }
    if(tagjets->size()>1) h2->Fill(et1,et2);
  }

  for(int m = 5; m<=15; m++){
    for(int n = 5; n<=15; n++){
      cout<< "Et>" << m*2 << ", "<< n*2 <<" GeV ==>" << h2->Integral(m,150,n,150)/h2->GetEntries() << endl;
    }
  }
  
  h2->Draw("COLZ");
  leg->Draw();
  
}


*/
