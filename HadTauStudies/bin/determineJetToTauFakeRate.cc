
/** \executable determineJetToTauFakeRate
 *
 * Determine ratio of isolated to anti-isolated tau candidates in same-sign (SS) tau_h tau_h events
 * as function of tau Pt and apply ratio to correction to events in OS anti-isolated region,
 * in order to obtain QCD shape template for signal (OS isolated) region of MSSM -> tau tau -> tau_h tau_h analysis.
 *
 * \author Christian Veelken, LLR
 *
 */

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "FWCore/Utilities/interface/Exception.h"

#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "DataFormats/FWLite/interface/InputSource.h"
#include "DataFormats/FWLite/interface/OutputFiles.h"

#include "LLRAnalysis/HadTauStudies/interface/histogramAuxFunctions.h"

#include <TFile.h>
#include <TH1.h>
#include <TBenchmark.h>
#include <TMath.h>
#include <TGraphAsymmErrors.h>
#include <TF1.h>
#include <TVectorD.h>
#include <TMatrixD.h>
#include <TFitResult.h>
#include <TString.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

typedef std::vector<std::string> vstring;

double square(double x)
{
  return x*x;
} 

void compIntegral_and_Error(const TH1* histogram, double& integral, double& integralErr)
{
  integral = 0.;
  double integralErr2 = 0.;
  int numBins = histogram->GetNbinsX();
  for ( int iBin = 1; iBin <= numBins; ++iBin ) {
    double binContent = histogram->GetBinContent(iBin);
    double binError = histogram->GetBinError(iBin);
    integral += binContent;
    integralErr2 += square(binError);
  }
  assert(integralErr2 >= 0.);
  integralErr = TMath::Sqrt(integralErr2);
}

struct EigenVector_and_Value
{
  EigenVector_and_Value(const TVectorD& eigenVector, double eigenValue)
    : eigenVector_(eigenVector),
      eigenValue_(eigenValue)
  {}
  ~EigenVector_and_Value() {}  
  TVectorD eigenVector_;
  double eigenValue_;
};

std::vector<EigenVector_and_Value> compEigenVectors_and_Values(const TMatrixD& cov)
{
  std::cout << "<compEigenVectors_and_Values>:" << std::endl;
  std::cout << " cov:" << std::endl;
  cov.Print();
  if ( cov.GetNcols() != cov.GetNrows() ) 
    throw cms::Exception("compEigenVectors_and_Values") 
      << "Matrix given as function argument is not symmetric !!\n";
  int dimension = cov.GetNrows();
  TVectorD eigenValues(dimension);
  TMatrixD eigenVectors = cov.EigenVectors(eigenValues);
  assert(eigenVectors.GetNrows() == dimension);
  assert(eigenVectors.GetNcols() == dimension);
  std::vector<EigenVector_and_Value> eigenVectors_and_Values;
  for ( int iEigenVector = 0; iEigenVector < dimension; ++iEigenVector ) {    
    TVectorD eigenVector(dimension);
    for ( int iComponent = 0; iComponent < dimension; ++iComponent ) {    
      eigenVector(iComponent) = eigenVectors(iComponent, iEigenVector);
    }
    double eigenValue = eigenValues(iEigenVector);
    TVectorD vec1 = cov*eigenVector;
    std::cout << "vec1:" << std::endl;
    vec1.Print();
    TVectorD vec2 = eigenValue*eigenVector;
    std::cout << "vec2:" << std::endl;
    vec2.Print();
    // CV: check that EigenVector is indeed an EigenVector,
    //     i.e. that we interpreted the ordering of columns and rows of the eigenVectors matrix correctly
    for ( int iComponent = 0; iComponent < dimension; ++iComponent ) {   
      //std::cout << "component #" << iComponent << ": vec1 = " << vec1(iComponent) << ", vec2 = " << vec2(iComponent) << std::endl;
      assert((vec1(iComponent) - vec2(iComponent)) < 1.e-3*(TMath::Abs(vec1(iComponent)) + TMath::Abs(vec2(iComponent))));
    }
    eigenVectors_and_Values.push_back(EigenVector_and_Value(eigenVector, eigenValue));
  }
  return eigenVectors_and_Values;
}

struct fitFunction_and_legendEntry
{
  fitFunction_and_legendEntry(TF1* fitFunction, const std::string& legendEntry)
    : fitFunction_(fitFunction),
      legendEntry_(legendEntry)
  {}
  ~fitFunction_and_legendEntry() {}
  TF1* fitFunction_;
  std::string legendEntry_;
};

void makeControlPlot(TGraphAsymmErrors* graph, 
		     double avJetToTauFakeRate, double avJetToTauFakeRateUp, double avJetToTauFakeRateDown, 
		     TF1* fitFunctionShape_central, std::vector<fitFunction_and_legendEntry>& fitFunctionsShape_sysShifts, 
		     double xMin, double xMax, const std::string& xAxisTitle, 
		     double yMin, double yMax, 
		     const std::string& outputFileName)
{
  if ( fitFunctionsShape_sysShifts.size() > 6 )
    throw cms::Exception("makeControlPlot") 
      << "Fit functions with more than 3 parameters are not supported yet !!\n";

  TCanvas* canvas = new TCanvas("canvas", "canvas", 800, 900);
  canvas->SetFillColor(10);
  canvas->SetBorderSize(2);
  canvas->SetTopMargin(0.12);
  canvas->SetLeftMargin(0.12);
  canvas->SetBottomMargin(0.12);
  
  TPad* topPad = new TPad("topPad", "topPad", 0.00, 0.35, 1.00, 1.00);
  topPad->SetFillColor(10);
  topPad->SetTopMargin(0.04);
  topPad->SetLeftMargin(0.15);
  topPad->SetBottomMargin(0.03);
  topPad->SetRightMargin(0.05);
  topPad->SetGridx();
  topPad->SetGridy();
  topPad->SetLogy(true);

  canvas->cd();
  topPad->Draw();
  topPad->cd();

  TH1* dummyHistogram_top = new TH1D("dummyHistogram_top", "dummyHistogram_top", 10, xMin, xMax);
  dummyHistogram_top->SetTitle("");
  dummyHistogram_top->SetStats(false);
  dummyHistogram_top->SetMaximum(yMax);
  dummyHistogram_top->SetMinimum(yMin);
  
  TAxis* xAxis_top = dummyHistogram_top->GetXaxis();
  xAxis_top->SetTitle(xAxisTitle.data());
  xAxis_top->SetTitleOffset(1.2);
  xAxis_top->SetLabelColor(10);
  xAxis_top->SetTitleColor(10);

  TAxis* yAxis_top = dummyHistogram_top->GetYaxis();
  //yAxis_top->SetTitle("Fake-rate");
  yAxis_top->SetTitle("f_{#tau}");
  yAxis_top->SetTitleSize(0.050);
  //yAxis_top->SetTitleOffset(1.2);
  yAxis_top->SetTitleOffset(1.15);

  dummyHistogram_top->Draw();

  TLegend* legend = new TLegend(0.69, 0.935 - 0.055*(1 + fitFunctionsShape_sysShifts.size()/2), 0.94, 0.935, "", "brNDC"); 
  legend->SetBorderSize(0);
  legend->SetFillColor(0);

  fitFunctionShape_central->SetLineColor(2);
  fitFunctionShape_central->Draw("Lsame");
  legend->AddEntry(fitFunctionShape_central, "Best Fit", "L");    

  int colors[3] = { 4, 6, 7 };

  int idx_top = 0;
  for ( std::vector<fitFunction_and_legendEntry>::const_iterator fitFunctionShape_sysShift = fitFunctionsShape_sysShifts.begin();
	fitFunctionShape_sysShift != fitFunctionsShape_sysShifts.end(); ++fitFunctionShape_sysShift ) {
    int iColor = idx_top / 2;
    assert(iColor <= 2);
    fitFunctionShape_sysShift->fitFunction_->SetLineColor(colors[iColor]);
    fitFunctionShape_sysShift->fitFunction_->SetLineStyle(8);
    fitFunctionShape_sysShift->fitFunction_->Draw("Lsame");
    if ( (idx_top % 2) == 0 ) legend->AddEntry(fitFunctionShape_sysShift->fitFunction_, fitFunctionShape_sysShift->legendEntry_.data(), "L");
    ++idx_top;
  }

  int numPoints = graph->GetN();
  TGraphAsymmErrors* graph_shape = new TGraphAsymmErrors(numPoints);
  for ( int iPoint = 0; iPoint < numPoints; ++iPoint ) {
    double x, y;
    graph->GetPoint(iPoint, x, y);
    graph_shape->SetPoint(iPoint, x, y);
    double xErrUp = graph->GetErrorXhigh(iPoint);
    double xErrDown = graph->GetErrorXlow(iPoint);
    double yErrUp2 = square(graph->GetErrorYhigh(iPoint));
    yErrUp2 += square(y*((avJetToTauFakeRateUp/avJetToTauFakeRate) - 1.0));
    double yErrDown2 = square(graph->GetErrorYlow(iPoint));
    yErrDown2 += square(y*((avJetToTauFakeRateDown/avJetToTauFakeRate) - 1.0));
    graph_shape->SetPointError(iPoint, xErrDown, xErrUp, TMath::Sqrt(yErrDown2), TMath::Sqrt(yErrUp2));
  }
  graph_shape->SetMarkerStyle(8);
  graph_shape->SetMarkerColor(1);
  graph_shape->Draw("p");

  legend->Draw();

  TPad* bottomPad = new TPad("bottomPad", "bottomPad", 0.00, 0.00, 1.00, 0.35);
  bottomPad->SetFillColor(10);
  bottomPad->SetTopMargin(0.02);
  bottomPad->SetLeftMargin(0.15);
  bottomPad->SetBottomMargin(0.24);
  bottomPad->SetRightMargin(0.05);
  bottomPad->SetGridx();
  bottomPad->SetGridy();

  canvas->cd();
  bottomPad->Draw();
  bottomPad->cd();

  TH1* dummyHistogram_bottom = new TH1D("dummyHistogram_bottom", "dummyHistogram_bottom", 10, xMin, xMax);
  
  dummyHistogram_bottom->SetMinimum(-0.50);
  dummyHistogram_bottom->SetMaximum(+0.50);

  TAxis* xAxis_bottom = dummyHistogram_bottom->GetXaxis();
  xAxis_bottom->SetTitle(xAxisTitle.data());
  xAxis_bottom->SetTitleOffset(1.20);
  xAxis_bottom->SetLabelColor(1);
  xAxis_bottom->SetTitleColor(1);
  xAxis_bottom->SetTitleSize(0.08);
  xAxis_bottom->SetLabelOffset(0.02);
  xAxis_bottom->SetLabelSize(0.08);
  xAxis_bottom->SetTickLength(0.055);

  TAxis* yAxis_bottom = dummyHistogram_bottom->GetYaxis();
  //yAxis_bottom->SetTitle("");
  yAxis_bottom->SetTitle("#frac{f_{#tau} - Fit}{Fit}");
  yAxis_bottom->SetTitleOffset(0.75);
  yAxis_bottom->SetNdivisions(505);
  yAxis_bottom->CenterTitle();
  yAxis_bottom->SetTitleSize(0.08);
  yAxis_bottom->SetLabelSize(0.08);
  yAxis_bottom->SetTickLength(0.04);

  dummyHistogram_bottom->SetTitle("");
  dummyHistogram_bottom->SetStats(false);
  dummyHistogram_bottom->Draw();

  TGraph* graph_line = new TGraph(numPoints);
  for ( int iPoint = 0; iPoint < numPoints; ++iPoint ) {
    Double_t x, y;
    graph->GetPoint(iPoint, x, y);
    graph_line->SetPoint(iPoint, x, 1.0);
  }
  graph_line->SetLineColor(8);
  graph_line->SetLineWidth(1);
  graph_line->Draw("L");

  std::vector<TF1*> fitFunctionsToDelete;

  int idx_bottom = 0;
  for ( std::vector<fitFunction_and_legendEntry>::const_iterator fitFunctionShape_sysShift = fitFunctionsShape_sysShifts.begin();
	fitFunctionShape_sysShift != fitFunctionsShape_sysShifts.end(); ++fitFunctionShape_sysShift ) {
    TF1* fitFunctionShape_pass = fitFunctionShape_sysShift->fitFunction_;
    TString fitFunctionShape_formula_pass = fitFunctionShape_pass->GetTitle();
    int numPar_pass = fitFunctionShape_pass->GetNpar();
    TF1* fitFunctionShape_fail = fitFunctionShape_central;
    TString fitFunctionShape_formula_fail = fitFunctionShape_fail->GetTitle();
    int numPar_fail = fitFunctionShape_fail->GetNpar();
    for ( int iPar = 0; iPar < numPar_fail; ++iPar ) {
      fitFunctionShape_formula_fail.ReplaceAll(Form("[%i]", iPar), Form("[%i]", iPar + numPar_pass));      
    }
    std::string fitFunctionShapeName_sysShift_div_central = Form("%s_div_central", fitFunctionShape_pass->GetName());
    std::string fitFunctionShape_formula_sysShift_div_central = Form("(%s)/(%s) - 1.0", fitFunctionShape_formula_pass.Data(), fitFunctionShape_formula_fail.Data());
    TF1* fitFunctionShape_sysShift_div_central = new TF1(fitFunctionShapeName_sysShift_div_central.data(), fitFunctionShape_formula_sysShift_div_central.data(), xMin, xMax);
    for ( int iPar_pass = 0; iPar_pass < numPar_fail; ++iPar_pass ) {
      fitFunctionShape_sysShift_div_central->SetParameter(iPar_pass, fitFunctionShape_fail->GetParameter(iPar_pass));
    }
    for ( int iPar_fail = 0; iPar_fail < numPar_fail; ++iPar_fail ) {
      fitFunctionShape_sysShift_div_central->SetParameter(iPar_fail + numPar_pass, fitFunctionShape_fail->GetParameter(iPar_fail));
    }
    int iColor = idx_bottom / 2;
    assert(iColor <= 2);
    fitFunctionShape_sysShift_div_central->SetLineColor(colors[iColor]);
    fitFunctionShape_sysShift_div_central->Draw("Lsame");
    fitFunctionsToDelete.push_back(fitFunctionShape_sysShift_div_central);
    ++idx_bottom;
  }

  TGraphAsymmErrors* graph_shape_div_fit = new TGraphAsymmErrors(numPoints);
  for ( int iPoint = 0; iPoint < numPoints; ++iPoint ) {
    double x, y;
    graph_shape->GetPoint(iPoint, x, y);
    double fitFunctionValue = fitFunctionShape_central->Eval(x);
    graph_shape_div_fit->SetPoint(iPoint, x, y/fitFunctionValue - 1.0);
    double xErrUp = graph_shape->GetErrorXhigh(iPoint);
    double xErrDown = graph_shape->GetErrorXlow(iPoint);
    double yErrUp = graph_shape->GetErrorYhigh(iPoint);
    double yErrDown = graph_shape->GetErrorYlow(iPoint);
    graph_shape_div_fit->SetPointError(iPoint, xErrDown, xErrUp, yErrDown/fitFunctionValue, yErrUp/fitFunctionValue);
  }
  graph_shape_div_fit->SetMarkerStyle(8);
  graph_shape_div_fit->SetMarkerColor(1);
  graph_shape_div_fit->Draw("p");
  
  canvas->Update();
  size_t idx = outputFileName.find_last_of('.');
  std::string outputFileName_plot = std::string(outputFileName, 0, idx);
  if ( idx != std::string::npos ) canvas->Print(std::string(outputFileName_plot).append(std::string(outputFileName, idx)).data());
  canvas->Print(std::string(outputFileName_plot).append(".png").data());
  canvas->Print(std::string(outputFileName_plot).append(".pdf").data());
  canvas->Print(std::string(outputFileName_plot).append(".root").data());
  
  delete legend;
  delete dummyHistogram_top;
  delete topPad;
  delete graph_shape;
  delete dummyHistogram_bottom;
  delete graph_line;
  for ( std::vector<TF1*>::iterator it = fitFunctionsToDelete.begin();
	it != fitFunctionsToDelete.end(); ++it ) {
    delete (*it);
  }
  delete graph_shape_div_fit;
  delete canvas;
}

std::pair<TH1*, TH1*> getHistogramsPass_and_Fail(TDirectory* inputDir_loose, const std::string& looseRegion, TDirectory* inputDir_tight, const std::string& tightRegion, 
							     const std::string& processData, const std::vector<std::string>& processesToSubtract, 
							     const std::string& histogramName)
{
  std::cout << "<getHistogramsPass_and_Fail>:" << std::endl;
  std::cout << " inputDir_loose = " << inputDir_loose << ": name = " << inputDir_loose->GetName() << std::endl;
  std::cout << " looseRegion = " << looseRegion << std::endl;
  std::cout << " inputDir_tight = " << inputDir_tight << ": name = " << inputDir_tight->GetName() << std::endl;
  std::cout << " tightRegion = " << tightRegion << std::endl;
  std::cout << " histogramName = " << histogramName << std::endl;

  TH1* histogramData_loose = getHistogram(inputDir_loose, processData, histogramName, "central", true);
  assert(histogramData_loose);
  std::cout << " histogramData_loose = " << histogramData_loose << ": name = " << histogramData_loose << ", integral = " << histogramData_loose->Integral() << std::endl;
  TH1* histogramData_tight = getHistogram(inputDir_tight, processData, histogramName, "central", true);
  assert(histogramData_tight);
  std::cout << " histogramData_tight = " << histogramData_tight << ": name = " << histogramData_tight << ", integral = " << histogramData_tight->Integral() << std::endl;
  checkCompatibleBinning(histogramData_loose, histogramData_tight);

  std::vector<TH1*> histogramsToSubtract_loose;
  std::vector<TH1*> histogramsToSubtract_tight;
  for ( vstring::const_iterator processToSubtract = processesToSubtract.begin();
	processToSubtract != processesToSubtract.end(); ++processToSubtract ) {
    TH1* histogramToSubtract_loose = getHistogram(inputDir_loose, *processToSubtract, histogramName, "central", true);
    histogramsToSubtract_loose.push_back(histogramToSubtract_loose);
    TH1* histogramToSubtract_tight = getHistogram(inputDir_tight, *processToSubtract, histogramName, "central", true);
    histogramsToSubtract_tight.push_back(histogramToSubtract_tight);
  }
      
  std::cout << "computing sum(histograms) in fail region" << std::endl;
  std::string histogramNameJetToTauFakeRate_fail = Form("jetToTauFakeRate_%s_%s_fail", histogramName.data(), looseRegion.data());
  TH1* histogramJetToTauFakeRate_fail = subtractHistograms(histogramNameJetToTauFakeRate_fail.data(), histogramData_loose, histogramsToSubtract_loose);  
  std::cout << "computing sum(histograms) in pass region" << std::endl;
  std::string histogramNameJetToTauFakeRate_pass = Form("jetToTauFakeRate_%s_%s_pass", histogramName.data(), tightRegion.data());
  TH1* histogramJetToTauFakeRate_pass = subtractHistograms(histogramNameJetToTauFakeRate_pass.data(), histogramData_tight, histogramsToSubtract_tight);

  return std::pair<TH1*, TH1*>(histogramJetToTauFakeRate_pass, histogramJetToTauFakeRate_fail);
}

void compFakeRate(double nPass, double nPassErr, double nFail, double nFailErr, double& avFakeRate, double& avFakeRateErrUp, double& avFakeRateErrDown, bool& errorFlag)
{ 
  double sumWeights = nPass + nFail;
  double sumWeights2 = square(nPassErr) + square(nFailErr);
  if ( sumWeights > 0. && sumWeights2 > 0. ) {
    double n_eff = square(sumWeights/TMath::Sqrt(sumWeights2));
    double sf = n_eff/(sumWeights);
    Int_t nPass_int = TMath::Nint(sf*nPass);
    TH1* histogram_pass_tmp = new TH1D("histogram_pass_tmp", "histogram_pass_tmp", 1, -0.5, +0.5);
    histogram_pass_tmp->SetBinContent(1, nPass_int);
    histogram_pass_tmp->SetBinError(1, TMath::Sqrt(nPass_int));
    Int_t nFail_int = TMath::Nint(sf*nFail);
    TH1* histogram_fail_tmp = new TH1D("histogram_fail_tmp", "histogram_fail_tmp", 1, -0.5, +0.5);
    histogram_fail_tmp->SetBinContent(1, nFail_int);
    histogram_fail_tmp->SetBinError(1, TMath::Sqrt(nFail_int));
    TGraphAsymmErrors* graph_pass_div_fail_tmp = new TGraphAsymmErrors(1);
    graph_pass_div_fail_tmp->Divide(histogram_pass_tmp, histogram_fail_tmp, "pois");
    double dummy;
    graph_pass_div_fail_tmp->GetPoint(0, dummy, avFakeRate);
    avFakeRateErrUp   = graph_pass_div_fail_tmp->GetErrorYhigh(0);
    avFakeRateErrDown = graph_pass_div_fail_tmp->GetErrorYlow(0);
    delete histogram_pass_tmp;
    delete histogram_fail_tmp;
    delete graph_pass_div_fail_tmp;
    errorFlag = false;
  } else {
    avFakeRate        = 0.5;
    avFakeRateErrUp   = 0.5;
    avFakeRateErrDown = 0.5;
    errorFlag = true;
  }
}

int main(int argc, char* argv[]) 
{
//--- parse command-line arguments
  if ( argc < 2 ) {
    std::cout << "Usage: " << argv[0] << " [parameters.py]" << std::endl;
    return 0;
  }

  std::cout << "<determineJetToTauFakeRate>:" << std::endl;

//--- keep track of time it takes the macro to execute
  TBenchmark clock;
  clock.Start("determineJetToTauFakeRate");

//--- read python configuration parameters
  if ( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") ) 
    throw cms::Exception("determineJetToTauFakeRate") 
      << "No ParameterSet 'process' found in configuration file = " << argv[1] << " !!\n";

  edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

  edm::ParameterSet cfgDetermineJetToTauFakeRate = cfg.getParameter<edm::ParameterSet>("determineJetToTauFakeRate");
  
  std::string type = cfgDetermineJetToTauFakeRate.getParameter<std::string>("type");

  std::string looseRegion = cfgDetermineJetToTauFakeRate.getParameter<std::string>("looseRegion");
  std::string tightRegion = cfgDetermineJetToTauFakeRate.getParameter<std::string>("tightRegion");

  std::string processData = cfgDetermineJetToTauFakeRate.getParameter<std::string>("processData");
  std::string processQCD  = cfgDetermineJetToTauFakeRate.getParameter<std::string>("processQCD");
  vstring processesToSubtract = cfgDetermineJetToTauFakeRate.getParameter<vstring>("processesToSubtract");
  
  std::string category = cfgDetermineJetToTauFakeRate.getParameter<std::string>("category");

  std::string tauPtBin = cfgDetermineJetToTauFakeRate.getParameter<std::string>("tauPtBin");

  std::string particle1EtaBin = cfgDetermineJetToTauFakeRate.getParameter<std::string>("particle1EtaBin");
  std::string particle2EtaBin = cfgDetermineJetToTauFakeRate.getParameter<std::string>("particle2EtaBin");

  vstring histogramsToFit = cfgDetermineJetToTauFakeRate.getParameter<vstring>("histogramsToFit");

  std::string fitFunction_formula = cfgDetermineJetToTauFakeRate.getParameter<std::string>("fitFunction");
  std::cout << "fitFunction_formula = " << fitFunction_formula << std::endl;
  std::map<std::string, double> initialParameters; // key = fitParameterName
  if ( cfgDetermineJetToTauFakeRate.exists("initialParameters") ) {
    edm::ParameterSet cfgInitialParameters = cfgDetermineJetToTauFakeRate.getParameter<edm::ParameterSet>("initialParameters");
    vstring fitParameterNames = cfgInitialParameters.getParameterNamesForType<double>();
    for ( vstring::const_iterator fitParameterName = fitParameterNames.begin();
	  fitParameterName != fitParameterNames.end(); ++fitParameterName ) {
      double initialParameter_value = cfgInitialParameters.getParameter<double>(*fitParameterName);
      initialParameters[*fitParameterName] = initialParameter_value;
    }
  }
  double xMin = cfgDetermineJetToTauFakeRate.getParameter<double>("xMin");
  double xMax = cfgDetermineJetToTauFakeRate.getParameter<double>("xMax");

  fwlite::InputSource inputFiles(cfg); 
  if ( !(inputFiles.files().size() == 1) )
    throw cms::Exception("determineJetToTauFakeRate") 
      << "Exactly one input file expected !!\n";
  TFile* inputFile = new TFile(inputFiles.files().front().data());

  fwlite::OutputFiles outputFile(cfg);
  fwlite::TFileService fs = fwlite::TFileService(outputFile.file().data());

  TDirectory* inputDir_loose = getDirectory(inputFile, looseRegion, category, tauPtBin, true);
  assert(inputDir_loose);
  std::cout << "inputDir_loose = " << inputDir_loose << ": name = " << inputDir_loose->GetName() << std::endl;
  TDirectory* inputDir_tight = getDirectory(inputFile, tightRegion, category, tauPtBin, true);
  assert(inputDir_tight);
  std::cout << "inputDir_tight = " << inputDir_tight << ": name = " << inputDir_tight->GetName() << std::endl;

  TDirectory* outputDir = createSubdirectory_recursively(fs, Form("%s/%s/%s%s", type.data(), category.data(), particle1EtaBin.data(), particle2EtaBin.data()));
  outputDir->cd();

  std::pair<TH1*, TH1*> histogram_pass_and_fail = getHistogramsPass_and_Fail(
    inputDir_loose, looseRegion, inputDir_tight, tightRegion, 
    processData, processesToSubtract, 
    Form("EventCounter_%s%s", particle1EtaBin.data(), particle2EtaBin.data()));
  double nPass, nPassErr;
  compIntegral_and_Error(histogram_pass_and_fail.first, nPass, nPassErr);
  double nFail, nFailErr;
  compIntegral_and_Error(histogram_pass_and_fail.second, nFail, nFailErr);
  double avJetToTauFakeRate, avJetToTauFakeRateErrUp, avJetToTauFakeRateErrDown;
  bool errorFlag;
  compFakeRate(nPass, nPassErr, nFail, nFailErr, avJetToTauFakeRate, avJetToTauFakeRateErrUp, avJetToTauFakeRateErrDown, errorFlag);
  std::cout << "nPass = " << nPass << " +/- " << nPassErr << ", nFail = " << nFail << " +/- " << nFailErr 
	    << " --> avJetToTauFakeRate = " << avJetToTauFakeRate << " + " << avJetToTauFakeRateErrUp << " - " << avJetToTauFakeRateErrDown << std::endl;

  std::string fitFunctionNormName = Form("fitFunctionNorm_%s_div_%s", tightRegion.data(), looseRegion.data());
  TF1* fitFunctionNorm = new TF1(fitFunctionNormName.data(), Form("%f", avJetToTauFakeRate), xMin, xMax);
  fitFunctionNorm->Write();
  std::string fitFunctionNormUpName = Form("fitFunctionNormUp_%s_div_%s", tightRegion.data(), looseRegion.data());
  TF1* fitFunctionNormUp = new TF1(fitFunctionNormUpName.data(), Form("%f", avJetToTauFakeRate + avJetToTauFakeRateErrUp), xMin, xMax);
  fitFunctionNormUp->Write();
  std::string fitFunctionNormDownName = Form("fitFunctionNormDown_%s_div_%s", tightRegion.data(), looseRegion.data());
  TF1* fitFunctionNormDown = new TF1(fitFunctionNormDownName.data(), Form("%f", TMath::Max(0., avJetToTauFakeRate - avJetToTauFakeRateErrDown)), xMin, xMax);
  fitFunctionNormDown->Write();

  for ( vstring::const_iterator histogramToFit = histogramsToFit.begin();
	histogramToFit != histogramsToFit.end(); ++histogramToFit ) {
    std::cout << "fitting " << (*histogramToFit) << ":" << std::endl;

    std::string particleEtaBin = "";
    if ( histogramToFit->find("tau1") != std::string::npos || histogramToFit->find("bJet1") != std::string::npos ) particleEtaBin.append(particle1EtaBin);
    if ( histogramToFit->find("tau2") != std::string::npos || histogramToFit->find("bJet2") != std::string::npos ) particleEtaBin.append(particle2EtaBin);
    std::pair<TH1*, TH1*> histogram_pass_and_fail = getHistogramsPass_and_Fail(
      inputDir_loose, looseRegion, inputDir_tight, tightRegion, 
      processData, processesToSubtract, 
      Form("%s_%s", histogramToFit->data(), particleEtaBin.data()));
    TH1* histogram_pass = histogram_pass_and_fail.first;
    TH1* histogram_fail = histogram_pass_and_fail.second;

    assert(histogram_pass->GetNbinsX() == histogram_fail->GetNbinsX());
    int numBins = histogram_fail->GetNbinsX();
    std::vector<double> points_x;
    std::vector<double> points_xErrUp;
    std::vector<double> points_xErrDown;
    std::vector<double> points_y;
    std::vector<double> points_yErrUp;
    std::vector<double> points_yErrDown;
    for ( int iBin = 0; iBin < numBins; ++iBin ) {
      double nPass = histogram_pass->GetBinContent(iBin);
      double nPassErr = histogram_pass->GetBinError(iBin);
      double nFail = histogram_fail->GetBinContent(iBin);
      double nFailErr = histogram_fail->GetBinError(iBin);     
      double jetToTauFakeRate, jetToTauFakeRateErrUp, jetToTauFakeRateErrDown;
      compFakeRate(nPass, nPassErr, nFail, nFailErr, jetToTauFakeRate, jetToTauFakeRateErrUp, jetToTauFakeRateErrDown, errorFlag);
      std::cout << "bin #" << iBin << "(x = " << histogram_pass->GetBinCenter(iBin) << ")" << ":" 
		<< " nPass = " << nPass << " +/- " << nPassErr << ", nFail = " << nFail << " +/- " << nFailErr 
		<< " --> jetToTauFakeRate = " << jetToTauFakeRate << " + " << jetToTauFakeRateErrUp << " - " << jetToTauFakeRateErrDown << std::endl;
      if ( errorFlag ) continue;
      assert(TMath::Abs(histogram_fail->GetBinCenter(iBin) - histogram_pass->GetBinCenter(iBin)) < 1.e-3*TMath::Abs(histogram_fail->GetBinCenter(iBin) + histogram_pass->GetBinCenter(iBin)));
      TAxis* xAxis = histogram_fail->GetXaxis();
      double x = xAxis->GetBinCenter(iBin);
      points_x.push_back(x);
      double y = jetToTauFakeRate/avJetToTauFakeRate;
      //double y = jetToTauFakeRate; // CV: only to be used for the purpose of making control plots !!
      points_y.push_back(y);
      double xErrUp = xAxis->GetBinUpEdge(iBin) - xAxis->GetBinCenter(iBin);
      points_xErrUp.push_back(xErrUp);
      double xErrDown = xAxis->GetBinCenter(iBin) - xAxis->GetBinLowEdge(iBin);
      points_xErrDown.push_back(xErrDown);
      double yErrUp = jetToTauFakeRateErrUp/avJetToTauFakeRate;
      //double yErrUp = jetToTauFakeRateErrUp; // CV: only to be used for the purpose of making control plots !!
      points_yErrUp.push_back(yErrUp);
      double yErrDown = jetToTauFakeRateErrDown/avJetToTauFakeRate;
      //double yErrDown = jetToTauFakeRateErrDown; // CV: only to be used for the purpose of making control plots !!
      points_yErrDown.push_back(yErrDown);
    }
    int numPoints = points_x.size();
    TGraphAsymmErrors* graph_pass_div_fail = new TGraphAsymmErrors(numPoints);
    for ( int iPoint = 0; iPoint < numPoints; ++iPoint ) {
      graph_pass_div_fail->SetPoint(iPoint, points_x[iPoint], points_y[iPoint]);
      graph_pass_div_fail->SetPointError(iPoint, points_xErrDown[iPoint], points_xErrUp[iPoint], points_yErrDown[iPoint], points_yErrUp[iPoint]);
    }
    std::string graphName_pass_div_fail = Form("%s_%s_%s_div_%s", type.data(), histogramToFit->data(), tightRegion.data(), looseRegion.data());
    graph_pass_div_fail->SetName(graphName_pass_div_fail.data());
    graph_pass_div_fail->Write();

    std::string fitFunctionShapeName = Form("fitFunctionShape_%s_%s_div_%s", histogramToFit->data(), tightRegion.data(), looseRegion.data());
    double x0 = histogram_fail->GetMean();
    std::string fitFunction_formula_wrt_x0 = TString(fitFunction_formula.data()).ReplaceAll("x", Form("(x - %f)", x0)).Data();
    TF1* fitFunctionShape = new TF1(fitFunctionShapeName.data(), fitFunction_formula_wrt_x0.data(), xMin, xMax);
    int numFitParameter = fitFunctionShape->GetNpar();
    for ( int iFitParameter = 0; iFitParameter < numFitParameter; ++iFitParameter ) {
      std::string fitParameterName = Form("p%i", iFitParameter);
      if ( initialParameters.find(fitParameterName) != initialParameters.end() ) {
	double initialParameter_value = initialParameters[fitParameterName];
	std::cout << "initializing fitParameter #" << iFitParameter << " = " << initialParameter_value << std::endl;
	fitFunctionShape->SetParameter(iFitParameter, initialParameter_value);
      }
    }

    TFitResultPtr fitResult = graph_pass_div_fail->Fit(fitFunctionShape, "ERNS");
    std::vector<fitFunction_and_legendEntry> fitFunctions_sysShifts;
    if ( fitResult->IsValid() ) {
      fitFunctionShape->Write();
      TMatrixD cov = fitResult->GetCovarianceMatrix();
      std::vector<EigenVector_and_Value> eigenVectors_and_Values = compEigenVectors_and_Values(cov);
      size_t dimension = fitFunctionShape->GetNpar();
      assert(eigenVectors_and_Values.size() == dimension);
      int idxPar = 1;
      for ( std::vector<EigenVector_and_Value>::const_iterator eigenVector_and_Value = eigenVectors_and_Values.begin();
	    eigenVector_and_Value != eigenVectors_and_Values.end(); ++eigenVector_and_Value ) {
	assert(eigenVector_and_Value->eigenVector_.GetNrows() == (int)dimension);
	std::cout << "EigenVector #" << idxPar << ":" << std::endl;
	eigenVector_and_Value->eigenVector_.Print();
	std::cout << "EigenValue #" << idxPar << " = " << eigenVector_and_Value->eigenValue_ << std::endl;
	assert(eigenVector_and_Value->eigenValue_ >= 0.);
	std::string fitFunctionShapeParUpName = Form("fitFunctionShapePar%iUp_%s_%s_div_%s", idxPar, histogramToFit->data(), tightRegion.data(), looseRegion.data());
	TF1* fitFunctionShapeParUp = new TF1(fitFunctionShapeParUpName.data(), fitFunction_formula_wrt_x0.data(), xMin, xMax);
	for ( size_t iComponent = 0; iComponent < dimension; ++iComponent ) {    
	  fitFunctionShapeParUp->SetParameter(
            iComponent, 
	    fitFunctionShape->GetParameter(iComponent) + TMath::Sqrt(eigenVector_and_Value->eigenValue_)*eigenVector_and_Value->eigenVector_(iComponent));
	}
	fitFunctions_sysShifts.push_back(fitFunction_and_legendEntry(fitFunctionShapeParUp, Form("EigenVec #%i", idxPar)));
	fitFunctionShapeParUp->Write();
	std::string fitFunctionShapeParDownName = Form("fitFunctionShapePar%iDown_%s_%s_div_%s", idxPar, histogramToFit->data(), tightRegion.data(), looseRegion.data());
	TF1* fitFunctionShapeParDown = new TF1(fitFunctionShapeParDownName.data(), fitFunction_formula_wrt_x0.data(), xMin, xMax);
	for ( size_t iComponent = 0; iComponent < dimension; ++iComponent ) {    
	  fitFunctionShapeParDown->SetParameter(
            iComponent, 
	    fitFunctionShape->GetParameter(iComponent) - TMath::Sqrt(eigenVector_and_Value->eigenValue_)*eigenVector_and_Value->eigenVector_(iComponent));
	}
	fitFunctions_sysShifts.push_back(fitFunction_and_legendEntry(fitFunctionShapeParDown, Form("EigenVec #%i", idxPar)));
	fitFunctionShapeParDown->Write();
	++idxPar;
      }    
    } else {
      std::cerr << "Warning: Fit failed to converge --> setting fitFunction to constant value !!" << std::endl;
      delete fitFunctionShape;
      fitFunctionShape = new TF1(fitFunctionShapeName.data(), "1.0", xMin, xMax);
      fitFunctionShape->Write();
    }
    std::string controlPlotFileName = TString(outputFile.file().data()).ReplaceAll(".root", Form("_%s_controlPlot.png", histogramToFit->data())).Data();    
    makeControlPlot(graph_pass_div_fail, avJetToTauFakeRate, avJetToTauFakeRate + avJetToTauFakeRateErrUp, TMath::Max(0., avJetToTauFakeRate - avJetToTauFakeRateErrDown), 
		    fitFunctionShape, fitFunctions_sysShifts, xMin, xMax, "P_{T} [GeV]", 1.e-1, 1.e+1, controlPlotFileName);
  }

  delete inputFile;

  clock.Show("determineJetToTauFakeRate");

  return 0;
}
