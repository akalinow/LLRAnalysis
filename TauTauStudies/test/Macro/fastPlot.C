

{

  TFile fPtEtaOrd("TMVAPtEtaOrd.root");
  TFile fPtDEtaOrd("TMVAPtDEtaOrd.root");
  TFile fPtMjjOrd("TMVAPtMjjOrd.root");
  TFile fPtOrd("TMVAPtOrd.root");
  TFile fPtOrdBDT("TMVAPtOrdBDT.root");

  TH1F* PtEtaOrd = (TH1F*)fPtEtaOrd.Get("Method_Cuts/Cuts/MVA_Cuts_rejBvsS");
  TH1F* PtDEtaOrd = (TH1F*)fPtDEtaOrd.Get("Method_Cuts/Cuts/MVA_Cuts_rejBvsS");
  TH1F* PtMjjOrd = (TH1F*)fPtMjjOrd.Get("Method_Cuts/Cuts/MVA_Cuts_rejBvsS");
  TH1F* PtOrd    = (TH1F*)fPtOrd.Get("Method_Cuts/Cuts/MVA_Cuts_rejBvsS");
  TH1F* PtOrdBDT = (TH1F*)fPtOrdBDT.Get("Method_BDT/BDT/MVA_BDT_rejBvsS");

  PtEtaOrd->SetLineColor(kRed);
  PtEtaOrd->SetLineStyle(kSolid);
  PtEtaOrd->SetLineWidth(2);

  PtDEtaOrd->SetLineColor(kYellow);
  PtDEtaOrd->SetLineStyle(kDashed);
  PtDEtaOrd->SetLineWidth(3);

  PtMjjOrd->SetLineColor(kBlue);
  PtMjjOrd->SetLineStyle(kDashed);
  PtMjjOrd->SetLineWidth(4);

  PtOrd->SetLineColor(kMagenta);
  PtOrd->SetLineStyle(kDotted);
  PtOrd->SetLineWidth(3);

  PtOrdBDT->SetLineColor(kGreen);
  PtOrdBDT->SetLineStyle(kDotted);
  PtOrdBDT->SetLineWidth(3);






  PtEtaOrd->Draw("HIST");
  PtDEtaOrd->Draw("HISTSAME");  
  PtMjjOrd->Draw("HISTSAME");
  PtOrd->Draw("HISTSAME");
  PtOrdBDT->Draw("HISTSAME");


  TLegend* leg = new TLegend(0.20,0.20,0.45,0.50,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader("");
  leg->AddEntry(PtEtaOrd,"p_{T}-#eta ordered","l");
  leg->AddEntry(PtDEtaOrd,"p_{T}-D#eta ordered","l");
  leg->AddEntry(PtMjjOrd,"p_{T}-Mjj ordered","l");
  leg->AddEntry(PtOrd,"p_{T} ordered","l");
  leg->AddEntry(PtOrdBDT,"p_{T} ordered BDT","l");
  leg->Draw();

}
