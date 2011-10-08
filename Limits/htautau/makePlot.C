#include <cstdlib>
#include <iostream> 
#include <fstream>
#include <map>
#include <string>
#include <cstring>

#include "TMath.h"
#include "TFile.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TPluginManager.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TGraphAsymmErrors.h"
#include "TGraphPainter.h"
#include "TMultiGraph.h"


using namespace std;


void plot( TString stream = "eleTau", TString variable = "diTauVisMass", Float_t xMax = 50){

  TCanvas *c1 = new TCanvas("c1","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  
  TLegend* leg = new TLegend(0.14,0.60,0.41,0.85,NULL,"brNDC");
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(0);

  ifstream is;

  
  is.open("submit/grid/results_"+stream+"_SM_"+variable+".txt");  
  int nMassPoints = 0;
  char* c0 = new char[100];
  while (is.good())     
    {
      is.getline(c0,100,'\n');
      printf("%s\n",c0);
      nMassPoints++;
    }
  is.close();
  nMassPoints -= 1;
  cout << "Using " << nMassPoints << " mass points" << endl;

  if(nMassPoints!=8){
    cout << "Check the mass points!" << endl;
    return;
  }


  Float_t obsX[]          = {0.,0.,0.,0.,0.,0.,0.,0.};
  float obsY[]            = {0.,0.,0.,0.,0.,0.,0.,0.};

  float X[]               = {0.,0.,0.,0.,0.,0.,0.,0.};
  float Y[]               = {0.,0.,0.,0.,0.,0.,0.,0.};

  float eX1sL[]           = {0.,0.,0.,0.,0.,0.,0.,0.};
  float eY1sL[]           = {0.,0.,0.,0.,0.,0.,0.,0.};
  float eX1sH[]           = {0.,0.,0.,0.,0.,0.,0.,0.};
  float eY1sH[]           = {0.,0.,0.,0.,0.,0.,0.,0.};

  float eX2sL[]           = {0.,0.,0.,0.,0.,0.,0.,0.};
  float eY2sL[]           = {0.,0.,0.,0.,0.,0.,0.,0.};
  float eX2sH[]           = {0.,0.,0.,0.,0.,0.,0.,0.};
  float eY2sH[]           = {0.,0.,0.,0.,0.,0.,0.,0.};

  char* c = new char[100];
  is.open("submit/grid/results_"+stream+"_SM_"+variable+".txt");

  int countMass = 0;

  while (is.good())     
    {
      is.getline(c,100,'\n');
      if (is.good()){

	cout << "Now doing mass point number " << countMass << endl;

	eX1sL[countMass] = 0.;
	eX1sH[countMass] = 0.;
	eX2sL[countMass] = 0.;
	eX2sH[countMass] = 0.;

	string line(c);
	char* cSplit = strtok ( c ," ");

	int counter = 0;
	while (cSplit != NULL)
	  {
	    cout << "Parsing the " << counter << "th number in line " << countMass << endl;
	    //cout << atof(cSplit) << endl;
	    if(counter==0){
	      X[countMass]           = atof(cSplit);
	      obsX[countMass]        = atof(cSplit);
	    }
	    if(counter==1) Y[countMass]       = atof(cSplit);
 
	    if(counter==2) eY1sH[countMass]   = atof(cSplit);
	    if(counter==3) eY2sH[countMass]   = atof(cSplit);

	    if(counter==4) eY1sL[countMass]   = atof(cSplit);
	    if(counter==5) eY2sL[countMass]   = atof(cSplit);

	    if(counter==6) obsY[countMass]    = atof(cSplit);

	    counter++;
	    cSplit = strtok ( NULL ," ");
	  }

	countMass++;

      }
    }
  

  for(int i = 0 ; i < nMassPoints ; i++){
    cout << "Mass " << X[i] << endl;
    cout << "Exp  " << Y[i] << endl;
    cout << "Obs  " << obsY[i] << endl;

    eY1sH[i] = eY1sH[i]-Y[i];
    eY1sL[i] = fabs(eY1sL[i]-Y[i]);
    eY2sH[i] = eY2sH[i]-Y[i];
    eY2sL[i] = TMath::Max( TMath::Abs(eY2sL[i]-Y[i]), eY1sL[i] );

    if((string(stream.Data())).find("mu")!=string::npos && (string(variable.Data())).find("diTauVisMass")!=string::npos &&
       (i==1 || i==4) ){
      eY1sH[i] = eY1sH[i-1];
      eY2sH[i] = eY2sH[i-1];
    }

    if((string(stream.Data())).find("mu")!=string::npos && (string(variable.Data())).find("diTauNSVfitMass")!=string::npos &&
       i==4 )
      eY2sH[i] = eY2sH[i-1];

    if((string(stream.Data())).find("ele")!=string::npos && (string(variable.Data())).find("diTauVisMass")!=string::npos &&
       i==6 )
      eY2sH[i] = eY2sH[7]-Y[7];

    cout << "1s Up   " << eY1sH[i] << endl;
    cout << "1s Down " << eY1sL[i] << endl;
  }

  TMultiGraph *mg = new TMultiGraph();
  mg->SetTitle("");

  TGraphAsymmErrors* expected = new TGraphAsymmErrors(nMassPoints, X, Y, eX1sL ,eX1sL , eX1sL, eX1sL);
  TGraphAsymmErrors* oneSigma = new TGraphAsymmErrors(nMassPoints, X, Y, eX1sL, eX1sL, eY1sL, eY1sH);
  TGraphAsymmErrors* twoSigma = new TGraphAsymmErrors(nMassPoints, X, Y, eX2sL, eX2sL, eY2sL, eY2sH);
  TGraphAsymmErrors* observed = new TGraphAsymmErrors(nMassPoints, X, obsY, eX1sL ,eX1sL , eX1sL, eX1sL);

 
  oneSigma->SetMarkerColor(kBlack);
  oneSigma->SetMarkerStyle(kFullCircle);
  oneSigma->SetFillColor(kGreen);
  oneSigma->SetFillStyle(1001);

  twoSigma->SetMarkerColor(kBlack);
  twoSigma->SetMarkerStyle(kFullCircle);
  twoSigma->SetFillColor(kYellow);
  twoSigma->SetFillStyle(1001);

  expected->SetMarkerColor(kBlack);
  expected->SetMarkerStyle(kFullCircle);
  expected->SetMarkerSize(1.5);
  expected->SetLineColor(kBlack);
  expected->SetLineWidth(2);

  observed->SetMarkerColor(kBlue);
  observed->SetMarkerStyle(1);
  observed->SetLineColor(kBlue);
  observed->SetLineWidth(4);

  mg->Add(twoSigma);
  mg->Add(oneSigma);
  mg->Add(expected);
  mg->Add(observed);

  mg->Draw("ALP3");

  c1->cd();
  gPad->Modified();
  mg->GetXaxis()->SetLimits(105,140);
  mg->GetYaxis()->SetTitleOffset(0.97);
  mg->SetMinimum(0.);
  mg->SetMaximum(xMax);
  mg->GetXaxis()->SetTitle("m_{H} (GeV)");
  mg->GetYaxis()->SetTitle("#sigma X BR(H#rightarrow#tau#tau)_{95% CLs}/#sigma_{SM}");

  if((string(stream.Data())).find("ele")!=string::npos )
    leg->SetHeader("#splitline{CMS Preliminary}{#sqrt{s}=7 TeV, 1.9 fb^{-1}, #tau_{e}#tau_{had}}");
  else if((string(stream.Data())).find("mu")!=string::npos )
    leg->SetHeader("#splitline{CMS Preliminary}{#sqrt{s}=7 TeV, 1.9 fb^{-1}, #tau_{#mu}#tau_{had}}");

  leg->AddEntry(expected,"Expected CLs limit","P");
  leg->AddEntry(observed,"Observed CLs limit","L");

  leg->Draw();

  TF1 *line = new TF1("line","1",100,150);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);

  line->Draw("SAME");

  gPad->SaveAs("submit/grid/limits_"+stream+"_"+variable+".png");

}
