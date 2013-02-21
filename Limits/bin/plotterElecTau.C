{

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

  TFile fData("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamSpring12_30Mar2012/nTupleRun2012A-ElecTau-PromptReco-v1-p1_run_Open_ElecTauStream.root");
  TFile fDYJets("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamSpring12_30Mar2012/nTupleDYJets-ElecTau-50-madgraph-Tarball_run_Open_ElecTauStream.root");
  TFile fWJets("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_06Dec2011/nTupleWJets-ElecTau-madgraph-PUS6_run_Open_ElecTauStream.root");
  TFile fTTJets("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_06Dec2011/nTupleTTJets-ElecTau-madgraph-PUS6_run_Open_ElecTauStream.root");


  TFile fData2011(  "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_06Dec2011/nTupleRun2011-ElecTau-All_run_Open_ElecTauStream.root");
  TFile fDYJets2011("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_06Dec2011/nTupleDYJets-ElecTau-50-madgraph-PUS6_run_Open_ElecTauStream.root");
  TFile fWJets2011( "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_06Dec2011/nTupleWJets-ElecTau-madgraph-PUS6_run_Open_ElecTauStream.root");
  TFile fTTJets2011("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_06Dec2011/nTupleTTJets-ElecTau-madgraph-PUS6_run_Open_ElecTauStream.root");

  //TFile fVBFH130("treeSkimmedElecTau_VBFH130.root");
  //TFile fGGFH130("treeSkimmedElecTau_GGFH130.root");

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
  //for(int k = 0 ; k <= nBins ; k++) bins[k] = 12*k; 
  TString variable("diTauVisMass");
  TString labels(" ; mass (GeV) ; Events");

  TH1F* hData    = new TH1F("hData"   ,labels, nBins, bins.GetArray());
  TH1F* hQCD     = new TH1F("hQCD"    ,labels, nBins, bins.GetArray());
  TH1F* hDYJets  = new TH1F("hDYJets" ,labels, nBins, bins.GetArray());
  TH1F* hZFakes  = new TH1F("hZFakes" ,labels, nBins, bins.GetArray());
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
  hZFakes->SetFillColor(kRed-3);
  hTTJets->SetFillColor(kBlue-2);
  //hVBFH130->SetLineWidth(3);
  //hVBFH130->SetLineStyle(kDashed);
  //hVBFH130->SetLineColor(kBlue);
  //hGGFH130->SetLineWidth(3);
  //hGGFH130->SetLineStyle(kDashed);
  //hGGFH130->SetLineColor(kBlue);

  // full signal selection
  TCut sbinRel(      "ptL1>20 && ptL2>20 && tightestHPSDBWP>0 && tightestMVAWP>=1 && diTauCharge==0 && elecFlag==0 && HLTx && HLTmatch");
  // full signal selection
  TCut sbin(         "ptL1>20 && ptL2>20 && tightestHPSDBWP>0 && tightestMVAWP>=1 && diTauCharge==0  && elecFlag==0 && HLTx && HLTmatch && combRelIsoLeg1DBeta<0.10");
  // selection of opposite-sign W-enriched control region
  TCut sbinAntiW(    "ptL1>20 && ptL2>20 && tightestHPSDBWP>0 && tightestMVAWP>=1 && diTauCharge==0  && elecFlag==0 && HLTx && HLTmatch && combRelIsoLeg1DBeta<0.10");
  // selection of smae-sign enriched control-region 
  TCut sbinSSAntiW(  "ptL1>20 && ptL2>20 && tightestHPSDBWP>0 && tightestMVAWP>=1 && diTauCharge!=0  && elecFlag==0 && HLTx && HLTmatch && combRelIsoLeg1DBeta<0.10");
  // selection of same-sign control region
  TCut sbinSS(       "ptL1>20 && ptL2>20 && tightestHPSDBWP>0 && tightestMVAWP>=1 && diTauCharge!=0  && elecFlag==0 && HLTx && HLTmatch && combRelIsoLeg1DBeta<0.10");
  // selection of same sign loosely-isolated control region
  TCut sbinSSRelIso( "ptL1>20 && ptL2>20 && tightestHPSDBWP>0 && tightestMVAWP>=1 && diTauCharge!=0  && elecFlag==0 && HLTx && HLTmatch && combRelIsoLeg1DBeta<0.10");
  TCut highMt("MtLeg1>60");
  TCut lowMt("MtLeg1<40");

  TCut slowMt("MtLeg1<40");

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

  //scaleFactWJets="1";
  
  if(debug){
    cout << totalWeight/numPVData->GetNbinsX() << endl;

    TH1F* numPVData     = new TH1F("numPVData","",  20,0,120);
    TH1F* numPVWJets    = new TH1F("numPVWJets","", 20,0,120);

    numPVData->Reset();
    numPVWJets->Reset();
    tWJets->Draw("MtLeg1>>numPVData",sbin);
    tWJets->Draw("MtLeg1>>numPVWJets",TString(scaleFactWJets.c_str())*sbin);
    
    cout << numPVWJets->Integral() << " -- " << numPVData->Integral() << endl;
    numPVWJets->SetLineColor(kRed);
    numPVWJets->Draw();
    numPVData->Draw("SAME");

    //return;

  }


  // luminoisty of data is 57.1 pb
  float lumiFact             = 57.1/1000;
  float crossSecScalingZ     = 3503.71/3048;
  float crossSecScalingW     = 36257./31314.;
  float crossSecScalingTT    =  225.2/157.5 ;

  // opposite-sign to same-sign ratio for QCD
  float OStoSSRatio = 1.11;

  /////////////////////////////////////////////////////////////////////
  // estimation of W+jets
  TH1F* h1 = new TH1F("h1","",1,-10,10);
  tWJets->Draw("etaL1>>h1",   TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbin&&highMt));
  float WsbinMCAntiW  = h1->Integral()*lumiFact*crossSecScalingW;
  h1->Reset();
  tWJets->Draw("etaL1>>h1",   TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbin&&lowMt));
  float WsbinMC       = h1->Integral()*lumiFact*crossSecScalingW;
  h1->Reset();
  tTTJets->Draw("etaL1>>h1",  TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbin&&highMt));
  float TTsbinAntiW = h1->Integral()*lumiFact*crossSecScalingTT;
  h1->Reset();
  tDYJets->Draw("etaL1>>h1",  TString(("sampleWeight*(  (abs(genDecay)==23*15) + (abs(genDecay)!=23*15)*( (TMath::Abs(etaL2)<1.5)*0.85 + (TMath::Abs(etaL2)>1.5)*0.65 ))*"+scaleFactDYJets).c_str())*(sbin&&highMt));
  float DYJetssbinAntiW = h1->Integral()*lumiFact;
  h1->Reset();

  tData->Draw("etaL1>>h1",   sbin&&highMt);
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
  TH1F* h1 = new TH1F("h1","",1,-10,10);
  tWJets->Draw("etaL1>>h1",   TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbinSS&&highMt));
  float WsbinMCSSAntiW  = h1->Integral()*lumiFact*crossSecScalingW;
  h1->Reset();
  tWJets->Draw("etaL1>>h1",   TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbinSS&&lowMt));
  float WsbinMCSS       = h1->Integral()*lumiFact*crossSecScalingW;
  h1->Reset();
  tTTJets->Draw("etaL1>>h1",  TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbinSS&&highMt));
  float TTsbinSSAntiW = h1->Integral()*lumiFact*crossSecScalingTT;
  h1->Reset();
  tDYJets->Draw("etaL1>>h1",  TString(("sampleWeight*(  (abs(genDecay)==23*15) + (abs(genDecay)!=23*15)*( (TMath::Abs(etaL2)<1.5)*0.85 + (TMath::Abs(etaL2)>1.5)*0.65 ))*"+scaleFactDYJets).c_str())*(sbinSS&&highMt));
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
  tData->Draw(  variable+">>hData",   "(index==0)"*(sbin&&slowMt));
  tData->Draw(  variable+">>hQCD",    "(index==0)"*(sbinSSRelIso&&slowMt));
  tDYJets->Draw(variable+">>hZFakes", TString(("sampleWeight*(index==0)*(abs(genDecay)!=23*15)*( 1 +(TMath::Abs(etaL2)<1.5)*(0.85-1) + (TMath::Abs(etaL2)>1.5)*(0.65-1) )*"+scaleFactDYJets).c_str())*(sbin&&slowMt));
  tDYJets->Draw(variable+">>hDYJets", TString(("sampleWeight*(index==0)*(abs(genDecay)==23*15)*"+scaleFactDYJets).c_str())*(sbin&&slowMt));
  tWJets->Draw( variable+">>hWJets",  TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbin&&slowMt));
  tWJets->Draw( variable+">>hWJetsSS",TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbinSS&&slowMt));
  tTTJets->Draw(variable+">>hTTJets", TString(("sampleWeight*"+scaleFactWJets).c_str())*(sbin&&slowMt));
  //tVBFH130->Draw(variable+">>hVBFH130", "puWeight*HLTweightMu*HLTweightTau*SFMu*SFTau*sampleWeight"*sbin);
  //tGGFH130->Draw(variable+">>hGGFH130", "puWeight*HLTweightMu*HLTweightTau*SFMu*SFTau*sampleWeight*HqTWeight"*sbin);
  
  // Scale histograms
  hDYJets->Scale( lumiFact );
  hZFakes->Scale( lumiFact );

  hWJets->Scale(  lumiFact*crossSecScalingW*WscaleFactorOS );
  hWJetsSS->Scale(lumiFact*crossSecScalingW*WscaleFactorSS );
  hTTJets->Scale( lumiFact*crossSecScalingTT );
  hQCD->Add(   hWJetsSS, -1);
  hQCD->Scale( QCDsbin/hQCD->Integral());
  //hVBFH130->Scale( lumiFact*100 );
  //hGGFH130->Scale( lumiFact*100 );
  //hVBFH130->Add(hGGFH130,1.0);
  hWJets->Add(hZFakes);


  //////////////////////////////////////////////////////////

  cout << "Old ==>" << endl;

  tData2011->Draw(variable+">>hOld", (sbin&&slowMt&&TCut("tightestAntiEMVAWP>0.5")));
  float data2011 = hOld->Integral();
  h1->Reset();
  tDYJets2011->Draw(variable+">>hOld",  "puWeight*sampleWeight*HLTweightTau*HLTweightElec*SFElec*(abs(genDecay)==23*15)"*(sbin&&slowMt&&TCut("tightestAntiEMVAWP>0.5")));
  float DYJets2011 = hOld->Integral();
  h1->Reset();
  tDYJets2011->Draw(variable+">>hOld",  "puWeight*sampleWeight*HLTweightTau*HLTweightElec*SFElec*(abs(genDecay)!=23*15)*( 1 +(TMath::Abs(etaL2)<1.5)*(0.85-1) + (TMath::Abs(etaL2)>1.5)*(0.65-1) )"*(sbin&&slowMt&&TCut("tightestAntiEMVAWP>0.5")));
  float ZFakes2011 = hOld->Integral();
  h1->Reset();
  tWJets2011->Draw(variable+">>hOld",   "puWeight*sampleWeight*HLTweightTau*HLTweightElec*SFElec"*(sbin&&slowMt&&TCut("tightestAntiEMVAWP>0.5")));
  float WJets2011 = hOld->Integral();
  h1->Reset();
  tTTJets2011->Draw(  variable+">>hOld","puWeight*sampleWeight*HLTweightTau*HLTweightElec*SFElec"*(sbin&&slowMt&&TCut("tightestAntiEMVAWP>0.5")));
  float TTJets2011 = hOld->Integral();
  h1->Reset();

  cout << "DYJets scaled to 57 pb = " << DYJets2011*0.057 << " ==> @ 8 TeV = " << DYJets2011*0.057*crossSecScalingZ
       << endl;
  cout << "    ==> ratio 2012/2011 = " << hDYJets->Integral()/(DYJets2011*0.057*crossSecScalingZ) << endl;

  cout << "ZFakes scaled to 57 pb = " << ZFakes2011*0.057 << " ==> @ 8 TeV = " << ZFakes2011*0.057*crossSecScalingZ
       << endl;
  cout << "    ==> ratio 2012/2011 = " << hZFakes->Integral()/(ZFakes2011*0.057*crossSecScalingZ) << endl;

  cout << "WJets scaled to 57 pb = " << 1.13*WJets2011*0.057 << " ==> @ 8 TeV = " << 1.13*WJets2011*0.057*crossSecScalingW
       << endl;
  cout << "    ==> ratio 2012/2011 = " << (hWJets->Integral()-hZFakes->Integral())/(1.13*WJets2011*0.057*crossSecScalingW) << endl;

  cout << "TTJets scaled to 57 pb = " << TTJets2011*0.057 << " ==> @ 8 TeV = " << TTJets2011*0.057*crossSecScalingTT
       << endl;
  cout << "    ==> ratio 2012/2011 = " << hTTJets->Integral()/(TTJets2011*0.057*crossSecScalingW) << endl;

  cout << "QCD scaled to 57 pb = " << (data2011-(DYJets2011+1.13*WJets2011+TTJets2011+ZFakes2011)*4.6)*0.057/4.6 << " ==> @ 8 TeV = ??" 
       << endl;
  cout << "    ==> ratio 2012/2011 = " 
       << (hQCD->Integral())/
    (data2011-(DYJets2011+ZFakes2011+1.13*WJets2011+TTJets2011)*4.6)/0.057*4.6 << endl;

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

  hData->SetAxisRange( 0, TMath::Max(hData->GetMaximum(), hAll->GetMaximum())*1.12, "Y");

  hData->Sumw2();
  hData->Draw("P");
  aStack->Draw("HISTSAME");
  hAll->Draw("E2SAME");
  hData->Draw("PSAME");

  cout << "DYJets = " << hDYJets->Integral() << endl;
  cout << "ZFakes = " << hZFakes->Integral() << endl;
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

  leg->SetHeader("#splitline{CMS Preliminary 2012}{#sqrt{s}=8 TeV, L=57.2 pb^{-1}, #tau_{e}#tau_{h}}");
  leg->AddEntry(hData,"Observed","P");
  leg->AddEntry(hDYJets,"Z#rightarrow#tau#tau","F");
  leg->AddEntry(hWJets,"W+jets","F");
  leg->AddEntry(hQCD,"QCD","F");
  leg->AddEntry(hTTJets,"t#bar{t}","F");
  //leg->AddEntry(hVBFH130,"100 X H#rightarrow#tau#tau, m_{H}=130 GeV","F");
  leg->Draw();

  gPad->SaveAs("plotElecTau_Test.png");

//////////////////////


  TCanvas *c2 = new TCanvas("c2","",5,30,650,600);
  c2->SetGrid(0,0);
  c2->SetFillStyle(4000);
  c2->SetFillColor(10);
  c2->SetTicky();
  c2->SetObjectStat(0);

  // Legend
  TLegend* leg2 = new TLegend(0.55,0.70,0.75,0.80,NULL,"brNDC");
  leg2->SetFillStyle(0);
  leg2->SetBorderSize(0);
  leg2->SetFillColor(10);
  leg2->SetTextSize(0.03);

  hDataOld->Sumw2();
  hDataNew->Sumw2();
  tData2011->Draw(variable+">>hDataOld", (sbin&&slowMt&&TCut("tightestAntiEMVAWP>0.5")));
  tData->Draw(variable+">>hDataNew", (sbin&&slowMt));
  hDataOld->SetLineColor(kBlack);
  hDataOld->SetLineWidth(3);
  hDataNew->SetLineColor(kRed);
  hDataNew->SetLineWidth(3);

  hDataNew->Scale(1./hDataNew->Integral());
  hDataOld->Scale(1./hDataOld->Integral());

  hDataNew->SetAxisRange( 0, TMath::Max(hDataNew->GetMaximum(), hDataOld->GetMaximum())*1.12, "Y");

  hDataNew->Draw("EHIST");
  hDataOld->Draw("HISTSAME");

  leg2->SetHeader("CMS Preliminary 2012  #tau_{e}#tau_{h}");
  leg2->AddEntry(hDataOld,"Run2011 L=4.6 fb^{-1} #sqrt{s}=7 TeV");
  leg2->AddEntry(hDataNew,"Run2012 L=0.056 fb^{-1} #sqrt{s}=8 TeV");

  leg2->Draw();

  return;

}
