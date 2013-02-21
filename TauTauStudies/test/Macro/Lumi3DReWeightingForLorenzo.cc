#include "Lumi3DReWeightingForLorenzo.h"


#include "TRandom1.h"
#include "TRandom2.h"
#include "TRandom3.h"
#include "TStopwatch.h"
#include "TH1.h"
#include "TH3.h"
#include "TFile.h"
#include <string>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <assert.h>

using namespace edm;


Lumi3DReWeightingForLorenzo::Lumi3DReWeightingForLorenzo( std::string generatedFile,
		   std::string dataFile,
							  std::string GenHistName,
		   std::string DataHistName) :
  generatedFileName_( generatedFile), 
  dataFileName_( dataFile ), 
  GenHistName_( GenHistName ), 
  DataHistName_( DataHistName )
{
  generatedFile_ = new TFile(generatedFileName_.c_str()); //MC distribution
  //std::cout << "generatedFile = " << generatedFile << std::endl;
  dataFile_      = new TFile(dataFileName_.c_str());      //Data distribution
	//std::cout << "dataFile = " << dataFile << std::endl;
  
  MC_distr_ = static_cast<TH1*>(generatedFile_->Get( GenHistName_.c_str() )->Clone() );
  //std::cout << "MC_distr = " << MC_distr_ << std::endl;
  Data_distr_ = static_cast<TH1*>(dataFile_->Get( DataHistName_.c_str() )->Clone() );
  //std::cout << "Data_distr = " << Data_distr_ << std::endl;
  
	// MC * data/MC = data, so the weights are data/MC:
  
	// normalize both histograms first

  Data_distr_->Scale( 1.0/ Data_distr_->Integral() );
  MC_distr_->Scale( 1.0/ MC_distr_->Integral() );
  
  //delete generatedFile_;
  //delete dataFile_ ;
}

Lumi3DReWeightingForLorenzo::~Lumi3DReWeightingForLorenzo(){
  delete generatedFile_;
  delete dataFile_ ;
}

double Lumi3DReWeightingForLorenzo::weight3D( int pv1, int pv2, int pv3 ) {
  
  using std::min;

  int npm1 = min(pv1,49);
  int np0 = min(pv2,49);
  int npp1 = min(pv3,49);

  return Weight3D_[npm1][np0][npp1];

}

void Lumi3DReWeightingForLorenzo::weight3D_init( float ScaleFactor ) { 

  // Scale factor is used to shift target distribution (i.e. luminosity scale)  1. = no shift

  //create histogram to write output weights, save pain of generating them again...

  TH3D* WHist = new TH3D("WHist","3D weights",50,-.5,49.5,50,-.5,49.5,50,-.5,49.5 );
  TH3D* DHist = new TH3D("DHist","3D weights",50,-.5,49.5,50,-.5,49.5,50,-.5,49.5 );
  TH3D* MHist = new TH3D("MHist","3D weights",50,-.5,49.5,50,-.5,49.5,50,-.5,49.5 );


  using std::min;

  if( MC_distr_->GetEntries() == 0 ) {
    std::cout << " MC and Data distributions are not initialized! You must call the Lumi3DReWeighting constructor. " << std::endl;
  }


  // arrays for storing number of interactions

  double MC_ints[50][50][50];
  double Data_ints[50][50][50];

  for (int i=0; i<50; i++) {
    for(int j=0; j<50; j++) {
      for(int k=0; k<50; k++) {
	MC_ints[i][j][k] = 0.;
	Data_ints[i][j][k] = 0.;
      }
    }
  }

  double factorial[50];
  double PowerSer[50];
  double base = 1.;

  factorial[0] = 1.;
  PowerSer[0]=1.;

  for (int i = 1; i<51; ++i) {
    base = base*float(i);
    factorial[i] = base;
  }


  double x;
  double xweight;
  double probi, probj, probk;
  double Expval, mean;
  int xi;

  // Get entries for Data, MC, fill arrays:

  int NMCbin = MC_distr_->GetNbinsX();

  for (int jbin=1;jbin<NMCbin+1;jbin++) {       
    x =  MC_distr_->GetBinCenter(jbin);
    xweight = MC_distr_->GetBinContent(jbin); //use as weight for matrix

    //for Summer 11, we have this int feature:
    xi = int(x);

    // Generate Poisson distribution for each value of the mean

    mean = double(xi);

    if(mean<0.) {
      std::cerr << " Your histogram generates MC luminosity values less than zero!"
		<< " Please Check.  Terminating." << std::endl;
      assert(0);
    }


    if(mean==0.){
      Expval = 1.;
    }
    else {
      Expval = exp(-1.*mean);
    }

    base = 1.;

    for (int i = 1; i<50; ++i) {
      base = base*mean;
      PowerSer[i] = base; // PowerSer is mean^i
    }

    // compute poisson probability for each Nvtx in weight matrix

    for (int i=0; i<50; i++) {
      probi = PowerSer[i]/factorial[i]*Expval;
      for(int j=0; j<50; j++) {
	probj = PowerSer[j]/factorial[j]*Expval;
	for(int k=0; k<50; k++) {
	  probk = PowerSer[k]/factorial[k]*Expval;
	  // joint probability is product of event weights multiplied by weight of input distribution bin
	  MC_ints[i][j][k] = MC_ints[i][j][k]+probi*probj*probk*xweight;
	}
      }
    }

  }
  
  int NDatabin = Data_distr_->GetNbinsX();

  for (int jbin=1;jbin<NDatabin+1;jbin++) {       
    mean =  (Data_distr_->GetBinCenter(jbin))*ScaleFactor;
    xweight = Data_distr_->GetBinContent(jbin);

    // Generate poisson distribution for each value of the mean

    if(mean<0.) {
      std::cerr << " Your histogram generates Data luminosity values less than zero!"
		<< " Please Check.  Terminating." << std::endl;
      assert(0);
    }

    if(mean==0.){
      Expval = 1.;
    }
    else {
      Expval = exp(-1.*mean);
    }

    base = 1.;

    for (int i = 1; i<50; ++i) {
      base = base*mean;
      PowerSer[i] = base;
    }

    // compute poisson probability for each Nvtx in weight matrix                                                                  

    for (int i=0; i<50; i++) {
      probi = PowerSer[i]/factorial[i]*Expval;
      for(int j=0; j<50; j++) {
	probj = PowerSer[j]/factorial[j]*Expval;
	for(int k=0; k<50; k++) {
	  probk = PowerSer[k]/factorial[k]*Expval;
	  // joint probability is product of event weights multiplied by weight of input distribution bin
	  Data_ints[i][j][k] = Data_ints[i][j][k]+probi*probj*probk*xweight;
	}
      }
    }

  }
 

  for (int i=0; i<50; i++) {  
    for(int j=0; j<50; j++) {
      for(int k=0; k<50; k++) {
	if( (MC_ints[i][j][k])>0.) {
	  Weight3D_[i][j][k]  =  Data_ints[i][j][k]/MC_ints[i][j][k];
	}
	else {
	  Weight3D_[i][j][k]  = 0.;
	}
	WHist->SetBinContent( i+1,j+1,k+1,Weight3D_[i][j][k] );
	DHist->SetBinContent( i+1,j+1,k+1,Data_ints[i][j][k] );
	MHist->SetBinContent( i+1,j+1,k+1,MC_ints[i][j][k] );
	//	if(i<10 && j<10) cout << Weights[i][j][k] << " " ;
      }
      //if(i<10 && j<10) cout << endl;
    }
  }
}

