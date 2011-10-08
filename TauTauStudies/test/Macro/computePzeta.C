

{

  TFile *fElec = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011_iter2/Inclusive/nTupleWJets-madgraph-PUS1_Open_ElecTauStream.root","READ");
  TFile *fMu = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleWJets-Mu-madgraph-PUS1_Open_MuTauStream.root","READ");

  TTree* treeElec = (TTree*)fElec->Get("outTreePtOrd");
  TTree* treeMu = (TTree*)fMu->Get("outTreePtOrd");

  TH1F* h1 = new TH1F("h1","",40,-200,200);
  TH1F* h2 = new TH1F("h2","",40,-200,200);

  treeElec->Draw("(0.0*pZeta-1.0*pZetaVis)>>h1","puWeight*(pt1>25 && pt2>20 && eta1*eta2<0)");
  //treeElec->Draw("(1.00*pZeta-1.5*pZetaVis)>>h1","pt1>25 && pt2>20 && eta1*eta2<0");
  treeMu->Draw("(0.0*pZeta-1.0*pZetaVis)>>h2","puWeight*(pt1>25 && pt2>20 && eta1*eta2<0)");
  //treeMu->Draw("(1.0*pZeta-1.5*pZetaVis)>>h2","pt1>25 && pt2>20 && eta1*eta2<0");

  h1->SetMarkerColor(kRed);
  h1->SetMarkerStyle(kOpenCircle);
  h1->Sumw2();
  h1->SetMarkerSize(1.8);
  h2->SetLineColor(kBlue);

  h1->DrawNormalized("P");
  h2->DrawNormalized("HISTSAME");


}
