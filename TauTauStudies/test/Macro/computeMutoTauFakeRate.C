{

  using namespace std;
  using namespace RooFit;

  const string tnp_      = "etoTauVBTFIDLoose";
  const string category_ = "tauAntiMuLoose";
  double cutValue_       = 0.5;
  const string bin_      = "abseta<1.5";
  const float binCenter_ = 0.75;
  const float binWidth_  = 0.75;
  const float xLow_      = 70;
  const float xHigh_     = 120;
  const float nBins_     = 24;
  bool doBinned_         = true;
  float deltaAlpha_      = 0.0;
  float deltaN_          = 0.0;
  float scale_           = 0.0;
   

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

  RooPlot* frame = mass.frame(Bins(nBins_),Title("template"));
  bkgDataHistP.plotOn(frame);
  bkgPdfP.plotOn(frame);
  frame->Draw();

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

  sgnPdfP.fitTo(sgnDataSetP,Minos(1), Save(1), NumCPU(4));

  RooPlot* frame = mass.frame(Bins(nBins_),Title("template"));
  sgnDataSetP.plotOn(frame);
  sgnTemplatePdfP.plotOn(frame, LineColor(kRed));
  sgnPdfP.plotOn(frame);
  frame->Draw();

  return;

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
  RooRealVar DataNumSgn("DataNumSgn","",0,1000000);
  RooRealVar DataEfficiency("DataEfficiency","",0.01,0,1);
  
  RooFormulaVar DataNumSgnP("DataNumSgnP","DataEfficiency*DataNumSgn",    RooArgSet(DataEfficiency,DataNumSgn));
  RooFormulaVar DataNumSgnF("DataNumSgnF","(1-DataEfficiency)*DataNumSgn",RooArgSet(DataEfficiency,DataNumSgn));
 
  RooAddPdf DataModelP("DataModelP","",RooArgList(sgnPdfP,bkgPdfP),RooArgList(DataNumSgnP,DataNumBkgP));
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
  RooAddPdf DataModelP_("DataModelP_","",RooArgList(sgnPdfP,bkgPdfP),RooArgList(DataNumSgnP_,DataNumBkgP));
  DataModelP_.fitTo(DataDataSetP, Extended(1), Minos(1), Save(1), NumCPU(4),SumW2Error(1) /*,ExternalConstraints( RooArgSet(meanSgn_CPdf,widthSgn_CPdf) )*/);

  RooPlot* frame2 = mass.frame(Title("template"));
  DataDataSetP.plotOn(frame2);
  DataModelP_.plotOn(frame2, LineColor(kBlue), LineStyle(kSolid));
  DataModelP_.plotOn(frame2, Components("sgnPdfP"), LineColor(kRed), LineStyle(kSolid));
  DataModelP_.plotOn(frame2, Components("bkgPdfP"), LineColor(kGreen), LineStyle(kSolid));
  frame2->Draw();

  //gPad->SaveAs("fitCanvas.png");
  return;



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

  RooPlot* DataFrameP = mass.frame(Bins(nBins_),Title("Data: passing sample"));
  DataCombData.plotOn(DataFrameP,Cut("category==category::pass"));
  DataSimPdf.plotOn(DataFrameP,Slice(category,"pass"), ProjWData(category,DataCombData), LineColor(kBlue));
  DataSimPdf.plotOn(DataFrameP,Slice(category,"pass"), ProjWData(category,DataCombData), Components("sgnPdfP"), LineColor(kRed), LineStyle(kSolid));
  DataSimPdf.plotOn(DataFrameP,Slice(category,"pass"), ProjWData(category,DataCombData), Components("bkgPdfP"), LineColor(kMagenta), LineStyle(kSolid));
 

  RooPlot* DataFrameF = mass.frame(Bins(nBins_),Title("Data: failing sample"));
  DataCombData.plotOn(DataFrameF,Cut("category==category::fail"));
  DataSimPdf.plotOn(DataFrameF,Slice(category,"fail"), ProjWData(category,DataCombData), LineColor(kBlue));
  DataSimPdf.plotOn(DataFrameF,Slice(category,"fail"), ProjWData(category,DataCombData), Components("sgnPdfF"), LineColor(kRed), LineStyle(kSolid));


  TCanvas *c = new TCanvas("fitCanvas","canvas",10,30,650,600);
  c->SetGrid(0,0);
  c->SetFillStyle(4000);
  c->SetFillColor(10);
  c->SetTicky();
  c->SetObjectStat(0);

  c->Divide(2,1);
  c->cd(1);
  DataFrameP->Draw();
  c->cd(2);
  DataFrameF->Draw();

  c->Draw();
  c->SaveAs("fitCanvas.png");

  ResDataCombinedFit->printArgs(std::cout);
  cout << endl;
  ResDataCombinedFit->printValue(std::cout);
  cout << endl;

  float DataErrorLo = DataEffFit->getErrorLo()<0 ? DataEffFit->getErrorLo() : (-1)*DataEffFit->getError();
  float DataErrorHi = DataEffFit->getErrorHi()>0 ? DataEffFit->getErrorHi() : DataEffFit->getError();
 
  Double_t truthMC_x[1]   = {binCenter_};
  Double_t truthMC_xL[1]  = {binWidth_};
  Double_t truthMC_xH[1]  = {binWidth_};
  Double_t truthMC_y[1]   = {McTruthEff};
  Double_t truthMC_yL[1]  = {BinomialError};
  Double_t truthMC_yH[1]  = {BinomialError};

  Double_t tnpData_x[1]   = {binCenter_};
  Double_t tnpData_xL[1]  = {binWidth_};
  Double_t tnpData_xH[1]  = {binWidth_};
  Double_t tnpData_y[1]   = {DataEffFit->getVal()};
  Double_t tnpData_yL[1]  = {(-1)*DataErrorLo};
  Double_t tnpData_yH[1]  = {DataErrorHi};

  cout << DataEffFit->getVal() << " +/- " << DataEffFit->getError() << "  ( " << DataErrorLo << ", " << DataErrorHi << ")" <<  endl;

  TCanvas *c2 = new TCanvas("results","canvas",10,30,650,600);
  c2->SetGrid(0,0);
  c2->SetFillStyle(4000);
  c2->SetFillColor(10);
  c2->SetTicky();
  c2->SetObjectStat(0);

  TH1F* hMaster = new TH1F("hMaster"," ; |#eta|; fake rate from electrons",1,0,1.5);

  TGraphAsymmErrors* graph_truthMC = new TGraphAsymmErrors(1,truthMC_x,truthMC_y, truthMC_xL,truthMC_xH,truthMC_yL,truthMC_yH);
  graph_truthMC->SetMarkerColor(kBlue);
  graph_truthMC->SetMarkerStyle(kOpenCircle);
  graph_truthMC->SetMarkerSize(1.5);

  TGraphAsymmErrors* graph_tnpData = new TGraphAsymmErrors(1,tnpData_x, tnpData_y, tnpData_xL,tnpData_xH,tnpData_yL,tnpData_yH);
  graph_tnpData->SetMarkerColor(kBlack);
  graph_tnpData->SetMarkerStyle(kFullCircle);
  graph_tnpData->SetMarkerSize(1.5);

  c2->cd();
  hMaster->Draw();
  graph_truthMC->Draw("PSAME");
  graph_tnpData->Draw("PSAME");

  
}
