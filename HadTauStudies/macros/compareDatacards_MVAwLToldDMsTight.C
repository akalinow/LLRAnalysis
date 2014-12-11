
#include <TFile.h>
#include <TString.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TMath.h>
#include <TROOT.h>
#include <TStyle.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <assert.h>

TH1* compRatioHistogram(const std::string& ratioHistogramName, const TH1* numerator, const TH1* denominator)
{
  TH1* histogramRatio = 0;
  
  if ( numerator->GetDimension() == denominator->GetDimension() &&
       numerator->GetNbinsX() == denominator->GetNbinsX() ) {
    histogramRatio = (TH1*)numerator->Clone(ratioHistogramName.data());
    histogramRatio->Divide(denominator);
    
    int nBins = histogramRatio->GetNbinsX();
    for ( int iBin = 1; iBin <= nBins; ++iBin ){
      double binContent = histogramRatio->GetBinContent(iBin);
      histogramRatio->SetBinContent(iBin, binContent - 1.);
    }
    
    histogramRatio->SetLineColor(numerator->GetLineColor());
    histogramRatio->SetLineWidth(numerator->GetLineWidth());
    histogramRatio->SetMarkerColor(numerator->GetMarkerColor());
    histogramRatio->SetMarkerStyle(numerator->GetMarkerStyle());
    histogramRatio->SetMarkerSize(numerator->GetMarkerSize());
  }

  return histogramRatio;
}

void showHistograms(double canvasSizeX, double canvasSizeY,
		    TH1* histogram_ref, const std::string& legendEntry_ref, double integral_ref,
		    TH1* histogram2, const std::string& legendEntry2, double integral2,
		    TH1* histogram3, const std::string& legendEntry3, double integral3,
		    TH1* histogram4, const std::string& legendEntry4, double integral4,
		    TH1* histogram5, const std::string& legendEntry5, double integral5,
		    TH1* histogram6, const std::string& legendEntry6, double integral6,
		    const std::string& xAxisTitle, double xAxisOffset,
		    bool useLogScale, double yMin, double yMax, const std::string& yAxisTitle, double yAxisOffset,
		    double legendX0, double legendY0, 
		    const std::string& outputFileName)
{
  TCanvas* canvas = new TCanvas("canvas", "canvas", canvasSizeX, canvasSizeY);
  canvas->SetFillColor(10);
  canvas->SetBorderSize(2);
  canvas->SetLeftMargin(0.12);
  canvas->SetBottomMargin(0.12);

  TPad* topPad = new TPad("topPad", "topPad", 0.00, 0.35, 1.00, 1.00);
  topPad->SetFillColor(10);
  topPad->SetTopMargin(0.04);
  topPad->SetLeftMargin(0.15);
  topPad->SetBottomMargin(0.03);
  topPad->SetRightMargin(0.05);
  topPad->SetLogy(useLogScale);

  TPad* bottomPad = new TPad("bottomPad", "bottomPad", 0.00, 0.00, 1.00, 0.35);
  bottomPad->SetFillColor(10);
  bottomPad->SetTopMargin(0.02);
  bottomPad->SetLeftMargin(0.15);
  bottomPad->SetBottomMargin(0.24);
  bottomPad->SetRightMargin(0.05);
  bottomPad->SetLogy(false);

  canvas->cd();
  topPad->Draw();
  topPad->cd();

  //int colors[6] = { kBlack, kGreen - 6, kBlue - 7,  kMagenta - 7, kCyan - 6, kRed - 6 };
  int colors[6] = { kBlack, kRed, kBlue - 7,  kMagenta - 7, kCyan - 6, kRed - 6 };
  int markerStyles[6] = { 24, 25, 20, 21, 22, 23 };
  int markerSizes[6] = { 1, 1, 1, 1, 1, 1 };

  TLegend* legend = new TLegend(legendX0, legendY0, legendX0 + 0.61, legendY0 + 0.21, "", "brNDC"); 
  legend->SetBorderSize(0);
  legend->SetFillColor(0);

  histogram_ref->SetTitle("");
  histogram_ref->SetStats(false);
  histogram_ref->SetMinimum(yMin);
  histogram_ref->SetMaximum(yMax);
  histogram_ref->SetLineColor(colors[0]);
  histogram_ref->SetLineWidth(2);
  histogram_ref->SetMarkerColor(colors[0]);
  histogram_ref->SetMarkerStyle(markerStyles[0]);
  histogram_ref->SetMarkerSize(markerSizes[0]);
  histogram_ref->Draw("e1p");
  if ( integral_ref >= 0. ) legend->AddEntry(histogram_ref, Form("%s: %1.2f", legendEntry_ref.data(), integral_ref), "p");
  else legend->AddEntry(histogram_ref, legendEntry_ref.data(), "p");

  TAxis* xAxis_top = histogram_ref->GetXaxis();
  xAxis_top->SetTitle(xAxisTitle.data());
  xAxis_top->SetTitleOffset(xAxisOffset);
  xAxis_top->SetLabelColor(10);
  xAxis_top->SetTitleColor(10);

  TAxis* yAxis_top = histogram_ref->GetYaxis();
  yAxis_top->SetTitle(yAxisTitle.data());
  yAxis_top->SetTitleOffset(yAxisOffset);

  if ( histogram2 ) {
    histogram2->SetLineColor(colors[1]);
    histogram2->SetLineWidth(2);
    histogram2->SetMarkerColor(colors[1]);
    histogram2->SetMarkerStyle(markerStyles[1]);
    histogram2->SetMarkerSize(markerSizes[1]);
    histogram2->Draw("e1psame");
    if ( integral2 >= 0. ) legend->AddEntry(histogram2, Form("%s: %1.2f", legendEntry2.data(), integral2), "p");
    else legend->AddEntry(histogram2, legendEntry2.data(), "p");
  }

  if ( histogram3 ) {
    histogram3->SetLineColor(colors[2]);
    histogram3->SetLineWidth(2);
    histogram3->SetMarkerColor(colors[2]);
    histogram3->SetMarkerStyle(markerStyles[2]);
    histogram3->SetMarkerSize(markerSizes[2]);
    histogram3->Draw("e1psame");
    if ( integral3 >= 0. ) legend->AddEntry(histogram3, Form("%s: %1.2f", legendEntry3.data(), integral3), "p");
    else legend->AddEntry(histogram3, legendEntry3.data(), "p");
  }

  if ( histogram4 ) {
    histogram4->SetLineColor(colors[3]);
    histogram4->SetLineWidth(2);
    histogram4->SetMarkerColor(colors[3]);
    histogram4->SetMarkerStyle(markerStyles[3]);
    histogram4->SetMarkerSize(markerSizes[3]);
    histogram4->Draw("e1psame");
    if ( integral4 >= 0. ) legend->AddEntry(histogram4, Form("%s: %1.2f", legendEntry4.data(), integral4), "p");
    else legend->AddEntry(histogram4, legendEntry4.data(), "p");
  }

  if ( histogram5 ) {
    histogram5->SetLineColor(colors[4]);
    histogram5->SetLineWidth(2);
    histogram5->SetMarkerColor(colors[4]);
    histogram5->SetMarkerStyle(markerStyles[4]);
    histogram5->SetMarkerSize(markerSizes[4]);
    histogram5->Draw("e1psame");
    if ( integral5 >= 0. ) legend->AddEntry(histogram5, Form("%s: %1.2f", legendEntry5.data(), integral5), "p");
    else legend->AddEntry(histogram5, legendEntry5.data(), "p");
  }

  if ( histogram6 ) {
    histogram6->SetLineColor(colors[5]);
    histogram6->SetLineWidth(2);
    histogram6->SetMarkerColor(colors[5]);
    histogram6->SetMarkerStyle(markerStyles[5]);
    histogram6->SetMarkerSize(markerSizes[5]);
    histogram6->Draw("e1psame");
    if ( integral6 >= 0. ) legend->AddEntry(histogram6, Form("%s: %1.2f", legendEntry6.data(), integral6), "p");
    else legend->AddEntry(histogram6, legendEntry6.data(), "p");
  }

  legend->Draw();

  canvas->cd();
  bottomPad->Draw();
  bottomPad->cd();

  TH1* histogram2_div_ref = 0;
  if ( histogram2 ) {
    std::string histogramName2_div_ref = std::string(histogram2->GetName()).append("_div_").append(histogram_ref->GetName());
    histogram2_div_ref = compRatioHistogram(histogramName2_div_ref, histogram2, histogram_ref);
    if ( histogram2_div_ref ) {
      histogram2_div_ref->SetTitle("");
      histogram2_div_ref->SetStats(false);
      histogram2_div_ref->SetMinimum(-0.50);
      histogram2_div_ref->SetMaximum(+0.50);
      
      TAxis* xAxis_bottom = histogram2_div_ref->GetXaxis();
      xAxis_bottom->SetTitle(xAxis_top->GetTitle());
      xAxis_bottom->SetLabelColor(1);
      xAxis_bottom->SetTitleColor(1);
      xAxis_bottom->SetTitleOffset(1.20);
      xAxis_bottom->SetTitleSize(0.08);
      xAxis_bottom->SetLabelOffset(0.02);
      xAxis_bottom->SetLabelSize(0.08);
      xAxis_bottom->SetTickLength(0.055);
      
      TAxis* yAxis_bottom = histogram2_div_ref->GetYaxis();
      yAxis_bottom->SetTitle(Form("#frac{%s - %s}{%s}", legendEntry2.data(), legendEntry_ref.data(), legendEntry_ref.data()));
      yAxis_bottom->SetTitleOffset(0.85);
      yAxis_bottom->SetNdivisions(505);
      yAxis_bottom->CenterTitle();
      yAxis_bottom->SetTitleSize(0.08);
      yAxis_bottom->SetLabelSize(0.08);
      yAxis_bottom->SetTickLength(0.04);  
      
      histogram2_div_ref->Draw("e1p");
    }
  }

  TH1* histogram3_div_ref = 0;
  if ( histogram3 ) {
    std::string histogramName3_div_ref = std::string(histogram3->GetName()).append("_div_").append(histogram_ref->GetName());
    histogram3_div_ref = compRatioHistogram(histogramName3_div_ref, histogram3, histogram_ref);
    if ( histogram3_div_ref ) {
      histogram3_div_ref->SetTitle("");
      histogram3_div_ref->SetStats(false);
      histogram3_div_ref->SetMinimum(-0.50);
      histogram3_div_ref->SetMaximum(+0.50);
      
      TAxis* xAxis_bottom = histogram3_div_ref->GetXaxis();
      xAxis_bottom->SetTitle(xAxis_top->GetTitle());
      xAxis_bottom->SetLabelColor(1);
      xAxis_bottom->SetTitleColor(1);
      xAxis_bottom->SetTitleOffset(1.20);
      xAxis_bottom->SetTitleSize(0.08);
      xAxis_bottom->SetLabelOffset(0.02);
      xAxis_bottom->SetLabelSize(0.08);
      xAxis_bottom->SetTickLength(0.055);
      
      TAxis* yAxis_bottom = histogram3_div_ref->GetYaxis();
      yAxis_bottom->SetTitle(Form("#frac{%s - %s}{%s}", legendEntry3.data(), legendEntry_ref.data(), legendEntry_ref.data()));
      yAxis_bottom->SetTitleOffset(0.85);
      yAxis_bottom->SetNdivisions(505);
      yAxis_bottom->CenterTitle();
      yAxis_bottom->SetTitleSize(0.08);
      yAxis_bottom->SetLabelSize(0.08);
      yAxis_bottom->SetTickLength(0.04);  
      
      if ( histogram2 ) histogram3_div_ref->Draw("e1psame");
      else histogram3_div_ref->Draw("e1p");
    }
  }

  TGraph* graph_line = new TGraph(2);
  graph_line->SetPoint(0, xAxis_top->GetXmin(), 0.);
  graph_line->SetPoint(1, xAxis_top->GetXmax(), 0.);
  graph_line->SetLineColor(8);
  graph_line->SetLineWidth(1);
  graph_line->Draw("L");

  if ( histogram2_div_ref ) histogram2_div_ref->Draw("e1psame");
  if ( histogram3_div_ref ) histogram3_div_ref->Draw("e1psame");

  TH1* histogram4_div_ref = 0;
  if ( histogram4 ) {
    std::string histogramName4_div_ref = std::string(histogram4->GetName()).append("_div_").append(histogram_ref->GetName());
    histogram4_div_ref = compRatioHistogram(histogramName4_div_ref, histogram4, histogram_ref);
    if ( histogram4_div_ref ) {
      histogram4_div_ref->Draw("e1psame");
    }
  }

  TH1* histogram5_div_ref = 0;
  if ( histogram5 ) {
    std::string histogramName5_div_ref = std::string(histogram5->GetName()).append("_div_").append(histogram_ref->GetName());
    histogram5_div_ref = compRatioHistogram(histogramName5_div_ref, histogram5, histogram_ref);
    if ( histogram5_div_ref ) {
      histogram5_div_ref->Draw("e1psame");
    }
  }

  TH1* histogram6_div_ref = 0;
  if ( histogram6 ) {
    std::string histogramName6_div_ref = std::string(histogram6->GetName()).append("_div_").append(histogram_ref->GetName());
    histogram6_div_ref = compRatioHistogram(histogramName6_div_ref, histogram6, histogram_ref);
    if ( histogram6_div_ref ) {
      histogram6_div_ref->Draw("e1psame");
    }
  }
  
  canvas->Update();
  size_t idx = outputFileName.find_last_of('.');
  std::string outputFileName_plot = std::string(outputFileName, 0, idx);
  if ( useLogScale ) outputFileName_plot.append("_log");
  else outputFileName_plot.append("_linear");
  if ( idx != std::string::npos ) canvas->Print(std::string(outputFileName_plot).append(std::string(outputFileName, idx)).data());
  canvas->Print(std::string(outputFileName_plot).append(".png").data());
  //canvas->Print(std::string(outputFileName_plot).append(".pdf").data());
  //canvas->Print(std::string(outputFileName_plot).append(".root").data());
  
  delete legend;
  delete histogram2_div_ref;
  delete histogram3_div_ref;
  delete histogram4_div_ref;
  delete histogram5_div_ref;
  delete histogram6_div_ref;
  delete topPad;
  delete bottomPad;
  delete canvas;  
}

void divideByBinWidth(TH1* histogram)
{
  TAxis* xAxis = histogram->GetXaxis();
  int numBins = xAxis->GetNbins();
  for ( int iBin = 1; iBin <= numBins; ++iBin ) {
    double binContent = histogram->GetBinContent(iBin);
    double binError = histogram->GetBinError(iBin);
    double binWidth = xAxis->GetBinWidth(iBin);
    histogram->SetBinContent(iBin, binContent/binWidth);
    histogram->SetBinError(iBin, binError/binWidth);
  }
}

TH1* linearizeHistogram(TH1* histogram)
{
  TH1* histogram2d = dynamic_cast<TH2*>(histogram);
  if ( !histogram2d ) return histogram;

  TAxis* xAxis = histogram2d->GetXaxis();
  int numBinsX = xAxis->GetNbins();
  TAxis* yAxis = histogram2d->GetYaxis();
  int numBinsY = yAxis->GetNbins();

  std::string histogramName1d = Form("%s_1d", histogram->GetName());
  int numBins1d = numBinsX*numBinsY;
  TH1* histogram1d = new TH1D(histogramName1d.data(),histogramName1d.data(), numBins1d, -0.5, numBins1d - 0.5);
  int iBin1d = 1;
  for ( int iBinX = 1; iBinX <= numBinsX; ++iBinX ) {
    for ( int iBinY = 1; iBinY <= numBinsY; ++iBinY ) {
      double binContent = histogram2d->GetBinContent(iBinX, iBinY);
      double binError = histogram2d->GetBinError(iBinX, iBinY);
      histogram1d->SetBinContent(iBin1d, binContent);
      histogram1d->SetBinError(iBin1d, binError);
      ++iBin1d;
    }
  }

  return histogram1d;
}

TH1* compHistogramErr(const TH1* histogram)
{
  std::string histogramErrName = Form("%sErr", histogram->GetName());
  TH1* histogramErr = (TH1*)histogram->Clone();
  int numBins = histogram->GetNbinsX();
  for ( int iBin = 1; iBin <= numBins; ++iBin ) {
    double binError = histogram->GetBinError(iBin);
    histogramErr->SetBinContent(iBin, binError);
    histogramErr->SetBinError(iBin, 0.);
  }
  return histogramErr;
}

void makePlot(const std::string& inputFilePath_ref, const std::string& inputFileName_ref, const std::string& histogramName_ref, const std::string& legendEntry_ref,
	      const std::string& inputFilePath_test, const std::string& inputFileName_test, const std::string& histogramName_test, const std::string& legendEntry_test, 
	      const std::string& inputFilePath_test2, const std::string& inputFileName_test2, const std::string& histogramName_test2, const std::string& legendEntry_test2,
	      const std::string& outputFileName)
{
  TString inputFileName_ref_full = inputFilePath_ref;
  if ( !inputFileName_ref_full.EndsWith("/") ) inputFileName_ref_full.Append("/");
  inputFileName_ref_full.Append(inputFileName_ref.data());
  TFile* inputFile_ref = new TFile(inputFileName_ref_full.Data());
  
  std::cout << "loading histogram = " << histogramName_ref << " from file = " << inputFile_ref->GetName() << std::endl;
  TH1* histogram_ref = dynamic_cast<TH1*>(inputFile_ref->Get(histogramName_ref.data()));
  std::cout << "histogram_ref = " << histogram_ref << std::endl;
  if ( !histogram_ref ) {
    std::cerr << "Failed to load histogram = " << histogramName_ref << " from file = " << inputFile_ref->GetName() << " --> skipping !!" << std::endl;
    delete inputFile_ref;
    return;
  }
  if ( !histogram_ref->GetSumw2N() ) histogram_ref->Sumw2();
  if ( dynamic_cast<TH2*>(histogram_ref) ) histogram_ref = linearizeHistogram(histogram_ref);
  //else if ( histogram_ref->GetNbinsX() >= 100 ) histogram_ref->Rebin(5);
  double integral_ref = histogram_ref->Integral();
  divideByBinWidth(histogram_ref);
  
  TString inputFileName_test_full = inputFilePath_test;
  if ( !inputFileName_test_full.EndsWith("/") ) inputFileName_test_full.Append("/");
  inputFileName_test_full.Append(inputFileName_test.data());
  TFile* inputFile_test = ( inputFileName_test_full != inputFileName_ref_full ) ? 
    new TFile(inputFileName_test_full.Data()) : inputFile_ref;
  
  std::cout << "loading histogram = " << histogramName_test << " from file = " << inputFile_test->GetName() << std::endl;
  TH1* histogram_test = dynamic_cast<TH1*>(inputFile_test->Get(histogramName_test.data()));
  std::cout << "histogram_test = " << histogram_test << std::endl;
  if ( !histogram_test ) {
    std::cerr << "Failed to load histogram = " << histogramName_test << " from file = " << inputFile_test->GetName() << " --> skipping !!" << std::endl;
    delete inputFile_ref;
    if ( inputFile_test != inputFile_ref ) delete inputFile_test;
    return;
  }
  if ( !histogram_test->GetSumw2N() ) histogram_test->Sumw2();
  if ( dynamic_cast<TH2*>(histogram_test) ) histogram_test = linearizeHistogram(histogram_test);
  //else if ( histogram_test->GetNbinsX() >= 100 ) histogram_test->Rebin(5);
  double integral_test = histogram_test->Integral();
  divideByBinWidth(histogram_test);
  
  TFile* inputFile_test2 = 0;
  TH1* histogram_test2 = 0;
  double integral_test2 = -1.;
  if ( inputFilePath_test2 != "" && inputFileName_test2 != "" && histogramName_test2 != "" ) {
    TString inputFileName_test2_full = inputFilePath_test2;
    if ( !inputFileName_test2_full.EndsWith("/") ) inputFileName_test2_full.Append("/");
    inputFileName_test2_full.Append(inputFileName_test2.data());
    inputFile_test2 = ( inputFileName_test2_full != inputFileName_ref_full ) ? 
      new TFile(inputFileName_test2_full.Data()) : inputFile_ref;
  
    std::cout << "loading histogram = " << histogramName_test2 << " from file = " << inputFile_test2->GetName() << std::endl;
    histogram_test2 = dynamic_cast<TH1*>(inputFile_test2->Get(histogramName_test2.data()));
    std::cout << "histogram_test2 = " << histogram_test2 << std::endl;
    if ( !histogram_test2 ) {
      std::cerr << "Failed to load histogram = " << histogramName_test2 << " from file = " << inputFile_test2->GetName() << " --> skipping !!" << std::endl;
      delete inputFile_ref;
      if ( inputFile_test != inputFile_ref ) delete inputFile_test;
      if ( inputFile_test2 != inputFile_ref && inputFile_test2 != inputFile_test ) delete inputFile_test2;
      return;
    }
    if ( !histogram_test2->GetSumw2N() ) histogram_test2->Sumw2();
    if ( dynamic_cast<TH2*>(histogram_test2) ) histogram_test2 = linearizeHistogram(histogram_test2);
    //else if ( histogram_test2->GetNbinsX() >= 100 ) histogram_test2->Rebin(5);
    integral_test2 = histogram_test2->Integral();
    divideByBinWidth(histogram_test2);
  }
  
  showHistograms(800, 900, 
		 histogram_ref, legendEntry_ref, integral_ref,
		 histogram_test, legendEntry_test, integral_test, 
		 histogram_test2, legendEntry_test2, integral_test2, 
		 //NULL, "", 0.,
		 NULL, "", 0.,
		 NULL, "", 0.,
		 NULL, "", 0.,
		 "m_{#tau#tau} [GeV]", 1.10,
		 true, 1.e-4, 1.e+4, "1/dm_{#tau#tau} [1/GeV]", 1.30,
		 0.34, 0.72,
		 outputFileName.data());

  TH1* histogramErr_ref = compHistogramErr(histogram_ref);
  TH1* histogramErr_test = compHistogramErr(histogram_test);
  TH1* histogramErr_test2 = ( histogram_test2 ) ?
    compHistogramErr(histogram_test2) : 0;
  std::string outputFileNameErr = TString(outputFileName.data()).ReplaceAll(".png", "_Err.png").Data();
  showHistograms(800, 900, 
		 histogramErr_ref, legendEntry_ref, -1.,
		 histogramErr_test, legendEntry_test, -1., 
		 histogramErr_test2, legendEntry_test2, -1., 
		 //NULL, "", 0.,
		 NULL, "", 0.,
		 NULL, "", 0.,
		 NULL, "", 0.,
		 "m_{#tau#tau} [GeV]", 1.10,
		 true, 1.e-4, 1.e+4, "#sigma(1/dm_{#tau#tau}) [1/GeV]", 1.30,
		 0.34, 0.72,
		 outputFileNameErr.data());

  delete histogramErr_ref;
  delete histogramErr_test;
  delete histogramErr_test2;
  
  delete inputFile_ref;
  if ( inputFile_test != inputFile_ref ) delete inputFile_test;
  if ( inputFile_test2 != inputFile_ref && inputFile_test2 != inputFile_test ) delete inputFile_test2;
}

void compareDatacards_MVAwLToldDMsTight()
{
  gROOT->SetBatch(true);

  TH1::AddDirectory(false);

  std::string inputFilePath_ref = "/data1/veelken/tmp/tauTauAnalysis/v3_10_2/";
  std::string inputFileName_ref = "htt_tt.inputs-mssm-8TeV-0_MVAwLToldDMsTight.root";
  std::string legendEntry_ref = "LLR";

  std::string inputFilePath_test = "/afs/cern.ch/user/v/veelken/scratch0/CMSSW_5_3_14/src/LLRAnalysis/HadTauStudies/macros/";
  std::string inputFileName_test = "fromRam/2014Nov16/DATACARD_MVAWLTTIGHT_smggh125_syst_included.root";
  //std::string inputFileName_test = "mergeDatacards.root";
  std::string legendEntry_test = "TIFR";

  std::string inputFilePath_test2 = "/afs/cern.ch/user/v/veelken/scratch0/CMSSW_5_3_14/src/LLRAnalysis/HadTauStudies/macros/";
  std::string inputFileName_test2 = "fromCecile/2014Dec01/htt_tt_mssm_mva_tailfit_1Dec.root";
  std::string legendEntry_test2 = "ULB";

  std::string channel = "tauTau";

  std::vector<std::string> categories_ref;
  //categories_ref.push_back("inclusive");
  //categories_ref.push_back("btag");
  categories_ref.push_back("btag_low");
  categories_ref.push_back("btag_high");
  //categories_ref.push_back("nobtag");
  categories_ref.push_back("nobtag_low");
  categories_ref.push_back("nobtag_medium");
  categories_ref.push_back("nobtag_high");
  
  std::map<std::string, std::string> categories_test; // key = category_ref
  categories_test["inclusive"] = "inclusive";
  categories_test["btag"] = "btag";
  categories_test["btag_low"] = "btag_low";
  categories_test["btag_high"] = "btag_high";
  categories_test["nobtag_low"] = "nobtag_low";
  categories_test["nobtag_medium"] = "nobtag_medium";
  categories_test["nobtag_high"] = "nobtag_high";
  
  std::map<std::string, std::string> categories_test2 = categories_test;

  std::vector<std::string> periods;
  periods.push_back("8TeV");
  
  std::map<std::string, std::map<std::string, std::vector<std::string> > > processes; // key = channel, period 
  processes["tauTau"]["8TeV"].push_back("ggH90");
  processes["tauTau"]["8TeV"].push_back("bbH90");
  processes["tauTau"]["8TeV"].push_back("ggH110");
  processes["tauTau"]["8TeV"].push_back("bbH110");
  processes["tauTau"]["8TeV"].push_back("ggH130");
  processes["tauTau"]["8TeV"].push_back("bbH130");
  processes["tauTau"]["8TeV"].push_back("ggH160");
  processes["tauTau"]["8TeV"].push_back("bbH160");
  processes["tauTau"]["8TeV"].push_back("ggH250");
  processes["tauTau"]["8TeV"].push_back("bbH250");
  processes["tauTau"]["8TeV"].push_back("ggH300");
  processes["tauTau"]["8TeV"].push_back("bbH300");
  processes["tauTau"]["8TeV"].push_back("ggH400");
  processes["tauTau"]["8TeV"].push_back("bbH400");
  processes["tauTau"]["8TeV"].push_back("ggH500");
  processes["tauTau"]["8TeV"].push_back("bbH500");
  processes["tauTau"]["8TeV"].push_back("ggH600");
  processes["tauTau"]["8TeV"].push_back("bbH600");
  processes["tauTau"]["8TeV"].push_back("ggH700");
  processes["tauTau"]["8TeV"].push_back("bbH700");
  processes["tauTau"]["8TeV"].push_back("ggH800");
  processes["tauTau"]["8TeV"].push_back("bbH800");
  processes["tauTau"]["8TeV"].push_back("ggH900");
  processes["tauTau"]["8TeV"].push_back("bbH900");
  processes["tauTau"]["8TeV"].push_back("ggH1000");
  processes["tauTau"]["8TeV"].push_back("bbH1000");
  processes["tauTau"]["8TeV"].push_back("ggH_SM125");
  processes["tauTau"]["8TeV"].push_back("qqH_SM125");
  processes["tauTau"]["8TeV"].push_back("data_obs");
  processes["tauTau"]["8TeV"].push_back("QCD");
  processes["tauTau"]["8TeV"].push_back("TT");
  processes["tauTau"]["8TeV"].push_back("VV");
  processes["tauTau"]["8TeV"].push_back("W");
  processes["tauTau"]["8TeV"].push_back("ZJ");
  processes["tauTau"]["8TeV"].push_back("ZL");
  processes["tauTau"]["8TeV"].push_back("ZTT");

  std::vector<std::string> sysShifts;
  sysShifts.push_back("CMS_scale_t_tautau_8TeVUp");
  sysShifts.push_back("CMS_scale_t_tautau_8TeVDown");
  sysShifts.push_back("CMS_eff_t_mssmHigh_tautau_8TeVUp");
  sysShifts.push_back("CMS_eff_t_mssmHigh_tautau_8TeVDown");
  sysShifts.push_back("CMS_htt_eff_trig_mssmHigh_tautau_8TeVUp");
  sysShifts.push_back("CMS_htt_eff_trig_mssmHigh_tautau_8TeVDown");
  sysShifts.push_back("CMS_htt_QCDfrShape_tautau_8TeVUp");
  sysShifts.push_back("CMS_htt_QCDfrShape_tautau_8TeVDown");  
  sysShifts.push_back("CMS_htt_ttbarPtReweight_8TeVUp");  
  sysShifts.push_back("CMS_htt_ttbarPtReweight_8TeVDown");  
  sysShifts.push_back("CMS_htt_ttbarJetFake_tautau_8TeVUp");
  sysShifts.push_back("CMS_htt_ttbarJetFake_tautau_8TeVDown");
  sysShifts.push_back("CMS_htt_WShape_tautau_8TeVUp");
  sysShifts.push_back("CMS_htt_WShape_tautau_8TeVDown");
  sysShifts.push_back("CMS_htt_higgsPtReweight_8TeVUp");
  sysShifts.push_back("CMS_htt_higgsPtReweight_8TeVDown");
  sysShifts.push_back("CMS_htt_higgsPtReweight_scale_8TeVUp");
  sysShifts.push_back("CMS_htt_higgsPtReweight_scale_8TeVDown");
  sysShifts.push_back("CMS_htt_higgsPtReweightSM_8TeVUp");
  sysShifts.push_back("CMS_htt_higgsPtReweightSM_8TeVDown");
  sysShifts.push_back(""); // central value

  for ( std::vector<std::string>::const_iterator category_ref = categories_ref.begin();
	category_ref != categories_ref.end(); ++category_ref ) {
    for ( std::vector<std::string>::const_iterator period = periods.begin();
	  period != periods.end(); ++period ) {
      for ( std::vector<std::string>::const_iterator process = processes[channel][*period].begin();
	    process != processes[channel][*period].end(); ++process ) {
	for ( std::vector<std::string>::const_iterator central_or_shift = sysShifts.begin();
	      central_or_shift != sysShifts.end(); ++central_or_shift ) {
	  if ( central_or_shift->find("CMS_htt_QCDfr") != std::string::npos && (*process) != "QCD" ) continue;
	  if ( central_or_shift->find("CMS_eff_t_mssmHigh") != std::string::npos && !((*process) != "ZTT" || process->find("ggH") != std::string::npos || process->find("bbH") != std::string::npos) ) continue;
	  if ( central_or_shift->find("CMS_htt_ttbarPtReweight") != std::string::npos && (*process) != "TT" ) continue;
	  if ( central_or_shift->find("CMS_htt_ttbarJetFake") != std::string::npos && (*process) != "TT" ) continue;
	  if ( central_or_shift->find("CMS_htt_WShape") != std::string::npos && (*process) != "W" ) continue;
	  if ( central_or_shift->find("CMS_htt_higgsPtReweight") != std::string::npos && process->find("ggH") == std::string::npos ) continue;
	  std::string histogramName_ref, histogramName_test, histogramName_test2, outputFileName;
	  if ( !((*central_or_shift) == "" || (*central_or_shift) == "central") ) {
	    histogramName_ref = Form("%s_%s/%s_%s", channel.data(), category_ref->data(), process->data(), central_or_shift->data());
	    histogramName_test = Form("%s_%s/%s_%s", channel.data(), categories_test[*category_ref].data(), process->data(), central_or_shift->data());
	    histogramName_test2 = Form("%s_%s/%s_%s", channel.data(), categories_test2[*category_ref].data(), process->data(), central_or_shift->data());
	    outputFileName = Form("plots/compareDatacards_MVAwLToldDMsTight_%s_%s_%s_%s_%s.png", channel.data(), category_ref->data(), process->data(), central_or_shift->data(), period->data());
	  } else {
	    histogramName_ref = Form("%s_%s/%s", channel.data(), category_ref->data(), process->data());
	    histogramName_test = Form("%s_%s/%s", channel.data(), categories_test[*category_ref].data(), process->data());
	    histogramName_test2 = Form("%s_%s/%s", channel.data(), categories_test2[*category_ref].data(), process->data());
	    outputFileName = Form("plots/compareDatacards_MVAwLToldDMsTight_%s_%s_%s_%s.png", channel.data(), category_ref->data(), process->data(), period->data());
	  }
	  makePlot(inputFilePath_ref, inputFileName_ref, histogramName_ref, legendEntry_ref, 
		   //inputFilePath_test, inputFileName_test, histogramName_test, legendEntry_test, 
		   inputFilePath_test2, inputFileName_test2, histogramName_test2, legendEntry_test2,
		   "", "", "", "",
		   outputFileName);
	}
      }
    }
  }
}


