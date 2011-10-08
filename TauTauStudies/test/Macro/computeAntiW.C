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


void plot(int steps_ = 40){


  TCanvas *c1 = new TCanvas("c1","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);

  TLegend* leg = new TLegend(0.55,0.65,0.80,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);

  TFile *fSgn = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011_iter2/Inclusive/nTupleVBFH135-powheg-PUS1_Open_ElecTauStream.root","READ");
  TFile *fBkg = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011_iter2/Inclusive/nTupleWJets-madgraph-PUS1_Open_ElecTauStream.root","READ");

  if((fSgn->IsZombie()) || (fBkg->IsZombie()) ){
    cout << "No file" << endl;
    exit(1);
  }

  float vx_Mt[steps_];
  float vy_Mt[steps_];
  float vx_pZeta00[steps_];
  float vy_pZeta00[steps_];
  float vx_pZeta05[steps_];
  float vy_pZeta05[steps_];
  float vx_pZeta10[steps_];
  float vy_pZeta10[steps_];
  float vx_pZeta15[steps_];
  float vy_pZeta15[steps_];
  float vx_pZeta20[steps_];
  float vy_pZeta20[steps_];

  TH2F* hMaster = new TH2F("hMaster"," ; qqH(115)#rightarrow #tau#tau efficiency ; W+jets efficiency", 100,0,1,100,0,1);

  TTree* treeSgn = (TTree*)fSgn->Get("outTreePtOrd");
  TTree* treeBkg = (TTree*)fBkg->Get("outTreePtOrd");

  TFile* dummy = new TFile("dummy.root","RECREATE");
  TTree* treeSgnCut = (TTree*)treeSgn->CopyTree("pt1>25 && pt2>25 && eta1*eta2<0");  
  TTree* treeBkgCut = (TTree*)treeBkg->CopyTree("pt1>25 && pt2>25 && eta1*eta2<0");  
  //TTree* treeSgnCut = (TTree*)treeSgn->CopyTree("tightestHPSWP>0");  
  //TTree* treeBkgCut = (TTree*)treeBkg->CopyTree("tightestHPSWP>0");  

  cout << treeSgnCut->GetEntries() << endl;
  cout << treeBkgCut->GetEntries() << endl;

  std::vector<string> antiW;
  antiW.push_back("MtLeg1");
  antiW.push_back("pZeta-0.0*pZetaVis");
  antiW.push_back("pZeta-0.5*pZetaVis");
  antiW.push_back("pZeta-1.0*pZetaVis");
  antiW.push_back("pZeta-1.5*pZetaVis");
  antiW.push_back("pZeta-2.0*pZetaVis");

  TH1F* h1 = new TH1F("h1","",1,-10,10);

  for(unsigned int m = 0; m < antiW.size() ; m++){
    
    treeSgnCut->Draw( "eta1>>h1" ,"puWeight");
    float sgnDen = (float)h1->Integral();
    cout << sgnDen << endl;
    h1->Reset();
    treeBkgCut->Draw( "eta1>>h1" ,"puWeight");
    float bkgDen = (float)h1->Integral();
    cout << bkgDen << endl;
    h1->Reset();
    
    for(int i = 0; i < steps_; i++){
	
      cout << antiW[m] << " --> " << i << endl;
      
      float step_i = antiW[m].find("Mt")!=string::npos ? 
	i*(100./steps_) : i*(160/steps_);

      TCut cut1( Form("puWeight*(%s<=%f)",  antiW[m].c_str(), step_i ) );
      TCut cut2( Form("puWeight*(%s>=(80-%f))", antiW[m].c_str(), step_i ) );

      TCut cut = antiW[m].find("Mt")!=string::npos ? cut1 : cut2;

      treeSgnCut->Draw( "eta1>>h1" ,cut );
      float sgnNum = (float)h1->Integral();
      cout << sgnNum << endl;
      h1->Reset();
      treeBkgCut->Draw( "eta1>>h1" ,cut );
      float bkgNum = (float)h1->Integral();
      cout << bkgNum << endl;
      h1->Reset();
 
      if(m==0){
	vx_Mt[i] = sgnNum/sgnDen;
	vy_Mt[i] = bkgNum/bkgDen;
      }
      if(m==1){
	vx_pZeta00[i] = sgnNum/sgnDen;
	vy_pZeta00[i] = bkgNum/bkgDen;
      }
      if(m==2){
	vx_pZeta05[i] = sgnNum/sgnDen;
	vy_pZeta05[i] = bkgNum/bkgDen;
      }
      if(m==3){
	vx_pZeta10[i] = sgnNum/sgnDen;
	vy_pZeta10[i] = bkgNum/bkgDen;
      }
      if(m==4){
	vx_pZeta15[i] = sgnNum/sgnDen;
	vy_pZeta15[i] = bkgNum/bkgDen;
      }
      if(m==5){
	vx_pZeta20[i] = sgnNum/sgnDen;
	vy_pZeta20[i] = bkgNum/bkgDen;
      }
    }
  }

  leg->SetHeader("ROC for anti-W cuts after VBF-preselection");

  hMaster->Draw();
  
  TGraph* graph_Mt    = new TGraph(steps_,vx_Mt,vy_Mt);
  TGraph* graph_pZeta00 = new TGraph(steps_,vx_pZeta00,vy_pZeta00);
  TGraph* graph_pZeta05 = new TGraph(steps_,vx_pZeta05,vy_pZeta05);
  TGraph* graph_pZeta10 = new TGraph(steps_,vx_pZeta10,vy_pZeta10);
  TGraph* graph_pZeta15 = new TGraph(steps_,vx_pZeta15,vy_pZeta15);
  TGraph* graph_pZeta20 = new TGraph(steps_,vx_pZeta20,vy_pZeta20);

  std::map<std::string, TGraph*> roc;
  roc["MtLeg1"]      = graph_Mt;
  roc["pZeta-0.0*pZetaVis"] = graph_pZeta00;
  roc["pZeta-0.5*pZetaVis"] = graph_pZeta05;
  roc["pZeta-1.0*pZetaVis"] = graph_pZeta10;
  roc["pZeta-1.5*pZetaVis"] = graph_pZeta15;
  roc["pZeta-2.0*pZetaVis"] = graph_pZeta20;

  for(unsigned int k = 0; k < antiW.size() ; k++){
    if(k==0){
      roc[antiW[k]]->SetMarkerColor(kBlue);
      roc[antiW[k]]->SetMarkerSize(1.4);
      roc[antiW[k]]->SetMarkerStyle(kOpenCircle);
      roc[antiW[k]]->GetYaxis()->SetRangeUser(0.80,1.1);
      leg->AddEntry(roc[antiW[k]],"Mt","P");
      roc[antiW[k]]->Draw("P");
    }
    if(k==1){
      roc[antiW[k]]->SetMarkerColor(kBlack);
      roc[antiW[k]]->SetMarkerSize(1.4);
      roc[antiW[k]]->SetMarkerStyle(kOpenSquare);
      leg->AddEntry(roc[antiW[k]],"pZeta","P");
      roc[antiW[k]]->Draw("PSAME");
    }
    if(k==2){
      roc[antiW[k]]->SetMarkerColor(kRed);
      roc[antiW[k]]->SetMarkerSize(1.4);
      roc[antiW[k]]->SetMarkerStyle(kOpenSquare);
      leg->AddEntry(roc[antiW[k]],"pZeta-0.5pZetaVis","P");
      roc[antiW[k]]->Draw("PSAME");
    }
    if(k==3){
      roc[antiW[k]]->SetMarkerColor(kGreen);
      roc[antiW[k]]->SetMarkerSize(1.4);
      roc[antiW[k]]->SetMarkerStyle(kOpenSquare);
      leg->AddEntry(roc[antiW[k]],"pZeta-1.0pZetaVis","P");
      roc[antiW[k]]->Draw("PSAME");
    }
    if(k==4){
      roc[antiW[k]]->SetMarkerColor(kMagenta);
      roc[antiW[k]]->SetMarkerSize(1.4);
      roc[antiW[k]]->SetMarkerStyle(kOpenSquare);
      leg->AddEntry(roc[antiW[k]],"pZeta-1.5pZetaVis","P");
      roc[antiW[k]]->Draw("PSAME");
    }
    if(k==5){
      roc[antiW[k]]->SetMarkerColor(kYellow);
      roc[antiW[k]]->SetMarkerSize(1.4);
      roc[antiW[k]]->SetMarkerStyle(kOpenSquare);
      leg->AddEntry(roc[antiW[k]],"pZeta-2.0pZetaVis","P");
      roc[antiW[k]]->Draw("PSAME");
    }
   
  }

  leg->Draw();
  
}


void plot2D(){


  TCanvas *c1 = new TCanvas("c1","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);

  TLegend* leg = new TLegend(0.55,0.65,0.80,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  
  TFile *fSgn = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleVBFH115-Mu-powheg-PUS1_Open_MuTauStream.root","READ");
  TFile *fBkg = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleWJets-Mu-madgraph-PUS1_Open_MuTauStream.root","READ");

  if((fSgn->IsZombie()) || (fBkg->IsZombie()) ){
    cout << "No file" << endl;
    exit(1);
  }

  TH2F* sgn = new TH2F("sgn","qqH(115)#rightarrow#tau#tau ; pZetaVis (GeV); pZeta (GeV)",100,0,200,100,-200,200);
  TH2F* bkg = new TH2F("bkg","W+jets ; pZetaVis (GeV); pZeta (GeV)",100,0,200,100,-200,200);

  sgn->SetMarkerColor(kRed);
  sgn->SetMarkerStyle(kOpenCircle);
  sgn->SetMarkerSize(0.5);
  bkg->SetMarkerColor(kBlue);
  bkg->SetMarkerStyle(kOpenSquare);
  bkg->SetMarkerSize(0.5);

  TTree* treeSgn = (TTree*)fSgn->Get("outTreePtOrd");
  TTree* treeBkg = (TTree*)fBkg->Get("outTreePtOrd");

  c1->Divide(2,1);

  c1->cd(1);
  treeSgn->Draw("pZeta:pZetaVis>>sgn","tightestHPSWP>0 && pt1>25 && pt2>20 && eta1*eta2<0","COLZ");
  c1->cd(2);
  treeBkg->Draw("pZeta:pZetaVis>>bkg","pt1>25 && pt2>20 && eta1*eta2<0","COLZ");

}
