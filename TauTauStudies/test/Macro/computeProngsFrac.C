


{

  using namespace std;
  using namespace RooFit;

  string numPV = "numPV>7";

  TFile* fData     = new TFile("nTupleRun2011-Mu_All_Open_MuTauStream.root","READ");
  TFile* fDYTauTau = new TFile("nTupleDYJets-Mu-50-madgraph-PUS4_run_Open_MuTauStream.root");
  TFile* fWJets    = new TFile("nTupleWJets-Mu-madgraph-PUS4_run_Open_MuTauStream.root");
  TFile* fTTbar    = new TFile("nTupleTT-Mu-pythia-PUS3_run_Open_MuTauStream.root");

  TString tree = "outTreePtOrd";
  TTree *data                = (TTree*)fData->Get(tree);
  TTree *backgroundDYTauTau  = (TTree*)fDYTauTau->Get(tree);
  TTree *backgroundWJets     = (TTree*)fWJets->Get(tree);
  TTree *backgroundTTbar     = (TTree*)fTTbar->Get(tree);
 
  RooRealVar mass("mass","visible mass (GeV/c^{2})",40,80);
  mass.setBins( 20 );
  RooPlot* plot1 = mass.frame(Bins(10));
  RooPlot* plot2 = mass.frame(Bins(10));
  RooPlot* plot3 = mass.frame(Bins(10));
  
  //////////// dealing with 1prong0pi0

  // data
  TH1F* hData1prong0pi0 = new TH1F("hData1prong0pi0","",60,20,120);
  data->Draw("diTauVisMass>>hData1prong0pi0",Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==0 && %s",numPV.c_str()));
  RooDataHist DataHist1prong0pi0("DataHist1prong0pi0","",RooArgSet(mass),Import(*hData1prong0pi0));
  
  // Z->tautau
  TH1F* hDYTauTau1prong0pi0 = new TH1F("hDYTauTau1prong0pi0","",50,20,120);
  backgroundDYTauTau->Draw("diTauVisMass>>hDYTauTau1prong0pi0",Form("sampleWeight*puWeight*(abs(genDecay)==23*15 && tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==0 && %s)",numPV.c_str()));
  RooDataHist DYTauTauHist1prong0pi0("DYTauTauHist1prong0pi0","",RooArgSet(mass),Import(*hDYTauTau1prong0pi0));
  RooHistPdf  DYTauTau1prong0pi0Pdf("DYTauTau1prong0pi0Pdf","",RooArgSet(mass),DYTauTauHist1prong0pi0);

  ////DataHist1prong0pi0.plotOn(plot);
  //DYTauTauPdf.plotOn(plot);
  //plot->Draw();
  //return;

  // W+jets
  TH1F* hWJets1prong0pi0 = new TH1F("hWJets1prong0pi0","",40,20,120);
  backgroundWJets->Draw("diTauVisMass>>hWJets1prong0pi0",Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode!=-99 && %s",numPV.c_str()));
  TH1F* hWMt = new TH1F("hWMt","",200,-200,200);
  hWMt->Reset();
  backgroundWJets->Draw("(pZetaCorr-1.5*pZetaVisCorr)>>hWMt",Form("puWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==0 && %s)",numPV.c_str()));
  float scaleFactor1 = (hWMt->Integral(0,80))/(hWMt->Integral(90,200));
  cout << "Conv. factor for W ==> " << scaleFactor1 << endl;
  float OSWinSignalRegion1 = data->GetEntries(Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)<-40 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==0 && %s",numPV.c_str()));
  OSWinSignalRegion1 *= (1./scaleFactor1);
  RooDataHist WJetsHist1prong0pi0("WJetsHist1prong0pi0","",RooArgSet(mass),Import(*hWJets1prong0pi0));
  RooHistPdf  WJets1prong0pi0Pdf("WJets1prong0pi0Pdf","",RooArgSet(mass),WJetsHist1prong0pi0);
  
  //DataHist1prong0pi0.plotOn(plot);
  //DYTauTauPdf.plotOn(plot);
  //WJetsPdf.plotOn(plot);
  //plot->Draw();
  //return;

  // SS for QCD
  TH1F* hSS1prong0pi0Pdf = new TH1F("hSS1prong0pi0Pdf","",40,20,120);
  data->Draw("diTauVisMass>>hSS1prong0pi0Pdf",Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode!=-99 && %s",numPV.c_str()));
  //hSS1prong0pi0Pdf->DrawNormalized("HIST");
  TH1F* hWMt2_ = new TH1F("hWMt2_","",200,-200,200);
  hWMt2_->Reset();
  backgroundWJets->Draw("(pZetaCorr-1.5*pZetaVisCorr)>>hWMt2_",Form("puWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode!=-99 && %s)",numPV.c_str()));
  float scaleFactor1SS_ = (hWMt2_->Integral(0,80))/(hWMt2_->Integral(90,200));
  float SSWinSignalRegion1_ = data->GetEntries(Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)<-40 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode!=-99 && %s",numPV.c_str()));
  SSWinSignalRegion1_ *= (1./scaleFactor1SS_);
  TH1F* hW1_ = (TH1F*)hSS1prong0pi0Pdf->Clone("hW1_");
  hW1_->Reset();
  backgroundWJets->Draw("diTauVisMass>>hW1_",Form("puWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode!=-99 && %s)",numPV.c_str()));

  cout << hSS1prong0pi0Pdf->Integral() << endl;
  hW1_->Scale(SSWinSignalRegion1_/hW1_->Integral());
  cout << hW1_->Integral() << endl;
  //hSS1prong0pi0Pdf->Add(hW1_, -1 );

  //hSS1prong0pi0Pdf->DrawNormalized("HIST");
  //return;

  TH1F* hSS1prong0pi0 = new TH1F("hSS1prong0pi0","",40,20,120);
  data->Draw("diTauVisMass>>hSS1prong0pi0",Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==0 && %s",numPV.c_str()));

  TH1F* hWMt2 = new TH1F("hWMt2","",200,-200,200);
  hWMt2->Reset();
  backgroundWJets->Draw("(pZetaCorr-1.5*pZetaVisCorr)>>hWMt2",Form("puWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==0 && %s)",numPV.c_str()));
  float scaleFactor1SS = (hWMt2->Integral(0,80))/(hWMt2->Integral(90,200));
  cout << "Conv. factor for W SS ==> " << scaleFactor1SS << endl;
  float SSWinSignalRegion1 = data->GetEntries(Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)<-40 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==0 && %s",numPV.c_str()));
  SSWinSignalRegion1 *= (1./scaleFactor1SS);
  
  TH1F* hW1 = (TH1F*)hSS1prong0pi0->Clone("hW1");
  hW1->Reset();
  backgroundWJets->Draw("diTauVisMass>>hW1",Form("puWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==0 && %s)",numPV.c_str()));
  hW1->Scale(SSWinSignalRegion1/hW1->Integral());
  hSS1prong0pi0->Add(hW1, -1 );
  hSS1prong0pi0->Scale(1.06);
  RooDataHist SSHist1prong0pi0("SSHist1prong0pi0","",RooArgSet(mass),Import(*hSS1prong0pi0Pdf));
  RooHistPdf  SS1prong0pi0Pdf("SS1prong0pi0Pdf","",RooArgSet(mass),SSHist1prong0pi0);


  RooRealVar NDYTauTau1prong0pi0("NDYTauTau1prong0pi0","",0,10000);
  RooRealVar NWJets1prong0pi0("NWJets1prong0pi0","",0,10000);
  RooRealVar NSS1prong0pi0("NSS1prong0pi0","",0,10000);

  RooLognormal NWJets1prong0pi0_C("NWJets1prong0pi0_C","",NWJets1prong0pi0,RooConst(OSWinSignalRegion1),RooConst(1.5));
  RooLognormal NSS1prong0pi0_C("NSS1prong0pi0_C","",NSS1prong0pi0,RooConst(hSS1prong0pi0->Integral()),RooConst(1.5));

  RooAddPdf model1prong0pi0("model1prong0pi0","",RooArgList(DYTauTau1prong0pi0Pdf,WJets1prong0pi0Pdf,SS1prong0pi0Pdf),RooArgList(NDYTauTau1prong0pi0,NWJets1prong0pi0,NSS1prong0pi0));

  RooFitResult* res1prong0pi0 = model1prong0pi0.fitTo(DataHist1prong0pi0,Extended(1), Minos(1), Save(1), NumCPU(4), ExternalConstraints( RooArgSet(NWJets1prong0pi0_C ,NSS1prong0pi0_C ) )  ,SumW2Error(1) );

  DataHist1prong0pi0.plotOn(plot1);
  model1prong0pi0.plotOn(plot1,LineColor(kBlue));
  model1prong0pi0.plotOn(plot1,LineColor(kRed), Components("DYTauTau1prong0pi0Pdf"),LineStyle(kDashed));
  model1prong0pi0.plotOn(plot1,LineColor(kBlue), Components("WJets1prong0pi0Pdf"),LineStyle(kDashed));
  model1prong0pi0.plotOn(plot1,LineColor(kBlack), Components("SS1prong0pi0Pdf"),LineStyle(kDashed));
  plot1->Draw();

  cout << "In data ==> " << DataHist1prong0pi0.numEntries() << "; W => " << OSWinSignalRegion1 << " , SS\W => " << hSS1prong0pi0->Integral() << endl;

  //return;

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  
  //////////// dealing with 1prong1pi0

  // data
  TH1F* hData1prong1pi0 = new TH1F("hData1prong1pi0","",60,20,120);
  data->Draw("diTauVisMass>>hData1prong1pi0",Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==1 && %s",numPV.c_str()));
  RooDataHist DataHist1prong1pi0("DataHist1prong1pi0","",RooArgSet(mass),Import(*hData1prong1pi0));
  
  cout << hData1prong1pi0->GetEntries() << endl;

  // Z->tautau
  TH1F* hDYTauTau1prong1pi0 = new TH1F("hDYTauTau1prong1pi0","",50,20,120);
  backgroundDYTauTau->Draw("diTauVisMass>>hDYTauTau1prong1pi0",Form("sampleWeight*puWeight*(abs(genDecay)==23*15 && tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==1 && %s)",numPV.c_str()));
  RooDataHist DYTauTauHist1prong1pi0("DYTauTauHist1prong1pi0","",RooArgSet(mass),Import(*hDYTauTau1prong1pi0));
  RooHistPdf  DYTauTau1prong1pi0Pdf("DYTauTau1prong1pi0Pdf","",RooArgSet(mass),DYTauTauHist1prong1pi0);

  cout << hDYTauTau1prong1pi0->Integral() << endl;

  ////DataHist1prong1pi0.plotOn(plot);
  //DYTauTauPdf.plotOn(plot);
  //plot->Draw();
  //return;

  // W+jets
  TH1F* hWJets1prong1pi0 = new TH1F("hWJets1prong1pi0","",40,20,120);
  backgroundWJets->Draw("diTauVisMass>>hWJets1prong1pi0",Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode!=-99 && %s",numPV.c_str()));
  TH1F* hWMt3 = new TH1F("hWMt3","",200,-200,200);
  hWMt3->Reset();
  backgroundWJets->Draw("(pZetaCorr-1.5*pZetaVisCorr)>>hWMt3",Form("puWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==1 && %s)",numPV.c_str()));
  float scaleFactor1 = (hWMt3->Integral(0,80))/(hWMt3->Integral(90,200));
  cout << "Conv. factor for W ==> " << scaleFactor1 << endl;
  float OSWinSignalRegion1 = data->GetEntries(Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)<-40 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==1 && %s",numPV.c_str()));
  OSWinSignalRegion1 *= (1./scaleFactor1);
  RooDataHist WJetsHist1prong1pi0("WJetsHist1prong1pi0","",RooArgSet(mass),Import(*hWJets1prong1pi0));
  RooHistPdf  WJets1prong1pi0Pdf("WJets1prong1pi0Pdf","",RooArgSet(mass),WJetsHist1prong1pi0);
  
  //DataHist1prong1pi0.plotOn(plot);
  //DYTauTauPdf.plotOn(plot);
  //WJetsPdf.plotOn(plot);
  //plot->Draw();
  //return;

  // SS for QCD
  TH1F* hSS1prong1pi0Pdf = new TH1F("hSS1prong1pi0Pdf","",40,20,120);
  data->Draw("diTauVisMass>>hSS1prong1pi0Pdf",Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode!=-99 && %s",numPV.c_str()));
  TH1F* hWMt4_ = new TH1F("hWMt4_","",200,-200,200);
  hWMt4_->Reset();
  backgroundWJets->Draw("(pZetaCorr-1.5*pZetaVisCorr)>>hWMt4_",Form("puWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode!=-99 && %s)",numPV.c_str()));
  float scaleFactor1SS_ = (hWMt4_->Integral(0,80))/(hWMt4_->Integral(90,200));
  float SSWinSignalRegion1_ = data->GetEntries(Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)<-40 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode!=-99 && %s",numPV.c_str()));
  SSWinSignalRegion1_ *= (1./scaleFactor1SS_);
  TH1F* hW2_ = (TH1F*)hSS1prong1pi0Pdf->Clone("hW2_");
  hW2_->Reset();
  backgroundWJets->Draw("diTauVisMass>>hW2_",Form("puWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode!=-99 && %s)",numPV.c_str()));

  cout << hSS1prong1pi0Pdf->Integral() << endl;
  hW2_->Scale(SSWinSignalRegion1_/hW1_->Integral());
  cout << hW2_->Integral() << endl;
  //hSS1prong1pi0Pdf->Add(hW2_, -1 );

  hSS1prong1pi0Pdf->DrawNormalized("HIST");
  //return;

  TH1F* hSS1prong1pi0 = new TH1F("hSS1prong1pi0","",40,20,120);
  data->Draw("diTauVisMass>>hSS1prong1pi0",Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==1 && %s",numPV.c_str()));
  TH1F* hWMt4 = new TH1F("hWMt4","",200,-200,200);
  hWMt4->Reset();
  backgroundWJets->Draw("(pZetaCorr-1.5*pZetaVisCorr)>>hWMt4",Form("puWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==1 && %s)",numPV.c_str()));
  float scaleFactor1SS = (hWMt4->Integral(0,80))/(hWMt4->Integral(90,200));
  cout << "Conv. factor for W SS ==> " << scaleFactor1SS << endl;
  float SSWinSignalRegion1 = data->GetEntries(Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)<-40 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==1 && %s",numPV.c_str()));
  SSWinSignalRegion1 *= (1./scaleFactor1SS);
  
  TH1F* hW2 = (TH1F*)hSS1prong1pi0->Clone("hW2");
  hW2->Reset();
  backgroundWJets->Draw("diTauVisMass>>hW2",Form("puWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==1 && %s)",numPV.c_str()));
  hW2->Scale(SSWinSignalRegion1/hW2->Integral());
  hSS1prong1pi0->Add(hW2, -1 );
  hSS1prong1pi0->Scale(1.06);
  RooDataHist SSHist1prong1pi0("SSHist1prong1pi0","",RooArgSet(mass),Import(*hSS1prong1pi0Pdf));
  RooHistPdf  SS1prong1pi0Pdf("SS1prong1pi0Pdf","",RooArgSet(mass),SSHist1prong1pi0);

  RooRealVar NDYTauTau1prong1pi0("NDYTauTau1prong1pi0","",0,10000);
  RooRealVar NWJets1prong1pi0("NWJets1prong1pi0","",0,10000);
  RooRealVar NSS1prong1pi0("NSS1prong1pi0","",0,10000);

  RooLognormal NWJets1prong1pi0_C("NWJets1prong1pi0_C","",NWJets1prong1pi0,RooConst(OSWinSignalRegion1),RooConst(1.5));
  RooLognormal NSS1prong1pi0_C("NSS1prong1pi0_C","",NSS1prong1pi0,RooConst(hSS1prong1pi0->Integral()),RooConst(1.5));

  RooAddPdf model1prong1pi0("model1prong1pi0","",RooArgList(DYTauTau1prong1pi0Pdf,WJets1prong1pi0Pdf,SS1prong1pi0Pdf),RooArgList(NDYTauTau1prong1pi0,NWJets1prong1pi0,NSS1prong1pi0));

  RooFitResult* res1prong1pi0 = model1prong1pi0.fitTo(DataHist1prong1pi0,Extended(1), Minos(1), Save(1), NumCPU(4), ExternalConstraints( RooArgSet(NWJets1prong1pi0_C ,NSS1prong1pi0_C ) )  ,SumW2Error(1) );

  DataHist1prong1pi0.plotOn(plot2);
  model1prong1pi0.plotOn(plot2,LineColor(kBlue));
  model1prong1pi0.plotOn(plot2,LineColor(kRed), Components("DYTauTau1prong1pi0Pdf"),LineStyle(kDashed));
  model1prong1pi0.plotOn(plot2,LineColor(kBlue), Components("WJets1prong1pi0Pdf"),LineStyle(kDashed));
  model1prong1pi0.plotOn(plot2,LineColor(kBlack), Components("SS1prong1pi0Pdf"),LineStyle(kDashed));
  plot2->Draw();

  cout << "In data ==> " << DataHist1prong1pi0.numEntries() << "; W => " << OSWinSignalRegion1 << " , SS\W => " << hSS1prong1pi0->Integral() << endl;


  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  //////////// dealing with 3prong

  // data
  TH1F* hData3prong = new TH1F("hData3prong","",60,20,120);
  data->Draw("diTauVisMass>>hData3prong",Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==2 && %s",numPV.c_str()));
  RooDataHist DataHist3prong("DataHist3prong","",RooArgSet(mass),Import(*hData3prong));
  
  // Z->tautau
  TH1F* hDYTauTau3prong = new TH1F("hDYTauTau3prong","",50,20,120);
  backgroundDYTauTau->Draw("diTauVisMass>>hDYTauTau3prong",Form("sampleWeight*puWeight*(abs(genDecay)==23*15 && tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==2 && %s)",numPV.c_str()));
  RooDataHist DYTauTauHist3prong("DYTauTauHist3prong","",RooArgSet(mass),Import(*hDYTauTau3prong));
  RooHistPdf  DYTauTau3prongPdf("DYTauTau3prongPdf","",RooArgSet(mass),DYTauTauHist3prong);

  ////DataHist3prong.plotOn(plot);
  //DYTauTauPdf.plotOn(plot);
  //plot->Draw();
  //return;

  // W+jets
  TH1F* hWJets3prong = new TH1F("hWJets3prong","",40,20,120);
  backgroundWJets->Draw("diTauVisMass>>hWJets3prong",Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode!=-99 && %s",numPV.c_str()));
  TH1F* hWMt5 = new TH1F("hWMt5","",200,-200,200);
  hWMt5->Reset();
  backgroundWJets->Draw("(pZetaCorr-1.5*pZetaVisCorr)>>hWMt5",Form("puWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==2 && %s)",numPV.c_str()));
  float scaleFactor1 = (hWMt5->Integral(0,80))/(hWMt5->Integral(90,200));
  cout << "Conv. factor for W ==> " << scaleFactor1 << endl;
  float OSWinSignalRegion1 = data->GetEntries(Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)<-40 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==2 && %s",numPV.c_str()));
  OSWinSignalRegion1 *= (1./scaleFactor1);
  RooDataHist WJetsHist3prong("WJetsHist3prong","",RooArgSet(mass),Import(*hWJets3prong));
  RooHistPdf  WJets3prongPdf("WJets3prongPdf","",RooArgSet(mass),WJetsHist3prong);
  
  //DataHist3prong.plotOn(plot);
  //DYTauTauPdf.plotOn(plot);
  //WJetsPdf.plotOn(plot);
  //plot->Draw();
  //return;

  // SS for QCD
  TH1F* hSS3prongPdf = new TH1F("hSS3prongPdf","",40,20,120);
  data->Draw("diTauVisMass>>hSS3prongPdf",Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode!=-99 && %s",numPV.c_str()));
  TH1F* hSS3prong = new TH1F("hSS3prong","",40,20,120);
  data->Draw("diTauVisMass>>hSS3prong",Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==2 && %s",numPV.c_str()));
  TH1F* hWMt6 = new TH1F("hWMt6","",200,-200,200);
  hWMt6->Reset();
  backgroundWJets->Draw("(pZetaCorr-1.5*pZetaVisCorr)>>hWMt6",Form("puWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==2 && %s)",numPV.c_str()));
  float scaleFactor1SS = (hWMt6->Integral(0,80))/(hWMt6->Integral(90,200));
  cout << "Conv. factor for W SS ==> " << scaleFactor1SS << endl;
  float SSWinSignalRegion1 = data->GetEntries(Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)<-40 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==2 && %s",numPV.c_str()));
  SSWinSignalRegion1 *= (1./scaleFactor1SS);
  
  TH1F* hW3 = (TH1F*)hSS3prong->Clone("hW3");
  hW3->Reset();
  backgroundWJets->Draw("diTauVisMass>>hW3",Form("puWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==2 && %s)",numPV.c_str()));
  hW3->Scale(SSWinSignalRegion1/hW3->Integral());
  hSS3prong->Add(hW3, -1 );
  hSS3prong->Scale(1.06);
  RooDataHist SSHist3prong("SSHist3prong","",RooArgSet(mass),Import(*hSS3prongPdf));
  RooHistPdf  SS3prongPdf("SS3prongPdf","",RooArgSet(mass),SSHist3prong);

  RooRealVar NDYTauTau3prong("NDYTauTau3prong","",0,10000);
  RooRealVar NWJets3prong("NWJets3prong","",0,10000);
  RooRealVar NSS3prong("NSS3prong","",0,10000);

  RooLognormal NWJets3prong_C("NWJets3prong_C","",NWJets3prong,RooConst(OSWinSignalRegion1),RooConst(1.5));
  RooLognormal NSS3prong_C("NSS3prong_C","",NSS3prong,RooConst(hSS3prong->Integral()),RooConst(1.5));

  RooAddPdf model3prong("model3prong","",RooArgList(DYTauTau3prongPdf,WJets3prongPdf,SS3prongPdf),RooArgList(NDYTauTau3prong,NWJets3prong,NSS3prong));

  RooFitResult* res3prong = model3prong.fitTo(DataHist3prong,Extended(1), Minos(1), Save(1), NumCPU(4), ExternalConstraints( RooArgSet(NWJets3prong_C ,NSS3prong_C ) )  ,SumW2Error(1) );

  DataHist3prong.plotOn(plot3);
  model3prong.plotOn(plot3,LineColor(kBlue));
  model3prong.plotOn(plot3,LineColor(kRed), Components("DYTauTau3prongPdf"),LineStyle(kDashed));
  model3prong.plotOn(plot3,LineColor(kBlue), Components("WJets3prongPdf"),LineStyle(kDashed));
  model3prong.plotOn(plot3,LineColor(kBlack), Components("SS3prongPdf"),LineStyle(kDashed));
  plot3->Draw();

  cout << "In data ==> " << DataHist3prong.numEntries() << "; W => " << OSWinSignalRegion1 << " , SS\W => " << hSS3prong->Integral() << endl;

  //return;

  RooArgSet FitParam1prong0pi0(res1prong0pi0->floatParsFinal());
  RooRealVar* N1prong0pi0 = (RooRealVar*)(&FitParam1prong0pi0["NDYTauTau1prong0pi0"]);
  RooRealVar* NfitWJets1prong0pi0 = (RooRealVar*)(&FitParam1prong0pi0["NWJets1prong0pi0"]);
  RooRealVar* NfitSS1prong0pi0 = (RooRealVar*)(&FitParam1prong0pi0["NSS1prong0pi0"]);

  RooArgSet FitParam1prong1pi0(res1prong1pi0->floatParsFinal());
  RooRealVar* N1prong1pi0 = (RooRealVar*)(&FitParam1prong1pi0["NDYTauTau1prong1pi0"]);
  RooRealVar* NfitWJets1prong1pi0 = (RooRealVar*)(&FitParam1prong1pi0["NWJets1prong1pi0"]);
  RooRealVar* NfitSS1prong1pi0 = (RooRealVar*)(&FitParam1prong1pi0["NSS1prong1pi0"]);

  RooArgSet FitParam3prong(res3prong->floatParsFinal());
  RooRealVar* N3prong = (RooRealVar*)(&FitParam3prong["NDYTauTau3prong"]);
  RooRealVar* NfitWJets3prong = (RooRealVar*)(&FitParam3prong["NWJets3prong"]);
  RooRealVar* NfitSS3prong = (RooRealVar*)(&FitParam3prong["NSS3prong"]);

  cout << "1prong0pi0 DATA ==> " << N1prong0pi0->getVal() << " +" << N1prong0pi0->getErrorLo() << " -" << N1prong0pi0->getErrorHi() << endl;
  cout << "1prong1pi0 DATA ==> " << N1prong1pi0->getVal() << " +" << N1prong1pi0->getErrorLo() << " -" << N1prong1pi0->getErrorHi() << endl;
  cout << "3prong DATA ==> " << N3prong->getVal() << " +" << N3prong->getErrorLo() << " -" << N3prong->getErrorHi() << endl;

  cout << "1prong0pi0 MC ==> " << hDYTauTau1prong0pi0->Integral() << endl;
  cout << "1prong1pi0 MC ==> " << hDYTauTau1prong1pi0->Integral() << endl;
  cout << "3prong MC ==> "     << hDYTauTau3prong->Integral() << endl;


  float totalDATA = N1prong0pi0->getVal() + N1prong1pi0->getVal() + N3prong->getVal();
  float frac1prong1pi0DATA =  N1prong1pi0->getVal()/(N1prong0pi0->getVal() + N1prong1pi0->getVal() + N3prong->getVal());
  float frac3prongDATA     =  N3prong->getVal()/(N1prong0pi0->getVal() + N1prong1pi0->getVal() + N3prong->getVal());
  float frac1prong0pi0DATA = 1-frac1prong1pi0DATA-frac3prongDATA;

  float errFrac1prong1pi0DATA =  frac1prong1pi0DATA/totalDATA*sqrt(   
								   pow(N1prong0pi0->getError(),  2 )+
								   pow(1./frac1prong1pi0DATA -1 ,2 )+
								   pow(N3prong->getError(),      2 ) );
  float errFrac3prongDATA     =  frac3prongDATA/totalDATA*sqrt(   
							       pow(N1prong0pi0->getError(),  2 )+
							       pow(1./frac3prongDATA -1 ,    2 )+
							       pow(N1prong1pi0->getError(),  2 ) );
  
  float totalMC = hDYTauTau1prong0pi0->Integral()+hDYTauTau1prong1pi0->Integral()+hDYTauTau3prong->Integral();
  float frac1prong1pi0MC = hDYTauTau1prong1pi0->Integral()/totalMC;
  float frac1prong0pi0MC = hDYTauTau1prong0pi0->Integral()/totalMC;
  float frac3prongMC     = hDYTauTau3prong->Integral()/totalMC;


  cout << "1prong0pi0 ==> MC: " <<  frac1prong0pi0MC << "  ---  DATA: " << frac1prong0pi0DATA << " +/- " << sqrt(pow(errFrac1prong1pi0DATA,2)+pow(errFrac3prongDATA,2)) << endl;
  cout << "1prong1pi0 ==> MC: " <<  frac1prong1pi0MC << "  ---  DATA: " << frac1prong1pi0DATA << " +/- " << errFrac1prong1pi0DATA << endl;
  cout << "3prong ==> MC: " <<  frac3prongMC << "  ---  DATA: " << frac3prongDATA << " +/- " << errFrac3prongDATA << endl;



  //////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////


  float Lumi = 1061;
  THStack* aStack = new THStack("aStack",Form("CMS Preliminary 2011    #sqrt{s}=7 TeV L=%.2f fb^{-1}", Lumi/1000. ));

  TH1F* histo = new TH1F("histo","CMS Preliminary 2011 #sqrt{s}=7 TeV L=1.06 fb^{-1}; ; a.u.",3,0,3);
  TH1F* histoData = (TH1F*)histo->Clone("histoData");
  histoData->SetMarkerStyle(20);
  histoData->SetMarkerSize(1.2);
  histoData->SetMarkerColor(kBlack);
  TH1F* histoDYTauTau = (TH1F*)histo->Clone("histoDYTauTau");
  histoDYTauTau->SetFillColor(kWhite);
  TH1F* histoWJets = (TH1F*)histo->Clone("histoWJets");
  histoWJets->SetFillColor(kBlue);
  TH1F* histoTTbar = (TH1F*)histo->Clone("histoTTbar");
  histoTTbar->SetFillColor(kMagenta);
  TH1F* histoSS = (TH1F*)histo->Clone("histoSS");
  histoSS->SetFillColor(42);

  float scaleFactor = 1061./1000*(0.906/0.825)*0.9967*0.997*0.968;

  histoDYTauTau->SetBinContent(1, hDYTauTau1prong0pi0->Integral()*scaleFactor);
  histoWJets->SetBinContent(1,    NfitWJets1prong0pi0->getVal()  );
  histoSS->SetBinContent(1,       NfitSS1prong0pi0->getVal() /*+ NfitSS1prong0pi0->getError()*/ );
  histoData->SetBinContent(1,data->GetEntries(Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==0 && %s",numPV.c_str())) );

  TH1F* hEta = new TH1F("hEta","",1,-10,10);
  backgroundTTbar->Draw("etaL1>>hEta",Form("puWeight*sampleWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==0 && %s)",numPV.c_str() ) ) ;
  histoTTbar->SetBinContent(1, hEta->Integral()*scaleFactor  );

  histoDYTauTau->SetBinContent(2, hDYTauTau1prong1pi0->Integral()*scaleFactor);
  histoWJets->SetBinContent(2,    NfitWJets1prong1pi0->getVal()  );
  histoSS->SetBinContent(2,       NfitSS1prong1pi0->getVal() /*+ NfitSS1prong1pi0->getError()*/ );
  histoData->SetBinContent(2,data->GetEntries(Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==1 && %s",numPV.c_str())) );

  TH1F* hEta = new TH1F("hEta","",1,-10,10);
  backgroundTTbar->Draw("etaL1>>hEta",Form("puWeight*sampleWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==1 && %s)",numPV.c_str() ) ) ;
  histoTTbar->SetBinContent(2, hEta->Integral()*scaleFactor  );

  
  histoDYTauTau->SetBinContent(3, hDYTauTau3prong->Integral()*scaleFactor);
  histoWJets->SetBinContent(3,    NfitWJets3prong->getVal()  );
  histoSS->SetBinContent(3,       NfitSS3prong->getVal() /*+ NfitSS3prong->getError()*/ );
  histoData->SetBinContent(3,data->GetEntries(Form("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==2 && %s",numPV.c_str())) );

  TH1F* hEta = new TH1F("hEta","",1,-10,10);
  backgroundTTbar->Draw("etaL1>>hEta",Form("puWeight*sampleWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>40 && diTauVisMass<80 && decayMode==2 && %s)",numPV.c_str() ) ) ;
  histoTTbar->SetBinContent(3, hEta->Integral()*scaleFactor  );

  TH1F* histoAll = (TH1F*)histoDYTauTau->Clone("histoAll");
  histoAll->Reset();
  histoAll->Add(histoDYTauTau,1);
  histoAll->Add(histoWJets,1);
  histoAll->Add(histoTTbar,1);
  histoAll->Add(histoSS,1);
  histoAll->SetBinError(1,sqrt(histoDYTauTau->GetBinContent(1)*(0.06)*histoDYTauTau->GetBinContent(1)*(0.06) + NfitWJets1prong0pi0->getError()*NfitWJets1prong0pi0->getError() + NfitSS1prong0pi0->getError()*NfitSS1prong0pi0->getError()));
  histoAll->SetBinError(2,sqrt(histoDYTauTau->GetBinContent(2)*(0.06)*histoDYTauTau->GetBinContent(2)*(0.06) +NfitWJets1prong1pi0->getError()*NfitWJets1prong1pi0->getError() + NfitSS1prong1pi0->getError()*NfitSS1prong1pi0->getError()));
  histoAll->SetBinError(3,sqrt(histoDYTauTau->GetBinContent(3)*(0.06)*histoDYTauTau->GetBinContent(3)*(0.06) +NfitWJets3prong->getError()*NfitWJets3prong->getError() + NfitSS3prong->getError()*NfitSS3prong->getError()));
  histoAll->Sumw2();

  //noralize
  float mcIntegral = histoDYTauTau->Integral() + histoWJets->Integral() + histoSS->Integral()+histoTTbar->Integral();

  histoDYTauTau->Scale(1./mcIntegral);
  histoWJets->Scale(1./mcIntegral);
  histoSS->Scale(1./mcIntegral);
  histoTTbar->Scale(1./mcIntegral);

  histoWJets->Add(histoTTbar,1);

  histoAll->Scale(1./mcIntegral);

  aStack->Add(histoWJets);
  //aStack->Add(histoTTbar);
  aStack->Add(histoSS);
  aStack->Add(histoDYTauTau);

  histoData->GetXaxis()->SetBinLabel(1,"#pi");
  histoData->GetXaxis()->SetBinLabel(2,"#pi#pi^{0}");
  histoData->GetXaxis()->SetBinLabel(3,"#pi#pi#pi");
  histoData->Sumw2();
  histoData->DrawNormalized("P");
  //histoData->Draw("P");
  aStack->Draw("HISTSAME");
  histoData->DrawNormalized("PSAME");
  //histoData->Draw("PSAME");

  histoAll->SetMarkerColor(kRed);
  histoAll->SetMarkerStyle(1);
  histoAll->SetFillColor(kRed);
  histoAll->SetFillStyle(3004);
  histoAll->Draw("E2SAME");
  histoData->DrawNormalized("PSAME");

  TLegend* leg = new TLegend(0.61,0.64,0.81,0.90,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader("");

  leg->AddEntry(histoData,"Observed","P");
  leg->AddEntry(histoDYTauTau,"Z#rightarrow#tau#tau","F");
  leg->AddEntry(histoSS,"QCD","F");
  leg->AddEntry(histoWJets,"ElectroWeak","F");

  leg->Draw();
  
}
