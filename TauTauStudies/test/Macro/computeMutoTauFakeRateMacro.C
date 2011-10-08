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
			 const string tnp_      = "etoTauVBTFIDLoose",
			 const string category_ = "tauAntiMuLoose",
			 double cutValue_       = 0.5,
			 const string bin_      = "abseta<1.5",
			 const float binCenter_ = 0.75,
			 const float binWidth_  = 0.75,
			 const float xLow_      = 70,
			 const float xHigh_     = 120,
			 const float nBins_     = 24,
			 bool doBinned_         = true,
			 float deltaAlpha_      = 0.0,
			 float deltaN_          = 0.0,
			 float scale_           = 0.0
			 )

{


  vector<float*> out;

  TFile fSgn("../prod/TauFakeRate/treeMuToTauTnP_DYToMuMu-20-PUS1.root");  
  TTree *fullTreeSgn  = (TTree*)fSgn.Get((tnp_+"/fitter_tree").c_str());
  fSgn.cd("allEventsFilter");
  TH1F* totalEventsSgn = (TH1F*)gDirectory->Get("totalEvents");
  float readEventsSgn = totalEventsSgn->GetBinContent(1);

  // data
  TFile fdat("../prod/TauFakeRate/treeMutoTauTnP_Run2011-Mu-FakeRate-v2.root");
  TTree *fullTreeData = (TTree*)fdat.Get((tnp_+"/fitter_tree").c_str());

  TH1F* hS           = new TH1F("hS","",1,0,150);
  TH1F* hSP          = new TH1F("hSP","",1,0,150);

  fullTreeSgn->Draw("mass>>hS",Form("tag_puMCWeight*(%s && mass>%f && mass<%f && mcTrue  && tag_pfRelIso<0.1 && pair_tnpCharge==0 && event_met_pfmet<25)",bin_.c_str(),xLow_,xHigh_));
  float SGNtrue = hS->Integral();
  fullTreeSgn->Draw("mass>>hSP",Form("tag_puMCWeight*(%s && %s>=%f && mass>%f && mass<%f && mcTrue  && tag_pfRelIso<0.1 && pair_tnpCharge==0 && event_met_pfmet<25)",bin_.c_str(),category_.c_str(),cutValue_,xLow_,xHigh_));
  float SGNtruePass = hSP->Integral();

  float McTruthEff    = SGNtruePass/SGNtrue;
  float BinomialError = TMath::Sqrt(SGNtruePass/SGNtrue*(1-SGNtruePass/SGNtrue)/SGNtrue);
  
  cout << bin_.c_str() << " ==> MCTRUTH: " << McTruthEff << " +/- " << BinomialError << endl;

  delete hS; delete hSP;

  // file to copy the trees
  TFile *templFile = new TFile(Form("dummyTempl_bin%.2f.root",binCenter_),"RECREATE");
  
  TTree* fullTreeSgnCutP = fullTreeSgn->CopyTree( Form("(mcTrue && tag_pfRelIso<0.1 && %s>=%f && %s && pair_tnpCharge==0 && event_met_pfmet<25)",category_.c_str(),cutValue_,bin_.c_str()) );
  TTree* fullTreeSgnCutF = fullTreeSgn->CopyTree( Form("(mcTrue && tag_pfRelIso<0.1 && %s< %f && %s && pair_tnpCharge==0 && event_met_pfmet<25)",category_.c_str(),cutValue_,bin_.c_str()) );
  
  RooRealVar mass("mass","m_{tp} (GeV/c^{2})",xLow_,xHigh_);
  mass.setBins( 10000, "fft" );
  mass.setBins( nBins_ );


  // Landau 
  TH1F* hmassBkg = new TH1F("hmassBkg","",15,xLow_,xHigh_);
  fullTreeData->Draw("mass>>hmassBkg", Form("(%s>=%f && tag_pfRelIso>0.2 && %s && pair_tnpCharge==0 && event_met_pfmet<25 &&  (tag_hlt1==1 || tag_hlt2==1))",category_.c_str(),cutValue_,bin_.c_str()) );
  RooRealVar meanBkg("meanBkg","",59,30,100);
  RooRealVar sigmaBkg("sigmaBkg","",11,0,50);
  //RooLandau bkgPdfP("bkgPdfP","",mass,meanBkg,sigmaBkg);

  RooRealVar DataCP("DataCP","",0,-10,10);
  RooExponential bkgPdfP("bkgPdfP","",mass,DataCP);

  RooRealVar a0("a0","",100,0,1000);
  RooRealVar a1("a1","",0,-100,100);
  RooRealVar a2("a2","",0,-100,100);
  RooRealVar a3("a3","",0,-100,100);
  //RooPolynomial bkgPdfP("bkgPdfP","",mass,RooArgList(a0,a1,a2,a3));

  RooDataHist bkgDataHistP("bkgDataHistP","",RooArgSet(mass),Import(*hmassBkg));
  RooDataSet bkgDataSetP("bkgDataSetP","",RooArgSet(mass),Import(*((TTree*)fullTreeData->CopyTree(Form("(%s>=%f && tag_pfRelIso>0.2 && %s && pair_tnpCharge!=0 && event_met_pfmet<25 && (tag_hlt1==1 || tag_hlt2==1))",category_.c_str(),cutValue_,bin_.c_str()) ) )) );

  //RooHistPdf  bkgPdfP("bkgPdfP","",RooArgSet(mass),bkgDataHistP);
  //RooKeysPdf bkgPdfP("bkgPdfP","",mass,bkgDataSetP);

  TCanvas *c0 = new TCanvas("fitCanvas","canvas",10,30,650,600);
  c0->SetGrid(0,0);
  c0->SetFillStyle(4000);
  c0->SetFillColor(10);
  c0->SetTicky();
  c0->SetObjectStat(0);

  RooPlot* frame = mass.frame(Bins(40),Title("CMS Preliminary 2011  #sqrt{s}=7 TeV  Simulation"));
  bkgDataHistP.plotOn(frame);
  bkgPdfP.plotOn(frame);
  c0->cd();
  frame->Draw();
  //c0->SaveAs(("template_"+tnp_+"_"+category_+".png").c_str());
  //return;

  mass.setBins( 50 );

  // failing:
  RooDataSet sgnDataSetF("sgnDataSetF","dataset for signal fail", RooArgSet(mass), Import( *fullTreeSgnCutF ) );
  TH1F* hmassSgnF = new TH1F("hmassSgnF","",50,xLow_,xHigh_);
  fullTreeSgnCutF->Draw("mass>>hmassSgnF", "tag_puMCWeight");
  RooDataHist sgnDataHistF("sgnDataHistF","",RooArgSet(mass),Import(*hmassSgnF));
  RooHistPdf  sgnPdfF_raw("sgnPdfF_raw","",RooArgSet(mass),sgnDataHistF);

  RooRealVar sgnMeanResF("sgnMeanResF","",0,-10,10);
  RooRealVar sgnSigmaResF("sgnSigmaResF","",0.5,0,10);
  RooGaussian resolModF("sgnResolModF","",mass,sgnMeanResF,sgnSigmaResF);
  RooFFTConvPdf sgnPdfF("sgnPdfF","",mass,sgnPdfF_raw,resolModF);

  // passing:
  RooDataSet sgnDataSetP("sgnDataSetP","dataset for signal", RooArgSet(mass), Import( *fullTreeSgnCutP ) );
  TH1F* hmassSgnP = new TH1F("hmassSgnP","",50,xLow_,xHigh_);
  fullTreeSgnCutP->Draw("mass>>hmassSgnP", "tag_puMCWeight");
  mass.setBins( 120 );
  RooDataHist sgnDataHistP("sgnDataHistP","",RooArgSet(mass),sgnDataSetP, 1.0);
  //RooHistPdf  sgnTemplatePdfP("sgnTemplatePdfP","",RooArgSet(mass),sgnDataHistP);
  RooKeysPdf sgnTemplatePdfP("sgnTemplatePdfP","",mass,sgnDataSetP);

  // Breit-Wigner
  RooRealVar meanSgn("meanSgn","mean",91.19,85,100);
  RooRealVar widthSgn("widthSgn","width",2.49,0.,10);
  RooBreitWigner bwSgn("bwSgn","bw",mass,meanSgn,widthSgn);

  // Crystall Ball
  RooRealVar m1Sgn("m1Sgn","m1",0,-20,20);
  RooRealVar sigmaSgn("sigmaSgn","sigma",0.5,0,20);
  RooRealVar alfaSgn("alfaSgn","alfa", 0.5,-10,10);
  RooRealVar nSgn("nSgn","n", 1,1e-06,50);
  RooCBShape cbSgn("cbSgn","",mass,m1Sgn,sigmaSgn,alfaSgn,nSgn);

  mass.setBins( 1000 );
  // BW (X) CB
  RooFFTConvPdf bvcbSgn("bvcbSgn","",mass,bwSgn, cbSgn);

  RooFitResult* ResSgnFit = bvcbSgn.fitTo(sgnDataSetP, Minos(1), Save(1), NumCPU(4) );
  RooArgSet FitParamSgn(ResSgnFit->floatParsFinal());
  RooRealVar* m1SgnFit    = (RooRealVar*)(&FitParamSgn["m1Sgn"]);
  RooRealVar* sigmaSgnFit = (RooRealVar*)(&FitParamSgn["sigmaSgn"]);
  RooRealVar* alfaSgnFit  = (RooRealVar*)(&FitParamSgn["alfaSgn"]);
  RooRealVar* nSgnFit     = (RooRealVar*)(&FitParamSgn["nSgn"]);
  RooRealVar* nMeanSgnFit = (RooRealVar*)(&FitParamSgn["meanSgn"]);
  RooRealVar* nWidthSgnFit= (RooRealVar*)(&FitParamSgn["widthSgn"]);

  RooRealVar m1Sgn_C("m1Sgn_C","m1",m1SgnFit->getVal(),-10,10);
  RooRealVar sigmaSgn_C("sigmaSgn_C","sigma",sigmaSgnFit->getVal(),0,20);
  // choose to let it float or not
  
  RooRealVar meanSgn_C("meanSgn_C","mean",  nMeanSgnFit->getVal() ,80,120);
  RooRealVar widthSgn_C("widthSgn_C","width",nWidthSgnFit->getVal() /*,0.,10*/);

  RooRealVar alfaSgn_C("alfaSgn_C","alfa",alfaSgnFit->getVal()*(1+deltaAlpha_)/*,0,20*/);
  RooRealVar nSgn_C("nSgn_C","n",nSgnFit->getVal()*(1+deltaN_)/*,0,50*/);

  RooCBShape cbSgn_C("cbSgn_C","",mass,m1Sgn_C,sigmaSgn_C,alfaSgn_C,nSgn_C);
  
  RooLognormal alfaSgn_CPdf("alfaSgn_CPdf","",alfaSgn_C,RooConst(alfaSgnFit->getVal()),RooConst(1.5));
  RooLognormal nSgn_CPdf("nSgn_CPdf","",nSgn_C,RooConst(nSgnFit->getVal()),            RooConst(1.5));

  RooLognormal meanSgn_CPdf("meanSgn_CPdf","",meanSgn_C,RooConst(nMeanSgnFit->getVal()),RooConst(1.5));
  RooLognormal widthSgn_CPdf("widthSgn_CPdf","",widthSgn_C,RooConst(nWidthSgnFit->getVal()),RooConst(1.5));

  // fitted BW (X) CB
  RooBreitWigner bwSgn_C("bwSgn_C","bw",mass,meanSgn_C,widthSgn_C);
  //RooFFTConvPdf sgnPdfP("sgnPdfP","",mass,bwSgn_C, cbSgn_C);

  RooRealVar sgnMeanResP("sgnMeanResP","",0,-10,10);
  RooRealVar sgnSigmaResP("sgnSigmaResP","",0.5,0,10);
  RooGaussian resolModP("sgnResolModP","",mass,sgnMeanResP,sgnSigmaResP);

  mass.setBins( 10000, "fft" );
  RooFFTConvPdf sgnPdfP("sgnPdfP","",mass,sgnTemplatePdfP,resolModP);

  //sgnPdfP.fitTo(sgnDataSetP,Minos(1), Save(1), NumCPU(4));

  //RooPlot* frame = mass.frame(Bins(nBins_),Title("template"));
  //sgnDataSetP.plotOn(frame);
  //sgnTemplatePdfP.plotOn(frame, LineColor(kRed));
  //sgnPdfP.plotOn(frame);
  //frame->Draw();



  mass.setBins(nBins_);
  //return;

  // Fit
  RooCategory category("category","category") ;
  category.defineType("pass") ;
  category.defineType("fail") ;

  RooRealVar DataCF("DataCF","",0);
  RooExponential bkgPdfF("bkgPdfF","",mass,DataCF);

  RooRealVar DataNumBkgF("DataNumBkgF","",0);
  RooRealVar DataNumBkgP("DataNumBkgP","",0,1000000);
  RooRealVar DataNumSgn("DataNumSgn","",  0,1000000);
  RooRealVar DataEfficiency("DataEfficiency","",0.01,0,1);
  
  RooFormulaVar DataNumSgnP("DataNumSgnP","DataEfficiency*DataNumSgn",    RooArgSet(DataEfficiency,DataNumSgn));
  RooFormulaVar DataNumSgnF("DataNumSgnF","(1-DataEfficiency)*DataNumSgn",RooArgSet(DataEfficiency,DataNumSgn));
 
  RooAddPdf DataModelP("DataModelP","",RooArgList(/*sgnPdfP*/sgnTemplatePdfP,bkgPdfP),RooArgList(DataNumSgnP,DataNumBkgP));
  RooAddPdf DataModelF("DataModelF","",RooArgList(sgnPdfF,bkgPdfF),RooArgList(DataNumSgnF,DataNumBkgF));
  
  TFile* dummyData = new TFile("dummyData.root","RECREATE");
  TTree* fullTreeDataCutP = fullTreeData->CopyTree( Form("(%s>=%f && %s  && tag_pfRelIso<0.1 && pair_tnpCharge==0 && event_met_pfmet<25 && (tag_hlt1==1 || tag_hlt2==1))",category_.c_str(),cutValue_,bin_.c_str()) ); 
  TTree* fullTreeDataCutF = fullTreeData->CopyTree( Form("(%s <%f && %s  && tag_pfRelIso<0.1 && pair_tnpCharge==0 && event_met_pfmet<25 && (tag_hlt1==1 || tag_hlt2==1))",category_.c_str(),cutValue_,bin_.c_str()) );


  mass.setBins(nBins_);
  RooDataSet DataDataSetP("DataDataSetP","dataset for Data pass", RooArgSet(mass), Import( *fullTreeDataCutP ) );
  std::cout << "data dataset Pass " << DataDataSetP.numEntries() << "  " << std::endl;
  //return out;
  RooDataHist DataDataHistP("DataDataHistP","",RooArgSet(mass),DataDataSetP, 1.0);
  RooDataSet DataDataSetF("DataDataSetF","dataset for Data fail", RooArgSet(mass), Import( *fullTreeDataCutF ) );
  std::cout << "data dataset Fail " << DataDataSetF.numEntries() << "  " << std::endl;
  RooDataHist DataDataHistF("DataDataHistF","",RooArgSet(mass),DataDataSetF, 1.0);

  RooRealVar DataNumSgnP_("DataNumSgnP_","",0,10000);
  RooAddPdf DataModelP_("DataModelP_","",RooArgList(/*sgnPdfP*/sgnTemplatePdfP,bkgPdfP),RooArgList(DataNumSgnP_,DataNumBkgP));
  DataModelP_.fitTo(DataDataSetP, Extended(1), Minos(1), Save(1), NumCPU(4),SumW2Error(1) /*,ExternalConstraints( RooArgSet(meanSgn_CPdf,widthSgn_CPdf) )*/);

  RooPlot* frame2 = mass.frame(Title("template"));
  DataDataSetP.plotOn(frame2);
  DataModelP_.plotOn(frame2, LineColor(kBlue), LineStyle(kSolid));
  DataModelP_.plotOn(frame2, Components("sgnPdfP"), LineColor(kRed), LineStyle(kSolid));
  DataModelP_.plotOn(frame2, Components("bkgPdfP"), LineColor(kGreen), LineStyle(kSolid));
  frame2->Draw();

  //return out;



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

  RooPlot* DataFrameP = mass.frame(Bins(40),Title("CMS Preliminary 2011  #sqrt{s}=7 TeV   L=186 pb^{-1}:  passing probe"));
  DataCombData.plotOn(DataFrameP,Cut("category==category::pass"));
  DataSimPdf.plotOn(DataFrameP,Slice(category,"pass"), ProjWData(category,DataCombData), LineColor(kBlue));
  DataSimPdf.plotOn(DataFrameP,Slice(category,"pass"), ProjWData(category,DataCombData), Components("sgnTemplatePdfP"/*"sgnPdfP"*/), LineColor(kRed), LineStyle(kSolid));
  DataSimPdf.plotOn(DataFrameP,Slice(category,"pass"), ProjWData(category,DataCombData), Components("bkgPdfP"), LineColor(kMagenta), LineStyle(kSolid));
 

  RooPlot* DataFrameF = mass.frame(Bins(40),Title("CMS Preliminary 2011  #sqrt{s}=7 TeV   L=186 pb^{-1}:  passing probe"));
  DataCombData.plotOn(DataFrameF,Cut("category==category::fail"));
  DataSimPdf.plotOn(DataFrameF,Slice(category,"fail"), ProjWData(category,DataCombData), LineColor(kBlue));
  DataSimPdf.plotOn(DataFrameF,Slice(category,"fail"), ProjWData(category,DataCombData), Components("sgnPdfF"), LineColor(kRed), LineStyle(kSolid));


  TCanvas *cPass = new TCanvas("fitCanvasP","canvas",10,30,650,600);
  cPass->SetGrid(0,0);
  cPass->SetFillStyle(4000);
  cPass->SetFillColor(10);
  cPass->SetTicky();
  cPass->SetObjectStat(0);

  cPass->cd();
  DataFrameP->Draw();
  string fileNameP = "fitCanvasPassMuToTau_"+tnp_+"_"+category_;
  cPass->SaveAs(Form("%s_%.2f.png",fileNameP.c_str(), binCenter_));

  TCanvas *cFail = new TCanvas("fitCanvasF","canvas",10,30,650,600);
  cFail->SetGrid(0,0);
  cFail->SetFillStyle(4000);
  cFail->SetFillColor(10);
  cFail->SetTicky();
  cFail->SetObjectStat(0);

  cFail->cd();
  DataFrameF->Draw();
  string fileNameF = "fitCanvasFailMuToTau_"+tnp_+"_"+category_;
  cFail->SaveAs(Form("%s_%.2f.png",fileNameF.c_str(), binCenter_));


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
}

void plot(
	  const string tnp_      = "etoTauVBTFIDLoose",
	  const string category_ = "tauAntiMuTight",
	  float ymax             = 0.02,
	  double cutValue_       = 0.5,
	  const string bin_      = "abseta<1.5",
	  const float binCenter_ = 0.75,
	  const float binWidth_  = 0.75,
	  const float xLow_      = 70,
	  const float xHigh_     = 120,
	  const float nBins_     = 24,
	  bool doBinned_         = true,
	  float deltaAlpha_      = 0.0,
	  float deltaN_          = 0.0,
	  float scale_           = 0.0
	  )
{
 
  std::ofstream out(("muToTaufakeRate_"+tnp_+"_"+category_+".txt").c_str());
  out.precision(4);

  vector<float*> bin1 = simFit(tnp_,category_,cutValue_,"abseta<1.5",0.75,0.75,xLow_,xHigh_,24,true,deltaAlpha_,deltaN_,scale_);
  vector<float*> bin2 = simFit(tnp_,category_,cutValue_,"abseta>1.5",1.90,0.4, xLow_,xHigh_,24,true,deltaAlpha_,deltaN_,scale_);

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

  out<<"%Fake rate with tag&probe " << tnp_ << ", WP " << category_ << endl;
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

  TLegend* leg = new TLegend(0.18,0.65,0.45,0.85,NULL,"brNDC");
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(0);

  TH1F* hMaster = new TH1F("hMaster","CMS Preliminary 2011  #sqrt{s}=7 TeV L=186 pb^{-1}; |#eta^{probe}|; fake rate from muons",1,0,2.3);

  hMaster->SetAxisRange(0,ymax,"Y");
  hMaster->SetXTitle( "|#eta^{probe}|" );
  string YTitle="fake rate from muons";
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
  if(category_.find("Loose")!=string::npos) header = "Against-muon loose" ;
  if(category_.find("Tight")!=string::npos) header = "Against-muon tight" ;
  leg->SetHeader(header.c_str());
  leg->AddEntry(graph_truthMC,"MC-truth","P");
  leg->AddEntry(graph_tnpData,"DATA","P");

  c2->cd();
  hMaster->Draw();
  graph_truthMC->Draw("PSAME");
  graph_tnpData->Draw("PSAME");

  leg->Draw();

  gPad->SaveAs( ("efficiencyMuToTau"+tnp_+"_"+category_+".png").c_str() );
  
}
