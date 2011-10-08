

{

 TFile* fZ = new TFile("tmvaRoot/TMVAPtOrd.root");
 TH1F* hZ = (TH1F*)fZ->Get("Method_Cuts/Cuts/MVA_Cuts_rejBvsS");

 TFile* fW = new TFile("tmvaRoot/TMVAPtOrd_qqH115vsW.root");
 TH1F* hW = (TH1F*)fW->Get("Method_Cuts/Cuts/MVA_Cuts_rejBvsS");

 TFile* fQCD = new TFile("tmvaRoot/TMVAPtOrd_qqH115vsQCD.root");
 TH1F* hQCD = (TH1F*)fQCD->Get("Method_Cuts/Cuts/MVA_Cuts_rejBvsS");

 TFile* fTTbar = new TFile("tmvaRoot/TMVAPtOrd_qqH115vsTTbar.root");
 TH1F* hTTbar = (TH1F*)fTTbar->Get("Method_Cuts/Cuts/MVA_Cuts_rejBvsS");

 //TFile* fAll = new TFile("tmvaRoot/TMVAPtOrd_qqH115vsWZttQCD.root");
 TFile* fAll = new TFile("tmvaRoot/TMVAPtOrd_qqH115vsWZttQCD.root");
 TH1F* hAll  = (TH1F*)fAll->Get("Method_Cuts/Cuts/MVA_Cuts_trainingRejBvsS");

 hZ->SetLineColor(kRed);
 hZ->SetLineWidth(2);
 hW->SetLineColor(kGreen);
 hW->SetLineWidth(2);
 hQCD->SetLineColor(kBlue);
 hQCD->SetLineWidth(2);
 hTTbar->SetLineColor(kMagenta);
 hTTbar->SetLineWidth(2);

 hAll->SetLineWidth(4);
 hAll->SetLineStyle(kSolid);

 hZ->SetXTitle("efficiency on qqH(115)");
 hZ->SetYTitle("background rejection");
 hZ->SetAxisRange(0.85,1.02,"Y");
 hZ->SetAxisRange(0.,0.9,"X");
 hZ->SetTitle("");
 hZ->SetTitleSize(0.05,"X");
 hZ->SetTitleSize(0.05,"Y");
 hZ->SetTitleOffset(0.85,"Y");
 hZ->SetTitleOffset(0.78,"X");


 hZ->Draw();
 hW->Draw("SAMEHIST");
 hQCD->Draw("SAMEHIST");
 hTTbar->Draw("SAMEHIST");
 hAll->Draw("SAMEHIST");

 TLegend* leg = new TLegend(0.20,0.20,0.45,0.50,NULL,"brNDC");
 leg->SetFillStyle(0);
 leg->SetBorderSize(0);
 leg->SetFillColor(10);
 leg->SetTextSize(0.04);
 leg->AddEntry(hZ,"Z+jets","L");
 leg->AddEntry(hW,"W+jets","L");
 leg->AddEntry(hQCD,"QCD#mu","L");
 leg->AddEntry(hTTbar,"t#bar{t}","L");
 leg->AddEntry(hAll,"all","L");
 leg->SetHeader("Optimized cuts: Method Cuts");

 leg->Draw();

}
