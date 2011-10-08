


{

  TCanvas *c1 = new TCanvas("c1","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);

  TLegend* leg = new TLegend(0.65,0.60,0.85,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader("");

  bool do1prong0pi0 = false;
  bool do1prong1pi0 = false;
  bool do3prong     = true;

  bool normalize = true;

  TFile* fData     = new TFile("nTupleRun2011-Mu_All_Open_MuTauStream.root","READ");
  TFile* fDYTauTau = new TFile("nTupleDYJets-Mu-50-madgraph-PUS4_run_Open_MuTauStream.root");
  TFile* fWJets    = new TFile("nTupleWJets-Mu-madgraph-PUS4_run_Open_MuTauStream.root");

  TString tree = "outTreePtOrd";
  TTree *data                = (TTree*)fData->Get(tree);
  TTree *backgroundDYTauTau  = (TTree*)fDYTauTau->Get(tree);
  TTree *backgroundWJets     = (TTree*)fWJets->Get(tree);
  

  vector<string> samples;
  samples.push_back("Data"); 
  //samples.push_back("SS"); 
  //samples.push_back("Ztautau"); 
  //samples.push_back("EWK"); 


  TCut signalOS("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>50 && diTauVisMass<70 && numPV>5");
  TCut signalSS("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge!=0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>50 && diTauVisMass<70 && numPV>5");
  
  TCut c1prong0Pi0("decayMode==0");
  TCut c1prong1Pi0("decayMode==1");
  TCut c3prong("decayMode==2");
  
  TH1F* hMass = new TH1F("hMass","CMS Preliminary 2011 #sqrt{s}=7 TeV L = 1.06 fb^{-1}; tau visible mass (GeV); a.u.",17,0.2,1.5);
  TH1F* h1prong0Pi0 = new TH1F("h1prong0Pi0","CMS Preliminary 2011 #sqrt{s}=7 TeV L = 1.06 fb^{-1}; tau visible mass  (GeV); a.u.",3,0.,0.3);
  TH1F* h1prong1Pi0 = new TH1F("h1prong1Pi0","CMS Preliminary 2011 #sqrt{s}=7 TeV L = 1.06 fb^{-1}; tau visible mass  (GeV); a.u.",8,0.2,1.4);
  TH1F* h3prong     = new TH1F("h3prong",    "CMS Preliminary 2011 #sqrt{s}=7 TeV L = 1.06 fb^{-1}; tau visible mass  (GeV); a.u.",9,0.7,1.6);;

  TH1F* h1prong0Pi0Data = (TH1F*)h1prong0Pi0->Clone("h1prong0Pi0Data");
  TH1F* h1prong1Pi0Data = (TH1F*)h1prong1Pi0->Clone("h1prong1Pi0Data");
  TH1F* h3prongData     = (TH1F*)h3prong->Clone("h3prongData");
  h1prong0Pi0Data->SetMarkerStyle(20);
  h1prong0Pi0Data->SetMarkerSize(1.2);
  h1prong0Pi0Data->SetMarkerColor(kBlack);
  h1prong1Pi0Data->SetMarkerStyle(20);
  h1prong1Pi0Data->SetMarkerSize(1.2);
  h1prong1Pi0Data->SetMarkerColor(kBlack);
  h3prongData->SetMarkerStyle(20);
  h3prongData->SetMarkerSize(1.2);
  h3prongData->SetMarkerColor(kBlack);

  TH1F* h1prong0Pi0DYTauTau = (TH1F*)h1prong0Pi0->Clone("h1prong0Pi0DYTauTau");
  TH1F* h1prong1Pi0DYTauTau = (TH1F*)h1prong1Pi0->Clone("h1prong1Pi0DYTauTau");
  TH1F* h1prong1Pi0DYTauTauUp = (TH1F*)h1prong1Pi0->Clone("h1prong1Pi0DYTauTauUp");
  TH1F* h1prong1Pi0DYTauTauDown = (TH1F*)h1prong1Pi0->Clone("h1prong1Pi0DYTauTauDown");
  TH1F* h3prongDYTauTau     = (TH1F*)h3prong->Clone("h3prongDYTauTau");
  TH1F* h3prongDYTauTauUp     = (TH1F*)h3prong->Clone("h3prongDYTauTauUp");
  TH1F* h3prongDYTauTauDown   = (TH1F*)h3prong->Clone("h3prongDYTauTauDown");
  h1prong0Pi0DYTauTau->SetFillColor(kWhite);
  h1prong0Pi0DYTauTau->SetLineColor(kBlack);
  h1prong1Pi0DYTauTau->SetFillColor(kWhite);
  h1prong1Pi0DYTauTau->SetLineColor(kBlack);
  h1prong1Pi0DYTauTauUp->SetLineColor(kMagenta);
  h1prong1Pi0DYTauTauUp->SetLineWidth(3);
  h1prong1Pi0DYTauTauUp->SetLineStyle(kDashed);
  h1prong1Pi0DYTauTauDown->SetLineColor(kMagenta);
  h1prong1Pi0DYTauTauDown->SetLineWidth(3);
  h1prong1Pi0DYTauTauDown->SetLineStyle(kDotted);
  h3prongDYTauTau->SetFillColor(kWhite);
  h3prongDYTauTau->SetLineColor(kBlack);
  h3prongDYTauTauUp->SetLineColor(kMagenta);
  h3prongDYTauTauUp->SetLineWidth(3);
  h3prongDYTauTauUp->SetLineStyle(kDashed);
  h3prongDYTauTauDown->SetLineColor(kMagenta);
  h3prongDYTauTauDown->SetLineWidth(3);
  h3prongDYTauTauDown->SetLineStyle(kDotted);

  TH1F* h1prong0Pi0SS = (TH1F*)h1prong0Pi0->Clone("h1prong0Pi0SS");
  TH1F* h1prong1Pi0SS = (TH1F*)h1prong1Pi0->Clone("h1prong1Pi0SS");
  TH1F* h3prongSS     = (TH1F*)h3prong->Clone("h3prongSS");
  h1prong0Pi0SS->SetFillColor(42);
  h1prong1Pi0SS->SetFillColor(42);
  h3prongSS->SetFillColor(42);

  TH1F* h1prong0Pi0EWK = (TH1F*)h1prong0Pi0->Clone("h1prong0Pi0EWK");
  TH1F* h1prong1Pi0EWK = (TH1F*)h1prong1Pi0->Clone("h1prong1Pi0EWK");
  TH1F* h1prong1Pi0EWKUp   = (TH1F*)h1prong1Pi0->Clone("h1prong1Pi0EWKUp");
  TH1F* h1prong1Pi0EWKDown = (TH1F*)h1prong1Pi0->Clone("h1prong1Pi0EWKDown");
  TH1F* h3prongEWK     = (TH1F*)h3prong->Clone("h3prongEWK");
  TH1F* h3prongEWKUp       = (TH1F*)h3prong->Clone("h3prongEWKUp");
  TH1F* h3prongEWKDown     = (TH1F*)h3prong->Clone("h3prongEWKDown");
  h1prong0Pi0EWK->SetFillColor(kBlue);
  h1prong1Pi0EWK->SetFillColor(kBlue);
  h3prongEWK->SetFillColor(kBlue);

  float Lumi = 1061.;
  THStack* aStack1 = new THStack("aStack1",Form("CMS Preliminary 2011    #sqrt{s}=7 TeV L=%.2f fb^{-1}", Lumi/1000. ));
  THStack* aStack2 = new THStack("aStack2",Form("CMS Preliminary 2011    #sqrt{s}=7 TeV L=%.2f fb^{-1}", Lumi/1000. ));
  THStack* aStack3 = new THStack("aStack3",Form("CMS Preliminary 2011    #sqrt{s}=7 TeV L=%.2f fb^{-1}", Lumi/1000. ));

  TH1F* hWMt = new TH1F("hWMt","",200,-200,200);



  ///////////////////////////////////////////////////////////////////////////////////
  //


  data->Draw("visibleTauMass>>h1prong0Pi0Data",signalOS && c1prong0Pi0);
  backgroundDYTauTau->Draw("visibleTauMass>>h1prong0Pi0DYTauTau","puWeight*sampleWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>50 && diTauVisMass<70 && abs(genDecay)==(23*15) && decayMode==0 && numPV>5)");
  h1prong0Pi0DYTauTau->Scale(Lumi/1000.*(0.906/0.825)*0.9967*0.997*0.968);
  data->Draw("visibleTauMass>>h1prong0Pi0SS",signalSS && c1prong0Pi0);
  h1prong0Pi0SS->Scale(1.06);

  backgroundWJets->Draw("visibleTauMass>>h1prong0Pi0EWK","puWeight*sampleWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>50 && diTauVisMass<70 && decayMode==0 && numPV>5)");
  hWMt->Reset();
  backgroundWJets->Draw("(pZetaCorr-1.5*pZetaVisCorr)>>hWMt","puWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && HLTx && diTauVisMass>50 && diTauVisMass<70 && decayMode==0 && numPV>5)");
  float scaleFactor1 = (hWMt->Integral(0,80))/(hWMt->Integral(90,200));
  cout << "Conv. factor for W ==> " << scaleFactor1 << endl;
  float OSWinSignalRegion1 = data->GetEntries("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)<-40 && HLTx && diTauVisMass>50 && diTauVisMass<70 && decayMode==0 && numPV>5");
  OSWinSignalRegion1 *= (1./scaleFactor1);
  h1prong0Pi0EWK->Scale(OSWinSignalRegion1/h1prong0Pi0EWK->Integral());
  //h1prong0Pi0EWK->Scale(Lumi/1000.);

  //////////////////////////////////////////////////////////////////////////////////

  data->Draw("visibleTauMass>>h1prong1Pi0Data",signalOS && c1prong1Pi0);
  backgroundDYTauTau->Draw("visibleTauMass>>h1prong1Pi0DYTauTau","puWeight*sampleWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>50 && diTauVisMass<70 && abs(genDecay)==(23*15) && decayMode==1 && numPV>5)");
  h1prong1Pi0DYTauTau->Scale(Lumi/1000.*(0.906/0.825)*0.9967*0.997*0.968);
  backgroundDYTauTau->Draw("(1.02*visibleTauMass)>>h1prong1Pi0DYTauTauUp","puWeight*sampleWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>50 && diTauVisMass<70 && abs(genDecay)==(23*15) && decayMode==1 && numPV>5)");
  h1prong1Pi0DYTauTauUp->Scale(Lumi/1000.*(0.906/0.825)*0.9967*0.997*0.968);
  backgroundDYTauTau->Draw("(0.98*visibleTauMass)>>h1prong1Pi0DYTauTauDown","puWeight*sampleWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>50 && diTauVisMass<70 && abs(genDecay)==(23*15) && decayMode==1 && numPV>5)");
  h1prong1Pi0DYTauTauDown->Scale(Lumi/1000.*(0.906/0.825)*0.9967*0.997*0.968);

  //h1prong1Pi0DYTauTau->Draw("HIST");
  //h1prong1Pi0DYTauTauUp->Draw("SAMEHIST");
  //h1prong1Pi0DYTauTauDown->Draw("SAMEHIST");
  //return;


  data->Draw("visibleTauMass>>h1prong1Pi0SS",signalSS && c1prong1Pi0);
  h1prong1Pi0SS->Scale(1.06);

  backgroundWJets->Draw("visibleTauMass>>h1prong1Pi0EWK","puWeight*sampleWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>50 && diTauVisMass<70 && decayMode==1 && numPV>5)");
  backgroundWJets->Draw("(1.02*visibleTauMass)>>h1prong1Pi0EWKUp","puWeight*sampleWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>50 && diTauVisMass<70 && decayMode==1 && numPV>5)");
  backgroundWJets->Draw("(0.98*visibleTauMass)>>h1prong1Pi0EWKDown","puWeight*sampleWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>50 && diTauVisMass<70 && decayMode==1 && numPV>5)");
  hWMt->Reset();
  backgroundWJets->Draw("(pZetaCorr-1.5*pZetaVisCorr)>>hWMt","puWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && HLTx && diTauVisMass>50 && diTauVisMass<70 && decayMode==1 && numPV>5)");
  float scaleFactor1 = (hWMt->Integral(0,80))/(hWMt->Integral(90,200));
  cout << "Conv. factor for W ==> " << scaleFactor1 << endl;
  float OSWinSignalRegion1 = data->GetEntries("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)<-40 && HLTx && diTauVisMass>50 && diTauVisMass<70 && decayMode==1 && numPV>5");
  OSWinSignalRegion1 *= (1./scaleFactor1);
  h1prong1Pi0EWK->Scale(OSWinSignalRegion1/h1prong1Pi0EWK->Integral());
  h1prong1Pi0EWKUp->Scale(OSWinSignalRegion1/h1prong1Pi0EWKUp->Integral());
  h1prong1Pi0EWKDown->Scale(OSWinSignalRegion1/h1prong1Pi0EWKDown->Integral());
  //h1prong1Pi0EWK->Scale(Lumi/1000.);

  //h1prong1Pi0EWK->Draw("HIST");
  //h1prong1Pi0EWKUp->Draw("SAMEHIST");
  //h1prong1Pi0EWKDown->Draw("SAMEHIST");
  //return;
 
  //////////////////////////////////////////////////////////////////////////////////


  data->Draw("visibleTauMass>>h3prongData",signalOS && c3prong);
  backgroundDYTauTau->Draw("visibleTauMass>>h3prongDYTauTau","puWeight*sampleWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>50 && diTauVisMass<70 && abs(genDecay)==(23*15) && decayMode==2 && numPV>5)");
  h3prongDYTauTau->Scale(Lumi/1000.*(0.906/0.825)*0.9967*0.997*0.968);
  backgroundDYTauTau->Draw("(1.02*visibleTauMass)>>h3prongDYTauTauUp","puWeight*sampleWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>50 && diTauVisMass<70 && abs(genDecay)==(23*15) && decayMode==2 && numPV>5)");
  h3prongDYTauTauUp->Scale(Lumi/1000.*(0.906/0.825)*0.9967*0.997*0.968);
  backgroundDYTauTau->Draw("(0.98*visibleTauMass)>>h3prongDYTauTauDown","puWeight*sampleWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>50 && diTauVisMass<70 && abs(genDecay)==(23*15) && decayMode==2 && numPV>5)");
  h3prongDYTauTauDown->Scale(Lumi/1000.*(0.906/0.825)*0.9967*0.997*0.968);
  data->Draw("visibleTauMass>>h3prongSS",signalSS && c3prong);
  h3prongSS->Scale(1.06);

  backgroundWJets->Draw("visibleTauMass>>h3prongEWK","puWeight*sampleWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>50 && diTauVisMass<70 && decayMode==2 && numPV>5)");
  backgroundWJets->Draw("(1.02*visibleTauMass)>>h3prongEWKUp","puWeight*sampleWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>50 && diTauVisMass<70 && decayMode==2 && numPV>5)");
  backgroundWJets->Draw("(0.98*visibleTauMass)>>h3prongEWKDown","puWeight*sampleWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)>-20 && HLTx && diTauVisMass>50 && diTauVisMass<70 && decayMode==2 && numPV>5)");
  hWMt->Reset();
  backgroundWJets->Draw("(pZetaCorr-1.5*pZetaVisCorr)>>hWMt","puWeight*(tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && HLTx && diTauVisMass>50 && diTauVisMass<70 && decayMode==2 && numPV>5)");
  float scaleFactor1 = (hWMt->Integral(0,80))/(hWMt->Integral(90,200));
  cout << "Conv. factor for W ==> " << scaleFactor1 << endl;
  float OSWinSignalRegion1 = data->GetEntries("tightestHPSDBWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZetaCorr-1.5*pZetaVisCorr)<-40 && HLTx && diTauVisMass>50 && diTauVisMass<70 && decayMode==2 && numPV>5");
  OSWinSignalRegion1 *= (1./scaleFactor1);
  h3prongEWK->Scale(OSWinSignalRegion1/h3prongEWK->Integral());
  h3prongEWKUp->Scale(OSWinSignalRegion1/h3prongEWKUp->Integral());
  h3prongEWKDown->Scale(OSWinSignalRegion1/h3prongEWKDown->Integral());
  //h3prongEWK->Scale(Lumi/1000.);

  float n1prong0Pi0Norm = h1prong0Pi0SS->Integral()+h1prong0Pi0EWK->Integral()+h1prong0Pi0DYTauTau->Integral();
  float n1prong1Pi0Norm = h1prong1Pi0SS->Integral()+h1prong1Pi0EWK->Integral()+h1prong1Pi0DYTauTau->Integral();
  float n3prongNorm     = h3prongSS->Integral()+h3prongEWK->Integral()+h3prongDYTauTau->Integral();

  cout << "1prong0pi0 " << n1prong0Pi0Norm << " <==> Data: " << h1prong0Pi0Data->Integral() << endl;
  cout << "1prong1pi0 " << n1prong1Pi0Norm << " <==> Data: " << h1prong1Pi0Data->Integral() << endl;
  cout << "3prong "     << n3prongNorm << " <==> Data: " << h3prongData->Integral() << endl;

  if(normalize){
    h1prong0Pi0SS->Scale(1. / (n1prong0Pi0Norm) );
    h1prong0Pi0EWK->Scale(1./ (n1prong0Pi0Norm));
    h1prong0Pi0DYTauTau->Scale(1./ (n1prong0Pi0Norm));

    //
 
    h1prong1Pi0SS->Scale(1./ (n1prong1Pi0Norm));
    h1prong1Pi0EWK->Scale(1./ (n1prong1Pi0Norm));
    h1prong1Pi0DYTauTau->Scale(1./ (n1prong1Pi0Norm));
    
    h1prong1Pi0EWKUp->Scale(1./ (n1prong1Pi0Norm));
    h1prong1Pi0DYTauTauUp->Scale(1./ (n1prong1Pi0Norm));
    h1prong1Pi0EWKDown->Scale(1./ (n1prong1Pi0Norm));
    h1prong1Pi0DYTauTauDown->Scale(1./ (n1prong1Pi0Norm));

    //

    h3prongSS->Scale(1./ (n3prongNorm));
    h3prongEWK->Scale(1./ (n3prongNorm));
    h3prongDYTauTau->Scale(1./ (n3prongNorm));

    h3prongEWKUp->Scale(1./ (n3prongNorm));
    h3prongDYTauTauUp->Scale(1./ (n3prongNorm));
    h3prongEWKDown->Scale(1./ (n3prongNorm));
    h3prongDYTauTauDown->Scale(1./ (n3prongNorm));

  }

  aStack1->Add(h1prong0Pi0SS);
  aStack1->Add(h1prong0Pi0EWK);
  aStack1->Add(h1prong0Pi0DYTauTau);

  aStack2->Add(h1prong1Pi0SS);
  aStack2->Add(h1prong1Pi0EWK);
  aStack2->Add(h1prong1Pi0DYTauTau);

  aStack3->Add(h3prongSS);
  aStack3->Add(h3prongEWK);
  aStack3->Add(h3prongDYTauTau);


  if(do1prong0pi0){
    h1prong0Pi0Data->Sumw2();
    if(normalize) h1prong0Pi0Data->DrawNormalized("P");
    else h1prong0Pi0Data->Draw("P");
    aStack1->Draw("HISTSAME");
    leg->SetHeader("1prong+0#pi^{0} #vtx>5");
    leg->AddEntry(h1prong0Pi0Data,"Observed","P");
    leg->AddEntry(h1prong0Pi0DYTauTau,"Z#rightarrow#tau#tau","F");
    leg->AddEntry(h1prong0Pi0SS,"QCD","F");
    leg->AddEntry(h1prong0Pi0EWK,"ElectroWeak","F");
    leg->Draw();
  }
  if(do1prong1pi0){
    h1prong1Pi0DYTauTauUp->Add(h1prong1Pi0EWKUp,1);
    h1prong1Pi0DYTauTauUp->Add(h1prong1Pi0SS,1);

    h1prong1Pi0DYTauTauDown->Add(h1prong1Pi0EWKDown,1);
    h1prong1Pi0DYTauTauDown->Add(h1prong1Pi0SS,1);

    h1prong1Pi0Data->Sumw2();

    //h1prong1Pi0Data->SetAxisRange(0,0.35,"Y");

    if(normalize) h1prong1Pi0Data->DrawNormalized("P");
    else h1prong1Pi0Data->Draw("P");
    aStack2->Draw("HISTSAME");
    if(normalize){
      h1prong1Pi0DYTauTauUp->DrawNormalized("HISTSAME");
      h1prong1Pi0DYTauTauDown->DrawNormalized("HISTSAME");
    }
    else{
      h1prong1Pi0DYTauTauUp->Draw("HISTSAME");
      h1prong1Pi0DYTauTauDown->Draw("HISTSAME");
    }
    leg->SetHeader("1prong+#pi^{0}s #vtx>5");
    leg->AddEntry(h1prong1Pi0Data,"Observed","P");
    leg->AddEntry(h1prong1Pi0DYTauTau,"Z#rightarrow#tau#tau","F");
    leg->AddEntry(h1prong1Pi0SS,"QCD","F");
    leg->AddEntry(h1prong1Pi0EWK,"ElectroWeak","F");
    leg->AddEntry(h1prong1Pi0DYTauTauUp,"TauES+2%","L");
    leg->AddEntry(h1prong1Pi0DYTauTauDown,"TauES-2%","L");
    leg->Draw();
  }
  if(do3prong){

    h3prongDYTauTauUp->Add(h3prongEWKUp,1);
    h3prongDYTauTauUp->Add(h3prongSS,1);

    h3prongDYTauTauDown->Add(h3prongEWKDown,1);
    h3prongDYTauTauDown->Add(h3prongSS,1);

    h3prongData->Sumw2();

    //h3prongData->SetAxisRange(0,h3prongDYTauTauUp->GetMaximum()*1.1,"Y");

    if(normalize) h3prongData->DrawNormalized("P");
    else h3prongData->Draw("P");
    aStack3->Draw("HISTSAME");
    if(normalize){
      h3prongDYTauTauUp->DrawNormalized("HISTSAME");
      h3prongDYTauTauDown->DrawNormalized("HISTSAME");
    }
    else{
      h3prongDYTauTauUp->Draw("HISTSAME");
      h3prongDYTauTauDown->Draw("HISTSAME");
    }
    leg->SetHeader("3prong #vtx>5");
    leg->AddEntry(h3prongData,"Observed","P");
    leg->AddEntry(h3prongDYTauTau,"Z#rightarrow#tau#tau","F");
    leg->AddEntry(h3prongSS,"QCD","F");
    leg->AddEntry(h3prongEWK,"ElectroWeak","F");
    leg->AddEntry(h3prongDYTauTauUp,"TauES+2%","L");
    leg->AddEntry(h3prongDYTauTauDown,"TauES-2%","L");
    leg->Draw();
  }

  return;

}
