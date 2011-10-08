

{


  TFile* file = new TFile("../root/patTuplesSkimmed_MuTauStream_10percent10GeV.root");
  file->cd();

  TH1F* hShift = new TH1F("hShift","",32,0,32);  
  TH1F* hPt = new TH1F("hPt","",100,-1,1);
  Events.Draw("(patMETs_rescaledMET_DDDDD_MUTAUANA.obj[0].pt_-patMETs_patMETsPFlow__PAT.obj[0].pt_)/patMETs_patMETsPFlow__PAT.obj[0].pt_>>hPt");

  int iter=0;
  char shift[] = {'U','D'};
  for(int i = 0; i < 2 ; i++){
    for(int j = 0; j < 2 ; j++){
      for(int k = 0; k < 2 ; k++){
	for(int l = 0; l < 2 ; l++){
	  for(int m = 0; m < 2 ; m++){
	    char buffer[5];
	    sprintf (buffer, "%c%c%c%c%c", 
		     shift[i],shift[j],shift[k],shift[l],shift[m]);
	    Events.Draw(Form("(patMETs_rescaledMET_%s_MUTAUANA.obj[0].pt_-patMETs_patMETsPFlow__PAT.obj[0].pt_)/patMETs_patMETsPFlow__PAT.obj[0].pt_>>hPt",buffer));
	    cout << buffer << " pt ==> mean=" << hPt->GetMean() << ", RMS=" << hPt->GetRMS() << endl;
	    iter++;
	    hShift->SetBinContent(iter, hPt->GetMean());
	    hShift->SetBinError(iter, hPt->GetMeanError());
	    hShift->GetXaxis()->SetBinLabel(iter, buffer);
	  }
	}
      }
    }
  }

  hShift->SetMarkerSize(1.2);
  hShift->SetMarkerStyle(kFullCircle);
  hShift->LabelsOption("v","X");
  hShift->Draw("P");

}
