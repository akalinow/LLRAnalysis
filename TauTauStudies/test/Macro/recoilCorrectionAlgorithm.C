#include <math.h> 
#include "TMath.h" 
#include <utility>
#include "TLorentzVector.h"
#include <iostream>
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"

typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;

#define sigmaU1min 5.0
#define sigmaU2min 5.0

using namespace std;

class recoilCorrectionAlgorithm {
public:
  recoilCorrectionAlgorithm(int verbose=0):
    verbose_(verbose),
    DATAd_(0.0) ,DATAk_(-0.874173)  ,DATAsigma1_(9.1567)  ,DATAb1_(0.00780813)  ,DATAc1_(2.34183E-05)  ,DATAsigma2_(9.18864)  ,DATAb2_(0.00594046)  ,DATAc2_(-3.73481E-06),
    MCd_  (0.0),MCk_   (-0.91035),MCsigma1_     (8.90269),MCb1_   (0.00430534),MCc1_    (2.07653e-05),MCsigma2_    (8.95895),MCb2_    (0.00179307),MCc2_    (-4.80025e-06),

    DATAdUnc_  (0.0)  ,DATAkUnc_  (0.00075441)   ,DATAsigma1Unc_  (0.0186172)   ,DATAb1Unc_  (0.000187149)  ,DATAc1Unc_  (2.21508e-06)  ,DATAsigma2Unc_  (0.01791930)  ,DATAb2Unc_  (0.000165015)  ,DATAc2Unc_  (1.82988e-06),
    MCdUnc_(0.0)  ,MCkUnc_(0.000177905)  ,MCsigma1Unc_(0.00487887)  ,MCb1Unc_(4.66588e-05)  ,MCc1Unc_(5.55936e-07)  ,MCsigma2Unc_(0.00460745)  ,MCb2Unc_(3.87975e-05)  ,MCc2Unc_(4.23705e-07)
  {} ;

  std::pair<double, double> compSigma(double qT, bool isMC);
  std::pair<double, double> compMEtProjU(LV zP4, double metPx, double metPy);
  LV buildZllCorrectedMEt(LV uncorrectedMEt, LV genMEt, LV qGenBoson);
  
private:

  int verbose_;

  double DATAd_;
  double DATAk_;
  double DATAsigma1_;
  double DATAb1_;
  double DATAc1_;
  double DATAsigma2_;
  double DATAb2_;
  double DATAc2_;
  double MCd_;
  double MCk_;
  double MCsigma1_;
  double MCb1_;
  double MCc1_;
  double MCsigma2_;
  double MCb2_;
  double MCc2_;


  double DATAdUnc_;
  double DATAkUnc_;
  double DATAsigma1Unc_;
  double DATAb1Unc_;
  double DATAc1Unc_;
  double DATAsigma2Unc_;
  double DATAb2Unc_;
  double DATAc2Unc_;
  double MCdUnc_;
  double MCkUnc_;
  double MCsigma1Unc_;
  double MCb1Unc_;
  double MCc1Unc_;
  double MCsigma2Unc_;
  double MCb2Unc_;
  double MCc2Unc_;

} ;



std::pair<double, double> recoilCorrectionAlgorithm::compSigma(double qT,  bool isMC){

  double sigmaU1 = isMC ? MCsigma1_*(1. + MCb1_*qT + MCc1_*qT*qT) : DATAsigma1_*(1. + DATAb1_*qT + DATAc1_*qT*qT);
  double sigmaU2 = isMC ? MCsigma2_*(1. + MCb2_*qT + MCc2_*qT*qT) : DATAsigma2_*(1. + DATAb2_*qT + DATAc2_*qT*qT);

  if ( sigmaU1 < sigmaU1min ) sigmaU1 = sigmaU1min;
  if ( sigmaU2 < sigmaU2min ) sigmaU2 = sigmaU2min;

  return std::pair<double, double> (sigmaU1, sigmaU2);

}

std::pair<double, double> recoilCorrectionAlgorithm::compMEtProjU(LV zP4, double metPx, double metPy)
{
 
  if(zP4.Pt()<=0.){
    std::cout << "Error: Z boson with pt = 0 !!!" << std::endl;
    return std::pair<double, double>(0., 0.);
  }
 
  double qX = zP4.Px();
  double qY = zP4.Py();
  double qT = TMath::Sqrt(qX*qX + qY*qY);
  
  double uX = -(qX + metPx);
  double uY = -(qY + metPy);
  
  double u1 = (uX*qX + uY*qY)/qT;
  double u2 = (uX*qY - uY*qX)/qT;
  
  return std::pair<double, double>(u1, u2);
}

LV recoilCorrectionAlgorithm::buildZllCorrectedMEt(LV uncorrectedMEt, LV genMEt, LV qGenBoson){

  LV correctedMEt = uncorrectedMEt;

  double qX = qGenBoson.Px();
  double qY = qGenBoson.Py();
  double qT = qGenBoson.Pt();

  if ( qT > 0. ) {

    std::pair<double, double> uTgen = compMEtProjU(qGenBoson, genMEt.Px(), genMEt.Py());
    double u1gen = uTgen.first;
    double u2gen = uTgen.second;

    std::pair<double, double> uTrec = compMEtProjU(qGenBoson, uncorrectedMEt.Px(), uncorrectedMEt.Py());
    double u1rec = uTrec.first;
    double u2rec = uTrec.second;

    std::pair<double, double> uTsigmaData = compSigma( qT , false);
    double u1sigmaData = uTsigmaData.first;
    double u2sigmaData = uTsigmaData.second;

    std::pair<double, double> uTsigmaMC = compSigma( qT, true);
    double u1sigmaMC = uTsigmaMC.first;
    double u2sigmaMC = uTsigmaMC.second; 

    double u1sigmaCorr = ( u1sigmaMC != 0. ) ? (u1sigmaData/u1sigmaMC) : 1.;
    double u2sigmaCorr = ( u2sigmaMC != 0. ) ? (u2sigmaData/u2sigmaMC) : 1.;

    double u1rec_corrected = (u1rec - u1gen)*u1sigmaCorr + u1gen;         
    double u2rec_corrected = (u2rec - u2gen)*u2sigmaCorr + u2gen;

    double dDiff = DATAd_ - MCd_;
    double kDiff = DATAk_ - MCk_;

    u1rec_corrected += dDiff + kDiff*qT;

    double uXrec_corrected = (u1rec_corrected*qX + u2rec_corrected*qY)/qT;
    double uYrec_corrected = (u1rec_corrected*qY - u2rec_corrected*qX)/qT;

    double uXrec = (u1rec*qX + u2rec*qY)/qT;
    double uYrec = (u1rec*qY - u2rec*qX)/qT; 

    double correctedMETpx = correctedMEt.Px() - (uXrec_corrected - uXrec);
    double correctedMETpy = correctedMEt.Py() - (uYrec_corrected - uYrec);
    double correctedMETpt = TMath::Sqrt(correctedMETpx*correctedMETpx + correctedMETpy*correctedMETpy);

    correctedMEt.SetPx( correctedMETpx );
    correctedMEt.SetPy( correctedMETpy );
    correctedMEt.SetPz( 0.0 );
    correctedMEt.SetE( correctedMETpt );

    if( verbose_ && (correctedMEt.Pt()-uncorrectedMEt.Pt())/uncorrectedMEt.Pt() > 3.0 ){
      cout << "Corr met is " << correctedMEt.Pt() << "/" << uncorrectedMEt.Pt()  << " >+300% larger than raw met ==>" << endl;
      cout << " Boson pT " << qT 
	   << " -- Gen MET " << genMEt.Pt() << endl;
      cout << "u1gen=" << u1gen << ", u2gen=" << u2gen << endl;
      cout << "u1rec=" << u1rec << ", u2rec=" << u2rec << endl;
      cout << "u1sigmaCorr=" << u1sigmaCorr << ", u2sigmaCorr=" << u2sigmaCorr << endl;
      cout << endl;
    }
    
 }

  return correctedMEt;
 
}



