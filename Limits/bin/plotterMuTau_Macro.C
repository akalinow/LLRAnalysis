#include <cstdlib>
#include <iostream> 
#include <fstream>
#include <map>
#include <string>

#include "TChain.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TPluginManager.h"
#include "TH1F.h"
#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TF1.h"
#include "TLegend.h"
#include "THStack.h"
#include "TCut.h"
#include "TArrayF.h"
#include "TStyle.h"




void plot( string variable_ = "diTauVisMass",
	   string xTitle_   = " ; mass (GeV) ; Events",
	   float binWidth_  = -1,
	   string MtCut_    = "MtLeg1<40",
	   string charge_   = "diTauCharge==0",
	   string outFile_  = "visibleMass"
	   ){

  int debug = 1;

  TCanvas *c1 = new TCanvas("c1","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);

  gStyle->SetPadColor(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetStatColor(0);
  gStyle->SetOptStat(0000000);
  gStyle->SetOptFit(0111);
  gStyle->SetOptTitle(0);
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetPalette(1);


  TFile fData("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamSpring12_30Mar2012/nTupleRun2012A-MuTau-PromptReco-v1-p1-t3_run_Open_MuTauStream.root");
  TFile fDYJets("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamSpring12_30Mar2012/nTupleDYJets-MuTau-50-madgraph-Tarball_Open_MuTauStream.root");
  TFile fWJets("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_09Feb2012/nTupleWJets-MuTau-madgraph-PUS6_run_Open_MuTauStream.root");
  TFile fTTJets("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_09Feb2012/nTupleTTJets-MuTau-madgraph-PUS6_run_Open_MuTauStream.root");

  TFile fData2011(  "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_06Dec2011/nTupleRun2011-MuTau-All_run_Open_MuTauStream.root");
  TFile fDYJets2011("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_06Dec2011/nTupleDYJets-MuTau-50-madgraph-PUS6_run_Open_MuTauStream.root");
  TFile fWJets2011( "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_06Dec2011/nTupleWJets-MuTau-madgraph-PUS6_run_Open_MuTauStream.root");
  TFile fTTJets2011("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_06Dec2011/nTupleTTJets-MuTau-madgraph-PUS6_run_Open_MuTauStream.root");
  //TFile fVBFH130("treeSkimmedMuTau_VBFH130.root");
  //TFile fGGFH130("treeSkimmedMuTau_GGFH130.root");

  TTree* tData   = (TTree*)fData.Get("outTreePtOrd");
  TTree* tDYJets = (TTree*)fDYJets.Get("outTreePtOrd");
  TTree* tWJets  = (TTree*)fWJets.Get("outTreePtOrd");
  TTree* tTTJets = (TTree*)fTTJets.Get("outTreePtOrd");
  //TTree* tVBFH130= (TTree*)fVBFH130.Get("outTreePtOrd");
  //TTree* tGGFH130= (TTree*)fGGFH130.Get("outTreePtOrd");

  TTree* tData2011   = (TTree*)fData2011.Get("outTreePtOrd");
  TTree* tDYJets2011 = (TTree*)fDYJets2011.Get("outTreePtOrd");
  TTree* tWJets2011  = (TTree*)fWJets2011.Get("outTreePtOrd");
  TTree* tTTJets2011 = (TTree*)fTTJets2011.Get("outTreePtOrd");


  int nBins = 13;
  TArrayF bins(nBins+1);
  bins[0] = 0;    bins[1] = 30;    bins[2] = 40;    bins[3] = 50;    bins[4]  = 60;    bins[5] = 70;
  bins[6] = 80;   bins[7] = 90;    bins[8] = 100;   bins[9] = 110;   bins[10] = 120;  bins[11] = 140; 
  bins[12] = 160; bins[13] = 200;
  if(binWidth_>0) 
    for(int k = 0 ; k <= nBins ; k++) bins[k] = binWidth_*k; 
  TString variable(variable_.c_str());
  TString labels(xTitle_.c_str());

  TH1F* hData    = new TH1F("hData"   ,labels, nBins, bins.GetArray());
  TH1F* hQCD     = new TH1F("hQCD"    ,labels, nBins, bins.GetArray());
  TH1F* hDYJets  = new TH1F("hDYJets" ,labels, nBins, bins.GetArray());
  TH1F* hWJets   = new TH1F("hWJets"  ,labels, nBins, bins.GetArray());
  TH1F* hWJetsSS = new TH1F("hWJetsSS",labels, nBins, bins.GetArray());
  TH1F* hTTJets  = new TH1F("hTTJets" ,labels, nBins, bins.GetArray());
  TH1F* hAll     = new TH1F("hAll"    ,labels, nBins, bins.GetArray());

  TH1F* hOld     = new TH1F("hOld"   ,labels, nBins, bins.GetArray());
  TH1F* hDataOld     = new TH1F("hDataOld"   ,labels, nBins, bins.GetArray());
  TH1F* hDataNew     = new TH1F("hDataNew"   ,labels, nBins, bins.GetArray());


  //TH1F* hVBFH130 = new TH1F("hVBFH130",labels, nBins, bins.GetArray());
  //TH1F* hGGFH130 = new TH1F("hGGFH130",labels, nBins, bins.GetArray());

  hData->SetMarkerStyle(kFullCircle);
  hQCD->SetFillColor(kMagenta-9);
  hDYJets->SetFillColor(kYellow-9);
  hWJets->SetFillColor(kRed-3);
  hTTJets->SetFillColor(kBlue-2);
  //hVBFH130->SetLineWidth(3);
  //hVBFH130->SetLineStyle(kDashed);
  //hVBFH130->SetLineColor(kBlue);
  //hGGFH130->SetLineWidth(3);
  //hGGFH130->SetLineStyle(kDashed);
  //hGGFH130->SetLineColor(kBlue);

  // full signal selection
  TCut sbinRel(      "ptL1>18 && ptL2>20 && tightestHPSDBWP>0 && diTauCharge==0 && muFlag==0 && HLTx && HLTmatch");
  // full signal selection
  TCut sbin(         "ptL1>18 && ptL2>20 && tightestHPSDBWP>0 && muFlag==0 && HLTx && HLTmatch && combRelIsoLeg1DBeta<0.10");
  // selection of opposite-sign W-enriched control region
  TCut sbinAntiW(    "ptL1>18 && ptL2>20 && tightestHPSDBWP>0 && muFlag==0 && HLTx && HLTmatch && combRelIsoLeg1DBeta<0.10");
  // selection of smae-sign enriched control-region 
  TCut sbinSSAntiW(  "ptL1>18 && ptL2>20 && tightestHPSDBWP>0 && diTauCharge!=0  && muFlag==0 && HLTx && HLTmatch && combRelIsoLeg1DBeta<0.10");
  // selection of same-sign control region
  TCut sbinSS(       "ptL1>18 && ptL2>20 && tightestHPSDBWP>0 && diTauCharge!=0  && muFlag==0 && HLTx && HLTmatch && combRelIsoLeg1DBeta<0.10");
  // selection of same sign loosely-isolated control region
  TCut sbinSSRelIso( "ptL1>18 && ptL2>20 && tightestHPSDBWP>0 && diTauCharge!=0  && muFlag==0 && HLTx && HLTmatch && combRelIsoLeg1DBeta<0.20");

  TCut highMt("MtLeg1>60");
  TCut lowMt("MtLeg1<40");
  TCut slowMt(MtCut_.c_str());

  TCut SSCharge("diTauCharge!=0");
  TCut OSCharge("diTauCharge==0");
  TCut sCharge(charge_.c_str());

  /////////// get approximated pu reweighting factors /////////////////
  TH1F* numPVData     = new TH1F("numPVData","",  20,0,40);
  TH1F* numPVDYJets   = new TH1F("numPVDYJets","",20,0,40);
  TH1F* numPVWJets    = new TH1F("numPVWJets","", 20,0,40);

  tData->Draw("numPV>>numPVData"    ,sbinRel);
  tDYJets->Draw("numPV>>numPVDYJets",sbinRel);
  tWJets->Draw("numPV>>numPVWJets"  ,sbinRel);

  numPVData->Scale(  1./numPVData->Integral());
  numPVDYJets->Scale(1./numPVDYJets->Integral());
  numPVWJets->Scale( 1./numPVWJets->Integral());

  float totalWeight = 0.;
  string scaleFactDYJets = "( ";
  for(int i = 1; i < numPVData->GetNbinsX(); i++){
    float binWidth = numPVDYJets->GetBinWidth(i);
    float weightBin_i = numPVDYJets->GetBinContent(i)>0 ?
      numPVData->GetBinContent(i)/numPVDYJets->GetBinContent(i) : 0.0;
 
    totalWeight += weightBin_i;

    scaleFactDYJets += string( Form("(numPV>=%f && numPV<%f)*%f",binWidth*(i-1), binWidth*i, weightBin_i) );
    if(i < (numPVData->GetNbinsX() - 1)) scaleFactDYJets += " + ";
  }
  scaleFactDYJets += " )";
  cout << scaleFactDYJets << endl;
  cout << endl;

  string scaleFactWJets = "( ";
  for(int i = 1; i < numPVData->GetNbinsX(); i++){
    float binWidth = numPVWJets->GetBinWidth(i);
    float weightBin_i = numPVWJets->GetBinContent(i)>0 ?
      numPVData->GetBinContent(i)/numPVWJets->GetBinContent(i) : 0.0;
 
    totalWeight += weightBin_i;

    scaleFactWJets += string( Form("(numPV>=%f && numPV<%f)*%f",binWidth*(i-1), binWidth*i, weightBin_i) );
    if(i < (numPVData->GetNbinsX() - 1)) scaleFactWJets += " + ";
  }
  scaleFactWJets += " )";
  cout << scaleFactWJets << endl;

  
  if(debug){
    cout << totalWeight/numPVData->GetNbinsX() << endl;

    TH1F* numPVData     = new TH1F("numPVData","",  20,0,120);
    TH1F* numPVWJets    = new TH1F("numPVWJets","", 20,0,120);

    numPVData->Reset();
    numPVWJets->Reset();
    tWJets->Draw("MtLeg1>>numPVData",sbin);
    tWJets->Draw("MtLeg1>>numPVWJets",TString(scaleFactWJets.c_str())*sbin);
    
    cout << numPVWJets->Integral() << " -- " << numPVData->Integral() << endl;
    numPVWJets->Draw();
    numPVData->Draw("SAME");

    //return;

  }


  // luminoisty of data is 57.1 pb
  float lumiFact             = 57.1/1000;
  float crossSecScalingW     = 36257./31314.;
  float crossSecScalingZ     = 3503.71/3048;
  float crossSecScalingTT    =  225.2/157.5;

  // opposite-sign to same-sign ratio for QCD
  float OStoSSRatio = 1.07;
  if( (string(sCharge.GetTitle())).find("diTauCharge!=0")!=string::npos ){
    OStoSSRatio = 1.0;
    cout << "SS selection" << endl;
  }

  /////////////////////////////////////////////////////////////////////
  // estimation of W+jets
  TH1F* h1 = new TH1F("h1","",1,-10,10);
  tWJets->Draw("etaL1>>h1",   TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbinAntiW&&OSCharge&&highMt));
  float WsbinMCAntiW  = h1->Integral()*lumiFact*crossSecScalingW;
  h1->Reset();
  tWJets->Draw("etaL1>>h1",   TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbin&&OSCharge&&lowMt));
  float WsbinMC       = h1->Integral()*lumiFact*crossSecScalingW;
  h1->Reset();
  tTTJets->Draw("etaL1>>h1",  TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbin&&OSCharge&&highMt));
  float TTsbinAntiW = h1->Integral()*lumiFact*crossSecScalingTT;
  h1->Reset();
  tDYJets->Draw("etaL1>>h1",  TString(("sampleWeight*"+scaleFactDYJets).c_str())*(sbin&&OSCharge&&highMt));
  float DYJetssbinAntiW = h1->Integral()*lumiFact;
  h1->Reset();
  tData->Draw("etaL1>>h1",   sbin&&OSCharge&&highMt);
  float DatasbinAntiW = h1->Integral();
  h1->Reset();

  float WscaleFactorOS =  1./WsbinMC;
  cout << "Wsbin (MC) = " << WsbinMC << endl;
  float Wsbin = (DatasbinAntiW - TTsbinAntiW - DYJetssbinAntiW)*(WsbinMC/WsbinMCAntiW);
  cout << "Wsbin = (" << DatasbinAntiW << " - " << TTsbinAntiW << " - " << DYJetssbinAntiW << " )*" << WsbinMC/WsbinMCAntiW << " = " << Wsbin << endl;
  WscaleFactorOS *= Wsbin;
  cout << " ==> scale factor (OS) = " << WscaleFactorOS << endl;

  /////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////
  // estimation of QCD
  h1->Reset();
  tWJets->Draw("etaL1>>h1",   TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbinSS&&highMt));
  float WsbinMCSSAntiW  = h1->Integral()*lumiFact*crossSecScalingW;
  h1->Reset();
  tWJets->Draw("etaL1>>h1",   TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbinSS&&lowMt));
  float WsbinMCSS       = h1->Integral()*lumiFact*crossSecScalingW;
  h1->Reset();
  tTTJets->Draw("etaL1>>h1",  TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbinSS&&highMt));
  float TTsbinSSAntiW = h1->Integral()*lumiFact*crossSecScalingTT;
  h1->Reset();
  tDYJets->Draw("etaL1>>h1",  TString(("sampleWeight*"+scaleFactDYJets).c_str())*(sbinSS&&highMt));
  float DYJetsSSAntiW = h1->Integral()*lumiFact;
  h1->Reset();
  tData->Draw("etaL1>>h1",   sbinSS&&highMt);
  float DatasbinSSAntiW = h1->Integral();
  h1->Reset();
  tData->Draw("etaL1>>h1",   sbinSS&&lowMt);
  float DatasbinSS = h1->Integral();
  h1->Reset();
  
  float WscaleFactorSS =  1./WsbinMCSS;
  cout << "WsbinSS (MC) = " << WsbinMCSS << endl; 
  float WsbinSS   = (DatasbinSSAntiW - TTsbinSSAntiW - DYJetsSSAntiW)*(WsbinMCSS/WsbinMCSSAntiW);
  cout << "WsbinSS = (" << DatasbinSSAntiW << " - " << TTsbinSSAntiW << " - " << DYJetsSSAntiW << " )*" << WsbinMCSS/WsbinMCSSAntiW << " = " << WsbinSS << endl;
  WscaleFactorSS *= WsbinSS;
  cout << " ==> scale factor (SS) = " << WscaleFactorSS << endl;
  float QCDsbinSS = DatasbinSS - WsbinSS;
  float QCDsbin   = QCDsbinSS*OStoSSRatio;
  /////////////////////////////////////////////////////////////////////

  hDYJets->Sumw2();
  hWJets->Sumw2();
  hTTJets->Sumw2();
  hQCD->Sumw2();
  hAll->Sumw2();


  // Draw with cuts and weights !!!
  tData->Draw(  variable+">>hData",   "(index==0)"*(sbin&&sCharge&&slowMt));
  tData->Draw(  variable+">>hQCD",    "(index==0)"*(sbinSSRelIso&&slowMt));
  tDYJets->Draw(variable+">>hDYJets", TString(("sampleWeight*(index==0)*"+scaleFactDYJets).c_str())*(sbin&&sCharge&&slowMt));
  tWJets->Draw( variable+">>hWJets",  TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbin&&sCharge&&slowMt));
  tWJets->Draw( variable+">>hWJetsSS",TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbinSS&&slowMt));
  tTTJets->Draw(variable+">>hTTJets", TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbin&&sCharge&&slowMt));
  //tVBFH130->Draw(variable+">>hVBFH130", "puWeight*HLTweightMu*HLTweightTau*SFMu*SFTau*sampleWeight"*sbin);
  //tGGFH130->Draw(variable+">>hGGFH130", "puWeight*HLTweightMu*HLTweightTau*SFMu*SFTau*sampleWeight*HqTWeight"*sbin);
  
  // Scale histograms
  hDYJets->Scale( lumiFact );
  hWJets->Scale(  lumiFact*crossSecScalingW*( int((string(sCharge.GetTitle())).find("diTauCharge==0")!=string::npos)*WscaleFactorOS +
					      int((string(sCharge.GetTitle())).find("diTauCharge!=0")!=string::npos)*WscaleFactorSS) );
  hWJetsSS->Scale(lumiFact*crossSecScalingW*WscaleFactorSS );
  hTTJets->Scale( lumiFact*crossSecScalingTT );
  hQCD->Add(   hWJetsSS, -1);
  hQCD->Scale( QCDsbin/hQCD->Integral());
  //hVBFH130->Scale( lumiFact*100 );
  //hGGFH130->Scale( lumiFact*100 );
  //hVBFH130->Add(hGGFH130,1.0);


  //////////////////////////////////////////////////////////

  cout << "Old ==>" << endl;

  tData2011->Draw(variable+">>hOld", (sbin&&sCharge&&slowMt));
  float data2011 = hOld->Integral();
  h1->Reset();
  tDYJets2011->Draw(variable+">>hOld",  "puWeight*sampleWeight*HLTweightTau*HLTweightMu*SFMu"*(sbin&&sCharge&&slowMt));
  float DYJets2011 = hOld->Integral();
  h1->Reset();
  tWJets2011->Draw(variable+">>hOld",   "puWeight*sampleWeight*HLTweightTau*HLTweightMu*SFMu"*(sbin&&sCharge&&slowMt));
  float WJets2011 = hOld->Integral();
  h1->Reset();
  tTTJets2011->Draw(  variable+">>hOld","puWeight*sampleWeight*HLTweightTau*HLTweightMu*SFMu"*(sbin&&sCharge&&slowMt));
  float TTJets2011 = hOld->Integral();
  h1->Reset();

  cout << "DYJets scaled to 57 pb = " << DYJets2011*0.057 << " ==> @ 8 TeV = " << DYJets2011*0.057*crossSecScalingZ
       << endl;
  cout << "    ==> ratio 2012/2011 = " << hDYJets->Integral()/(DYJets2011*0.057*crossSecScalingZ) << endl;

  cout << "WJets scaled to 57 pb = " << WJets2011*0.057 << " ==> @ 8 TeV = " << WJets2011*0.057*crossSecScalingW
       << endl;
  cout << "    ==> ratio 2012/2011 = " << hWJets->Integral()/(WJets2011*0.057*crossSecScalingW) << endl;

  cout << "TTJets scaled to 57 pb = " << TTJets2011*0.057 << " ==> @ 8 TeV = " << TTJets2011*0.057*crossSecScalingTT
       << endl;
  cout << "    ==> ratio 2012/2011 = " << hTTJets->Integral()/(TTJets2011*0.057*crossSecScalingW) << endl;

  cout << "QCD scaled to 57 pb = " << (data2011-(DYJets2011+WJets2011+TTJets2011)*4.6)*0.057/4.6 << " ==> @ 8 TeV = ??" 
       << endl;
  cout << "    ==> ratio 2012/2011 = " << (hData->Integral()-hDYJets->Integral()-hWJets->Integral()-hTTJets->Integral())/(data2011-(DYJets2011+WJets2011+TTJets2011)*4.6)/0.057*4.6 << endl;

  //////////////////////////////////////////////////////

  hAll->Add(hDYJets);
  hAll->Add(hWJets);
  hAll->Add(hTTJets);
  hAll->Add(hQCD);
  hAll->SetLineWidth(1);
  hAll->SetLineColor(kRed);
  hAll->SetFillColor(kRed);
  hAll->SetFillStyle(3003);

  for(int i = 1; i < hAll->GetNbinsX(); i++){
    float err = 0.;
    err += TMath::Power(hDYJets->GetBinError(i),2);
    err += TMath::Power(hWJets->GetBinError(i),2);
    err += TMath::Power(hTTJets->GetBinError(i),2);
    err += TMath::Power(hQCD->GetBinError(i),2);
    hAll->SetBinError(i,TMath::Sqrt(err));
  }


  // Add all together
  THStack* aStack = new THStack("aStack","");
  aStack->Add(hTTJets);
  aStack->Add(hQCD);
  aStack->Add(hWJets);
  aStack->Add(hDYJets);
  //aStack->Add(hVBFH130);

  hData->SetAxisRange( 0, TMath::Max(hData->GetMaximum(), hAll->GetMaximum())*1.20, "Y");

  hData->Sumw2();
  hData->Draw("P");
  aStack->Draw("HISTSAME");
  hAll->Draw("E2SAME");
  hData->Draw("PSAME");

  cout << "DYJets = " << hDYJets->Integral() << endl;
  cout << "TT = " << hTTJets->Integral() << endl;
  cout << "QCD = " << hQCD->Integral() << endl;
  cout << "WJets = " << hWJets->Integral() << endl;
  cout << "Data = " << hData->Integral() << endl;


  // Legend
  TLegend* leg = new TLegend(0.60,0.62,0.78,0.87,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.03);

  leg->SetHeader("#splitline{CMS Preliminary 2012}{#sqrt{s}=8 TeV, L=57.2 pb^{-1}, #tau_{#mu}#tau_{h}}");
  leg->AddEntry(hData,"Observed","P");
  leg->AddEntry(hDYJets,"Z#rightarrow ll","F");
  leg->AddEntry(hWJets,"W+jets","F");
  leg->AddEntry(hQCD,"QCD","F");
  leg->AddEntry(hTTJets,"t#bar{t}","F");
  //leg->AddEntry(hVBFH130,"100 X H#rightarrow#tau#tau, m_{H}=130 GeV","F");
  leg->Draw();

  if(charge_.find("diTauCharge==0")!=string::npos)
    c1->SaveAs(("plots/plotMuTauDataMC_"+outFile_+".png").c_str());
  else
    c1->SaveAs(("plots/plotMuTauDataMC_"+outFile_+"_SS.png").c_str());
  if(charge_.find("diTauCharge==0")!=string::npos)
    c1->SaveAs(("plots/plotMuTauDataMC_"+outFile_+".pdf").c_str());
  else
    c1->SaveAs(("plots/plotMuTauDataMC_"+outFile_+"_SS.pdf").c_str());



  //////////////////////


  TCanvas *c2 = new TCanvas("c2","",5,30,650,600);
  c2->SetGrid(0,0);
  c2->SetFillStyle(4000);
  c2->SetFillColor(10);
  c2->SetTicky();
  c2->SetObjectStat(0);

  // Legend
  TLegend* leg2 = new TLegend(0.50,0.70,0.70,0.85,NULL,"brNDC");
  leg2->SetFillStyle(0);
  leg2->SetBorderSize(0);
  leg2->SetFillColor(10);
  leg2->SetTextSize(0.03);

  hDataOld->Sumw2();
  hDataNew->Sumw2();
  tData2011->Draw(variable+">>hDataOld", (sbin&&sCharge&&slowMt));
  tData->Draw(variable+">>hDataNew", (sbin&&sCharge&&slowMt));
  hDataOld->SetLineColor(kBlack);
  hDataOld->SetLineWidth(3);
  hDataNew->SetLineColor(kRed);
  hDataNew->SetLineWidth(3);

  hDataNew->Scale(1./hDataNew->Integral());
  hDataOld->Scale(1./hDataOld->Integral());

  hDataNew->SetAxisRange( 0, TMath::Max(hDataNew->GetMaximum(), hDataOld->GetMaximum())*1.20, "Y");

  hDataNew->Draw("EHIST");
  hDataOld->Draw("HISTSAME");

  leg2->SetHeader("CMS Preliminary 2012  #tau_{#mu}#tau_{h}");
  leg2->AddEntry(hDataOld,"Run2011 L=4.6 fb^{-1} #sqrt{s}=7 TeV");
  leg2->AddEntry(hDataNew,"Run2012 L=0.056 fb^{-1} #sqrt{s}=8 TeV");

  leg2->Draw();

  if(charge_.find("diTauCharge==0")!=string::npos)
    c2->SaveAs(("plots/plotMuTauDataData_"+outFile_+".png").c_str());
  else
    c2->SaveAs(("plots/plotMuTauDataData_"+outFile_+"_SS.png").c_str());
  if(charge_.find("diTauCharge==0")!=string::npos)
    c2->SaveAs(("plots/plotMuTauDataData_"+outFile_+".pdf").c_str());
  else
    c2->SaveAs(("plots/plotMuTauDataData_"+outFile_+"_SS.pdf").c_str());

  return;


}


void plotAll(){

  //////// SS

  plot("diTauVisMass",   " ; mass (GeV) ; Events", -1,      "MtLeg1<40", "diTauCharge!=0", "visibleMass");
  plot("diTauSVFitMass", " ; full mass (GeV) ; Events", 18, "MtLeg1<40", "diTauCharge!=0",  "fullMass");
  plot("ptL1", " ; #mu p_{T} (GeV) ; Events", 6 ,           "MtLeg1<40", "diTauCharge!=0",  "muonPt");
  plot("ptL2", " ; #tau p_{T} (GeV) ; Events", 5 ,          "MtLeg1<40", "diTauCharge!=0",  "tauPt");
  plot("abs(etaL1)", " ; #mu #eta ; Events", 0.2 ,          "MtLeg1<40", "diTauCharge!=0",  "muonEta");
  plot("abs(etaL2)", " ; #tau #eta ; Events",0.2 ,           "MtLeg1<40", "diTauCharge!=0",  "tauEta");
  plot("MtLeg1",     " ; M_{T}(#mu,MET) (GeV) ; Events", 10, "MtLeg1<999", "diTauCharge!=0",  "Mt");
  plot("MEt",       " ; MET (GeV) ; Events", 8, "MtLeg1<999", "diTauCharge!=0", "MEt");
  plot("numPV",     " ; vertices ; Events", 3,  "MtLeg1<40",  "diTauCharge!=0",  "PV");

  plot("nJets30",   " ; jet multiplicity ; Events", 1,            "MtLeg1<40", "diTauCharge!=0",  "jetMult");
  plot("pt1",   " ; lead jet p_{T} (GeV) ; Events", 10,           "MtLeg1<40", "diTauCharge!=0",  "jet1Pt");
  plot("pt2",   " ; sublead jet p_{T} (GeV) ; Events", 10,        "MtLeg1<40", "diTauCharge!=0",  "jet2Pt");
  plot("abs(eta1)",   " ; lead jet #eta ; Events", 0.2,           "MtLeg1<40", "diTauCharge!=0",  "jet1Eta");
  plot("abs(eta2)",   " ; sublead jet #eta ; Events", 0.2,        "MtLeg1<40", "diTauCharge!=0",  "jet2Eta");

  //////// OS

  plot("diTauVisMass",   " ; mass (GeV) ; Events", -1,      "MtLeg1<40", "diTauCharge==0", "visibleMass");
  plot("diTauSVFitMass", " ; full mass (GeV) ; Events", 18, "MtLeg1<40", "diTauCharge==0",  "fullMass");
  plot("ptL1", " ; #mu p_{T} (GeV) ; Events", 6 ,           "MtLeg1<40", "diTauCharge==0",  "muonPt");
  plot("ptL2", " ; #tau p_{T} (GeV) ; Events", 5 ,          "MtLeg1<40", "diTauCharge==0",  "tauPt");
  plot("abs(etaL1)", " ; #mu #eta ; Events", 0.2 ,          "MtLeg1<40", "diTauCharge==0",  "muonEta");
  plot("abs(etaL2)", " ; #tau #eta ; Events",0.2 ,           "MtLeg1<40", "diTauCharge==0",  "tauEta");
  plot("MtLeg1",     " ; M_{T}(#mu,MET) (GeV) ; Events", 10, "MtLeg1<999", "diTauCharge==0",  "Mt");
  plot("MEt",       " ; MET (GeV) ; Events", 8, "MtLeg1<999", "diTauCharge==0", "MEt");
  plot("numPV",     " ; vertices ; Events", 3, "MtLeg1<40",   "diTauCharge==0",  "PV");

  plot("nJets30",   " ; jet multiplicity ; Events", 1,            "MtLeg1<40", "diTauCharge==0",  "jetMult");
  plot("pt1",   " ; lead jet p_{T} (GeV) ; Events", 10,           "MtLeg1<40", "diTauCharge==0",  "jet1Pt");
  plot("pt2",   " ; sublead jet p_{T} (GeV) ; Events", 10,        "MtLeg1<40", "diTauCharge==0",  "jet2Pt");
  plot("abs(eta1)",   " ; lead jet #eta ; Events", 0.2,           "MtLeg1<40", "diTauCharge==0",  "jet1Eta");
  plot("abs(eta2)",   " ; sublead jet #eta ; Events", 0.2,        "MtLeg1<40", "diTauCharge==0",  "jet2Eta");

}
