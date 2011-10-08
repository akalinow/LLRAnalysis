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

void plotJetMultiplicity(  FileList fileList  , float Lumi_ = 30 , float cutOff_ = 30., float maxEta_ = 4.5, float corrFactor_ = 1.0){

  for(int i = 0; i<fileList.size(); i++){
    (fileList[i].first)->Close();
    delete (fileList[i].first);
  }
  
  TFile* fData   = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_Mu-Run2010.root","READ");
  TFile* fDYJets = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_DYJets-madgraph-50-PU.root","READ");
  TFile* fTT     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_TT-madgraph-PU.root","READ");
  TFile* fWJets  = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WJets-madgraph-PU.root","READ");
  TFile* fWZ     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WZ-pythia-PU.root","READ");
  TFile* fWW     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WW-pythia-PU.root","READ");
  TFile* fT      = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_TToBLNu-tW-madhraph-PU.root","READ");
  TFile* fQCD    = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_QCD-pythia-PU.root","READ");

  FileList fileList_;
  fileList_.push_back( make_pair(fWW   ,  make_pair("WW"   ,      2.9 )  ));
  fileList_.push_back( make_pair(fWZ,     make_pair("WZ",        10.4 )  ));
  fileList_.push_back( make_pair(fT,      make_pair("tW",        10.6 )  ));
  fileList_.push_back( make_pair(fQCD,    make_pair("QCD",   349988.0 )  ));
  fileList_.push_back( make_pair(fWJets,  make_pair("Wjets",  31314.0 )  ));
  fileList_.push_back( make_pair(fTT,     make_pair("ttbar",    157.5 )  ));
  fileList_.push_back( make_pair(fDYJets, make_pair("Zjets",   3048.0 )  ));
  fileList_.push_back( make_pair(fData,   make_pair("data",       -99 )  ));

  TCanvas *c1 = new TCanvas(Form("c1JetMultiplicityPt%.0f_Eta%.1f",cutOff_,maxEta_),"",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad(Form("pad1JetMultiplicityPt%.0f_Eta%.1f",cutOff_,maxEta_),"",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad(Form("pad2JetMultiplicityPt%.0f_Eta%.1f",cutOff_,maxEta_),"",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(1);

  TLegend* leg = new TLegend(0.60,0.47,0.90,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader( Form("E^{jet}_{T}>%.0f GeV/c, |#eta^{jet}|<%.1f",cutOff_,maxEta_) );
  
  THStack* aStack = new THStack("aStack",Form("CMS Preliminary 2010    #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_));
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
      leg->AddEntry(h1,"Pythia WW","F");
    }
    if( ((fileList_[i].second).first).find("WZ")!=string::npos ) {
      h1->SetFillColor(38);
      leg->AddEntry(h1,"Pythia WZ","F");
    }
    if( ((fileList_[i].second).first).find("tW")!=string::npos ){
      h1->SetFillColor(kYellow);
      leg->AddEntry(h1,"MadGraph single-t","F");
    }
    if( ((fileList_[i].second).first).find("QCD")!=string::npos ) {
      h1->SetFillColor(kBlack);
      leg->AddEntry(h1,"Pythia QCD","F");
    }
    if( ((fileList_[i].second).first).find("data")!=string::npos ) {
      h1->SetMarkerColor(kBlack);
      h1->SetMarkerStyle(kFullCircle);
      h1->SetMarkerSize(1.0);
      leg->AddEntry(h1,"DATA","lep");
    }

    int nEntries = currentTree->GetEntries() ;
    
    std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets;
    currentTree->SetBranchAddress("jetsIDP4",&jets);

    for (int n = 0; n < nEntries ; n++) {
      currentTree->GetEntry(n);
      int counter = 0;
      for(unsigned k=0; k < jets->size(); k++){
	if((*jets)[k].Et()>=cutOff_ && abs((*jets)[k].Eta())<maxEta_ ) counter++;
      }
      h1->Fill(counter);
    }

    if(((fileList_[i].second).first).find("data")!=string::npos){
      hData = h1;
      hData->Sumw2();
      continue;
    }

    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) * corrFactor_ );

    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }

  //float numData = hData->GetEntries();
  //float numSiml = hSiml->Integral();
  //float dataToSimlRatio = numData/numSiml;
  //cout << "data " << numData << "  ---  simul " << numSiml << endl;  

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
  hRatio->SetMarkerSize(1.0);
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

  if(SAVE) c1->SaveAs(Form("Zmm_JetMultiplicity_Pt%.0f_Eta%.0f.png",cutOff_,maxEta_*10));

}


void plotDEta(  FileList fileList  , float Lumi_ = 30 , float cutOff_ = 30., float maxEta_ = 4.5, float corrFactor_ = 1.0){

  for(int i = 0; i<fileList.size(); i++){
    (fileList[i].first)->Close();
    delete (fileList[i].first);
  }
  
  TFile* fData   = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_Mu-Run2010.root","READ");
  TFile* fDYJets = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_DYJets-madgraph-50-PU.root","READ");
  TFile* fTT     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_TT-madgraph-PU.root","READ");
  TFile* fWJets  = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WJets-madgraph-PU.root","READ");
  TFile* fWZ     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WZ-pythia-PU.root","READ");
  TFile* fWW     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WW-pythia-PU.root","READ");
  TFile* fT      = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_TToBLNu-tW-madhraph-PU.root","READ");
  TFile* fQCD    = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_QCD-pythia-PU.root","READ");

  FileList fileList_;
  fileList_.push_back( make_pair(fWW   ,  make_pair("WW"   ,      2.9 )  ));
  fileList_.push_back( make_pair(fWZ,     make_pair("WZ",        10.4 )  ));
  fileList_.push_back( make_pair(fT,      make_pair("tW",        10.6 )  ));
  fileList_.push_back( make_pair(fQCD,    make_pair("QCD",   349988.0 )  ));
  fileList_.push_back( make_pair(fWJets,  make_pair("Wjets",  31314.0 )  ));
  fileList_.push_back( make_pair(fTT,     make_pair("ttbar",    157.5 )  ));
  fileList_.push_back( make_pair(fDYJets, make_pair("Zjets",   3048.0 )  ));
  fileList_.push_back( make_pair(fData,   make_pair("data",       -99 )  ));

  TCanvas *c1 = new TCanvas(Form("c1DEtaPt%.0f_Eta%.1f",cutOff_,maxEta_),"",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad(Form("pad1DEtaPt%.0f_Eta%.1f",cutOff_,maxEta_),"",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad(Form("pad2DEtaPt%.0f_Eta%.1f",cutOff_,maxEta_),"",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(1);

  TLegend* leg = new TLegend(0.60,0.47,0.90,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader( Form("E^{j1,j2}_{T}>%.0f GeV/c, |#eta^{j1,j2}|<%.1f",cutOff_,maxEta_) );
  
  THStack* aStack = new THStack("aStack",Form("CMS Preliminary 2010    #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_));
  TH1F* hData = new TH1F();
  TH1F* hSiml = new TH1F();

  for(unsigned int i = 0 ; i < fileList_.size() ; i++){

    TFile* currentFile = (TFile*)fileList_[i].first ;
    if( currentFile->IsZombie() ) continue;
    TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    TTree* currentTree = (TTree*)currentFile->Get("zPlusJetsAnalyzer/tree");
    string h1Name = "h1_"+(fileList_[i].second).first;
    TH1F* h1 = new TH1F( h1Name.c_str() ,"",16 ,0 , 8);

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
      leg->AddEntry(h1,"Pythia WW","F");
    }
    if( ((fileList_[i].second).first).find("WZ")!=string::npos ) {
      h1->SetFillColor(38);
      leg->AddEntry(h1,"Pythia WZ","F");
    }
    if( ((fileList_[i].second).first).find("tW")!=string::npos ){
      h1->SetFillColor(kYellow);
      leg->AddEntry(h1,"MadGraph single-t","F");
    }
    if( ((fileList_[i].second).first).find("QCD")!=string::npos ) {
      h1->SetFillColor(kBlack);
      leg->AddEntry(h1,"Pythia QCD","F");
    }
    if( ((fileList_[i].second).first).find("data")!=string::npos ) {
      h1->SetMarkerColor(kBlack);
      h1->SetMarkerStyle(kFullCircle);
      h1->SetMarkerSize(1.0);
      leg->AddEntry(h1,"DATA","lep");
    }

    int nEntries = currentTree->GetEntries() ;
    
    std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets;
    currentTree->SetBranchAddress("jetsIDP4",&jets);

    for (int n = 0; n < nEntries ; n++) {
      currentTree->GetEntry(n);
      if(jets->size()>1 && 
	 (*jets)[0].Et()>cutOff_ && 
	 (*jets)[1].Et()>cutOff_ &&
	 abs((*jets)[0].Eta())<maxEta_ && 
	 abs((*jets)[1].Eta())<maxEta_
	 )  h1->Fill( abs((*jets)[0].Eta()-(*jets)[1].Eta()) );
    }

    if(((fileList_[i].second).first).find("data")!=string::npos){
      hData = h1;
      hData->Sumw2();
      continue;
    }

    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) * corrFactor_ );

    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }

  //float numData = hData->GetEntries();
  //float numSiml = hSiml->Integral();
  //float dataToSimlRatio = numData/numSiml;
  //cout << "data " << numData << "  ---  simul " << numSiml << endl;  

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
  hRatio->SetAxisRange(-0.5,0.5,"Y");
  hRatio->Draw();
  
  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");
  
  if(SAVE) c1->SaveAs(Form("Zmm_DEtaPt%.0f_Eta%.0f.png",cutOff_,maxEta_*10));

}


void plotMass(FileList fileList,  float Lumi_ = 30 , float cutOff_ = 30, float maxEta_ = 4.5, float corrFactor_ = 1.0){

  for(int i = 0; i<fileList.size(); i++){
    (fileList[i].first)->Close();
    delete (fileList[i].first);
  }
  
  TFile* fData   = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_Mu-Run2010.root","READ");
  TFile* fDYJets = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_DYJets-madgraph-50-PU.root","READ");
  TFile* fTT     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_TT-madgraph-PU.root","READ");
  TFile* fWJets  = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WJets-madgraph-PU.root","READ");
  TFile* fWZ     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WZ-pythia-PU.root","READ");
  TFile* fWW     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WW-pythia-PU.root","READ");
  TFile* fT      = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_TToBLNu-tW-madhraph-PU.root","READ");
  TFile* fQCD    = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_QCD-pythia-PU.root","READ");

  FileList fileList_;
  fileList_.push_back( make_pair(fWW   ,  make_pair("WW"   ,      2.9 )  ));
  fileList_.push_back( make_pair(fWZ,     make_pair("WZ",        10.4 )  ));
  fileList_.push_back( make_pair(fT,      make_pair("tW",        10.6 )  ));
  fileList_.push_back( make_pair(fQCD,    make_pair("QCD",   349988.0 )  ));
  fileList_.push_back( make_pair(fWJets,  make_pair("Wjets",  31314.0 )  ));
  fileList_.push_back( make_pair(fTT,     make_pair("ttbar",    157.5 )  ));
  fileList_.push_back( make_pair(fDYJets, make_pair("Zjets",   3048.0 )  ));
  fileList_.push_back( make_pair(fData,   make_pair("data",       -99 )  ));

  TCanvas *c1 = new TCanvas(Form("c1MassPt%.0f_Eta%.1f",cutOff_,maxEta_),"",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad(Form("pad1MassPt%.0f_Eta%.1f",cutOff_,maxEta_),"",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad(Form("pad2MassPt%.0f_Eta%.1f",cutOff_,maxEta_),"",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(1);

  TLegend* leg = new TLegend(0.60,0.55,0.90,0.90,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader(Form("E^{j1,j2}_{T}>%.0f GeV, |#eta^{j1,j2}|<%.1f",cutOff_,maxEta_));
  
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
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", 40 ,0 , 800);

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
    if( ((fileList_[i].second).first).find("WW")!=string::npos ) {
      h1->SetFillColor(42);
      leg->AddEntry(h1,"Pythia WW","F");
    }
    if( ((fileList_[i].second).first).find("WZ")!=string::npos ) {
      h1->SetFillColor(38);
      leg->AddEntry(h1,"Pythia WZ","F");
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

    std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets;
    //std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diMu;
    currentTree->SetBranchAddress("jetsIDP4",&jets);
    //currentTree->SetBranchAddress("diMuonP4",&diMu);

    for (int n = 0; n < nEntries ; n++) {
      //if(n%100==0) cout << n << endl;
      currentTree->GetEntry(n);
      XYZTVector q;
      if( jets->size()>1 && 
	  (*jets)[0].Et()> cutOff_ &&
	  (*jets)[1].Et()> cutOff_ && 
	  abs((*jets)[0].Eta())<maxEta_ && 
	  abs((*jets)[1].Eta())<maxEta_ /* && ((*jets)[0]+(*jets)[1]).Pt()>30 && abs((*diMu)[0].Pt()-90.)<10*/
	  ) h1->Fill(  ((*jets)[0]+(*jets)[1]).M());
    }


    if(((fileList_[i].second).first).find("data")!=string::npos){
      hData = h1;
      hData->Sumw2();
      continue;
    }

    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) * corrFactor_ );
   
    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }

  cout << "Exp bkg = " << hSiml->Integral() << " --- DATA = " << hData->GetEntries() << endl;

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
  hRatio->SetAxisRange(-0.5,0.5,"Y");

  hRatio->Draw();
  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");

  if(SAVE) c1->SaveAs(Form("Zmm_Mass_%.0f_Eta%.0f.png",cutOff_,maxEta_*10));

}


void plotBtagInMassBump(FileList fileList,  float Lumi_ = 30 , float cutOff_ = 30, float maxEta_ = 4.5, float corrFactor_ = 1.0){

  for(int i = 0; i<fileList.size(); i++){
    (fileList[i].first)->Close();
    delete (fileList[i].first);
  }
  
  TFile* fData   = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_Mu-Run2010.root","READ");
  TFile* fDYJets = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_DYJets-madgraph-50-PU.root","READ");
  TFile* fTT     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_TT-madgraph-PU.root","READ");
  TFile* fWJets  = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WJets-madgraph-PU.root","READ");
  TFile* fWZ     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WZ-pythia-PU.root","READ");
  TFile* fWW     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WW-pythia-PU.root","READ");
  TFile* fT      = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_TToBLNu-tW-madhraph-PU.root","READ");
  TFile* fQCD    = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_QCD-pythia-PU.root","READ");

  FileList fileList_;
  fileList_.push_back( make_pair(fWW   ,  make_pair("WW"   ,      2.9 )  ));
  fileList_.push_back( make_pair(fWZ,     make_pair("WZ",        10.4 )  ));
  fileList_.push_back( make_pair(fT,      make_pair("tW",        10.6 )  ));
  fileList_.push_back( make_pair(fQCD,    make_pair("QCD",   349988.0 )  ));
  fileList_.push_back( make_pair(fWJets,  make_pair("Wjets",  31314.0 )  ));
  fileList_.push_back( make_pair(fTT,     make_pair("ttbar",    157.5 )  ));
  fileList_.push_back( make_pair(fDYJets, make_pair("Zjets",   3048.0 )  ));
  fileList_.push_back( make_pair(fData,   make_pair("data",       -99 )  ));

  TCanvas *c1 = new TCanvas(Form("c1BtagInMassBumpPt%.0f_Eta%.1f",cutOff_,maxEta_),"",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad(Form("pad1BtagInMassBumpPt%.0f_Eta%.1f",cutOff_,maxEta_),"",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad(Form("pad2BtagInMassBumpPt%.0f_Eta%.1f",cutOff_,maxEta_),"",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(1);

  TLegend* leg = new TLegend(0.60,0.55,0.90,0.90,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader(Form("E^{j1,j2}_{T}>%.0f GeV, |#eta^{j1,j2}|<%.1f",cutOff_,maxEta_));
  
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
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", 16,-4,4);

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
    if( ((fileList_[i].second).first).find("WW")!=string::npos ) {
      h1->SetFillColor(42);
      leg->AddEntry(h1,"Pythia WW","F");
    }
    if( ((fileList_[i].second).first).find("WZ")!=string::npos ) {
      h1->SetFillColor(38);
      leg->AddEntry(h1,"Pythia WZ","F");
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

    std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets;
    std::vector<double>* bTag;
    currentTree->SetBranchAddress("jetsIDP4",&jets);
    currentTree->SetBranchAddress("jetsBtagHE",&bTag);

    for (int n = 0; n < nEntries ; n++) {
      currentTree->GetEntry(n);
      XYZTVector q;
      if( jets->size()>1 && 
	  (*jets)[0].Et()> cutOff_ &&
	  (*jets)[1].Et()> cutOff_ && 
	  abs((*jets)[0].Eta())<maxEta_ && 
	  abs((*jets)[1].Eta())<maxEta_ &&
	  ((*jets)[0]+(*jets)[1]).M()>200 &&
	  ((*jets)[0]+(*jets)[1]).M()<280
	  ) //h1->Fill( 2*((*jets)[0].Et()-(*jets)[1].Et())/((*jets)[0].Et()+(*jets)[1].Et()) );//(*bTag)[0]
	h1->Fill( (*jets)[0].Eta() );//(*bTag)[0]
    }


    if(((fileList_[i].second).first).find("data")!=string::npos){
      hData = h1;
      hData->Sumw2();
      continue;
    }

    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) * corrFactor_ );
   
    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }

  cout << "Exp bkg = " << hSiml->Integral() << " --- DATA = " << hData->GetEntries() << endl;

  aStack->Draw("HIST");
  hData->Draw("PSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("b-tagger TCHE");
  hStack->SetYTitle(Form(" Events/(%.1f unities)", hStack->GetBinWidth(1) ) );
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
  hRatio->SetAxisRange(-0.5,0.5,"Y");

  hRatio->Draw();
  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");

  if(SAVE) c1->SaveAs(Form("Zmm_BtagInMassBump_%.0f_Eta%.0f.png",cutOff_,maxEta_*10));

}



void plotLeadingJetEt(  FileList fileList_  , float Lumi_ = 30 , float etaCutLow_=0.0, float etaCutHigh_=5.0,float corrFactor_ = 1.0){


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
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", 24 ,10, 250);

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
      leg->AddEntry(h1,"Pythia WW","F");
    }
    if( ((fileList_[i].second).first).find("WZ")!=string::npos ) {
      h1->SetFillColor(38);
      leg->AddEntry(h1,"Pythia WZ","F");
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
    
    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) * corrFactor_  );

    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }


  aStack->Draw("HIST");
  hData->Draw("PSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("E_{T} leading jet");
  hStack->SetYTitle("Events/(10 GeV/c)");
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

  if(SAVE) c1->SaveAs(Form("Zmm_LeadJetEt_%.0f.png",etaCutLow_));
}




void plotSubLeadingJetEt(  FileList fileList_  , float Lumi_ = 30 , float etaCutLow_=0.0, float etaCutHigh_=5.0,float corrFactor_ = 1.0){

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
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", 24 ,10, 250);

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
      leg->AddEntry(h1,"Pythia WW","F");
    }
    if( ((fileList_[i].second).first).find("WZ")!=string::npos ) {
      h1->SetFillColor(38);
      leg->AddEntry(h1,"Pythia WZ","F");
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
    
    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) * corrFactor_ );

    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }


  aStack->Draw("HIST");
  hData->Draw("PSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("E_{T} sub-leading jet");
  hStack->SetYTitle("Events/(10 GeV/c)");
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

  if(SAVE) c1->SaveAs(Form("Zmm_SubLeadJetEt_%.0f.png",etaCutLow_));
}


void plotZPt(  FileList fileList_  , float Lumi_ = 30 , float cutOff_ = 30., float etaCutLow_=0.0, float etaCutHigh_=5.0,float corrFactor_ = 1.0){


  TCanvas *c1 = new TCanvas(Form("c1ZPt%f",etaCutLow_),"",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad(Form("pad1ZPt%f",etaCutLow_),"",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad(Form("pad2ZPt%f",etaCutLow_),"",0.05,0.02,0.96,0.26);
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
  leg->SetHeader( Form("E^{j1,j2}_{T}>%.0f GeV, |#eta^{j1,j2}|<%.1f",cutOff_,etaCutHigh_) );
  
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
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", 12 ,10, 250);

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
      leg->AddEntry(h1,"Pythia WW","F");
    }
    if( ((fileList_[i].second).first).find("WZ")!=string::npos ) {
      h1->SetFillColor(38);
      leg->AddEntry(h1,"Pythia WZ","F");
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
		       Form("jetsIDP4@.size()>1 && jetsIDP4[0].Et()>%f && jetsIDP4[1].Et()>%f && abs(jetsIDP4[0].eta())<%f && abs(jetsIDP4[0].eta())>%f",cutOff_,cutOff_,etaCutHigh_,etaCutLow_ ) );

    if(((fileList_[i].second).first).find("data")!=string::npos){
      hData = h1;
      hData->Sumw2();
      continue;
    }
    
    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second)) * corrFactor_  );

    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }


  aStack->Draw("HIST");
  hData->Draw("PSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("p_{T}^{#mu#mu}");
  hStack->SetYTitle(Form(" Events/(%.0f GeV)", hStack->GetBinWidth(1) ) );
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

  if(SAVE) c1->SaveAs(Form("Zmm_ZPt_%.0f.png",etaCutLow_));
}





void plotJetVeto(  FileList fileList  , float Lumi_ = 30 , float cutOff_ = 30. , float minDEta_ = 1.0, float maxEta_ = 5.0){

  for(int i = 0; i<fileList.size(); i++){
    (fileList[i].first)->Close();
    delete (fileList[i].first);
  }

  TFile* fData   = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_Mu-Run2010.root","READ");
  TFile* fDYJets = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_DYJets-madgraph-50-PU.root","READ");
  TFile* fTT     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_TT-madgraph-PU.root","READ");
  TFile* fWJets  = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WJets-madgraph-PU.root","READ");
  TFile* fWZ     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WZ-pythia-PU.root","READ");
  TFile* fWW     = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_WW-pythia-PU.root","READ");
  TFile* fT      = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_TToBLNu-tW-madhraph-PU.root","READ");
  TFile* fQCD    = new TFile("/data_CMS/cms/lbianchini/ZmumuPlusJetsStudy/v2/treeZmumuPlusJets_QCD-pythia-PU.root","READ");

  FileList fileList_;
  fileList_.push_back( make_pair(fQCD,    make_pair("QCD",   349988.0 )  ));
  fileList_.push_back( make_pair(fWJets,  make_pair("Wjets",  31314.0 )  ));
  fileList_.push_back( make_pair(fT,      make_pair("tW",        10.6 )  ));
  fileList_.push_back( make_pair(fWZ,     make_pair("WZ",        10.4 )  ));
  fileList_.push_back( make_pair(fWW   ,  make_pair("WW"   ,      2.9 )  ));
  fileList_.push_back( make_pair(fTT,     make_pair("ttbar",    157.5 )  ));
  fileList_.push_back( make_pair(fDYJets, make_pair("Zjets",   3048.0 )  ));
  fileList_.push_back( make_pair(fData,   make_pair("data",     -99   )  ));

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

  TLegend* leg = new TLegend(0.15,0.6,0.50,0.88,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader( Form("E^{j1,j2}_{T}>%.0f GeV/c ; |#eta_{j1}-#eta_{j2}|>%.1f ; (#eta_{j1,j2}+ 0.5) < #eta_{j3} < (#eta_{j2,j1}- 0.5)",cutOff_,minDEta_) );

  //char* ptCut[3] = {"25","30","35"};
  int ptCut[3] = {5,10,15};
 
  float numPV[3] = {1,2,5};
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

  for(unsigned int i = 0 ; i< 3; i++){ // pt cut

    for(unsigned int j = 0 ; j< 3; j++){ // PV
      
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
   
      float lastJ = (j<2) ? numPV[j+1] : 99;
	
      for(unsigned int f = 0 ; f < fileList_.size() ; f++){ // file
	
	TFile* currentFile = (TFile*)fileList_[f].first ;
	if( currentFile->IsZombie() ) continue;
	TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
	float totalEvents = allEvents->GetBinContent(1);
	
	TTree* currentTree = (TTree*)currentFile->Get("zPlusJetsAnalyzer/tree");
	//string h1Name = "h1_"+(fileList_[f].second).first;
	TH1F* h1Fail = new TH1F( "h1Fail" ,"", 1 ,55, 125);
	TH1F* h1Norm = new TH1F( "h1Norm" ,"", 1 ,55, 125);

	int nEntries = currentTree->GetEntries() ;

	std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets;
	float numPVs;
	currentTree->SetBranchAddress("jetsIDP4",&jets);
	currentTree->SetBranchAddress("numPV",&numPVs);
	
	for (int n = 0; n < nEntries ; n++) {
	  currentTree->GetEntry(n);
	
	  if( jets->size()>1 && 
	      (*jets)[0].Et()> cutOff_ &&
	      (*jets)[1].Et()> cutOff_ && 
	      abs((*jets)[0].Eta())<maxEta_ && 
	      abs((*jets)[1].Eta())<maxEta_ &&
	      abs((*jets)[0].eta()-(*jets)[1].eta())> minDEta_ &&
	      numPVs>=numPV[j] && numPVs<lastJ
	      ) h1Norm->Fill(90.);
	 
	  bool jetVeto=false;
	  for(unsigned k=0; k < jets->size(); k++){
	    if(k>1 && 
	       (  ((*jets)[k].Eta()>(*jets)[1].Eta()+0.5 &&  (*jets)[k].Eta()<(*jets)[0].Eta()-0.5) || 
		  ((*jets)[k].Eta()>(*jets)[0].Eta()+0.5 &&  (*jets)[k].Eta()<(*jets)[1].Eta()-0.5) ) &&
	       (*jets)[k].Et()>ptCut[i]
	       )  jetVeto=true;  
	  }

	  if( jets->size()>1 && 
	      (*jets)[0].Et()> cutOff_ &&
	      (*jets)[1].Et()> cutOff_ && 
	      abs((*jets)[0].Eta())<maxEta_ && 
	      abs((*jets)[1].Eta())<maxEta_ &&	    
	      abs((*jets)[0].eta()-(*jets)[1].eta())> minDEta_ &&
	      numPVs>=numPV[j] && numPVs<lastJ &&
	      jetVeto
	      ) h1Fail->Fill(90.);
	}

	


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

      cout << "MC: N of PV in [" <<  numPV[j] << "," << lastJ << "[  ===> " << fail_i_j << "   /   " << norm_i_j << endl;
      cout << "DATA: N of PV in [" <<  numPV[j] << "," << lastJ << "[  ===> " << failD_i_j << "   /   " << normD_i_j << endl;

      if(i==0){
	vyPV15[j]  =  1-fail_i_j/norm_i_j;
	veyPV15[j] = sqrt( vyPV15[j]*(1-vyPV15[j]) / norm_i_j_num);
	vyDPV15[j]  =  1-failD_i_j/normD_i_j;
	veyDPV15[j] = sqrt( vyDPV15[j]*(1-vyDPV15[j]) / normD_i_j_num);
      }
      if(i==1){
	vyPV25[j] =  1-fail_i_j/norm_i_j;
	veyPV25[j] = sqrt( vyPV25[j]*(1-vyPV25[j]) / norm_i_j_num);
	vyDPV25[j]  =  1-failD_i_j/normD_i_j;
	veyDPV25[j] = sqrt( vyDPV25[j]*(1-vyDPV25[j]) / normD_i_j_num);
      }
      if(i==2) {
	vyPV35[j] =  1-fail_i_j/norm_i_j;
	veyPV35[j] = sqrt( vyPV35[j]*(1-vyPV35[j]) / norm_i_j_num);
	vyDPV35[j]  =  1-failD_i_j/normD_i_j;
	veyDPV35[j] = sqrt( vyDPV35[j]*(1-vyDPV35[j]) / normD_i_j_num);
      }

    }//end j

  }//end i

 
  
  TH2F* hMaster = new TH2F("hMaster",Form("CMS Preliminary 2010   #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_),3,0,3,10,0.4,1.2);

  hMaster->SetXTitle("num PV");
  hMaster->SetYTitle( "jet veto efficiency" );
  hMaster->SetTitleSize(0.08,"X");
  hMaster->SetTitleSize(0.08,"Y");
  hMaster->SetTitleOffset(0.85,"Y");
  hMaster->SetLabelSize(0.4,"X");
  hMaster->SetLabelSize(0.4,"Y");
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
  mg->Add(graph35,"3");

  mg->Add(graphD15,"P");
  mg->Add(graphD25,"P");
  mg->Add(graphD35,"P");

  mg->Draw("A");
  mg->SetTitle(hMaster->GetTitle());
  mg->GetXaxis()->SetTitle("Num PV");
  mg->GetYaxis()->SetTitle("jet veto efficiency");
  mg->GetXaxis()->Set(3,0,3);
  mg->GetXaxis()->SetBinLabel(1,"1");
  mg->GetXaxis()->SetBinLabel(2,"2-4");
  mg->GetXaxis()->SetBinLabel(3,">4");
  mg->GetYaxis()->SetRangeUser(0.0,1.25);
  mg->Draw("A");


  leg->AddEntry(graph15, "E_{T}^{veto}=5 GeV MC","F");
  leg->AddEntry(graphD15,"E_{T}^{veto}=5 GeV DATA","lep");
  leg->AddEntry(graph25, "E_{T}^{veto}=10 GeV MC","F");
  leg->AddEntry(graphD25,"E_{T}^{veto}=10 GeV DATA","lep");
  leg->AddEntry(graph35, "E_{T}^{veto}=15 GeV MC","F");
  leg->AddEntry(graphD35,"E_{T}^{veto}=15 GeV DATA","lep");

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
  hRatio->SetLabelSize(0.13,"X");
  hRatio->SetLabelSize(0.13,"Y");

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
  hRatio15->SetBinError(1,  ((graphD15->GetErrorY(0)/numY)+(graph15->GetErrorY(0)/denY))* numY/denY  );
  graphD15->GetPoint(1,numX,numY);
  graph15->GetPoint(1,denX,denY);
  cout<<numY<< "  " << denY << endl;
  hRatio15->SetBinContent(2,numY/denY-1);
  hRatio15->SetBinError(2,  ((graphD15->GetErrorY(1)/numY)+(graph15->GetErrorY(1)/denY))* numY/denY );
  graphD15->GetPoint(2,numX,numY);
  graph15->GetPoint(2,denX,denY);
  cout<<numY<< "  " << denY << endl;
  hRatio15->SetBinContent(3,numY/denY-1);
  hRatio15->SetBinError(3,  ((graphD15->GetErrorY(2)/numY)+(graph15->GetErrorY(2)/denY))* numY/denY);

  TH1F* hRatio25 =  (TH1F*)hRatio->Clone("hRatio25");
  hRatio25->SetMarkerStyle(kFullTriangleDown);
  hRatio25->SetMarkerSize(1.2);
  hRatio25->SetMarkerColor(kBlue);

  graphD25->GetPoint(0,numX,numY);
  graph25->GetPoint(0,denX,denY);
  hRatio25->SetBinContent(1,numY/denY-1);
  hRatio25->SetBinError(1,  ((graphD25->GetErrorY(0)/numY)+(graph25->GetErrorY(0)/denY))* numY/denY);
  graphD25->GetPoint(1,numX,numY);
  graph25->GetPoint(1,denX,denY);
  hRatio25->SetBinContent(2,numY/denY-1);
  hRatio25->SetBinError(2,  ((graphD25->GetErrorY(1)/numY)+(graph25->GetErrorY(1)/denY))* numY/denY);
  graphD25->GetPoint(2,numX,numY);
  graph25->GetPoint(2,denX,denY);
  hRatio25->SetBinContent(3,numY/denY-1);
  hRatio25->SetBinError(3,  ((graphD25->GetErrorY(2)/numY)+(graph25->GetErrorY(2)/denY))* numY/denY);

  TH1F* hRatio35 =  (TH1F*)hRatio->Clone("hRatio35");
  hRatio35->SetMarkerStyle(kFullCircle);
  hRatio35->SetMarkerSize(1.2);
  hRatio35->SetMarkerColor(kMagenta);

  graphD35->GetPoint(0,numX,numY);
  graph35->GetPoint(0,denX,denY);
  hRatio35->SetBinContent(1,numY/denY-1);
  hRatio35->SetBinError(1,  ((graphD35->GetErrorY(0)/numY)+(graph35->GetErrorY(0)/denY))* numY/denY);
  graphD35->GetPoint(1,numX,numY);
  graph35->GetPoint(1,denX,denY);
  hRatio35->SetBinContent(2,numY/denY-1);
  hRatio35->SetBinError(2,  ((graphD35->GetErrorY(1)/numY)+(graph35->GetErrorY(1)/denY))* numY/denY);
  graphD35->GetPoint(2,numX,numY);
  graph35->GetPoint(2,denX,denY);
  hRatio35->SetBinContent(3,numY/denY-1);
  hRatio35->SetBinError(3,  ((graphD35->GetErrorY(2)/numY)+(graph35->GetErrorY(2)/denY))* numY/denY);

  hRatio15->SetAxisRange(-1,0.5,"Y");
  hRatio15->Draw("P");
  hRatio25->Draw("PSAME");
  hRatio35->Draw("PSAME");

  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hRatio->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");

  if(SAVE) c1->SaveAs(Form("Zmm_JetVeto_%.0f_%.0fDEta.png",cutOff_,minDEta_));
}



////////////////////////////////////////////////////


void mainPlot(){

  FileList dummyFileList;
  dummyFileList.clear();
  //plotJetMultiplicity( dummyFileList, 36. , 30.,  2.4, 0.9795 );
  plotJetMultiplicity( dummyFileList, 36. , 30.,  4.5, 0.9795 );
  //plotDEta( dummyFileList, 36. , 30.,  4.5, 0.9795 );
  //plotDEta( dummyFileList, 36. , 30.,  2.4, 0.9795 );
  //plotMass( dummyFileList, 36. , 30.,  2.4, 0.9795 );
  //plotMass( dummyFileList, 36. , 30.,  4.5, 0.9795 );
  //plotBtagInMassBump( dummyFileList, 36. , 30.,  4.5, 0.9795 );
  //plotJetVeto( dummyFileList , 36 , 30. , 2.4, 4.5);

  return;

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
  fileList.push_back( make_pair(fT,      make_pair("tW",        10.6 )  ));
  fileList.push_back( make_pair(fWZ,     make_pair("WZ",        10.4 )  ));
  fileList.push_back( make_pair(fWW   ,  make_pair("WW"   ,      2.9 )  ));
  fileList.push_back( make_pair(fTT,     make_pair("ttbar",    157.5 )  ));
  fileList.push_back( make_pair(fDYJets, make_pair("Zjets",   3048.0 )  ));
  fileList.push_back( make_pair(fData,   make_pair("data",     -99   )  ));
 
  //plotZPt( fileList, 36. , 30 ,  0.0, 4.5,0.9795);
  // plotLeadingJetEt( fileList, 36. ,    0.0, 2.4,0.9795);
  // plotSubLeadingJetEt( fileList, 36. , 0.0, 2.4,0.9795);
  //plotLeadingJetEt( fileList, 36. ,    3.0, 4.5,0.9795);
  //plotSubLeadingJetEt( fileList, 36. , 3.0, 4.5,0.9795);
  
}

