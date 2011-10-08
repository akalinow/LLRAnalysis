


{

  using namespace std;
  using namespace RooFit;
  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;

  // pZeta distribution from Z+jets MC
  TFile *fZ = new TFile("/data_CMS/cms/lbianchini/MuTauStream2011_iter2/treeMuTauStream_DYJets-Mu-50-madgraph-PUS1.root");

  if(fZ->IsZombie()) exit(0);

  TTree* tZIncl = (TTree*)fZ->Get("muTauStreamAnalyzer/tree");

  TFile* dummy1 = new TFile("dummy1.root","RECREATE");
  //TTree* tZ0j   = (TTree*)tZIncl->CopyTree("muFlag==1 && ( jetsIDP4@.size()==0 || (jetsIDP4@.size()>0 && jetsIDP4[0].Pt()<30))"); 
  TTree* tZ1j   = (TTree*)tZIncl->CopyTree("muFlag==1 && jetsIDP4@.size()>0 && jetsIDP4[0].Pt()>30"); 
  //TTree* tZ2j   = (TTree*)tZIncl->CopyTree("muFlag==1 && jetsIDP4@.size()>1 && jetsIDP4[0].Pt()>30 && jetsIDP4[1].Pt()>30 && jetsIDP4[0].Eta()*jetsIDP4[1].Eta()<0 && (jetsIDP4[0].Eta()-jetsIDP4[1].Eta())>3.5"); 


 
  float puWeight;
  tZ1j->SetBranchAddress("mcPUweight",&puWeight);
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* extraMuons = new std::vector<  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  tZ1j->SetBranchAddress("extraMuons",&extraMuons);
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* MET        = new std::vector<  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  tZ1j->SetBranchAddress("METP4",&MET);
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauLegs = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  tZ1j->SetBranchAddress("diTauLegsP4",&diTauLegs);

  TBranch* brextraMuons = tZ1j->GetBranch("extraMuons");
  TBranch* brMET = tZ1j->GetBranch("METP4");
  TBranch* brpuWeight = tZ1j->GetBranch("mcPUweight");
  TBranch* brdiTauLegs = tZ1j->GetBranch("diTauLegsP4");

  Int_t nEntries = tZ1j->GetEntries();

  TH1F* hZ1j = new TH1F("hZ1j","",60,-200,100);

  for(int n = 0; n<nEntries ; n++){
   
    if(n%1000==0) cout << n << endl;

    brextraMuons->GetEntry(n);
    brMET->GetEntry(n);
    brdiTauLegs->GetEntry(n);
    brpuWeight->GetEntry(n);

    float cos1 = ((*diTauLegs)[0]).Px() / ((*diTauLegs)[0]).Pt();
    float cos2 = ((*diTauLegs)[1]).Px() / ((*diTauLegs)[1]).Pt();
    float sen1 = ((*diTauLegs)[0]).Py() / ((*diTauLegs)[0]).Pt();
    float sen2 = ((*diTauLegs)[1]).Py() / ((*diTauLegs)[1]).Pt();
    
    float bisecX = cos1 + cos2;
    float bisecY = sen1 + sen2;
    
    float norm = TMath::Sqrt(bisecX*bisecX + bisecY*bisecY);
    
    if(norm>0.){
      bisecX /= norm;
      bisecY /= norm;
    }
    
    float pZetaVis = ((*diTauLegs)[0]+(*diTauLegs)[1]).Px()*bisecX + ((*diTauLegs)[0]+(*diTauLegs)[1]).Py()*bisecY;
    float pZeta    = ((*diTauLegs)[0]+(*diTauLegs)[1]+( (*extraMuons)[0]+(*MET)[0] )).Px()*bisecX + ((*diTauLegs)[0]+(*diTauLegs)[1]+( (*extraMuons)[0]+(*MET)[0] )).Py()*bisecY;

    hZ1j->Fill(pZeta-1.5*pZetaVis, puWeight );

    cos1 = ((*extraMuons)[0]).Px()/ ((*extraMuons)[0]).Pt();
    cos2 = ((*diTauLegs)[1]).Px()/((*diTauLegs)[1]).Pt();
    sen1 = ((*extraMuons)[0]).Py()/((*extraMuons)[0]).Pt();
    sen2 = ((*diTauLegs)[1]).Py()/((*diTauLegs)[1]).Pt();
    
    bisecX = cos1 + cos2;
    bisecY = sen1 + sen2;
    
    norm = TMath::Sqrt(bisecX*bisecX + bisecY*bisecY);
    
    if(norm>0.){
      bisecX /= norm;
      bisecY /= norm;
    }
    
    pZetaVis = ((*extraMuons)[0]+(*diTauLegs)[1]).Px()*bisecX + ((*extraMuons)[0]+(*diTauLegs)[1]).Py()*bisecY;
    pZeta    = ((*extraMuons)[0]+(*diTauLegs)[1]+( (*diTauLegs)[0]+(*MET)[0] )).Px()*bisecX + ((*extraMuons)[0]+(*diTauLegs)[1]+( (*diTauLegs)[0]+(*MET)[0] )).Py()*bisecY;
    
    hZ1j->Fill(pZeta-1.5*pZetaVis, puWeight );

  }

  //RooRealVar pZeta("pZeta","",-200,200);
  //RooRealVar pZetaVis("pZetaVis","",0,200);
  //RooFormulaVar pZ("pZ","pZeta-1.5*pZetaVis",RooArgSet(pZeta,pZetaVis));
  RooRealVar pZ("pZ"," p_{#zeta} (GeV)",-200,200);

  RooDataHist pZHist("pZHist","",RooArgSet(pZ),Import(*hZ1j));

  RooRealVar m1("m1","",-50,-100,0);
  RooRealVar s1("s1","",50,0,200);
  RooGaussian g1("g1","",pZ,m1,s1);

  RooRealVar m2("m2","",-20,-100,0);
  RooRealVar s2("s2","",50,0,100);
  RooGaussian g2("g2","",pZ,m2,s2);

  RooRealVar m3("m3","",-30,-100,100);
  RooRealVar s3("s3","",10,0,50);
  RooGaussian g3("g3","",pZ,m3,s3);

  RooRealVar m4("m4","",-40,-100,20);
  RooRealVar s4("s4","",10,0,50);
  RooGaussian g4("g4","",pZ,m4,s4);

  RooRealVar n1("n1","",100,0,100000);
  RooRealVar n2("n2","",100,0,100000);
  RooRealVar n3("n3","",100,0,100000);
  RooRealVar n4("n4","",100,0,100000);

  RooRealVar f1("f1","",0.5,0,1);
  RooRealVar f2("f2","",0.5,0,1);


  RooAddPdf g("g","",RooArgList(g1,g2 /*,g3*/ ), /*RooArgList(n1,n2,n3)*/ RooArgList(f1 /*,f2*/ ) );
  g.fitTo(pZHist, Extended(0), Minos(1), Save(1), NumCPU(4), SumW2Error(kTRUE));

  RooPlot* plot = pZ.frame(Bins(20),Title("Template Z MC"));
  pZHist.plotOn(plot);
  g.plotOn(plot);
  g.plotOn(plot, Components("g1"), LineColor(kRed));
  g.plotOn(plot, Components("g2"), LineColor(kGreen));
  //g.plotOn(plot, Components("g3"), LineColor(kMagenta));
  //g.plotOn(plot, Components("g4"), LineColor(kYellow));
  plot->Draw();









  /*
  hW0j->SetLineColor(kRed);
  hW0j->SetLineStyle(kDashed);
  hW0j->SetLineWidth(3);

  hW1j->SetLineColor(kBlue);
  hW1j->SetLineStyle(kDotted);
  hW1j->SetLineWidth(3);

  hW2j->SetLineColor(kMagenta);
  hW2j->SetLineStyle(kSolid);
  hW2j->SetLineWidth(3);

  hW0j->Sumw2();
  hW1j->Sumw2();
  hW2j->Sumw2();

  hW0j->DrawNormalized("HIST");
  hW1j->DrawNormalized("EHISTSAME");
  hW2j->DrawNormalized("EHISTSAME");
  */



}





/*


   1  f1           8.27694e-02   2.30059e-02  -1.98428e-02   3.93269e-02
   2  f2           3.16396e-01   3.82213e-02  -3.91087e-02   4.34298e-02
   3  m1          -8.47833e+01   1.39506e+00  -1.36602e+00   1.46514e+00
   4  m2          -4.68467e+01   1.17304e+00  -1.15148e+00   1.26153e+00
   5  m3          -5.61363e+01   1.23174e+00  -1.10136e+00   2.03514e+00
   6  s1           1.48629e+01   1.65094e+00  -1.68552e+00   1.98132e+00
   7  s2           4.81733e+01   1.23049e+00  -1.21833e+00   1.34606e+00
   8  s3           2.82433e+01   9.50554e-01  -1.44777e+00   9.13702e-01


*/
