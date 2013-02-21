#include "TFile.h"
#include "TF1.h"
#include "TSystem.h"

#include "ratioEfficiencyElec.C"
#include "ratioEfficiencyTest.C"


Double_t myFuncRatioElec15BL(Double_t* x, Double_t *par) {

  //gSystem->Load("ratioEfficiencyElec_C.so");
  ratioEfficiencyElec* ratioEffElec15 = new ratioEfficiencyElec();

  Float_t xx = x[0];

  return ratioEffElec15->ratio(xx, true);

}

Double_t myFuncTurnOnElec15BL(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyElec_C.so");
  ratioEfficiencyElec* ratioEffElec15 = new ratioEfficiencyElec();
  //ratioEfficiencyTest* ratioEffElec15BL = new ratioEfficiencyTest(14.87,0.3112 , 0.2210, 1.877, 0.987 );

  Float_t xx = x[0];

  return ratioEffElec15->dataEfficiency(xx, true);

}

Double_t myFuncRatioElec15EC(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyElec_C.so");
  ratioEfficiencyElec* ratioEffElec15 = new ratioEfficiencyElec();

  Float_t xx = x[0];

  return ratioEffElec15->ratio(xx, false);

}

Double_t myFuncTurnOnElec15EC(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyElec_C.so");
  ratioEfficiencyElec* ratioEffElec15 = new ratioEfficiencyElec();
  //ratioEfficiencyTest* ratioEffElec15EC = new ratioEfficiencyTest(15.66, 0.7591 , 0.4775, 2.0215, 0.99881 );
 
  Float_t xx = x[0];

  return ratioEffElec15->dataEfficiency(xx, false);

}

Double_t myFuncRatioEle18BL(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffEle18EB = new ratioEfficiencyTest(18.99, 0.0010589 , 1.2828e-05, 1.6480, 0.99604 );

  gSystem->Load("ratioEfficiencyElec_C.so");
  ratioEfficiencyElec* ratioEffElec15MC = new ratioEfficiencyElec();

  Float_t xx = x[0];
  return fitEffEle18EB->turnOn(xx)/ratioEffElec15MC->mcEfficiency(xx, true);
}

Double_t myFuncTurnOnEle18BL(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffEle18EB = new ratioEfficiencyTest(18.99,0.001058, 1.282e-05,1.648, 0.9960);

  Float_t xx = x[0];
  return fitEffEle18EB->turnOn(xx);
}


Double_t myFuncRatioEle18EC(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffEle18EC = new ratioEfficiencyTest(18.84, 1.5717e-03 , 1.6403e-06, 2.866, 0.96996 );

  gSystem->Load("ratioEfficiencyElec_C.so");
  ratioEfficiencyElec* ratioEffElec15MC = new ratioEfficiencyElec();

  Float_t xx = x[0];
  return fitEffEle18EC->turnOn(xx)/ratioEffElec15MC->mcEfficiency(xx, false);;
}


Double_t myFuncTurnOnEle18EC(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffEle18EC = new ratioEfficiencyTest(18.84, 1.5717e-03 , 1.6403e-06, 2.866, 0.96996 );

  Float_t xx = x[0];
  return fitEffEle18EC->turnOn(xx);
}


Double_t myFuncRatioEle20BL(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffEle20EB = new ratioEfficiencyTest(20.55,0.6837,0.85557,1.459166, 1.0395732 );

  gSystem->Load("ratioEfficiencyElec_C.so");
  ratioEfficiencyElec* ratioEffElec15MC = new ratioEfficiencyElec();

  Float_t xx = x[0];
  return fitEffEle20EB->turnOn(xx)/ratioEffElec15MC->mcEfficiency(xx, true);
}

Double_t myFuncTurnOnEle20BL(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffEle20EB = new ratioEfficiencyTest(20.55,0.6837,0.85557,1.4591,1.0395 );

  Float_t xx = x[0];
  return fitEffEle20EB->turnOn(xx);
}

Double_t myFuncRatioEle20EC(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffEle20EC = new ratioEfficiencyTest( 23.63, -1.6077, 1.7209, 1.413, 1.1396);

  gSystem->Load("ratioEfficiencyElec_C.so");
  ratioEfficiencyElec* ratioEffElec15MC = new ratioEfficiencyElec();

  Float_t xx = x[0];
  return fitEffEle20EC->turnOn(xx)/ratioEffElec15MC->mcEfficiency(xx, false);
}


Double_t myFuncTurnOnEle20EC(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffEle20EC = new ratioEfficiencyTest( 23.63, -1.6077, 1.7209, 1.413, 1.1396);

  Float_t xx = x[0];
  return fitEffEle20EC->turnOn(xx);
}

Double_t myFuncRatioEleAllBL(Double_t* x, Double_t *par) {

  ratioEfficiencyTest* fitEffEle15BL = new ratioEfficiencyTest(14.87,0.3112 , 0.2210, 1.877, 0.987 );
  ratioEfficiencyTest* fitEffEle18BL = new ratioEfficiencyTest(18.99,0.0010589 ,1.2828e-05, 1.6480, 0.99604 );
  ratioEfficiencyTest* fitEffEle20BL = new ratioEfficiencyTest(20.55,0.6837,0.85557,1.4591,1.0395 );

  // Summer11:
  //gSystem->Load("ratioEfficiencyElec_C.so");
  //ratioEfficiencyElec* ratioEffElec15MC = new ratioEfficiencyElec();
  // Fall11:
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffElec18MCBL = new ratioEfficiencyTest(15.514, 1.9595, 2.6040, 1.8351, 1.00116);

  float weightElec15 = 1871.;
  float weightElec18 = 1713.;
  float weightElec20 =  895.;

  float total = weightElec15+weightElec18+weightElec20;
  weightElec15/=total;
  weightElec18/=total;
  weightElec20/=total;

  Float_t xx = x[0];

  return (fitEffEle15BL->turnOn(xx) * weightElec15 +
	  fitEffEle18BL->turnOn(xx) * weightElec18 +
	  fitEffEle20BL->turnOn(xx) * weightElec20
	  )/fitEffElec18MCBL->turnOn(xx);
}

Double_t myFuncTurnOnEleAllBL(Double_t* x, Double_t *par) {

  ratioEfficiencyTest* fitEffEle15BL = new ratioEfficiencyTest(14.87,0.3112 , 0.2210, 1.877, 0.987 );
  ratioEfficiencyTest* fitEffEle18BL = new ratioEfficiencyTest(18.99,0.0010589 , 1.2828e-05, 1.6480, 0.99604 );
  ratioEfficiencyTest* fitEffEle20BL = new ratioEfficiencyTest(20.55,0.6837,0.85557,1.4591,1.0395 );

  float weightElec15 = 1871.;
  float weightElec18 = 1713.;
  float weightElec20 =  895.;

  float total = weightElec15+weightElec18+weightElec20;
  weightElec15/=total;
  weightElec18/=total;
  weightElec20/=total;

  Float_t xx = x[0];

  return (fitEffEle15BL->turnOn(xx) * weightElec15 +
	  fitEffEle18BL->turnOn(xx) * weightElec18 +
	  fitEffEle20BL->turnOn(xx) * weightElec20
	  );
 
}



Double_t myFuncRatioEleAllEC(Double_t* x, Double_t *par) {

  ratioEfficiencyTest* fitEffEle15EC = new ratioEfficiencyTest(15.66, 0.7591 , 0.4775, 2.0215, 0.99881 );
  ratioEfficiencyTest* fitEffEle18EC = new ratioEfficiencyTest(18.84, 1.5717e-03 , 1.6403e-06, 2.866, 0.96996 );
  ratioEfficiencyTest* fitEffEle20EC = new ratioEfficiencyTest( 23.63, -1.6077, 1.7209, 1.413, 1.1396);

  // Summer11:
  //gSystem->Load("ratioEfficiencyElec_C.so");
  //ratioEfficiencyElec* ratioEffElec15MC = new ratioEfficiencyElec();
  // Fall11:
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffElec18MCEC = new ratioEfficiencyTest(16.99, 0.08302, 0.008002, 2.0694, 0.9981);
  //ratioEfficiencyTest* fitEffElec18MCEC = new ratioEfficiencyTest(17.00, 0.07, 0.01, 1.96, 1.01);

  float weightElec15 = 1871.;
  float weightElec18 = 1713.;
  float weightElec20 =  895.;

  float total = weightElec15+weightElec18+weightElec20;
  weightElec15/=total;
  weightElec18/=total;
  weightElec20/=total;

  Float_t xx = x[0];

  return (fitEffEle15EC->turnOn(xx) * weightElec15 +
	  fitEffEle18EC->turnOn(xx) * weightElec18 +
	  fitEffEle20EC->turnOn(xx) * weightElec20
	  )/fitEffElec18MCEC->turnOn(xx);
}

Double_t myFuncTurnOnEleAllEC(Double_t* x, Double_t *par) {

  ratioEfficiencyTest* fitEffEle15EC = new ratioEfficiencyTest( 15.66, 0.7591 , 0.4775, 2.0215, 0.99881 );
  ratioEfficiencyTest* fitEffEle18EC = new ratioEfficiencyTest( 18.84, 1.5717e-03 , 1.6403e-06, 2.866, 0.96996 );
  ratioEfficiencyTest* fitEffEle20EC = new ratioEfficiencyTest( 23.63,-1.6077, 1.7209, 1.413, 1.1396);

  float weightElec15 = 1871.;
  float weightElec18 = 1713.;
  float weightElec20 =  895.;

  float total = weightElec15+weightElec18+weightElec20;
  weightElec15/=total;
  weightElec18/=total;
  weightElec20/=total;

  Float_t xx = x[0];

  return (fitEffEle15EC->turnOn(xx) * weightElec15 +
	  fitEffEle18EC->turnOn(xx) * weightElec18 +
	  fitEffEle20EC->turnOn(xx) * weightElec20
	  );
 
}


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
Double_t myFuncRatioElecIDBL(Double_t* x, Double_t *par) {
  return 1.0;
}
Double_t myFuncTurnOnElecIDBL(Double_t* x, Double_t *par) {
  return 1.0;
}
Double_t myFuncRatioElecIDEC(Double_t* x, Double_t *par) {
  return 1.0;
}
Double_t myFuncTurnOnElecIDEC(Double_t* x, Double_t *par) {
  return 1.0;
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

Double_t myFuncRatioElecIDIsoBL(Double_t* x, Double_t *par) {
  
  double ratio = 1.;

  Float_t xx = x[0];
  
//   if( xx < 20 ) 
//     ratio = 0.9857;
//   else if( xx >= 20 && xx< 25) 
//     ratio = 0.9857 ;
//   else if( xx >= 25 && xx< 30)
//     ratio = 0.9998;
//   else if( xx >= 30 && xx< 35)
//     ratio = 0.9908;
//   else if( xx >= 35 && xx< 50)
//     ratio = 0.9880;
//   else if( xx >= 50 && xx< 80)
//     ratio = 0.9829;
//   else if( xx >= 80)
//     ratio = 0.9794;

  if( xx < 20 ) 
    ratio = 1.00;
  else if( xx >= 20 && xx< 30) 
    ratio = 0.955*0.980 ;
  else if( xx >= 30)
    ratio = 1.044*0.984;

  return ratio;
}

Double_t myFuncTurnOnElecIDIsoBL(Double_t* x, Double_t *par) {

  double ratio = 1.;

  Float_t xx = x[0];
  
 if( xx < 20 ) 
    ratio = 0.6006;
  else if( xx >= 20 && xx< 25) 
    ratio = 0.6006 ;
  else if( xx >= 25 && xx< 30)
    ratio = 0.6820;
  else if( xx >= 30 && xx< 35)
    ratio = 0.7322;
  else if( xx >= 35 && xx< 50)
    ratio = 0.8139;
  else if( xx >= 50 && xx< 80)
    ratio = 0.8517;
  else if( xx >= 80)
    ratio = 0.8807;

  return ratio;

}

Double_t myFuncRatioElecIDIsoEC(Double_t* x, Double_t *par) {

  double ratio = 1.;

  Float_t xx = x[0];

//   if( xx < 20 ) 
//     ratio = 1.1023;
//   else if( xx >= 20 && xx< 25) 
//     ratio = 1.1023 ;
//   else if( xx >= 25 && xx< 30)
//     ratio = 1.0783;
//   else if( xx >= 30 && xx< 35)
//     ratio = 1.0660;
//   else if( xx >= 35 && xx< 50)
//     ratio = 1.0377;
//   else if( xx >= 50 && xx< 80)
//     ratio = 1.0111;
//   else if( xx >= 80)
//     ratio = 0.9989;

  if( xx < 20 ) 
    ratio = 1.00;
  else if( xx >= 20 && xx< 30) 
    ratio = 0.938*0.967 ;
  else if( xx >= 30)
    ratio = 0.977*0.989;

  return ratio;
}

Double_t myFuncTurnOnElecIDIsoEC(Double_t* x, Double_t *par) {
 
  double ratio = 1.;

  Float_t xx = x[0];
 
  if( xx < 20 ) 
    ratio = 0.;
  else if( xx >= 20 && xx< 25) 
    ratio = 0.4780 ;
  else if( xx >= 25 && xx< 30)
    ratio = 0.5382;
  else if( xx >= 30 && xx< 35)
    ratio = 0.5896;
  else if( xx >= 35 && xx< 50)
    ratio = 0.6792;
  else if( xx >= 50 && xx< 80)
    ratio = 0.7316;
  else if( xx >= 80)
    ratio = 0.7702;

  return ratio;

}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

Double_t myFuncRatioMuIDBL(Double_t* x, Double_t *par) {
  return 1.0;
}
Double_t myFuncTurnOnMuIDBL(Double_t* x, Double_t *par) {
  return 1.0;
}
Double_t myFuncRatioMuIDEC(Double_t* x, Double_t *par) {
  return 1.0;
}
Double_t myFuncTurnOnMuIDEC(Double_t* x, Double_t *par) {
  return 1.0;
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////


Double_t myFuncRatioMuIDIsoBL(Double_t* x, Double_t *par) {
  
  double ratio = 1.;

  Float_t xx = x[0];
  
  if( xx < 15 ) 
    ratio = 0.9467;
  else if( xx >= 15 && xx< 20) 
    ratio = 0.9467 ;
  else if( xx >= 20 && xx< 25) 
    ratio = 0.9557 ;
  else if( xx >= 25 && xx< 30)
    ratio = 0.9615;
  else if( xx >= 30 && xx< 35)
    ratio = 0.9778;
  else if( xx >= 35 && xx< 50)
    ratio = 0.9891;
  else if( xx >= 50)
    ratio = 0.9909;

  return ratio;
}

Double_t myFuncTurnOnMuIDIsoBL(Double_t* x, Double_t *par) {

  double ratio = 1.;

  Float_t xx = x[0];
  
  if( xx < 15 ) 
    ratio = 0.6277;
  else if( xx >= 15 && xx< 20) 
    ratio = 0.6277 ;
  else if( xx >= 20 && xx< 25) 
    ratio = 0.7073 ;
  else if( xx >= 25 && xx< 30)
    ratio = 0.7628;
  else if( xx >= 30 && xx< 35)
    ratio = 0.8150;
  else if( xx >= 35 && xx< 50)
    ratio = 0.8963;
  else if( xx >= 50)
    ratio = 0.9307;

  return ratio;

}

Double_t myFuncRatioMuIDIsoEC(Double_t* x, Double_t *par) {

  double ratio = 1.;

  Float_t xx = x[0];
  
  if( xx < 15 ) 
    ratio = 0.9611;
  else if( xx >= 15 && xx< 20) 
    ratio = 0.9611 ;
  else if( xx >= 20 && xx< 25) 
    ratio = 0.9737 ;
  else if( xx >= 25 && xx< 30)
    ratio = 0.9818;
  else if( xx >= 30 && xx< 35)
    ratio = 0.9919;
  else if( xx >= 35 && xx< 50)
    ratio = 0.9968;
  else if( xx >= 50)
    ratio = 0.9954;

  return ratio;
}

Double_t myFuncTurnOnMuIDIsoEC(Double_t* x, Double_t *par) {
 
  double ratio = 1.;

  Float_t xx = x[0];
  
  if( xx < 15 ) 
    ratio = 0.6534;
  else if( xx >= 15 && xx< 20) 
    ratio = 0.6534 ;
  else if( xx >= 20 && xx< 25) 
    ratio = 0.7275 ;
  else if( xx >= 25 && xx< 30)
    ratio = 0.7813;
  else if( xx >= 30 && xx< 35)
    ratio = 0.8219;
  else if( xx >= 35 && xx< 50)
    ratio = 0.8893;
  else if( xx >= 50)
    ratio = 0.9201;

  return ratio;

}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////


Double_t myFuncRatioElecIsoBL(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffElecIsoRD = new ratioEfficiencyTest(0.1319,3.449,0.568 , 109.24,1.056);
  ratioEfficiencyTest* ratioEffElecIsoMC = new ratioEfficiencyTest(1.699,7.7149,2.738 , 121.6, 1.0646);
  
  Float_t xx = x[0];
  return ratioEffElecIsoRD->turnOn(xx)/ratioEffElecIsoMC->turnOn(xx);
}

Double_t myFuncTurnOnElecIsoBL(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffElecIso = new ratioEfficiencyTest(0.1319,3.449,0.568 , 109.24,1.056);

  Float_t xx = x[0];
  return ratioEffElecIso->turnOn(xx);
}

Double_t myFuncRatioElecIsoEC(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffElecIsoRD = new ratioEfficiencyTest(-0.3521,4.477,  1.0056, 113.9,     1.04108);
  ratioEfficiencyTest* ratioEffElecIsoMC = new ratioEfficiencyTest( 7.233, 1.7088, 1.3650,    1.0521, 4.33366);
  
  Float_t xx = x[0];
  return ratioEffElecIsoRD->turnOn(xx)/ratioEffElecIsoMC->turnOn(xx);
}

Double_t myFuncTurnOnElecIsoEC(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffElecIso = new ratioEfficiencyTest(-0.3521,4.477,1.005609, 113.9, 1.04108);

  Float_t xx = x[0];
  return ratioEffElecIso->turnOn(xx);
}


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////



Double_t myFuncRatioMuIdBL(Double_t* x, Double_t *par) {
  return 0.9897;
}

Double_t myFuncTurnOnMuIdBL(Double_t* x, Double_t *par) {
  return 0.9580;
}

Double_t myFuncRatioMuIdEC(Double_t* x, Double_t *par) {
  return 0.9758;
}

Double_t myFuncTurnOnMuIdEC(Double_t* x, Double_t *par) {
  return 0.9373;
}


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////


Double_t myFuncRatioMuIsoBL(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffMuIsoRD = new ratioEfficiencyTest(0.1319,3.449,0.568 , 109.24,1.056);
  ratioEfficiencyTest* ratioEffMuIsoMC = new ratioEfficiencyTest(1.699,7.7149,2.738 , 121.6, 1.0646);

  Float_t xx = x[0];
  return ratioEffMuIsoRD->turnOn(xx)/ratioEffMuIsoMC->turnOn(xx);
}

Double_t myFuncTurnOnMuIsoBL(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffMuIso = new ratioEfficiencyTest(0.1319,3.449,0.568 , 109.24,1.056);

  Float_t xx = x[0];
  return ratioEffMuIso->turnOn(xx);
}

Double_t myFuncRatioMuIsoEC(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffMuIsoRD = new ratioEfficiencyTest(-0.3521,4.477,1.005609, 113.9, 1.04108);
  ratioEfficiencyTest* ratioEffMuIsoMC = new ratioEfficiencyTest( 7.233, 1.7088, 1.3650,    1.0521, 4.33366);

  Float_t xx = x[0];
  return ratioEffMuIsoRD->turnOn(xx)/ratioEffMuIsoMC->turnOn(xx);
}

Double_t myFuncTurnOnMuIsoEC(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffMuIso = new ratioEfficiencyTest(-0.3521,4.477,1.005609, 113.9, 1.04108);

  Float_t xx = x[0];
  return ratioEffMuIso->turnOn(xx);
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

Double_t myFuncTurnOnMu12MCBL(Double_t* x, Double_t *par) {

  Float_t xx = x[0];
  if(xx<12) 
    return 0;
  else 
    return 0.93;
}

Double_t myFuncTurnOnMu12MCEC(Double_t* x, Double_t *par) {

  Float_t xx = x[0];
  if(xx<12) 
    return 0;
  else 
    return 0.84;
}

Double_t myFuncTurnOnMu15MCBL(Double_t* x, Double_t *par) {

  Float_t xx = x[0];
  if(xx<12) 
    return 0;
  else 
    return 0.917;
}

Double_t myFuncTurnOnMu15MCEC(Double_t* x, Double_t *par) {

  Float_t xx = x[0];
  if(xx<12) 
    return 0;
  else 
    return 0.836;
}


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

Double_t myFuncRatioMu15L110BL(Double_t* x, Double_t *par) {

  TF1* turnOnMu15MCBL = new TF1("turnOnMu15MCBL",myFuncTurnOnMu15MCBL,0,400,0);

  Float_t xx = x[0];
  if(xx<12) 
    return 0;
  else 
    //return 0.901/turnOnMu12MCBL->Eval(xx);
    return 0.901/turnOnMu15MCBL->Eval(xx);

}

Double_t myFuncTurnOnMu15L110BL(Double_t* x, Double_t *par) {

  Float_t xx = x[0];
  if(xx<12) 
    return 0;
  else 
    return 0.901;
}

Double_t myFuncRatioMu15L110EC(Double_t* x, Double_t *par) {

  TF1* turnOnMu15MCEC = new TF1("turnOnMu15MCEC",myFuncTurnOnMu15MCEC,0,400,0);

  Float_t xx = x[0];
  if(xx<12) 
    return 0;
  else 
    //return 0.862/turnOnMu12MCEC->Eval(xx);
    return 0.862/turnOnMu15MCEC->Eval(xx);

}

Double_t myFuncTurnOnMu15L110EC(Double_t* x, Double_t *par) {

  Float_t xx = x[0];
  if(xx<12) 
    return 0;
  else 
    return 0.862;
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

Double_t myFuncRatioMu15L114BL(Double_t* x, Double_t *par) {

  TF1* turnOnMu15MCBL = new TF1("turnOnMu15MCBL",myFuncTurnOnMu15MCBL,0,400,0);

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffMuL114BL = new ratioEfficiencyTest(15.06, 0.55278, 1.3423, 1.002975, 3.3676);

  Float_t xx = x[0];
  //return ratioEffMuL114BL->turnOn(xx)/turnOnMu12MCBL->Eval(xx);
  return ratioEffMuL114BL->turnOn(xx)/turnOnMu15MCBL->Eval(xx);
}

Double_t myFuncTurnOnMu15L114BL(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffMuL114BL = new ratioEfficiencyTest(15.06, 0.55278, 1.3423, 1.002975, 3.3676);


  Float_t xx = x[0];
  return ratioEffMuL114BL->turnOn(xx);
}

Double_t myFuncRatioMu15L114EC(Double_t* x, Double_t *par) {

  TF1* turnOnMu15MCEC = new TF1("turnOnMu15MCEC",myFuncTurnOnMu15MCEC,0,400,0);

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffMuL114EC = new ratioEfficiencyTest(15.32, 0.866114, 1.25008, 1.63711, 0.84490);

  Float_t xx = x[0];
  //return ratioEffMuL114EC->turnOn(xx)/turnOnMu12MCEC->Eval(xx);
  return ratioEffMuL114EC->turnOn(xx)/turnOnMu15MCEC->Eval(xx);
}

Double_t myFuncTurnOnMu15L114EC(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffMuL114EC = new ratioEfficiencyTest(15.32, 0.866114, 1.25008, 1.63711, 0.84490);

  Float_t xx = x[0];
  return ratioEffMuL114EC->turnOn(xx);
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////





Double_t myFuncRatioMuAllBL(Double_t* x, Double_t *par) {

  TF1* ratioMu15L110BL = new TF1("ratioMu15L110BL", myFuncRatioMu15L110BL,0,400,0);
  TF1* ratioMu15L114BL = new TF1("ratioMu15L114BL", myFuncRatioMu15L114BL,0,400,0);
  
  float weightIsoMu15L110 = 2200.;
  float weightIsoMu15L114 = 2400.;

  float total = weightIsoMu15L110+weightIsoMu15L114;
  weightIsoMu15L110 /= total;
  weightIsoMu15L114 /= total;

  Float_t xx = x[0];

  return (ratioMu15L110BL->Eval(xx) * weightIsoMu15L110 +
	  ratioMu15L114BL->Eval(xx) * weightIsoMu15L114 );

}

Double_t myFuncTurnOnMuAllBL(Double_t* x, Double_t *par) {

  TF1* turnOnMu15L110BL = new TF1("turnOnMu15L110BL", myFuncTurnOnMu15L110BL,0,400,0);
  TF1* turnOnMu15L114BL = new TF1("turnOnMu15L114BL", myFuncTurnOnMu15L114BL,0,400,0);

  float weightIsoMu15L110 = 2200.;
  float weightIsoMu15L114 = 2400.;

  float total = weightIsoMu15L110+weightIsoMu15L114;
  weightIsoMu15L110 /=total;
  weightIsoMu15L114 /=total;

  Float_t xx = x[0];

  return (turnOnMu15L110BL->Eval(xx) * weightIsoMu15L110 +
	  turnOnMu15L114BL->Eval(xx) * weightIsoMu15L114 );
}


Double_t myFuncRatioMuAllEC(Double_t* x, Double_t *par) {

  TF1* ratioMu15L110EC = new TF1("ratioMu15L110EC", myFuncRatioMu15L110EC,0,400,0);
  TF1* ratioMu15L114EC = new TF1("ratioMu15L114EC", myFuncRatioMu15L114EC,0,400,0);
  
  float weightIsoMu15L110 = 2200.;
  float weightIsoMu15L114 = 2400.;

  float total = weightIsoMu15L110+weightIsoMu15L114;
  weightIsoMu15L110 /= total;
  weightIsoMu15L114 /= total;

  Float_t xx = x[0];

  return (ratioMu15L110EC->Eval(xx) * weightIsoMu15L110 +
	  ratioMu15L114EC->Eval(xx) * weightIsoMu15L114 );

}

Double_t myFuncTurnOnMuAllEC(Double_t* x, Double_t *par) {

  TF1* turnOnMu15L110EC = new TF1("turnOnMu15L110EC", myFuncTurnOnMu15L110EC,0,400,0);
  TF1* turnOnMu15L114EC = new TF1("turnOnMu15L114EC", myFuncTurnOnMu15L114EC,0,400,0);

  float weightIsoMu15L110 = 2200.;
  float weightIsoMu15L114 = 2400.;

  float total = weightIsoMu15L110+weightIsoMu15L114;
  weightIsoMu15L110 /=total;
  weightIsoMu15L114 /=total;

  Float_t xx = x[0];

  return (turnOnMu15L110EC->Eval(xx) * weightIsoMu15L110 +
	  turnOnMu15L114EC->Eval(xx) * weightIsoMu15L114 );
}






/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////



// HLT PFTauLoose10 / MC Summer11 / mu+tau

Double_t myFuncTurnOnTauLoose10MuTauMC(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  //ratioEfficiencyTest* ratioEffTauLoose10MC = new ratioEfficiencyTest(12.96,1.41,3.31,1.10,0.868);
  ratioEfficiencyTest* ratioEffTauLoose10MC = new ratioEfficiencyTest(13.06,2.60,5.03,1.11,1.0);
  
  Float_t xx = x[0];

  return ratioEffTauLoose10MC->turnOn(xx);

}

/////////////////////////////////////////////////

// HLT PFTauLoose15 / MC Summer11 / mu+tau

Double_t myFuncTurnOnTauLoose15MuTauMC(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  //ratioEfficiencyTest* ratioEffTauLoose15MC = new ratioEfficiencyTest(15.5,1.50,6.4,31,0.804);
  ratioEfficiencyTest* ratioEffTauLoose15MC = new ratioEfficiencyTest(15.5,1.76,3.37,1.11,1.00);
  
  Float_t xx = x[0];

  return ratioEffTauLoose15MC->turnOn(xx);

}

/////////////////////////////////////////////////

// HLT PFTauLoose20 / MC Summer11 / mu+tau

Double_t myFuncTurnOnTauLoose20MuTauMC(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  //ratioEfficiencyTest* ratioEffTauLoose20MC = new ratioEfficiencyTest(20.91,3.43,7.94,29.8,0.810);
  ratioEfficiencyTest* ratioEffTauLoose20MC = new ratioEfficiencyTest(20.89,3.25,7.99,34.5,0.811);
  
  Float_t xx = x[0];

  return ratioEffTauLoose20MC->turnOn(xx);

}

/////////////////////////////////////////////////

// HLT PFTauLoose15 / MC Fall11 / mu+tau

Double_t myFuncTurnOnTauLoose15MuTauMC_v2(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose15MC = new ratioEfficiencyTest(14.53,-0.01519,0.0022261,1.6761,0.8912);
  
  Float_t xx = x[0];

  return ratioEffTauLoose15MC->turnOn(xx);

}


/////////////////////////////////////////////////
/////////////////////////////////////////////////

// HLT PFTauLoose15 / Run2011A    / mu+tau

Double_t myFuncRatioTauLoose15MuTauRunA(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose15MuTauRunA = new ratioEfficiencyTest(14.274,1.048,1.706,1.336,1.0);  
  TF1* turnOnTauLoose10MuTauMC = new TF1("turnOnTauLoose10MuTauMC", myFuncTurnOnTauLoose10MuTauMC,0,400,0);

  Float_t xx = x[0];

  return ratioEffTauLoose15MuTauRunA->turnOn(xx)/turnOnTauLoose10MuTauMC->Eval(xx);

}

Double_t myFuncTurnOnTauLoose15MuTauRunA(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose15MuTauRunA = new ratioEfficiencyTest(14.274,1.048,1.706,1.336,1.0);
 
  Float_t xx = x[0];

  return ratioEffTauLoose15MuTauRunA->turnOn(xx);

}

/////////////////////////////////////////////////

// HLT PFTauLoose20 / Run2011A    / mu+tau

Double_t myFuncRatioTauLoose20MuTauRunA(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose20MuTauRunA = new ratioEfficiencyTest(19.32,1.14,2.3,1.19,1.0);

  TF1* turnOnTauLoose10MuTauMC = new TF1("turnOnTauLoose10MuTauMC", myFuncTurnOnTauLoose10MuTauMC,0,400,0);

  Float_t xx = x[0];

  return ratioEffTauLoose20MuTauRunA->turnOn(xx)/ turnOnTauLoose10MuTauMC->Eval(xx);

}

Double_t myFuncTurnOnTauLoose20MuTauRunA(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  //ratioEfficiencyTest* ratioEffTauLoose20MuTauRunA = new ratioEfficiencyTest(19.32,1.14,2.3,1.19,1.0);
  ratioEfficiencyTest* ratioEffTauLoose20MuTauRunA =  new ratioEfficiencyTest(19.72,0.84,1.17,1.007,9.35 );
    //new ratioEfficiencyTest(19.72,0.84,1.17,1.01,9.35);
  //

  Float_t xx = x[0];

  return ratioEffTauLoose20MuTauRunA->turnOn(xx);

}

/////////////////////////////////////////////////

// HLT PFTauLoose20 / Run2011B    / mu+tau

Double_t myFuncRatioTauLoose20MuTauRunB(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose20MuTauRunB = new ratioEfficiencyTest(18.6748,2.4703,6.59219,1.10415,0.908873);
  
  TF1* turnOnTauLoose10MuTauMC = new TF1("turnOnTauLoose10MuTauMC", myFuncTurnOnTauLoose10MuTauMC,0,400,0);

  Float_t xx = x[0];

  return ratioEffTauLoose20MuTauRunB->turnOn(xx)/ turnOnTauLoose10MuTauMC->Eval(xx);

}

Double_t myFuncTurnOnTauLoose20MuTauRunB(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose20MuTauRunB = new ratioEfficiencyTest(18.6748,2.4703,6.59219,1.10415,0.908873);
  
  Float_t xx = x[0];

  return ratioEffTauLoose20MuTauRunB->turnOn(xx);

}

/////////////////////////////////////////////////


Double_t myFuncRatioTauMuTauAll(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyTest_C.so");
 
  //IVO:
  //ratioEfficiencyTest* ratioEffTauLoose15MuTauRunA = new ratioEfficiencyTest(14.274,1.048,1.706,1.336,1.0);  
  //ratioEfficiencyTest* ratioEffTauLoose20MuTauRunA = new ratioEfficiencyTest(19.32,1.14,2.3,1.19,1.0);
  //ratioEfficiencyTest* ratioEffTauLoose20MuTauRunB = new ratioEfficiencyTest(18.6748,2.4703,6.59219,1.10415,0.908873);

  //NOTE:
  ratioEfficiencyTest* ratioEffTauLoose10MuTau = new ratioEfficiencyTest(16.785,-0.6938,4.5728, 94127.12,0.8831);
  ratioEfficiencyTest* ratioEffTauLoose15MuTau = new ratioEfficiencyTest(14.67 , 0.4082,0.5519, 1.4477, 0.9613);
  ratioEfficiencyTest* ratioEffTauLoose20MuTau = new ratioEfficiencyTest(19.19 ,-1.36,  2.827,  1.02721,1.5086);
 

  //TF1* turnOnTauLoose10MuTauMC = new TF1("turnOnTauLoose10MuTauMC", myFuncTurnOnTauLoose10MuTauMC,0,800,0);
  //turnOnTauLoose10MuTauMC->SetNpx(3200);
  TF1* turnOnTauLoose15MuTauMC_v2 = new TF1("turnOnTauLoose15MuTauMC_v2", myFuncTurnOnTauLoose15MuTauMC_v2,0,800,0);
  turnOnTauLoose15MuTauMC_v2->SetNpx(3200);

  float weightLoose10 =    0.;
  float weightLoose15 = 1968.+169;
  float weightLoose20 = 2460.;
  
  float total = weightLoose10+weightLoose15+weightLoose20;
  weightLoose10 /= total;
  weightLoose15 /= total;
  weightLoose20 /= total;
 
  Float_t xx = x[0];

  return (ratioEffTauLoose10MuTau->turnOn(xx) * weightLoose10 + 
	  ratioEffTauLoose15MuTau->turnOn(xx) * weightLoose15 +
	  ratioEffTauLoose20MuTau->turnOn(xx) * weightLoose20
	  )/turnOnTauLoose15MuTauMC_v2->Eval(xx) ;

}

Double_t myFuncTurnOnTauMuTauAll(Double_t* x, Double_t *par) {

  gSystem->Load("ratioEfficiencyTest_C.so");
  //ratioEfficiencyTest* ratioEffTauLoose15MuTauRunA = new ratioEfficiencyTest(14.274,1.048,1.706,1.336,1.0);
  //ratioEfficiencyTest* ratioEffTauLoose20MuTauRunB = new ratioEfficiencyTest(18.6748,2.4703,6.59219,1.10415,0.908873);
  //ratioEfficiencyTest* ratioEffTauLoose10MC = new ratioEfficiencyTest(12.96,1.41,3.31,1.10,0.868);
  
  //NOTE:
  ratioEfficiencyTest* ratioEffTauLoose10MuTau = new ratioEfficiencyTest(16.785,-0.6938,4.5728, 94127.12,   0.8831);
  ratioEfficiencyTest* ratioEffTauLoose15MuTau = new ratioEfficiencyTest(14.67,0.4082,0.5519,       1.4477, 0.9613);
  ratioEfficiencyTest* ratioEffTauLoose20MuTau = new ratioEfficiencyTest(19.19,-1.36,2.827,         1.02721,1.5086);
  
  float weightLoose10 =    0.;
  float weightLoose15 = 1968.+169;
  float weightLoose20 = 2460.;
  
  float total = weightLoose10+weightLoose15+weightLoose20;
  weightLoose10 /= total;
  weightLoose15 /= total;
  weightLoose20 /= total;
  
  Float_t xx = x[0];
  
  return (ratioEffTauLoose10MuTau->turnOn(xx) * weightLoose10 + 
	  ratioEffTauLoose15MuTau->turnOn(xx) * weightLoose15 +
	  ratioEffTauLoose20MuTau->turnOn(xx) * weightLoose20
	  );
  
 

}



////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////




// HLT PFTauLoose10 / MC Summer11 / elec+tau

Double_t myFuncTurnOnTauLoose10ElecTauMC(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose10MC = new ratioEfficiencyTest(12.92, 2.41,4.09,1.33,0.90);
  
  Float_t xx = x[0];

  return ratioEffTauLoose10MC->turnOn(xx);

}

/////////////////////////////////////////////////

// HLT PFTauLoose15 / MC Summer11 / elec+tau

Double_t myFuncTurnOnTauLoose15ElecTauMC(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose15MC = new ratioEfficiencyTest(15.57,1.77,3.34,1.11,1.00);
  
  Float_t xx = x[0];

  return ratioEffTauLoose15MC->turnOn(xx);

}

/////////////////////////////////////////////////

// HLT PFTauLoose20 / MC Summer11 / elec+tau

Double_t myFuncTurnOnTauLoose20ElecTauMC(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose20MC = new ratioEfficiencyTest(20.99,3.13,7.99,34.9,0.813);
  
  Float_t xx = x[0];

  return ratioEffTauLoose20MC->turnOn(xx);

}

/////////////////////////////////////////////////

// HLT PFTauMedium20 / MC Fall11 / elec+tau

Double_t myFuncTurnOnTauMedium20ElecTauMC(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauMedium20MC = new ratioEfficiencyTest(19.451,-0.0554166,0.0518694,1.24892,0.950397);
  
  Float_t xx = x[0];

  return ratioEffTauMedium20MC->turnOn(xx);

}

/////////////////////////////////////////////////
/////////////////////////////////////////////////

// HLT PFTauLoose15 / Run2011A    / e+tau

Double_t myFuncRatioTauLoose15ElecTauRunA(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose15ElecTauRunA = new ratioEfficiencyTest(14.32,1.11,1.84,1.34,1.0);
  
  TF1* turnOnTauLoose15ElecTauMC = new TF1("turnOnTauLoose15ElecTauMC", myFuncTurnOnTauLoose10ElecTauMC,0,400,0);

  Float_t xx = x[0];

  return ratioEffTauLoose15ElecTauRunA->turnOn(xx)/ turnOnTauLoose15ElecTauMC->Eval(xx);

}

Double_t myFuncTurnOnTauLoose15ElecTauRunA(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose15ElecTauRunA = new ratioEfficiencyTest(14.32,1.11,1.84,1.34,1.0);  
 
  Float_t xx = x[0];

  return ratioEffTauLoose15ElecTauRunA->turnOn(xx);

}



/////////////////////////////////////////////////

// HLT PFTauLoose20 / Run2011A    / e+tau

Double_t myFuncRatioTauLoose20ElecTauRunA(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose20ElecTauRunA = new ratioEfficiencyTest(19.38,1.05,1.99,1.25,1.0);
  
  TF1* turnOnTauLoose15ElecTauMC = new TF1("turnOnTauLoose15ElecTauMC", myFuncTurnOnTauLoose10ElecTauMC,0,400,0);

  Float_t xx = x[0];

  return ratioEffTauLoose20ElecTauRunA->turnOn(xx)/ turnOnTauLoose15ElecTauMC->Eval(xx);

}

Double_t myFuncTurnOnTauLoose20ElecTauRunA(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose20ElecTauRunA = new ratioEfficiencyTest(19.38,1.05,1.99,1.25,1.0); 

  Float_t xx = x[0];

  return ratioEffTauLoose20ElecTauRunA->turnOn(xx);

}

/////////////////////////////////////////////////

// HLT PFTauMedium20 / Run2011A / e+tau

Double_t myFuncRatioTauMedium20ElecTauRunA(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauMedium20ElecTauRunA = new ratioEfficiencyTest(19.37 ,2.34, 3.06,1.37,1.0);
  
  TF1* turnOnTauLoose15ElecTauMC = new TF1("turnOnTauLoose15ElecTauMC", myFuncTurnOnTauLoose10ElecTauMC,0,400,0);
  
  Float_t xx = x[0];

  return ratioEffTauMedium20ElecTauRunA->turnOn(xx)/ turnOnTauLoose15ElecTauMC->Eval(xx);

}

Double_t myFuncTurnOnTauMedium20ElecTauRunA(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauMedium20ElecTauRunA = new ratioEfficiencyTest(19.37 ,2.34, 3.06,1.37,1.0);
  
  Float_t xx = x[0];

  return ratioEffTauMedium20ElecTauRunA->turnOn(xx);

}

/////////////////////////////////////////////////

// HLT PFTauTight20 / Run2011B / e+tau

Double_t myFuncRatioTauTight20ElecTauRunA(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauTight20ElecTauRunA = new ratioEfficiencyTest(19.49,1.16,1.82,1.10,1.04);
  
  TF1* turnOnTauLoose15ElecTauMC = new TF1("turnOnTauLoose15ElecTauMC", myFuncTurnOnTauLoose10ElecTauMC,0,400,0);
  
  Float_t xx = x[0];

  return ratioEffTauTight20ElecTauRunA->turnOn(xx)/ turnOnTauLoose15ElecTauMC->Eval(xx);

}

Double_t myFuncTurnOnTauTight20ElecTauRunA(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauTight20ElecTauRunA = new ratioEfficiencyTest(19.49,1.16,1.82,1.10,1.04);
   
  Float_t xx = x[0];

  return ratioEffTauTight20ElecTauRunA->turnOn(xx);

}

/////////////////////////////////////////////////





// HLT PFTauLoose20 / Run2011B / e+tau

Double_t myFuncRatioTauLoose20ElecTauRunB(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose20ElecTauRunB = new ratioEfficiencyTest(18.79,2.498,6.37,1.10,0.921);
  
  TF1* turnOnTauLoose15ElecTauMC = new TF1("turnOnTauLoose15ElecTauMC", myFuncTurnOnTauLoose10ElecTauMC,0,400,0);
  
  Float_t xx = x[0];

  return ratioEffTauLoose20ElecTauRunB->turnOn(xx)/ turnOnTauLoose15ElecTauMC->Eval(xx);

}

Double_t myFuncTurnOnTauLoose20ElecTauRunB(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose20ElecTauRunB = new ratioEfficiencyTest(18.79,2.498,6.37,1.10,0.921);
  
  Float_t xx = x[0];

  return ratioEffTauLoose20ElecTauRunB->turnOn(xx);

}

/////////////////////////////////////////////////

// HLT PFTauMedium20 / Run2011B / e+tau

Double_t myFuncRatioTauMedium20ElecTauRunB(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauMedium20ElecTauRunB = new ratioEfficiencyTest(19.47,2.4184,5.161,1.11,0.875);
  
  TF1* turnOnTauLoose15ElecTauMC = new TF1("turnOnTauLoose15ElecTauMC", myFuncTurnOnTauLoose10ElecTauMC,0,400,0);
 
  Float_t xx = x[0];

  return ratioEffTauMedium20ElecTauRunB->turnOn(xx)/ turnOnTauLoose15ElecTauMC->Eval(xx);

}

Double_t myFuncTurnOnTauMedium20ElecTauRunB(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauMedium20ElecTauRunB = new ratioEfficiencyTest(19.47,2.4184,5.161,1.11,0.875);
  
  Float_t xx = x[0];

  return ratioEffTauMedium20ElecTauRunB->turnOn(xx);

}


/////////////////////////////////////////////////

// HLT PFTauMedium20 / Run2011A+B / e+tau

Double_t myFuncRatioTauMedium20ElecTauRunAB(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  //IVO:
  ratioEfficiencyTest* ratioEffTauMedium20ElecTauRunA = new ratioEfficiencyTest(19.37 ,2.34,  3.06, 1.37,1.0);
  ratioEfficiencyTest* ratioEffTauMedium20ElecTauRunB = new ratioEfficiencyTest(19.47 ,2.4184,5.161,1.11,0.875);

  TF1* turnOnTauLoose15ElecTauMC = new TF1("turnOnTauLoose15ElecTauMC", myFuncTurnOnTauLoose10ElecTauMC,0,400,0);

  Float_t xx = x[0];

  return (0.14*(ratioEffTauMedium20ElecTauRunA->turnOn(xx)) + 0.86*(ratioEffTauMedium20ElecTauRunB->turnOn(xx)) )/turnOnTauLoose15ElecTauMC->Eval(xx);

 

}

Double_t myFuncTurnOnTauMedium20ElecTauRunAB(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauMedium20ElecTauRunA = new ratioEfficiencyTest(19.37 ,2.34,  3.06, 1.37,1.0);
  ratioEfficiencyTest* ratioEffTauMedium20ElecTauRunB = new ratioEfficiencyTest(19.47 ,2.4184,5.161,1.11,0.875);
  
  Float_t xx = x[0];

  return (0.14*(ratioEffTauMedium20ElecTauRunA->turnOn(xx)) + 0.86*(ratioEffTauMedium20ElecTauRunB->turnOn(xx)) );

}


/////////////////////////////////////////////////

// HLT PFTauTight20 / Run2011B / e+tau

Double_t myFuncRatioTauTight20ElecTauRunB(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauTight20ElecTauRunB = new ratioEfficiencyTest(19.49,2.44,4.44,1.12,0.850);
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose15MC = new ratioEfficiencyTest(15.57,1.77,3.34,1.11,1.00);

  
  Float_t xx = x[0];
  float effMC = ratioEffTauLoose15MC->turnOn(xx);

  return ratioEffTauTight20ElecTauRunB->turnOn(xx)/ effMC;

}

Double_t myFuncTurnOnTauTight20ElecTauRunB(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauTight20ElecTauRunB = new ratioEfficiencyTest(19.49,2.44,4.44,1.12,0.850);
  
  Float_t xx = x[0];

  return ratioEffTauTight20ElecTauRunB->turnOn(xx);

}

/////////////////////////////////////////////////

// HLT PFTau All / e+tau

Double_t myFuncRatioTauElecTauAll(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
 
  ratioEfficiencyTest* ratioEffTauLoose20ElecTau  = new ratioEfficiencyTest(19.63,-0.986,1.94,  1.023979,1.911);
  ratioEfficiencyTest* ratioEffTauMedium20ElecTau = new ratioEfficiencyTest(19.35, 0.37, 0.1582,3.311,   0.763);
  ratioEfficiencyTest* ratioEffTauTight20ElecTau  = new ratioEfficiencyTest(19.72, 0.844,1.167, 1.007466,9.351);


  //turnOnTauLoose10MuTauMC->SetNpx(3200);
  //Summer11:
  //TF1* turnOnTauLoose15ElecTauMC = new TF1("turnOnTauLoose15ElecTauMC", myFuncTurnOnTauLoose10ElecTauMC,0,400,0);
  //Fall11:
  TF1* turnOnTauMedium20ElecTauMC = new TF1("turnOnTauMedium20ElecTauMC", myFuncTurnOnTauMedium20ElecTauMC,19.5,800,0);
  turnOnTauMedium20ElecTauMC->SetNpx(3200);

  float weightLoose20  =  1103.;
  float weightMedium20 =  2772.;
  float weightTight20  =   786.;

  float total = weightLoose20+weightMedium20+weightTight20;
  weightLoose20  /= total;
  weightMedium20 /= total;
  weightTight20  /= total;

  Float_t xx = x[0];

  float ratio = turnOnTauMedium20ElecTauMC->Eval(xx) > 0 ? 
    (ratioEffTauLoose20ElecTau->turnOn(xx)  * weightLoose20 +
     ratioEffTauMedium20ElecTau->turnOn(xx) * weightMedium20 +
     ratioEffTauTight20ElecTau->turnOn(xx)  * weightTight20
     ) / turnOnTauMedium20ElecTauMC->Eval(xx) : 0.;
  
  return ratio;




  //ratioEfficiencyTest* ratioEffTauLoose15ElecTauRunA  = new ratioEfficiencyTest(14.32,1.11,1.84,1.34,1.0);
  //ratioEfficiencyTest* ratioEffTauLoose20ElecTauRunA  = new ratioEfficiencyTest(19.38,1.05,1.99,1.25,1.0);
  //ratioEfficiencyTest* ratioEffTauTight20ElecTauRunA  = new ratioEfficiencyTest(19.49,1.16,1.82,1.10,1.04);
  //ratioEfficiencyTest* ratioEffTauMedium20ElecTauRunA = new ratioEfficiencyTest(19.37 ,2.34,  3.06, 1.37,1.0);
  //ratioEfficiencyTest* ratioEffTauMedium20ElecTauRunB = new ratioEfficiencyTest(19.47 ,2.4184,5.161,1.11,0.875);
  
  //float weightLoose15RunA  =  168.6 ;
  //float weightLoose20RunA  =  934.0 ;
  //float weightTight20RunA  =  785.7 ;
  //float weightMedium20RunA =  240.0 ;
  //float weightMedium20RunB = 2522.0 ;
  //float totalRun2011 = weightLoose15RunA+weightLoose20RunA+weightTight20RunA+weightMedium20RunA+weightMedium20RunB;

  //weightLoose15RunA  /= totalRun2011;
  //weightLoose20RunA  /= totalRun2011;
  //weightTight20RunA  /= totalRun2011;
  //weightMedium20RunA /= totalRun2011;
  //weightMedium20RunB /= totalRun2011;

  //gSystem->Load("ratioEfficiencyTest_C.so");
  //ratioEfficiencyTest* ratioEffTauLoose15MC = new ratioEfficiencyTest(15.57,1.77,3.34,1.11,1.00);

  
  //Float_t xx = x[0];
  //float effMC = ratioEffTauLoose15MC->turnOn(xx);

  //return (ratioEffTauLoose15ElecTauRunA->turnOn(xx) *weightLoose15RunA  +
  //	  ratioEffTauLoose20ElecTauRunA->turnOn(xx) *weightLoose20RunA  +
  //  ratioEffTauTight20ElecTauRunA->turnOn(xx) *weightTight20RunA  +
  // ratioEffTauMedium20ElecTauRunA->turnOn(xx)*weightMedium20RunA +
  //  ratioEffTauMedium20ElecTauRunB->turnOn(xx)*weightMedium20RunB
  //  )
  /// effMC;

}


Double_t myFuncTurnOnTauElecTauAll(Double_t* x, Double_t *par) {
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  
  ratioEfficiencyTest* ratioEffTauLoose20ElecTau  = new ratioEfficiencyTest(19.63,-0.986,1.94,1.023979,1.911);
  ratioEfficiencyTest* ratioEffTauMedium20ElecTau = new ratioEfficiencyTest(19.35,0.37,0.1582,3.311,0.763);
  ratioEfficiencyTest* ratioEffTauTight20ElecTau  = new ratioEfficiencyTest(19.72,0.844,1.167,1.007466,9.351);

  float weightLoose20  =  1103.;
  float weightMedium20 =  2772.;
  float weightTight20  =   786.;

  float total = weightLoose20+weightMedium20+weightTight20;
  weightLoose20  /= total;
  weightMedium20 /= total;
  weightTight20  /= total;

  Float_t xx = x[0];

  return (ratioEffTauLoose20ElecTau->turnOn(xx)  * weightLoose20 +
	  ratioEffTauMedium20ElecTau->turnOn(xx) * weightMedium20 +
	  ratioEffTauTight20ElecTau->turnOn(xx)  * weightTight20
	  );

  //ratioEfficiencyTest* ratioEffTauLoose15ElecTauRunA  = new ratioEfficiencyTest(14.32,1.11,1.84,1.34,1.0);
  //ratioEfficiencyTest* ratioEffTauLoose20ElecTauRunA  = new ratioEfficiencyTest(19.38,1.05,1.99,1.25,1.0);
  //ratioEfficiencyTest* ratioEffTauTight20ElecTauRunA  = new ratioEfficiencyTest(19.49,1.16,1.82,1.10,1.04);
  //ratioEfficiencyTest* ratioEffTauMedium20ElecTauRunA = new ratioEfficiencyTest(19.37 ,2.34,  3.06, 1.37,1.0);
  //ratioEfficiencyTest* ratioEffTauMedium20ElecTauRunB = new ratioEfficiencyTest(19.47 ,2.4184,5.161,1.11,0.875);
  
  //float weightLoose15RunA  =  168.6 ;
  //float weightLoose20RunA  =  934.0 ;
  //float weightTight20RunA  =  785.7 ;
  //float weightMedium20RunA =  240.0 ;
  //float weightMedium20RunB = 2522.0 ;
  //float totalRun2011 = weightLoose15RunA+weightLoose20RunA+weightTight20RunA+weightMedium20RunA+weightMedium20RunB;

  //weightLoose15RunA  /= totalRun2011;
  //weightLoose20RunA  /= totalRun2011;
  //weightTight20RunA  /= totalRun2011;
  //weightMedium20RunA /= totalRun2011;
  //weightMedium20RunB /= totalRun2011;
 
  //Float_t xx = x[0];

  //return (ratioEffTauLoose15ElecTauRunA->turnOn(xx) *weightLoose15RunA  +
  //  ratioEffTauLoose20ElecTauRunA->turnOn(xx) *weightLoose20RunA  +
  //  ratioEffTauTight20ElecTauRunA->turnOn(xx) *weightTight20RunA  +
  //  ratioEffTauMedium20ElecTauRunA->turnOn(xx)*weightMedium20RunA +
  //  ratioEffTauMedium20ElecTauRunB->turnOn(xx)*weightMedium20RunB
  //  );

}








/////////////////////////////////////////////////






void makeFile(){

  TFile* fout = new TFile("llrCorrectionsReload.root","RECREATE");

  TF1 *ratioElec15BL        = new TF1("ratioElec15BL",           myFuncRatioElec15BL,       15,800,0);
  TF1 *turnOnElec15BL       = new TF1("turnOnElec15BL",          myFuncTurnOnElec15BL,      15,800,0);
  TF1 *ratioElec15EC        = new TF1("ratioElec15EC",           myFuncRatioElec15EC,       15,800,0);
  TF1 *turnOnElec15EC       = new TF1("turnOnElec15EC",          myFuncTurnOnElec15EC,      15,800,0);

  TF1 *ratioElec18BL        = new TF1("ratioElec18BL",           myFuncRatioEle18BL,        15,800,0);
  TF1 *turnOnElec18BL       = new TF1("turnOnElec18BL",          myFuncTurnOnEle18BL,       15,800,0);
  TF1 *ratioElec18EC        = new TF1("ratioElec18EC",           myFuncRatioEle18EC,        15,800,0);
  TF1 *turnOnElec18EC       = new TF1("turnOnElec18EC",          myFuncTurnOnEle18EC,       15,800,0);

  TF1 *ratioElec20BL        = new TF1("ratioElec20BL",           myFuncRatioEle20BL,        15,800,0);
  TF1 *turnOnElec20BL       = new TF1("turnOnElec20BL",          myFuncTurnOnEle20BL,       15,800,0);
  TF1 *ratioElec20EC        = new TF1("ratioElec20EC",           myFuncRatioEle20EC,        15,800,0);
  TF1 *turnOnElec20EC       = new TF1("turnOnElec20EC",          myFuncTurnOnEle20EC,       15,800,0);

  TF1 *ratioElecAllBL       = new TF1("ratioElecAllBL",          myFuncRatioEleAllBL,       15,800,0);
  TF1 *turnOnElecAllBL      = new TF1("turnOnElecAllBL",         myFuncTurnOnEleAllBL,      15,800,0);
  TF1 *ratioElecAllEC       = new TF1("ratioElecAllEC",          myFuncRatioEleAllEC,       15,800,0);
  TF1 *turnOnElecAllEC      = new TF1("turnOnElecAllEC",         myFuncTurnOnEleAllEC,      15,800,0);

  TF1 *ratioElecIDBL        = new TF1("ratioElecIDBL",           myFuncRatioElecIDBL ,      15,800,0);
  TF1 *turnOnElecIDBL       = new TF1("turnOnElecIDBL",          myFuncTurnOnElecIDBL ,     15,800,0);
  TF1 *ratioElecIDEC        = new TF1("ratioElecIDEC",           myFuncRatioElecIDEC ,      15,800,0);
  TF1 *turnOnElecIDEC       = new TF1("turnOnElecIDEC",          myFuncTurnOnElecIDEC ,     15,800,0);

  TF1 *ratioElecIsoBL       = new TF1("ratioElecIsoBL",          myFuncRatioElecIsoBL ,     15,800,0);
  TF1 *turnOnElecIsoBL      = new TF1("turnOnElecIsoBL",         myFuncTurnOnElecIsoBL ,    15,800,0);

  TF1 *ratioElecIsoEC       = new TF1("ratioElecIsoEC",          myFuncRatioElecIsoEC ,     15,800,0);
  TF1 *turnOnElecIsoEC      = new TF1("turnOnElecIsoEC",         myFuncTurnOnElecIsoEC ,    15,800,0);

  TF1 *ratioElecIDIsoBL     = new TF1("ratioElecIDIsoBL",        myFuncRatioElecIDIsoBL ,   15,800,0);
  TF1 *turnOnElecIDIsoBL    = new TF1("turnOnElecIDIsoBL",       myFuncTurnOnElecIDIsoBL ,  15,800,0);

  TF1 *ratioElecIDIsoEC     = new TF1("ratioElecIDIsoEC",        myFuncRatioElecIDIsoEC ,   15,800,0);
  TF1 *turnOnElecIDIsoEC    = new TF1("turnOnElecIDIsoEC",       myFuncTurnOnElecIDIsoEC ,  15,800,0);

  TF1 *ratioMu15L110BL      = new TF1("ratioMu15L110BL",         myFuncRatioMu15L110BL,     14,800,0);
  TF1 *turnOnMu15L110BL     = new TF1("turnOnMu15L110BL",        myFuncTurnOnMu15L110BL,    14,800,0);

  TF1 *ratioMu15L110EC      = new TF1("ratioMu15L110EC",         myFuncRatioMu15L110EC,     14,800,0);
  TF1 *turnOnMu15L110EC     = new TF1("turnOnMu15L110EC",        myFuncTurnOnMu15L110EC,    14,800,0);

  TF1 *ratioMu15L114BL      = new TF1("ratioMu15L114BL",         myFuncRatioMu15L114BL,     14,800,0);
  TF1 *turnOnMu15L114BL     = new TF1("turnOnMu15L114BL",        myFuncTurnOnMu15L114BL,    14,800,0);

  TF1 *ratioMu15L114EC      = new TF1("ratioMu15L114EC",         myFuncRatioMu15L114EC,     14,800,0);
  TF1 *turnOnMu15L114EC     = new TF1("turnOnMu15L114EC",        myFuncTurnOnMu15L114EC,    14,800,0);
 
  TF1 *ratioMuAllBL         = new TF1("ratioMuAllBL",            myFuncRatioMuAllBL,        14,800,0);
  TF1 *turnOnMuAllBL        = new TF1("turnOnMuAllBL",           myFuncTurnOnMuAllBL,       14,800,0);

  TF1 *ratioMuAllEC         = new TF1("ratioMuAllEC",            myFuncRatioMuAllEC,        14,800,0);
  TF1 *turnOnMuAllEC        = new TF1("turnOnMuAllEC",           myFuncTurnOnMuAllEC,       14,800,0);
 
  TF1 *ratioMuIdBL          = new TF1("ratioMuIdBL",             myFuncRatioMuIdBL ,        14,800,0);
  TF1 *turnOnMuIdBL         = new TF1("turnOnMuIdBL",            myFuncTurnOnMuIdBL ,       14,800,0);

  TF1 *ratioMuIdEC          = new TF1("ratioMuIdEC",             myFuncRatioMuIdEC ,        14,800,0);
  TF1 *turnOnMuIdEC         = new TF1("turnOnMuIdEC",            myFuncTurnOnMuIdEC ,       14,800,0);

  TF1 *ratioMuIsoBL         = new TF1("ratioMuIsoBL",            myFuncRatioMuIsoBL ,       14,800,0);
  TF1 *turnOnMuIsoBL        = new TF1("turnOnMuIsoBL",           myFuncTurnOnMuIsoBL ,      14,800,0);

  TF1 *ratioMuIsoEC         = new TF1("ratioMuIsoEC",            myFuncRatioMuIsoEC ,       14,800,0);
  TF1 *turnOnMuIsoEC        = new TF1("turnOnMuIsoEC",           myFuncTurnOnMuIsoEC ,      14,800,0);

  TF1 *ratioMuIDIsoBL       = new TF1("ratioMuIDIsoBL",          myFuncRatioMuIDIsoBL ,     14,800,0);
  TF1 *turnOnMuIDIsoBL      = new TF1("turnOnMuIDIsoBL",         myFuncTurnOnMuIDIsoBL ,    14,800,0);

  TF1 *ratioMuIDIsoEC       = new TF1("ratioMuIDIsoEC",          myFuncRatioMuIDIsoEC ,     14,800,0);
  TF1 *turnOnMuIDIsoEC      = new TF1("turnOnMuIDIsoEC",         myFuncTurnOnMuIDIsoEC ,    14,800,0);

  TF1 *turnOnTauLoose10MuTauMC      = new TF1("turnOnTauLoose10MuTauMC",       myFuncTurnOnTauLoose10MuTauMC,      18,800,0);
  TF1 *turnOnTauLoose15MuTauMC      = new TF1("turnOnTauLoose15MuTauMC",       myFuncTurnOnTauLoose15MuTauMC,      18,800,0);
  TF1 *turnOnTauLoose20MuTauMC      = new TF1("turnOnTauLoose20MuTauMC",       myFuncTurnOnTauLoose20MuTauMC,      18,800,0);
  TF1 *turnOnTauLoose15MuTauMC_v2   = new TF1("turnOnTauLoose15MuTauMC_v2",    myFuncTurnOnTauLoose15MuTauMC_v2,   18,800,0);

  TF1 *ratioTauLoose15MuTauRunA      = new TF1("ratioTauLoose15MuTauRunA",     myFuncRatioTauLoose15MuTauRunA,     18,800,0);
  TF1 *turnOnTauLoose15MuTauRunA     = new TF1("turnOnTauLoose15MuTauRunA",    myFuncTurnOnTauLoose15MuTauRunA,    18,800,0);
  TF1 *ratioTauLoose15ElecTauRunA    = new TF1("ratioTauLoose15ElecTauRunA",   myFuncRatioTauLoose15ElecTauRunA ,  18,800,0);
  TF1 *turnOnTauLoose15ElecTauRunA   = new TF1("turnOnTauLoose15ElecTauRunA",  myFuncTurnOnTauLoose15ElecTauRunA,  18,800,0);
  TF1 *ratioTauLoose20MuTauRunA      = new TF1("ratioTauLoose20MuTauRunA",     myFuncRatioTauLoose20MuTauRunA,     18,800,0);
  TF1 *turnOnTauLoose20MuTauRunA     = new TF1("turnOnTauLoose20MuTauRunA",    myFuncTurnOnTauLoose20MuTauRunA,    18,800,0);
  TF1 *ratioTauLoose20ElecTauRunA    = new TF1("ratioTauLoose20ElecTauRunA",   myFuncRatioTauLoose20ElecTauRunA ,  18,800,0);
  TF1 *turnOnTauLoose20ElecTauRunA   = new TF1("turnOnTauLoose20ElecTauRunA",  myFuncTurnOnTauLoose20ElecTauRunA,  18,800,0);
  TF1 *ratioTauMedium20ElecTauRunA   = new TF1("ratioTauMedium20ElecTauRunA",  myFuncRatioTauMedium20ElecTauRunA , 18,800,0);
  TF1 *turnOnTauMedium20ElecTauRunA  = new TF1("turnOnTauMedium20ElecTauRunA", myFuncTurnOnTauMedium20ElecTauRunA, 18,800,0);
  TF1 *ratioTauTight20ElecTauRunA    = new TF1("ratioTauTight20ElecTauRunA",   myFuncRatioTauTight20ElecTauRunA ,  18,800,0);
  TF1 *turnOnTauTight20ElecTauRunA   = new TF1("turnOnTauTight20ElecTauRunA",  myFuncTurnOnTauTight20ElecTauRunA,  18,800,0);
  TF1 *ratioTauLoose20MuTauRunB      = new TF1("ratioTauLoose20MuTauRunB",     myFuncRatioTauLoose20MuTauRunB,     18,800,0);
  TF1 *turnOnTauLoose20MuTauRunB     = new TF1("turnOnTauLoose20MuTauRunB",    myFuncTurnOnTauLoose20MuTauRunB,    18,800,0);
  TF1 *ratioTauLoose20ElecTauRunB    = new TF1("ratioTauLoose20ElecTauRunB",   myFuncRatioTauLoose20ElecTauRunB ,  18,800,0);
  TF1 *turnOnTauLoose20ElecTauRunB   = new TF1("turnOnTauLoose20ElecTauRunB",  myFuncTurnOnTauLoose20ElecTauRunB,  18,800,0);
  TF1 *ratioTauMedium20ElecTauRunB   = new TF1("ratioTauMedium20ElecTauRunB",  myFuncRatioTauMedium20ElecTauRunB , 18,800,0);
  TF1 *turnOnTauMedium20ElecTauRunB  = new TF1("turnOnTauMedium20ElecTauRunB", myFuncTurnOnTauMedium20ElecTauRunB, 18,800,0);
  TF1 *ratioTauTight20ElecTauRunB    = new TF1("ratioTauTight20ElecTauRunB",   myFuncRatioTauTight20ElecTauRunB ,  18,800,0);
  TF1 *turnOnTauTight20ElecTauRunB   = new TF1("turnOnTauTight20ElecTauRunB",  myFuncTurnOnTauTight20ElecTauRunB,  18,800,0);
  TF1 *ratioTauMedium20ElecTauRunAB  = new TF1("ratioTauMedium20ElecTauRunAB", myFuncRatioTauMedium20ElecTauRunAB ,18,800,0);
  TF1 *turnOnTauMedium20ElecTauRunAB = new TF1("turnOnTauMedium20ElecTauRunAB",myFuncTurnOnTauMedium20ElecTauRunAB,18,800,0);
 
  TF1 *ratioTauElecTauAll    = new TF1("ratioTauElecTauAll", myFuncRatioTauElecTauAll ,19.5,800,0);
  TF1 *turnOnTauElecTauAll   = new TF1("turnOnTauElecTauAll",myFuncTurnOnTauElecTauAll,19.5,800,0);
 
  TF1 *ratioTauMuTauAll      = new TF1("ratioTauMuTauAll", myFuncRatioTauMuTauAll ,18,800,0);
  TF1 *turnOnTauMuTauAll     = new TF1("turnOnTauMuTauAll",myFuncTurnOnTauMuTauAll,18,800,0);
 

  fout->cd();
  

  ratioElecIsoBL->SetNpx(3200);
  turnOnElecIsoBL->SetNpx(3200);
  ratioElecIsoEC->SetNpx(3200);
  turnOnElecIsoEC->SetNpx(3200);
  ratioMuIsoBL->SetNpx(3200);
  ratioMuIsoEC->SetNpx(3200);
  turnOnMuIsoBL->SetNpx(3200);
  turnOnMuIsoEC->SetNpx(3200);
  ratioElec15BL->SetNpx(3200);
  turnOnElec15BL->SetNpx(3200);
  ratioElec15EC->SetNpx(3200);
  turnOnElec15EC->SetNpx(3200);
  ratioElec18BL->SetNpx(3200);
  turnOnElec18BL->SetNpx(3200);
  ratioElec18EC->SetNpx(3200);
  turnOnElec18EC->SetNpx(3200);
  ratioElec20BL->SetNpx(3200);
  turnOnElec20BL->SetNpx(3200);
  ratioElec20EC->SetNpx(3200);
  turnOnElec20EC->SetNpx(3200);
  ratioElecAllBL->SetNpx(3200);
  turnOnElecAllBL->SetNpx(3200);
  ratioElecAllEC->SetNpx(3200);
  turnOnElecAllEC->SetNpx(3200);
  ratioMu15L114BL->SetNpx(3200);
  turnOnMu15L114BL->SetNpx(3200);
  ratioMu15L114EC->SetNpx(3200);
  turnOnMu15L114EC->SetNpx(3200);
  ratioMuAllBL->SetNpx(3200);
  turnOnMuAllBL->SetNpx(3200);
  ratioMuAllEC->SetNpx(3200);
  turnOnMuAllEC->SetNpx(3200);
  ratioTauElecTauAll->SetNpx(3200);
  turnOnTauElecTauAll->SetNpx(3200);
  ratioTauMuTauAll->SetNpx(3200);
  turnOnTauMuTauAll->SetNpx(3200);

  ratioElecIDIsoBL->SetNpx(6400);
  turnOnElecIDIsoBL->SetNpx(6400);
  ratioElecIDIsoEC->SetNpx(6400);
  turnOnElecIDIsoEC->SetNpx(6400);
  ratioMuIDIsoBL->SetNpx(6400);
  turnOnMuIDIsoBL->SetNpx(6400);
  ratioMuIDIsoEC->SetNpx(6400);
  turnOnMuIDIsoEC->SetNpx(6400);

  ratioElec15BL->Write();
  turnOnElec15BL->Write();
  ratioElec15EC->Write();
  turnOnElec15EC->Write();
  ratioElec18BL->Write();
  turnOnElec18BL->Write();
  ratioElec18EC->Write();
  turnOnElec18EC->Write();
  ratioElec20BL->Write();
  turnOnElec20BL->Write();
  ratioElec20EC->Write();
  turnOnElec20EC->Write();
  ratioElecAllBL->Write();
  turnOnElecAllBL->Write();
  ratioElecAllEC->Write();
  turnOnElecAllEC->Write();
  ratioElecIDBL->Write();
  turnOnElecIDBL->Write();
  ratioElecIDEC->Write();
  turnOnElecIDEC->Write();
  ratioElecIsoBL->Write();
  turnOnElecIsoBL->Write();
  ratioElecIsoEC->Write();
  turnOnElecIsoEC->Write();
  ratioElecIDIsoBL->Write();
  turnOnElecIDIsoBL->Write();
  ratioElecIDIsoEC->Write();
  turnOnElecIDIsoEC->Write();
  ratioMu15L110BL->Write();
  turnOnMu15L110BL->Write();
  ratioMu15L110EC->Write();
  turnOnMu15L110EC->Write();
  ratioMu15L114BL->Write();
  turnOnMu15L114BL->Write();
  ratioMu15L114EC->Write();
  turnOnMu15L114EC->Write();
  ratioMuAllBL->Write();
  turnOnMuAllBL->Write();
  ratioMuAllEC->Write();
  turnOnMuAllEC->Write();
  ratioMuIdBL->Write();
  ratioMuIdEC->Write();
  turnOnMuIdBL->Write();
  turnOnMuIdEC->Write();
  ratioMuIsoBL->Write();
  ratioMuIsoEC->Write();
  turnOnMuIsoBL->Write();
  turnOnMuIsoEC->Write();
  ratioMuIDIsoBL->Write();
  ratioMuIDIsoEC->Write();
  turnOnMuIDIsoBL->Write();
  turnOnMuIDIsoEC->Write();
  turnOnTauLoose10MuTauMC->Write();
  turnOnTauLoose15MuTauMC->Write();
  turnOnTauLoose20MuTauMC->Write();
  turnOnTauLoose15MuTauMC_v2->Write();
  ratioTauLoose15MuTauRunA->Write();
  turnOnTauLoose15MuTauRunA->Write();
  ratioTauLoose15ElecTauRunA->Write();
  turnOnTauLoose15ElecTauRunA->Write();
  ratioTauLoose20MuTauRunA->Write();
  turnOnTauLoose20MuTauRunA->Write();
  ratioTauLoose20ElecTauRunA->Write();
  turnOnTauLoose20ElecTauRunA->Write();
  ratioTauMedium20ElecTauRunA->Write();
  turnOnTauMedium20ElecTauRunA->Write();
  ratioTauTight20ElecTauRunA->Write();
  turnOnTauTight20ElecTauRunA->Write();
  ratioTauLoose20MuTauRunB->Write();
  turnOnTauLoose20MuTauRunB->Write();
  ratioTauLoose20ElecTauRunB->Write();
  turnOnTauLoose20ElecTauRunB->Write();
  ratioTauMedium20ElecTauRunB->Write();
  turnOnTauMedium20ElecTauRunB->Write();
  ratioTauTight20ElecTauRunB->Write();
  turnOnTauTight20ElecTauRunB->Write();
  ratioTauMedium20ElecTauRunAB->Write();
  turnOnTauMedium20ElecTauRunAB->Write();
  ratioTauElecTauAll->Write();
  turnOnTauElecTauAll->Write();
  ratioTauMuTauAll->Write();
  turnOnTauMuTauAll->Write();

  fout->Write();
  fout->Close();


}
