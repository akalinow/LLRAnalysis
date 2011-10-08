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

#define SAVE false

using namespace ROOT::Math;
using namespace std;

typedef std::vector< pair<TFile*, pair<std::string,float> > > FileList; 

void plotDEta(  FileList fileList_  , float Lumi_ = 30 ){

  TCanvas *c1 = new TCanvas("c1DEta","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad("pad1DEta","",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad("pad2DEta","",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(1);

  TLegend* leg = new TLegend(0.60,0.55,0.85,0.88,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader("N^{jet}>1, p^{j1,j2}_{T}>30 GeV");
  
  THStack* aStack = new THStack("aStack",Form("CMS Preliminary 2010   #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_));
  TH1F* hData = new TH1F();
  TH1F* hSiml = new TH1F();

  for(unsigned int i = 0 ; i < fileList_.size() ; i++){

    TFile* currentFile = (TFile*)fileList_[i].first ;
    if( currentFile->IsZombie() ) continue;
    TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    TTree* currentTree = (TTree*)currentFile->Get("zPlusJetsAnalyzer/tree");
    string h1Name = "h1_"+(fileList_[i].second).first;
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", 16 ,0 , 8);

    if( ((fileList_[i].second).first).find("Zjets")!=string::npos ) {
      h1->SetFillColor(kRed);
      leg->AddEntry(h1,"MadGraph Z+jets","F");
    }
    if( ((fileList_[i].second).first).find("ttbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"MadGraph t#bar{t}+jets","F");
    }
    if( ((fileList_[i].second).first).find("Wjets")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"MadGraph W+jets","F");
    }
    if( ((fileList_[i].second).first).find("WW")!=string::npos ) {
      h1->SetFillColor(42);
      leg->AddEntry(h1,"PYTHIA WW","F");
    }
    if( ((fileList_[i].second).first).find("WZ")!=string::npos ) {
      h1->SetFillColor(43);
      leg->AddEntry(h1,"PYTHIA WZ","F");
    }
    if( ((fileList_[i].second).first).find("tW")!=string::npos ){
      h1->SetFillColor(kYellow);
      leg->AddEntry(h1,"MadGraph single-t","F");
    }
    if( ((fileList_[i].second).first).find("QCD")!=string::npos ) {
      h1->SetFillColor(kBlack);
      leg->AddEntry(h1,"PYTHIA QCD","F");
    }
    if( ((fileList_[i].second).first).find("data")!=string::npos ) {
      h1->SetMarkerColor(kBlack);
      h1->SetMarkerStyle(kFullCircle);
      h1->SetMarkerSize(0.8);
      leg->AddEntry(h1,"DATA","lep");
    }

    currentTree->Draw( Form("abs(jetsIDP4[0].eta()-jetsIDP4[1].eta())>>%s",h1Name.c_str()), 
		       "jetsIDP4@.size()>1 && jetsIDP4[1].pt()>30" );

    if(((fileList_[i].second).first).find("data")!=string::npos){
      hData = h1;
      hData->Sumw2();
      continue;
    }

    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) );
   
    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }

  aStack->Draw("HIST");
  hData->Draw("PSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("|#Delta #eta_{j1,j2}|");
  hStack->SetYTitle(Form(" Events/(%.1f units)", hStack->GetBinWidth(1) ) );
  hStack->SetTitleSize(0.04,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.75,"Y");
  leg->Draw();

  pad2->cd();
  TH1F* hRatio = new TH1F("hRatio", " ; ; #frac{(DATA-MC)}{MC}",
			  hStack->GetNbinsX(), 
			  hStack->GetXaxis()->GetXmin(), hStack->GetXaxis()->GetXmax());
  hRatio->SetMarkerStyle(kFullCircle);
  hRatio->SetMarkerSize(0.8);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  TH1F* hDataClone = (TH1F*)hData->Clone("hDataClone");
  hDataClone->Add(hSiml,-1);
  hRatio->Divide( hDataClone ,hSiml,1.0,1.0);
  hRatio->SetAxisRange(-3,3,"Y");
  hRatio->Draw();

  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");


  if(SAVE) c1->SaveAs("Zmm_DEta.png");

}


void plotMass(FileList fileList,  float Lumi_ = 30 , float cutOff_ = 30){

  for(int i = 0; i<fileList.size(); i++){
    (fileList[i].first)->Close();
    delete (fileList[i].first);
  }
  
  TFile* fData   = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/treeZmumuPlusJets_Mu-Run2010AB.root","READ");
  TFile* fDYJets = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/treeZmumuPlusJets_DYJets-madgraph-50-PU-v2.root","READ");
  TFile* fTT     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/treeZmumuPlusJets_TT-madgraph-PU-v2.root","READ");
  TFile* fWJets  = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/treeZmumuPlusJets_WJets-madgraph-PU-v2.root","READ");
  TFile* fT      = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/treeZmumuPlusJets_TToBLNu-tW-madhraph-PU-v2.root","READ");
  TFile* fQCD    = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/treeZmumuPlusJets_QCD-pythia-PU-v2.root","READ");

  FileList fileList_;
  fileList_.push_back( make_pair(fQCD,    make_pair("QCD",   349988.0 )  ));
  fileList_.push_back( make_pair(fWJets,  make_pair("Wjets",  31314.0 )  ));
  fileList_.push_back( make_pair(fT,      make_pair("tW",        10.6 )  ));
  fileList_.push_back( make_pair(fTT,     make_pair("ttbar",    157.5 )  ));
  fileList_.push_back( make_pair(fDYJets, make_pair("Zjets",   3048.0 )  ));
  fileList_.push_back( make_pair(fData,   make_pair("data",       -99 )  ));

  TCanvas *c1 = new TCanvas("c1Mass","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad("pad1Mass","",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad("pad2Mass","",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(1);

  TLegend* leg = new TLegend(0.75,0.70,0.97,0.95,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader(Form("p^{j1,j2}_{T}>%.0f GeV",cutOff_));
  
  THStack* aStack = new THStack("aStack",Form("CMS Preliminary    #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_));
  TH1F* hData = new TH1F();
  TH1F* hSiml = new TH1F();

  for(unsigned int i = 0 ; i < fileList_.size() ; i++){

    TFile* currentFile = (TFile*)fileList_[i].first ;
    if( currentFile->IsZombie() ) continue;
    TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    TTree* currentTree = (TTree*)currentFile->Get("zPlusJetsAnalyzer/tree");
    string h1Name = "h1_"+(fileList_[i].second).first;
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", 50 ,0 , 500);

    int nEntries = currentTree->GetEntries() ;

    if( ((fileList_[i].second).first).find("Zjets")!=string::npos ) {
      h1->SetFillColor(kRed);
      leg->AddEntry(h1,"MadGraph Z+jets","F");
    }
    if( ((fileList_[i].second).first).find("ttbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"MadGraph t#bar{t}+jets","F");
    }
    if( ((fileList_[i].second).first).find("Wjets")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"MadGraph W+jets","F");
    }
    if( ((fileList_[i].second).first).find("tW")!=string::npos ){
      h1->SetFillColor(kYellow);
      leg->AddEntry(h1,"MadGraph single-t","F");
    }
    if( ((fileList_[i].second).first).find("QCD")!=string::npos ) {
      h1->SetFillColor(kBlack);
      leg->AddEntry(h1,"PYTHIA QCD","F");
    }
    if( ((fileList_[i].second).first).find("data")!=string::npos ) {
      h1->SetMarkerColor(kBlack);
      h1->SetMarkerStyle(kFullCircle);
      h1->SetMarkerSize(0.8);
      leg->AddEntry(h1,"DATA","lep");
    }

    std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* pJets;
    currentTree->SetBranchAddress("jetsIDP4",&pJets);

    for (int n = 0; n < nEntries ; n++) {
      currentTree->GetEntry(n);
      XYZTVector q;
      if( pJets->size()>1 && (*pJets)[1].Pt()> cutOff_ ){
	q += (*pJets)[0];
	q += (*pJets)[1];
	h1->Fill(q.M());
      }
    }


    if(((fileList_[i].second).first).find("data")!=string::npos){
      hData = h1;
      hData->Sumw2();
      continue;
    }

    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) );
   
    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }

  aStack->Draw("HIST");
  hData->Draw("PSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("M_{j1,j2} (GeV/c^{2})");
  hStack->SetYTitle(Form(" Events/(%.0f GeV/c^{2})", hStack->GetBinWidth(1) ) );
  hStack->SetTitleSize(0.04,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.75,"Y");
  leg->Draw();

  pad2->cd();
  TH1F* hRatio = new TH1F("hRatio", " ; ; #frac{(DATA-MC)}{MC}",
			  hStack->GetNbinsX(), 
			  hStack->GetXaxis()->GetXmin(), hStack->GetXaxis()->GetXmax());
  hRatio->SetMarkerStyle(kFullCircle);
  hRatio->SetMarkerSize(0.8);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  TH1F* hDataClone = (TH1F*)hData->Clone("hDataClone");
  hDataClone->Add(hSiml,-1);
  hRatio->Divide( hDataClone ,hSiml,1.0,1.0);
  hRatio->SetAxisRange(-3,3,"Y");

  hRatio->Draw();
  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");

  if(SAVE) c1->SaveAs(Form("Zmm_Mass_%.0f.png",cutOff_));

}



void plotJetMultiplicity(  FileList fileList_  , float Lumi_ = 30 , float cutOff_ = 30.){

  TCanvas *c1 = new TCanvas("c1JetMultiplicity","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad("pad1JetMultiplicity","",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad("pad2JetMultiplicity","",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(1);

  TLegend* leg = new TLegend(0.60,0.50,0.85,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader( Form("E^{jet}_{T}>%.0f GeV/c",cutOff_) );
  
  THStack* aStack = new THStack("aStack",Form("CMS Preliminary 2010   #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_));
  TH1F* hData = new TH1F();
  TH1F* hSiml = new TH1F();

  for(unsigned int i = 0 ; i < fileList_.size() ; i++){

    TFile* currentFile = (TFile*)fileList_[i].first ;
    if( currentFile->IsZombie() ) continue;
    TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    TTree* currentTree = (TTree*)currentFile->Get("zPlusJetsAnalyzer/tree");
    string h1Name = "h1_"+(fileList_[i].second).first;
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", 7 ,-0.5, 6.5);

    if( ((fileList_[i].second).first).find("Zjets")!=string::npos ) {
      h1->SetFillColor(kRed);
      leg->AddEntry(h1,"MadGraph Z+jets","F");
    }
    if( ((fileList_[i].second).first).find("ttbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"MadGraph t#bar{t}+jets","F");
    }
    if( ((fileList_[i].second).first).find("Wjets")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"MadGraph W+jets","F");
    }
    if( ((fileList_[i].second).first).find("WW")!=string::npos ) {
      h1->SetFillColor(42);
      leg->AddEntry(h1,"PYTHIA WW","F");
    }
    if( ((fileList_[i].second).first).find("WZ")!=string::npos ) {
      h1->SetFillColor(43);
      leg->AddEntry(h1,"PYTHIA WZ","F");
    }
    if( ((fileList_[i].second).first).find("tW")!=string::npos ){
      h1->SetFillColor(kYellow);
      leg->AddEntry(h1,"MadGraph single-t","F");
    }
    if( ((fileList_[i].second).first).find("QCD")!=string::npos ) {
      h1->SetFillColor(kBlack);
      leg->AddEntry(h1,"PYTHIA QCD","F");
    }
    if( ((fileList_[i].second).first).find("data")!=string::npos ) {
      h1->SetMarkerColor(kBlack);
      h1->SetMarkerStyle(kFullCircle);
      h1->SetMarkerSize(0.8);
      leg->AddEntry(h1,"DATA","lep");
    }

    currentTree->Draw( Form("jetsIDP4@.size()>>%s",h1Name.c_str()), 
		       Form("jetsIDP4@.size()<1 || (jetsIDP4@.size()>0 && jetsIDP4[jetsIDP4@.size()-1].Et()>%f)",cutOff_) );

    if(((fileList_[i].second).first).find("data")!=string::npos){
      hData = h1;
      hData->Sumw2();
      continue;
    }

    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) );

    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }


  aStack->Draw("HIST");
  hData->Draw("PSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("Exclusive jet number");
  hStack->SetYTitle("Number of events");
  hStack->SetTitleSize(0.05,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.75,"Y");
  leg->Draw();

  pad2->cd();
  TH1F* hRatio = new TH1F("hRatio", " ; ; #frac{(DATA-MC)}{MC}",
			  hStack->GetNbinsX(), 
			  hStack->GetXaxis()->GetXmin(), hStack->GetXaxis()->GetXmax());
  hRatio->SetMarkerStyle(kFullCircle);
  hRatio->SetMarkerSize(0.8);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  TH1F* hDataClone = (TH1F*)hData->Clone("hDataClone");
  hDataClone->Add(hSiml,-1);
  hRatio->Divide( hDataClone ,hSiml,1.0,1.0);
  hRatio->SetAxisRange(-0.5,0.5,"Y");

  hRatio->Draw();
  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");

  if(SAVE) c1->SaveAs(Form("Zmm_JetMultiplicity_%.0f.png",cutOff_));

}


void plotBtag(  FileList fileList_  , float Lumi_ = 30 , float cutOff_ = 30.){

  TCanvas *c1 = new TCanvas("c1Btag","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad("pad1Btag","",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad("pad2Btag","",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(1);

  TLegend* leg = new TLegend(0.60,0.50,0.85,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader( Form("p^{jet}_{T}>%.0f GeV/c",cutOff_) );
  
  THStack* aStack = new THStack("aStack",Form(" CMS Preliminary    #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_));
  TH1F* hData = new TH1F();
  TH1F* hSiml = new TH1F();

  for(unsigned int i = 0 ; i < fileList_.size() ; i++){

    TFile* currentFile = (TFile*)fileList_[i].first ;
    if( currentFile->IsZombie() ) continue;
    TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    TTree* currentTree = (TTree*)currentFile->Get("zPlusJetsAnalyzer/tree");
    string h1Name = "h1_"+(fileList_[i].second).first;
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", 20 ,-20, 20);

    if( ((fileList_[i].second).first).find("Zjets")!=string::npos ) {
      h1->SetFillColor(kRed);
      leg->AddEntry(h1,"MadGraph Z+jets","F");
    }
    if( ((fileList_[i].second).first).find("ttbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"MadGraph t#bar{t}+jets","F");
    }
    if( ((fileList_[i].second).first).find("Wjets")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"MadGraph W+jets","F");
    }
    if( ((fileList_[i].second).first).find("tW")!=string::npos ){
      h1->SetFillColor(kYellow);
      leg->AddEntry(h1,"MadGraph single-t","F");
    }
    if( ((fileList_[i].second).first).find("QCD")!=string::npos ) {
      h1->SetFillColor(kBlack);
      leg->AddEntry(h1,"PYTHIA QCD","F");
    }
    if( ((fileList_[i].second).first).find("data")!=string::npos ) {
      h1->SetMarkerColor(kBlack);
      h1->SetMarkerStyle(kFullCircle);
      h1->SetMarkerSize(0.8);
      leg->AddEntry(h1,"DATA","lep");
    }

    currentTree->Draw( Form("jetsBtagHE>>%s",h1Name.c_str()), 
		       Form("jetsIDP4@.size()<1 || (jetsIDP4@.size()>0 && jetsIDP4[jetsIDP4@.size()-1].pt()>%f)",cutOff_) );


    if(((fileList_[i].second).first).find("data")!=string::npos){
      hData = h1;
      hData->Sumw2();
      h1->SetBinContent(1,h1->GetBinContent(0));
      h1->SetBinContent( h1->GetNbinsX() , h1->GetBinContent(h1->GetNbinsX()+1));
      continue;
    }
    
    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) );

    h1->SetBinContent(1,h1->GetBinContent(0));
    h1->SetBinContent( h1->GetNbinsX() , h1->GetBinContent(h1->GetNbinsX()+1));

    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }


  aStack->Draw("HIST");
  hData->Draw("PSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("TCHE output");
  hStack->SetYTitle("Number of events");
  hStack->SetTitleSize(0.05,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.75,"Y");
  leg->Draw();

  pad2->cd();
  TH1F* hRatio = new TH1F("hRatio", " ; ; #frac{(DATA-MC)}{MC}",
			  hStack->GetNbinsX(), 
			  hStack->GetXaxis()->GetXmin(), hStack->GetXaxis()->GetXmax());
  hRatio->SetMarkerStyle(kFullCircle);
  hRatio->SetMarkerSize(0.8);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  TH1F* hDataClone = (TH1F*)hData->Clone("hDataClone");
  hDataClone->Add(hSiml,-1);
  hRatio->Divide( hDataClone ,hSiml,1.0,1.0);
  hRatio->SetAxisRange(-3,3,"Y");

  hRatio->Draw();
  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");

  if(SAVE) c1->SaveAs(Form("Zmm_BtagTCHE_%.0f.png",cutOff_));
}


void plotBtag2(  FileList fileList_  , float Lumi_ = 30 , float cutOff_ = 30.){

  TCanvas *c1 = new TCanvas("c1Btag2","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad("pad1Btag2","",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad("pad2Btag2","",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(1);

  TLegend* leg = new TLegend(0.60,0.50,0.85,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader( Form("p^{jet}_{T}>%.0f GeV/c",cutOff_) );
  
  THStack* aStack = new THStack("aStack",Form("CMS Preliminary    #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_));
  TH1F* hData = new TH1F();
  TH1F* hSiml = new TH1F();

  for(unsigned int i = 0 ; i < fileList_.size() ; i++){

    TFile* currentFile = (TFile*)fileList_[i].first ;
    if( currentFile->IsZombie() ) continue;
    TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    TTree* currentTree = (TTree*)currentFile->Get("zPlusJetsAnalyzer/tree");
    string h1Name = "h1_"+(fileList_[i].second).first;
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", 20 ,-20, 20);

    if( ((fileList_[i].second).first).find("Zjets")!=string::npos ) {
      h1->SetFillColor(kRed);
      leg->AddEntry(h1,"MadGraph Z+jets","F");
    }
    if( ((fileList_[i].second).first).find("ttbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"MadGraph t#bar{t}+jets","F");
    }
    if( ((fileList_[i].second).first).find("Wjets")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"MadGraph W+jets","F");
    }
    if( ((fileList_[i].second).first).find("tW")!=string::npos ){
      h1->SetFillColor(kYellow);
      leg->AddEntry(h1,"MadGraph single-t","F");
    }
    if( ((fileList_[i].second).first).find("QCD")!=string::npos ) {
      h1->SetFillColor(kBlack);
      leg->AddEntry(h1,"PYTHIA QCD","F");
    }
    if( ((fileList_[i].second).first).find("data")!=string::npos ) {
      h1->SetMarkerColor(kBlack);
      h1->SetMarkerStyle(kFullCircle);
      h1->SetMarkerSize(0.8);
      leg->AddEntry(h1,"DATA","lep");
    }

    currentTree->Draw( Form("jetsBtagHP>>%s",h1Name.c_str()), 
		       Form("jetsIDP4@.size()<1 || (jetsIDP4@.size()>0 && jetsIDP4[jetsIDP4@.size()-1].pt()>%f)",cutOff_) );

    if(((fileList_[i].second).first).find("data")!=string::npos){
      hData = h1;
      hData->Sumw2();
      h1->SetBinContent(1,h1->GetBinContent(1)+h1->GetBinContent(0));
      h1->SetBinContent( h1->GetNbinsX() , h1->GetBinContent(h1->GetNbinsX()) + h1->GetBinContent(h1->GetNbinsX()+1));
      continue;
    }

    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) );

    h1->SetBinContent(1,h1->GetBinContent(1)+h1->GetBinContent(0));
    h1->SetBinContent( h1->GetNbinsX() , h1->GetBinContent(h1->GetNbinsX()) + h1->GetBinContent(h1->GetNbinsX()+1));

    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }


  aStack->Draw("HIST");
  hData->Draw("PSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("TCHP output");
  hStack->SetYTitle("Number of events");
  hStack->SetTitleSize(0.05,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.75,"Y");
  leg->Draw();

  pad2->cd();
  TH1F* hRatio = new TH1F("hRatio", " ; ; #frac{(DATA-MC)}{MC}",
			  hStack->GetNbinsX(), 
			  hStack->GetXaxis()->GetXmin(), hStack->GetXaxis()->GetXmax());
  hRatio->SetMarkerStyle(kFullCircle);
  hRatio->SetMarkerSize(0.8);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  TH1F* hDataClone = (TH1F*)hData->Clone("hDataClone");
  hDataClone->Add(hSiml,-1);
  hRatio->Divide( hDataClone ,hSiml,1.0,1.0);
  hRatio->SetAxisRange(-3,3,"Y");
  
  hRatio->Draw();
  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");

  if(SAVE) c1->SaveAs(Form("Zmm_BtagTCHP_%.0f.png",cutOff_));
}


void plotLeadingJetEt(  FileList fileList_  , float Lumi_ = 30 , float etaCutLow_=0.0, float etaCutHigh_=5.0){

  TCanvas *c1 = new TCanvas(Form("c1LeadingJetEt%f",etaCutLow_),"",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad(Form("pad1LeadingJetEt%f",etaCutLow_),"",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad(Form("pad2LeadingJetEt%f",etaCutLow_),"",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(1);

  TLegend* leg = new TLegend(0.60,0.50,0.85,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader( Form("%.1f<|#eta^{jet}|<%.1f",etaCutLow_,etaCutHigh_) );
  
  THStack* aStack = new THStack("aStack",Form("CMS Preliminary    #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_));
  TH1F* hData = new TH1F();
  TH1F* hSiml = new TH1F();

  for(unsigned int i = 0 ; i < fileList_.size() ; i++){

    TFile* currentFile = (TFile*)fileList_[i].first ;
    if( currentFile->IsZombie() ) continue;
    TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    TTree* currentTree = (TTree*)currentFile->Get("zPlusJetsAnalyzer/tree");
    string h1Name = "h1_"+(fileList_[i].second).first;
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", 36 ,20, 200);

    if( ((fileList_[i].second).first).find("Zjets")!=string::npos ) {
      h1->SetFillColor(kRed);
      leg->AddEntry(h1,"MadGraph Z+jets","F");
    }
    if( ((fileList_[i].second).first).find("ttbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"MadGraph t#bar{t}+jets","F");
    }
    if( ((fileList_[i].second).first).find("Wjets")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"MadGraph W+jets","F");
    }
    if( ((fileList_[i].second).first).find("tW")!=string::npos ){
      h1->SetFillColor(kYellow);
      leg->AddEntry(h1,"MadGraph single-t","F");
    }
    if( ((fileList_[i].second).first).find("QCD")!=string::npos ) {
      h1->SetFillColor(kBlack);
      leg->AddEntry(h1,"PYTHIA QCD","F");
    }
    if( ((fileList_[i].second).first).find("data")!=string::npos ) {
      h1->SetMarkerColor(kBlack);
      h1->SetMarkerStyle(kFullCircle);
      h1->SetMarkerSize(0.8);
      leg->AddEntry(h1,"DATA","lep");
    }

    currentTree->Draw( Form("jetsIDP4[0].Et()>>%s",h1Name.c_str()), 
		       Form("jetsIDP4@.size()>0 && abs(jetsIDP4[0].eta())<%f && abs(jetsIDP4[0].eta())>%f",etaCutHigh_,etaCutLow_ ) );

    if(((fileList_[i].second).first).find("data")!=string::npos){
      hData = h1;
      hData->Sumw2();
      continue;
    }
    
    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) );

    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }


  aStack->Draw("HIST");
  hData->Draw("PSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("E_{T} leading jet");
  hStack->SetYTitle("Events/(5 GeV/c)");
  hStack->SetTitleSize(0.05,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.75,"Y");
  leg->Draw();

  pad2->cd();
  TH1F* hRatio = new TH1F("hRatio", " ; ; #frac{(DATA-MC)}{MC}",
			  hStack->GetNbinsX(), 
			  hStack->GetXaxis()->GetXmin(), hStack->GetXaxis()->GetXmax());
  hRatio->SetMarkerStyle(kFullCircle);
  hRatio->SetMarkerSize(0.8);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  TH1F* hDataClone = (TH1F*)hData->Clone("hDataClone");
  hDataClone->Add(hSiml,-1);
  hRatio->Divide( hDataClone ,hSiml,1.0,1.0);
  hRatio->SetAxisRange(-3,3,"Y");

  hRatio->Draw();
  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");

  if(SAVE) c1->SaveAs(Form("Zmm_LeadJetEt_%.0f.png",etaCutLow_));
}




void plotSubLeadingJetEt(  FileList fileList_  , float Lumi_ = 30 , float etaCutLow_=0.0, float etaCutHigh_=5.0){

  TCanvas *c1 = new TCanvas(Form("c1SubLeadingJetEt%f",etaCutLow_),"",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad(Form("pad1SubLeadingJetEt%f",etaCutLow_),"",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad(Form("pad2SubLeadingJetEt%f",etaCutLow_),"",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(1);

  TLegend* leg = new TLegend(0.60,0.50,0.85,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader( Form("%.1f<|#eta^{jet}|<%.1f",etaCutLow_,etaCutHigh_) );
  
  THStack* aStack = new THStack("aStack",Form("#sqrt{s}=7 TeV L=%.0f pb^{-1}   CMS Preliminary",Lumi_));
  TH1F* hData = new TH1F();
  TH1F* hSiml = new TH1F();

  for(unsigned int i = 0 ; i < fileList_.size() ; i++){

    TFile* currentFile = (TFile*)fileList_[i].first ;
    if( currentFile->IsZombie() ) continue;
    TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    TTree* currentTree = (TTree*)currentFile->Get("zPlusJetsAnalyzer/tree");
    string h1Name = "h1_"+(fileList_[i].second).first;
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", 36 ,20, 200);

    if( ((fileList_[i].second).first).find("Zjets")!=string::npos ) {
      h1->SetFillColor(kRed);
      leg->AddEntry(h1,"MadGraph Z+jets","F");
    }
    if( ((fileList_[i].second).first).find("ttbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"MadGraph t#bar{t}+jets","F");
    }
    if( ((fileList_[i].second).first).find("Wjets")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"MadGraph W+jets","F");
    }
    if( ((fileList_[i].second).first).find("tW")!=string::npos ){
      h1->SetFillColor(kYellow);
      leg->AddEntry(h1,"MadGraph single-t","F");
    }
    if( ((fileList_[i].second).first).find("QCD")!=string::npos ) {
      h1->SetFillColor(kBlack);
      leg->AddEntry(h1,"PYTHIA QCD","F");
    }
    if( ((fileList_[i].second).first).find("data")!=string::npos ) {
      h1->SetMarkerColor(kBlack);
      h1->SetMarkerStyle(kFullCircle);
      h1->SetMarkerSize(0.8);
      leg->AddEntry(h1,"DATA","lep");
    }

    currentTree->Draw( Form("jetsIDP4[1].Et()>>%s",h1Name.c_str()), 
		       Form("jetsIDP4@.size()>1 && abs(jetsIDP4[1].eta())<%f && abs(jetsIDP4[1].eta())>%f",etaCutHigh_,etaCutLow_ ) );

    if(((fileList_[i].second).first).find("data")!=string::npos){
      hData = h1;
      hData->Sumw2();
      continue;
    }
    
    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) );

    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }


  aStack->Draw("HIST");
  hData->Draw("PSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("E_{T} sub-leading jet");
  hStack->SetYTitle("Events/(5 GeV/c)");
  hStack->SetTitleSize(0.05,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.75,"Y");
  leg->Draw();

  pad2->cd();
  TH1F* hRatio = new TH1F("hRatio", " ; ; #frac{(DATA-MC)}{MC}",
			  hStack->GetNbinsX(), 
			  hStack->GetXaxis()->GetXmin(), hStack->GetXaxis()->GetXmax());
  hRatio->SetMarkerStyle(kFullCircle);
  hRatio->SetMarkerSize(0.8);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  TH1F* hDataClone = (TH1F*)hData->Clone("hDataClone");
  hDataClone->Add(hSiml,-1);
  hRatio->Divide( hDataClone ,hSiml,1.0,1.0);
  hRatio->SetAxisRange(-3,3,"Y");

  hRatio->Draw();
  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");

  if(SAVE) c1->SaveAs(Form("Zmm_SubLeadJetEt_%.0f.png",etaCutLow_));
}


void plotZpt(  FileList fileList_  , float Lumi_ = 30 , float cutOff_ = 30., int jetMult_ = 99, int nBins_ = 40){

  TCanvas *c1 = new TCanvas(Form("c1Zpt%d",jetMult_),"",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad(Form("pad1Zpt%d",jetMult_),"",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad(Form("pad2Zpt%d",jetMult_),"",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(1);

  TLegend* leg = new TLegend(0.60,0.40,0.85,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader( Form("p^{jet}_{T}>%.0f GeV/c, N^{jets}=%d ",cutOff_,jetMult_) );
  
  THStack* aStack = new THStack("aStack",Form("CMS Preliminary    #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_));
  TH1F* hData = new TH1F();
  TH1F* hSiml = new TH1F();

  for(unsigned int i = 0 ; i < fileList_.size() ; i++){

    TFile* currentFile = (TFile*)fileList_[i].first ;
    if( currentFile->IsZombie() ) continue;
    TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    TTree* currentTree = (TTree*)currentFile->Get("zPlusJetsAnalyzer/tree");
    string h1Name = "h1_"+(fileList_[i].second).first;
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", nBins_ ,0, 200);

    if( ((fileList_[i].second).first).find("Zjets")!=string::npos ) {
      h1->SetFillColor(kRed);
      leg->AddEntry(h1,"MadGraph Z+jets","F");
    }
    if( ((fileList_[i].second).first).find("ttbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"MadGraph t#bar{t}+jets","F");
    }
    if( ((fileList_[i].second).first).find("Wjets")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"MadGraph W+jets","F");
    }
    if( ((fileList_[i].second).first).find("tW")!=string::npos ){
      h1->SetFillColor(kYellow);
      leg->AddEntry(h1,"MadGraph single-t","F");
    }
    if( ((fileList_[i].second).first).find("QCD")!=string::npos ) {
      h1->SetFillColor(kBlack);
      leg->AddEntry(h1,"PYTHIA QCD","F");
    }
    if( ((fileList_[i].second).first).find("data")!=string::npos ) {
      h1->SetMarkerColor(kBlack);
      h1->SetMarkerStyle(kFullCircle);
      h1->SetMarkerSize(0.8);
      leg->AddEntry(h1,"DATA","lep");
    }

    currentTree->Draw( Form("diMuonP4[0].Pt()>>%s",h1Name.c_str()), 
		       Form("(jetsIDP4@.size()>%f && jetsIDP4@.size()<%f && ( (jetsIDP4@.size()<1) || (jetsIDP4@.size()>0 && jetsIDP4[jetsIDP4@.size()-1].pt()>%f)) )",(float)(jetMult_-0.5),(float)(jetMult_+0.5),cutOff_) );

    if(((fileList_[i].second).first).find("data")!=string::npos){
      hData = h1;
      hData->Sumw2();
      continue;
    }

    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) );

    h1->SetBinContent( h1->GetNbinsX() , h1->GetBinContent(h1->GetNbinsX()) + h1->GetBinContent(h1->GetNbinsX()+1));

    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);
    
    aStack->Add(h1);

  }

  aStack->Draw("HIST");
  hData->Draw("PSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("p_{T}^{#mu#mu}");
  hStack->SetYTitle( Form("Events/(%.0f GeV/c)",hStack->GetBinWidth(1)) );
  hStack->SetTitleSize(0.05,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.75,"Y");

  leg->Draw();

  pad2->cd();
  TH1F* hRatio = new TH1F("hRatio", " ; ; #frac{(DATA-MC)}{MC}",
			  hStack->GetNbinsX(), 
			  hStack->GetXaxis()->GetXmin(), hStack->GetXaxis()->GetXmax());
  hRatio->SetMarkerStyle(kFullCircle);
  hRatio->SetMarkerSize(0.8);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  TH1F* hDataClone = (TH1F*)hData->Clone("hDataClone");
  hDataClone->Add(hSiml,-1);
  hRatio->Divide( hDataClone ,hSiml,1.0,1.0);
  hRatio->SetAxisRange(-3,3,"Y");

  hRatio->Draw();
  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");

  if(SAVE) c1->SaveAs(Form("Zmm_Zpt_%.0f_%djets.png",cutOff_,jetMult_));
}


void plotSubLeadMuonRelIso(  FileList fileList_  , float Lumi_ = 30 ){

  TCanvas *c1 = new TCanvas("c1SubLeadMuonRelIso","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad("pad1SubLeadMuonRelIso","",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad("pad2SubLeadMuonRelIso","",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(1);

  TLegend* leg = new TLegend(0.60,0.50,0.85,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader( "muon isolation" );
  
  THStack* aStack = new THStack("aStack",Form("CMS Preliminary    #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_));
  TH1F* hData = new TH1F();
  TH1F* hSiml = new TH1F();

  for(unsigned int i = 0 ; i < fileList_.size() ; i++){

    TFile* currentFile = (TFile*)fileList_[i].first ;
    if( currentFile->IsZombie() ) continue;
    TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    TTree* currentTree = (TTree*)currentFile->Get("zPlusJetsAnalyzer/tree");
    string h1Name = "h1_"+(fileList_[i].second).first;
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", 40 ,0, 0.40);

    if( ((fileList_[i].second).first).find("Zjets")!=string::npos ) {
      h1->SetFillColor(kRed);
      leg->AddEntry(h1,"MadGraph Z+jets","F");
    }
    if( ((fileList_[i].second).first).find("ttbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"MadGraph t#bar{t}+jets","F");
    }
    if( ((fileList_[i].second).first).find("Wjets")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"MadGraph W+jets","F");
    }
    if( ((fileList_[i].second).first).find("tW")!=string::npos ){
      h1->SetFillColor(kYellow);
      leg->AddEntry(h1,"MadGraph single-t","F");
    }
    if( ((fileList_[i].second).first).find("QCD")!=string::npos ) {
      h1->SetFillColor(kBlack);
      leg->AddEntry(h1,"PYTHIA QCD","F");
    }
    if( ((fileList_[i].second).first).find("data")!=string::npos ) {
      h1->SetMarkerColor(kBlack);
      h1->SetMarkerStyle(kFullCircle);
      h1->SetMarkerSize(0.8);
      leg->AddEntry(h1,"DATA","lep");
    }

    currentTree->Draw( Form("(chIsoLeg2+nhIsoLeg2+phIsoLeg2)/muonsP4[1].Pt()>>%s",h1Name.c_str()), 
		       "" );

    if(((fileList_[i].second).first).find("data")!=string::npos){
      hData = h1;
      hData->Sumw2();
      continue;
    }
    
    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) );

    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }


  aStack->Draw("HIST");
  hData->Draw("PSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("PF-rel iso (#Delta R_{ISO}=0.3)");
  hStack->SetYTitle("Events");
  hStack->SetTitleSize(0.05,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.75,"Y");
  leg->Draw();

  pad2->cd();
  TH1F* hRatio = new TH1F("hRatio", " ; ; #frac{(DATA-MC)}{MC}",
			  hStack->GetNbinsX(), 
			  hStack->GetXaxis()->GetXmin(), hStack->GetXaxis()->GetXmax());
  hRatio->SetMarkerStyle(kFullCircle);
  hRatio->SetMarkerSize(0.8);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  TH1F* hDataClone = (TH1F*)hData->Clone("hDataClone");
  hDataClone->Add(hSiml,-1);
  hRatio->Divide( hDataClone ,hSiml,1.0,1.0);
  hRatio->SetAxisRange(-1,1,"Y");

  hRatio->Draw();
  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");

  if(SAVE) c1->SaveAs("Zmm_MuIso.png");
}


void plotZeppZ(  FileList fileList_  , float Lumi_ = 30 , float cutOff_ = 30.){

  TCanvas *c1 = new TCanvas("c1ZeppZ","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad("pad1ZeppZ","",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad("pad2ZeppZ","",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(1);

  TLegend* leg = new TLegend(0.75,0.75,0.95,0.95,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader( Form("p^{j1,j2}_{T}>%.0f GeV/c",cutOff_) );
  
  THStack* aStack = new THStack("aStack",Form("CMS Preliminary    #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_));
  TH1F* hData = new TH1F();
  TH1F* hSiml = new TH1F();

  for(unsigned int i = 0 ; i < fileList_.size() ; i++){

    TFile* currentFile = (TFile*)fileList_[i].first ;
    if( currentFile->IsZombie() ) continue;
    TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    TTree* currentTree = (TTree*)currentFile->Get("zPlusJetsAnalyzer/tree");
    string h1Name = "h1_"+(fileList_[i].second).first;
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", 28 ,-7, 7);

    if( ((fileList_[i].second).first).find("Zjets")!=string::npos ) {
      h1->SetFillColor(kRed);
      leg->AddEntry(h1,"MadGraph Z+jets","F");
    }
    if( ((fileList_[i].second).first).find("ttbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"MadGraph t#bar{t}+jets","F");
    }
    if( ((fileList_[i].second).first).find("Wjets")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"MadGraph W+jets","F");
    }
    if( ((fileList_[i].second).first).find("tW")!=string::npos ){
      h1->SetFillColor(kYellow);
      leg->AddEntry(h1,"MadGraph single-t","F");
    }
    if( ((fileList_[i].second).first).find("QCD")!=string::npos ) {
      h1->SetFillColor(kBlack);
      leg->AddEntry(h1,"PYTHIA QCD","F");
    }
    if( ((fileList_[i].second).first).find("data")!=string::npos ) {
      h1->SetMarkerColor(kBlack);
      h1->SetMarkerStyle(kFullCircle);
      h1->SetMarkerSize(0.8);
      leg->AddEntry(h1,"DATA","lep");
    }

    currentTree->Draw( Form("(diMuonP4[0].Eta()-(jetsIDP4[0].Eta()+jetsIDP4[1].Eta())/2.)>>%s",h1Name.c_str()), 
		       Form("jetsIDP4@.size()>1 && jetsIDP4[1].pt()>%f",cutOff_) );

    if(((fileList_[i].second).first).find("data")!=string::npos){
      hData = h1;
      hData->Sumw2();
      continue;
    }

    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) );

    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }


  aStack->Draw("HIST");
  hData->Draw("PSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("#eta^{#mu#mu}- <#eta_{j1,j2}>");
  hStack->SetYTitle( Form("Events/(%.1f unities)",hStack->GetBinWidth(1)) );
  hStack->SetTitleSize(0.05,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.75,"Y");

  leg->Draw();

  pad2->cd();
  TH1F* hRatio = new TH1F("hRatio", " ; ; #frac{(DATA-MC)}{MC}",
			  hStack->GetNbinsX(), 
			  hStack->GetXaxis()->GetXmin(), hStack->GetXaxis()->GetXmax());
  hRatio->SetMarkerStyle(kFullCircle);
  hRatio->SetMarkerSize(0.8);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  TH1F* hDataClone = (TH1F*)hData->Clone("hDataClone");
  hDataClone->Add(hSiml,-1);
  hRatio->Divide( hDataClone ,hSiml,1.0,1.0);
  hRatio->SetAxisRange(-3,3,"Y");

  hRatio->Draw();
  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");

  if(SAVE) c1->SaveAs(Form("Zmm_ZeppZ_%.0f.png",cutOff_));
}





void plotJetVeto(  FileList fileList_  , float Lumi_ = 30 , float cutOff_ = 30. , float minDEta_ = 1.0){

  TCanvas *c1 = new TCanvas("c1JetVeto","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(0);

  TPad* pad1 = new TPad("pad1JetVeto","",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad("pad2JetVeto","",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(0);

  TLegend* leg = new TLegend(0.15,0.10,0.50,0.35,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader( Form("p^{j1,j2}_{T}>%.0f GeV/c ; |#eta_{j1}-#eta_{j2}|>%.1f ; (#eta_{j1,j2}+ 0.5) < #eta_{j3} < (#eta_{j2,j1}- 0.5)",cutOff_,minDEta_) );

  //char* ptCut[3] = {"25","30","35"};
  char* ptCut[3] = {"20","25","30"};
 
  char* numPV[3] = {"1","2","5"};
  float vxPV[]={0.5,1.5,2.5};
  float vexPV[]={0.,0.,0.};
  float vexDPV[]={0.,0.,0.};
  // MC
  float vyPV15[]={-1.,-1.,-1.};
  float vyPV25[]={-1.,-1.,-1.};
  float vyPV35[]={-1.,-1.,-1.};
  float veyPV15[]={0.,0.,0.};
  float veyPV25[]={0.,0.,0.};
  float veyPV35[]={0.,0.,0.};
  // DATA
  float vyDPV15[]={0.,0.,0.};
  float vyDPV25[]={0.,0.,0.};
  float vyDPV35[]={0.,0.,0.};
  float veyDPV15[]={0.,0.,0.};
  float veyDPV25[]={0.,0.,0.};
  float veyDPV35[]={0.,0.,0.};

  for(unsigned int i = 0 ; i< 2; i++){

    for(unsigned int j = 0 ; j< 3; j++){
      
      // MC
      float fail_i_j = 0;
      float norm_i_j = 0; 
      float fail_i_j_num = 0; 
      float norm_i_j_num = 0;
      // DATA
      float failD_i_j = 0;
      float normD_i_j = 0; 
      float failD_i_j_num = 0; 
      float normD_i_j_num = 0;
   
      const char* lastJ = (j<2) ? numPV[j+1] : "99";
	
      for(unsigned int f = 0 ; f < fileList_.size() ; f++){
	
	TFile* currentFile = (TFile*)fileList_[f].first ;
	if( currentFile->IsZombie() ) continue;
	TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
	float totalEvents = allEvents->GetBinContent(1);
	
	TTree* currentTree = (TTree*)currentFile->Get("zPlusJetsAnalyzer/tree");
	//string h1Name = "h1_"+(fileList_[f].second).first;
	TH1F* h1Fail = new TH1F( "h1Fail" ,"", 1 ,55, 125);
	TH1F* h1Norm = new TH1F( "h1Norm" ,"", 1 ,55, 125);
	
	currentTree->Draw("Zmass>>h1Fail", 
			  Form("(jetsIDP4@.size()>2 && abs(jetsIDP4[0].eta()-jetsIDP4[1].eta())>%f && jetsIDP4[1].Et()>%f && (jetsIDP4[2].Et()>%s && ( (jetsIDP4[2].eta()> (jetsIDP4[1].eta()+0.5) &&  jetsIDP4[2].eta()<(jetsIDP4[0].eta()-0.5)) || (jetsIDP4[2].eta()>(jetsIDP4[0].eta()+0.5) &&  jetsIDP4[2].eta()<(jetsIDP4[1].eta()-0.5)) ) ) && numPV>=%s && numPV<%s)", minDEta_,cutOff_,ptCut[i],numPV[j],lastJ) );
	
	currentTree->Draw("Zmass>>h1Norm", 
			  Form("(jetsIDP4@.size()>1 && abs(jetsIDP4[0].eta()-jetsIDP4[1].eta())>%f && jetsIDP4[1].Et()>%f && numPV>=%s && numPV<%s)",minDEta_,cutOff_,numPV[j],lastJ) );
	
	if(((fileList_[f].second).first).find("data")!=string::npos){
	  failD_i_j+=h1Fail->Integral();
	  failD_i_j_num+=h1Fail->GetEntries();
	  normD_i_j+=h1Norm->Integral();
	  normD_i_j_num+=h1Norm->GetEntries();
	  
	  delete h1Norm;
	  delete h1Fail;

	}
	else{

	  h1Fail->Scale(  Lumi_ / (totalEvents/((fileList_[f].second).second)) );
	  h1Norm->Scale(  Lumi_ / (totalEvents/((fileList_[f].second).second)) );

	  fail_i_j+=h1Fail->Integral();
	  fail_i_j_num+=h1Fail->GetEntries();
	  norm_i_j+=h1Norm->Integral();
	  norm_i_j_num+=h1Norm->GetEntries();
	  
	  delete h1Norm;
	  delete h1Fail;
	}
	
      }//end f

      cout << "N of PV in [" <<  numPV[j] << "," << lastJ << "[  ===> " << fail_i_j << "   /   " << norm_i_j << endl;

      if(ptCut[i]=="20"){
	vyPV15[j]  =  1-fail_i_j/norm_i_j;
	veyPV15[j] = sqrt( vyPV15[j]*(1-vyPV15[j]) / norm_i_j_num);
	vyDPV15[j]  =  1-failD_i_j/normD_i_j;
	veyDPV15[j] = sqrt( vyDPV15[j]*(1-vyDPV15[j]) / normD_i_j_num);
      }
      if(ptCut[i]=="25"){
	vyPV25[j] =  1-fail_i_j/norm_i_j;
	veyPV25[j] = sqrt( vyPV25[j]*(1-vyPV25[j]) / norm_i_j_num);
	vyDPV25[j]  =  1-failD_i_j/normD_i_j;
	veyDPV25[j] = sqrt( vyDPV25[j]*(1-vyDPV25[j]) / normD_i_j_num);
      }
      if(ptCut[i]=="30") {
	vyPV35[j] =  1-fail_i_j/norm_i_j;
	veyPV35[j] = sqrt( vyPV35[j]*(1-vyPV35[j]) / norm_i_j_num);
	vyDPV35[j]  =  1-failD_i_j/normD_i_j;
	veyDPV35[j] = sqrt( vyDPV35[j]*(1-vyDPV35[j]) / normD_i_j_num);
      }

    }//end j

  }//end i

 
  
  TH2F* hMaster = new TH2F("hMaster",Form("CMS Preliminary    #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_),3,0,3,10,0.4,1.2);

  hMaster->SetXTitle("num PV");
  hMaster->SetYTitle( "jet veto efficiency" );
  hMaster->SetTitleSize(0.05,"X");
  hMaster->SetTitleSize(0.05,"Y");
  hMaster->SetTitleOffset(0.85,"Y");
  hMaster->GetXaxis()->SetBinLabel(1,"1");
  hMaster->GetXaxis()->SetBinLabel(2,"2-4");
  hMaster->GetXaxis()->SetBinLabel(3,">4");
  //hMaster->Draw();
  

  TMultiGraph *mg = new TMultiGraph();

  //TGraphErrors* graph15 = new TGraphErrors(TvxPV15,TvyPV15,TvexPV15,TveyPV15);
  TGraphErrors* graph15 = new TGraphErrors(3,vxPV,vyPV15,vexPV,veyPV15);
  graph15->SetMarkerStyle(kOpenCircle);
  graph15->SetMarkerSize(1.2);
  graph15->SetMarkerColor(kRed);
  graph15->SetFillStyle(3005);
  graph15->SetFillColor(kRed);

  //TGraphErrors* graph25 = new TGraphErrors(TvxPV25,TvyPV25,TvexPV25,TveyPV25);
  TGraphErrors* graph25 = new TGraphErrors(3,vxPV,vyPV25,vexPV,veyPV25);
  graph25->SetMarkerStyle(kOpenStar);
  graph25->SetMarkerSize(1.2);
  graph25->SetMarkerColor(kBlue);
  graph25->SetFillStyle(3004);
  graph25->SetFillColor(kBlue);

  //TGraphErrors* graph35 = new TGraphErrors(TvxPV35,TvyPV35,TvexPV35,TveyPV35);
  TGraphErrors* graph35 = new TGraphErrors(3,vxPV,vyPV35,vexPV,veyPV35);
  graph35->SetMarkerStyle(kOpenSquare);
  graph35->SetMarkerSize(1.2);
  graph35->SetMarkerColor(kMagenta);
  graph35->SetFillStyle(3003);
  graph35->SetFillColor(kMagenta);

  //TGraphErrors* graphD15 = new TGraphErrors(TvxDPV15,TvyDPV15,TvexDPV15,TveyDPV15);
  TGraphErrors* graphD15 = new TGraphErrors(3,vxPV,vyDPV15,vexPV,veyDPV15);
  graphD15->SetMarkerStyle(kFullTriangleUp);
  graphD15->SetMarkerSize(1.2);
  graphD15->SetMarkerColor(kRed);

  //TGraphErrors* graphD25 = new TGraphErrors(TvxDPV25,TvyDPV25,TvexDPV25,TveyDPV25);
  TGraphErrors* graphD25 = new TGraphErrors(3,vxPV,vyDPV25,vexPV,veyDPV25);
  graphD25->SetMarkerStyle(kFullTriangleDown);
  graphD25->SetMarkerSize(1.2);
  graphD25->SetMarkerColor(kBlue);

  //TGraphErrors* graphD35 = new TGraphErrors(TvxDPV35,TvyDPV35,TvexDPV35,TveyDPV35);
  TGraphErrors* graphD35 = new TGraphErrors(3,vxPV,vyDPV35,vexPV,veyDPV35);
  graphD35->SetMarkerStyle(kFullCircle);
  graphD35->SetMarkerSize(1.2);
  graphD35->SetMarkerColor(kMagenta);

  mg->Add(graph15,"3");
  mg->Add(graph25,"3");
  //mg->Add(graph35,"3");

  mg->Add(graphD15,"P");
  mg->Add(graphD25,"P");
  //mg->Add(graphD35,"P");

  mg->Draw("A");
  mg->SetTitle(hMaster->GetTitle());
  mg->GetXaxis()->SetTitle("Num PV");
  mg->GetYaxis()->SetTitle("jet veto efficiency");
  mg->GetXaxis()->Set(3,0,3);
  mg->GetXaxis()->SetBinLabel(1,"1");
  mg->GetXaxis()->SetBinLabel(2,"2-4");
  mg->GetXaxis()->SetBinLabel(3,">4");
  mg->GetYaxis()->SetRangeUser(0.4,1.1);
  mg->Draw("A");


  leg->AddEntry(graph15,"E_{T}^{veto}=20 GeV MC","F");
  leg->AddEntry(graphD15,"E_{T}^{veto}=20 GeV DATA","lep");
  leg->AddEntry(graph25,"E_{T}^{veto}=25 GeV MC","F");
  leg->AddEntry(graphD25,"E_{T}^{veto}=25 GeV DATA","lep");
  //leg->AddEntry(graph35,"E_{T}^{veto}=26 GeV MC","F");
  //leg->AddEntry(graphD35,"E_{T}^{veto}=26 GeV DATA","lep");

  leg->Draw();


  pad2->cd();
  TH1F* hRatio = new TH1F("hRatio", " ; ; #frac{(DATA-MC)}{MC}",
			  hMaster->GetNbinsX(), 
			  hMaster->GetXaxis()->GetXmin(), hMaster->GetXaxis()->GetXmax());
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  hRatio->GetXaxis()->SetBinLabel(1,"1");
  hRatio->GetXaxis()->SetBinLabel(2,"2-4");
  hRatio->GetXaxis()->SetBinLabel(3,">4");

  Double_t numX=0;
  Double_t numY=0;
  Double_t denX=1.;
  Double_t denY=1.;

  TH1F* hRatio15 =  (TH1F*)hRatio->Clone("hRatio15");
  hRatio15->SetMarkerStyle(kFullTriangleUp);
  hRatio15->SetMarkerSize(1.2);
  hRatio15->SetMarkerColor(kRed);

  graphD15->GetPoint(0,numX,numY);
  graph15->GetPoint(0,denX,denY);
  hRatio15->SetBinContent(1,numY/denY-1);
  cout<<numY<< "  " << denY << endl;
  hRatio15->SetBinError(1,  ((graphD15->GetErrorY(0)/numY)+(graph15->GetErrorY(0)/denY))*hRatio15->GetBinContent(1) );
  graphD15->GetPoint(1,numX,numY);
  graph15->GetPoint(1,denX,denY);
  cout<<numY<< "  " << denY << endl;
  hRatio15->SetBinContent(2,numY/denY-1);
  hRatio15->SetBinError(2,  ((graphD15->GetErrorY(1)/numY)+(graph15->GetErrorY(1)/denY))*hRatio15->GetBinContent(2) );
  graphD15->GetPoint(2,numX,numY);
  graph15->GetPoint(2,denX,denY);
  cout<<numY<< "  " << denY << endl;
  hRatio15->SetBinContent(3,numY/denY-1);
  hRatio15->SetBinError(3,  ((graphD15->GetErrorY(2)/numY)+(graph15->GetErrorY(2)/denY))*hRatio15->GetBinContent(3) );

  TH1F* hRatio25 =  (TH1F*)hRatio->Clone("hRatio25");
  hRatio25->SetMarkerStyle(kFullTriangleDown);
  hRatio25->SetMarkerSize(1.2);
  hRatio25->SetMarkerColor(kBlue);

  graphD25->GetPoint(0,numX,numY);
  graph25->GetPoint(0,denX,denY);
  hRatio25->SetBinContent(1,numY/denY-1);
  hRatio25->SetBinError(1,  ((graphD25->GetErrorY(0)/numY)+(graph25->GetErrorY(0)/denY))*hRatio25->GetBinContent(1) );
  graphD25->GetPoint(1,numX,numY);
  graph25->GetPoint(1,denX,denY);
  hRatio25->SetBinContent(2,numY/denY-1);
  hRatio25->SetBinError(2,  ((graphD25->GetErrorY(1)/numY)+(graph25->GetErrorY(1)/denY))*hRatio25->GetBinContent(2) );
  graphD25->GetPoint(2,numX,numY);
  graph25->GetPoint(2,denX,denY);
  hRatio25->SetBinContent(3,numY/denY-1);
  hRatio25->SetBinError(3,  ((graphD25->GetErrorY(2)/numY)+(graph25->GetErrorY(2)/denY))*hRatio25->GetBinContent(3) );

  TH1F* hRatio35 =  (TH1F*)hRatio->Clone("hRatio35");
  hRatio35->SetMarkerStyle(kFullCircle);
  hRatio35->SetMarkerSize(1.2);
  hRatio35->SetMarkerColor(kMagenta);

  graphD35->GetPoint(0,numX,numY);
  graph35->GetPoint(0,denX,denY);
  hRatio35->SetBinContent(1,numY/denY-1);
  hRatio35->SetBinError(1,  ((graphD35->GetErrorY(0)/numY)+(graph35->GetErrorY(0)/denY))*hRatio35->GetBinContent(1) );
  graphD35->GetPoint(1,numX,numY);
  graph35->GetPoint(1,denX,denY);
  hRatio35->SetBinContent(2,numY/denY-1);
  hRatio35->SetBinError(2,  ((graphD35->GetErrorY(1)/numY)+(graph35->GetErrorY(1)/denY))*hRatio35->GetBinContent(2) );
  graphD35->GetPoint(3,numX,numY);
  graph35->GetPoint(3,denX,denY);
  hRatio35->SetBinContent(3,numY/denY-1);
  hRatio35->SetBinError(3,  ((graphD35->GetErrorY(2)/numY)+(graph35->GetErrorY(2)/denY))*hRatio35->GetBinContent(3) );

  hRatio15->SetAxisRange(-0.3,0.3,"Y");
  hRatio15->Draw("P");
  hRatio25->Draw("PSAME");
  //hRatio35->Draw("PSAME");

  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hRatio->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");

  if(SAVE) c1->SaveAs(Form("Zmm_JetVeto_%.0f_%.0fDEta.png",cutOff_,minDEta_));
}


void plotBenchMarkYield(  FileList fileList_  , float Lumi_ = 30 ){

  TCanvas *c1 = new TCanvas("c1","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(0);


  TPad* pad1 = new TPad("pad1BenchMarkYield","",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad("pad2BenchMarkYield","",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(0);

  TLegend* leg = new TLegend(0.50,0.55,0.75,0.88,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader("yields for benchmark selections");
  
  THStack* aStack = new THStack("aStack",Form("CMS Preliminary    #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_));
  TH1F* hData = new TH1F();
  TH1F* hSiml = new TH1F();

  for(unsigned int i = 0 ; i < fileList_.size() ; i++){

    TFile* currentFile = (TFile*)fileList_[i].first ;
    if( currentFile->IsZombie() ) continue;
    TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    TTree* currentTree = (TTree*)currentFile->Get("zPlusJetsAnalyzer/tree");
    string h1Name = "h1_"+(fileList_[i].second).first;
    TH1F* h1 = new TH1F( h1Name.c_str() ,"; (p_{T}^{j1,j2}>X,|#Delta#eta|>Y,M_{j1,j2}>Z) ; Events/selection", 4 ,0 , 4);
    h1->SetTitleSize(0.05,"X");
    h1->SetTitleSize(0.05,"Y");
    h1->SetTitleOffset(0.75,"Y");
    //h1->SetAxisRange(0,100,"Y");

    h1->GetXaxis()->SetBinLabel(1,"20,2.0,350");
    h1->GetXaxis()->SetBinLabel(2,"30,2.3,400");
    h1->GetXaxis()->SetBinLabel(3,"30,3.5,300");
    h1->GetXaxis()->SetBinLabel(4,"25,4.0,600");

    if( ((fileList_[i].second).first).find("Zjets")!=string::npos ) {
      h1->SetFillColor(kRed);
      leg->AddEntry(h1,"MadGraph Z+jets","F");
    }
    if( ((fileList_[i].second).first).find("ttbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"MadGraph t#bar{t}+jets","F");
    }
    if( ((fileList_[i].second).first).find("Wjets")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"MadGraph W+jets","F");
    }
    if( ((fileList_[i].second).first).find("tW")!=string::npos ){
      h1->SetFillColor(kYellow);
      leg->AddEntry(h1,"MadGraph single-t","F");
    }
    if( ((fileList_[i].second).first).find("QCD")!=string::npos ) {
      h1->SetFillColor(kBlack);
      leg->AddEntry(h1,"PYTHIA QCD","F");
    }
    if( ((fileList_[i].second).first).find("data")!=string::npos ) {
      h1->SetMarkerColor(kBlack);
      h1->SetMarkerStyle(kFullCircle);
      h1->SetMarkerSize(0.8);
      leg->AddEntry(h1,"DATA","lep");
    }


    h1->SetBinContent(2, currentTree->GetEntries( "jetsIDP4@.size()>1 && jetsIDP4[1].pt()>30 && abs(jetsIDP4[0].eta()-jetsIDP4[1].eta())>2.3 && sqrt((jetsIDP4[0].E()+jetsIDP4[1].E())*(jetsIDP4[0].E()+jetsIDP4[1].E()) - (jetsIDP4[0].Px()+jetsIDP4[1].Px())*(jetsIDP4[0].Px()+jetsIDP4[1].Px()) - (jetsIDP4[0].Py()+jetsIDP4[1].Py())*(jetsIDP4[0].Py()+jetsIDP4[1].Py()) - (jetsIDP4[0].Pz()+jetsIDP4[1].Pz())*(jetsIDP4[0].Pz()+jetsIDP4[1].Pz()) )> 400 " ) );
    h1->SetBinContent(3, currentTree->GetEntries( "jetsIDP4@.size()>1 && jetsIDP4[1].pt()>30 && abs(jetsIDP4[0].eta()-jetsIDP4[1].eta())>3.5 && sqrt((jetsIDP4[0].E()+jetsIDP4[1].E())*(jetsIDP4[0].E()+jetsIDP4[1].E()) - (jetsIDP4[0].Px()+jetsIDP4[1].Px())*(jetsIDP4[0].Px()+jetsIDP4[1].Px()) - (jetsIDP4[0].Py()+jetsIDP4[1].Py())*(jetsIDP4[0].Py()+jetsIDP4[1].Py()) - (jetsIDP4[0].Pz()+jetsIDP4[1].Pz())*(jetsIDP4[0].Pz()+jetsIDP4[1].Pz()) )> 300 " ) );
    h1->SetBinContent(1, currentTree->GetEntries( "jetsIDP4@.size()>1 && jetsIDP4[1].pt()>20 && abs(jetsIDP4[0].eta()-jetsIDP4[1].eta())>2.0 && sqrt((jetsIDP4[0].E()+jetsIDP4[1].E())*(jetsIDP4[0].E()+jetsIDP4[1].E()) - (jetsIDP4[0].Px()+jetsIDP4[1].Px())*(jetsIDP4[0].Px()+jetsIDP4[1].Px()) - (jetsIDP4[0].Py()+jetsIDP4[1].Py())*(jetsIDP4[0].Py()+jetsIDP4[1].Py()) - (jetsIDP4[0].Pz()+jetsIDP4[1].Pz())*(jetsIDP4[0].Pz()+jetsIDP4[1].Pz()) )> 350 " ) );
    h1->SetBinContent(4, currentTree->GetEntries( "jetsIDP4@.size()>1 && jetsIDP4[1].pt()>30 && abs(jetsIDP4[0].eta()-jetsIDP4[1].eta())>4.0 && sqrt((jetsIDP4[0].E()+jetsIDP4[1].E())*(jetsIDP4[0].E()+jetsIDP4[1].E()) - (jetsIDP4[0].Px()+jetsIDP4[1].Px())*(jetsIDP4[0].Px()+jetsIDP4[1].Px()) - (jetsIDP4[0].Py()+jetsIDP4[1].Py())*(jetsIDP4[0].Py()+jetsIDP4[1].Py()) - (jetsIDP4[0].Pz()+jetsIDP4[1].Pz())*(jetsIDP4[0].Pz()+jetsIDP4[1].Pz()) )> 600 " ) );



    if(((fileList_[i].second).first).find("data")!=string::npos){
      hData = h1;
      hData->Sumw2();
      continue;
    }

    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) );
   
    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }


  hData->Draw("P");
  aStack->Draw("HISTSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("(p_{T}^{j1,j2}>X,|#Delta#eta|>Y,M_{j1,j2}>Z)");
  hStack->GetXaxis()->SetBinLabel(1,"30,2.3,400");
  hStack->GetXaxis()->SetBinLabel(2,"30,3.5,300");
  hStack->GetXaxis()->SetBinLabel(3,"20,2.0,350");
  hStack->GetXaxis()->SetBinLabel(3,"25,4.0,600");
  hStack->SetYTitle(Form(" Events/selection)", hStack->GetBinWidth(1) ) );
  hStack->SetTitleSize(0.06,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.75,"Y");
  hStack->SetAxisRange(0,50,"Y");
  leg->Draw();

  pad2->cd();
  TH1F* hRatio = new TH1F("hRatio", " ; ; #frac{(DATA-MC)}{MC}",
			  hStack->GetNbinsX(), 
			  hStack->GetXaxis()->GetXmin(), hStack->GetXaxis()->GetXmax());
  hRatio->SetMarkerStyle(kFullCircle);
  hRatio->SetMarkerSize(0.8);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  hRatio->GetXaxis()->SetBinLabel(1,"30,2.3,400");
  hRatio->GetXaxis()->SetBinLabel(2,"30,3.5,300");
  hRatio->GetXaxis()->SetBinLabel(3,"20,2.0,350");
  TH1F* hDataClone = (TH1F*)hData->Clone("hDataClone");
  hDataClone->Add(hSiml,-1);
  hRatio->Divide( hDataClone ,hSiml,1.0,1.0);
  hRatio->SetAxisRange(-2,2,"Y");

  hRatio->Draw();
  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");

  if(SAVE) c1->SaveAs("Zmm_BenchmarkYield_%.0f.png");

}


void mainPlot(){

  FileList dummyFileList;
  dummyFileList.clear();
  //plotMass(dummyFileList, 36., 30.);  
  
  
  TFile* fData   = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_Mu-Run2010.root","READ");
  TFile* fDYJets = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_DYJets-madgraph-50-PU.root","READ");
  TFile* fTT     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_TT-madgraph-PU.root","READ");
  TFile* fWJets  = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WJets-madgraph-PU.root","READ");
  TFile* fWZ     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WZ-pythia-PU.root","READ");
  TFile* fWW     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WW-pythia-PU.root","READ");
  TFile* fT      = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_TToBLNu-tW-madhraph-PU.root","READ");
  TFile* fQCD    = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_QCD-pythia-PU.root","READ");

  FileList fileList;
  fileList.push_back( make_pair(fQCD,    make_pair("QCD",   349988.0 )  ));
  fileList.push_back( make_pair(fWJets,  make_pair("Wjets",  31314.0 )  ));
  fileList.push_back( make_pair(fWZ,     make_pair("WZ",        10.4 )  ));
  fileList.push_back( make_pair(fWW   ,  make_pair("WW"   ,      2.9 )  ));
  fileList.push_back( make_pair(fT,      make_pair("tW",        10.6 )  ));
  fileList.push_back( make_pair(fTT,     make_pair("ttbar",    157.5 )  ));
  fileList.push_back( make_pair(fDYJets, make_pair("Zjets",   3048.0 )  ));
  fileList.push_back( make_pair(fData,    make_pair("data",       -99 )  ));


  plotJetMultiplicity( fileList, 36. , 30.);
  /*
  plotLeadingJetEt( fileList, 36. , 0.0, 2.4);
  plotLeadingJetEt( fileList, 36. , 2.4, 4.5);
  plotSubLeadingJetEt( fileList, 36. , 0.0, 2.4);
  plotSubLeadingJetEt( fileList, 36. , 2.4, 4.5);
  plotDEta( fileList , 36.);
  plotBtag( fileList, 36. , 30.);
  plotBtag2( fileList, 36. , 30.);
  plotZpt( fileList, 36. , 30., 0, 40);
  plotZpt( fileList, 36. , 30., 1, 20);
  plotZpt( fileList, 36. , 30., 2, 20);
  plotZpt( fileList, 36. , 30., 3, 10);
  //plotZpt( fileList, 36. , 30., 4, 10);
  //plotZpt( fileList, 36. , 30., 5,  5);
  //plotZpt( fileList, 36. , 30., 6,  5);
  plotZeppZ(  fileList, 36. , 30.);
  plotJetVeto(  fileList, 36. , 30., 1.5 ); 
  plotSubLeadMuonRelIso( fileList, 36.);
  */
  //plotBenchMarkYield(fileList, 36.);
  
  
}


