#ifndef Lumi3DReWeightingForLorenzo_h
#define Lumi3DReWeightingForLorenzo_h


/**
  \class    Lumi3DReWeighting Lumi3DReWeighting.h "PhysicsTools/Utilities/interface/Lumi3DReWeighting.h"
  \brief    Class to provide lumi weighting for analyzers to weight "flat-to-N" MC samples to data

  This class will trivially take two histograms:
  1. The generated "flat-to-N" distributions from a given processing
  2. A histogram generated from the "estimatePileup" macro here:

  https://twiki.cern.ch/twiki/bin/view/CMS/LumiCalc#How_to_use_script_estimatePileup

  \author Salvatore Rappoccio
*/

#include "TH1.h"
#include "TFile.h"

#include <cmath>
#include <string>
#include <vector>

namespace edm {
class Lumi3DReWeightingForLorenzo {
 public:
    Lumi3DReWeightingForLorenzo( 
				std::string generatedFile,
				std::string dataFile,
				std::string GenHistName,
				std::string DataHistName);

    ~Lumi3DReWeightingForLorenzo ( ) ;

    double weight3D( int, int, int );

    void weight3D_init( float Scale );

  protected:

    std::string generatedFileName_;
    std::string dataFileName_;
    std::string GenHistName_;
    std::string DataHistName_;
    TFile*     generatedFile_;
    TFile*     dataFile_;
    TH1*      weights_;

    //keep copies of normalized distributions:

    TH1*      MC_distr_;
    TH1*      Data_distr_;


    double Weight3D_[50][50][50];

  };
}



#endif
