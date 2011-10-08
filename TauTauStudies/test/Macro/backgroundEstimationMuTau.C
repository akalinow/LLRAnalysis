


{

  using namespace std;
  using namespace RooFit;

  float Lumi = 24.86+159.15;

  float hltCorr = 0.92;

  TCut hlt("( ((HLTmu==1 && run<=163261) || (HLTx==1 && run>163261)) )");
  TCut hltMC("HLTmu==1");
  
  TCut signal_region_MC("sampleWeight*puWeight*(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==0 && MtLeg1<40 && diTauCharge==0 && HLTmu==1)");
  TCut signal_region("sampleWeight*puWeight*(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==0 && MtLeg1<40 && diTauCharge==0)");
  TCut signal_SS_region("sampleWeight*puWeight*(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==0 && MtLeg1<40 && diTauCharge!=0)");
  TCut signal_SS_region_MC("sampleWeight*puWeight*(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==0 && MtLeg1<40 && diTauCharge!=0 && HLTmu==1)");
  TCut Wenrich_OS_region("sampleWeight*puWeight*(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==0 && MtLeg1>60 && diTauCharge==0)");
  TCut Wenrich_OS_region_MC("sampleWeight*puWeight*(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==0 && MtLeg1>60 && diTauCharge==0 && HLTmu==1)");
  TCut Wenrich_SS_region("sampleWeight*puWeight*(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==0 && MtLeg1>60 && diTauCharge!=0)");
  TCut Wenrich_SS_region_MC("sampleWeight*puWeight*(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==0 && MtLeg1>60 && diTauCharge!=0 && HLTmu==1)");
  TCut Wrelaxed_OS_region("sampleWeight*puWeight*(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==0 && diTauCharge==0)");
  TCut Wrelaxed_SS_region("sampleWeight*puWeight*(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==0 && diTauCharge!=0)");
  TCut QCDenrich_OS_region("sampleWeight*puWeight*(tightestHPSWP>0 && combRelIsoLeg1DBeta>0.15 && muFlag==0 && MtLeg1<40 && diTauCharge==0)");
  TCut QCDenrich_OS_region_MC("sampleWeight*puWeight*(tightestHPSWP>0 && combRelIsoLeg1DBeta>0.15 && muFlag==0 && MtLeg1<40 && diTauCharge==0 && HLTmu==1)");
  TCut QCDenrich_SS_region("sampleWeight*puWeight*(tightestHPSWP>0 && combRelIsoLeg1DBeta>0.15 && muFlag==0 && MtLeg1<40 && diTauCharge!=0)");
  TCut QCDenrich_SS_region_MC("sampleWeight*puWeight*(tightestHPSWP>0 && combRelIsoLeg1DBeta>0.15 && muFlag==0 && MtLeg1<40 && diTauCharge!=0 && HLTmu==1)");
  TCut ZMuMuFakeJet_OS_region("sampleWeight*puWeight*(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==1 && MtLeg1<40 && diTauCharge==0)");
  TCut ZMuMuFakeJet_SS_region("sampleWeight*puWeight*(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==1 && MtLeg1<40 && diTauCharge!=0) ");
  TCut ZMuMuFakeJet_SS_region_MC("sampleWeight*puWeight*(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==1 && MtLeg1<40 && diTauCharge!=0 && HLTmu==1 ) ");

  TFile *fData = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2//Inclusive/nTupleRun2011-Mu_Open_MuTauStream.root","READ");
  TTree* treeData = (TTree*)fData->Get("outTreePtOrd");

  TFile *fDiBoson = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleDiBoson-Mu_Open_MuTauStream.root","READ");
  TTree* treeDiBoson = (TTree*)fDiBoson->Get("outTreePtOrd");

  TFile *fTTbar = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleTTJets-Mu-madgraph-PUS1_Open_MuTauStream.root","READ");
  TTree* treeTTbar = (TTree*)fTTbar->Get("outTreePtOrd");

  TFile *fSingleTop = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleSingleTop-Mu_Open_MuTauStream.root","READ");
  TTree* treeSingleTop = (TTree*)fSingleTop->Get("outTreePtOrd");

  TFile *fWJets = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleWJets-Mu-madgraph-PUS1_Open_MuTauStream.root","READ");
  TTree* treeWJets = (TTree*)fWJets->Get("outTreePtOrd");

  TFile *fQCD = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleQCDmu_Open_MuTauStream.root","READ");
  TTree* treeQCD = (TTree*)fQCD->Get("outTreePtOrd");

  TFile *fZTauTau = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleDYJets-Mu-50-madgraph-PUS1_Open_MuTauStream.root","READ");
  TTree* treeZTauTauAll = (TTree*)fZTauTau->Get("outTreePtOrd");
  TFile* dummy1 = new TFile("dummy1.root","RECREATE");
  TTree *treeZTauTau = (TTree*)treeZTauTauAll->CopyTree("isTauLegMatched>0.5");
  TTree *treeZFakes  = (TTree*)treeZTauTauAll->CopyTree("isTauLegMatched<0.5");


  TH1F* h1 = new TH1F("h1","",1,-10,10);

  //////////////// DiBoson + TTbar + SingleTop
 
  treeDiBoson->Draw("etaL1>>h1",signal_region_MC);
  float N_DiBoson_signal_region = (float)h1->Integral()*(Lumi/1000.*hltCorr);
  h1->Reset();

  treeDiBoson->Draw("etaL1>>h1",Wenrich_OS_region_MC);
  float N_DiBoson_Wenrich_OS_region = (float)h1->Integral()*(Lumi/1000.*hltCorr);
  h1->Reset();

  treeTTbar->Draw("etaL1>>h1",signal_region_MC);
  float N_TTbar_signal_region = (float)h1->Integral()*(Lumi/1000.*hltCorr);
  h1->Reset();

  treeTTbar->Draw("etaL1>>h1",Wenrich_OS_region_MC);
  float N_TTbar_Wenrich_OS_region = (float)h1->Integral()*(Lumi/1000.*hltCorr);
  h1->Reset();

  treeSingleTop->Draw("etaL1>>h1",signal_region_MC);
  float N_SingleTop_signal_region = (float)h1->Integral()*(Lumi/1000.*hltCorr);
  h1->Reset();

  treeSingleTop->Draw("etaL1>>h1",Wenrich_OS_region_MC);
  float N_SingleTop_Wenrich_OS_region = (float)h1->Integral()*(Lumi/1000.*hltCorr);
  h1->Reset();


  cout << "N_DiBoson_signal_region = " <<  N_DiBoson_signal_region << endl;
  cout << "N_DiBoson_Wenrich_OS_region = " << N_DiBoson_Wenrich_OS_region << endl;
  cout << "N_TTbar_signal_region = " << N_TTbar_signal_region << endl;
  cout << "N_TTbar_Wenrich_OS_region = " << N_TTbar_Wenrich_OS_region << endl;
  cout << "N_SingleTop_signal_region = " << N_SingleTop_signal_region << endl;
  cout << "N_SingleTop_Wenrich_OS_region = " << N_SingleTop_Wenrich_OS_region << endl;

  cout << "//////////////////////////////////////" << endl;

  //////////////// Z->mumu, jet->tau, mu->tau
  
  treeData->Draw("etaL1>>h1",ZMuMuFakeJet_SS_region&&hlt);
  float N_ZMuMuFakeJet_signal_region = (float)h1->Integral();
  h1->Reset();
  // correct for A and eff (from AN-10-430)
  float eA = 0.57;
  float eI = 0.976;
  N_ZMuMuFakeJet_signal_region *= (  (1-eA+eA*(1-eI))/(eA*eI) );
  float N_ZMuMuFakeJet_signal_SS_region =  N_ZMuMuFakeJet_signal_region;

  // apply factor 2 ratio between Z->mumu, jet->tau and Z->mumu, mu->tau
  float N_ZMuMuFakeMu_signal_region = N_ZMuMuFakeJet_signal_region * 2;

  treeZFakes->Draw("etaL1>>h1",signal_region&&hltMC);
  float Exp_ZFake_signal_region = (float)h1->Integral();
  Exp_ZFake_signal_region *= (Lumi/1000.*hltCorr);
  h1->Reset();

  cout << "N_ZMuMuFakeJet_signal_region = " << N_ZMuMuFakeJet_signal_region << endl;
  cout << "N_ZMuMuFakeJet_signal_SS_region = " << N_ZMuMuFakeJet_signal_SS_region << endl;
  cout << "N_ZMuMuFakeMu_signal_region = "  << N_ZMuMuFakeMu_signal_region << endl;
  cout << "Exp_ZFake_signal_region = "  << Exp_ZFake_signal_region << endl;

  cout << "//////////////////////////////////////" << endl;


  /////////////// W for OS
  treeData->Draw("etaL1>>h1", Wenrich_OS_region&&hlt);
  float N_WMuNu_Wenrich_OS_region = (float)h1->Integral();
  h1->Reset();

  TH1F* h1_Mt = new TH1F("h1_Mt","",200,0,200);
  treeWJets->Draw("MtLeg1>>h1_Mt", Wrelaxed_OS_region);
  float WratioOS = (h1_Mt->Integral(61,120))/(h1_Mt->Integral(0,40));
  h1_Mt->Reset();

  N_WMuNu_Wenrich_OS_region -= N_DiBoson_Wenrich_OS_region;
  N_WMuNu_Wenrich_OS_region -= N_TTbar_Wenrich_OS_region;
  N_WMuNu_Wenrich_OS_region -= N_SingleTop_Wenrich_OS_region;

  float N_WMuNu_signal_region = N_WMuNu_Wenrich_OS_region*(1./WratioOS);

  float WTauWMuRatio = 0.267;
  float N_WTauNu_signal_region = N_WMuNu_signal_region*WTauWMuRatio;


  cout << "WratioOS = " << WratioOS << endl;
  cout << "N_WMuNu_Wenrich_OS_region = " << N_WMuNu_Wenrich_OS_region << endl;
  cout << "N_WMuNu_signal_region = "  <<N_WMuNu_signal_region  << endl;
  cout << "N_WTauNu_signal_region = " << N_WTauNu_signal_region << endl;
  cout << "N_WTauNu_signal_region = " << N_WTauNu_signal_region << endl;

  cout << "//////////////////////////////////////" << endl;

  /////////////// W for SS
  treeData->Draw("etaL1>>h1", Wenrich_SS_region&&hlt);
  float N_WMuNu_Wenrich_SS_region = (float)h1->Integral();
  h1->Reset();

  treeWJets->Draw("MtLeg1>>h1_Mt", Wrelaxed_SS_region);
  float WratioSS = (h1_Mt->Integral(61,120))/(h1_Mt->Integral(0,40));
  h1_Mt->Reset();

  float N_WMuNu_signal_SS_region  = N_WMuNu_Wenrich_SS_region*(1./WratioSS);
  float N_WTauNu_signal_SS_region = N_WMuNu_signal_SS_region*WTauWMuRatio;

  cout << "WratioSS = " << WratioSS << endl;
  cout << "N_WMuNu_signal_SS_region = " << N_WMuNu_signal_SS_region << endl;
  cout << "N_WTauNu_signal_SS_region = " <<  N_WTauNu_signal_SS_region << endl;

  cout << "//////////////////////////////////////" << endl;


  //QCD
  treeData->Draw("etaL1>>h1",signal_SS_region&&hlt);
  float N_QCD_signal_SS_region = (float)h1->Integral();
  h1->Reset();

  N_QCD_signal_SS_region -= N_WMuNu_signal_SS_region; 
  N_QCD_signal_SS_region -= N_WTauNu_signal_SS_region;
  N_QCD_signal_SS_region -= N_ZMuMuFakeJet_signal_SS_region;

  treeData->Draw("etaL1>>h1",QCDenrich_OS_region&&hlt);
  float N_QCD_QCDenrich_OS_region = (float)h1->Integral();
  h1->Reset();
  treeData->Draw("etaL1>>h1",QCDenrich_SS_region&&hlt);
  float N_QCD_QCDenrich_SS_region = (float)h1->Integral();
  h1->Reset();
  
  float SSOSratio = N_QCD_QCDenrich_OS_region/N_QCD_QCDenrich_SS_region;
  float N_QCD_signal_region = N_QCD_signal_SS_region * SSOSratio;

  treeQCD->Draw("etaL1>>h1",signal_region_MC);
  float Exp_QCD_signal_region = (float)h1->Integral();
  Exp_QCD_signal_region *= (Lumi/1000.*hltCorr);
  h1->Reset();

  cout << "SSOSratio = " << SSOSratio  << endl;
  cout << "N_QCD_signal_SS_region = " << N_QCD_signal_SS_region << endl;
  cout << "N_QCD_signal_region = " << N_QCD_signal_region << endl;
  cout << "Exp_QCD_signal_region = " << Exp_QCD_signal_region << endl;

  cout << "//////////////////////////////////////" << endl;

 
  treeData->Draw("etaL1>>h1",signal_region&&hlt);
  float N_Z_signal_region = (float)h1->Integral();
  h1->Reset();

  N_Z_signal_region -= N_QCD_signal_region;
  N_Z_signal_region -= N_DiBoson_signal_region;
  N_Z_signal_region -= N_TTbar_signal_region;
  N_Z_signal_region -= N_SingleTop_signal_region;
  N_Z_signal_region -= N_ZMuMuFakeJet_signal_region;
  N_Z_signal_region -= N_ZMuMuFakeMu_signal_region;
  N_Z_signal_region -= N_WMuNu_signal_region;
  N_Z_signal_region -= N_WTauNu_signal_region;

  treeZTauTau->Draw("etaL1>>h1",signal_region_MC);
  float Exp_Z_signal_region = (float)h1->Integral();
  Exp_Z_signal_region *= (Lumi/1000.*hltCorr);
  h1->Reset();


  TFile* dummy1 = new TFile("dummy2.root","RECREATE");
  TTree *treeDataCut = (TTree*)treeData->CopyTree("(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==0 && MtLeg1<40 && diTauCharge==0 && ( ((HLTmu==1 && run<=163261) || (HLTx==1 && run>163261)) ))");
  TTree *treeQCDCut = (TTree*)treeData->CopyTree("(tightestHPSWP>0 && combRelIsoLeg1DBeta>0.15 && muFlag==0 && MtLeg1<40 && diTauCharge!=0)");
  TTree *treeQCDIsoCut = (TTree*)treeData->CopyTree("(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && muFlag==0 && MtLeg1<40 && diTauCharge!=0)");
  TTree *treeZTauTauCut = (TTree*)treeZTauTau->CopyTree("(HLTmu==1 && tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==0 && MtLeg1<40 && diTauCharge==0)");
  TTree *treeZFakesCut = (TTree*)treeZFakes->CopyTree("(HLTmu==1 && tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==0 && MtLeg1<40 && diTauCharge==0)");
  TTree *treeDiBosonCut = (TTree*)treeDiBoson->CopyTree("(HLTmu==1 && tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==0 && MtLeg1<40 && diTauCharge==0)");
  TTree *treeTTbarCut = (TTree*)treeTTbar->CopyTree("(HLTmu==1 && tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==0 && MtLeg1<40 && diTauCharge==0)");
  TTree *treeSingleTopCut = (TTree*)treeSingleTop->CopyTree("(HLTmu==1 && tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==0 && MtLeg1<40 && diTauCharge==0)");
  TTree *treeWJetsCut = (TTree*)treeWJets->CopyTree("(HLTmu==1 && tightestHPSWP>0 && combRelIsoLeg1DBeta<0.1 && muFlag==0 && MtLeg1<40 && diTauCharge==0)");

  RooRealVar Nqcd("Nqcd","",        100,0,10000);
  RooRealVar Nztt("Nztt","",        100,0,10000);
  RooRealVar Nwen("Nwen","",        100,0,10000);
  RooRealVar Nttb("Nttb","",        100,0,10000);
  RooRealVar Nstp("Nstp","",        100,0,10000);
  RooRealVar Ndib("Ndib","",        100,0,10000);
  RooRealVar Nzfk("Nzfk","",        100,0,10000);

  RooLognormal McNqcdConstraint("McNqcdConstraint","",Nqcd,RooConst(N_QCD_signal_region),RooConst(1.5)) ;
  RooLognormal McNzttConstraint("McNzttConstraint","",Nztt,RooConst(N_Z_signal_region),RooConst(1.5));
  RooLognormal McNwenConstraint("McNwenConstraint","",Nwen,RooConst(N_WMuNu_signal_region+N_WTauNu_signal_region),RooConst(1.5)) ;
  RooLognormal McNttbConstraint("McNttbConstraint","",Nttb,RooConst(N_TTbar_signal_region),RooConst(1.5)) ;
  RooLognormal McNstpConstraint("McNstbConstraint","",Nstp,RooConst(N_SingleTop_signal_region),RooConst(1.5)) ;
  RooLognormal McNdibConstraint("McNdibConstraint","",Ndib,RooConst(N_DiBoson_signal_region),RooConst(1.5)) ;
  RooLognormal McNzfkConstraint("McNzfkConstraint","",Nzfk,RooConst(N_ZMuMuFakeJet_signal_region+N_ZMuMuFakeMu_signal_region),RooConst(1.5)) ;

  RooRealVar mass("diTauVisMass","visible mass (GeV/c^{2})", 0 , 200);
  mass.setBins(40);
  RooRealVar weight("puWeight","weight", 0 , 10);

  TH1F* data_SS = new TH1F("data_SS","",40,0,200);
  TH1F* W_SS = (TH1F*)data_SS->Clone("W_SS");
  treeQCDIsoCut->Draw("diTauVisMass>>data_SS");
  treeWJets->Draw("diTauVisMass>>W_SS",signal_SS_region);
  W_SS->Scale((N_WMuNu_signal_SS_region+N_WTauNu_signal_SS_region)/W_SS->Integral());
  data_SS->Add(W_SS, -1 );
  TH1F* dataNotIso_SS = (TH1F*)data_SS->Clone("dataNotIso_SS");
  treeQCDCut->Draw("diTauVisMass>>dataNotIso_SS");

  //dataNotIso_SS->SetLineColor(kRed);
  //W_SS->SetLineColor(kBlue);
  //data_SS->DrawNormalized();
  //W_SS->DrawNormalized("SAME");
  //dataNotIso_SS->DrawNormalized("SAME");



  RooDataSet qcdDataSet("qcdDataSet","dataset for qcd", RooArgSet(mass), Import( *treeQCDCut ) );
  //RooDataHist qcdDataHist("qcdDataHist","",RooArgSet(mass),qcdDataSet, 1.0);
  RooDataHist qcdDataHist("qcdDataHist","",RooArgSet(mass), Import(*data_SS));
  RooHistPdf  qcdPdf("qcdPdf","",RooArgSet(mass),qcdDataHist);

  RooDataSet zttDataSet("zttDataSet","dataset for Ztautau", RooArgSet(mass), Import( *treeZTauTauCut ) );
  TH1F* ztt = new TH1F("ztt","",40,0,200);
  treeZTauTauCut->Draw("diTauVisMass>>ztt","puWeight");
  //RooDataHist zttDataHist("zttDataHist","",RooArgSet(mass),zttDataSet, 1.0);
  RooDataHist zttDataHist("zttDataHist","",RooArgSet(mass), Import(*ztt));
  RooHistPdf  zttPdf("zttPdf","",RooArgSet(mass),zttDataHist);

  RooDataSet zFakesDataSet("zFakesDataSet","dataset for Ztautau", RooArgSet(mass), Import( *treeZFakesCut ) );
  RooDataHist zFakesDataHist("zFakesDataHist","",RooArgSet(mass),zFakesDataSet, 1.0);
  RooHistPdf  zFakesPdf("zFakesPdf","",RooArgSet(mass),zFakesDataHist);

  RooDataSet dibDataSet("dibDataSet","dataset for Ztautau", RooArgSet(mass), Import( *treeDiBosonCut ) );
  RooDataHist dibDataHist("dibDataHist","",RooArgSet(mass), dibDataSet, 1.0);
  RooHistPdf  dibPdf("dibPdf","",RooArgSet(mass),dibDataHist);

  RooDataSet ttbDataSet("DataSet","dataset for Ztautau", RooArgSet(mass), Import( *treeTTbarCut ) );
  RooDataHist ttbDataHist("ttbDataHist","",RooArgSet(mass), ttbDataSet, 1.0);
  RooHistPdf  ttbPdf("ttbPdf","",RooArgSet(mass),ttbDataHist);

  RooDataSet stpDataSet("stpDataSet","dataset for Ztautau", RooArgSet(mass), Import( *treeSingleTopCut ) );
  RooDataHist stpDataHist("stpDataHist","",RooArgSet(mass), stpDataSet, 1.0);
  RooHistPdf  stpPdf("stpPdf","",RooArgSet(mass),stpDataHist);

  RooDataSet wenDataSet("wenDataSet","dataset for Ztautau", RooArgSet(mass), Import( *treeWJetsCut ) );
  TH1F* wen = new TH1F("wen","",40,0,200);
  treeWJetsCut->Draw("diTauVisMass>>wen","puWeight");
  //RooDataHist wenDataHist("wenDataHist","",RooArgSet(mass), wenDataSet, 1.0);
  RooDataHist wenDataHist("wenDataHist","",RooArgSet(mass), Import(*wen));
  RooHistPdf  wenPdf("wenPdf","",RooArgSet(mass),wenDataHist);

  RooDataSet dataDataSet("dataDataSet","dataset for qcd", RooArgSet(mass), Import( *treeDataCut ) );
  RooDataHist dataDataHist("dataDataHist","",RooArgSet(mass), dataDataSet, 1.0);

  RooAddPdf model("model","",RooArgList(zttPdf,qcdPdf,wenPdf,ttbPdf,zFakesPdf,dibPdf,stpPdf),RooArgList(Nztt,Nqcd,Nwen,Nttb,Nzfk,Ndib,Nstp));

  RooFitResult* res = model.fitTo(dataDataHist , Extended(1), Minos(1), Save(1), NumCPU(4), ExternalConstraints( RooArgSet(McNqcdConstraint,/*McNzttConstraint,*/McNwenConstraint,McNttbConstraint,McNstpConstraint,McNdibConstraint,McNzfkConstraint) )  ,SumW2Error(1));

  RooArgSet fitParam(res->floatParsFinal());
  RooRealVar* Fit_Z_signal_region = (RooRealVar*)(&fitParam["Nztt"]);


  RooPlot* frame = mass.frame(Bins(40),Title(Form("CMS Preliminary 2011  #sqrt{s}=7 TeV   L=%.0f pb^{-1}",Lumi)));

  dataDataHist.plotOn(frame);
  model.plotOn(frame, LineColor(kBlue),  LineStyle(kSolid));
  model.plotOn(frame, Components("zttPdf"), LineColor(kRed),  LineStyle(kSolid));
  model.plotOn(frame, Components("zFakesPdf"), LineColor(kRed),  LineStyle(kDashed));
  model.plotOn(frame, Components("qcdPdf"), LineColor(kBlack), LineStyle(4));
  model.plotOn(frame, Components("wenPdf"), LineColor(kGreen), LineStyle(5));
  model.plotOn(frame, Components("ttbPdf"), LineColor(kYellow), LineStyle(6));
  model.plotOn(frame, Components("stpPdf"), LineColor(kMagenta), LineStyle(7));
  model.plotOn(frame, Components("dibPdf"), LineColor(kBlack), LineStyle(8));

  frame->Draw();

  cout << "Estimated ==> " << N_Z_signal_region 
       << " --- Expected --- "   << Exp_Z_signal_region 
       << " --- Fit --- " <<  Fit_Z_signal_region->getVal() << " +/- " << Fit_Z_signal_region->getError()
       << endl;

  TH1F* h_0 = new TH1F("h_0","",1,0,1);
  h_0->SetLineColor(kBlue);
  h_0->SetLineWidth(3);
  h_0->SetLineStyle(kSolid);
  TH1F* h_1 = new TH1F("h_1","",1,0,1);
  h_1->SetLineColor(kRed);
  h_1->SetLineWidth(3);
  h_1->SetLineStyle(kSolid);
  TH1F* h_2 = new TH1F("h_2","",1,0,1);
  h_2->SetLineColor(kRed);
  h_2->SetLineWidth(3);
  h_2->SetLineStyle(kDashed);
  TH1F* h_3 = new TH1F("h_3","",1,0,1);
  h_3->SetLineColor(kBlack);
  h_3->SetLineWidth(3);
  h_3->SetLineStyle(4);
  TH1F* h_4 = new TH1F("h_4","",1,0,1);
  h_4->SetLineColor(kGreen);
  h_4->SetLineWidth(3);
  h_4->SetLineStyle(5);
  TH1F* h_5 = new TH1F("h_5","",1,0,1);
  h_5->SetLineColor(kYellow);
  h_5->SetLineWidth(3);
  h_5->SetLineStyle(6);
  TH1F* h_6 = new TH1F("h_6","",1,0,1);
  h_6->SetLineColor(kMagenta);
  h_6->SetLineWidth(3);
  h_6->SetLineStyle(7);
  TH1F* h_7 = new TH1F("h_7","",1,0,1);
  h_7->SetLineColor(kBlack);
  h_7->SetLineWidth(3);
  h_7->SetLineStyle(8);

  TLegend* leg = new TLegend(0.55,0.47,0.85,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);

  //leg->SetHeader(Form("Exp: %.0f - Est: %.0f - Fit:%.0f#pm%.0f",Exp_Z_signal_region,N_Z_signal_region,Fit_Z_signal_region->getVal(),Fit_Z_signal_region->getError()));
  leg->SetHeader("#mu+#tau");
  leg->AddEntry(h_0,"Fit to data","L");
  leg->AddEntry(h_1,"Z#tau#tau","L");
  leg->AddEntry(h_2,"Z+fakes","L");
  leg->AddEntry(h_3,"SS data \\ W","L");
  leg->AddEntry(h_4,"W+jets","L");
  leg->AddEntry(h_5,"t#bar{t}","L");
  leg->AddEntry(h_6,"Single-t","L");
  leg->AddEntry(h_7,"Di-Boson","L");

  leg->Draw();

}
