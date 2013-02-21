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
#include "TFitResultPtr.h"


void fakeRateElec( string name_ = "ElecTau_Elec_ptL1",
		   float nJets30 = 0){

  TCanvas *c1 = new TCanvas(("c1"+name_).c_str(),"",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);


  float Lumi   = (-47.4 + 215.6 + 955.3 + 389.9 + 706.7 + 2714);
  float lumiCorrFactor = (1-0.056);
  Lumi *= lumiCorrFactor;

  TFile* out = new TFile("FakeRate.root","UPDATE");



  TFile *fData              
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval/nTupleRun2011-ElecTau-All_run_Open_ElecTauStream.root", "READ");  

  TFile *fBackgroundWJets   
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval/nTupleWJets-ElecTau-madgraph-PUS6_run_Open_ElecTauStream.root","READ"); 
  TFile *fBackgroundDY
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval/nTupleDYJets-ElecTau-50-madgraph-PUS6_run_Open_ElecTauStream.root","READ"); 
  TFile *fBackgroundTTbar  
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval/nTupleTTJets-ElecTau-madgraph-PUS6_run_Open_ElecTauStream.root","READ"); 
  TFile *fBackgroundOthers  
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval/nTupleOthers-ElecTau-PUS6_run_Open_ElecTauStream.root","READ"); 
  
  TString tree         = "outTreePtOrdRaw";

  TTree *data          = (TTree*)fData->Get(tree);

  TTree *backgroundTTbar     = (TTree*)fBackgroundTTbar->Get(tree);
  TTree *backgroundWJets     = (TTree*)fBackgroundWJets->Get(tree);
  TTree *backgroundOthers    = (TTree*)fBackgroundOthers->Get(tree);
 
  TFile* dummy1 = new TFile("dummy1.root","RECREATE");
  cout << "Now copying g/Z -> tau+ tau- " << endl;
  TTree *backgroundDYTauTau  = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)==(23*15)");                 // g/Z -> tau+ tau-
  cout << "Now copying g/Z -> e+e- e->tau" << endl;
  TTree *backgroundDYEtoTau = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)!=(23*15) &&  leptFakeTau"); // g/Z -> mu+mu- mu->tau
  cout << "Now copying g/Z -> e+e- jet->tau" << endl;
  TTree *backgroundDYJtoTau  = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)!=(23*15) && !leptFakeTau"); // g/Z -> mu+mu- jet->tau


  bool useMt      = true;
  string antiWcut = useMt ? "MtLeg1Corr" : "-(pZetaCorr-1.5*pZetaVisCorr)" ;
  float antiWsgn  = useMt ? 40. :  20. ; 
  float antiWsdb  = useMt ? 60. :  40. ; 

  TCut lpt(Form("ptL1>20 && TMath::Abs(etaL1)<2.1 && nJets30>=%f",nJets30));
  TCut tpt("ptL2>20 && ptL2<999 && TMath::Abs(etaL2)<2.3");

  TCut lID("((TMath::Abs(etaL1)<0.925 && mvaPOGNonTrig>0.85) || (TMath::Abs(etaL1)<1.479 && TMath::Abs(etaL1)>0.80 && mvaPOGNonTrig>0.975) || (TMath::Abs(etaL1)>1.479 && mvaPOGNonTrig>0.985))");
  tpt = tpt&&lID;

  TCut tiso("tightestHPSMVAWP>=0 && tightestAntiECutWP>1"); 
  TCut liso("combRelIsoLeg1DBetav2<0.10");
  TCut laiso("combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50");
  TCut lliso("combRelIsoLeg1DBetav2<0.20");
  TCut lveto("elecFlag==0");
  TCut SS("diTauCharge!=0");
  TCut OS("diTauCharge==0");
  TCut hltevent("pairIndex<1 && HLTx==1 && (run>=163269 || run==1)");
  TCut hltmatch("HLTmatch==1");

  TCut pZ( Form("((%s)<%f)",antiWcut.c_str(),antiWsgn));
  TCut apZ(Form("((%s)>%f)",antiWcut.c_str(),antiWsdb));

 
  vector<int> bins;
  bins.push_back(20);
  bins.push_back(22);
  bins.push_back(24);
  bins.push_back(26);
  bins.push_back(28);
  bins.push_back(30);
  bins.push_back(32);
  bins.push_back(34);
  bins.push_back(36);
  bins.push_back(40);
  bins.push_back(45);
  bins.push_back(50);
  bins.push_back(60); 
  bins.push_back(80); 
  bins.push_back(100); 
  bins.push_back(1000); 

  int nBins =  bins.size() ;
  TArrayF binsT(nBins);

  for(unsigned int k = 0 ; k < nBins ; k++ )
    binsT[k] = bins[k];

  TH1F* hFakeRate  = new TH1F(("hFakeRate"+name_).c_str(), "; electron p_{T} ; fake-rate", nBins-1, binsT.GetArray() );

  TH1F* hFakeRateErrUp    = new TH1F(("hFakeRateErrUp"+name_).c_str(),   "", nBins-1, binsT.GetArray() );
  TH1F* hFakeRateErrDown  = new TH1F(("hFakeRateErrDown"+name_).c_str(), "", nBins-1, binsT.GetArray() );

  TH1F* hPuritySdb = new TH1F("hPuritySdb","", nBins-1, binsT.GetArray() );
  TH1F* hPurityQCD = new TH1F("hPurityQCD","", nBins-1, binsT.GetArray() );
  TH1F* hPurityAIs = new TH1F("hPurityAIs","", nBins-1, binsT.GetArray() );

  //TH1F* hFakeRate  = new TH1F("hFakeRate", "", nBins-1, 20,100 );
  //TH1F* hPuritySdb = new TH1F("hPuritySdb","", nBins-1, 20,100 );
  //TH1F* hPurityQCD = new TH1F("hPurityQCD","", nBins-1, 20,100 );

  TH1F* hExtrap        = new TH1F("hExtrap","",1, -10,10);
  TH1F* hSVfit         = new TH1F(("hSVfit"+name_).c_str(),     "",40, 0,400);
  TH1F* hSVfitFake     = new TH1F(("hSVfitFake"+name_).c_str(), "",40, 0,400);
  TH1F* hSVfitFakeErrUp   = new TH1F(("hSVfitFakeErrUp"+name_).c_str(), "",40, 0,400);
  TH1F* hSVfitFakeErrDown = new TH1F(("hSVfitFakeErrDown"+name_).c_str(), "",40, 0,400);

  TH1F* hSVfitRatio    = new TH1F("hSVfitRatio", "",40, 0,400);
  
  TH1F* hSVfitAIso     = new TH1F(("hSVfitAIso"+name_).c_str(), "",40, 0,400);
  TH1F* hSVfitHelpAdd  = new TH1F("hSVfitHelpAdd", "",40, 0,400);
  TH1F* hSVfitHelp     = new TH1F("hSVfitHelp", "",40, 0,400);

  TCut sbinSSaIsoInclusiveAllPt = lpt && tpt && tiso && laiso && lveto && SS && pZ  && hltevent && hltmatch;

  for(int i = 0; i <  bins.size()-1 ; i++){

    float SSIsoToSSAIsoRatioQCD = 1.0;

    float min = bins[i];
    float max = bins[i+1]  ;

    TCut lpt_i = lpt && TCut(Form("ptL1>%f && ptL1<=%f", min, max));

    TCut sbinInclusive;
    sbinInclusive            = tiso && lpt_i && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch;
    TCut sbinPZetaRelSSInclusive;
    sbinPZetaRelSSInclusive  = tiso && lpt_i && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch;
    TCut sbinSSInclusive;
    sbinSSInclusive          = tiso && lpt_i && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch;
    TCut sbinSSaIsoInclusive;
    sbinSSaIsoInclusive      = tiso && lpt_i && tpt && tiso && laiso&& lveto && SS && pZ  && hltevent && hltmatch;


    cout << "******** Extrapolation factors for QCD normalization: " << " bin " << min << "," << max << " " << "********" << endl;

    hExtrap->Reset();
    backgroundWJets->Draw("etaL1>>hExtrap","(sampleWeight*puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRelSSInclusive&&pZ));
    float ExtrapSSWinSignalRegionMC   = hExtrap->Integral();
    hExtrap->Reset();
    backgroundWJets->Draw("etaL1>>hExtrap","(sampleWeight*puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRelSSInclusive&&apZ));
    float ExtrapSSWinSidebandRegionMC = hExtrap->Integral();
    float ExtrapscaleFactorSS         = ExtrapSSWinSignalRegionMC>0 ? ExtrapSSWinSidebandRegionMC/ExtrapSSWinSignalRegionMC : 1.0;
    cout << " Extrapolation factor W SS (inclusive) " << ExtrapscaleFactorSS << endl;
    
    hExtrap->Reset();
    backgroundTTbar->Draw("etaL1>>hExtrap","(sampleWeight*puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRelSSInclusive&&apZ));
    float ExtrapttbarExtrSS    = hExtrap->Integral()*Lumi/1000;
    hExtrap->Reset();
    backgroundOthers->Draw("etaL1>>hExtrap","(sampleWeight*puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRelSSInclusive&&apZ));
    float ExtrapothersExtrSS   = hExtrap->Integral()*Lumi/1000;
    hExtrap->Reset();
    backgroundDYJtoTau->Draw("etaL1>>hExtrap","(sampleWeight*puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRelSSInclusive&&apZ));
    float ExtrapdyjtotauExtrSS = hExtrap->Integral()*Lumi/1000;


    hExtrap->Reset();
    data->Draw("etaL1>>hExtrap", sbinPZetaRelSSInclusive&&apZ);
    float ExtrapSSWinSignalRegionDATA = hExtrap->Integral();
    cout << "Extrapolation for QCD (inclusive): total data events in sideband " << ExtrapSSWinSignalRegionDATA << endl;

    float totalBkgSdb = 
      ExtrapttbarExtrSS +
      ExtrapothersExtrSS +
      ExtrapdyjtotauExtrSS;

    hPuritySdb->SetBinContent(i+1, totalBkgSdb/ExtrapSSWinSignalRegionDATA);


    ExtrapSSWinSignalRegionDATA -= ExtrapttbarExtrSS;
    ExtrapSSWinSignalRegionDATA -= ExtrapothersExtrSS;
    ExtrapSSWinSignalRegionDATA -= ExtrapdyjtotauExtrSS;
    ExtrapSSWinSignalRegionDATA /= ExtrapscaleFactorSS;
    cout << "Extrapolation for QCD (inclusive): W+jets in SS signal region (inclusive) is estimated to be " << ExtrapSSWinSignalRegionDATA << endl;
    

    hSVfitHelpAdd->Reset(); 

    float totalBkg =  0.;
    hExtrap->Reset();
    data->Draw("etaL1>>hExtrap", sbinSSInclusive);
    data->Draw("diTauNSVfitMass>>hSVfitHelpAdd", sbinSSInclusive);
    hSVfit->Add(hSVfitHelpAdd,+1);

    float SSeventsExtrapTot = hExtrap->Integral();
    float SSeventsExtrap    = hExtrap->Integral();

    cout << "Extrapolation for SS events in data (inclusive) " << hExtrap->GetEntries() << endl;
    cout << "Subtracting W+jets (SS)..." << endl;
    SSeventsExtrap  -= ExtrapSSWinSignalRegionDATA;
    totalBkg        += ExtrapSSWinSignalRegionDATA;

    hSVfitHelp->Reset();

    backgroundWJets->Draw("diTauNSVfitMass>>hSVfitHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinSSInclusive);
    if(hSVfitHelp->Integral()>0) hSVfitHelp->Scale(ExtrapSSWinSignalRegionDATA/hSVfitHelp->Integral());
    hSVfit->Add(hSVfitHelp,-1);
    hSVfitHelp->Reset();

    hExtrap->Reset();
    backgroundTTbar->Draw("etaL1>>hExtrap", "(sampleWeight*puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinSSInclusive);
    backgroundTTbar->Draw("diTauNSVfitMass>>hSVfitHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinSSInclusive);
    SSeventsExtrap  -= hExtrap->Integral()*Lumi/1000;
    totalBkg        += hExtrap->Integral()*Lumi/1000;
    hSVfitHelp->Scale(Lumi/1000);
    hSVfit->Add(hSVfitHelp,-1);
    hSVfitHelp->Reset();

    hExtrap->Reset();
    backgroundDYEtoTau->Draw("etaL1>>hExtrap", "(sampleWeight*puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinSSInclusive);
    backgroundDYEtoTau->Draw("diTauNSVfitMass>>hSVfitHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinSSInclusive);
    SSeventsExtrap  -= hExtrap->Integral()*Lumi/1000;
    totalBkg        += hExtrap->Integral()*Lumi/1000;
    hSVfitHelp->Scale(Lumi/1000);
    hSVfit->Add(hSVfitHelp,-1);
    hSVfitHelp->Reset();

    hExtrap->Reset();
    backgroundDYJtoTau->Draw("etaL1>>hExtrap", "(sampleWeight*puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinSSInclusive);
    backgroundDYJtoTau->Draw("diTauNSVfitMass>>hSVfitHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinSSInclusive);
    SSeventsExtrap  -= hExtrap->Integral()*Lumi/1000;
    totalBkg        += hExtrap->Integral()*Lumi/1000;
    hExtrap->Reset();
    hSVfitHelp->Scale(Lumi/1000);
    hSVfit->Add(hSVfitHelp,-1);
    hSVfitHelp->Reset();

    data->Draw("etaL1>>hExtrap", sbinSSaIsoInclusive);
    float SSeventsExtrapAiso = hExtrap->GetEntries();

    backgroundWJets->Draw("etaL1>>hExtrap", "(sampleWeight*puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinSSaIsoInclusive);
    hExtrap->Scale(Lumi/1000);
    hPurityAIs->SetBinContent(i+1, hExtrap->Integral()/SSeventsExtrapAiso );
    hExtrap->Reset();
    SSeventsExtrapAiso -= hExtrap->Integral();

    SSIsoToSSAIsoRatioQCD = SSeventsExtrap>0 ? 1./(SSeventsExtrap/SSeventsExtrapAiso) : 1.;
    cout << "The extrapolation factor Iso<0.1 / 0.3<Iso<0.5 is " << SSIsoToSSAIsoRatioQCD << endl;
    

    hPurityQCD->SetBinContent(i+1,totalBkg/(SSeventsExtrap+totalBkg));


    float error_iDen = 0.;
    if(SSeventsExtrapAiso>0) error_iDen += 1./TMath::Sqrt(SSeventsExtrapAiso);

    float error_iNum2 = 0.;
    error_iNum2 += SSeventsExtrap;
    error_iNum2 += (totalBkg*0.06)*(totalBkg*0.06);
    float error_iNum = SSeventsExtrap>0 ? TMath::Sqrt(error_iNum2)/SSeventsExtrap : 0.0;

    hFakeRate->SetBinContent(i+1, SSIsoToSSAIsoRatioQCD);
    hFakeRate->SetBinError(i+1,   (error_iDen+error_iNum)*SSIsoToSSAIsoRatioQCD );

    cout <<  hFakeRate->GetBinContent(i+1) << endl;
    cout <<  hFakeRate->GetBinError(i+1) << endl;
    cout << "************** END extrapolation *******************" << endl;

  }


  //return;

  TF1* fit = new TF1(("fit_"+name_).c_str(),"[0]*TMath::Exp([1]*x)+[2]",20, bins[bins.size()-1]);
  fit->SetLineColor(kRed);
  fit->SetParLimits(0,0,20);
  fit->SetParLimits(1,-5,0);
  fit->SetParLimits(2,0,1);

  //TF1* fit = new TF1("fit","[0]+[1]*x+[2]*x*x + [3]*x*x*x",20, bins[bins.size()-2]);
  //fit->SetLineColor(kRed);
  //fit->SetParLimits(0,0,10);
  //fit->SetParLimits(1,-5,5);
  //fit->SetParLimits(2,0,1);
  //fit->SetParLimits(3,-1,1);


  //TF1 *fit = new TF1("fit","[0]+[1]*TMath::Erf([2]*x+[3])",20, bins[bins.size()-1]);
  //fit->SetParLimits(0,-200,200);
  //fit->SetParLimits(1,0,500);
  //fit->SetParLimits(2,0,0.5);
  //fit->SetParLimits(3,-100,1);


  hFakeRate->Fit(("fit"+name_).c_str(), "", "", 20, 1000);

  float par0 = fit->GetParameter(0);
  float par1 = fit->GetParameter(1);
  float par2 = fit->GetParameter(2);

  float parE0 = fit->GetParError(0);
  float parE1 = fit->GetParError(1);
  float parE2 = fit->GetParError(2);



  for(int i = 0; i <  bins.size()-1 ; i++){

    float min = bins[i];
    float max = bins[i+1]  ;

    double integE    = fit->IntegralError(min,max );
    double integ     = fit->Integral(min,max);
    double binWidth  = max-min;
    cout << "bin " << min << "," << max << " ==> " << integ/binWidth << endl;
    hFakeRateErrUp->SetBinContent( i+1,   (integ + integE)/binWidth );
    hFakeRateErrDown->SetBinContent( i+1, (integ - integE)/binWidth );
  }
  hFakeRateErrDown->SetLineColor(kRed);
  hFakeRateErrDown->SetLineStyle(kDashed);
  hFakeRateErrUp->SetLineColor(kRed);
  hFakeRateErrUp->SetLineStyle(kDotted);

  cout << "error @ 30 = " << TMath::Sqrt(parE0*parE0 + parE1*parE1 + parE2*parE2) << endl;

  TF1* fitE1 = new TF1("fitE1",Form("(%f)*TMath::Exp((%f)*x)+(%f)", par0+parE0, par1, par2),20, bins[bins.size()-1]);
  TF1* fitE2 = new TF1("fitE2",Form("(%f)*TMath::Exp((%f)*x)+(%f)", par0, par1+parE1, par2),20, bins[bins.size()-1]);
  TF1* fitE3 = new TF1("fitE3",Form("(%f)*TMath::Exp((%f)*x)+(%f)", par0, par1, par2+parE2),20, bins[bins.size()-1]);
  TF1* fitE4 = new TF1("fitE4",Form("(%f)*TMath::Exp((%f)*x)+(%f)", par0-parE0, par1, par2),20, bins[bins.size()-1]);
  TF1* fitE5 = new TF1("fitE5",Form("(%f)*TMath::Exp((%f)*x)+(%f)", par0, par1-parE1, par2),20, bins[bins.size()-1]);
  TF1* fitE6 = new TF1("fitE6",Form("(%f)*TMath::Exp((%f)*x)+(%f)", par0, par1, par2-parE2),20, bins[bins.size()-1]);

  fitE1->SetLineColor(kBlue);
  fitE2->SetLineColor(kGreen);
  fitE3->SetLineColor(kMagenta);
  fitE4->SetLineColor(kYellow);
  fitE5->SetLineColor(kBlack);
  fitE6->SetLineColor(kYellow-6);

  string scaleFact = "( ";
  string scaleFactErrUp   = "( ";
  string scaleFactErrDown = "( ";

  for(int i = 0; i < bins.size()-1; i++){
    
    float min = bins[i];
    float max = bins[i+1];
    cout << min << "," << max << endl;
    float bin = hFakeRate->FindBin((max+min)/2.);
    //float bin = i+1;
    cout << bin << endl;
    float weightBin_i        =  fit->Eval( (max+min)/2.);
    float weightBin_iErrUp   =  hFakeRateErrUp->GetBinContent( bin );
    float weightBin_iErrDown =  hFakeRateErrDown->GetBinContent( bin );
    
    scaleFact        += string( Form("(ptL1>=%f && ptL1<%f)*%f", min , max, 1./weightBin_i ) );
    scaleFactErrUp   += string( Form("(ptL1>=%f && ptL1<%f)*%f", min , max, 1./weightBin_iErrUp   ) );
    scaleFactErrDown += string( Form("(ptL1>=%f && ptL1<%f)*%f", min , max, 1./weightBin_iErrDown ) );

    if(i < bins.size() - 2 ){
      scaleFact += " + ";
      scaleFactErrUp   += " + ";
      scaleFactErrDown += " + ";
    }
  }
  scaleFact += " )";
  scaleFactErrUp   += " )";
  scaleFactErrDown += " )";

  cout << scaleFact << endl;
  
  c1->cd();


  data->Draw(("diTauNSVfitMass>>hSVfitAIso"+name_).c_str(), sbinSSaIsoInclusiveAllPt);

  data->Draw(("diTauNSVfitMass>>hSVfitFake"+name_).c_str(), (TCut(scaleFact.c_str()))*sbinSSaIsoInclusiveAllPt);
  data->Draw(("diTauNSVfitMass>>hSVfitFakeErrUp"+name_).c_str(), (TCut(scaleFactErrUp.c_str()))*sbinSSaIsoInclusiveAllPt);
  data->Draw(("diTauNSVfitMass>>hSVfitFakeErrDown"+name_).c_str(), (TCut(scaleFactErrDown.c_str()))*sbinSSaIsoInclusiveAllPt);


  hSVfitFake->SetMarkerStyle(kOpenCircle);
  hSVfitFake->SetMarkerSize(1.2);
  hSVfitFake->SetMarkerColor(kRed);

  hSVfitAIso->SetLineColor(kMagenta);

  hSVfit->Print("all");
  hSVfitFake->Print("all");


  hSVfit->Draw();
  hSVfitFake->Draw("PSAME");
  hSVfitAIso->Draw("SAME");
  //return;

  out->cd();
  c1->Draw();
  c1->Write();


  TCanvas *c2 = new TCanvas(("c2"+name_).c_str(),"",5,30,650,600);
  c2->SetGrid(0,0);
  c2->SetFillStyle(4000);
  c2->SetFillColor(10);
  c2->SetTicky();
  c2->SetObjectStat(0);

  hFakeRate->SetLineColor(kRed);
  hFakeRate->Draw("E");

  hFakeRateErrDown->Draw("SAME");
  hFakeRateErrUp->Draw("SAME");

//   fitE1->Draw("SAME");
//   fitE2->Draw("SAME");
//   fitE3->Draw("SAME");
//   fitE4->Draw("SAME");
//   fitE5->Draw("SAME");
//   fitE6->Draw("SAME");

  hPurityQCD->SetLineColor(kBlack);
  hPurityQCD->Draw("SAME");
  hPuritySdb->SetLineColor(kBlue);
  hPuritySdb->Draw("SAME");
  hPurityAIs->SetLineColor(kGreen);
  hPurityAIs->Draw("SAME");

  out->cd();
  c2->Draw();
  c2->Write();
  fit->Write();
  hFakeRate->Write();
  hFakeRateErrUp->Write();
  hFakeRateErrDown->Write();



  TCanvas *c3 = new TCanvas("c3","",5,30,650,600);
  c3->SetGrid(0,0);
  c3->SetFillStyle(4000);
  c3->SetFillColor(10);
  c3->SetTicky();
  c3->SetObjectStat(0);

  for( int m = 1 ; m <= hSVfitRatio->GetNbinsX() ; m++ ){
    if(hSVfitFake->GetBinContent(m)>0 && hSVfitFakeErrUp->GetBinContent(m)>0){
      hSVfitRatio->SetBinContent( m, hSVfit->GetBinContent(m)/hSVfitFake->GetBinContent(m) ) ;
      hSVfitRatio->SetBinError(   m, TMath::Abs(hSVfit->GetBinContent(m)/hSVfitFake->GetBinContent(m) - 
						hSVfit->GetBinContent(m)/hSVfitFakeErrUp->GetBinContent(m))) ;
    }
    else {
      hSVfitRatio->SetBinContent( m, 1.0) ;
      hSVfitRatio->SetBinError(   m, 1.0) ;
    }
   
  }

  hSVfitRatio->Draw("EHIST");

  out->Write();
  out->Close();

}



void fakeRateTau( string name_ = "ElecTau_Tau_pfJetPt",
		  float nJets30 = 0,
		  float  isoCut = 0.20,
		  float  MtCut  =   60,
		  float  diTauCharge = -1 // SS
		  ){

  TCanvas *c1 = new TCanvas(("c1"+name_).c_str(),"",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);


  float Lumi   = (-47.4 + 215.6 + 955.3 + 389.9 + 706.7 + 2714);
  float lumiCorrFactor = (1-0.056);
  Lumi *= lumiCorrFactor;

  TFile* out = new TFile("FakeRate.root","UPDATE");



  TFile *fData              
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval/nTupleRun2011-ElecTau-All_run_Open_ElecTauStream.root", "READ");  

  TFile *fBackgroundWJets   
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval/nTupleWJets-ElecTau-madgraph-PUS6_run_Open_ElecTauStream.root","READ"); 
  TFile *fBackgroundDY
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval/nTupleDYJets-ElecTau-50-madgraph-PUS6_run_Open_ElecTauStream.root","READ"); 
  TFile *fBackgroundTTbar  
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval/nTupleTTJets-ElecTau-madgraph-PUS6_run_Open_ElecTauStream.root","READ"); 
  TFile *fBackgroundOthers  
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval/nTupleOthers-ElecTau-PUS6_run_Open_ElecTauStream.root","READ"); 
  
  TString tree         = "outTreePtOrdRaw";

  TTree *data          = (TTree*)fData->Get(tree);

  TTree *backgroundTTbar     = (TTree*)fBackgroundTTbar->Get(tree);
  TTree *backgroundWJets     = (TTree*)fBackgroundWJets->Get(tree);
  TTree *backgroundOthers    = (TTree*)fBackgroundOthers->Get(tree);
 
  TFile* dummy1 = new TFile("dummy1.root","RECREATE");
  cout << "Now copying g/Z -> tau+ tau- " << endl;
  TTree *backgroundDYTauTau  = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)==(23*15)");                 // g/Z -> tau+ tau-
  cout << "Now copying g/Z -> e+e- e->tau" << endl;
  TTree *backgroundDYEtoTau = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)!=(23*15) &&  leptFakeTau"); // g/Z -> mu+mu- mu->tau
  cout << "Now copying g/Z -> e+e- jet->tau" << endl;
  TTree *backgroundDYJtoTau  = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)!=(23*15) && !leptFakeTau"); // g/Z -> mu+mu- jet->tau


  bool useMt      = true;
  string antiWcut = useMt ? "MtLeg1Corr" : "-(pZetaCorr-1.5*pZetaVisCorr)" ;
  float antiWsgn  = useMt ? 40.   :  20. ; 
  float antiWsdb  = useMt ? MtCut :  40. ; 

  TCut lpt(Form("ptL1>20 && TMath::Abs(etaL1)<2.1 && nJets30>=%f",nJets30));
  TCut tpt("ptL2>20 && ptL2<999 && TMath::Abs(etaL2)<2.3");

  TCut lID("((TMath::Abs(etaL1)<0.925 && mvaPOGNonTrig>0.85) || (TMath::Abs(etaL1)<1.479 && TMath::Abs(etaL1)>0.80 && mvaPOGNonTrig>0.975) || (TMath::Abs(etaL1)>1.479 && mvaPOGNonTrig>0.985))");
  tpt = tpt&&lID;

  TCut tiso("tightestHPSMVAWP>=0   && tightestAntiECutWP>1"); 
  TCut taiso("tightestHPSMVAWP>-99 && tightestAntiECutWP>1"); 

  TCut liso("combRelIsoLeg1DBeta<0.10");
  TCut laiso(Form("combRelIsoLeg1DBetav2>=%f && combRelIsoLeg1DBetav2<0.50",isoCut));
  TCut lliso("combRelIsoLeg1DBetav2<0.20");
  TCut lveto("elecFlag==0");
  TCut SS("diTauCharge!=0");
  TCut OS("diTauCharge==0");
  TCut hltevent("pairIndex<1 && HLTx==1 && (run>=163269 || run==1)");
  TCut hltmatch("HLTmatch==1");

  TCut pZ( Form("((%s)<%f)",antiWcut.c_str(),antiWsgn));
  TCut apZ(Form("((%s)>%f)",antiWcut.c_str(),antiWsdb));

 
  vector<int> bins;
  bins.push_back(20);
  bins.push_back(22);
  bins.push_back(24);
  bins.push_back(26);
  bins.push_back(28);
  bins.push_back(30);
  bins.push_back(32);
  bins.push_back(34);
  bins.push_back(36);
  bins.push_back(40);
  bins.push_back(45);
  bins.push_back(50);
  bins.push_back(60); 
  bins.push_back(80); 
  bins.push_back(100); 

  int nBins =  bins.size() ;
  TArrayF binsT(nBins);

  for(unsigned int k = 0 ; k < nBins ; k++ )
    binsT[k] = bins[k];

  //////////////////////////////////////////////////////////////////////////////////////////

  TH1F* hFakeRateQCD         = new TH1F(("hFakeRateQCD"+name_).c_str(), " ; tau jet p_{T}; fake-rate",        nBins-1, binsT.GetArray() );
  TH1F* hFakeRateQCDErrUp    = new TH1F(("hFakeRateQCDErrUp"+name_).c_str(),   " ; tau jet p_{T}; fake-rate", nBins-1, binsT.GetArray() );
  TH1F* hFakeRateQCDErrDown  = new TH1F(("hFakeRateQCDErrDown"+name_).c_str(), " ; tau jet p_{T}; fake-rate", nBins-1, binsT.GetArray() );

  TH1F* hFakeRateW         = new TH1F(("hFakeRateW"+name_).c_str(), " ; tau jet p_{T}; fake-rate",        nBins-1, binsT.GetArray() );
  TH1F* hFakeRateWErrUp    = new TH1F(("hFakeRateWErrUp"+name_).c_str(),   " ; tau jet p_{T}; fake-rate", nBins-1, binsT.GetArray() );
  TH1F* hFakeRateWErrDown  = new TH1F(("hFakeRateWErrDown"+name_).c_str(), " ; tau jet p_{T}; fake-rate", nBins-1, binsT.GetArray() );

  //////////////////////////////////////////////////////////////////////////////////////////
 
  TH1F* hExtrap        = new TH1F("hExtrap","",1, -10,10);
  TH1F* hSVfitFake     = new TH1F(("hSVfitFake"+name_).c_str(), "",40, 0,400);
  TH1F* hSVfitFakeErrUp   = new TH1F(("hSVfitFakeErrUp"+name_).c_str(), "",40, 0,400);
  TH1F* hSVfitFakeErrDown = new TH1F(("hSVfitFakeErrDown"+name_).c_str(), "",40, 0,400);
  TH1F* hSVfitRatio    = new TH1F("hSVfitRatio", "",40, 0,400);
  TH1F* hSVfitAIso     = new TH1F(("hSVfitAIso"+name_).c_str(), "",40, 0,400);

  TCut sbinSSaIsoInclusiveAllPt    = lpt && tpt && tiso && laiso && lveto && SS && pZ  && hltevent && hltmatch;
  TCut sbinSSaIsoAIdInclusiveAllPt = lpt && tpt && taiso&& laiso && lveto && SS && pZ  && hltevent && hltmatch;

  for(int i = 0; i <  bins.size()-1 ; i++){

    float min = bins[i];
    float max = bins[i+1]  ;

    TCut tpt_i = tpt && TCut(Form("ptL2>%f && ptL2<=%f", min, max));

    TCut sbinSSaIsoInclusive;
    if(diTauCharge<0) 
      sbinSSaIsoInclusive         = lpt && tpt_i && tiso && laiso&& lveto && SS && pZ  && hltevent && hltmatch;
    else
      sbinSSaIsoInclusive         = lpt && tpt_i && tiso && laiso&& lveto && OS && pZ  && hltevent && hltmatch;
    
    TCut sbinSSaIsoaIdInclusive;
     if(diTauCharge<0) 
       sbinSSaIsoaIdInclusive      = lpt && tpt_i && taiso&& laiso&& lveto && SS && pZ  && hltevent && hltmatch;
     else
       sbinSSaIsoaIdInclusive      = lpt && tpt_i && taiso&& laiso&& lveto && OS && pZ  && hltevent && hltmatch;
     
    TCut sbinPZetaRelSSInclusive;
    if(diTauCharge<0) 
      sbinPZetaRelSSInclusive     = lpt && tpt_i && tiso && liso && lveto && SS        && hltevent && hltmatch;
    else
      sbinPZetaRelSSInclusive     = lpt && tpt_i && tiso && liso && lveto && OS        && hltevent && hltmatch;
    TCut sbinPZetaRelaIdSSInclusive;
    if(diTauCharge<0) 
      sbinPZetaRelaIdSSInclusive  = lpt && tpt_i && taiso&& liso && lveto && SS        && hltevent && hltmatch;
    else
      sbinPZetaRelaIdSSInclusive  = lpt && tpt_i && taiso&& liso && lveto && OS        && hltevent && hltmatch;
    

    hExtrap->Reset();
    data->Draw("etaL1>>hExtrap", sbinSSaIsoInclusive);
    float SSeventsAiso = hExtrap->GetEntries();

    hExtrap->Reset();
    data->Draw("etaL1>>hExtrap", sbinSSaIsoaIdInclusive);
    float SSeventsAisoAid = hExtrap->GetEntries();


    hExtrap->Reset();
    data->Draw("etaL1>>hExtrap", sbinPZetaRelSSInclusive&&apZ);
    float SSeventsApZ = hExtrap->GetEntries();

    hExtrap->Reset();
    data->Draw("etaL1>>hExtrap", sbinPZetaRelaIdSSInclusive&&apZ);
    float SSeventsAidApZ = hExtrap->GetEntries();


    float ratioQCD = SSeventsAisoAid>0 ? SSeventsAiso/SSeventsAisoAid : 0;
    float errorQCD = SSeventsAisoAid>0 ? TMath::Sqrt(ratioQCD*(1-ratioQCD)/SSeventsAisoAid) : 1.0;
    if( errorQCD == 0 ) errorQCD = 0.1;

    float ratioW = SSeventsAidApZ>0 ? SSeventsApZ/SSeventsAidApZ : 0;
    float errorW = SSeventsAidApZ>0 ? TMath::Sqrt(ratioW*(1-ratioW)/SSeventsAidApZ) : 1.0;
    if( errorW == 0 ) errorW = 0.1;

    cout << "QCD: Bin " <<  min << "," << max << " ==> " << ratioQCD << " +/- " << errorQCD << endl;
    cout << "W: Bin "   <<  min << "," << max << " ==> " << ratioW   << " +/- " << errorW   << endl;

    hFakeRateQCD->SetBinContent(i+1, ratioQCD );
    hFakeRateQCD->SetBinError(i+1,   errorQCD );

    hFakeRateW->SetBinContent(i+1, ratioW );
    hFakeRateW->SetBinError(i+1,   errorW );

    cout << "************** END extrapolation *******************" << endl;

  }


  //return;

  TF1* fitQCD = new TF1(("fitQCD_"+name_).c_str(),"[0]*TMath::Exp([1]*x)+[2]",20, bins[bins.size()-1]);
  fitQCD->SetLineColor(kRed);
  fitQCD->SetParLimits(0,0,20);
  fitQCD->SetParLimits(1,-5,0);
  fitQCD->SetParLimits(2,0,1);

  hFakeRateQCD->Fit(("fitQCD_"+name_).c_str(), "", "", 20, 1000);

  TF1* fitW = new TF1(("fitW_"+name_).c_str(),"[0]*TMath::Exp([1]*x)+[2]",20, bins[bins.size()-1]);
  fitW->SetLineColor(kBlue);
  fitW->SetParLimits(0,0,20);
  fitW->SetParLimits(1,-5,0);
  fitW->SetParLimits(2,0,1);

  hFakeRateW->Fit(("fitW_"+name_).c_str(), "", "", 20, 1000);

  for(int i = 0; i <  bins.size()-1 ; i++){

    float min = bins[i];
    float max = bins[i+1];
    double binWidth  = max-min;


    double integEQCD    = fitQCD->IntegralError(min,max );
    double integQCD     = fitQCD->Integral(min,max);
    cout << "QCD: bin " << min << "," << max << " ==> " << integQCD/binWidth << endl;
    hFakeRateQCDErrUp->SetBinContent( i+1,   (integQCD + integEQCD)/binWidth );
    hFakeRateQCDErrDown->SetBinContent( i+1, (integQCD - integEQCD)/binWidth );

    double integEW    = fitW->IntegralError(min,max );
    double integW     = fitW->Integral(min,max);
    cout << "W: bin " << min << "," << max << " ==> " << integW/binWidth << endl;
    hFakeRateWErrUp->SetBinContent( i+1,   (integW + integEW)/binWidth );
    hFakeRateWErrDown->SetBinContent( i+1, (integW - integEW)/binWidth );

  }

  hFakeRateQCDErrDown->SetLineColor(kRed);
  hFakeRateQCDErrDown->SetLineStyle(kDashed);
  hFakeRateQCDErrUp->SetLineColor(kRed);
  hFakeRateQCDErrUp->SetLineStyle(kDotted);

  hFakeRateWErrDown->SetLineColor(kBlue);
  hFakeRateWErrDown->SetLineStyle(kDashed);
  hFakeRateWErrUp->SetLineColor(kBlue);
  hFakeRateWErrUp->SetLineStyle(kDotted);


  string scaleFactQCD = "( ";
  string scaleFactQCDErrUp   = "( ";
  string scaleFactQCDErrDown = "( ";

  string scaleFactW = "( ";
  string scaleFactWErrUp   = "( ";
  string scaleFactWErrDown = "( ";


  for(int i = 0; i < bins.size()-1; i++){
    
    float min = bins[i];
    float max = bins[i+1];

    cout << min << "," << max << endl;
    float bin = hFakeRateQCD->FindBin((max+min)/2.);

    cout << bin << endl;
    float weightBinQCD_i        =  fitQCD->Eval( (max+min)/2.);
    float weightBinQCD_iErrUp   =  hFakeRateQCDErrUp->GetBinContent( bin );
    float weightBinQCD_iErrDown =  hFakeRateQCDErrDown->GetBinContent( bin );
    
    scaleFactQCD        += string( Form("(ptL2>=%f && ptL2<%f)*%f", min , max, weightBinQCD_i ) );
    scaleFactQCDErrUp   += string( Form("(ptL2>=%f && ptL2<%f)*%f", min , max, weightBinQCD_iErrUp   ) );
    scaleFactQCDErrDown += string( Form("(ptL2>=%f && ptL2<%f)*%f", min , max, weightBinQCD_iErrDown ) );

    float weightBinW_i        =  fitW->Eval( (max+min)/2.);
    float weightBinW_iErrUp   =  hFakeRateWErrUp->GetBinContent( bin );
    float weightBinW_iErrDown =  hFakeRateWErrDown->GetBinContent( bin );
    
    scaleFactW        += string( Form("(ptL2>=%f && ptL2<%f)*%f", min , max, weightBinW_i ) );
    scaleFactWErrUp   += string( Form("(ptL2>=%f && ptL2<%f)*%f", min , max, weightBinW_iErrUp   ) );
    scaleFactWErrDown += string( Form("(ptL2>=%f && ptL2<%f)*%f", min , max, weightBinW_iErrDown ) );

    if(i < bins.size() - 2 ){
      scaleFactQCD += " + ";
      scaleFactQCDErrUp   += " + ";
      scaleFactQCDErrDown += " + ";
      scaleFactW += " + ";
      scaleFactWErrUp   += " + ";
      scaleFactWErrDown += " + ";
    }
  }
  scaleFactQCD += " )";
  scaleFactQCDErrUp   += " )";
  scaleFactQCDErrDown += " )";
  
  scaleFactW += " )";
  scaleFactWErrUp   += " )";
  scaleFactWErrDown += " )";

  cout << scaleFactQCD << endl;
  cout << scaleFactW << endl;
  
  c1->cd();

  data->Draw(("diTauNSVfitMass>>hSVfitAIso"+name_).c_str(), sbinSSaIsoInclusiveAllPt);

  data->Draw(("diTauNSVfitMass>>hSVfitFake"+name_).c_str(),        (TCut(scaleFactQCD.c_str()))*sbinSSaIsoAIdInclusiveAllPt);
  data->Draw(("diTauNSVfitMass>>hSVfitFakeErrUp"+name_).c_str(),   (TCut(scaleFactQCDErrUp.c_str()))*sbinSSaIsoAIdInclusiveAllPt);
  data->Draw(("diTauNSVfitMass>>hSVfitFakeErrDown"+name_).c_str(), (TCut(scaleFactQCDErrDown.c_str()))*sbinSSaIsoAIdInclusiveAllPt);

  hSVfitFake->SetMarkerStyle(kOpenCircle);
  hSVfitFake->SetMarkerSize(1.2);
  hSVfitFake->SetMarkerColor(kRed);
  hSVfitAIso->SetLineColor(kMagenta);

  cout << "Fake " << hSVfitFake->Integral() << endl;
  cout << "AIso " << hSVfitAIso->Integral() << endl;

  hSVfitFake->Draw("P");
  hSVfitAIso->Draw("SAME");

  out->cd();
  c1->Draw();
  c1->Write();


  TCanvas *c2 = new TCanvas(("c2"+name_).c_str(),"",5,30,650,600);
  c2->SetGrid(0,0);
  c2->SetFillStyle(4000);
  c2->SetFillColor(10);
  c2->SetTicky();
  c2->SetObjectStat(0);

  hFakeRateQCD->SetLineColor(kRed);
  hFakeRateQCD->Draw("E");

  hFakeRateQCDErrDown->Draw("SAME");
  hFakeRateQCDErrUp->Draw("SAME");

  hFakeRateW->SetLineColor(kBlue);
  hFakeRateW->Draw("ESAME");

  hFakeRateWErrDown->Draw("SAME");
  hFakeRateWErrUp->Draw("SAME");

  out->cd();
  c2->Draw();
  c2->Write();
  fitQCD->Write();
  fitW->Write();

  hFakeRateQCD->Write();
  hFakeRateQCDErrUp->Write();
  hFakeRateQCDErrDown->Write();

  hFakeRateW->Write();
  hFakeRateWErrUp->Write();
  hFakeRateWErrDown->Write();


  TCanvas *c3 = new TCanvas("c3","",5,30,650,600);
  c3->SetGrid(0,0);
  c3->SetFillStyle(4000);
  c3->SetFillColor(10);
  c3->SetTicky();
  c3->SetObjectStat(0);

  for( int m = 1 ; m <= hSVfitRatio->GetNbinsX() ; m++ ){
    if(hSVfitFake->GetBinContent(m)>0 && hSVfitFakeErrUp->GetBinContent(m)>0){
      hSVfitRatio->SetBinContent( m, hSVfitAIso->GetBinContent(m)/hSVfitFake->GetBinContent(m) ) ;
      hSVfitRatio->SetBinError(   m, TMath::Abs(hSVfitAIso->GetBinContent(m)/hSVfitFake->GetBinContent(m) - 
						hSVfitAIso->GetBinContent(m)/hSVfitFakeErrUp->GetBinContent(m))) ;
    }
    else {
      hSVfitRatio->SetBinContent( m, 1.0) ;
      hSVfitRatio->SetBinError(   m, 1.0) ;
    }
   
  }

  hSVfitRatio->Draw("EHIST");

  out->Write();
  out->Close();

}


void makeAll(){

  //fakeRateElec("ElecTau_Elec_ptL1_incl",0);
  //fakeRateElec("ElecTau_Elec_ptL1_1jet",1);
  //fakeRateElec("ElecTau_Elec_ptL1_2jet",2);

  //fakeRateTau("ElecTau_Tau_ptL2_QCDSS03_WSS60_incl", 0, 0.30, 60, -1);
  fakeRateTau("ElecTau_Tau_ptL2_QCDSS02_WSS60_incl", 0, 0.20, 60, -1);
  //fakeRateTau("ElecTau_Tau_ptL2_QCDSS00_WSS60_incl", 0, 0.00, 60, -1);

  //fakeRateTau("ElecTau_Tau_ptL2_QCDOS02_WOS70_incl", 0, 0.20, 70, +1);
  fakeRateTau("ElecTau_Tau_ptL2_QCDOS02_WOS60_incl", 0, 0.20, 60, +1);

  //fakeRateTau("ElecTau_Tau_ptL2_QCDOS03_WOS60_1jet", 1, 0.30, 60, +1);
  //fakeRateTau("ElecTau_Tau_ptL2_QCDOS03_WOS60_2jet", 2, 0.30, 60, +1);

  //fakeRateTau("ElecTau_Tau_ptL2_QCDSS03_WSS60_1jet", 1, 0.30, 60, -1);
  //fakeRateTau("ElecTau_Tau_ptL2_QCDSS03_WSS60_2jet", 2, 0.30, 60, -1);

}
