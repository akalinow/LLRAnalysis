

{


  TFile fIncl("histograms/muTau_mH120_novbf__diTauVisMass.root");
  TFile fTwoJ("histograms/muTau_mH120_twoJets__diTauVisMass.root");
  TFile fVbf("histograms/muTau_mH120_oneJet__diTauVisMass.root");

  TString sample="QCD";

  TH1F* hIncl = (TH1F*)fIncl.Get("h"+sample);
  TH1F* hTwoJ = (TH1F*)fTwoJ.Get("h"+sample);
  TH1F* hVbf  = (TH1F*)fVbf.Get("h"+sample);

  hIncl->SetLineColor(kRed);
  hTwoJ->SetLineColor(kBlue);
  hVbf->SetLineColor(kBlack);

  hIncl->Sumw2();
  hTwoJ->Sumw2();
  hVbf->Sumw2();

  cout << hIncl->GetEntries() << endl;
  cout << hTwoJ->GetEntries() << endl;

  hVbf->DrawNormalized("HIST");
  hTwoJ->DrawNormalized("ESAME");
  //hIncl->DrawNormalized("HIST");

}
