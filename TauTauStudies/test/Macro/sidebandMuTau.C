{

  // Open the files
  TFile *fData              
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_06Dec2011_v3//nTupleRun2011-MuTau-All_run_Open_MuTauStream.root", "READ"); 
  TFile *fDataLooseIso              
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_06Dec2011_v3//nTupleRun2011-MuTau-LooseIso-All_run_Open_MuTauStream.root", "READ"); 
  TFile *fBackgroundDY
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_06Dec2011_v3//nTupleDYJets-MuTau-50-madgraph-PUS6_run_Open_MuTauStream.root","READ"); 
  TFile *fBackgroundWJets   
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_06Dec2011_v3//nTupleWJets-MuTau-madgraph-PUS6_run_Open_MuTauStream.root","READ"); 
  TFile *fBackgroundTTbar  
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_06Dec2011_v3//nTupleTTJets-MuTau-madgraph-PUS6_run_Open_MuTauStream.root","READ"); 
  TFile *fBackgroundOthers  
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_06Dec2011_v3//nTupleOthers-MuTau-PUS6_run_Open_MuTauStream.root","READ");
  int mH_ = 120;
  TFile *fSignalVBF         
    //= new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_06Dec2011_v3//nTupleVBFH%d-MuTau-powheg-PUS6_run_Open_MuTauStream.root",mH_) ,"READ"); 
    = new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_06Dec2011_v3//nTupleGGFH%d-MuTau-powheg-PUS6_run_Open_MuTauStream.root",mH_) ,"READ"); 
 
  TString tree = "outTreePtOrd";

  TTree *data                = (TTree*)fData->Get("outTreePtOrd");
  TTree *dataLooseIso        = (TTree*)fDataLooseIso->Get("outTreePtOrd");
  TTree *backgroundDYTauTau  = (TTree*)fBackgroundDY->Get(tree);
  TTree *backgroundTTbar     = (TTree*)fBackgroundTTbar->Get(tree);
  TTree *backgroundWJets     = (TTree*)fBackgroundWJets->Get(tree);
  TTree *backgroundOthers    = (TTree*)fBackgroundOthers->Get(tree);
  TTree *signalVBF           = (TTree*)fSignalVBF->Get(tree);

  TCut lpt("ptL1>17 && abs(dz1)<0.2 && (pZetaCorr-1.5*pZetaVisCorr)<999 && MtLeg1Corr>60 && MEtCorr>-999");
  TCut tpt("ptL2>20");
  TCut tiso("tightestHPSDBWP>0 && !(decayMode==0 && (HoP+EoP)<0.2)");
  TCut liso("combRelIsoLeg1DBeta<0.10");
  TCut laiso("combRelIsoLeg1DBeta>0.30 && combRelIsoLeg1DBeta<0.50");
  TCut lliso("combRelIsoLeg1DBeta<0.30");
  TCut lveto("muFlag==0");
  TCut SS("diTauCharge!=0");
  TCut OS("diTauCharge==0");
  TCut oneJet("nJets30>=1");
  TCut twoJets("nJets30>=2");
  TCut vbf("pt1>30 && pt2>30 && eta1*eta2<0 && Mjj>400 && Deta>4.0 && isVetoInJets!=1");
  TCut novbf("pt1<150 && pt2<30");
  TCut boost("pt1>150 && !(pt2>30 && eta1*eta2<0 && Mjj>400 && Deta>4.0 && isVetoInJets!=1)");
  TCut hltevent("HLTx==1 && (run>=163269 || run==1)");
  TCut hltmatch("HLTmatch==1");
  TCut bTag("nJets30<=1 && nJets20BTagged>=1");
  TCut nobTag("nJets20BTagged==0");
  TCut antibTag("nJets20BTagged>=1");

  TCut sbin          =  lpt && tpt && tiso && liso && lveto && OS && hltevent && hltmatch && vbf && antibTag;
  TCut sbinSS        =  lpt && tpt && tiso && liso && lveto && SS && hltevent && hltmatch && vbf && antibTag;
  TCut sbinAntiIsoOS =  lpt && tpt && tiso && laiso && lveto && OS && hltevent && hltmatch && vbf && antibTag;
  TCut sbinAntiIsoSS =  lpt && tpt && tiso && laiso && lveto && SS && hltevent && hltmatch && vbf && antibTag;

  int nBins  =   10;
  float xMin =    0;
  float xMax =  200;

  THStack* aStack = new THStack("aStack","");

  TH1F* hZTT = new TH1F("hZTT","",nBins,xMin,xMax);
  hZTT->SetFillColor(kYellow);
  backgroundDYTauTau->Draw("MtLeg1Corr>>hZTT","puWeight*sampleWeight*HLTweightTau*HLTweightMu*SFMu"*sbin);
  hZTT->Scale(4.6);
  aStack->Add(hZTT);
  if(hZTT->Integral()>0){
  cout << "ZTT = " << hZTT->Integral() << "  (" << hZTT->GetEntries() << ") ==> " << hZTT->Integral() << "\\pm" <<
    TMath::Sqrt(hZTT->GetEntries())/hZTT->GetEntries()*hZTT->Integral()  << endl;
  }
  else 
    cout << "ZTT = " << hZTT->Integral() << endl;

  TH1F* hW = new TH1F("hW","",nBins,xMin,xMax);
  hW->SetFillColor(kBlue);
  backgroundWJets->Draw("MtLeg1Corr>>hW","puWeight*sampleWeight*HLTweightTau*HLTweightMu*SFMu"*sbin);
  hW->Scale(4.6);
  aStack->Add(hW);
  if(hW->Integral()>0){
    cout << "W = " << hW->Integral() << "  (" << hW->GetEntries() << ") ==> " << hW->Integral() << "\\pm" <<
      TMath::Sqrt(hW->GetEntries())/hW->GetEntries()*hW->Integral()  << endl;
  }
  else
    cout << "W = " << hW->Integral() << endl;

  TH1F* hWSS = new TH1F("hWSS","",nBins,xMin,xMax);
  backgroundWJets->Draw("MtLeg1Corr>>hWSS","puWeight*sampleWeight*HLTweightTau*HLTweightMu*SFMu"*sbinSS);
  hWSS->Scale(4.6);
  if(hWSS->Integral()>0){
    cout << "W SS= " << hWSS->Integral() << "  (" << hWSS->GetEntries() << ") ==> " << hWSS->Integral() << "\\pm" <<
      TMath::Sqrt(hWSS->GetEntries())/hWSS->GetEntries()*hWSS->Integral()  << endl;
  }
  else
     cout << "WSS = " << hWSS->Integral() << endl;

  TH1F* hTT = new TH1F("hTT","",nBins,xMin,xMax);
  hTT->SetFillColor(kMagenta);
  backgroundTTbar->Draw("MtLeg1Corr>>hTT","puWeight*sampleWeight*HLTweightTau*HLTweightMu*SFMu"*sbin);
  hTT->Scale(4.6);
  aStack->Add(hTT);
  if(hTT->Integral()>0){
    cout << "TT = " << hTT->Integral() << "  (" << hTT->GetEntries() << ") ==> " << hTT->Integral() << "\\pm" <<
      TMath::Sqrt(hTT->GetEntries())/hTT->GetEntries()*hTT->Integral()  << endl;
  }
  else{
    cout << "TT = " << hTT->Integral() << endl;
  }


  TH1F* hOthers = new TH1F("hOthers","",nBins,xMin,xMax);
  hOthers->SetFillColor(kMagenta);
  backgroundOthers->Draw("MtLeg1Corr>>hOthers","puWeight*sampleWeight*HLTweightTau*HLTweightMu*SFMu"*sbin);
  hOthers->Scale(4.6);
  cout << "Others = " << hOthers->Integral() << "  (" << hOthers->GetEntries() << ")" << endl;
  hTT->Add(hOthers,1.0);
  cout << "TT + Others = " << hTT->Integral() << endl;

  TH1F* hTTSS = new TH1F("hTTSS","",nBins,xMin,xMax);
  backgroundTTbar->Draw("MtLeg1Corr>>hTTSS","puWeight*sampleWeight*HLTweightTau*HLTweightMu*SFMu"*sbinSS);
  hTTSS->Scale(4.6);
  cout << "TT SS = " << hTTSS->Integral() << endl;

  TH1F* hVBF = new TH1F("hVBF","",nBins,xMin,xMax);
  hTT->SetFillColor(kMagenta);
  signalVBF->Draw("MtLeg1Corr>>hVBF","puWeight*sampleWeight*HLTweightTau*HLTweightMu*SFMu"*sbin);
  hVBF->SetLineWidth(3);
  hVBF->Scale(4.6*5);
  if(hVBF->Integral()>0){
    cout << "VBF = " << hVBF->Integral()/5. << "  (" << hVBF->GetEntries() << ") ==> " << hVBF->Integral()/5. << "\\pm" <<
      TMath::Sqrt(hVBF->GetEntries())/hVBF->GetEntries()*(hVBF->Integral()/5.)  << endl;
  }
  else
    cout << "VBF = " << hVBF->Integral()/5. << endl;


  TH1F* hData = new TH1F("hData"," ;  M_{T} (GeV) ; Events",nBins,xMin,xMax);
  hData->SetMarkerStyle(kFullCircle);
  data->Draw("MtLeg1Corr>>hData",sbin);
  hData->Sumw2();
  cout << "Data = " << hData->Integral() << endl;

  TH1F* hDataSS = new TH1F("hDataSS","",nBins,xMin,xMax);
  data->Draw("MtLeg1Corr>>hDataSS",sbinSS);
  cout << "Data SS = " << hDataSS->Integral() << endl;

  TLegend* leg = new TLegend(0.63,0.48,0.85,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.03);

  leg->SetHeader(Form("#splitline{CMS Preliminary #sqrt{s}=7 TeV}{%.1f fb^{-1} #tau_{#mu}#tau_{had}}", 4.6 ));
  leg->AddEntry(hData,"Observed","P");
  leg->AddEntry(hZTT,"Z#rightarrow ll","F");
  leg->AddEntry(hW,"W+jets","F");
  leg->AddEntry(hTT,"t#bar{t}","F");

  hData->Draw("P");
  aStack->Draw("SAME");
  //hVBF->Draw("SAME");
  hData->Draw("PSAME");

  leg->Draw();

  cout << "SS anti-iso = " << dataLooseIso->GetEntries(sbinAntiIsoSS) << " ; OS anti-iso = " << dataLooseIso->GetEntries(sbinAntiIsoOS) << endl; 

}
