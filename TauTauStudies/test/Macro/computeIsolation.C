
#include <iostream> 
#include <map>
#include <string>

#include "TChain.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TCut.h"
#include "TGraph.h"


void plot(bool sgn_ = true, bool doROC_ = false, int steps_ = 50){


  TCanvas *c1 = new TCanvas("c1","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);

  TLegend* leg = new TLegend(0.55,0.47,0.85,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);

  TFile *fSgn = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleDYJets-Mu-50-madgraph-PUS1_Open_MuTauStream.root","READ");
  TFile *fBkg = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleQCDmu_Open_MuTauStream.root","READ");

  if((fSgn->IsZombie()) || (fBkg->IsZombie()) ){
    cout << "No file" << endl;
    exit(1);
  }

  float vx_combRelIsoLeg1Raw[steps_];
  float vy_combRelIsoLeg1Raw[steps_];
  float vx_combRelIsoLeg1[steps_];
  float vy_combRelIsoLeg1[steps_];
  float vx_combRelIsoLeg1Beta[steps_];
  float vy_combRelIsoLeg1Beta[steps_];
  float vx_combRelIsoLeg1DBeta[steps_];
  float vy_combRelIsoLeg1DBeta[steps_];
  float vx_combRelIsoLeg1Rho[steps_];
  float vy_combRelIsoLeg1Rho[steps_];

  TH2F* hMaster = new TH2F("hMaster"," ; signal efficiency ; background efficiency", 100,0,1,100,0,1);

  TTree* treeSgn = (TTree*)fSgn->Get("outTreePtOrd");
  TTree* treeBkg = (TTree*)fBkg->Get("outTreePtOrd");

  TFile* dummy = new TFile("dummy.root","RECREATE");
  //TTree* treeSgnCut = (TTree*)treeSgn->CopyTree("isTauLegMatched && pt1>25 && pt2>20 && eta1*eta2<0");  
  //TTree* treeBkgCut = (TTree*)treeBkg->CopyTree("tightestHPSWP>-1 && pt1>25 && pt2>20 && eta1*eta2<0");  
  TTree* treeSgnCut = (TTree*)treeSgn->CopyTree("isTauLegMatched");  
  TTree* treeBkgCut = (TTree*)treeBkg->CopyTree("tightestHPSWP>0");  


  cout << treeSgnCut->GetEntries() << endl;
  cout << treeBkgCut->GetEntries() << endl;

  TH1F* SgnEffVsNvtx = new TH1F("SgnEffVsNvtx","; # reco-vtx; efficiency for CombRelIso<0.1",16,1,17);
  TH1F* BkgEffVsNvtx = new TH1F("BkgEffVsNvtx","; # reco-vtx; efficiency for CombRelIso<0.1",16,1,17);

  std::vector<string> isoDep;
  isoDep.push_back("combRelIsoLeg1");
  isoDep.push_back("combRelIsoLeg1Beta");
  isoDep.push_back("combRelIsoLeg1DBeta");
  isoDep.push_back("combRelIsoLeg1Rho");
  isoDep.push_back("combRelIsoLeg1Raw");

  std::map<std::string, TH1F*> aMapSgn;
  aMapSgn["combRelIsoLeg1Raw"] = (TH1F*)SgnEffVsNvtx->Clone("combRelIsoLeg1Raw");
  aMapSgn["combRelIsoLeg1"] = (TH1F*)SgnEffVsNvtx->Clone("combRelIsoLeg1");
  aMapSgn["combRelIsoLeg1Beta"] = (TH1F*)SgnEffVsNvtx->Clone("combRelIsoLeg1Beta");
  aMapSgn["combRelIsoLeg1DBeta"] = (TH1F*)SgnEffVsNvtx->Clone("combRelIsoLeg1DBeta");
  aMapSgn["combRelIsoLeg1Rho"] = (TH1F*)SgnEffVsNvtx->Clone("combRelIsoLeg1Rho");
  std::map<std::string, TH1F*> aMapBkg;
  aMapBkg["combRelIsoLeg1Raw"] = (TH1F*)BkgEffVsNvtx->Clone("combRelIsoLeg1Raw");
  aMapBkg["combRelIsoLeg1"] = (TH1F*)BkgEffVsNvtx->Clone("combRelIsoLeg1");
  aMapBkg["combRelIsoLeg1Beta"] = (TH1F*)BkgEffVsNvtx->Clone("combRelIsoLeg1Beta");
  aMapBkg["combRelIsoLeg1DBeta"] = (TH1F*)BkgEffVsNvtx->Clone("combRelIsoLeg1DBeta");
  aMapBkg["combRelIsoLeg1Rho"] = (TH1F*)BkgEffVsNvtx->Clone("combRelIsoLeg1Rho");

  TH1F* h1 = new TH1F("h1","",1,-10,10);

  if(doROC_){
    for(unsigned int m = 0; m < isoDep.size() ; m++){
    
      treeSgnCut->Draw( "eta1>>h1" ,"puWeight");
      float sgnDen = (float)h1->Integral();
      cout << sgnDen << endl;
      h1->Reset();
      treeBkgCut->Draw( "eta1>>h1" ,"puWeight");
      float bkgDen = (float)h1->Integral();
      cout << bkgDen << endl;
      h1->Reset();
      
      for(int i = 0; i < steps_; i++){
	
      cout << isoDep[m] << " --> " << i << endl;
      
      TCut cut(Form("puWeight*(%s<=%f)", isoDep[m].c_str(), i*(1./steps_)) );
      treeSgnCut->Draw( "eta1>>h1" ,cut );
      float sgnNum = (float)h1->Integral();
      cout << sgnNum << endl;
      h1->Reset();
      treeBkgCut->Draw( "eta1>>h1" ,cut );
      float bkgNum = (float)h1->Integral();
      cout << bkgNum << endl;
      h1->Reset();
 
      if(m==0){
	vx_combRelIsoLeg1[i] = sgnNum/sgnDen;
	vy_combRelIsoLeg1[i] = bkgNum/bkgDen;
      }
      if(m==1){
	vx_combRelIsoLeg1Beta[i] = sgnNum/sgnDen;
	vy_combRelIsoLeg1Beta[i] = bkgNum/bkgDen;
      }
      if(m==2){
	vx_combRelIsoLeg1DBeta[i] = sgnNum/sgnDen;
	vy_combRelIsoLeg1DBeta[i] = bkgNum/bkgDen;
      }
      if(m==3){
	vx_combRelIsoLeg1Rho[i] = sgnNum/sgnDen;
	vy_combRelIsoLeg1Rho[i] = bkgNum/bkgDen;
      }
      if(m==4){
	vx_combRelIsoLeg1Raw[i] = sgnNum/sgnDen;
	vy_combRelIsoLeg1Raw[i] = bkgNum/bkgDen;
      }
      }
    }
  }

  if(!doROC_){

    for(unsigned int m = 0; m < isoDep.size() ; m++){
      for(int i = 0; i<16; i++){

	float number = i+1;
	TCut cutNum(Form("%s<0.1 && numPV<(%f+0.5) && numPV>(%f-0.5)", isoDep[m].c_str(), number, number));
	TCut cutDen(Form("numPV<(%f+0.5) && numPV>(%f-0.5)", number, number));
	//TCut totCut( "" );
	
	//float tot = (float)treeSgn->GetEntries( totCut );
	float num = (float)treeSgnCut->GetEntries( cutNum );
	float den = (float)treeSgnCut->GetEntries( cutDen );
	
	aMapSgn[isoDep[m]]->SetBinContent(i+1, num/den );
	aMapSgn[isoDep[m]]->SetBinError(i+1, TMath::Sqrt(num/den*(1-num/den) /  den) );
	
	num = treeBkgCut->GetEntries( cutNum );
	den = treeBkgCut->GetEntries( cutDen );
	
	aMapBkg[isoDep[m]]->SetBinContent(i+1, num/den );
	aMapSgn[isoDep[m]]->SetBinError(i+1, TMath::Sqrt(num/den*(1-num/den) /  den) );
	
      }
      
    } 
  }

  if(sgn_ && !doROC_){

    leg->SetHeader("Z#rightarrow#tau#tau");

    for(unsigned int k = 0; k < isoDep.size() ; k++){
      if(k==0){
	aMapSgn[isoDep[k]]->SetMarkerColor(kBlack);
	aMapSgn[isoDep[k]]->SetMarkerSize(1.4);
	aMapSgn[isoDep[k]]->SetMarkerStyle(kFullCircle);
	aMapSgn[isoDep[k]]->GetYaxis()->SetRangeUser(0.50,1.1);
	leg->AddEntry(aMapSgn[isoDep[k]],"pfNoPileUp","P");
	aMapSgn[isoDep[k]]->Draw("P");
      }
      if(k==1){
	aMapSgn[isoDep[k]]->SetMarkerColor(kRed);
	aMapSgn[isoDep[k]]->SetMarkerSize(1.4);
	aMapSgn[isoDep[k]]->SetMarkerStyle(kFullSquare);
	leg->AddEntry(aMapSgn[isoDep[k]],"Beta","P");
	aMapSgn[isoDep[k]]->Draw("PSAME");
      }
      if(k==2){
	aMapSgn[isoDep[k]]->SetMarkerColor(kBlue);
	aMapSgn[isoDep[k]]->SetMarkerSize(1.4);
	aMapSgn[isoDep[k]]->SetMarkerStyle(kFullTriangleUp);
	leg->AddEntry(aMapSgn[isoDep[k]],"Delta Beta","P");
	aMapSgn[isoDep[k]]->Draw("PSAME");
      }
      if(k==3){
	aMapSgn[isoDep[k]]->SetMarkerColor(kGreen);
	aMapSgn[isoDep[k]]->SetMarkerSize(1.4);
	aMapSgn[isoDep[k]]->SetMarkerStyle(kFullTriangleDown);
	leg->AddEntry(aMapSgn[isoDep[k]],"Rho","P");
	aMapSgn[isoDep[k]]->Draw("PSAME");
      }
      if(k==4){
	aMapSgn[isoDep[k]]->SetMarkerColor(kMagenta);
	aMapSgn[isoDep[k]]->SetMarkerSize(1.4);
	aMapSgn[isoDep[k]]->SetMarkerStyle(kFullStar);
	leg->AddEntry(aMapSgn[isoDep[k]],"No correction","P");
	aMapSgn[isoDep[k]]->Draw("PSAME");
      }
    }
  }
  else if(!sgn_ && !doROC_){
    
    leg->SetHeader("QCD #mu-enriched");

    for(unsigned int k = 0; k < isoDep.size() ; k++){
      if(k==0){
	aMapBkg[isoDep[k]]->SetMarkerColor(kBlack);
	aMapSgn[isoDep[k]]->SetMarkerSize(1.4);
	aMapBkg[isoDep[k]]->SetMarkerStyle(kFullCircle);
	aMapSgn[isoDep[k]]->GetYaxis()->SetRangeUser(0.0,0.10);
	leg->AddEntry(aMapBkg[isoDep[k]],"pfNoPileUp","P");
	aMapBkg[isoDep[k]]->Draw("P");
      }
      if(k==1){
	aMapBkg[isoDep[k]]->SetMarkerColor(kRed);
	aMapSgn[isoDep[k]]->SetMarkerSize(1.4);
	aMapBkg[isoDep[k]]->SetMarkerStyle(kFullSquare);
	leg->AddEntry(aMapBkg[isoDep[k]],"Beta","P");
	aMapBkg[isoDep[k]]->Draw("PSAME");
      }
      if(k==2){
	aMapBkg[isoDep[k]]->SetMarkerColor(kBlue);
	aMapSgn[isoDep[k]]->SetMarkerSize(1.4);
	aMapBkg[isoDep[k]]->SetMarkerStyle(kFullTriangleUp);
	leg->AddEntry(aMapBkg[isoDep[k]],"Delta Beta","P");
	aMapBkg[isoDep[k]]->Draw("PSAME");
      }
      if(k==3){
	aMapBkg[isoDep[k]]->SetMarkerColor(kGreen);
	aMapSgn[isoDep[k]]->SetMarkerSize(1.4);
	aMapBkg[isoDep[k]]->SetMarkerStyle(kFullTriangleDown);
	leg->AddEntry(aMapBkg[isoDep[k]],"Rho","P");
	aMapBkg[isoDep[k]]->Draw("PSAME");
      }
      if(k==4){
	aMapBkg[isoDep[k]]->SetMarkerColor(kMagenta);
	aMapBkg[isoDep[k]]->SetMarkerSize(1.4);
	aMapBkg[isoDep[k]]->SetMarkerStyle(kFullStar);
	leg->AddEntry(aMapBkg[isoDep[k]],"No correction","P");
	aMapBkg[isoDep[k]]->Draw("PSAME");
      }
    }
  } else if(doROC_){

    leg->SetHeader("ROC");

    hMaster->Draw();

    TGraph* graph_combRelIsoLeg1Raw = new TGraph(steps_,vx_combRelIsoLeg1Raw,vy_combRelIsoLeg1Raw);
    TGraph* graph_combRelIsoLeg1 = new TGraph(steps_,vx_combRelIsoLeg1,vy_combRelIsoLeg1);
    TGraph* graph_combRelIsoLeg1Beta = new TGraph(steps_,vx_combRelIsoLeg1Beta,vy_combRelIsoLeg1Beta);
    TGraph* graph_combRelIsoLeg1DBeta = new TGraph(steps_,vx_combRelIsoLeg1DBeta,vy_combRelIsoLeg1DBeta);
    TGraph* graph_combRelIsoLeg1Rho = new TGraph(steps_,vx_combRelIsoLeg1Rho,vy_combRelIsoLeg1Rho);
    
    std::map<std::string, TGraph*> roc;
    roc["combRelIsoLeg1Raw"] = graph_combRelIsoLeg1Raw;
    roc["combRelIsoLeg1"] = graph_combRelIsoLeg1;
    roc["combRelIsoLeg1Beta"] = graph_combRelIsoLeg1Beta;
    roc["combRelIsoLeg1DBeta"] = graph_combRelIsoLeg1DBeta;
    roc["combRelIsoLeg1Rho"] = graph_combRelIsoLeg1Rho;


    for(unsigned int k = 0; k < isoDep.size() ; k++){
      if(k==0){
	roc[isoDep[k]]->SetMarkerColor(kBlack);
	roc[isoDep[k]]->SetMarkerSize(1.4);
	roc[isoDep[k]]->SetMarkerStyle(kFullCircle);
	roc[isoDep[k]]->GetYaxis()->SetRangeUser(0.50,1.1);
	leg->AddEntry(roc[isoDep[k]],"pfNoPileUp","P");
	roc[isoDep[k]]->Draw("P");
      }
      if(k==1){
	roc[isoDep[k]]->SetMarkerColor(kRed);
	roc[isoDep[k]]->SetMarkerSize(1.4);
	roc[isoDep[k]]->SetMarkerStyle(kFullSquare);
	leg->AddEntry(roc[isoDep[k]],"Beta","P");
	roc[isoDep[k]]->Draw("PSAME");
      }
      if(k==2){
	roc[isoDep[k]]->SetMarkerColor(kBlue);
	roc[isoDep[k]]->SetMarkerSize(1.4);
	roc[isoDep[k]]->SetMarkerStyle(kFullTriangleUp);
	leg->AddEntry(roc[isoDep[k]],"Delta Beta","P");
	roc[isoDep[k]]->Draw("PSAME");
      }
      if(k==3){
	roc[isoDep[k]]->SetMarkerColor(kGreen);
	roc[isoDep[k]]->SetMarkerSize(1.4);
	roc[isoDep[k]]->SetMarkerStyle(kFullTriangleDown);
	leg->AddEntry(roc[isoDep[k]],"Rho","P");
	roc[isoDep[k]]->Draw("PSAME");
      }
      if(k==4){
	roc[isoDep[k]]->SetMarkerColor(kMagenta);
	roc[isoDep[k]]->SetMarkerSize(1.4);
	roc[isoDep[k]]->SetMarkerStyle(kFullStar);
	leg->AddEntry(roc[isoDep[k]],"No correction","P");
	roc[isoDep[k]]->Draw("PSAME");
      }
    }
  }

  leg->Draw();



}
