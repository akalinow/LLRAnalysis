#include "FWCore/FWLite/interface/AutoLibraryLoader.h"


#include <cstdlib>
#include <iostream> 
#include <fstream>
#include <map>
#include <string>

#include "TChain.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TPluginManager.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TF1.h"
#include "TF2.h"
#include "TLegend.h"
#include "THStack.h"
#include "TCut.h"
#include "TArrayF.h"
#include "TAxis.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TPaveText.h"

#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "DataFormats/FWLite/interface/InputSource.h"
#include "DataFormats/FWLite/interface/OutputFiles.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <string>

using namespace std;


void showHistograms(double canvasSizeX, double canvasSizeY,
		    TH1* histogram1, const std::string& legendEntry1,
		    TH1* histogram2, const std::string& legendEntry2,
		    double legendPosX, double legendPosY, double legendSizeX, double legendSizeY, 
		    std::vector<std::string>& labelTextLines, double labelTextSize,
		    double labelPosX, double labelPosY, double labelSizeX, double labelSizeY,
		    const std::string& xAxisTitle, double xAxisOffset,
		    double yMin, double yMax, const std::string& yAxisTitle, double yAxisOffset,
		    const std::string& outputFileName)
{
  TCanvas* canvas = new TCanvas("canvas", "canvas", 800, 800);
  canvas->SetFillColor(10);
  canvas->SetBorderSize(2);
  
  canvas->SetLeftMargin(0.12);
  canvas->SetBottomMargin(0.12);

  histogram1->SetTitle("");
  histogram1->SetStats(false);
  histogram1->SetMinimum(yMin);
  histogram1->SetMaximum(yMax);

  TAxis* xAxis = histogram1->GetXaxis();
  xAxis->SetTitle(xAxisTitle.data());
  xAxis->SetTitleOffset(xAxisOffset);

  TAxis* yAxis = histogram1->GetYaxis();
  yAxis->SetTitle(yAxisTitle.data());
  yAxis->SetTitleOffset(yAxisOffset);

  int colors[4] = { 1, 2, 3, 4 };
  int lineStyles[4] = { 1, 7, 4, 3 };

  histogram1->SetLineColor(colors[0]);
  histogram1->SetLineWidth(2);
  histogram1->SetLineStyle(lineStyles[0]);
  histogram1->Draw("hist");

  if ( histogram2 ) {
    histogram2->SetLineColor(colors[1]);
    histogram2->SetLineWidth(2);
    histogram2->SetLineStyle(lineStyles[1]);
    histogram2->Draw("histsame");
  }

  TLegend* legend = new TLegend(legendPosX, legendPosY, legendPosX + legendSizeX, legendPosY + legendSizeY, "", "brNDC"); 
  legend->SetBorderSize(0);
  legend->SetFillColor(0);
  legend->AddEntry(histogram1, legendEntry1.data(), "l");
  if ( histogram2 ) legend->AddEntry(histogram2, legendEntry2.data(), "l");
  legend->Draw();

  TPaveText* label = new TPaveText(labelPosX, labelPosY, labelPosX + labelSizeX, labelPosY + labelSizeY, "brNDC");
  for ( std::vector<std::string>::const_iterator labelTextLine = labelTextLines.begin();
	labelTextLine != labelTextLines.end(); ++labelTextLine ) {
    label->AddText(labelTextLine->data());
  }
  label->SetFillColor(10);
  label->SetBorderSize(0);
  label->SetTextColor(1);
  label->SetTextAlign(12);
  label->SetTextSize(labelTextSize);
  label->Draw();

  canvas->Update();
  std::string outputFileName_plot = "plots/";
  size_t idx = outputFileName.find_last_of('.');
  outputFileName_plot.append(std::string(outputFileName, 0, idx));
  if ( idx != std::string::npos ) canvas->Print(std::string(outputFileName_plot).append(std::string(outputFileName, idx)).data());
  canvas->Print(std::string(outputFileName_plot).append(".png").data());
  canvas->Print(std::string(outputFileName_plot).append(".pdf").data());
  
  delete label;
  delete legend;
  delete canvas;  
}


void showGraphs(double canvasSizeX, double canvasSizeY,
		TGraph* graph1, const std::string& legendEntry1,
		TGraph* graph2, const std::string& legendEntry2,
		TGraph* graph3, const std::string& legendEntry3,
		TGraph* graph4, const std::string& legendEntry4,
		double legendPosX, double legendPosY, double legendSizeX, double legendSizeY, 
		std::vector<std::string>& labelTextLines, double labelTextSize,
		double labelPosX, double labelPosY, double labelSizeX, double labelSizeY,
		double xMin, double xMax, const std::string& xAxisTitle, double xAxisOffset,
		double yMin, double yMax, const std::string& yAxisTitle, double yAxisOffset,
		const std::string& outputFileName)
{
  TCanvas* canvas = new TCanvas("canvas", "canvas", 800, 800);
  canvas->SetFillColor(10);
  canvas->SetBorderSize(2);
  
  canvas->SetLeftMargin(0.12);
  canvas->SetBottomMargin(0.12);

  TH1* dummyHistogram = new TH1D("dummyHistogram", "dummyHistogram", 100, xMin, xMax);
  dummyHistogram->SetTitle("");
  dummyHistogram->SetStats(false);
  dummyHistogram->SetMinimum(yMin);
  dummyHistogram->SetMaximum(yMax);

  dummyHistogram->Draw();

  TAxis* xAxis = dummyHistogram->GetXaxis();
  xAxis->SetTitle(xAxisTitle.data());
  xAxis->SetTitleOffset(xAxisOffset);

  TAxis* yAxis = dummyHistogram->GetYaxis();
  yAxis->SetTitle(yAxisTitle.data());
  yAxis->SetTitleOffset(yAxisOffset);

  int colors[4] = { 1, 2, 3, 4 };
  int markerStyles[4] = { 20, 21, 22, 23 };

  graph1->SetLineColor(colors[0]);
  graph1->SetMarkerColor(colors[0]);
  graph1->SetMarkerStyle(markerStyles[0]);
  graph1->Draw("PL");

  if ( graph2 ) {
    graph2->SetLineColor(colors[1]);
    graph2->SetMarkerColor(colors[1]);
    graph2->SetMarkerStyle(markerStyles[1]);
    graph2->Draw("PL");
  }
  
  if ( graph3 ) {
    graph3->SetLineColor(colors[2]);
    graph3->SetMarkerColor(colors[2]);
    graph3->SetMarkerStyle(markerStyles[2]);
    graph3->Draw("PL");
  }

  if ( graph4 ) {
    graph4->SetLineColor(colors[3]);
    graph4->SetMarkerColor(colors[3]);
    graph4->SetMarkerStyle(markerStyles[3]);
    graph4->Draw("PL");
  }
  
  TLegend* legend = new TLegend(legendPosX, legendPosY, legendPosX + legendSizeX, legendPosY + legendSizeY, "", "brNDC"); 
  legend->SetBorderSize(0);
  legend->SetFillColor(0);
  legend->AddEntry(graph1, legendEntry1.data(), "l");
  if ( graph2 ) legend->AddEntry(graph2, legendEntry2.data(), "l");
  if ( graph3 ) legend->AddEntry(graph3, legendEntry3.data(), "l");
  if ( graph4 ) legend->AddEntry(graph4, legendEntry4.data(), "l");
  legend->Draw();

  TPaveText* label = new TPaveText(labelPosX, labelPosY, labelPosX + labelSizeX, labelPosY + labelSizeY, "brNDC");
  for ( std::vector<std::string>::const_iterator labelTextLine = labelTextLines.begin();
	labelTextLine != labelTextLines.end(); ++labelTextLine ) {
    label->AddText(labelTextLine->data());
  }
  label->SetFillColor(10);
  label->SetBorderSize(0);
  label->SetTextColor(1);
  label->SetTextAlign(12);
  label->SetTextSize(labelTextSize);
  label->Draw();

  canvas->Update();
  std::string outputFileName_plot = "plots/";
  size_t idx = outputFileName.find_last_of('.');
  outputFileName_plot.append(std::string(outputFileName, 0, idx));
  if ( idx != std::string::npos ) canvas->Print(std::string(outputFileName_plot).append(std::string(outputFileName, idx)).data());
  canvas->Print(std::string(outputFileName_plot).append(".png").data());
  canvas->Print(std::string(outputFileName_plot).append(".pdf").data());
  
  delete dummyHistogram;
  delete label;
  delete legend;
  delete canvas;  
}





int main(int argc, const char* argv[])
{

  std::cout << "Executing computeCalibration" << std::endl;
  gROOT->SetBatch(true);
 
  if ( argc < 2 ) {
    std::cout << "Usage: " << argv[0] << " [parameters.py]" << std::endl;
    return 0;
  }


  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  if ( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") )
    throw cms::Exception("computeCalibration")
      << "No ParameterSet 'process' found in configuration file = " << argv[1] << " !!\n";
  
  edm::ParameterSet cfg = 
    edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");
  edm::ParameterSet pset 
    = cfg.getParameter<edm::ParameterSet>("svFitCalibrator");

  std::string inputFileName             = pset.getParameter<std::string>("inputFileName");
  std::string outputFileName            = pset.getParameter<std::string>("outputFileName");
  std::string outputTxtFileName         = pset.getParameter<std::string>("outputTxtFileName");
  std::vector<std::string> directories  = pset.getParameter<std::vector<std::string> >("directories");
  std::vector<double> massPoints        = pset.getParameter<std::vector<double> >("massPoints");
  std::vector<std::string> histograms   = pset.getParameter<std::vector<std::string> >("histograms");

  if( massPoints.size() != directories.size())
    throw cms::Exception("computeCalibration")
      << "Number of mass points should match number of directories" << " !!\n";

  std::map<double, string> massToDirMap;
  for(unsigned int i = 0 ; i <  massPoints.size(); i++){
    string directoryName = directories[i];
    while(directoryName.find("/")!=string::npos){
      size_t pos =  directoryName.find("/");
      directoryName.replace( pos, 1, "_");
    }
    massToDirMap.insert( make_pair(massPoints[i], directoryName) );
  }

  std::map<string, vector<double> > massToFitParamMap;

  TFile* fInput = new TFile(inputFileName.c_str(), "READ");
  if(fInput==0 || fInput->IsZombie())
    throw cms::Exception("computeCalibration")
      << "No valid input file" << " !!\n";

  double intercept_a = 0.;
  double slope_a     = 0.;
  double intercept_b = 0.;
  double slope_b     = 0.;
  double intercept_c = 0.;
  double slope_c     = 0.;
  double intercept_d = 0.;
  double slope_d     = 0.;


  fwlite::TFileService fs = fwlite::TFileService(outputFileName.c_str());

  for(unsigned int j = 0 ; j < histograms.size() ; j++){

    string histogramName = histograms[j];
    while(histogramName.find("/")!=string::npos){
      size_t pos =  histogramName.find("/");
      histogramName.replace( pos, 1, "_");
    }
    
    TGraph* graphMeanRaw = new TGraph(massPoints.size());
    graphMeanRaw->SetName(("graphMeanRaw_"+histogramName).c_str());
    gDirectory->Append(graphMeanRaw);
    TGraph* graphResRaw  = new TGraph(massPoints.size());
    graphResRaw->SetName(("graphResRaw_"+histogramName).c_str());
    gDirectory->Append(graphResRaw);
    TGraph* graphMeanCal = new TGraph(massPoints.size());
    graphMeanCal->SetName(("graphMeanCal_"+histogramName).c_str());
    gDirectory->Append(graphMeanCal);
    TGraph* graphResCal = new TGraph(massPoints.size());
    graphResCal->SetName(("graphResCal_"+histogramName).c_str());
    gDirectory->Append(graphResCal);

    for(unsigned int i = 0 ; i < directories.size() ; i++){

      TH2F* h2 = (TH2F*)fInput->Get((directories[i]+"/"+histograms[j]).c_str());
      if(!h2){
	cout << "Error: histogram " << histograms[j] << " does not exist in directory " 
	     << directories[i] 
	     << " => skipping!!" << endl;
	continue;
      }
      if(h2->GetEntries()<100){
	cout << "Histogram contains less than 100 entries => skipping!!" << endl;
	continue;
      }

      string directoryName = directories[i];
      while(directoryName.find("/")!=string::npos){
	size_t pos =  directoryName.find("/");
	directoryName.replace( pos, 1, "_");
      }

      cout << "Now doing " << directoryName << "_" << histogramName << endl;

      h2->SetName(("h2_"+directoryName+"_"+histogramName).c_str());

      TH1F* h1 =  fs.make<TH1F>(("h1_"+directoryName+"_"+histogramName).c_str(),
				(directoryName+"_"+histogramName+" ; #sigma_{SVFit} (GeV/c^{2}); #mu_{SVFit} (GeV/c^{2})").c_str(),
				h2->GetNbinsX(),h2->GetXaxis()->GetXmin(),h2->GetXaxis()->GetXmax());
      
      
      TH1F* h2_py = 0;
      for(int k = 1 ; k <= h2->GetNbinsX() ; k++){
	h2_py = (TH1F*)h2->ProjectionY((directoryName+"_"+histogramName+"_py").c_str(),k+1,k+1,"");
	Double_t M = h2_py->GetMean();
	Double_t E = h2_py->GetMeanError();
	h1->SetBinContent(k+1, M);
	h1->SetBinError(k+1, E );
      }

      TF1 *func = new TF1("func","[0]+[1]*TMath::Erf([2]*x+[3])",h2->GetXaxis()->GetXmin(),h2->GetXaxis()->GetXmax());
      func->SetParLimits(0,-200,200);
      func->SetParLimits(1,0,500);
      func->SetParLimits(2,0,0.5);
      func->SetParLimits(3,-100,1);

      h1->Fit("func","","",h2->GetXaxis()->GetXmin(),h2->GetXaxis()->GetXmax());
      TF1* func_fit =  h1->GetFunction("func");
      double aa = func_fit->GetParameter(0);
      double bb = func_fit->GetParameter(1);
      double cc = func_fit->GetParameter(2);
      double dd = func_fit->GetParameter(3);
      double aaE = func_fit->GetParError(0);
      double bbE = func_fit->GetParError(1);
      double ccE = func_fit->GetParError(2);
      double ddE = func_fit->GetParError(3);

      vector<double> fitParam;
      fitParam.push_back(aa);
      fitParam.push_back(bb);
      fitParam.push_back(cc);
      fitParam.push_back(dd);
      fitParam.push_back(aaE);
      fitParam.push_back(bbE);
      fitParam.push_back(ccE);
      fitParam.push_back(ddE);

      massToFitParamMap.insert( make_pair(directoryName+"_"+histogramName, fitParam) );

      TF1* absDiff = new TF1("absDiff",Form("(%f+%f*TMath::Erf(%f*x+%f) - %f)",
					    aa,bb,cc,dd, massPoints[i]),h2->GetXaxis()->GetXmin(),h2->GetXaxis()->GetXmax()); 

      // make monitoring histograms
      TH1F* h1_raw =  fs.make<TH1F>(("h1_raw_"+directoryName+"_"+histogramName).c_str(),
				    (directories[i]+"_"+histograms[j]+"; mass_{SVFit} (GeV/c^{2})").c_str(),
				    h2->GetNbinsY(),h2->GetYaxis()->GetXmin(),h2->GetYaxis()->GetXmax());
      TH1F* h1_cal0 =  fs.make<TH1F>(("h1_cal0_"+directoryName+"_"+histogramName).c_str(),
				     (directories[i]+"_"+histograms[j]+"; mass_{SVFit} (GeV/c^{2})").c_str(),
				    h2->GetNbinsY(),h2->GetYaxis()->GetXmin(),h2->GetYaxis()->GetXmax());

      cout << "Filling monitor plots from h2 with name" << string(h2->GetName()) << endl;
      for(int k = 1 ; k <= h2->GetNbinsX() ; k++){

	double sigma_bin_k = (k-0.5)*(h2->GetXaxis()->GetBinWidth(1));
	TH1F* h2v2_py = (TH1F*)h2->ProjectionY((directoryName+"_"+histogramName+"_v2py").c_str(),k+1,k+1,"");

	//cout << "Profile plot with name " << string(h2v2_py->GetName()) << endl;

	for(int m = 1 ; m <= h2v2_py->GetNbinsX() ; m++){
	  double mass_bin_m = (m-0.5)*h2v2_py->GetBinWidth(1);
	  double rescaled_mass_bin_m = mass_bin_m - absDiff->Eval(sigma_bin_k);
	  h1_raw->Fill(mass_bin_m,          h2v2_py->GetBinContent(m));
	  h1_cal0->Fill(rescaled_mass_bin_m, h2v2_py->GetBinContent(m));
	}
      }

      graphMeanRaw->SetPoint(i, massPoints[i], h1_raw->GetMean());
      graphMeanCal->SetPoint(i, massPoints[i], h1_cal0->GetMean());
      graphResRaw->SetPoint(i,  massPoints[i], h1_raw->GetRMS()/h1_raw->GetMean());
      graphResCal->SetPoint(i,  massPoints[i], h1_cal0->GetRMS()/h1_cal0->GetMean());

      std::vector<std::string> labelTextLines;
      //labelTextLines.push_back("label1");
      //labelTextLines.push_back("label2");

      showHistograms(800,800,
		     h1_raw, "Raw",
		     h1_cal0, "Calibrated",
		     0.50,0.65,0.20,0.20,
		     labelTextLines,0.02,
		     0.10,0.75,0.01,0.01,
		     "mass",1.10,
		     0, 1.1*TMath::Max(double(h1_raw->GetMaximum()), double(h1_cal0->GetMaximum())),
		     "entries", 1.30,
		     "plot_massPeak_"+directoryName+"_"+histogramName
		     );

      delete func; delete absDiff;


    }// end directories

    std::vector<std::string> labelTextLines;
    //labelTextLines.push_back("label1");
    //labelTextLines.push_back("label2");

    showGraphs(800,800,
	       graphMeanRaw, "Raw",
	       graphMeanCal, "Calibrated",
	       0,"",
	       0,"",
	       0.50,0.65,0.20,0.20,
	       labelTextLines,0.02,
	       0.10,0.75,0.001,0.001,
	       massPoints[0]-10.,massPoints[massPoints.size()-1]+10.,"mass",1.10,
	       0, 1.1*TMath::Max(double(graphMeanRaw->GetMaximum()), double(graphMeanCal->GetMaximum())),
	       "#mu(SVfit)", 1.30,
	       "plot_meanMass_"+histogramName
	       );

    showGraphs(800,800,
	       graphResRaw, "Raw",
	       graphResCal, "Calibrated",
	       0,"",
	       0,"",
	       0.50,0.65,0.20,0.20,
	       labelTextLines,0.02,
	       0.10,0.75,0.001,0.001,
	       massPoints[0]-10,massPoints[massPoints.size()-1]+10,"mass",1.10,
	       0, 1.1*TMath::Max(double(graphResRaw->GetMaximum()), double(graphResCal->GetMaximum())),
	       "RMS(SVfit)/#mu(SVfit)", 1.30,
	       "plot_resMass_"+histogramName
	       );
    
  }// end shistograms


  ofstream out(outputTxtFileName.c_str(),ios_base::app);
  out << "import FWCore.ParameterSet.Config as cms" << endl;
  out << endl;
  
  for(unsigned int j = 0 ; j < histograms.size() ; j++){

    TF1* linear = new TF1("linear","[0]+x*[1]",0,500);

    string histogramName = histograms[j];
    while(histogramName.find("/")!=string::npos){
      size_t pos =  histogramName.find("/");
      histogramName.replace( pos, 1, "_");
    }

    TGraphAsymmErrors* graph_param_a = new TGraphAsymmErrors(massPoints.size());
    for(unsigned int v = 0 ; v < massPoints.size() ; v++){
      graph_param_a->SetPoint(v, massPoints[v], ((massToFitParamMap.find( ((massToDirMap.find(massPoints[v]))->second)+"_"+histogramName ))->second)[0]);
      graph_param_a->SetPointError(v, 
				   0.0, 0.0, 
				   ((massToFitParamMap.find( ((massToDirMap.find(massPoints[v]))->second)+"_"+histogramName ))->second)[4], 
				   ((massToFitParamMap.find( ((massToDirMap.find(massPoints[v]))->second)+"_"+histogramName ))->second)[4]);
    }
    graph_param_a->SetName("graph_param_a");
    graph_param_a->Fit("linear","","",80,500);
    TF1* linear_fit_a = (TF1*)graph_param_a->GetFunction("linear");
    intercept_a = linear_fit_a->GetParameter(0);
    slope_a     = linear_fit_a->GetParameter(1);
    gDirectory->Append(graph_param_a);
    
    TGraphAsymmErrors* graph_param_b = new TGraphAsymmErrors(massPoints.size());
    for(unsigned int v = 0 ; v < massPoints.size() ; v++){
      graph_param_b->SetPoint(v, massPoints[v], ((massToFitParamMap.find( ((massToDirMap.find(massPoints[v]))->second)+"_"+histogramName ))->second)[1]);
      graph_param_b->SetPointError(v, 
				   0.0, 0.0, 
				   ((massToFitParamMap.find( ((massToDirMap.find(massPoints[v]))->second)+"_"+histogramName ))->second)[5], 
				   ((massToFitParamMap.find( ((massToDirMap.find(massPoints[v]))->second)+"_"+histogramName ))->second)[5]);
    }
    graph_param_b->SetName("graph_param_b");
    graph_param_b->Fit("linear","","",80,500);
    TF1* linear_fit_b = (TF1*)graph_param_b->GetFunction("linear");
    intercept_b = linear_fit_b->GetParameter(0);
    slope_b     = linear_fit_b->GetParameter(1);
    gDirectory->Append(graph_param_b);
    
    TGraphAsymmErrors* graph_param_c = new TGraphAsymmErrors(massPoints.size());
    for(unsigned int v = 0 ; v < massPoints.size() ; v++){
      graph_param_c->SetPoint(v, massPoints[v], ((massToFitParamMap.find( ((massToDirMap.find(massPoints[v]))->second)+"_"+histogramName ))->second)[2]);
      graph_param_c->SetPointError(v, 
				   0.0, 0.0, 
				   ((massToFitParamMap.find( ((massToDirMap.find(massPoints[v]))->second)+"_"+histogramName ))->second)[6], 
				   ((massToFitParamMap.find( ((massToDirMap.find(massPoints[v]))->second)+"_"+histogramName ))->second)[6]);
    }
    graph_param_c->SetName("graph_param_c");
    graph_param_c->Fit("linear","","", 80, 500);
    TF1* linear_fit_c = (TF1*)graph_param_c->GetFunction("linear");
    intercept_c = linear_fit_c->GetParameter(0);
    slope_c     = linear_fit_c->GetParameter(1);
    gDirectory->Append(graph_param_c);
    
    TGraphAsymmErrors* graph_param_d = new TGraphAsymmErrors(massPoints.size());
    for(unsigned int v = 0 ; v < massPoints.size() ; v++){
      graph_param_d->SetPoint(v, massPoints[v], ((massToFitParamMap.find( ((massToDirMap.find(massPoints[v]))->second)+"_"+histogramName ))->second)[3]);
      graph_param_d->SetPointError(v, 
				   0.0, 0.0, 
				   ((massToFitParamMap.find( ((massToDirMap.find(massPoints[v]))->second)+"_"+histogramName ))->second)[7], 
				   ((massToFitParamMap.find( ((massToDirMap.find(massPoints[v]))->second)+"_"+histogramName ))->second)[7]);
    }
    graph_param_d->SetName("graph_param_d");
    graph_param_d->Fit("linear","","", 80, 500);
    TF1* linear_fit_d = (TF1*)graph_param_d->GetFunction("linear");
    intercept_d = linear_fit_d->GetParameter(0);
    slope_d     = linear_fit_d->GetParameter(1);
    gDirectory->Append(graph_param_d);
    
    std::vector<std::string> labelTextLines;

    showGraphs(800,800,
	       graph_param_a, "Parameter 0",
	       0,"",
	       0,"",
	       0,"",
	       0.50,0.65,0.20,0.20,
	       labelTextLines,0.02,
	       0.10,0.75,0.01,0.01,
	       massPoints[0]-10,massPoints[massPoints.size()-1]+10,"mass",1.10,
	       1.3*graph_param_a->GetMinimum(), 1.3*graph_param_a->GetMaximum(),
	       "", 1.30,
	       "plot_param_a"
	       );
    showGraphs(800,800,
	       graph_param_b, "Parameter 1",
	       0,"",
	       0,"",
	       0,"",
	       0.50,0.65,0.20,0.20,
	       labelTextLines,0.02,
	       0.10,0.75,0.01,0.01,
	       massPoints[0]-10,massPoints[massPoints.size()-1]+10,"mass",1.10,
	       1.3*graph_param_b->GetMinimum(), 1.3*graph_param_b->GetMaximum(),
	       "", 1.30,
	       "plot_param_b"
	       );

    showGraphs(800,800,
	       graph_param_c, "Parameter 2",
	       0,"",
	       0,"",
	       0,"",
	       0.50,0.65,0.20,0.20,
	       labelTextLines,0.02,
	       0.10,0.75,0.01,0.01,
	       massPoints[0]-10,massPoints[massPoints.size()-1]+10,"mass",1.10,
	       1.3*graph_param_c->GetMinimum(), 1.3*graph_param_c->GetMaximum(),
	       "", 1.30,
	       "plot_param_c"
	       );
    showGraphs(800,800,
	       graph_param_d, "Parameter 3",
	       0,"",
	       0,"",
	       0,"",
	       0.50,0.65,0.20,0.20,
	       labelTextLines,0.02,
	       0.10,0.75,0.01,0.01,
	       massPoints[0]-10,massPoints[massPoints.size()-1]+10,"mass",1.10,
	       1.3*graph_param_d->GetMinimum(), 1.3*graph_param_d->GetMaximum(),
	       "", 1.30,
	       "plot_param_d"
	       );


    out << "SVFitCalibration" << histogramName << " = cms.PSet(" << endl;
    out << "     formula = cms.string(" 
	<< string(Form("((%f)*y+(%f))+((%f)*y+(%f))*TMath::Erf(((%f)*y+(%f))*x+((%f)*y+(%f))) - y", 
		       slope_a, intercept_a, slope_b, intercept_b,slope_c, intercept_c,slope_d, intercept_d))
	<< " ),"
	<< endl; 
    out << "     massForCalibration = cms.double(120)," << endl;
    out << "                 )" << endl;
    out << endl;
    out << endl;

    delete linear;

  }







  TF2* calibration = new TF2("calibration",Form("((%f)*y+(%f))+((%f)*y+(%f))*TMath::Erf(((%f)*y+(%f))*x+((%f)*y+(%f))) - y", 
						slope_a, intercept_a, slope_b, intercept_b,slope_c, intercept_c,slope_d, intercept_d), 0,200,0,1000);
  for(unsigned int j = 0 ; j < histograms.size() ; j++){

    string histogramName = histograms[j];
    while(histogramName.find("/")!=string::npos){
      size_t pos =  histogramName.find("/");
      histogramName.replace( pos, 1, "_");
    }
    
    
    for(unsigned int i = 0 ; i < directories.size() ; i++){
      
      TH2F* h2 = (TH2F*)fInput->Get((directories[i]+"/"+histograms[j]).c_str());
      if(!h2){
	cout << "Error: histogram " << histograms[j] << " does not exist in directory " 
	     << directories[i] 
	     << " => skipping!!" << endl;
	continue;
      }
      if(h2->GetEntries()<10){
	cout << "Histogram contains less than 10 entries => skipping!!" << endl;
	continue;
      }
      
      string directoryName = directories[i];
      while(directoryName.find("/")!=string::npos){
	size_t pos =  directoryName.find("/");
	directoryName.replace( pos, 1, "_");
      }

      h2->SetName(("h2_"+directoryName+"_"+histogramName).c_str());
      
      TH1F* h1_raw_formula =  fs.make<TH1F>(("h1_raw_formula_"+directoryName+"_"+histogramName).c_str(),
					    (directories[i]+"_"+histograms[j]+"; mass_{SVFit} (GeV/c^{2})").c_str(),
					    h2->GetNbinsY(),h2->GetYaxis()->GetXmin(),h2->GetYaxis()->GetXmax());
      TH1F* h1_cal0_formula =  fs.make<TH1F>(("h1_cal0_formula_"+directoryName+"_"+histogramName).c_str(),
					     (directories[i]+"_"+histograms[j]+"; mass_{SVFit} (GeV/c^{2})").c_str(),
					     h2->GetNbinsY(),h2->GetYaxis()->GetXmin(),h2->GetYaxis()->GetXmax());
      TH1F* h1_cal1_formula =  fs.make<TH1F>(("h1_cal1_formula_"+directoryName+"_"+histogramName).c_str(),
					     (directories[i]+"_"+histograms[j]+"; mass_{SVFit} (GeV/c^{2})").c_str(),
					     h2->GetNbinsY(),h2->GetYaxis()->GetXmin(),h2->GetYaxis()->GetXmax());
      
      
      for(int k = 1 ; k <= h2->GetNbinsX() ; k++){
	
	double sigma_bin_k = (k-0.5)*(h2->GetXaxis()->GetBinWidth(1));
	TH1F* h2v3_py = (TH1F*)h2->ProjectionY((directoryName+"_"+histogramName+"_v3py").c_str(),k+1,k+1,"");
	
	for(int m = 1 ; m <= h2v3_py->GetNbinsX() ; m++){
	  double mass_bin_m = (m-0.5)*h2v3_py->GetBinWidth(1);
	  double rescaled_mass_bin_m = mass_bin_m - calibration->Eval(sigma_bin_k, 90.);

	  double bias    = calibration->Eval(sigma_bin_k,mass_bin_m);
	  double massNew = mass_bin_m;
	  double diff    = 999.;
	  int nMax       = -1;
	  bool exit      = false;

	  for(int it=0; it< nMax && !exit; it++){
	    double bias_it = calibration->Eval(sigma_bin_k,massNew);
	    massNew        = mass_bin_m - bias_it;
	    diff           = TMath::Abs(bias - bias_it);
	    bias           = bias_it;
	    if(it==(nMax-1)) cout << "Reached level: stop without convergence" << endl;
	    if(it>0 && diff<2){
	      //if() bias=0;
	      exit = true;
	    }
	  }

	  h1_raw_formula->Fill(mass_bin_m,           h2v3_py->GetBinContent(m));
	  h1_cal0_formula->Fill(rescaled_mass_bin_m, h2v3_py->GetBinContent(m));
	  h1_cal1_formula->Fill(mass_bin_m-bias,     h2v3_py->GetBinContent(m));
	}
      }

      std::vector<std::string> labelTextLines;

      showHistograms(800,800,
		     h1_raw_formula, "Raw",
		     h1_cal0_formula, "Calibrated",
		     0.50,0.65,0.20,0.20,
		     labelTextLines,0.02,
		     0.10,0.75,0.01,0.01,
		     "mass",1.10,
		     0, 1.1*TMath::Max(double(h1_raw_formula->GetMaximum()), double(h1_cal0_formula->GetMaximum())),
		     "entries", 1.30,
		     "plot_massPeakFormulaCal0_"+directoryName+"_"+histogramName
		     );
      showHistograms(800,800,
		     h1_raw_formula, "Raw",
		     h1_cal1_formula, "Calibrated",
		     0.50,0.65,0.20,0.20,
		     labelTextLines,0.02,
		     0.10,0.75,0.01,0.01,
		     "mass",1.10,
		     0, 1.1*TMath::Max(double(h1_raw_formula->GetMaximum()), double(h1_cal1_formula->GetMaximum())),
		     "entries", 1.30,
		     "plot_massPeakFormulaCa1_"+directoryName+"_"+histogramName
		     );



    } // directories

 } // histograms



}

