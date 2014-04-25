
#include <TFile.h>
#include <TString.h>
#include <TH1.h>
#include <TMath.h>
#include <TROOT.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <assert.h>

enum { kCentral, kUp, kDown };

TH1* getHistogram(TFile* inputFile, const std::string& channel, const std::string& category, const std::string& tauPtBin, const std::string& process, int massPoint, int central_or_shift)
{
  std::string histogramName = Form("%s_OSiso1_iso2_TightBtag_%s/%s/%s%i/%s%i", channel.data(), category.data(), tauPtBin.data(), process.data(), massPoint, process.data(), massPoint);
  if      ( central_or_shift == kUp   ) histogramName.append("_CMS_htt_higgsPtReweight_8TeVUp");
  else if ( central_or_shift == kDown ) histogramName.append("_CMS_htt_higgsPtReweight_8TeVDown");
  histogramName.append("_EventCounter");
  TH1* histogram = dynamic_cast<TH1*>(inputFile->Get(histogramName.data()));
  if ( !histogram ) {
    std::cerr << "<getHistogram>: Failed to find histogram = " << histogramName << " in file = " << inputFile->GetName() << " !!" << std::endl;
    assert(0);
  }
  return histogram;
}

double compAccTimesEff_value(const TH1* histogramEventCounter, double intLumiData)
{
  double accTimesEff = histogramEventCounter->Integral()/intLumiData;
  return accTimesEff;
}

std::string getStringRep(double accTimesEff)
{
  //std::cout << "<getStringRep>:" << std::endl;
  //std::cout << " accTimesEff = " << accTimesEff << std::endl;
  double exponent = 0.;
  if ( accTimesEff > 0. ) {
    exponent = TMath::Floor(TMath::Log10(accTimesEff));
  } 
  double mantissa = accTimesEff/TMath::Power(10., exponent);
  TString retVal = Form("$%1.2f \\cdot 10^{%1.0f}$", mantissa, exponent);
  retVal.ReplaceAll(" \\cdot 10^{0}", "");
  if ( retVal == "$0.00$" ) retVal = "-";
  //std::cout << "--> returning retVal = '" << retVal << "'" << std::endl;
  return retVal.Data();
}

std::string compAccTimesEff(const TH1* histogramEventCounter, double intLumiData)
{
  double accTimesEff = compAccTimesEff_value(histogramEventCounter, intLumiData);
  return getStringRep(accTimesEff);
}

std::string getStringRep(double accTimesEff_central, double accTimesEff_up, double accTimesEff_down)
{
  //std::cout << "<getStringRep>:" << std::endl;
  //std::cout << " accTimesEff: central = " << accTimesEff_central << ", up = " << accTimesEff_up << ", down = " << accTimesEff_down << std::endl;  
  double exponent = 0.;
  if ( accTimesEff_central > 0. ) {
    exponent = TMath::Floor(TMath::Log10(accTimesEff_central));
  } else if ( accTimesEff_up > 0. ) {
    exponent = TMath::Floor(TMath::Log10(accTimesEff_up));
  } else if ( accTimesEff_down > 0. ) {
    exponent = TMath::Floor(TMath::Log10(accTimesEff_down));
  } 
  double mantissa_central = accTimesEff_central/TMath::Power(10., exponent);
  double mantissa_up = TMath::Abs(accTimesEff_up - accTimesEff_central)/TMath::Power(10., exponent);
  double mantissa_down = TMath::Abs(accTimesEff_central - accTimesEff_down)/TMath::Power(10., exponent);
  TString retVal = Form("$%1.2f^{+%1.2f}_{-%1.2f} \\cdot 10^{%1.0f}$", mantissa_central, mantissa_up, mantissa_down, exponent);
  retVal.ReplaceAll(" \\cdot 10^{0}", "");
  if ( retVal == "$0.00^{+0.00}_{-0.00}$" ) retVal = "-";
  //std::cout << "--> returning retVal = '" << retVal << "'" << std::endl;
  assert(accTimesEff_central > 0.);
  return retVal.Data();
}

std::string compAccTimesEff(const TH1* histogramEventCounter_central, const TH1* histogramEventCounter_up, const TH1* histogramEventCounter_down, double intLumiData)
{
  //std::cout << "<compAccTimesEff>:" << std::endl;
  double accTimesEff_central = compAccTimesEff_value(histogramEventCounter_central, intLumiData);
  double accTimesEff_up = compAccTimesEff_value(histogramEventCounter_up, intLumiData);
  double accTimesEff_down = compAccTimesEff_value(histogramEventCounter_down, intLumiData);
  double accTimesEff_max = TMath::Max(accTimesEff_up, accTimesEff_down);
  if ( accTimesEff_max < accTimesEff_central ) accTimesEff_max = accTimesEff_central;
  double accTimesEff_min = TMath::Min(accTimesEff_up, accTimesEff_down);
  if ( accTimesEff_min > accTimesEff_central ) accTimesEff_min = accTimesEff_central;
  return getStringRep(accTimesEff_central, accTimesEff_max, accTimesEff_min);
}

void makeAccTimesEffTable()
{
  gROOT->SetBatch(true);

  TH1::AddDirectory(false);

  std::string inputFilePath_unweighted = "/data1/veelken/tmp/tauTauAnalysis/v1_11_3_woHiggsPtReweighting/";
  std::string inputFileName_unweighted = "MVAwLToldDMsTight/hadd_stage5_MVAwLToldDMsTight.root";
  TFile* inputFile_unweighted = new TFile(Form("%s%s", inputFilePath_unweighted.data(), inputFileName_unweighted.data()));
  assert(inputFile_unweighted);

  std::string inputFilePath_weighted = "/data1/veelken/tmp/tauTauAnalysis/v1_11_3/";
  std::string inputFileName_weighted = "MVAwLToldDMsTight/hadd_stage5_MVAwLToldDMsTight.root";
  TFile* inputFile_weighted = new TFile(Form("%s%s", inputFilePath_weighted.data(), inputFileName_weighted.data()));
  assert(inputFile_weighted);

  std::string channel = "tauTau";

  typedef std::vector<std::string> vstring;
  vstring categories;
  categories.push_back("nobtag");
  categories.push_back("btag");

  std::map<std::string, vstring> tauPtBins; // key = category
  tauPtBins["nobtag"].push_back("tau1PtGt45tau2Pt45to60");
  tauPtBins["nobtag"].push_back("tau1PtGt45tau2Pt60to80");
  tauPtBins["nobtag"].push_back("tau1PtGt45tau2PtGt80");
  tauPtBins["btag"].push_back("tau1PtGt45tau2Pt45to60");
  tauPtBins["btag"].push_back("tau1PtGt45tau2PtGt60");
    
  typedef std::vector<int> vint;
  vint massPoints;
  massPoints.push_back(90);
  massPoints.push_back(100);
  massPoints.push_back(120);
  massPoints.push_back(130);
  massPoints.push_back(140);
  massPoints.push_back(160);
  massPoints.push_back(180);
  massPoints.push_back(200);
  massPoints.push_back(250);
  massPoints.push_back(300);
  massPoints.push_back(350);
  massPoints.push_back(400);
  massPoints.push_back(450);
  massPoints.push_back(500);
  massPoints.push_back(600);
  massPoints.push_back(700);
  massPoints.push_back(800);
  massPoints.push_back(900);
  massPoints.push_back(1000);

  std::map<std::string, double> intLumiData; // key = channel, NOTE: units = pb^-1
  intLumiData["tauTau"] = 18300; 
  intLumiData["eTau"]   = 19800;
  intLumiData["muTau"]  = 19800;

  std::string outputFileName = Form("makeAccTimesEffTable_%s.txt", channel.data());
  std::ofstream* outputFile = new std::ofstream(outputFileName.data(), std::ios::out);

  for ( vint::const_iterator massPoint = massPoints.begin();
	massPoint != massPoints.end(); ++massPoint ) {
    (*outputFile) << "$" << (*massPoint) << "$~\\GeV & $\\Pg\\Pg \\to \\Phi \\to \\tau\\tau$";
    for ( vstring::const_iterator category = categories.begin();
	  category != categories.end(); ++category ) {
      for ( vstring::const_iterator tauPtBin = tauPtBins[*category].begin();
	    tauPtBin != tauPtBins[*category].end(); ++tauPtBin ) {
	(*outputFile) << " & ";
	TH1* histogramEventCounter_central = getHistogram(inputFile_weighted, channel, *category, *tauPtBin, "ggH", *massPoint, kCentral);
	TH1* histogramEventCounter_up      = getHistogram(inputFile_weighted, channel, *category, *tauPtBin, "ggH", *massPoint, kUp);
	TH1* histogramEventCounter_down    = getHistogram(inputFile_weighted, channel, *category, *tauPtBin, "ggH", *massPoint, kDown);
	std::string key_ggH = Form("ggPhi_%i", *massPoint);
	(*outputFile) << compAccTimesEff(histogramEventCounter_central, histogramEventCounter_up, histogramEventCounter_down, intLumiData[channel]);
	//TH1* histogramEventCounter = getHistogram(inputFile_unweighted, channel, *category, *tauPtBin, "ggH", *massPoint, kCentral);
	//(*outputFile) << " (" << compAccTimesEff(histogramEventCounter, intLumiData[channel]) << ")";
      }
    }
    (*outputFile) << " \\\\" << std::endl;
    (*outputFile) << " & $\\Pg\\Pg \\to \\Phi b \\to \\tau\\tau b$";
    for ( vstring::const_iterator category = categories.begin();
	  category != categories.end(); ++category ) {
      for ( vstring::const_iterator tauPtBin = tauPtBins[*category].begin();
	    tauPtBin != tauPtBins[*category].end(); ++tauPtBin ) {
	(*outputFile) << " & ";
	TH1* histogramEventCounter = getHistogram(inputFile_unweighted, channel, *category, *tauPtBin, "bbH", *massPoint, kCentral);
	std::string key_bbH = Form("bbPhi_%i", *massPoint);
	(*outputFile) << compAccTimesEff(histogramEventCounter, intLumiData[channel]);
      }
    }
    (*outputFile) << " \\\\" << std::endl;
    (*outputFile) << "\\hline" << std::endl;
  }

  delete outputFile;

  delete inputFile_unweighted;
  delete inputFile_weighted;
}


