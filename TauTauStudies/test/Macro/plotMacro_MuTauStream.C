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



void plotInclusiveMass(  FileList fileList  , float Lumi_ = 30) {

  for(int i = 0; i<fileList.size(); i++){
    (fileList[i].first)->Close();
    delete (fileList[i].first);
  }

  TFile* fVBF115 = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_VBFH115.root","READ");
  TFile* fVBF135 = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_VBFH135.root","READ");
  TFile* fDYJets = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_DYJets-madgraph-50-PU.root","READ");
  TFile* fTT     = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_TT-madgraph-PU.root","READ");
  TFile* fWJets  = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_WJets-madgraph-PU.root","READ");
  TFile* fT      = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_TToBLNu-tW-madhraph-PU.root","READ");
  TFile* fQCD    = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_QCD-pythia-PU.root","READ");

  FileList fileList_;
  fileList_.push_back( make_pair(fT,      make_pair("tW",        10.6      )  ));
  fileList_.push_back( make_pair(fQCD,    make_pair("QCD",   349988.0      )  ));
  fileList_.push_back( make_pair(fWJets,  make_pair("Wjets",  31314.0      )  ));
  fileList_.push_back( make_pair(fTT,     make_pair("ttbar",    157.5      )  ));
  fileList_.push_back( make_pair(fDYJets, make_pair("Zjets",   3048.0      )  ));
  fileList_.push_back( make_pair(fVBF115, make_pair("qqH115",       0.1012 )  ));
  fileList_.push_back( make_pair(fVBF135, make_pair("qqH135",       0.05049)  ));
 

  TCanvas *c1 = new TCanvas("c1InclusiveMass","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad("pad1InclusiveMass","",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad("pad2InclusiveMass","",0.05,0.02,0.96,0.26);
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
  leg->SetHeader( "#mu+#tau Inclusive" );
  
  THStack* aStack = new THStack("aStack",Form("CMS Preliminary 2010    #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_));
  TH1F* hqqH115 = new TH1F();
  TH1F* hqqH135 = new TH1F();
  TH1F* hSiml   = new TH1F();

  float signalScale = 1;

  for(unsigned int i = 0 ; i < fileList_.size() ; i++){

    TFile* currentFile = (TFile*)fileList_[i].first ;
    if( currentFile->IsZombie() ) continue;
    TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    TTree* currentTree = (TTree*)currentFile->Get("muTauStreamAnalyzer/tree");
    string h1Name = "h1_"+(fileList_[i].second).first;
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", 28 , 20, 300);

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
      h1->SetFillColor( 44 );
      leg->AddEntry(h1,"MadGraph single-top","F");
    }
    if( ((fileList_[i].second).first).find("QCD")!=string::npos ) {
      h1->SetFillColor(11);
      leg->AddEntry(h1,"Pythia QCD","F");
    }
    if( ((fileList_[i].second).first).find("qqH115")!=string::npos ) {
      h1->SetLineColor(kBlack);
      h1->SetLineStyle(kDashed);
      h1->SetLineWidth(3.0);
      leg->AddEntry(h1,"VBF H(115)#rightarrow#tau#tau X 100","l");
      signalScale = 100;
    }
    if( ((fileList_[i].second).first).find("qqH135")!=string::npos ) {
      h1->SetLineColor(kBlack);
      h1->SetLineStyle(kDotted);
      h1->SetLineWidth(3.0);
      leg->AddEntry(h1,"VBF H(135)#rightarrow#tau#tau X 100","l");
      signalScale = 100;
    }

    int nEntries = currentTree->GetEntries() ;
    
    std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauSVfit3;
    currentTree->SetBranchAddress("diTauSVfit3P4",&diTauSVfit3);

    for (int n = 0; n < nEntries ; n++) {
      currentTree->GetEntry(n);
      if( diTauSVfit3->size() < 1) continue;
      h1->Fill( (*diTauSVfit3)[0].M() ) ;
    }

    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second * signalScale)) );

    if(((fileList_[i].second).first).find("qqH115")!=string::npos){
      hqqH115=(TH1F*)h1->Clone("hqqH115");
      hqqH115 = h1;
      hqqH115->Sumw2();
      continue;
    }
    if(((fileList_[i].second).first).find("qqH135")!=string::npos){
      hqqH135=(TH1F*)h1->Clone("hqqH135");
      hqqH135 = h1;
      hqqH135->Sumw2();
      continue;
    }

    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }

  //float numData = hData->GetEntries();
  //float numSiml = hSiml->Integral();
  //float dataToSimlRatio = numData/numSiml;
  //cout << "data " << numData << "  ---  simul " << numSiml << endl;  

  aStack->Draw("HIST");
  hqqH115->Draw("HISTSAME");
  hqqH135->Draw("HISTSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("SVfit #tau#tau mass");
  hStack->SetYTitle(Form("Number of events/%.0f GeV",hStack->GetBinWidth(1)));
  hStack->SetTitleSize(0.05,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.75,"Y");
  leg->Draw();

  pad2->cd();
  TH1F* hRatio = new TH1F("hRatio", " ; ; purity",
			  hStack->GetNbinsX(), 
			  hStack->GetXaxis()->GetXmin(), hStack->GetXaxis()->GetXmax());
  hRatio->SetLineStyle(kDashed);
  hRatio->SetLineWidth(1.0);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  TH1F* hqqH115Clone = (TH1F*)hqqH115->Clone("hqqH115Clone");
  TH1F* hqqH135Clone = (TH1F*)hqqH135->Clone("hqqH135Clone");
  hqqH115Clone->Divide( hqqH115 ,hSiml,1./signalScale,1.0);
  hqqH135Clone->Divide( hqqH135 ,hSiml,1./signalScale,1.0);
  hRatio->SetAxisRange(0.,0.001,"Y");

  hRatio->Draw();
  hqqH115Clone->Draw("HISTSAME");
  hqqH135Clone->Draw("HISTSAME");

  if(SAVE) c1->SaveAs("Zmm_InclusiveMass.png");

}


void plotExclusiveMass(  FileList fileList , float Lumi_ = 30, 
			 float pt1_ = 20., float pt2_ = 15, float DEta_ = 1.0, float Mjj_ = 300. , float jetVeto_ = 15,
			 float signalScale_ = 1.0) {

  for(int i = 0; i<fileList.size(); i++){
    (fileList[i].first)->Close();
    delete (fileList[i].first);
  }

  TFile* fVBF115 = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_VBFH115.root","READ");
  TFile* fVBF135 = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_VBFH135.root","READ");
  TFile* fDYJets = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_DYJets-madgraph-50-PU.root","READ");
  TFile* fTT     = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_TT-madgraph-PU.root","READ");
  TFile* fWJets  = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_WJets-madgraph-PU.root","READ");
  TFile* fT      = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_TToBLNu-tW-madhraph-PU.root","READ");
  TFile* fQCD    = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_QCD-pythia-PU.root","READ");

  FileList fileList_;
  fileList_.push_back( make_pair(fT,      make_pair("tW",        10.6      )  ));
  fileList_.push_back( make_pair(fQCD,    make_pair("QCD",   349988.0      )  ));
  fileList_.push_back( make_pair(fWJets,  make_pair("Wjets",  31314.0      )  ));
  fileList_.push_back( make_pair(fTT,     make_pair("ttbar",    157.5      )  ));
  fileList_.push_back( make_pair(fDYJets, make_pair("Zjets",   3048.0      )  ));
  fileList_.push_back( make_pair(fVBF115, make_pair("qqH115",       0.1012 )  ));
  fileList_.push_back( make_pair(fVBF135, make_pair("qqH135",       0.05049)  ));
 

  TCanvas *c1 = new TCanvas("c1ExclusiveMass","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(0);


  TPad* pad1 = new TPad("pad1ExclusiveMass","",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad("pad2ExclusiveMass","",0.05,0.02,0.96,0.26);
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(0);

  TLegend* leg = new TLegend(0.60,0.47,0.90,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader( "#mu-#tau + VBF 1% + CJV" );
  
  THStack* aStack = new THStack("aStack",Form("CMS Preliminary 2010    #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_));
  TH1F* hqqH115 = new TH1F();
  TH1F* hqqH135 = new TH1F();
  TH1F* hSiml   = new TH1F();

  float signalScale = 1;

  for(unsigned int i = 0 ; i < fileList_.size() ; i++){

    signalScale = 1;

    TFile* currentFile = (TFile*)fileList_[i].first ;
    if( currentFile->IsZombie() ) continue;
    TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    TTree* currentTree = (TTree*)currentFile->Get("muTauStreamAnalyzer/tree");
    string h1Name = "h1_"+(fileList_[i].second).first;
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", 14 , 20, 300);

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
      h1->SetFillColor( 44 );
      leg->AddEntry(h1,"MadGraph single-top","F");
    }
    if( ((fileList_[i].second).first).find("QCD")!=string::npos ) {
      h1->SetFillColor(11);
      leg->AddEntry(h1,"Pythia QCD","F");
    }
    if( ((fileList_[i].second).first).find("qqH115")!=string::npos ) {
      h1->SetLineColor(kBlack);
      h1->SetLineStyle(kDashed);
      h1->SetLineWidth(3.0);
      signalScale = signalScale_;
      leg->AddEntry(h1,Form("VBF H(115)#rightarrow#tau#tau X %.0f",signalScale),"l");
    }
    if( ((fileList_[i].second).first).find("qqH135")!=string::npos ) {
      h1->SetLineColor(kBlack);
      h1->SetLineStyle(kDotted);
      h1->SetLineWidth(3.0);
      signalScale = signalScale_;
      leg->AddEntry(h1,Form("VBF H(135)#rightarrow#tau#tau X %.0f",signalScale),"l");
    }

    int nEntries = currentTree->GetEntries() ;
    
    std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauSVfit3;
    std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets;
    
    currentTree->SetBranchAddress("diTauSVfit3P4",&diTauSVfit3);
    currentTree->SetBranchAddress("jetsIDP4",&jets);

    for (int n = 0; n < nEntries ; n++) {
      currentTree->GetEntry(n);
      if( diTauSVfit3->size() < 1) continue;
      bool vbfCut = false;
      bool jetVeto = false;
      if( jets->size()>1 && 
	  (*jets)[0].Et()>pt1_ && (*jets)[1].Et()>pt2_ && 
	  abs((*jets)[0].Eta()-(*jets)[1].Eta())>DEta_ && 
	  ((*jets)[0]+(*jets)[1]).M()>Mjj_ ) vbfCut = true;
      for(unsigned k=0; k < jets->size(); k++){
	if(k>1 && 
	   (  ((*jets)[k].Eta()>(*jets)[1].Eta()+0.5 &&  (*jets)[k].Eta()<(*jets)[0].Eta()-0.5) || 
	      ((*jets)[k].Eta()>(*jets)[0].Eta()+0.5 &&  (*jets)[k].Eta()<(*jets)[1].Eta()-0.5) ) &&
	   (*jets)[k].Et()>jetVeto_ ) jetVeto=true;  
      }

      if(vbfCut && !jetVeto)  h1->Fill( (*diTauSVfit3)[0].M() ) ;
      
    }

    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second * signalScale)) );
    //cout<< "Scaling " << (fileList_[i].second).first  << " by " << (Lumi_/ (totalEvents/((fileList_[i].second).second * signalScale))) << endl;
    cout<< "Exp. contribution from " << (fileList_[i].second).first  
	<< " in the sgn region " << h1->Integral()/signalScale 
	<< " +/- " << sqrt(h1->GetEntries())*Lumi_/(totalEvents/((fileList_[i].second).second)) << endl;

    if(((fileList_[i].second).first).find("qqH115")!=string::npos){
      hqqH115=(TH1F*)h1->Clone("hqqH115");
      hqqH115 = h1;
      hqqH115->Sumw2();
      continue;
    }
    if(((fileList_[i].second).first).find("qqH135")!=string::npos){
      hqqH135=(TH1F*)h1->Clone("hqqH135");
      hqqH135 = h1;
      hqqH135->Sumw2();
      continue;
    }

    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }

  //float numData = hData->GetEntries();
  //float numSiml = hSiml->Integral();
  //float dataToSimlRatio = numData/numSiml;
  //cout << "data " << numData << "  ---  simul " << numSiml << endl;  

  aStack->Draw("HIST");
  hqqH115->Draw("HISTSAME");
  hqqH135->Draw("HISTSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("SVfit #tau#tau mass");
  hStack->SetYTitle(Form("Number of events/%.0f GeV",hStack->GetBinWidth(1)));
  hStack->SetTitleSize(0.05,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.75,"Y");
  leg->Draw();

  pad2->cd();
  TH1F* hRatio = new TH1F("hRatio", " ; ; purity",
			  hStack->GetNbinsX(), 
			  hStack->GetXaxis()->GetXmin(), hStack->GetXaxis()->GetXmax());
  hRatio->SetLineStyle(kDashed);
  hRatio->SetLineWidth(1.0);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  TH1F* hqqH115Clone = (TH1F*)hqqH115->Clone("hqqH115Clone");
  TH1F* hqqH135Clone = (TH1F*)hqqH135->Clone("hqqH135Clone");
  hqqH115Clone->Divide( hqqH115 ,hSiml,1./signalScale,1.0);
  hqqH135Clone->Divide( hqqH135 ,hSiml,1./signalScale,1.0);
  hRatio->SetAxisRange(0.,1,"Y");

  hRatio->Draw();
  hqqH115Clone->Draw("HISTSAME");
  hqqH135Clone->Draw("HISTSAME");

  if(SAVE) c1->SaveAs("MuTauStream_ExclusiveMass.png");

}



void plotCutFlow(  FileList fileList , float Lumi_ = 30, 
		   float pt1_ = 20., float pt2_ = 15, float DEta_ = 1.0, float Mjj_ = 300. , float jetVeto_ = 15,
		   float signalScale_ = 1.0) {
  
  for(int i = 0; i<fileList.size(); i++){
    (fileList[i].first)->Close();
    delete (fileList[i].first);
  }

  TFile* fVBF115 = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_VBFH115.root","READ");
  TFile* fVBF135 = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_VBFH135.root","READ");
  TFile* fDYJets = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_DYJets-madgraph-50-PU.root","READ");
  TFile* fTT     = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_TT-madgraph-PU.root","READ");
  TFile* fWJets  = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_WJets-madgraph-PU.root","READ");
  TFile* fT      = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_TToBLNu-tW-madhraph-PU.root","READ");
  TFile* fQCD    = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_QCD-pythia-PU.root","READ");

  FileList fileList_;
  fileList_.push_back( make_pair(fT,      make_pair("tW",        10.6      )  ));
  fileList_.push_back( make_pair(fTT,     make_pair("ttbar",    157.5      )  ));
  fileList_.push_back( make_pair(fDYJets, make_pair("Zjets",   3048.0      )  ));
  fileList_.push_back( make_pair(fWJets,  make_pair("Wjets",  31314.0      )  ));
  fileList_.push_back( make_pair(fQCD,    make_pair("QCD",   349988.0      )  ));
  fileList_.push_back( make_pair(fVBF115, make_pair("qqH115",       0.1012 )  ));
  fileList_.push_back( make_pair(fVBF135, make_pair("qqH135",       0.05049)  ));
 

  TCanvas *c1 = new TCanvas("c1CutFlowMass","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad("pad1CutFlowMass","",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad("pad2CutFlowMass","",0.05,0.02,0.96,0.26);
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
  leg->SetHeader( "Cut Flow" );
  
  THStack* aStack = new THStack("aStack",Form("CMS Preliminary 2010    #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_));
  TH1F* hqqH115 = new TH1F();
  TH1F* hqqH135 = new TH1F();
  TH1F* hSiml   = new TH1F();

  vector< pair<string,string> > cutList;
  cutList.push_back(make_pair("vertexScarpingFilter/totalEvents","good vertex"));
  cutList.push_back(make_pair("oneMuonFilter/totalEvents","one muon"));
  cutList.push_back(make_pair("noElecFilter/totalEvents","electron veto"));
  cutList.push_back(make_pair("muonLegFilter/totalEvents","#mu cuts"));
  cutList.push_back(make_pair("tauLegFilter/totalEvents","#tau cuts"));
  cutList.push_back(make_pair("atLeastOneDiTauFilter/totalEvents","OS + m_{T} cut"));
  vector<float> cutFlow; 

  float signalScale = 1;

  for(unsigned int i = 0 ; i < fileList_.size() ; i++){

    signalScale = 1;

    TFile* currentFile = (TFile*)fileList_[i].first ;
    if( currentFile->IsZombie() ) continue;
    TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    cutFlow.clear();
    for(int m = 0; m<cutList.size(); m++){
      TH1F* tmph1 =  (TH1F*)currentFile->Get( (cutList[m].first).c_str() );
      if(tmph1!=0){
	cutFlow.push_back( tmph1->GetBinContent(1) );
	//cout << cutList[m].first << " ===> " << tmph1->GetBinContent(1) << endl;
      }
    }

    TTree* currentTree = (TTree*)currentFile->Get("muTauStreamAnalyzer/tree");
    string h1Name = "h1_"+(fileList_[i].second).first;
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", (int)(cutFlow.size()+3) ,0 , cutFlow.size()+3);

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
      h1->SetFillColor( 44 );
      leg->AddEntry(h1,"MadGraph single-top","F");
    }
    if( ((fileList_[i].second).first).find("QCD")!=string::npos ) {
      h1->SetFillColor(11);
      leg->AddEntry(h1,"Pythia QCD","F");
    }
    if( ((fileList_[i].second).first).find("qqH115")!=string::npos ) {
      h1->SetLineColor(kBlack);
      h1->SetLineStyle(kDashed);
      h1->SetLineWidth(3.0);
      signalScale = signalScale_;
      leg->AddEntry(h1,Form("VBF H(115)#rightarrow#tau#tau X %.0f",signalScale),"l");
    }
    if( ((fileList_[i].second).first).find("qqH135")!=string::npos ) {
      h1->SetLineColor(kBlack);
      h1->SetLineStyle(kDotted);
      h1->SetLineWidth(3.0);
      signalScale = signalScale_;
      leg->AddEntry(h1,Form("VBF H(135)#rightarrow#tau#tau X %.0f",signalScale),"l");
    }

    h1->SetBinContent(1,totalEvents);
    h1->GetXaxis()->SetBinLabel(1,"#sigma*BR*#int L");  
    for(int m = 0; m<cutFlow.size(); m++){
      h1->SetBinContent(m+2,cutFlow[m]);
      h1->GetXaxis()->SetBinLabel(m+2, (cutList[m].second).c_str() ); 
    }
    h1->GetXaxis()->SetBinLabel(cutFlow.size()+2,"VBF 1%");
    h1->GetXaxis()->SetBinLabel(cutFlow.size()+3,"CJV (15 GeV)");
   
    int nEntries = currentTree->GetEntries() ;    
    
    std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauSVfit3;
    std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets;
    
    currentTree->SetBranchAddress("diTauSVfit3P4",&diTauSVfit3);
    currentTree->SetBranchAddress("jetsIDP4",&jets);
    
    for (int n = 0; n < nEntries ; n++) {
      
      currentTree->GetEntry(n);
   

      if( diTauSVfit3->size() < 1) continue;
      bool vbfCut = false;
      bool jetVeto = false;
      if( jets->size()>1 && 
	  (*jets)[0].Et()>pt1_ && (*jets)[1].Et()>pt2_ && 
	  abs((*jets)[0].Eta()-(*jets)[1].Eta())>DEta_ && 
	  ((*jets)[0]+(*jets)[1]).M()>Mjj_ ) vbfCut = true;
      for(unsigned k=0; k < jets->size(); k++){
	if(k>1 && 
	   (  ((*jets)[k].Eta()>(*jets)[1].Eta()+0.5 &&  (*jets)[k].Eta()<(*jets)[0].Eta()-0.5) || 
	      ((*jets)[k].Eta()>(*jets)[0].Eta()+0.5 &&  (*jets)[k].Eta()<(*jets)[1].Eta()-0.5) ) &&
	   (*jets)[k].Et()>jetVeto_ ) jetVeto=true;  
      }
      
      if(vbfCut)  h1->Fill( cutFlow.size()+1.5 ) ;      
      if(vbfCut && !jetVeto)  h1->Fill( cutFlow.size()+2.5 ) ;      
    
    }
   

    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second * signalScale)) );

    if(((fileList_[i].second).first).find("qqH115")!=string::npos){
      hqqH115=(TH1F*)h1->Clone("hqqH115");
      hqqH115 = h1;
      hqqH115->Sumw2();
      continue;
    }
    if(((fileList_[i].second).first).find("qqH135")!=string::npos){
      hqqH135=(TH1F*)h1->Clone("hqqH135");
      hqqH135 = h1;
      hqqH135->Sumw2();
      continue;
    }

    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

  }

  //float numData = hData->GetEntries();
  //float numSiml = hSiml->Integral();
  //float dataToSimlRatio = numData/numSiml;
  //cout << "data " << numData << "  ---  simul " << numSiml << endl;  

  aStack->Draw("HIST");
  hqqH115->Draw("HISTSAME");
  hqqH135->Draw("HISTSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle("");
  hStack->SetYTitle("Number of events");
  hStack->SetTitleSize(0.05,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.75,"Y");
  hStack->SetAxisRange(1,hSiml->GetMaximum()*1.1,"Y");

  leg->Draw();

  pad2->cd();
  TH1F* hRatio = new TH1F("hRatio", " ; ; purity",
			  hStack->GetNbinsX(), 
			  hStack->GetXaxis()->GetXmin(), hStack->GetXaxis()->GetXmax());
  hRatio->SetLineStyle(kDashed);
  hRatio->SetLineWidth(1.0);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  TH1F* hqqH115Clone = (TH1F*)hqqH115->Clone("hqqH115Clone");
  TH1F* hqqH135Clone = (TH1F*)hqqH135->Clone("hqqH135Clone");
  hqqH115Clone->Divide( hqqH115 ,hSiml,1./signalScale,1.0);
  hqqH135Clone->Divide( hqqH135 ,hSiml,1./signalScale,1.0);
  hRatio->SetAxisRange(0.,0.01,"Y");

  hRatio->Draw();
  hqqH115Clone->Draw("HISTSAME");
  hqqH135Clone->Draw("HISTSAME");

  if(SAVE) c1->SaveAs("Zmm_CutFlowMass.png");

}




void plotCutFlowNotStaggered(  FileList fileList , float Lumi_ = 30, 
		   float pt1_ = 20., float pt2_ = 15, float DEta_ = 1.0, float Mjj_ = 300. , float jetVeto_ = 15,
		   float signalScale_ = 1.0) {
  
  for(int i = 0; i<fileList.size(); i++){
    (fileList[i].first)->Close();
    delete (fileList[i].first);
  }

  TFile* fVBF115 = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_VBFH115.root","READ");
  TFile* fVBF135 = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_VBFH135.root","READ");
  TFile* fDYJets = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_DYJets-madgraph-50-PU.root","READ");
  TFile* fTT     = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_TT-madgraph-PU.root","READ");
  TFile* fWJets  = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_WJets-madgraph-PU.root","READ");
  TFile* fT      = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_TToBLNu-tW-madhraph-PU.root","READ");
  TFile* fQCD    = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_QCD-pythia-PU.root","READ");

  FileList fileList_;
  fileList_.push_back( make_pair(fT,      make_pair("tW",        10.6      )  ));
  fileList_.push_back( make_pair(fTT,     make_pair("ttbar",    157.5      )  ));
  fileList_.push_back( make_pair(fDYJets, make_pair("Zjets",   3048.0      )  ));
  fileList_.push_back( make_pair(fWJets,  make_pair("Wjets",  31314.0      )  ));
  fileList_.push_back( make_pair(fQCD,    make_pair("QCD",   349988.0      )  ));
  fileList_.push_back( make_pair(fVBF115, make_pair("qqH115",       0.1012 )  ));
  fileList_.push_back( make_pair(fVBF135, make_pair("qqH135",       0.05049)  ));
 

  TCanvas *c1 = new TCanvas("c1CutFlowNotStaggeredMass","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);


  TPad* pad1 = new TPad("pad1CutFlowNotStaggeredMass","",0.05,0.27,0.96,0.97);
  TPad* pad2 = new TPad("pad2CutFlowNotStaggeredMass","",0.05,0.02,0.96,0.26);
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
  leg->SetHeader( "Cut Flow" );
  
  vector<TH1F*> histos;
  histos.clear();

  TH1F* hqqH115 = new TH1F();
  TH1F* hqqH135 = new TH1F();
  TH1F* hSiml   = new TH1F();

  vector< pair<string,string> > cutList;
  cutList.push_back(make_pair("vertexScarpingFilter/totalEvents","good vertex"));
  cutList.push_back(make_pair("oneMuonFilter/totalEvents","one gl. muon p_{T}>15 GeV"));
  cutList.push_back(make_pair("noElecFilter/totalEvents","electron veto"));
  cutList.push_back(make_pair("muonLegFilter/totalEvents","#mu cuts"));
  cutList.push_back(make_pair("tauLegFilter/totalEvents","#tau cuts"));
  cutList.push_back(make_pair("atLeastOneDiTauFilter/totalEvents","OS + m_{T} cut"));
  vector<float> cutFlow; 

  float signalScale = 1;

  for(unsigned int i = 0 ; i < fileList_.size() ; i++){

    signalScale = 1;

    TFile* currentFile = (TFile*)fileList_[i].first ;
    if( currentFile->IsZombie() ) continue;
    TH1F* allEvents = (TH1F*)currentFile->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    cutFlow.clear();
    for(int m = 0; m<cutList.size(); m++){
      TH1F* tmph1 =  (TH1F*)currentFile->Get( (cutList[m].first).c_str() );
      if(tmph1!=0){
	cutFlow.push_back( tmph1->GetBinContent(1) );
	//cout << tmph1->GetBinContent(1) << endl;
      }
    }

    TTree* currentTree = (TTree*)currentFile->Get("muTauStreamAnalyzer/tree");
    string h1Name = "h1_"+(fileList_[i].second).first;
    TH1F* h1 = new TH1F( h1Name.c_str() ,"", (int)(cutFlow.size()+3) ,0 , cutFlow.size()+3);

    if( ((fileList_[i].second).first).find("Zjets")!=string::npos ) {
      h1->SetLineColor(kRed);
      leg->AddEntry(h1,"MadGraph Z+jets","F");
    }
    if( ((fileList_[i].second).first).find("ttbar")!=string::npos ) {
      h1->SetLineColor(kBlue);
      leg->AddEntry(h1,"MadGraph t#bar{t}+jets","F");
    }
    if( ((fileList_[i].second).first).find("Wjets")!=string::npos ) {
      h1->SetLineColor(kGreen);
      leg->AddEntry(h1,"MadGraph W+jets","F");
    }
    if( ((fileList_[i].second).first).find("tW")!=string::npos ){
      h1->SetLineColor( 44 );
      leg->AddEntry(h1,"MadGraph single-top","F");
    }
    if( ((fileList_[i].second).first).find("QCD")!=string::npos ) {
      h1->SetLineColor(11);
      leg->AddEntry(h1,"Pythia QCD","F");
    }
    if( ((fileList_[i].second).first).find("qqH115")!=string::npos ) {
      h1->SetLineColor(kBlack);
      h1->SetLineStyle(kDashed);
      h1->SetLineWidth(3.0);
      signalScale = signalScale_;
      leg->AddEntry(h1,Form("VBF H(115)#rightarrow#tau#tau X %.0f",signalScale),"l");
    }
    if( ((fileList_[i].second).first).find("qqH135")!=string::npos ) {
      h1->SetLineColor(kBlack);
      h1->SetLineStyle(kDotted);
      h1->SetLineWidth(3.0);
      signalScale = signalScale_;
      leg->AddEntry(h1,Form("VBF H(135)#rightarrow#tau#tau X %.0f",signalScale),"l");
    }

    h1->SetBinContent(1,totalEvents);
    h1->GetXaxis()->SetBinLabel(1,"#sigma*BR*#int L");  
    for(int m = 0; m<cutFlow.size(); m++){
      h1->SetBinContent(m+2,cutFlow[m]);
      h1->GetXaxis()->SetBinLabel(m+2, (cutList[m].second).c_str() ); 
    }
    h1->GetXaxis()->SetBinLabel(cutFlow.size()+2,"VBF 1%");
    h1->GetXaxis()->SetBinLabel(cutFlow.size()+3,"CJV (15 GeV)");
   
    int nEntries = currentTree->GetEntries() ;    
    
    std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauSVfit3;
    std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets;
    
    currentTree->SetBranchAddress("diTauSVfit3P4",&diTauSVfit3);
    currentTree->SetBranchAddress("jetsIDP4",&jets);
    
    for (int n = 0; n < nEntries ; n++) {
      
      currentTree->GetEntry(n);
   

      if( diTauSVfit3->size() < 1) continue;
      bool vbfCut = false;
      bool jetVeto = false;
      if( jets->size()>1 && 
	  (*jets)[0].Et()>pt1_ && (*jets)[1].Et()>pt2_ && 
	  abs((*jets)[0].Eta()-(*jets)[1].Eta())>DEta_ && 
	  ((*jets)[0]+(*jets)[1]).M()>Mjj_ ) vbfCut = true;
      for(unsigned k=0; k < jets->size(); k++){
	if(k>1 && 
	   (  ((*jets)[k].Eta()>(*jets)[1].Eta()+0.5 &&  (*jets)[k].Eta()<(*jets)[0].Eta()-0.5) || 
	      ((*jets)[k].Eta()>(*jets)[0].Eta()+0.5 &&  (*jets)[k].Eta()<(*jets)[1].Eta()-0.5) ) &&
	   (*jets)[k].Et()>jetVeto_ ) jetVeto=true;  
      }
      
      if(vbfCut)  h1->Fill( cutFlow.size()+1.5 ) ;      
      if(vbfCut && !jetVeto)  h1->Fill( cutFlow.size()+2.5 ) ;      
    
    }
   

    h1->Scale( Lumi_ / (totalEvents/((fileList_[i].second).second * signalScale)) );

    if(((fileList_[i].second).first).find("qqH115")!=string::npos){
      hqqH115=(TH1F*)h1->Clone("hqqH115");
      hqqH115 = h1;
      hqqH115->Sumw2();
      continue;
    }
    if(((fileList_[i].second).first).find("qqH135")!=string::npos){
      hqqH135=(TH1F*)h1->Clone("hqqH135");
      hqqH135 = h1;
      hqqH135->Sumw2();
      continue;
    }

    if(i==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    histos.push_back(h1);

  }

  //float numData = hData->GetEntries();
  //float numSiml = hSiml->Integral();
  //float dataToSimlRatio = numData/numSiml;
  //cout << "data " << numData << "  ---  simul " << numSiml << endl;  

  hqqH115->SetTitle(Form("CMS Preliminary 2010    #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi_));
  hqqH115->SetXTitle("");
  hqqH115->SetYTitle("Number of events");
  hqqH115->SetTitleSize(0.05,"X");
  hqqH115->SetTitleSize(0.05,"Y");
  hqqH115->SetTitleOffset(0.75,"Y");
  hqqH115->SetAxisRange(0.1,hSiml->GetMaximum()*1.2,"Y");

  hqqH115->Draw("HIST");
  hqqH135->Draw("HISTSAME");
  for(int p = 0; p<histos.size(); p++){
    histos[p]->Draw("HISTSAME");
  }


  leg->Draw();

  pad2->cd();
  TH1F* hRatio = new TH1F("hRatio", " ; ; purity",
			  hqqH115->GetNbinsX(), 
			  hqqH115->GetXaxis()->GetXmin(), hqqH115->GetXaxis()->GetXmax());
  hRatio->SetLineStyle(kDashed);
  hRatio->SetLineWidth(1.0);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  TH1F* hqqH115Clone = (TH1F*)hqqH115->Clone("hqqH115Clone");
  TH1F* hqqH135Clone = (TH1F*)hqqH135->Clone("hqqH135Clone");
  hqqH115Clone->Divide( hqqH115 ,hSiml,1./signalScale,1.0);
  hqqH135Clone->Divide( hqqH135 ,hSiml,1./signalScale,1.0);
  hRatio->SetAxisRange(0.,0.1,"Y");

  hRatio->Draw();
  hqqH115Clone->Draw("HISTSAME");
  hqqH135Clone->Draw("HISTSAME");

  if(SAVE) c1->SaveAs("Zmm_CutFlowNotStaggeredMass.png");

}



////////////////////////////////////////////////////


void mainPlot(){

  FileList dummyFileList;
  dummyFileList.clear();
  //plotInclusiveMass( dummyFileList, 500. );
  //plotExclusiveMass( dummyFileList, 500., 32, 22, 4.0, 488, 15., 10); //WP1%
  //plotExclusiveMass( dummyFileList, 500., 26, 21, 2.9, 417, 15., 10); //WP3%
  //plotCutFlow( dummyFileList, 500., 32, 22, 4.0, 488, 15., 10);
  plotCutFlowNotStaggered( dummyFileList, 500., 32, 22, 4.0, 488, 15., 10);
 

  return;

  TFile* fVBF115 = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_VBFH115.root","READ");
  TFile* fVBF135 = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_VBFH135.root","READ");
  TFile* fDYJets = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_DYJets-madgraph-50-PU.root","READ");
  TFile* fTT     = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_TT-madgraph-PU.root","READ");
  TFile* fWJets  = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_WJets-madgraph-PU.root","READ");
  TFile* fT      = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_TToBLNu-tW-madhraph-PU.root","READ");
  TFile* fQCD    = new TFile("/data_CMS/cms/lbianchini/MuTauStream/treeMuTauStream_QCD-pythia-PU.root","READ");

  FileList fileList;
  fileList.push_back( make_pair(fT,      make_pair("tW",        10.6      )  ));
  fileList.push_back( make_pair(fQCD,    make_pair("QCD",   349988.0      )  ));
  fileList.push_back( make_pair(fWJets,  make_pair("Wjets",  31314.0      )  ));
  fileList.push_back( make_pair(fTT,     make_pair("ttbar",    157.5      )  ));
  fileList.push_back( make_pair(fDYJets, make_pair("Zjets",   3048.0      )  ));
  fileList.push_back( make_pair(fVBF115, make_pair("qqH115",       0.1012 )  ));
  fileList.push_back( make_pair(fVBF135, make_pair("qqH135",       0.05049)  ));

 
 
  
  
}

