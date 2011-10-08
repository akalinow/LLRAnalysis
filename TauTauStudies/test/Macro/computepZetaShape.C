


{

  using namespace std;
  using namespace RooFit;

  // pZeta distribution from W+jets MC
  TFile *fW = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleWJets-Mu-madgraph-PUS1_Open_MuTauStream.root");
  TTree* tWIncl = (TTree*)fW->Get("outTreePtOrd");

  TFile* dummy1 = new TFile("dummy1.root","RECREATE");
  TTree* tW0j   = (TTree*)tWIncl->CopyTree("pt1<30"); 
  TTree* tW1j   = (TTree*)tWIncl->CopyTree("pt1>30 && pt2<30"); 
  TTree* tW2j   = (TTree*)tWIncl->CopyTree("pt1>30 && pt2>30 && Deta>3.5 && eta1*eta2<0 && Mjj>450 && ptVeto<30"); 

  TH1F* hW0j = new TH1F("hW0j","",60,-200,100);
  TH1F* hW1j = new TH1F("hW1j","",60,-200,100);
  TH1F* hW2j = new TH1F("hW2j","",60,-200,100);

  tW0j->Draw("(pZeta-1.5*pZetaVis)>>hW0j","puWeight");
  tW1j->Draw("(pZeta-1.5*pZetaVis)>>hW1j","puWeight");
  tW2j->Draw("(pZeta-1.5*pZetaVis)>>hW2j","puWeight");

  RooRealVar pZeta("pZeta","",-200,200);
  RooRealVar pZetaVis("pZetaVis","",0,200);
  //RooFormulaVar pZ("pZ","pZeta-1.5*pZetaVis",RooArgSet(pZeta,pZetaVis));
  RooRealVar pZ("pZ"," p_{#zeta} (GeV)",-200,200);

  RooDataHist pZHist("pZHist","",RooArgSet(pZ),Import(*hW1j));

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


  RooAddPdf g("g","",RooArgList(g1,g2,g3),/*RooArgList(n1,n2,n3)*/ RooArgList(f1,f2));
  g.fitTo(pZHist, Extended(0), Minos(1), Save(1), NumCPU(4), SumW2Error(kTRUE));

  RooPlot* plot = pZ.frame(Bins(20),Title("Template W MC"));
  pZHist.plotOn(plot);
  g.plotOn(plot);
  g.plotOn(plot, Components("g1"), LineColor(kRed));
  g.plotOn(plot, Components("g2"), LineColor(kGreen));
  g.plotOn(plot, Components("g3"), LineColor(kMagenta));
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
