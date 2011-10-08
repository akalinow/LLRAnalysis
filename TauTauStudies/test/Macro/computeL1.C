

{

  TCanvas *c0 = new TCanvas("c0","",5,30,650,600);
  c0->SetGrid(0,0);
  c0->SetFillStyle(4000);
  c0->SetFillColor(10);
  c0->SetTicky();
  c0->SetObjectStat(0);
  c0->SetLogy(1);

  TFile *f = new TFile("../prod/ZplusJets_iter2/treeZmumuPlusJets_Run2011-Mu-Zjets.root");

  TTree* tree = (TTree*)f->Get("zPlusJetsAnalyzer/tree");

  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >  *jets1 = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >  *jets2 = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  TH1F* h1 = new TH1F("h1",";Exclusive number of jets Pt>30 GeV; events",10,0,10);
  TH1F* h2 = new TH1F("h2","",10,0,10);

  TH1F* h1_ = new TH1F("h1_",";#eta; events",20,0,10);
  TH1F* h2_ = new TH1F("h2_","",20,0,10);

  tree->SetBranchAddress("jetsIDP4", &jets1);
  tree->SetBranchAddress("jetsIDL1OffsetP4", &jets2);

  TBranch *brjetsIDP4             = tree->GetBranch("jetsIDP4");
  TBranch *brjetsIDL1OffsetP4     = tree->GetBranch("jetsIDL1OffsetP4");

  int nEntries = tree->GetEntries();
  for(unsigned n = 0 ; n< nEntries ; n++){
    brjetsIDP4->GetEntry(n);
    brjetsIDL1OffsetP4->GetEntry(n);

    int count1= 0;
    for(int i = 0; i < jets1->size(); i++){
      if(((*jets1)[i]).Pt()>30) count1++;
    }
    int count2= 0;
    for(int i = 0; i < jets2->size(); i++){
      if(((*jets2)[i]).Pt()>30) count2++;
    }

    h1->Fill(count1);
    h2->Fill(count2);

    if(jets1->size()>1 && ((*jets1)[0]).Pt()>30 && ((*jets1)[1]).Pt()>30 ) 
      //h1_->Fill( fabs( ((*jets1)[0]).Eta()-((*jets1)[1]).Eta() )  );
      h1_->Fill( fabs(((*jets1)[1]).Eta()) );

    if(jets2->size()>1 && ((*jets2)[0]).Pt()>30 && ((*jets2)[1]).Pt()>30 ) 
      //h2_->Fill( fabs( ((*jets2)[0]).Eta()-((*jets2)[1]).Eta() )  );
      h2_->Fill( fabs(((*jets2)[1]).Eta()) );
  }

  h1->Sumw2();
  h2->Sumw2();

  h1->SetLineColor(kRed);
  h2->SetMarkerColor(kBlack);
  h2->SetMarkerStyle(kOpenCircle);
  h2->SetMarkerSize(1.1);

  h1_->Sumw2();
  h2_->Sumw2();

  h1_->SetLineColor(kRed);
  h2_->SetMarkerColor(kBlack);
  h2_->SetMarkerStyle(kOpenCircle);
  h2_->SetMarkerSize(1.1);

  //h1->Draw("HISTE");
  //h2->Draw("PSAME");

  h1_->Draw("HISTE");
  h2_->Draw("PSAME");

  TLegend* leg = new TLegend(0.55,0.68,0.85,0.88,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);

  leg->SetHeader("Data from DoubleMu");
  leg->AddEntry(h1_,"L1FastJet+L2L3");
  leg->AddEntry(h2_,"L1Offset+L2L3");

  leg->Draw();

}
