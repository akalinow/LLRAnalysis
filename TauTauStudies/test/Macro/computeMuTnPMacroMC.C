#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooDataSet.h"
#include "RooPlot.h"
#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooCBShape.h"
#include "RooGaussian.h"
#include "RooExponential.h"
#include "RooFormulaVar.h"
#include "RooLandau.h"
#include "RooUniform.h"
#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooFitResult.h"
#include "RooKeysPdf.h"
#include "RooBifurGauss.h"
#include "RooBreitWigner.h"
#include "RooVoigtian.h"
#include "RooSimultaneous.h"
#include "RooCategory.h"
#include "RooAbsCategory.h"
#include "RooNumConvPdf.h"
#include "RooFFTConvPdf.h"
#include "RooTruthModel.h"
#include "RooDataHist.h"
#include "RooGenericPdf.h"
#include "RooConstVar.h"
#include "RooLognormal.h"

#include <vector>
#include <fstream>

#include "TTree.h"
#include "TFile.h"
#include "TMath.h"
#include "TH1F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGraphAsymmErrors.h"


using namespace std;
using namespace RooFit;


vector<float*> simFit(
		      const string tnp_      = "muTnP",
		      const string category_ = "muID",
		      const string binLabel_ = "15to20",
		      double cutValue_       = 0.5,
		      const string bin_      = "abseta>1.5",
		      const float binCenter_ = 0.75,
		      const float binWidth_  = 0.75,
		      const float xLow_      = 40,
		      const float xHigh_     = 120,
		      const float nBins_     = 24,
		      bool doBinned_         = true,
		      float deltaAlpha_      = 0.0,
		      float deltaN_          = 0.0,
		      float scale_           = 0.0,
		      float mcLumi_          = 500,
		      bool doMC_             = false
		      )
  
{


  vector<float*> out;

  TFile fSgn("../prod/MuTnP/treeMuTnP_DYJets-Mu-50-madgraph-PUS4-TnP.root");
  TTree *fullTreeSgn  = (TTree*)fSgn.Get((tnp_+"/fitter_tree").c_str());
  fSgn.cd("allEventsFilter");
  TH1F* totalEventsSgn = (TH1F*)gDirectory->Get("totalEvents");
  float readEventsSgn = totalEventsSgn->GetBinContent(1);

  // data
  TFile fdat("../prod/MuTnP/treeMuTnP_Run2011-Muon-TnP.root");
  TTree *fullTreeData = (TTree*)fdat.Get((tnp_+"/fitter_tree").c_str());
  

  TFile fZtau("../prod/MuTnP/treeMuTnP_DYJets-Mu-50-madgraph-PUS4-TnP.root");
  TTree *fullTreeZtt  = (TTree*)fZtau.Get((tnp_+"/fitter_tree").c_str());
  fZtau.cd("allEventsFilter");
  TH1F* totalEventsZtt = (TH1F*)gDirectory->Get("totalEvents");
  float readEventsZtt = totalEventsZtt->GetBinContent(1);

  TFile fWen("../prod/MuTnP/treeMuTnP_WJets-Mu-madgraph-PUS4-TnP.root");
  TTree *fullTreeWen = (TTree*)fWen.Get((tnp_+"/fitter_tree").c_str());
  fWen.cd("allEventsFilter");
  TH1F* totalEventsWen = (TH1F*)gDirectory->Get("totalEvents");
  float readEventsWen = totalEventsWen->GetBinContent(1);

  TFile fTTb("../prod/MuTnP/treeMuTnP_TT-Mu-pythia-PUS3-TnP.root");
  TTree *fullTreeTTb  = (TTree*)fTTb.Get((tnp_+"/fitter_tree").c_str());
  fTTb.cd("allEventsFilter");
  TH1F* totalEventsTTb = (TH1F*)gDirectory->Get("totalEvents");
  float readEventsTTb = totalEventsTTb->GetBinContent(1);




  TH1F* hS           = new TH1F("hS","",1,0,150);
  TH1F* hSP          = new TH1F("hSP","",1,0,150);
  
  fullTreeSgn->Draw("mass>>hS",Form("tag_puMCWeight*(%s && mass>%f && mass<%f && mcTrue && tag_genDecay==23*13 && tag_pfRelIso<0.1 && pair_tnpCharge==0 && event_met_pfmet<25 && tag_pt>25 && tag_hlt)",bin_.c_str(),xLow_,xHigh_));
  float SGNtrue = hS->Integral();
  fullTreeSgn->Draw("mass>>hSP",Form("tag_puMCWeight*(%s && %s>=%f && mass>%f && mass<%f && mcTrue && tag_genDecay==23*13 && tag_pfRelIso<0.1 && pair_tnpCharge==0 && event_met_pfmet<25 && tag_pt>25 && tag_hlt)",bin_.c_str(),category_.c_str(),cutValue_,xLow_,xHigh_));
  float SGNtruePass = hSP->Integral();

  float McTruthEff    = SGNtruePass/SGNtrue;
  float BinomialError = TMath::Sqrt(SGNtruePass/SGNtrue*(1-SGNtruePass/SGNtrue)/SGNtrue);
  
  cout << bin_.c_str() << " ==> MCTRUTH: " << McTruthEff << " +/- " << BinomialError << endl;

  delete hS; delete hSP;

  float reductionFactor = 0.3;
  // file to copy the trees
  TFile *templFile = new TFile(Form("dummyMuTempl_%s_bin%.2f.root",binLabel_.c_str(),binCenter_),"RECREATE");
  
  TTree* fullTreeSgnCutP = fullTreeSgn->CopyTree( Form("(%s>=%f && %s && pair_tnpCharge==0 && event_met_pfmet<25 && mcTrue && tag_genDecay==23*13 && tag_pfRelIso<0.1 && tag_pt>25 && tag_hlt && mass>%f && mass<%f)",category_.c_str(),cutValue_,bin_.c_str(),xLow_,xHigh_) ,"",int(fullTreeSgn->GetEntries()*reductionFactor));
  TTree* fullTreeSgnCutF = fullTreeSgn->CopyTree( Form("(%s< %f && %s && pair_tnpCharge==0 && event_met_pfmet<25 && mcTrue && tag_genDecay==23*13 && tag_pfRelIso<0.1 && tag_pt>25 && tag_hlt && mass>%f && mass<%f)",category_.c_str(),cutValue_,bin_.c_str(),xLow_,xHigh_) ,"",int(fullTreeSgn->GetEntries()*reductionFactor));
  
  float unWeightedMCEff = float(fullTreeSgnCutP->GetEntries())/(fullTreeSgnCutF->GetEntries()+fullTreeSgnCutP->GetEntries());
  int sgnPToAppend =  unWeightedMCEff<McTruthEff ? int(fullTreeSgnCutP->GetEntries()) : int(McTruthEff/(1-McTruthEff)*fullTreeSgnCutF->GetEntries()) ;
  int sgnFToAppend =  unWeightedMCEff>McTruthEff ? int(fullTreeSgnCutF->GetEntries()) : int((1-McTruthEff)/McTruthEff*fullTreeSgnCutP->GetEntries()) ; 

  cout << "unWeightedMCEff = " << unWeightedMCEff << " %%%%%%%  sgnToAppend PASS " << sgnPToAppend << " -- sgnToAppend FAIL " << sgnFToAppend << endl;

  RooRealVar mass("mass","m_{tp} (GeV/c^{2})",xLow_,xHigh_);
  mass.setBins( 10000, "fft" );
  mass.setBins( nBins_ );

  RooRealVar meanBkgP("meanBkgP","",59,30,100);
  RooRealVar sigmaBkgP("sigmaBkgP","",11,0,50);
  //RooLandau bkgPdfP("bkgPdfP","",mass,meanBkgP,sigmaBkgP);

  RooRealVar DataCP("DataCP","",0,-10,10);
  RooExponential bkgPdfP("bkgPdfP","",mass,DataCP);

  RooRealVar meanBkgF("meanBkgF","",59,30,100);
  RooRealVar sigmaBkgF("sigmaBkgF","",11,0,50);
  //RooLandau bkgPdfF("bkgPdfF","",mass,meanBkgF,sigmaBkgF);

  RooRealVar DataCF("DataCF","",0,-10,10);
  RooExponential bkgPdfF("bkgPdfF","",mass,DataCF);

  TCanvas *c0 = new TCanvas("fitCanvas","canvas",10,30,650,600);
  c0->SetGrid(0,0);
  c0->SetFillStyle(4000);
  c0->SetFillColor(10);
  c0->SetTicky();
  c0->SetObjectStat(0);

  mass.setBins( 50 );

  /////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////// 
  // passing:

  RooDataSet sgnDataSetP("sgnDataSetP","dataset for signal", RooArgSet(mass), Import( *fullTreeSgnCutP ) );
  RooDataHist sgnDataHistP("sgnDataHistP","",RooArgSet(mass),sgnDataSetP, 1.0);
  //RooHistPdf  sgnTemplatePdfP("sgnTemplatePdfP","",RooArgSet(mass),sgnDataHistP);
  RooKeysPdf sgnTemplatePdfP("sgnTemplatePdfP","",mass,sgnDataSetP);

  // Breit-Wigner
  RooRealVar meanSgnP("meanSgnP","mean",91.19/*,85,100*/);
  RooRealVar widthSgnP("widthSgnP","width",2.49/*,0.,10*/);
  RooBreitWigner bwSgnP("bwSgnP","bw",mass,meanSgnP,widthSgnP);

  // Crystall Ball
  RooRealVar m1SgnP("m1SgnP","m1",0,-20,20);
  RooRealVar sigmaSgnP("sigmaSgnP","sigma",0.5,0,20);
  RooRealVar alfaSgnP("alfaSgnP","alfa", 0.5,-10,10);
  RooRealVar nSgnP("nSgnP","n", 1,1e-06,50);
  RooCBShape cbSgnP("cbSgnP","",mass,m1SgnP,sigmaSgnP,alfaSgnP,nSgnP);

  mass.setBins( 1000 , "fft");
  // BW (X) CB
  RooFFTConvPdf bvcbSgnP("bvcbSgnP","",mass,bwSgnP, cbSgnP);

  RooFitResult* ResSgnFitP = bvcbSgnP.fitTo(sgnDataSetP, Minos(1), Save(1), NumCPU(4) );
  RooArgSet FitParamSgnP(ResSgnFitP->floatParsFinal());
  RooRealVar* m1SgnFitP    = (RooRealVar*)(&FitParamSgnP["m1SgnP"]);
  RooRealVar* sigmaSgnFitP = (RooRealVar*)(&FitParamSgnP["sigmaSgnP"]);
  RooRealVar* alfaSgnFitP  = (RooRealVar*)(&FitParamSgnP["alfaSgnP"]);
  RooRealVar* nSgnFitP     = (RooRealVar*)(&FitParamSgnP["nSgnP"]);
  //RooRealVar* nMeanSgnFitP = (RooRealVar*)(&FitParamSgnP["meanSgnP"]);
  //RooRealVar* nWidthSgnFitP= (RooRealVar*)(&FitParamSgnP["widthSgnP"]);

  RooRealVar m1SgnP_C("m1SgnP_C","m1",m1SgnFitP->getVal(),-10,10);
  RooRealVar sigmaSgnP_C("sigmaSgnP_C","sigma",sigmaSgnFitP->getVal(),0,20);
  // choose to let it float or not
  
  //RooRealVar meanSgnP_C("meanSgnP_C","mean",  nMeanSgnFitP->getVal() ,80,120);
  //RooRealVar widthSgnP_C("widthSgnP_C","width",nWidthSgnFitP->getVal() /*,0.,10*/);

  RooRealVar alfaSgnP_C("alfaSgnP_C","alfa",alfaSgnFitP->getVal()*(1+deltaAlpha_)/*,0,20*/);
  RooRealVar nSgnP_C("nSgnP_C","n",nSgnFitP->getVal()*(1+deltaN_)/*,0,50*/);

  RooCBShape cbSgnP_C("cbSgnP_C","",mass,m1SgnP_C,sigmaSgnP_C,alfaSgnP_C,nSgnP_C);
  
  RooLognormal alfaSgnP_CPdf("alfaSgnP_CPdf","",alfaSgnP_C,RooConst(alfaSgnFitP->getVal()),RooConst(1.5));
  RooLognormal nSgnP_CPdf("nSgnP_CPdf","",nSgnP_C,RooConst(nSgnFitP->getVal()),            RooConst(1.5));

  //RooLognormal meanSgnP_CPdf("meanSgnP_CPdf","",meanSgnP_C,RooConst(nMeanSgnFitP->getVal()),RooConst(1.5));
  //RooLognormal widthSgnP_CPdf("widthSgnP_CPdf","",widthSgnP_C,RooConst(nWidthSgnFitP->getVal()),RooConst(1.5));

  // fitted BW (X) CB
  RooBreitWigner bwSgnP_C("bwSgnP_C","bw",mass,meanSgnP/*meanSgnP_C*/,widthSgnP/*widthSgnP_C*/);
  RooFFTConvPdf sgnPdfP("sgnPdfP","",mass,bwSgnP_C, cbSgnP_C);


  RooRealVar sgnMeanResP("sgnMeanResP","",0,-10,10);
  RooRealVar sgnSigmaResP("sgnSigmaResP","",0.5,0,10);
  RooGaussian resolModP("sgnResolModP","",mass,sgnMeanResP,sgnSigmaResP);
  //RooFFTConvPdf sgnPdfP("sgnPdfP","",mass,sgnTemplatePdfP, resolModP);

  mass.setBins(nBins_);
  //return;

  /////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////// 
  // failing:
  RooDataSet sgnDataSetF("sgnDataSetF","dataset for signal", RooArgSet(mass), Import( *fullTreeSgnCutF ) );
  RooDataHist sgnDataHistF("sgnDataHistF","",RooArgSet(mass),sgnDataSetF, 1.0);
  //RooHistPdf  sgnTemplatePdfF("sgnTemplatePdfF","",RooArgSet(mass),sgnDataHistF);
  RooKeysPdf sgnTemplatePdfF("sgnTemplatePdfF","",mass,sgnDataSetF);

  // Breit-Wigner
  RooRealVar meanSgnF("meanSgnF","mean",91.19/*,85,100*/);
  RooRealVar widthSgnF("widthSgnF","width",2.49/*,0.,10*/);
  RooBreitWigner bwSgnF("bwSgnF","bw",mass,meanSgnF,widthSgnF);

  // Crystall Ball
  RooRealVar m1SgnF("m1SgnF","m1",0,-20,20);
  RooRealVar sigmaSgnF("sigmaSgnF","sigma",0.5,0,20);
  RooRealVar alfaSgnF("alfaSgnF","alfa", 0.5,-10,10);
  RooRealVar nSgnF("nSgnF","n", 1,1e-06,50);
  RooCBShape cbSgnF("cbSgnF","",mass,m1SgnF,sigmaSgnF,alfaSgnF,nSgnF);

  // BW (X) CB
  RooFFTConvPdf bvcbSgnF("bvcbSgnF","",mass,bwSgnF, cbSgnF);

  RooFitResult* ResSgnFitF = bvcbSgnF.fitTo(sgnDataSetF, Minos(1), Save(1), NumCPU(4) );
  RooArgSet FitParamSgnF(ResSgnFitF->floatParsFinal());
  RooRealVar* m1SgnFitF    = (RooRealVar*)(&FitParamSgnF["m1SgnF"]);
  RooRealVar* sigmaSgnFitF = (RooRealVar*)(&FitParamSgnF["sigmaSgnF"]);
  RooRealVar* alfaSgnFitF  = (RooRealVar*)(&FitParamSgnF["alfaSgnF"]);
  RooRealVar* nSgnFitF     = (RooRealVar*)(&FitParamSgnF["nSgnF"]);
  //RooRealVar* nMeanSgnFitF = (RooRealVar*)(&FitParamSgnF["meanSgnF"]);
  //RooRealVar* nWidthSgnFitF= (RooRealVar*)(&FitParamSgnF["widthSgnF"]);

  RooRealVar m1SgnF_C("m1SgnF_C","m1",m1SgnFitF->getVal(),-10,10);
  RooRealVar sigmaSgnF_C("sigmaSgnF_C","sigma",sigmaSgnFitF->getVal(),0,20);
  // choose to let it float or not
  
  // RooRealVar meanSgnF_C("meanSgnF_C","mean",  nMeanSgnFitF->getVal() ,80,120);
  //RooRealVar widthSgnF_C("widthSgnF_C","width",nWidthSgnFitF->getVal() /*,0.,10*/);

  RooRealVar alfaSgnF_C("alfaSgnF_C","alfa",alfaSgnFitF->getVal()*(1+deltaAlpha_)/*,0,20*/);
  RooRealVar nSgnF_C("nSgnF_C","n",nSgnFitF->getVal()*(1+deltaN_)/*,0,50*/);

  RooCBShape cbSgnF_C("cbSgnF_C","",mass,m1SgnF_C,sigmaSgnF_C,alfaSgnF_C,nSgnF_C);
  
  RooLognormal alfaSgnF_CPdf("alfaSgnF_CPdf","",alfaSgnF_C,RooConst(alfaSgnFitF->getVal()),RooConst(1.5));
  RooLognormal nSgnF_CPdf("nSgnF_CPdf","",nSgnF_C,RooConst(nSgnFitF->getVal()),            RooConst(1.5));

  //RooLognormal meanSgnF_CPdf("meanSgnF_CPdf","",meanSgnF_C,RooConst(nMeanSgnFitF->getVal()),RooConst(1.5));
  //RooLognormal widthSgnF_CPdf("widthSgnF_CPdf","",widthSgnF_C,RooConst(nWidthSgnFitF->getVal()),RooConst(1.5));

  // fitted BW (X) CB
  RooBreitWigner bwSgnF_C("bwSgnF_C","bw",mass,meanSgnF/*meanSgnF_C*/,widthSgnF/*widthSgnF_C*/);
  RooFFTConvPdf sgnPdfF("sgnPdfF","",mass,bwSgnF_C, cbSgnF_C);


  RooRealVar sgnMeanResF("sgnMeanResF","",0,-10,10);
  RooRealVar sgnSigmaResF("sgnSigmaResF","",0.5,0,10);
  RooGaussian resolModF("sgnResolModF","",mass,sgnMeanResF,sgnSigmaResF);
  //RooFFTConvPdf sgnPdfF("sgnPdfF","",mass,sgnTemplatePdfF, resolModF);


  mass.setBins(nBins_);
  //return;





  // Fit
  RooCategory category("category","category") ;
  category.defineType("pass") ;
  category.defineType("fail") ;

  RooRealVar DataNumBkgF("DataNumBkgF","",0,10000000);
  RooRealVar DataNumBkgP("DataNumBkgP","",0,10000000);
  RooRealVar DataNumSgn("DataNumSgn","",  0,10000000);
  RooRealVar DataEfficiency("DataEfficiency","",0.5,0,1);
  
  RooFormulaVar DataNumSgnP("DataNumSgnP","DataEfficiency*DataNumSgn",    RooArgSet(DataEfficiency,DataNumSgn));
  RooFormulaVar DataNumSgnF("DataNumSgnF","(1-DataEfficiency)*DataNumSgn",RooArgSet(DataEfficiency,DataNumSgn));
 
  RooAddPdf DataModelP("DataModelP","",RooArgList(sgnPdfP/*sgnTemplatePdfP*/,bkgPdfP),RooArgList(DataNumSgnP,DataNumBkgP));
  RooAddPdf DataModelF("DataModelF","",RooArgList(sgnPdfF/*sgnTemplatePdfF*/,bkgPdfF),RooArgList(DataNumSgnF,DataNumBkgF));
  
  TFile* dummyData = new TFile(("dummyMuData_"+binLabel_+".root").c_str(),"RECREATE");
  TTree* fullTreeDataCutP = fullTreeData->CopyTree( Form("(%s>=%f && %s  && tag_pfRelIso<0.1 && pair_tnpCharge==0 && event_met_pfmet<25 && tag_hlt && tag_pt>25)",category_.c_str(),cutValue_,bin_.c_str()) ); 
  TTree* fullTreeDataCutF = fullTreeData->CopyTree( Form("(%s <%f && %s  && tag_pfRelIso<0.1 && pair_tnpCharge==0 && event_met_pfmet<25 && tag_hlt && tag_pt>25)",category_.c_str(),cutValue_,bin_.c_str()) );

  mass.setBins(nBins_);
  RooDataSet DataDataSetP("DataDataSetP","dataset for Data pass", RooArgSet(mass), Import( *fullTreeDataCutP ) );
  std::cout << "data dataset Pass " << DataDataSetP.numEntries() << "  " << std::endl;
  //return out;
  RooDataHist DataDataHistP("DataDataHistP","",RooArgSet(mass),DataDataSetP, 1.0);
  RooDataSet DataDataSetF("DataDataSetF","dataset for Data fail", RooArgSet(mass), Import( *fullTreeDataCutF ) );
  std::cout << "data dataset Fail " << DataDataSetF.numEntries() << "  " << std::endl;
  RooDataHist DataDataHistF("DataDataHistF","",RooArgSet(mass),DataDataSetF, 1.0);


  TFile* dummyBkg = new TFile(Form("dummyBkgMu%s_%.2f.root",binLabel_.c_str(),binCenter_),"RECREATE");
  TTree* fullTreeZttCutP = fullTreeZtt->CopyTree( Form("(%s>=%f && %s  && tag_pfRelIso<0.1 && pair_tnpCharge==0 && event_met_pfmet<25 && tag_pt>25 && tag_genDecay==23*15  && tag_hlt && mass>%f && mass<%f)",category_.c_str(),cutValue_,bin_.c_str(),xLow_,xHigh_) ); 
  TTree* fullTreeWenCutP = fullTreeWen->CopyTree( Form("(%s>=%f && %s  && tag_pfRelIso<0.1 && pair_tnpCharge==0 && event_met_pfmet<25 && tag_pt>25 && tag_hlt && mass>%f && mass<%f)",category_.c_str(),cutValue_,bin_.c_str(), xLow_, xHigh_) ); 
  TTree* fullTreeTTbCutP = fullTreeTTb->CopyTree( Form("(%s>=%f && %s  && tag_pfRelIso<0.1 && pair_tnpCharge==0 && event_met_pfmet<25 && tag_pt>25 && tag_hlt && mass>%f && mass<%f)",category_.c_str(),cutValue_,bin_.c_str(), xLow_, xHigh_) ); 
  //
  TTree* fullTreeZttCutF = fullTreeZtt->CopyTree( Form("(%s< %f && %s  && tag_pfRelIso<0.1 && pair_tnpCharge==0 && event_met_pfmet<25 && tag_pt>25 && tag_genDecay==23*15  && tag_hlt && mass>%f && mass<%f)",category_.c_str(),cutValue_,bin_.c_str(),xLow_,xHigh_) ); 
  TTree* fullTreeWenCutF = fullTreeWen->CopyTree( Form("(%s< %f && %s  && tag_pfRelIso<0.1 && pair_tnpCharge==0 && event_met_pfmet<25 && tag_pt>25 && tag_hlt && mass>%f && mass<%f)",category_.c_str(),cutValue_,bin_.c_str(),xLow_,xHigh_) ); 
  TTree* fullTreeTTbCutF = fullTreeTTb->CopyTree( Form("(%s< %f && %s  && tag_pfRelIso<0.1 && pair_tnpCharge==0 && event_met_pfmet<25 && tag_pt>25 && tag_hlt && mass>%f && mass<%f)",category_.c_str(),cutValue_,bin_.c_str(),xLow_,xHigh_) ); 

  TH1F* hEta = new TH1F("hEta","",1,-10,10); 

  RooDataSet ZttDataSetP("ZttDataSetP","", RooArgSet(mass), Import( *fullTreeZttCutP ) );
  RooDataSet WenDataSetP("WenDataSetP","", RooArgSet(mass), Import( *fullTreeWenCutP ) );
  RooDataSet TTbDataSetP("TTbDataSetP","", RooArgSet(mass), Import( *fullTreeTTbCutP ) );

  RooDataSet ZttDataSetF("ZttDataSetF","", RooArgSet(mass), Import( *fullTreeZttCutF ) );
  RooDataSet WenDataSetF("WenDataSetF","", RooArgSet(mass), Import( *fullTreeWenCutF ) );
  RooDataSet TTbDataSetF("TTbDataSetF","", RooArgSet(mass), Import( *fullTreeTTbCutF ) );

  if(doMC_){
    
    cout << "Pass " << endl;
    
    DataDataSetP.reset();

    //fullTreeSgnCutP->Draw("abseta>>hEta","tag_puMCWeight");
    //cout << "Sgn ==> " << fullTreeSgnCutP->GetEntries() << " --- req. " << (int)(hEta->Integral()*mcLumi_/(readEventsSgn/3048.)) << endl;
    cout << "Sgn ==> " << fullTreeSgnCutP->GetEntries() << " --- req. " << (int)(sgnPToAppend*mcLumi_/(readEventsSgn*reductionFactor/3048.)) << endl;
    //DataDataSetP.append( *((RooDataSet*)sgnDataSetP.reduce(EventRange(1,(int)(hEta->Integral()*mcLumi_/(readEventsSgn/3048.))  )))  );
    DataDataSetP.append( *((RooDataSet*)sgnDataSetP.reduce(EventRange(1,(int)(sgnPToAppend*mcLumi_/(readEventsSgn*reductionFactor/3048.))  )))  );
    hEta->Reset();

    cout << DataDataSetP.numEntries() << endl;

    fullTreeZttCutP->Draw("abseta>>hEta","tag_puMCWeight");
    cout << "Ztt ==> " << fullTreeZttCutP->GetEntries() << " --- req. " << (int)(hEta->Integral()*mcLumi_/(readEventsZtt/3048.)) << endl;
    DataDataSetP.append( *((RooDataSet*)ZttDataSetP.reduce(EventRange(1,(int)(hEta->Integral()*mcLumi_/(readEventsZtt/3048.))  )))  );
    hEta->Reset();

    cout << DataDataSetP.numEntries() << endl;

    fullTreeWenCutP->Draw("abseta>>hEta","tag_puMCWeight");
    cout << "Wen ==> " << fullTreeWenCutP->GetEntries() << " --- req. " << (int)(hEta->Integral()*mcLumi_/(readEventsWen/31314.)) << endl;
    DataDataSetP.append( *((RooDataSet*)WenDataSetP.reduce(EventRange(1,(int)(hEta->Integral()*mcLumi_/(readEventsWen/31314.))  )))  );
    hEta->Reset();

    cout << DataDataSetP.numEntries() << endl;

    fullTreeTTbCutP->Draw("abseta>>hEta","tag_puMCWeight");
    cout << "TTb ==> " << fullTreeTTbCutP->GetEntries() << " --- req. " << (int)(hEta->Integral()*mcLumi_/(readEventsTTb/157.5)) << endl;
    DataDataSetP.append( *((RooDataSet*)TTbDataSetP.reduce(EventRange(1,(int)(hEta->Integral()*mcLumi_/(readEventsTTb/157.5))  )))  );
    hEta->Reset();

    cout << DataDataSetP.numEntries() << endl;


    //////////////////////////////////////
    cout << "Fail " << endl;
    //////////////////////////////////////

    DataDataSetF.reset();

    fullTreeSgnCutF->Draw("abseta>>hEta","tag_puMCWeight");
    //cout << "Sgn ==> " << fullTreeSgnCutF->GetEntries() << " --- req. " << (int)(hEta->Integral()*mcLumi_/(readEventsSgn/3048.)) << endl;
    cout << "Sgn ==> " << fullTreeSgnCutF->GetEntries() << " --- req. " << (int)(sgnFToAppend*mcLumi_/(readEventsSgn*reductionFactor/3048.)) << endl;
    //DataDataSetF.append( *((RooDataSet*)sgnDataSetF.reduce(EventRange(1,(int)(hEta->Integral()*mcLumi_/(readEventsSgn/3048.))  )))  );
    DataDataSetF.append( *((RooDataSet*)sgnDataSetF.reduce(EventRange(1,(int)(sgnFToAppend*mcLumi_/(readEventsSgn*reductionFactor/3048.))  )))  );  
    hEta->Reset();

    fullTreeZttCutF->Draw("abseta>>hEta","tag_puMCWeight");
    cout << "Ztt ==> " << fullTreeZttCutF->GetEntries() << " --- req. " << (int)(hEta->Integral()*mcLumi_/(readEventsZtt/3048.)) << endl;
    DataDataSetF.append( *((RooDataSet*)ZttDataSetF.reduce(EventRange(1,(int)(hEta->Integral()*mcLumi_/(readEventsZtt/3048.))  )))  );
    hEta->Reset();

    fullTreeWenCutF->Draw("abseta>>hEta","tag_puMCWeight");
    cout << "Wen ==> " << fullTreeWenCutF->GetEntries() << " --- req. " << (int)(hEta->Integral()*mcLumi_/(readEventsWen/31314.)) << endl;
    DataDataSetF.append( *((RooDataSet*)WenDataSetF.reduce(EventRange(1,(int)(hEta->Integral()*mcLumi_/(readEventsWen/31314.))  )))  );
    hEta->Reset();

    fullTreeTTbCutF->Draw("abseta>>hEta","tag_puMCWeight");
    cout << "TTb ==> " << fullTreeTTbCutF->GetEntries() << " --- req. " << (int)(hEta->Integral()*mcLumi_/(readEventsTTb/157.5)) << endl;
    DataDataSetF.append( *((RooDataSet*)TTbDataSetF.reduce(EventRange(1,(int)(hEta->Integral()*mcLumi_/(readEventsTTb/157.5))  )))  );
    hEta->Reset();
   
  }

  RooRealVar DataNumSgnP_("DataNumSgnP_","",0,10000);
  RooAddPdf DataModelP_("DataModelP_","",RooArgList(sgnPdfP,bkgPdfP),RooArgList(DataNumSgnP_,DataNumBkgP));
  //DataModelP_.fitTo(DataDataSetP, Extended(1), Minos(1), Save(1), NumCPU(4),SumW2Error(1) /*,ExternalConstraints( RooArgSet(meanSgn_CPdf,widthSgn_CPdf) )*/);

  RooPlot* frame2 = mass.frame(Title("template"));
  DataDataSetP.plotOn(frame2);
  DataModelP_.plotOn(frame2, LineColor(kBlue), LineStyle(kSolid));
  DataModelP_.plotOn(frame2, Components("sgnPdfP"), LineColor(kRed), LineStyle(kSolid));
  DataModelP_.plotOn(frame2, Components("bkgPdfP"), LineColor(kGreen), LineStyle(kSolid));
  frame2->Draw();

  //return;



  // binned combined dataset
  RooDataHist DataCombData("DataCombData","combined data",mass,Index(category),Import("pass", *(DataDataSetP.createHistogram("histoDataP",mass)) ) ,Import("fail", *(DataDataSetF.createHistogram("histoDataF",mass))), Weight(0.5) ) ;
  std::cout << "data dataHist Comb " << DataCombData.sumEntries() << "  " << std::endl;
  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  // unbinned combined dataset
  RooDataSet DataCombDataUnBinned("DataCombDataUnBinned","combined data",mass,Index(category),Import("pass", DataDataSetP ) ,Import("fail",DataDataSetF), Weight(0.5) ) ;
  std::cout << "data dataset Comb " << DataCombDataUnBinned.numEntries() << "  " << std::endl;
  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  //return out;
  
  RooSimultaneous DataSimPdf("DataSimPdf","simultaneous pdf",category) ;
  DataSimPdf.addPdf(DataModelP,"pass") ;
  DataSimPdf.addPdf(DataModelF,"fail") ;

  //mass.setBins( 10000, "fft" );
  mass.setBins( nBins_ );
  RooFitResult* ResDataCombinedFit =  0;
  if(doBinned_)  ResDataCombinedFit = DataSimPdf.fitTo(DataCombData , Extended(1), Minos(1), Save(1), NumCPU(4), /*ExternalConstraints( RooArgSet(alfaSgn_CPdf,nSgn_CPdf) )*/  SumW2Error(1));
  else ResDataCombinedFit = DataSimPdf.fitTo(DataCombDataUnBinned , Extended(1), Minos(1), Save(1), NumCPU(4),  /*ExternalConstraints( RooArgSet(alfaSgn_CPdf,nSgn_CPdf) )*/ SumW2Error(1));


  RooArgSet DataFitParam(ResDataCombinedFit->floatParsFinal());
  RooRealVar* DataEffFit      = (RooRealVar*)(&DataFitParam["DataEfficiency"]);
  RooRealVar* DataNumSigFit   = (RooRealVar*)(&DataFitParam["DataNumSgn"]);

  string theSample = doMC_ ? "Simulation" : "Data" ;
  float theLumi = doMC_ ? mcLumi_ : 195.;

  RooPlot* DataFrameP = mass.frame(Bins(40),Title(Form("CMS Preliminary 2011  #sqrt{s}=7 TeV %s  L=%.0f pb^{-1}:  passing probe",theSample.c_str(),theLumi)));
  DataCombData.plotOn(DataFrameP,Cut("category==category::pass"));
  DataSimPdf.plotOn(DataFrameP,Slice(category,"pass"), ProjWData(category,DataCombData), LineColor(kBlue));
  DataSimPdf.plotOn(DataFrameP,Slice(category,"pass"), ProjWData(category,DataCombData), Components("sgnPdfP"), LineColor(kRed), LineStyle(kSolid));
  DataSimPdf.plotOn(DataFrameP,Slice(category,"pass"), ProjWData(category,DataCombData), Components("bkgPdfP"), LineColor(kMagenta), LineStyle(kDashed));
 

  RooPlot* DataFrameF = mass.frame(Bins(40),Title(Form("CMS Preliminary 2011  #sqrt{s}=7 TeV %s  L=%.0f pb^{-1}:  failing probe",theSample.c_str(),theLumi)));
  DataCombData.plotOn(DataFrameF,Cut("category==category::fail"));
  DataSimPdf.plotOn(DataFrameF,Slice(category,"fail"), ProjWData(category,DataCombData), LineColor(kBlue));
  DataSimPdf.plotOn(DataFrameF,Slice(category,"fail"), ProjWData(category,DataCombData), Components("sgnPdfF"), LineColor(kRed), LineStyle(kSolid));
  DataSimPdf.plotOn(DataFrameF,Slice(category,"fail"), ProjWData(category,DataCombData), Components("bkgPdfF"), LineColor(kMagenta), LineStyle(kDashed));


  TCanvas *cPass = new TCanvas("fitCanvasP","canvas",10,30,650,600);
  cPass->SetGrid(0,0);
  cPass->SetFillStyle(4000);
  cPass->SetFillColor(10);
  cPass->SetTicky();
  cPass->SetObjectStat(0);

  cPass->cd();
  DataFrameP->Draw();
  string fileNameP = "fitCanvasPassMuTnP_"+tnp_+"_"+category_;
  if(doMC_) fileNameP = fileNameP+"MC";
  else fileNameP = fileNameP+"DATA";
  cPass->SaveAs(Form("./MuTnP/%s_%s.png",fileNameP.c_str(), binLabel_.c_str()));

  TCanvas *cFail = new TCanvas("fitCanvasF","canvas",10,30,650,600);
  cFail->SetGrid(0,0);
  cFail->SetFillStyle(4000);
  cFail->SetFillColor(10);
  cFail->SetTicky();
  cFail->SetObjectStat(0);

  cFail->cd();
  DataFrameF->Draw();
  string fileNameF = "fitCanvasFailMuTnP_"+tnp_+"_"+category_;
  if(doMC_) fileNameF = fileNameF+"MC";
  else fileNameF = fileNameF+"DATA";
  cFail->SaveAs(Form("./MuTnP/%s_%s.png",fileNameF.c_str(), binLabel_.c_str()));

  ResDataCombinedFit->printArgs(std::cout);
  cout << endl;
  ResDataCombinedFit->printValue(std::cout);
  cout << endl;

  float DataErrorLo = DataEffFit->getErrorLo()<0 ? DataEffFit->getErrorLo() : (-1)*DataEffFit->getError();
  float DataErrorHi = DataEffFit->getErrorHi()>0 ? DataEffFit->getErrorHi() : DataEffFit->getError();

  cout << DataEffFit->getVal() << " +/- " << DataEffFit->getError() << "  ( " << DataErrorLo << ", " << DataErrorHi << ")" <<  endl;

  float* out1 = new float[6];
  float* out2 = new float[6];

  out1[0]=(binCenter_);
  out1[1]=(binWidth_);
  out1[2]=(binWidth_);
  out1[3]=(McTruthEff);
  out1[4]=(BinomialError);
  out1[5]=(BinomialError);

  out2[0]=(binCenter_);
  out2[1]=(binWidth_);
  out2[2]=(binWidth_);
  out2[3]=(DataEffFit->getVal());
  out2[4]=((-1)*DataErrorLo);
  out2[5]=(DataErrorHi);

  out.push_back(out1);
  out.push_back(out2);

  return out;

  dummyData->Close();
  dummyBkg->Close();
  delete dummyData; delete dummyBkg;
}

void plot(
	  const string tnp_      = "muTnP",
	  const string category_ = "muID",
	  const string ptRange_  = "pt>15 && pt<20",
	  const string binLabel_ = "15to20",
	  float ymax             = 1.0,
	  double cutValue_       = 0.5,
	  const float xLow_      = 70,
	  const float xHigh_     = 120,
	  const float nBins_     = 24,
	  bool doBinned_         = true,
	  float deltaAlpha_      = 0.0,
	  float deltaN_          = 0.0,
	  float scale_           = 0.0,
	  float mcLumi_          = 250,
	  bool doMC_             = true
	  )
{
 
  string outName = "./MuTnP/muTnP_"+category_+"_"+binLabel_;
  if(doMC_) outName = outName+"MC";
  else  outName = outName+"DATA";
  outName = outName+".txt";
  std::ofstream out(outName.c_str());
  out.precision(4);

  vector<float*> bin1 = simFit(tnp_,category_,binLabel_,cutValue_,"(abseta<1.5 && "+ptRange_+")", 0.75,0.75,xLow_,xHigh_,nBins_,doBinned_,deltaAlpha_,deltaN_,scale_,mcLumi_,doMC_);
  vector<float*> bin2 = simFit(tnp_,category_,binLabel_,cutValue_,"(abseta>1.5 && "+ptRange_+")", 1.80,0.3, xLow_,xHigh_,nBins_,doBinned_,deltaAlpha_,deltaN_,scale_,mcLumi_,doMC_);

  float truthMC_x[2]   = {(bin1[0])[0], (bin2[0])[0]};
  float truthMC_xL[2]  = {(bin1[0])[1], (bin2[0])[1]};
  float truthMC_xH[2]  = {(bin1[0])[2], (bin2[0])[2]};
  float truthMC_y[2]   = {(bin1[0])[3], (bin2[0])[3]};
  float truthMC_yL[2]  = {(bin1[0])[4], (bin2[0])[4]};
  float truthMC_yH[2]  = {(bin1[0])[5], (bin2[0])[5]};

  float tnpData_x[2]   = {(bin1[1])[0], (bin2[1])[0]};
  float tnpData_xL[2]  = {(bin1[1])[1], (bin2[1])[1]};
  float tnpData_xH[2]  = {(bin1[1])[2], (bin2[1])[2]};
  float tnpData_y[2]   = {(bin1[1])[3], (bin2[1])[3]};
  float tnpData_yL[2]  = {(bin1[1])[4], (bin2[1])[4]};
  float tnpData_yH[2]  = {(bin1[1])[5], (bin2[1])[5]};

  out<<"%Tag&probe measurement :" << category_ << ", bin: " << ptRange_ << endl;
  out<<"\\begin{tabular}[!htbp]{|c|c|c|}" << endl;
  out << "\\hline" << endl;
  out << "Bin & MC & Data \\\\" << endl;
  out << "\\hline" << endl;
  out << "|\\eta|<1.5 & " << truthMC_y[0] << " \\pm " << truthMC_yL[0] << " & " << tnpData_y[0] << " \\pm_{" << tnpData_yL[0] << "}^{" 
      << tnpData_yH[0] << "} \\\\" << endl;
  out << "\\hline" << endl;
  out << "|\\eta|>1.5 & " << truthMC_y[1] << " \\pm " << truthMC_yL[1] << " & " << tnpData_y[1] << " \\pm_{" << tnpData_yL[1] << "}^{" 
      << tnpData_yH[1] << "} \\\\" << endl;
  out << "\\hline" << endl;
  out<<"\\end{tabular}"<<endl;

  TCanvas *c2 = new TCanvas("results","canvas",10,30,650,600);
  c2->SetGrid(0,0);
  c2->SetFillStyle(4000);
  c2->SetFillColor(10);
  c2->SetTicky();
  c2->SetObjectStat(0);

  TLegend* leg = new TLegend(0.18,0.20,0.45,0.45,NULL,"brNDC");
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(0);

  string theSample = doMC_ ? "Simulation" : "Data" ;
  float theLumi = doMC_ ? mcLumi_ : 195.;

    TH1F* hMaster = new TH1F("hMaster",Form("CMS Preliminary 2011  #sqrt{s}=7 TeV %s  L=%.0f pb^{-1}; |#eta^{probe}|; efficiency",theSample.c_str(),theLumi),1,0,2.1);

  hMaster->SetAxisRange(0,ymax,"Y");
  hMaster->SetXTitle( "|#eta^{probe}|" );
  string YTitle="efficiency";
  hMaster->SetYTitle( YTitle.c_str() );
  hMaster->GetYaxis()->SetTitleOffset(1.4);

  TGraphAsymmErrors* graph_truthMC = new TGraphAsymmErrors(2,truthMC_x,truthMC_y, truthMC_xL,truthMC_xH,truthMC_yL,truthMC_yH);
  graph_truthMC->SetMarkerColor(kBlue);
  graph_truthMC->SetMarkerStyle(kOpenCircle);
  graph_truthMC->SetMarkerSize(1.5);

  TGraphAsymmErrors* graph_tnpData = new TGraphAsymmErrors(2,tnpData_x, tnpData_y, tnpData_xL,tnpData_xH,tnpData_yL,tnpData_yH);
  graph_tnpData->SetMarkerColor(kBlack);
  graph_tnpData->SetMarkerStyle(kFullCircle);
  graph_tnpData->SetMarkerSize(1.5);

  string header = "";
  if(category_.find("ID")!=string::npos)  header = "Eff. of Id for "+ptRange_ ;
  if(category_.find("Iso")!=string::npos) header = "Eff. of Iso(#Delta#beta) for "+ptRange_ ;
  if(category_.find("HLT")!=string::npos) header = "Eff. of HLT for "+ptRange_ ;
  leg->SetHeader(header.c_str());
  leg->AddEntry(graph_truthMC,"MC-truth","P");
  leg->AddEntry(graph_tnpData,"DATA","P");

  c2->cd();
  hMaster->Draw();
  graph_truthMC->Draw("PSAME");
  graph_tnpData->Draw("PSAME");

  leg->Draw();

  string effPlotName = "./MuTnP/efficiencyMuTnP"+category_+"_"+binLabel_;
  if(doMC_) effPlotName = effPlotName+"MC";
  else  effPlotName = effPlotName+"DATA";
  effPlotName= effPlotName+".png";
  gPad->SaveAs( effPlotName.c_str() );
  
}



void plotVsPt(
	      const string tnp_      = "muTnP",
	      const string category_ = "muHLT15",
	      const string ptRange_  = "abseta<2.1 && muID && muIso",
	      const string binLabel_  = "HLT15",
	      float ymax             = 1.05,
	      float ymin             = 0.5,
	      double cutValue_       = 0.5,
	      const float xLow_      = 70,
	      const float xHigh_     = 120,
	      const float nBins_     = 24,
	      bool doBinned_         = false,
	      float deltaAlpha_      = 0.0,
	      float deltaN_          = 0.0,
	      float scale_           = 0.0,
	      float mcLumi_          = 195,
	      bool doMC_             = false
	      )
{
 

  TFile *f = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamSummer11_iter1/nTupleDYJets-Mu-50-madgraph-PUS4_run_Open_MuTauStream.root","READ");
  TTree* tree = (TTree*)f->Get("outTreePtOrd");
  TH1F* h1 = new TH1F("h1","; p_{T}",100,15,115);
  tree->Draw("ptL1>>h1","puWeight*(genDecay==23*15 && HLTmu && combIsoLeg2<2 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20)");
  vector<float> binFracs; 
  binFracs.push_back(h1->Integral(1,5)/h1->Integral()  );
  binFracs.push_back(h1->Integral(6,10)/h1->Integral() );
  binFracs.push_back(h1->Integral(10,20)/h1->Integral());
  binFracs.push_back(h1->Integral(20,35)/h1->Integral());

  cout << "[15,20]" << h1->Integral(1,5)/h1->Integral()  << endl;
  cout << "[20,25]" << h1->Integral(6,10)/h1->Integral() << endl;
  cout << "[25,35]" << h1->Integral(10,20)/h1->Integral() << endl;
  cout << "[35,50]" << h1->Integral(20,35)/h1->Integral() << endl;

  string outName = "./MuTnP/muTnP_"+category_+"_"+binLabel_;
  if(doMC_) outName = outName+"MC";
  else  outName = outName+"DATA";
  outName = outName+".txt";
  std::ofstream out(outName.c_str());
  out.precision(4);

  vector<float*> bin1 = simFit(tnp_,category_,binLabel_+"15to20",cutValue_,"(pt>=15 && pt<20 && "+ptRange_+")", (15+20)/2., (20-15)/2.,xLow_,xHigh_,nBins_,doBinned_,deltaAlpha_,deltaN_,scale_,mcLumi_,doMC_);
  vector<float*> bin2 = simFit(tnp_,category_,binLabel_+"20to25",cutValue_,"(pt>=20 && pt<25 && "+ptRange_+")", (25+20)/2. ,(25-20)/2., xLow_,xHigh_,nBins_,doBinned_,deltaAlpha_,deltaN_,scale_,mcLumi_,doMC_);
  vector<float*> bin3 = simFit(tnp_,category_,binLabel_+"25to35",cutValue_,"(pt>=25 && pt<35 && "+ptRange_+")", (35+25)/2., (35-25)/2., xLow_,xHigh_,nBins_,doBinned_,deltaAlpha_,deltaN_,scale_,mcLumi_,doMC_);
  vector<float*> bin4 = simFit(tnp_,category_,binLabel_+"35to50",cutValue_,"(pt>=35 && pt<50 && "+ptRange_+")", (50+35)/2., (50-35)/2., xLow_,xHigh_,nBins_,true,deltaAlpha_,deltaN_,scale_,mcLumi_,doMC_);

  float truthMC_x[4]   = {(bin1[0])[0], (bin2[0])[0],(bin3[0])[0], (bin4[0])[0]};
  float truthMC_xL[4]  = {(bin1[0])[1], (bin2[0])[1],(bin3[0])[1], (bin4[0])[1]};
  float truthMC_xH[4]  = {(bin1[0])[2], (bin2[0])[2],(bin3[0])[2], (bin4[0])[2]};
  float truthMC_y[4]   = {(bin1[0])[3], (bin2[0])[3],(bin3[0])[3], (bin4[0])[3]};
  float truthMC_yL[4]  = {(bin1[0])[4], (bin2[0])[4],(bin3[0])[4], (bin4[0])[4]};
  float truthMC_yH[4]  = {(bin1[0])[5], (bin2[0])[5],(bin3[0])[5], (bin4[0])[5]};

  float tnpData_x[4]   = {(bin1[1])[0], (bin2[1])[0],(bin3[1])[0], (bin4[1])[0]};
  float tnpData_xL[4]  = {(bin1[1])[1], (bin2[1])[1],(bin3[1])[1], (bin4[1])[1]};
  float tnpData_xH[4]  = {(bin1[1])[2], (bin2[1])[2],(bin3[1])[2], (bin4[1])[2]};
  float tnpData_y[4]   = {(bin1[1])[3], (bin2[1])[3],(bin3[1])[3], (bin4[1])[3]};
  float tnpData_yL[4]  = {(bin1[1])[4], (bin2[1])[4],(bin3[1])[4], (bin4[1])[4]};
  float tnpData_yH[4]  = {(bin1[1])[5], (bin2[1])[5],(bin3[1])[5], (bin4[1])[5]};

  float averageSF   = 0.;
  float averageSFE2 = 0.;
  float sumWeight   = 0.;
  for(unsigned int k = 0; k<4; k++){
    averageSF += binFracs[k]*(tnpData_y[k]/truthMC_y[k]);
    sumWeight += binFracs[k];
    averageSFE2 += binFracs[k]*binFracs[k]*pow((tnpData_yH[k]/tnpData_y[k]+truthMC_yL[k]/truthMC_y[k])*(tnpData_y[k]/truthMC_y[k]), 2 );
  }
  averageSF/=sumWeight;
  averageSFE2/=(sumWeight*sumWeight);

  out<<"%Tag&probe measurement :" << category_ << ", bin: " << ptRange_ << endl;
  out <<"% average SF: " << averageSF << "$\\pm$" << sqrt(averageSFE2) << endl;
  out<<"\\begin{tabular}[!htbp]{|c|c|c|c|}" << endl;
  out << "\\hline" << endl;
  out << "Bin & MC & Data & SF \\\\" << endl;
  out << "\\hline" << endl;
  out << "[15,20] & " << truthMC_y[0] << " $\\pm$ " << truthMC_yL[0] << " & " << tnpData_y[0] << " $\\pm_{" << tnpData_yL[0] << "}^{" 
      << tnpData_yH[0] << "}$ & " << tnpData_y[0]/truthMC_y[0] << " $\\pm$" << (tnpData_yH[0]/tnpData_y[0]+truthMC_yL[0]/truthMC_y[0])*(tnpData_y[0]/truthMC_y[0])  << " \\\\" << endl;
  out << "\\hline" << endl;
  out << "[20,25] & " << truthMC_y[1] << " $\\pm$ " << truthMC_yL[1] << " & " << tnpData_y[1] << " $\\pm_{" << tnpData_yL[1] << "}^{" 
      << tnpData_yH[1] << "}$ & " << tnpData_y[1]/truthMC_y[1] << " $\\pm$" << (tnpData_yH[1]/tnpData_y[1]+truthMC_yL[1]/truthMC_y[1])*(tnpData_y[1]/truthMC_y[1])  << " \\\\" << endl;
  out << "\\hline" << endl;
  out << "[25,35] & " << truthMC_y[2] << " $\\pm$ " << truthMC_yL[2] << " & " << tnpData_y[2] << " $\\pm_{" << tnpData_yL[2] << "}^{" 
      << tnpData_yH[2] << "}$ & " << tnpData_y[2]/truthMC_y[2] << " $\\pm$" << (tnpData_yH[2]/tnpData_y[2]+truthMC_yL[2]/truthMC_y[2])*(tnpData_y[2]/truthMC_y[2])  << " \\\\" << endl;
  out << "\\hline" << endl;
  out << "[35,50] & " << truthMC_y[3] << " $\\pm$ " << truthMC_yL[3] << " & " << tnpData_y[3] << " $\\pm_{" << tnpData_yL[3] << "}^{" 
      << tnpData_yH[3]  << "}$ & " << tnpData_y[3]/truthMC_y[3] << " $\\pm$" << (tnpData_yH[3]/tnpData_y[3]+truthMC_yL[3]/truthMC_y[3])*(tnpData_y[3]/truthMC_y[3])  << " \\\\" << endl;
  out << "\\hline" << endl;
  out<<"\\end{tabular}"<<endl;

  TCanvas *c2 = new TCanvas("results","canvas",10,30,650,600);
  c2->SetGrid(0,0);
  c2->SetFillStyle(4000);
  c2->SetFillColor(10);
  c2->SetTicky();
  c2->SetObjectStat(0);

  TLegend* leg = new TLegend(0.40,0.20,0.65,0.45,NULL,"brNDC");
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(0);

  string theSample = doMC_ ? "Simulation" : "Data" ;
  float theLumi = doMC_ ? mcLumi_ : 195.;

  TH1F* hMaster = new TH1F("hMaster",Form("CMS Preliminary 2011  #sqrt{s}=7 TeV %s  L=%.0f pb^{-1}; p_{T}^{probe} (GeV); efficiency",theSample.c_str(),theLumi),1,15,50);

  hMaster->SetAxisRange(ymin,ymax,"Y");
  //hMaster->SetXTitle( "|#eta^{probe}|" );
  string YTitle="efficiency";
  hMaster->SetYTitle( YTitle.c_str() );
  hMaster->GetYaxis()->SetTitleOffset(1.4);

  TGraphAsymmErrors* graph_truthMC = new TGraphAsymmErrors(4,truthMC_x,truthMC_y, truthMC_xL,truthMC_xH,truthMC_yL,truthMC_yH);
  graph_truthMC->SetMarkerColor(kBlue);
  graph_truthMC->SetMarkerStyle(kOpenCircle);
  graph_truthMC->SetMarkerSize(1.5);

  TGraphAsymmErrors* graph_tnpData = new TGraphAsymmErrors(4,tnpData_x, tnpData_y, tnpData_xL,tnpData_xH,tnpData_yL,tnpData_yH);
  graph_tnpData->SetMarkerColor(kBlack);
  graph_tnpData->SetMarkerStyle(kFullCircle);
  graph_tnpData->SetMarkerSize(1.5);

  string header = "";
  if(category_.find("ID")!=string::npos)  header = "Eff. of Id for "+ptRange_ ;
  if(category_.find("Iso")!=string::npos) header = "Eff. of Iso(#Delta#beta) for "+ptRange_ ;
  if(category_.find("HLT")!=string::npos) header = "Eff. of HLT for "+ptRange_ ;
  leg->SetHeader(header.c_str());
  leg->AddEntry(graph_truthMC,"MC-truth","P");
  leg->AddEntry(graph_tnpData,"DATA","P");

  c2->cd();
  hMaster->Draw();
  graph_truthMC->Draw("PSAME");
  graph_tnpData->Draw("PSAME");

  leg->Draw();

  string effPlotName = "./MuTnP/efficiencyMuTnP"+category_+"_"+binLabel_;
  if(doMC_) effPlotName = effPlotName+"MC";
  else  effPlotName = effPlotName+"DATA";
  effPlotName= effPlotName+".png";
  gPad->SaveAs( effPlotName.c_str() );
  
}
