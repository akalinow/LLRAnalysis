

{

  vector<TString> SM;
  vector<TString> MEt;
  vector<int> mH;

  MEt.push_back("");
  //MEt.push_back("Cuts");
  //MEt.push_back("MVA");
  //MEt.push_back("Cuts25");

  SM.push_back("muTau_SM0");
  SM.push_back("muTau_SM1");
  SM.push_back("muTau_SM2");

  //mH.push_back(110);
  //mH.push_back(115);
  mH.push_back(120);
  //mH.push_back(125);
  //mH.push_back(130);
  //mH.push_back(135);
  //mH.push_back(140);
  //mH.push_back(145);


  TFile* file = new TFile("muTauStudy.root","RECREATE");

  for(int i = 0 ; i < MEt.size() ; i++){

    for(int j = 0 ; j < SM.size() ; j++){

      TH1F* h     = new TH1F("h"+MEt[i]+"_"+SM[j], "" , 3, 0,3);
      TH1F* hTrue = new TH1F("hTrue"+MEt[i]+"_"+SM[j], "" , 3, 0,3);
 
      TFile f("muTauSM_diTauSVFitMass"+MEt[i]+".root");

      for(int k = 0 ; k < mH.size() ; k++){

	f.cd(SM[j]);
	
	TH1F* data_obs = (TH1F*)gDirectory->Get("data_obs");
	TH1F* ZTT    = (TH1F*)gDirectory->Get("ZTT");
	TH1F* SM120  = (TH1F*)gDirectory->Get(Form("SM%d", mH[k]));
	TH1F* VBF120 = (TH1F*)gDirectory->Get(Form("VBF%d",mH[k]));
	TH1F* QCD    = (TH1F*)gDirectory->Get("QCD");
	TH1F* W      = (TH1F*)gDirectory->Get("W");
	//TH1F* ZL     = (TH1F*)gDirectory->Get("ZL");
	//TH1F* ZJ     = (TH1F*)gDirectory->Get("ZJ");
	TH1F* ZLL    = (TH1F*)gDirectory->Get("ZLL");
	TH1F* TT     = (TH1F*)gDirectory->Get("TT");
	TH1F* VV     = (TH1F*)gDirectory->Get("VV");
	
      
	THStack* aStack = new THStack("aStack","");
	TH1F* all = (TH1F*)ZTT->Clone("all");
	all->Reset();
	all->SetTitle("#mu+#tau: "+SM[j]+" for "+MEt[i]);
	
	all->Add(ZTT,1.0);
	all->Add(QCD,1.0);
	all->Add(W,1.0);
	//all->Add(ZL,1.0);
	//all->Add(ZJ,1.0);
	all->Add(ZLL,1.0);
       	all->Add(TT,1.0);
	all->Add(VV,1.0);

	
	QCD->SetFillColor(kMagenta-9);
	aStack->Add(QCD);
	W->SetFillColor(kRed-3);
	aStack->Add(W);
	//ZL->SetFillColor(kRed-3);
	//ZL->SetLineColor(kRed-3);
	//aStack->Add(ZL);
	//ZJ->SetFillColor(kRed-3);
	//ZJ->SetLineColor(kRed-3);
	//aStack->Add(ZJ);

	ZLL->SetFillColor(kRed-3);
	ZLL->SetLineColor(kRed-3);
	aStack->Add(ZLL);

	VV->SetFillColor(kRed-3);
	aStack->Add(VV);
	TT->SetFillColor(kBlue-2);
	aStack->Add(TT);
	ZTT->SetFillColor(kYellow-9);
	aStack->Add(ZTT);
	

	all->SetLineWidth(1);
	all->SetLineColor(kBlack);
	
	SM120->Add(VBF120,1.0);
	SM120->Scale(5.0);
	SM120->SetLineWidth(2);
	SM120->SetLineStyle(kDashed);
	SM120->SetLineColor(kBlack);
	
	//TFitResultPtr res = SM120->Fit("gaus","S","",80,160);
	//res->Print("V");
	//cout << SM[j] << " with " << MEt[i] << " e-ID ==> RMS/mean " << (SM120->GetRMS())/(SM120->GetMean()) << endl;
	
	TCanvas *c1 = new TCanvas("c1"+MEt[i]+SM[j]+TString(Form("%d",mH[k])),"",5,30,650,600);
	c1->SetGrid(0,0);
	c1->SetFillStyle(4000);
	c1->SetFillColor(10);
	c1->SetTicky();
	c1->SetObjectStat(0);

	TLegend* leg = new TLegend(0.50,0.50,0.75,0.85,NULL,"brNDC");
	leg->SetFillStyle(0);
	leg->SetBorderSize(0);
	leg->SetFillColor(10);
	leg->SetTextSize(0.03);
	leg->SetHeader("#splitline{CMS Preliminary 2011 #sqrt{s}=7 TeV}{4.6 fb^{-1} #tau_{e}#tau_{h}: Pre-fit plot}");
	
	//leg->AddEntry(all,"SM background");
	leg->AddEntry(ZTT,"Z#rightarrow#tau#tau","F");
	leg->AddEntry(TT,"Top","F");
	leg->AddEntry(W,"EWK","F");
	leg->AddEntry(QCD,"QCD","F");

	leg->AddEntry(SM120,Form("Higgs(%d) X 5",mH[k]));
	leg->AddEntry(data_obs,"Observed");

	c1->cd();

	//all->Draw("HIST");

	float max = data_obs->GetMaximum()*1.1;
	if( (string(SM[j].Data())).find("SM0") != string::npos ) max = 8000.;//max = 4000.;
	if( (string(SM[j].Data())).find("SM1") != string::npos ) max = 500.;//max = 240.;
	if( (string(SM[j].Data())).find("SM2") != string::npos ) max = 60.;//max = 34.;

	data_obs->GetYaxis()->SetRangeUser(0, max);
	data_obs->Draw("P");
	aStack->Draw("HISTSAME");
	SM120->Draw("HISTSAME");
	data_obs->Draw("PSAME");
	
	leg->Draw();

	h->SetBinContent(k+1, SM120->GetRMS()/ SM120->GetMean());
	h->SetBinError(k+1,   SM120->GetRMSError()/ SM120->GetMean());

	hTrue->SetBinContent(k+1, SM120->GetMean()/ mH[k]);
	hTrue->SetBinError(k+1,   SM120->GetMeanError()/mH[k] );

	c1->Update();
	file->cd();
	c1->Write();
	c1->SaveAs("plot"+MEt[i]+SM[j]+TString(Form("%d",mH[k]))+".png");
	c1->SaveAs("plot"+MEt[i]+SM[j]+TString(Form("%d",mH[k]))+".pdf");
      }

      file->cd();
      h->Write();
      hTrue->Write();
    }
  }

  file->Write();
  file->Close();

}
