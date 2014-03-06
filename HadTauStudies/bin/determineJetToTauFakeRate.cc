
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

namespace
{
  TDirectory* getDirectory(TFile* inputFile, const std::string& region, const std::string& category, const std::string& tauPtBin, bool enableException)
  {
    std::string dirName = Form("tauTau_%s_%s/%s", region.data(), category.data(), tauPtBin.data());
    TDirectory* dir = dynamic_cast<TDirectory*>(inputFile->Get(dirName.data()));
    if ( enableException && !dir )
      throw cms::Exception("getDirectory") 
	<< "Failed to find directory for category = " << category << ", tauPtBin = " << tauPtBin << " !!\n";
    return dir;
  }

  TH1* getHistogram(TDirectory* dir, const std::string& process, const std::string& histogramName, const std::string& central_or_shift, bool enableException)
  {
    std::string histogramName_full = Form("%s/%s", process.data(), process.data());
    if ( !(central_or_shift == "" || central_or_shift == "central") ) histogramName_full.append("_").append(central_or_shift);
    histogramName_full.append("_").append(histogramName);
    TH1* histogram = dynamic_cast<TH1*>(dir->Get(histogramName_full.data()));
    if ( enableException && !histogram ) 
      throw cms::Exception("getHistogram") 
	<< "Failed to find histogram = " << histogramName_full << " in directory = " << dir->GetName() << " !!\n";    
    return histogram;
  }
  
  TDirectory* createSubdirectory(TDirectory* dir, const std::string& subdirName)
  {
    dir->cd();
    if ( !dir->Get(subdirName.data()) ) {
      dir->mkdir(subdirName.data());
    }
    TDirectory* subdir = dynamic_cast<TDirectory*>(dir->Get(subdirName.data()));
    assert(subdir);
    return subdir;
  }
  
  TDirectory* createSubdirectory_recursively(TFileDirectory& dir, const std::string& fullSubdirName)
  {
    TString fullSubdirName_tstring = fullSubdirName.data();
    TObjArray* subdirNames = fullSubdirName_tstring.Tokenize("/");
    int numSubdirectories = subdirNames->GetEntries();
    TDirectory* parent = dir.getBareDirectory();
    for ( int iSubdirectory = 0; iSubdirectory < numSubdirectories; ++iSubdirectory ) {
      const TObjString* subdirName = dynamic_cast<TObjString*>(subdirNames->At(iSubdirectory));
      assert(subdirName);
      TDirectory* subdir = createSubdirectory(parent, subdirName->GetString().Data());
      parent = subdir;
    }
    return parent;
  }

  //-------------------------------------------------------------------------------
  void getBinomialBounds(Int_t n, Int_t r, Float_t& rMin, Float_t& rMax)
  {
    rMin = 0.;
    rMax = 0.;
    
    if ( n == 0 ){
      return;
    }
    if ( r < 0 ){
      std::cerr << "Error in <getBinomialBounds>: n = " << n << ", r = " << r << std::endl;
      return;
    }
    
    if ( ((Double_t)r*(n - r)) > (9.*n) ){
      rMin = r - TMath::Sqrt((Double_t)r*(n - r)/((Double_t)n));
      rMax = r + TMath::Sqrt((Double_t)r*(n - r)/((Double_t)n));
      return;
    }
    
    Double_t binomialCoefficient = 1.;
    
    Double_t rMinLeft       = 0.;
    Double_t rMinMiddle     = TMath::Max(0.5*r, n - 1.5*r);
    Double_t rMinRight      = n;
    Double_t rMinLeftProb   = 0.;
    Double_t rMinMiddleProb = 0.5;
    Double_t rMinRightProb  = 1.;
    while ( (rMinRight - rMinLeft) > (0.001*n) ){
      
      rMinMiddleProb = 0;
      for ( Int_t i = r; i <= n; i++ ){
	binomialCoefficient = 1;
	
	for ( Int_t j = n; j > i; j-- ){
	  binomialCoefficient *= j/((Double_t)(j - i));
	}
	
	rMinMiddleProb += binomialCoefficient*TMath::Power(rMinMiddle/((Double_t)(n)), i)
	                 *TMath::Power((n - rMinMiddle)/((Double_t)(n)), n - i);
      }
      
      if ( rMinMiddleProb > 0.16 ){
	rMinRight     = rMinMiddle;
	rMinRightProb = rMinMiddleProb;
      } else if ( rMinMiddleProb < 0.16 ){
	rMinLeft      = rMinMiddle;
	rMinLeftProb  = rMinMiddleProb;
      } else {
	rMinLeft      = rMinRight     = rMinMiddle;
	rMinLeftProb  = rMinRightProb = rMinMiddleProb;
      }
      
      rMinMiddle = 0.5*(rMinLeft + rMinRight);
      
      if ( rMinLeft > r ){
	rMinMiddle = rMinLeft = rMinRight = 0;
      }
    }
    
    Double_t rMaxLeft       = 0.;
    Double_t rMaxMiddle     = TMath::Min(1.5*r, n - 0.5*r);
    Double_t rMaxRight      = n;
    Double_t rMaxLeftProb   = 1.;
    Double_t rMaxMiddleProb = 0.5;
    Double_t rMaxRightProb  = 0.;
    while ( (rMaxRight - rMaxLeft) > (0.001*n) ){
      
      rMaxMiddleProb = 0;
      for ( Int_t i = 0; i <= r; i++ ){
	binomialCoefficient = 1;
	
	for ( Int_t j = n; j > (n - i); j-- ){
	  binomialCoefficient *= j/((Double_t)(i - (n - j)));
	}
	
	rMaxMiddleProb += binomialCoefficient*TMath::Power(rMaxMiddle/((Double_t)(n)), i)
	                 *TMath::Power((n - rMaxMiddle)/((Double_t)(n)), n - i);
      }

      if ( rMaxMiddleProb > 0.16 ){
	rMaxLeft      = rMaxMiddle;
	rMaxLeftProb  = rMaxMiddleProb;
      } else if ( rMaxMiddleProb < 0.16 ){
	rMaxRight     = rMaxMiddle;
	rMaxRightProb = rMaxMiddleProb;
      } else {
	rMaxLeft      = rMaxRight     = rMaxMiddle;
	rMaxLeftProb  = rMaxRightProb = rMaxMiddleProb;
      }
      
      rMaxMiddle = 0.5*(rMaxLeft + rMaxRight);
      
      if ( rMaxRight < r ){
	rMaxMiddle = rMaxLeft = rMaxRight = n;
      }
    }
    
    rMin = rMinMiddle;
    rMax = rMaxMiddle;
  }
  
  TGraphAsymmErrors* getEfficiency(const TH1* histogram_numerator, const TH1* histogram_denominator)
  {
    Int_t error = 0;
    if ( !(histogram_numerator->GetNbinsX()           == histogram_denominator->GetNbinsX())           ) error = 1;
    if ( !(histogram_numerator->GetXaxis()->GetXmin() == histogram_denominator->GetXaxis()->GetXmin()) ) error = 1;
    if ( !(histogram_numerator->GetXaxis()->GetXmax() == histogram_denominator->GetXaxis()->GetXmax()) ) error = 1;
    
    if ( error ){
      std::cerr << "Error in <getEfficiency>: Dimensionality of histograms does not match !!" << std::endl;
      return 0;
    }
    
    TAxis* xAxis = histogram_numerator->GetXaxis();
    
    Int_t nBins = xAxis->GetNbins();
    TArrayF x(nBins);
    TArrayF dxUp(nBins);
    TArrayF dxDown(nBins);
    TArrayF y(nBins);
    TArrayF dyUp(nBins);
    TArrayF dyDown(nBins);
    
    for ( Int_t ibin = 1; ibin <= nBins; ibin++ ){
      Int_t nObs = TMath::Nint(histogram_denominator->GetBinContent(ibin));
      Int_t rObs = TMath::Nint(histogram_numerator->GetBinContent(ibin));
      
      Float_t xCenter = histogram_denominator->GetBinCenter(ibin);
      Float_t xWidth  = histogram_denominator->GetBinWidth(ibin);
      
      x[ibin - 1]      = xCenter;
      dxUp[ibin - 1]   = 0.5*xWidth;
      dxDown[ibin - 1] = 0.5*xWidth;
      
      if ( nObs > 0 ){
	Float_t rMin = 0.;
	Float_t rMax = 0.;
	
	getBinomialBounds(nObs, rObs, rMin, rMax);
	
	y[ibin - 1]      = rObs/((Float_t)nObs);
	dyUp[ibin - 1]   = (rMax - rObs)/((Float_t)nObs);
	dyDown[ibin - 1] = (rObs - rMin)/((Float_t)nObs);
      } else{
	y[ibin - 1]      = 0.;
	dyUp[ibin - 1]   = 0.;
	dyDown[ibin - 1] = 0.;
      }
    }
    
    TString name  = TString(histogram_numerator->GetName()).Append("Graph");
    TString title = histogram_numerator->GetTitle();
    
    TGraphAsymmErrors* graph = 
      new TGraphAsymmErrors(nBins, x.GetArray(), y.GetArray(), 
			    dxDown.GetArray(), dxUp.GetArray(), dyDown.GetArray(), dyUp.GetArray());
    
    graph->SetName(name);
    graph->SetTitle(title);
    
    return graph;
  }
  //-------------------------------------------------------------------------------
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
    //std::cout << "vec1:" << std::endl;
    //vec1.Print();
    TVectorD vec2 = eigenValue*eigenVector;
    //std::cout << "vec2:" << std::endl;
    //vec2.Print();
    // CV: check that EigenVector is indeed an EigenVector,
    //     i.e. that we interpreted the ordering of columns and rows of the eigenVectors matrix correctly
    for ( int iComponent = 0; iComponent < dimension; ++iComponent ) {   
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
		     TF1* fitFunction_central, std::vector<fitFunction_and_legendEntry>& fitFunctions_sysShifts, 
		     double xMin, double xMax, const std::string& xAxisTitle, 
		     double yMin, double yMax, 
		     const std::string& outputFileName)
{
  if ( fitFunctions_sysShifts.size() > 3 )
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
  yAxis_top->SetTitle("Fake-rate");
  yAxis_top->SetTitleOffset(1.2);

  dummyHistogram_top->Draw();

  TLegend* legend = new TLegend(0.51, 0.95*0.05*(1 + fitFunctions_sysShifts.size()/2), 0.95, 0.95, "", "brNDC"); 
  legend->SetBorderSize(0);
  legend->SetFillColor(0);

  fitFunction_central->SetLineColor(2);
  fitFunction_central->Draw("Lsame");
  legend->AddEntry(fitFunction_central, "Best Fit", "L");    

  int colors[3] = { 4, 6, 7 };

  int idx_top = 0;
  for ( std::vector<fitFunction_and_legendEntry>::const_iterator fitFunction_sysShift = fitFunctions_sysShifts.begin();
	fitFunction_sysShift != fitFunctions_sysShifts.end(); ++fitFunction_sysShift ) {
    int iColor = idx_top / 2;
    assert(iColor <= 2);
    fitFunction_sysShift->fitFunction_->SetLineColor(colors[iColor]);
    fitFunction_sysShift->fitFunction_->Draw("Lsame");
    legend->AddEntry(fitFunction_sysShift->fitFunction_, fitFunction_sysShift->legendEntry_.data(), "L");
    ++idx_top;
  }

  graph->SetMarkerStyle(8);
  graph->SetMarkerColor(1);
  graph->Draw("p");

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
  
  dummyHistogram_bottom->SetMinimum(-0.20);
  dummyHistogram_bottom->SetMaximum(+0.20);

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
  yAxis_bottom->SetTitle("");
  yAxis_bottom->SetTitleOffset(0.85);
  yAxis_bottom->SetNdivisions(505);
  yAxis_bottom->CenterTitle();
  yAxis_bottom->SetTitleSize(0.08);
  yAxis_bottom->SetLabelSize(0.08);
  yAxis_bottom->SetTickLength(0.04);

  dummyHistogram_bottom->SetTitle("");
  dummyHistogram_bottom->SetStats(false);
  dummyHistogram_bottom->Draw();

  int numPoints = graph->GetN();
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
  for ( std::vector<fitFunction_and_legendEntry>::const_iterator fitFunction_sysShift = fitFunctions_sysShifts.begin();
	fitFunction_sysShift != fitFunctions_sysShifts.end(); ++fitFunction_sysShift ) {
    TF1* fitFunction_numerator = fitFunction_sysShift->fitFunction_;
    TString fitFunction_formula_numerator = fitFunction_numerator->GetTitle();
    int numPar_numerator = fitFunction_numerator->GetNpar();
    TF1* fitFunction_denominator = fitFunction_central;
    TString fitFunction_formula_denominator = fitFunction_denominator->GetTitle();
    int numPar_denominator = fitFunction_denominator->GetNpar();
    for ( int iPar = 0; iPar < numPar_denominator; ++iPar ) {
      fitFunction_formula_denominator.ReplaceAll(Form("[%i]", iPar), Form("[%i]", iPar + numPar_numerator));      
    }
    std::string fitFunctionName_sysShift_div_central = Form("%s_div_central", fitFunction_numerator->GetName());
    std::string fitFunction_formula_sysShift_div_central = Form("(%s)/(%s) - 1.0", fitFunction_formula_numerator.Data(), fitFunction_formula_denominator.Data());
    TF1* fitFunction_sysShift_div_central = new TF1(fitFunctionName_sysShift_div_central.data(), fitFunction_formula_sysShift_div_central.data(), xMin, xMax);
    for ( int iPar_numerator = 0; iPar_numerator < numPar_denominator; ++iPar_numerator ) {
      fitFunction_sysShift_div_central->SetParameter(iPar_numerator, fitFunction_denominator->GetParameter(iPar_numerator));
    }
    for ( int iPar_denominator = 0; iPar_denominator < numPar_denominator; ++iPar_denominator ) {
      fitFunction_sysShift_div_central->SetParameter(iPar_denominator + numPar_numerator, fitFunction_denominator->GetParameter(iPar_denominator));
    }
    int iColor = idx_bottom / 2;
    assert(iColor <= 2);
    fitFunction_sysShift_div_central->SetLineColor(colors[iColor]);
    fitFunction_sysShift_div_central->Draw("Lsame");
    fitFunctionsToDelete.push_back(fitFunction_sysShift_div_central);
    ++idx_bottom;
  }

  TGraphAsymmErrors* graph_div_fit = new TGraphAsymmErrors(numPoints);
  for ( int iPoint = 0; iPoint < numPoints; ++iPoint ) {
    double x, y;
    graph->GetPoint(iPoint, x, y);
    double fitFunctionValue = fitFunction_central->Eval(x);
    graph_div_fit->SetPoint(iPoint, x, y/fitFunctionValue - 1.0);
    double xErrUp = graph->GetErrorXhigh(iPoint);
    double xErrDown = graph->GetErrorXlow(iPoint);
    double yErrUp = graph->GetErrorYhigh(iPoint);
    double yErrDown = graph->GetErrorYlow(iPoint);
    graph_div_fit->SetPointError(iPoint, xErrDown, xErrUp, yErrDown/fitFunctionValue, yErrUp/fitFunctionValue);
  }
  graph_div_fit->SetMarkerStyle(8);
  graph_div_fit->SetMarkerColor(1);
  graph_div_fit->Draw("p");
  
  canvas->Update();
  size_t idx = outputFileName.find_last_of('.');
  std::string outputFileName_plot = std::string(outputFileName, 0, idx);
  if ( idx != std::string::npos ) canvas->Print(std::string(outputFileName_plot).append(std::string(outputFileName, idx)).data());
  canvas->Print(std::string(outputFileName_plot).append(".png").data());
  canvas->Print(std::string(outputFileName_plot).append(".pdf").data());
  
  delete legend;
  delete dummyHistogram_top;
  delete topPad;
  delete dummyHistogram_bottom;
  delete graph_line;
  for ( std::vector<TF1*>::iterator it = fitFunctionsToDelete.begin();
	it != fitFunctionsToDelete.end(); ++it ) {
    delete (*it);
  }
  delete graph_div_fit;
  delete canvas;
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
  
  std::string looseRegion = cfgDetermineJetToTauFakeRate.getParameter<std::string>("looseRegion");
  std::string tightRegion = cfgDetermineJetToTauFakeRate.getParameter<std::string>("tightRegion");

  std::string processData = cfgDetermineJetToTauFakeRate.getParameter<std::string>("processData");
  std::string processQCD  = cfgDetermineJetToTauFakeRate.getParameter<std::string>("processQCD");
  vstring processesToSubtract = cfgDetermineJetToTauFakeRate.getParameter<vstring>("processesToSubtract");
  
  std::string category = cfgDetermineJetToTauFakeRate.getParameter<std::string>("category");

  std::string tauPtBin = cfgDetermineJetToTauFakeRate.getParameter<std::string>("tauPtBin");

  vstring histogramsToFit = cfgDetermineJetToTauFakeRate.getParameter<vstring>("histogramsToFit");

  std::string fitFunction_formula = cfgDetermineJetToTauFakeRate.getParameter<std::string>("fitFunction");
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
  TDirectory* inputDir_tight = getDirectory(inputFile, tightRegion, category, tauPtBin, true);
  assert(inputDir_tight);

  for ( vstring::const_iterator histogramToFit = histogramsToFit.begin();
	histogramToFit != histogramsToFit.end(); ++histogramToFit ) {
    std::cout << "fitting " << (*histogramToFit) << ":" << std::endl;

    TH1* histogramData_loose = getHistogram(inputDir_loose, processData, *histogramToFit, "central", true);
    assert(histogramData_loose);
    TH1* histogramData_tight = getHistogram(inputDir_tight, processData, *histogramToFit, "central", true);
    assert(histogramData_tight);
    checkCompatibleBinning(histogramData_loose, histogramData_tight);

    std::vector<TH1*> histogramsToSubtract_loose;
    std::vector<TH1*> histogramsToSubtract_tight;
    for ( vstring::const_iterator processToSubtract = processesToSubtract.begin();
	  processToSubtract != processesToSubtract.end(); ++processToSubtract ) {
      TH1* histogramToSubtract_loose = getHistogram(inputDir_loose, *processToSubtract, *histogramToFit, "central", true);
      histogramsToSubtract_loose.push_back(histogramToSubtract_loose);
      TH1* histogramToSubtract_tight = getHistogram(inputDir_tight, *processToSubtract, *histogramToFit, "central", true);
      histogramsToSubtract_tight.push_back(histogramToSubtract_tight);
    }
    
    TDirectory* outputDir = createSubdirectory_recursively(fs, Form("jetToTauFakeRate/%s", category.data()));
    outputDir->cd();

    std::string histogramNameJetToTauFakeRate_denominator = Form("jetToTauFakeRate_%s_%s_denominator", histogramToFit->data(), looseRegion.data());
    TH1* histogramJetToTauFakeRate_denominator = subtractHistograms(histogramNameJetToTauFakeRate_denominator.data(), histogramData_loose, histogramsToSubtract_loose);
    std::string histogramNameJetToTauFakeRate_numerator = Form("jetToTauFakeRate_%s_%s_numerator", histogramToFit->data(), tightRegion.data());
    TH1* histogramJetToTauFakeRate_numerator = subtractHistograms(histogramNameJetToTauFakeRate_numerator.data(), histogramData_tight, histogramsToSubtract_tight);

    std::string graphNameJetToTauFakeRate = Form("jetToTauFakeRate_%s_%s_div_%s", histogramToFit->data(), tightRegion.data(), looseRegion.data());
    TGraphAsymmErrors* graphJetToTauFakeRate = getEfficiency(histogramJetToTauFakeRate_numerator, histogramJetToTauFakeRate_denominator);
    graphJetToTauFakeRate->SetName(graphNameJetToTauFakeRate.data());
    graphJetToTauFakeRate->Write();
    
    std::string fitFunctionName = Form("fitFunction_%s_%s_div_%s", histogramToFit->data(), tightRegion.data(), looseRegion.data());
    double x0 = histogramJetToTauFakeRate_denominator->GetMean();
    std::string fitFunction_formula_wrt_x0 = TString(fitFunction_formula.data()).ReplaceAll("x", Form("(x - %f)", x0)).Data();
    TF1* fitFunction = new TF1(fitFunctionName.data(), fitFunction_formula_wrt_x0.data(), xMin, xMax);
    fitFunction->Write();
    TFitResultPtr fitResult = graphJetToTauFakeRate->Fit(fitFunction, "ERNS");
    TMatrixD cov = fitResult->GetCovarianceMatrix();
    std::vector<EigenVector_and_Value> eigenVectors_and_Values = compEigenVectors_and_Values(cov);
    size_t dimension = fitFunction->GetNpar();
    assert(eigenVectors_and_Values.size() == dimension);
    std::vector<fitFunction_and_legendEntry> fitFunctions_sysShifts;
    int idxPar = 1;
    for ( std::vector<EigenVector_and_Value>::const_iterator eigenVector_and_Value = eigenVectors_and_Values.begin();
	  eigenVector_and_Value != eigenVectors_and_Values.end(); ++eigenVector_and_Value ) {
      assert(eigenVector_and_Value->eigenVector_.GetNrows() == (int)dimension);
      std::string fitFunctionUpName = Form("fitFunctionPar%iUp_%s_%s_div_%s", idxPar, histogramToFit->data(), tightRegion.data(), looseRegion.data());
      TF1* fitFunctionUp = new TF1(fitFunctionUpName.data(), fitFunction_formula.data(), xMin, xMax);
      for ( size_t iComponent = 0; iComponent < dimension; ++iComponent ) {    
	fitFunctionUp->SetParameter(iComponent, fitFunction->GetParameter(iComponent) + eigenVector_and_Value->eigenValue_*eigenVector_and_Value->eigenVector_(iComponent));
      }
      fitFunctions_sysShifts.push_back(fitFunction_and_legendEntry(fitFunction, Form("EigenVec #%i Up", idxPar)));
      fitFunctionUp->Write();
      std::string fitFunctionDownName = Form("fitFunctionPar%iDown_%s_%s_div_%s", idxPar, histogramToFit->data(), tightRegion.data(), looseRegion.data());
      TF1* fitFunctionDown = new TF1(fitFunctionDownName.data(), fitFunction_formula.data(), xMin, xMax);
      for ( size_t iComponent = 0; iComponent < dimension; ++iComponent ) {    
	fitFunctionDown->SetParameter(iComponent, fitFunction->GetParameter(iComponent) + eigenVector_and_Value->eigenValue_*eigenVector_and_Value->eigenVector_(iComponent));
      }
      fitFunctions_sysShifts.push_back(fitFunction_and_legendEntry(fitFunction, Form("EigenVec #%i Down", idxPar)));
      fitFunctionDown->Write();
      ++idxPar;
    }    
    
    std::string controlPlotFileName = TString(outputFile.file().data()).ReplaceAll(".root", "_controlPlot.png").Data();    
    makeControlPlot(graphJetToTauFakeRate, fitFunction, fitFunctions_sysShifts, xMin, xMax, *histogramToFit, 1.e-2, 1.e+1, controlPlotFileName);
  }

  delete inputFile;

  clock.Show("determineJetToTauFakeRate");

  return 0;
}
