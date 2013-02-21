#include "TFile.h"
#include "TF1.h"
#include "TSystem.h"

//#include "ratioEfficiencyElec.C"
#include "ratioEfficiencyTest.C"

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
//ELEC ID
Double_t myFuncRatioElecIDBL(Double_t* x, Double_t *par) {  
  double ratio = 1.; 
  Float_t xx = x[0]; 
  if( xx < 24 )  
    ratio = 0.9130; 
  else if( xx >= 24 && xx< 30)  
    ratio = 0.9130; 
  else if( xx >= 30)
    ratio = 0.9567; 
  return ratio; 
}
Double_t myFuncTurnOnElecIDBL(Double_t* x, Double_t *par) {  
  double ratio = 1.;  
  Float_t xx = x[0];  
  if( xx < 24 )   
    ratio = 0.7532;  
  else if( xx >= 24 && xx< 30)   
    ratio = 0.7532;  
  else if( xx >= 30) 
    ratio = 0.8587;  
  return ratio;  
}
Double_t myFuncRatioElecIDEC(Double_t* x, Double_t *par) {
  double ratio = 1.;   
  Float_t xx = x[0];   
  if( xx < 24 )    
    ratio = 0.8509;   
  else if( xx >= 24 && xx< 30)    
    ratio = 0.8509 ;   
  else if( xx >= 30)  
    ratio = 0.9239 ;   
  return ratio;   
}
Double_t myFuncTurnOnElecIDEC(Double_t* x, Double_t *par) {
  double ratio = 1.;    
  Float_t xx = x[0];    
  if( xx < 24 )     
    ratio = 0.3631;    
  else if( xx >= 24 && xx< 30)     
    ratio = 0.3631;    
  else if( xx >= 30)   
    ratio = 0.5611 ;    
  return ratio;    
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
//ELECISO
Double_t myFuncRatioElecIsoBL(Double_t* x, Double_t *par) {
  double ratio = 1.;
  Float_t xx = x[0];
  if( xx < 24 ) 
    ratio = 0.9602;
  else if( xx >= 24 && xx< 30) 
    ratio = 0.9602;
  else if( xx >= 30)
    ratio = 0.9858 ;
  return ratio;
}
Double_t myFuncTurnOnElecIsoBL(Double_t* x, Double_t *par) { 
  double ratio = 1.;
  Float_t xx = x[0];
  if( xx < 24 ) 
    ratio = 0.7496;
  else if( xx >= 24 && xx< 30) 
    ratio = 0.7496;
  else if( xx >= 30)
    ratio = 0.8956;
  return ratio; 
}
Double_t myFuncRatioElecIsoEC(Double_t* x, Double_t *par) { 
  double ratio = 1.;
  Float_t xx = x[0];
  if( xx < 24 )  
    ratio = 0.9661; 
  else if( xx >= 24 && xx< 30)  
    ratio = 0.9661; 
  else if( xx >= 30) 
    ratio = 0.9942; 
  return ratio;
}
Double_t myFuncTurnOnElecIsoEC(Double_t* x, Double_t *par) {  
  double ratio = 1.;
  Float_t xx = x[0];
  if( xx < 24 )   
    ratio = 0.8244;  
  else if( xx >= 24 && xx< 30)   
    ratio = 0.8244;  
  else if( xx >= 30)  
    ratio = 0.9187 ;  
  return ratio;
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
//ELECTRIGGER
//Run 2012A
Double_t myFuncRatioEleRunABL(Double_t* x, Double_t *par) {  
  gSystem->Load("ratioEfficiencyTest_C.so");  
  ratioEfficiencyTest* fitEffElecRunABL = new ratioEfficiencyTest(20.4669,1.20429,1.84954,1.38645,0.891122);
  ratioEfficiencyTest* fitEffElecBLMC = new ratioEfficiencyTest(21.4136,0.000422,2.47314e-06,1.42487,1.00104);  
  Float_t xx = x[0];  
  return fitEffElecRunABL->turnOn(xx)/fitEffElecBLMC->turnOn(xx);  
}
Double_t myFuncTurnOnEleRunABL(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so"); 
  ratioEfficiencyTest* fitEffElecRunABL = new ratioEfficiencyTest(20.4669,1.20429,1.84954,1.38645,0.891122);
  Float_t xx = x[0];   
  return fitEffElecRunABL->turnOn(xx);
}
Double_t myFuncRatioEleRunAEC(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffElecRunAEC = new ratioEfficiencyTest(21.4136,1.93922,2.43562,1.00186,51.947);
  ratioEfficiencyTest* fitEffElecECMC = new ratioEfficiencyTest(20.9985,0.002918,3.43131e-05,1.41479,1.06506);
  Float_t xx = x[0];   
  return fitEffElecRunAEC->turnOn(xx)/fitEffElecECMC->turnOn(xx);   
}
Double_t myFuncTurnOnEleRunAEC(Double_t* x, Double_t *par) {
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffElecRunAEC = new ratioEfficiencyTest(21.4136,1.93922,2.43562,1.00186,51.947);
  Float_t xx = x[0];    
  return fitEffElecRunAEC->turnOn(xx);
}
//Run2012B
Double_t myFuncRatioEleRunBBL(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffElecRunBBL = new ratioEfficiencyTest(22.8618,0.844755,1.07941,1.27956,1.07722);
  ratioEfficiencyTest* fitEffElecBLMC = new ratioEfficiencyTest(21.4136,0.000422,2.47314e-06,1.42487,1.00104);  
  Float_t xx = x[0];   
  return fitEffElecRunBBL->turnOn(xx)/fitEffElecBLMC->turnOn(xx);   
}
Double_t myFuncTurnOnEleRunBBL(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");    
  ratioEfficiencyTest* fitEffElecRunBBL = new ratioEfficiencyTest(22.8618,0.844755,1.07941,1.27956,1.07722);
  Float_t xx = x[0];    
  return fitEffElecRunBBL->turnOn(xx);
}
/////////
Double_t myFuncRatioEleRunBEC(Double_t* x, Double_t *par) {
  gSystem->Load("ratioEfficiencyTest_C.so");    
  ratioEfficiencyTest* fitEffElecRunBEC = new ratioEfficiencyTest(22.1045,1.08481,0.780119,1.91846,0.962174);
  ratioEfficiencyTest* fitEffElecECMC = new ratioEfficiencyTest(20.9985,0.002918,3.43131e-05,1.41479,1.06506);
  Float_t xx = x[0];    
  return fitEffElecRunBEC->turnOn(xx)/fitEffElecECMC->turnOn(xx);     
}
Double_t myFuncTurnOnEleRunBEC(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");     
  ratioEfficiencyTest* fitEffElecRunBEC = new ratioEfficiencyTest(22.1045,1.08481,0.780119,1.91846,0.962174);
  Float_t xx = x[0];     
  return fitEffElecRunBEC->turnOn(xx);
}
//Run2012C
Double_t myFuncRatioEleRunCBL(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffElecRunCBL = new ratioEfficiencyTest(22.8598,0.855666,1.02951,1.32713,1.05486);
  ratioEfficiencyTest* fitEffElecBLMC = new ratioEfficiencyTest(21.4136,0.000422,2.47314e-06,1.42487,1.00104);  
  Float_t xx = x[0];   
  return fitEffElecRunCBL->turnOn(xx)/fitEffElecBLMC->turnOn(xx);   
}
Double_t myFuncTurnOnEleRunCBL(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");  
  ratioEfficiencyTest* fitEffElecRunCBL = new ratioEfficiencyTest(22.8598,0.855666,1.02951,1.32713,1.05486);
  Float_t xx = x[0];    
  return fitEffElecRunCBL->turnOn(xx);
}
Double_t myFuncRatioEleRunCEC(Double_t* x, Double_t *par) {
  gSystem->Load("ratioEfficiencyTest_C.so");    
  ratioEfficiencyTest* fitEffElecRunCEC = new ratioEfficiencyTest(21.7643,1.45024,0.785753,3.14722,0.926788);
  ratioEfficiencyTest* fitEffElecECMC = new ratioEfficiencyTest(20.9985,0.002918,3.43131e-05,1.41479,1.06506);
  Float_t xx = x[0];    
  return fitEffElecRunCEC->turnOn(xx)/fitEffElecECMC->turnOn(xx);     
}
Double_t myFuncTurnOnEleRunCEC(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");     
  ratioEfficiencyTest* fitEffElecRunCEC = new ratioEfficiencyTest(21.7643,1.45024,0.785753,3.14722,0.926788);
  Float_t xx = x[0];     
  return fitEffElecRunCEC->turnOn(xx);
}
//Combined Electron 
Double_t myFuncRatioEleAllBL(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffElecRunABL = new ratioEfficiencyTest(20.4669,1.20429,1.84954,1.38645,0.891122);
  ratioEfficiencyTest* fitEffElecRunBBL = new ratioEfficiencyTest(22.8618,0.844755,1.07941,1.27956,1.07722);
  ratioEfficiencyTest* fitEffElecRunCBL = new ratioEfficiencyTest(22.8598,0.855666,1.02951,1.32713,1.05486);
  ratioEfficiencyTest* fitEffElecBLMC = new ratioEfficiencyTest(21.4136,0.000422,2.47314e-06,1.42487,1.00104);  

  float weightElecRunA = 0.067; 
  float weightElecRunB = 0.367; 
  float weightElecRunC = 0.566; 
  float total = weightElecRunA+weightElecRunB+weightElecRunC;
  weightElecRunA/=total;
  weightElecRunB/=total;
  weightElecRunC/=total;
  Float_t xx = x[0];
  return (fitEffElecRunABL->turnOn(xx) * weightElecRunA +
	  fitEffElecRunBBL->turnOn(xx) * weightElecRunB +
	  fitEffElecRunCBL->turnOn(xx) * weightElecRunC
	  )/fitEffElecBLMC->turnOn(xx);
}
Double_t myFuncTurnOnEleAllBL(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so"); 
  ratioEfficiencyTest* fitEffElecRunABL = new ratioEfficiencyTest(20.4669,1.20429,1.84954,1.38645,0.891122);
  ratioEfficiencyTest* fitEffElecRunBBL = new ratioEfficiencyTest(22.8618,0.844755,1.07941,1.27956,1.07722);
  ratioEfficiencyTest* fitEffElecRunCBL = new ratioEfficiencyTest(22.8598,0.855666,1.02951,1.32713,1.05486);

  float weightElecRunA = 0.067; 
  float weightElecRunB = 0.367; 
  float weightElecRunC = 0.566; 
  float total = weightElecRunA+weightElecRunB+weightElecRunC;
  weightElecRunA/=total;
  weightElecRunB/=total;
  weightElecRunC/=total;
  Float_t xx = x[0];
  return (fitEffElecRunABL->turnOn(xx) * weightElecRunA +
	  fitEffElecRunBBL->turnOn(xx) * weightElecRunB +
	  fitEffElecRunCBL->turnOn(xx) * weightElecRunC
	  );
}
Double_t myFuncRatioEleAllEC(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffElecRunAEC = new ratioEfficiencyTest(21.4136,1.93922,2.43562,1.00186,51.947);
  ratioEfficiencyTest* fitEffElecRunBEC = new ratioEfficiencyTest(22.1045,1.08481,0.780119,1.91846,0.962174);
  ratioEfficiencyTest* fitEffElecRunCEC = new ratioEfficiencyTest(21.7643,1.45024,0.785753,3.14722,0.926788);
  ratioEfficiencyTest* fitEffElecECMC = new ratioEfficiencyTest(20.9985,0.002918,3.43131e-05,1.41479,1.06506);

  float weightElecRunA = 0.067; 
  float weightElecRunB = 0.367; 
  float weightElecRunC = 0.566; 
  float total = weightElecRunA+weightElecRunB+weightElecRunC;
  weightElecRunA/=total;
  weightElecRunB/=total;
  weightElecRunC/=total;
  Float_t xx = x[0];
  return (fitEffElecRunAEC->turnOn(xx) * weightElecRunA +
	  fitEffElecRunBEC->turnOn(xx) * weightElecRunB +
	  fitEffElecRunCEC->turnOn(xx) * weightElecRunC
	  )/fitEffElecECMC->turnOn(xx);
}

Double_t myFuncTurnOnEleAllEC(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffElecRunAEC = new ratioEfficiencyTest(21.4136,1.93922,2.43562,1.00186,51.947);
  ratioEfficiencyTest* fitEffElecRunBEC = new ratioEfficiencyTest(22.1045,1.08481,0.780119,1.91846,0.962174);
  ratioEfficiencyTest* fitEffElecRunCEC = new ratioEfficiencyTest(21.7643,1.45024,0.785753,3.14722,0.926788);

  float weightElecRunA = 0.067; 
  float weightElecRunB = 0.367; 
  float weightElecRunC = 0.566; 
  float total = weightElecRunA+weightElecRunB+weightElecRunC;
  weightElecRunA/=total;
  weightElecRunB/=total;
  weightElecRunC/=total;
  Float_t xx = x[0];
  return (fitEffElecRunAEC->turnOn(xx) * weightElecRunA +
	  fitEffElecRunBEC->turnOn(xx) * weightElecRunB +
	  fitEffElecRunCEC->turnOn(xx) * weightElecRunC
	  );
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
//MUID
///eta < 0.8
Double_t myFuncRatioMuIDBL1(Double_t* x, Double_t *par) {  
  double ratio = 1.;  
  Float_t xx = x[0];  
  if( xx < 15 )   
    ratio = 0.9877;  
  else if( xx >= 15 && xx< 20)   
    ratio = 0.9877;  
  else if( xx >= 20 && xx< 30)   
    ratio = 0.9870;  
  else if( xx >= 30) 
    ratio = 0.9871;  
  return ratio; 
} 
Double_t myFuncTurnOnMuIDBL1(Double_t* x, Double_t *par) {  
  double ratio = 1.;   
  Float_t xx = x[0];   
  if( xx < 15 )    
    ratio = 0.9538;   
  else if( xx >= 15 && xx< 20)    
    ratio = 0.9538;   
  else if( xx >= 20 && xx< 30)    
    ratio = 0.9524;   
  else if( xx >= 30)  
    ratio = 0.9508;   
  return ratio;   
 } 
///0.8 < eta < 1.2
Double_t myFuncRatioMuIDBL2(Double_t* x, Double_t *par) { 
  double ratio = 1.; 
  Float_t xx = x[0]; 
  if( xx < 15 )  
    ratio = 0.9813; 
  else if( xx >= 15 && xx< 20)  
    ratio = 0.9813; 
  else if( xx >= 20 && xx< 30)  
    ratio = 0.9837; 
  else if( xx >= 30)
    ratio = 0.9817; 
  return ratio; 
}
Double_t myFuncTurnOnMuIDBL2(Double_t* x, Double_t *par) { 
  double ratio = 1.;  
  Float_t xx = x[0];  
  if( xx < 15 )   
    ratio = 0.9400;  
  else if( xx >= 15 && xx< 20)   
    ratio = 0.9400;  
  else if( xx >= 20 && xx< 30)   
    ratio = 0.9430;  
  else if( xx >= 30) 
    ratio = 0.9406;  
  return ratio;  
}
///1.2 < eta < 2.1
Double_t myFuncRatioMuIDEC(Double_t* x, Double_t *par) {  
  double ratio = 1.;   
  Float_t xx = x[0];   
  if( xx < 15 )    
    ratio = 0.9885;   
  else if( xx >= 15 && xx< 20)    
    ratio = 0.9885;   
  else if( xx >= 20 && xx< 30)    
    ratio = 0.9914;   
  else if( xx >= 30)  
    ratio = 1.0018;   
  return ratio;   
}
Double_t myFuncTurnOnMuIDEC(Double_t* x, Double_t *par) {  
  double ratio = 1.;    
  Float_t xx = x[0];    
  if( xx < 15 )     
    ratio = 0.9417;    
  else if( xx >= 15 && xx< 20)     
    ratio = 0.9417;    
  else if( xx >= 20 && xx< 30)     
    ratio = 0.9430;    
  else if( xx >= 30)   
    ratio = 0.9508;    
  return ratio;    
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
//MUISO
///eta < 0.8
Double_t myFuncRatioMuIsoBL1(Double_t* x, Double_t *par) {  
  double ratio = 1.;      
  Float_t xx = x[0];      
  if( xx < 15 )       
    ratio = 0.9720 ;      
  else if( xx >= 15 && xx< 20)       
    ratio = 0.9720 ;      
  else if( xx >= 20 && xx< 30)       
    ratio = 0.9715 ;      
  else if( xx >= 30)     
    ratio = 0.9880 ;      
  return ratio;      
} 
Double_t myFuncTurnOnMuIsoBL1(Double_t* x, Double_t *par) {   
  double ratio = 1.;       
  Float_t xx = x[0];       
  if( xx < 15 )        
    ratio = 0.6390 ;       
  else if( xx >= 15 && xx< 20)        
    ratio = 0.6390 ;       
  else if( xx >= 20 && xx< 30)        
    ratio = 0.7642 ;       
  else if( xx >= 30)      
    ratio = 0.9041 ;       
  return ratio;       
} 
///0.8 < eta < 1.2
Double_t myFuncRatioMuIsoBL2(Double_t* x, Double_t *par) { 
  double ratio = 1.;     
  Float_t xx = x[0];     
  if( xx < 15 )      
    ratio = 0.9677 ;     
  else if( xx >= 15 && xx< 20)      
    ratio = 0.9677 ;     
  else if( xx >= 20 && xx< 30)      
    ratio = 0.9826 ;     
  else if( xx >= 30)    
    ratio = 0.9870 ;     
  return ratio;     
}
Double_t myFuncTurnOnMuIsoBL2(Double_t* x, Double_t *par) {  
  double ratio = 1.;      
  Float_t xx = x[0];      
  if( xx < 15 )       
    ratio = 0.7036 ;      
  else if( xx >= 15 && xx< 20)       
    ratio = 0.7036 ;      
  else if( xx >= 20 && xx< 30)       
    ratio = 0.8080 ;      
  else if( xx >= 30)     
    ratio = 0.9239 ;      
  return ratio;      
}
///1.2 < eta < 2.1
Double_t myFuncRatioMuIsoEC(Double_t* x, Double_t *par) {  
  double ratio = 1.;       
  Float_t xx = x[0];       
  if( xx < 15 )        
    ratio = 0.9843 ;       
  else if( xx >= 15 && xx< 20)        
    ratio = 0.9843 ;       
  else if( xx >= 20 && xx< 30)        
    ratio = 0.9960 ;       
  else if( xx >= 30)      
    ratio = 1.0008 ;       
  return ratio;       
}
Double_t myFuncTurnOnMuIsoEC(Double_t* x, Double_t *par) { 
  double ratio = 1.;        
  Float_t xx = x[0];        
  if( xx < 15 )         
    ratio = 0.7561 ;        
  else if( xx >= 15 && xx< 20)         
    ratio = 0.7561 ;        
  else if( xx >= 20 && xx< 30)         
    ratio = 0.8497 ;        
  else if( xx >= 30)       
    ratio = 0.9397 ;        
  return ratio;        
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
//MUTRIGGER
//Run 2012A 
//eta(mu)>0
Double_t myFuncTurnOnMuRunABL1Pos(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");  
  ratioEfficiencyTest* fitEffMuBL1Pos = new ratioEfficiencyTest(17.9605,0.500059,0.865294,1.04633,1.69027);
  Float_t xx = x[0];  
  return fitEffMuBL1Pos->turnOn(xx);
}
Double_t myFuncTurnOnMuRunABL2Pos(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffMuBL2Pos = new ratioEfficiencyTest(18.094,0.607997,0.89385,1.36337,0.92399); 
  Float_t xx = x[0];   
  return fitEffMuBL2Pos->turnOn(xx);
}
Double_t myFuncTurnOnMuRunAECPos(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffMuECPos = new ratioEfficiencyTest(16.9805,9.18396e-05,2.81836e-08,1.83783,0.858988); 
  Float_t xx = x[0];   
  return fitEffMuECPos->turnOn(xx);
}
//eta(mu)<0
Double_t myFuncTurnOnMuRunABL1Neg(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");  
  ratioEfficiencyTest* fitEffMuBL1Neg = new ratioEfficiencyTest(17.2736,0.13896,0.198452,1.13119,1.21897);
  Float_t xx = x[0];  
  return fitEffMuBL1Neg->turnOn(xx);
}
Double_t myFuncTurnOnMuRunABL2Neg(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffMuBL2Neg = new ratioEfficiencyTest(16.9824,0.0694986,0.0186614,1.66577,0.908218); 
  Float_t xx = x[0];   
  return fitEffMuBL2Neg->turnOn(xx);
}
Double_t myFuncTurnOnMuRunAECNeg(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffMuECNeg = new ratioEfficiencyTest(16.9993,8.82202e-05,7.91529e-08,1.40792,0.928102); 
  Float_t xx = x[0];   
  return fitEffMuECNeg->turnOn(xx);
}
///////////////////////////////////////////////////////
//Run2012B
//eta(mu)>0
Double_t myFuncTurnOnMuRunBBL1Pos(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");  
  ratioEfficiencyTest* fitEffMuBL1Pos = new ratioEfficiencyTest(15.9829,0.0435624,0.0196399,1.71605,0.967839);
  Float_t xx = x[0];  
  return fitEffMuBL1Pos->turnOn(xx);
}
Double_t myFuncTurnOnMuRunBBL2Pos(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffMuBL2Pos = new ratioEfficiencyTest(17.4688,0.0494554,0.0628053,1.34067,0.904989); 
  Float_t xx = x[0];   
  return fitEffMuBL2Pos->turnOn(xx);
}
Double_t myFuncTurnOnMuRunBECPos(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffMuECPos = new ratioEfficiencyTest(16.0029,4.01862e-05,6.62491e-08,1.42189,0.880251);
  Float_t xx = x[0];   
  return fitEffMuECPos->turnOn(xx);
}
//eta(mu)<0
Double_t myFuncTurnOnMuRunBBL1Neg(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");  
  ratioEfficiencyTest* fitEffMuBL1Neg = new ratioEfficiencyTest(16.4569,0.214484,0.302707,1.42363,0.982643);
  Float_t xx = x[0];  
  return fitEffMuBL1Neg->turnOn(xx);
}
Double_t myFuncTurnOnMuRunBBL2Neg(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffMuBL2Neg = new ratioEfficiencyTest(18.015,0.051293,0.0603545,1.36001,0.907481); 
  Float_t xx = x[0];   
  return fitEffMuBL2Neg->turnOn(xx);
}
Double_t myFuncTurnOnMuRunBECNeg(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffMuECNeg = new ratioEfficiencyTest(16.0015,5.59745e-07,1.3395e-07,1.37357,0.891284); 
  Float_t xx = x[0];   
  return fitEffMuECNeg->turnOn(xx);
}
///////////////////////////////////////////////////////
//Run2012C
//eta(mu)>0
Double_t myFuncTurnOnMuRunCBL1Pos(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");  
  ratioEfficiencyTest* fitEffMuBL1Pos = new ratioEfficiencyTest(15.9762,0.0552286,0.0231409,1.78576,0.96848);
  Float_t xx = x[0];  
  return fitEffMuBL1Pos->turnOn(xx);
}
Double_t myFuncTurnOnMuRunCBL2Pos(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffMuBL2Pos = new ratioEfficiencyTest(17.462,0.804351,1.62323,1.22776,0.900085);
  Float_t xx = x[0];   
  return fitEffMuBL2Pos->turnOn(xx);
}
Double_t myFuncTurnOnMuRunCECPos(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffMuECPos = new ratioEfficiencyTest(16.0051,-4.10239e-05,1.15509e-08,1.82463,0.865417);
  Float_t xx = x[0];   
  return fitEffMuECPos->turnOn(xx);
}
//eta(mu)<0
Double_t myFuncTurnOnMuRunCBL1Neg(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");  
  ratioEfficiencyTest* fitEffMuBL1Neg = new ratioEfficiencyTest(15.9788,0.044455,0.0215911,1.71024,0.965673);
  Float_t xx = x[0];  
  return fitEffMuBL1Neg->turnOn(xx);
}
Double_t myFuncTurnOnMuRunCBL2Neg(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffMuBL2Neg = new ratioEfficiencyTest(17.446,0.760355,1.58032,1.0623,1.10472); 
  Float_t xx = x[0];   
  return fitEffMuBL2Neg->turnOn(xx);
}
Double_t myFuncTurnOnMuRunCECNeg(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffMuECNeg = new ratioEfficiencyTest(15.9974,7.20337e-05,7.72238e-08,1.5461,0.87064); 
  Float_t xx = x[0];   
  return fitEffMuECNeg->turnOn(xx);
}


///////////////////////////////////
//combined Mu
Double_t myFuncRatioMuAllBL1Pos(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffMuRunABL1Pos = new ratioEfficiencyTest(17.9605,0.500059,0.865294,1.04633,1.69027);
  ratioEfficiencyTest* fitEffMuRunBBL1Pos = new ratioEfficiencyTest(15.9829,0.0435624,0.0196399,1.71605,0.967839);
  ratioEfficiencyTest* fitEffMuRunCBL1Pos = new ratioEfficiencyTest(15.9762,0.0552286,0.0231409,1.78576,0.96848);
  ratioEfficiencyTest* fitEffMuMCBL1Pos = new ratioEfficiencyTest(15.9618,0.0587497,0.0189749,1.94016,0.978294);
  float weightMuRunA = 0.067; 
  float weightMuRunB = 0.367; 
  float weightMuRunC = 0.566; 
  float total = weightMuRunA+weightMuRunB+weightMuRunC;
  weightMuRunA/=total;
  weightMuRunB/=total;
  weightMuRunC/=total;
  Float_t xx = x[0];
  return (fitEffMuRunABL1Pos->turnOn(xx) * weightMuRunA +
	  fitEffMuRunBBL1Pos->turnOn(xx) * weightMuRunB +
	  fitEffMuRunCBL1Pos->turnOn(xx) * weightMuRunC
	  )/fitEffMuMCBL1Pos->turnOn(xx);
}
Double_t myFuncTurnOnMuAllBL1Pos(Double_t* x, Double_t *par) {
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffMuRunABL1Pos = new ratioEfficiencyTest(17.9605,0.500059,0.865294,1.04633,1.69027);
  ratioEfficiencyTest* fitEffMuRunBBL1Pos = new ratioEfficiencyTest(15.9829,0.0435624,0.0196399,1.71605,0.967839);
  ratioEfficiencyTest* fitEffMuRunCBL1Pos = new ratioEfficiencyTest(15.9762,0.0552286,0.0231409,1.78576,0.96848);
  float weightMuRunA = 0.067; 
  float weightMuRunB = 0.367; 
  float weightMuRunC = 0.566; 
  float total = weightMuRunA+weightMuRunB+weightMuRunC;
  weightMuRunA/=total;
  weightMuRunB/=total;
  weightMuRunC/=total;
  Float_t xx = x[0];
  return (fitEffMuRunABL1Pos->turnOn(xx) * weightMuRunA +
	  fitEffMuRunBBL1Pos->turnOn(xx) * weightMuRunB +
	  fitEffMuRunCBL1Pos->turnOn(xx) * weightMuRunC
	  );
}
Double_t myFuncRatioMuAllBL2Pos(Double_t* x, Double_t *par) {
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffMuRunABL2Pos = new ratioEfficiencyTest(18.094,0.607997,0.89385,1.36337,0.92399); 
  ratioEfficiencyTest* fitEffMuRunBBL2Pos = new ratioEfficiencyTest(17.4688,0.0494554,0.0628053,1.34067,0.904989); 
  ratioEfficiencyTest* fitEffMuRunCBL2Pos = new ratioEfficiencyTest(17.462,0.804351,1.62323,1.22776,0.900085);
  ratioEfficiencyTest* fitEffMuMCBL2Pos = new ratioEfficiencyTest(16.7859,0.443337,0.571078,1.62214,0.919211);
  float weightMuRunA = 0.067; 
  float weightMuRunB = 0.367; 
  float weightMuRunC = 0.566; 
  float total = weightMuRunA+weightMuRunB+weightMuRunC;
  weightMuRunA/=total;
  weightMuRunB/=total;
  weightMuRunC/=total;
  Float_t xx = x[0];
  return (fitEffMuRunABL2Pos->turnOn(xx) * weightMuRunA +
	  fitEffMuRunBBL2Pos->turnOn(xx) * weightMuRunB +
	  fitEffMuRunCBL2Pos->turnOn(xx) * weightMuRunC
	  )/fitEffMuMCBL2Pos->turnOn(xx);
}
Double_t myFuncTurnOnMuAllBL2Pos(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffMuRunABL2Pos = new ratioEfficiencyTest(18.094,0.607997,0.89385,1.36337,0.92399); 
  ratioEfficiencyTest* fitEffMuRunBBL2Pos = new ratioEfficiencyTest(17.4688,0.0494554,0.0628053,1.34067,0.904989); 
  ratioEfficiencyTest* fitEffMuRunCBL2Pos = new ratioEfficiencyTest(17.462,0.804351,1.62323,1.22776,0.900085);
  float weightMuRunA = 0.067; 
  float weightMuRunB = 0.367; 
  float weightMuRunC = 0.566; 
  float total = weightMuRunA+weightMuRunB+weightMuRunC;
  weightMuRunA/=total;
  weightMuRunB/=total;
  weightMuRunC/=total;
  Float_t xx = x[0];
  return (fitEffMuRunABL2Pos->turnOn(xx) * weightMuRunA +
	  fitEffMuRunBBL2Pos->turnOn(xx) * weightMuRunB +
	  fitEffMuRunCBL2Pos->turnOn(xx) * weightMuRunC
	  );
}
Double_t myFuncRatioMuAllECPos(Double_t* x, Double_t *par) {
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffMuRunAECPos = new ratioEfficiencyTest(16.9805,9.18396e-05,2.81836e-08,1.83783,0.858988); 
  ratioEfficiencyTest* fitEffMuRunBECPos = new ratioEfficiencyTest(16.0029,4.01862e-05,6.62491e-08,1.42189,0.880251);
  ratioEfficiencyTest* fitEffMuRunCECPos = new ratioEfficiencyTest(16.0051,-4.10239e-05,1.15509e-08,1.82463,0.865417);
  ratioEfficiencyTest* fitEffMuMCECPos = new ratioEfficiencyTest(15.9974,8.50572e-05,5.53033e-08,1.64714,0.888026);
  float weightMuRunA = 0.067; 
  float weightMuRunB = 0.367; 
  float weightMuRunC = 0.566; 
  float total = weightMuRunA+weightMuRunB+weightMuRunC;
  weightMuRunA/=total;
  weightMuRunB/=total;
  weightMuRunC/=total;
  Float_t xx = x[0];
  return (fitEffMuRunAECPos->turnOn(xx) * weightMuRunA +
	  fitEffMuRunBECPos->turnOn(xx) * weightMuRunB +
	  fitEffMuRunCECPos->turnOn(xx) * weightMuRunC
	  )/fitEffMuMCECPos->turnOn(xx);
}
Double_t myFuncTurnOnMuAllECPos(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffMuRunAECPos = new ratioEfficiencyTest(16.9805,9.18396e-05,2.81836e-08,1.83783,0.858988); 
  ratioEfficiencyTest* fitEffMuRunBECPos = new ratioEfficiencyTest(16.0029,4.01862e-05,6.62491e-08,1.42189,0.880251);
  ratioEfficiencyTest* fitEffMuRunCECPos = new ratioEfficiencyTest(16.0051,-4.10239e-05,1.15509e-08,1.82463,0.865417);
  float weightMuRunA = 0.067; 
  float weightMuRunB = 0.367; 
  float weightMuRunC = 0.566; 
  float total = weightMuRunA+weightMuRunB+weightMuRunC;
  weightMuRunA/=total;
  weightMuRunB/=total;
  weightMuRunC/=total;
  Float_t xx = x[0];
  return (fitEffMuRunAECPos->turnOn(xx) * weightMuRunA +
	  fitEffMuRunBECPos->turnOn(xx) * weightMuRunB +
	  fitEffMuRunCECPos->turnOn(xx) * weightMuRunC
	  );
}
//eta(mu)<0
Double_t myFuncRatioMuAllBL1Neg(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffMuRunABL1Neg = new ratioEfficiencyTest(17.2736,0.13896,0.198452,1.13119,1.21897);
  ratioEfficiencyTest* fitEffMuRunBBL1Neg = new ratioEfficiencyTest(16.4569,0.214484,0.302707,1.42363,0.982643);
  ratioEfficiencyTest* fitEffMuRunCBL1Neg = new ratioEfficiencyTest(15.9788,0.044455,0.0215911,1.71024,0.965673);
  ratioEfficiencyTest* fitEffMuMCBL1Neg = new ratioEfficiencyTest(15.959,0.0229759,0.00597735,1.76124,0.980734);
  float weightMuRunA = 0.067; 
  float weightMuRunB = 0.367; 
  float weightMuRunC = 0.566; 
  float total = weightMuRunA+weightMuRunB+weightMuRunC;
  weightMuRunA/=total;
  weightMuRunB/=total;
  weightMuRunC/=total;
  Float_t xx = x[0];
  return (fitEffMuRunABL1Neg->turnOn(xx) * weightMuRunA +
	  fitEffMuRunBBL1Neg->turnOn(xx) * weightMuRunB +
	  fitEffMuRunCBL1Neg->turnOn(xx) * weightMuRunC
	  )/fitEffMuMCBL1Neg->turnOn(xx);
}
Double_t myFuncTurnOnMuAllBL1Neg(Double_t* x, Double_t *par) {
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffMuRunABL1Neg = new ratioEfficiencyTest(17.2736,0.13896,0.198452,1.13119,1.21897);
  ratioEfficiencyTest* fitEffMuRunBBL1Neg = new ratioEfficiencyTest(16.4569,0.214484,0.302707,1.42363,0.982643);
  ratioEfficiencyTest* fitEffMuRunCBL1Neg = new ratioEfficiencyTest(15.9788,0.044455,0.0215911,1.71024,0.965673);
  float weightMuRunA = 0.067; 
  float weightMuRunB = 0.367; 
  float weightMuRunC = 0.566; 
  float total = weightMuRunA+weightMuRunB+weightMuRunC;
  weightMuRunA/=total;
  weightMuRunB/=total;
  weightMuRunC/=total;
  Float_t xx = x[0];
  return (fitEffMuRunABL1Neg->turnOn(xx) * weightMuRunA +
	  fitEffMuRunBBL1Neg->turnOn(xx) * weightMuRunB +
	  fitEffMuRunCBL1Neg->turnOn(xx) * weightMuRunC
	  );
}
Double_t myFuncRatioMuAllBL2Neg(Double_t* x, Double_t *par) {
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffMuRunABL2Neg = new ratioEfficiencyTest(16.9824,0.0694986,0.0186614,1.66577,0.908218); 
  ratioEfficiencyTest* fitEffMuRunBBL2Neg = new ratioEfficiencyTest(18.015,0.051293,0.0603545,1.36001,0.907481); 
  ratioEfficiencyTest* fitEffMuRunCBL2Neg = new ratioEfficiencyTest(17.446,0.760355,1.58032,1.0623,1.10472); 
  ratioEfficiencyTest* fitEffMuMCBL2Neg = new ratioEfficiencyTest(17.3339,0.768105,1.31172,1.35161,0.942887);
  float weightMuRunA = 0.067; 
  float weightMuRunB = 0.367; 
  float weightMuRunC = 0.566; 
  float total = weightMuRunA+weightMuRunB+weightMuRunC;
  weightMuRunA/=total;
  weightMuRunB/=total;
  weightMuRunC/=total;
  Float_t xx = x[0];
  return (fitEffMuRunABL2Neg->turnOn(xx) * weightMuRunA +
	  fitEffMuRunBBL2Neg->turnOn(xx) * weightMuRunB +
	  fitEffMuRunCBL2Neg->turnOn(xx) * weightMuRunC
	  )/fitEffMuMCBL2Neg->turnOn(xx);
}
Double_t myFuncTurnOnMuAllBL2Neg(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffMuRunABL2Neg = new ratioEfficiencyTest(16.9824,0.0694986,0.0186614,1.66577,0.908218); 
  ratioEfficiencyTest* fitEffMuRunBBL2Neg = new ratioEfficiencyTest(18.015,0.051293,0.0603545,1.36001,0.907481); 
  ratioEfficiencyTest* fitEffMuRunCBL2Neg = new ratioEfficiencyTest(17.446,0.760355,1.58032,1.0623,1.10472); 
  float weightMuRunA = 0.067; 
  float weightMuRunB = 0.367; 
  float weightMuRunC = 0.566; 
  float total = weightMuRunA+weightMuRunB+weightMuRunC;
  weightMuRunA/=total;
  weightMuRunB/=total;
  weightMuRunC/=total;
  Float_t xx = x[0];
  return (fitEffMuRunABL2Neg->turnOn(xx) * weightMuRunA +
	  fitEffMuRunBBL2Neg->turnOn(xx) * weightMuRunB +
	  fitEffMuRunCBL2Neg->turnOn(xx) * weightMuRunC
	  );
}
Double_t myFuncRatioMuAllECNeg(Double_t* x, Double_t *par) {
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffMuRunAECNeg = new ratioEfficiencyTest(16.9993,8.82202e-05,7.91529e-08,1.40792,0.928102); 
  ratioEfficiencyTest* fitEffMuRunBECNeg = new ratioEfficiencyTest(16.0015,5.59745e-07,1.3395e-07,1.37357,0.891284); 
  ratioEfficiencyTest* fitEffMuRunCECNeg = new ratioEfficiencyTest(15.9974,7.20337e-05,7.72238e-08,1.5461,0.87064); 
  ratioEfficiencyTest* fitEffMuMCECNeg = new ratioEfficiencyTest(15.997,8.73042e-05,5.36172e-08,1.67934,0.871415);
  float weightMuRunA = 0.067; 
  float weightMuRunB = 0.367; 
  float weightMuRunC = 0.566; 
  float total = weightMuRunA+weightMuRunB+weightMuRunC;
  weightMuRunA/=total;
  weightMuRunB/=total;
  weightMuRunC/=total;
  Float_t xx = x[0];
  return (fitEffMuRunAECNeg->turnOn(xx) * weightMuRunA +
	  fitEffMuRunBECNeg->turnOn(xx) * weightMuRunB +
	  fitEffMuRunCECNeg->turnOn(xx) * weightMuRunC
	  )/fitEffMuMCECNeg->turnOn(xx);
}
Double_t myFuncTurnOnMuAllECNeg(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffMuRunAECNeg = new ratioEfficiencyTest(16.9993,8.82202e-05,7.91529e-08,1.40792,0.928102); 
  ratioEfficiencyTest* fitEffMuRunBECNeg = new ratioEfficiencyTest(16.0015,5.59745e-07,1.3395e-07,1.37357,0.891284); 
  ratioEfficiencyTest* fitEffMuRunCECNeg = new ratioEfficiencyTest(15.9974,7.20337e-05,7.72238e-08,1.5461,0.87064); 
  float weightMuRunA = 0.067; 
  float weightMuRunB = 0.367; 
  float weightMuRunC = 0.566; 
  float total = weightMuRunA+weightMuRunB+weightMuRunC;
  weightMuRunA/=total;
  weightMuRunB/=total;
  weightMuRunC/=total;
  Float_t xx = x[0];
  return (fitEffMuRunAECNeg->turnOn(xx) * weightMuRunA +
	  fitEffMuRunBECNeg->turnOn(xx) * weightMuRunB +
	  fitEffMuRunCECNeg->turnOn(xx) * weightMuRunC
	  );
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//TAU TRIGGER
//
/////////////////////////////////////////////////////////
// HLT PFTauLoose20 / MC Summer12 53X / elec+tau
//
Double_t myFuncTurnOnTauElecTauMCBL(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauMCBL = new ratioEfficiencyTest(18.40815138, 1.53235636, 3.55989632, 1.74542709, 0.90118450);
  Float_t xx = x[0];
  return ratioEffTauMCBL->turnOn(xx);
}
Double_t myFuncTurnOnTauElecTauMCEC(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so"); 
  ratioEfficiencyTest* ratioEffTauMCEC = new ratioEfficiencyTest(18.29028052, 1.56239255, 11.03605631, 155.89290151, 0.85683995);
  Float_t xx = x[0]; 
  return ratioEffTauMCEC->turnOn(xx); 
} 
/////////////////////////////////////////////////
// HLT PFTau All / e+tau
Double_t myFuncTurnOnTauElecTauBL(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauElecTauBL = new ratioEfficiencyTest(18.43442868,2.08967536,3.27357845,6.96327309,0.85564484);
  Float_t xx = x[0];
  return ratioEffTauElecTauBL->turnOn(xx);
}

Double_t myFuncRatioTauElecTauBL(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauElecTauBL = new ratioEfficiencyTest(18.43442868,2.08967536,3.27357845,6.96327309,0.85564484);
  ratioEfficiencyTest* ratioEffTauMCBL = new ratioEfficiencyTest(18.40815138, 1.53235636, 3.55989632, 1.74542709, 0.90118450);
  Float_t xx = x[0];
  return ratioEffTauElecTauBL->turnOn(xx)/ratioEffTauMCBL->turnOn(xx);
}

Double_t myFuncTurnOnTauElecTauEC(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauElecTauEC = new ratioEfficiencyTest(18.16839440,1.86184564,4.39116712,1.01410741,1.39240481);
  Float_t xx = x[0];
  return ratioEffTauElecTauEC->turnOn(xx);
}

Double_t myFuncRatioTauElecTauEC(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauElecTauEC = new ratioEfficiencyTest(18.16839440,1.86184564,4.39116712,1.01410741,1.39240481);
  ratioEfficiencyTest* ratioEffTauMCEC = new ratioEfficiencyTest(18.29028052, 1.56239255, 11.03605631, 155.89290151, 0.85683995);
  Float_t xx = x[0];
  return ratioEffTauElecTauEC->turnOn(xx)/ratioEffTauMCEC->turnOn(xx);
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// HLT PFTauLoose20 / MC Summer12 53X / elec+tau
//
Double_t myFuncTurnOnTauMuTauMCBL(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauMuTauMCBL = new ratioEfficiencyTest(18.80484409, 0.19082817, 0.19983010, 1.81979820, 0.93270649);
  Float_t xx = x[0];
  return ratioEffTauMuTauMCBL->turnOn(xx);
}
Double_t myFuncTurnOnTauMuTauMCEC(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauMuTauMCEC = new ratioEfficiencyTest(18.25975478, 1.32745225, 1.70380810, 149.18410074, 0.87377770);
  Float_t xx = x[0]; 
  return ratioEffTauMuTauMCEC->turnOn(xx); 
} 
/////////////////////////////////////////////////
// HLT PFTauLoose20 / Run2012A+B+C     / mu+tau
Double_t myFuncRatioTauMuTauBL(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauMuTauBL = new ratioEfficiencyTest(18.50940288, 1.62285299, 2.73232995, 1.79135412, 0.91481432);
  ratioEfficiencyTest* ratioEffTauMuTauMCBL = new ratioEfficiencyTest(18.80484409, 0.19082817, 0.19983010, 1.81979820, 0.93270649);
  Float_t xx = x[0];
  return ratioEffTauMuTauBL->turnOn(xx)/ratioEffTauMuTauMCBL->turnOn(xx);
}
Double_t myFuncTurnOnTauMuTauBL(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauMuTauBL = new ratioEfficiencyTest(18.50940288, 1.62285299, 2.73232995, 1.79135412, 0.91481432);
  Float_t xx = x[0];
  return ratioEffTauMuTauBL->turnOn(xx);
}
Double_t myFuncRatioTauMuTauEC(Double_t* x, Double_t *par) {    
  gSystem->Load("ratioEfficiencyTest_C.so"); 
  ratioEfficiencyTest* ratioEffTauMuTauEC = new ratioEfficiencyTest(18.45678784, 0.68697618, 0.57008697, 3.73470825, 0.84747211);
  ratioEfficiencyTest* ratioEffTauMuTauMCEC = new ratioEfficiencyTest(18.25975478, 1.32745225, 1.70380810, 149.18410074, 0.87377770);
  Float_t xx = x[0]; 
  return ratioEffTauMuTauEC->turnOn(xx)/ratioEffTauMuTauMCEC->turnOn(xx); 
} 
Double_t myFuncTurnOnTauMuTauEC(Double_t* x, Double_t *par) { 
  gSystem->Load("ratioEfficiencyTest_C.so");  
  ratioEfficiencyTest* ratioEffTauMuTauEC = new ratioEfficiencyTest(18.45678784, 0.68697618, 0.57008697, 3.73470825, 0.84747211);
  Float_t xx = x[0];  
  return ratioEffTauMuTauEC->turnOn(xx);
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
void makeFile(){

  TFile* fout = new TFile("llrCorrections.root","RECREATE");


  TF1 *ratioElecIDBL        = new TF1("ratioElecIDBL",           myFuncRatioElecIDBL ,      14,800,0);
  TF1 *turnOnElecIDBL       = new TF1("turnOnElecIDBL",          myFuncTurnOnElecIDBL ,     14,800,0);
  TF1 *ratioElecIDEC        = new TF1("ratioElecIDEC",           myFuncRatioElecIDEC ,      14,800,0);
  TF1 *turnOnElecIDEC       = new TF1("turnOnElecIDEC",          myFuncTurnOnElecIDEC ,     14,800,0);

  TF1 *ratioElecIsoBL       = new TF1("ratioElecIsoBL",          myFuncRatioElecIsoBL ,     14,800,0);
  TF1 *turnOnElecIsoBL      = new TF1("turnOnElecIsoBL",         myFuncTurnOnElecIsoBL ,    14,800,0);
  TF1 *ratioElecIsoEC       = new TF1("ratioElecIsoEC",          myFuncRatioElecIsoEC ,     14,800,0);
  TF1 *turnOnElecIsoEC      = new TF1("turnOnElecIsoEC",         myFuncTurnOnElecIsoEC ,    14,800,0);

  TF1 *ratioElecRunABL      = new TF1("ratioElecRunABL",         myFuncRatioEleRunABL,      24,800,0);
  TF1 *turnOnElecRunABL     = new TF1("turnOnElecRunABL",        myFuncTurnOnEleRunABL,     24,800,0);
  TF1 *ratioElecRunAEC      = new TF1("ratioElecRunAEC",         myFuncRatioEleRunAEC,      24,800,0);
  TF1 *turnOnElecRunAEC     = new TF1("turnOnElecRunAEC",        myFuncTurnOnEleRunAEC,     24,800,0);

  TF1 *ratioElecRunBBL      = new TF1("ratioElecRunBBL",         myFuncRatioEleRunBBL,      24,800,0);
  TF1 *turnOnElecRunBBL     = new TF1("turnOnElecRunBBL",        myFuncTurnOnEleRunBBL,     24,800,0);
  TF1 *ratioElecRunBEC      = new TF1("ratioElecRunBEC",         myFuncRatioEleRunBEC,      24,800,0);
  TF1 *turnOnElecRunBEC     = new TF1("turnOnElecRunBEC",        myFuncTurnOnEleRunBEC,     24,800,0);

  TF1 *ratioElecRunCBL      = new TF1("ratioElecRunCBL",         myFuncRatioEleRunCBL,      24,800,0);
  TF1 *turnOnElecRunCBL     = new TF1("turnOnElecRunCBL",        myFuncTurnOnEleRunCBL,     24,800,0);
  TF1 *ratioElecRunCEC      = new TF1("ratioElecRunCEC",         myFuncRatioEleRunCEC,      24,800,0);
  TF1 *turnOnElecRunCEC     = new TF1("turnOnElecRunCEC",        myFuncTurnOnEleRunCEC,     24,800,0);

  TF1 *ratioElecAllBL       = new TF1("ratioElecAllBL",          myFuncRatioEleAllBL,       24,800,0);
  TF1 *turnOnElecAllBL      = new TF1("turnOnElecAllBL",         myFuncTurnOnEleAllBL,      24,800,0);
  TF1 *ratioElecAllEC       = new TF1("ratioElecAllEC",          myFuncRatioEleAllEC,       24,800,0);
  TF1 *turnOnElecAllEC      = new TF1("turnOnElecAllEC",         myFuncTurnOnEleAllEC,      24,800,0);

  //

  TF1 *ratioMuIDBL1         = new TF1("ratioMuIDBL1",            myFuncRatioMuIDBL1,        14,800,0);
  TF1 *turnOnMuIDBL1        = new TF1("turnOnMuIDBL1",           myFuncTurnOnMuIDBL1,       14,800,0);
  TF1 *ratioMuIDBL2         = new TF1("ratioMuIDBL2",            myFuncRatioMuIDBL2,        14,800,0); 
  TF1 *turnOnMuIDBL2        = new TF1("turnOnMuIDBL2",           myFuncTurnOnMuIDBL2,       14,800,0); 
  TF1 *ratioMuIDEC          = new TF1("ratioMuIDEC",             myFuncRatioMuIDEC ,        14,800,0);
  TF1 *turnOnMuIDEC         = new TF1("turnOnMuIDEC",            myFuncTurnOnMuIDEC ,       14,800,0);

  TF1 *ratioMuIsoBL1        = new TF1("ratioMuIsoBL1",           myFuncRatioMuIsoBL1,       14,800,0);
  TF1 *turnOnMuIsoBL1       = new TF1("turnOnMuIsoBL1",          myFuncTurnOnMuIsoBL1,      14,800,0);
  TF1 *ratioMuIsoBL2        = new TF1("ratioMuIsoBL2",           myFuncRatioMuIsoBL2,       14,800,0); 
  TF1 *turnOnMuIsoBL2       = new TF1("turnOnMuIsoBL2",          myFuncTurnOnMuIsoBL2,      14,800,0);
  TF1 *ratioMuIsoEC         = new TF1("ratioMuIsoEC",            myFuncRatioMuIsoEC ,       14,800,0);
  TF1 *turnOnMuIsoEC        = new TF1("turnOnMuIsoEC",           myFuncTurnOnMuIsoEC ,      14,800,0);

  TF1 *turnOnMuRunABL1Pos   = new TF1("turnOnMuRunABL1Pos",      myFuncTurnOnMuRunABL1Pos,  20,800,0);
  TF1 *turnOnMuRunABL2Pos   = new TF1("turnOnMuRunABL2Pos",      myFuncTurnOnMuRunABL2Pos,  20,800,0);
  TF1 *turnOnMuRunAECPos    = new TF1("turnOnMuRunAECPos",       myFuncTurnOnMuRunAECPos,   20,800,0);
  TF1 *turnOnMuRunABL1Neg   = new TF1("turnOnMuRunABL1Neg",      myFuncTurnOnMuRunABL1Neg,  20,800,0);
  TF1 *turnOnMuRunABL2Neg   = new TF1("turnOnMuRunABL2Neg",      myFuncTurnOnMuRunABL2Neg,  20,800,0);
  TF1 *turnOnMuRunAECNeg    = new TF1("turnOnMuRunAECNeg",       myFuncTurnOnMuRunAECNeg,   20,800,0);

  TF1 *turnOnMuRunBBL1Pos   = new TF1("turnOnMuRunBBL1Pos",      myFuncTurnOnMuRunBBL1Pos,  20,800,0);
  TF1 *turnOnMuRunBBL2Pos   = new TF1("turnOnMuRunBBL2Pos",      myFuncTurnOnMuRunBBL2Pos,  20,800,0);
  TF1 *turnOnMuRunBECPos    = new TF1("turnOnMuRunBECPos",       myFuncTurnOnMuRunBECPos,   20,800,0);
  TF1 *turnOnMuRunBBL1Neg   = new TF1("turnOnMuRunBBL1Neg",      myFuncTurnOnMuRunBBL1Neg,  20,800,0);
  TF1 *turnOnMuRunBBL2Neg   = new TF1("turnOnMuRunBBL2Neg",      myFuncTurnOnMuRunBBL2Neg,  20,800,0);
  TF1 *turnOnMuRunBECNeg    = new TF1("turnOnMuRunBECNeg",       myFuncTurnOnMuRunBECNeg,   20,800,0);

  TF1 *turnOnMuRunCBL1Pos   = new TF1("turnOnMuRunCBL1Pos",      myFuncTurnOnMuRunCBL1Pos,  20,800,0);
  TF1 *turnOnMuRunCBL2Pos   = new TF1("turnOnMuRunCBL2Pos",      myFuncTurnOnMuRunCBL2Pos,  20,800,0);
  TF1 *turnOnMuRunCECPos    = new TF1("turnOnMuRunCECPos",       myFuncTurnOnMuRunCECPos,   20,800,0);
  TF1 *turnOnMuRunCBL1Neg   = new TF1("turnOnMuRunCBL1Neg",      myFuncTurnOnMuRunCBL1Neg,  20,800,0);
  TF1 *turnOnMuRunCBL2Neg   = new TF1("turnOnMuRunCBL2Neg",      myFuncTurnOnMuRunCBL2Neg,  20,800,0);
  TF1 *turnOnMuRunCECNeg    = new TF1("turnOnMuRunCECNeg",       myFuncTurnOnMuRunCECNeg,   20,800,0);

  TF1 *ratioMuAllBL1Pos     = new TF1("ratioMuAllBL1Pos",        myFuncRatioMuAllBL1Pos,    20,800,0);
  TF1 *turnOnMuAllBL1Pos    = new TF1("turnOnMuAllBL1Pos",       myFuncTurnOnMuAllBL1Pos,   20,800,0);
  TF1 *ratioMuAllBL2Pos     = new TF1("ratioMuAllBL2Pos",        myFuncRatioMuAllBL2Pos,    20,800,0);
  TF1 *turnOnMuAllBL2Pos    = new TF1("turnOnMuAllBL2Pos",       myFuncTurnOnMuAllBL2Pos,   20,800,0);
  TF1 *ratioMuAllECPos      = new TF1("ratioMuAllECPos",         myFuncRatioMuAllECPos,     20,800,0);
  TF1 *turnOnMuAllECPos     = new TF1("turnOnMuAllECPos",        myFuncTurnOnMuAllECPos,    20,800,0);
  TF1 *ratioMuAllBL1Neg     = new TF1("ratioMuAllBL1Neg",        myFuncRatioMuAllBL1Neg,    20,800,0);
  TF1 *turnOnMuAllBL1Neg    = new TF1("turnOnMuAllBL1Neg",       myFuncTurnOnMuAllBL1Neg,   20,800,0);
  TF1 *ratioMuAllBL2Neg     = new TF1("ratioMuAllBL2Neg",        myFuncRatioMuAllBL2Neg,    20,800,0);
  TF1 *turnOnMuAllBL2Neg    = new TF1("turnOnMuAllBL2Neg",       myFuncTurnOnMuAllBL2Neg,   20,800,0);
  TF1 *ratioMuAllECNeg      = new TF1("ratioMuAllECNeg",         myFuncRatioMuAllECNeg,     20,800,0);
  TF1 *turnOnMuAllECNeg     = new TF1("turnOnMuAllECNeg",        myFuncTurnOnMuAllECNeg,    20,800,0);

  //

  TF1 *turnOnTauElecTauMCBL      = new TF1("turnOnTauElecTauMCBL",   myFuncTurnOnTauElecTauMCBL, 20,800,0);
  TF1 *turnOnTauElecTauMCEC      = new TF1("turnOnTauElecTauMCEC",   myFuncTurnOnTauElecTauMCEC, 20,800,0);

  TF1 *ratioTauElecTauBL    = new TF1("ratioTauElecTauBL", myFuncRatioTauElecTauBL, 20,800,0);
  TF1 *turnOnTauElecTauBL   = new TF1("turnOnTauElecTauBL", myFuncTurnOnTauElecTauBL, 20,800,0);
  TF1 *ratioTauElecTauEC    = new TF1("ratioTauElecTauEC", myFuncRatioTauElecTauEC, 20,800,0);
  TF1 *turnOnTauElecTauEC   = new TF1("turnOnTauElecTauEC", myFuncTurnOnTauElecTauEC, 20,800,0);

  TF1 *turnOnTauMuTauMCBL      = new TF1("turnOnTauMuTauMCBL",   myFuncTurnOnTauMuTauMCBL,      20,800,0);
  TF1 *turnOnTauMuTauMCEC      = new TF1("turnOnTauMuTauMCEC",   myFuncTurnOnTauMuTauMCEC,	20,800,0); 

  TF1 *ratioTauMuTauBL      = new TF1("ratioTauMuTauBL", myFuncRatioTauMuTauBL ,20,800,0);
  TF1 *turnOnTauMuTauBL     = new TF1("turnOnTauMuTauBL",myFuncTurnOnTauMuTauBL,20,800,0);
  TF1 *ratioTauMuTauEC      = new TF1("ratioTauMuTauEC", myFuncRatioTauMuTauEC ,20,800,0); 
  TF1 *turnOnTauMuTauEC     = new TF1("turnOnTauMuTauEC",myFuncTurnOnTauMuTauEC,20,800,0);
  

  fout->cd();

  ratioElecIDBL->SetNpx(25600);
  turnOnElecIDBL->SetNpx(25600);
  ratioElecIDEC->SetNpx(25600);
  turnOnElecIDEC->SetNpx(25600);

  ratioElecIsoBL->SetNpx(25600);
  turnOnElecIsoBL->SetNpx(25600);
  ratioElecIsoEC->SetNpx(25600);
  turnOnElecIsoEC->SetNpx(25600);

  ratioElecRunABL->SetNpx(25600);
  turnOnElecRunABL->SetNpx(25600);
  ratioElecRunAEC->SetNpx(25600);
  turnOnElecRunAEC->SetNpx(25600);
  
  ratioElecRunBBL->SetNpx(25600);
  turnOnElecRunBBL->SetNpx(25600);
  ratioElecRunBEC->SetNpx(25600);
  turnOnElecRunBEC->SetNpx(25600);
  
  ratioElecRunCBL->SetNpx(25600);
  turnOnElecRunCBL->SetNpx(25600);
  ratioElecRunCEC->SetNpx(25600);
  turnOnElecRunCEC->SetNpx(25600);
  
  ratioElecAllBL->SetNpx(25600);
  turnOnElecAllBL->SetNpx(25600);
  ratioElecAllEC->SetNpx(25600);
  turnOnElecAllEC->SetNpx(25600);

  //

  ratioMuIDBL1->SetNpx(25600);
  turnOnMuIDBL1->SetNpx(25600);
  ratioMuIDBL2->SetNpx(25600); 
  turnOnMuIDBL2->SetNpx(25600);
  ratioMuIDEC->SetNpx(25600);
  turnOnMuIDEC->SetNpx(25600);

  ratioMuIsoBL1->SetNpx(25600);
  ratioMuIsoBL2->SetNpx(25600);
  ratioMuIsoEC->SetNpx(25600);
  turnOnMuIsoBL1->SetNpx(25600);
  turnOnMuIsoBL2->SetNpx(25600);
  turnOnMuIsoEC->SetNpx(25600);
  
  turnOnMuRunABL1Pos->SetNpx(25600);
  turnOnMuRunABL2Pos->SetNpx(25600);
  turnOnMuRunAECPos->SetNpx(25600);
  turnOnMuRunABL1Neg->SetNpx(25600);
  turnOnMuRunABL2Neg->SetNpx(25600);
  turnOnMuRunAECNeg->SetNpx(25600);
  
  turnOnMuRunBBL1Pos->SetNpx(25600);
  turnOnMuRunBBL2Pos->SetNpx(25600);  
  turnOnMuRunBECPos->SetNpx(25600); 
  turnOnMuRunBBL1Neg->SetNpx(25600);
  turnOnMuRunBBL2Neg->SetNpx(25600);
  turnOnMuRunBECNeg->SetNpx(25600);
  
  turnOnMuRunCBL1Pos->SetNpx(25600);
  turnOnMuRunCBL2Pos->SetNpx(25600);
  turnOnMuRunCECPos->SetNpx(25600);
  turnOnMuRunCBL1Neg->SetNpx(25600);
  turnOnMuRunCBL2Neg->SetNpx(25600);
  turnOnMuRunCECNeg->SetNpx(25600);

  ratioMuAllBL1Pos->SetNpx(25600);
  turnOnMuAllBL1Pos->SetNpx(25600);
  ratioMuAllBL2Pos->SetNpx(25600);
  turnOnMuAllBL2Pos->SetNpx(25600);
  ratioMuAllECPos->SetNpx(25600);
  turnOnMuAllECPos->SetNpx(25600);
  ratioMuAllBL1Neg->SetNpx(25600);
  turnOnMuAllBL1Neg->SetNpx(25600);
  ratioMuAllBL2Neg->SetNpx(25600);
  turnOnMuAllBL2Neg->SetNpx(25600);
  ratioMuAllECNeg->SetNpx(25600);
  turnOnMuAllECNeg->SetNpx(25600);

  //

  turnOnTauElecTauMCBL->SetNpx(25600);
  turnOnTauElecTauMCEC->SetNpx(25600);

  ratioTauElecTauBL->SetNpx(25600);
  turnOnTauElecTauBL->SetNpx(25600);
  ratioTauElecTauEC->SetNpx(25600);
  turnOnTauElecTauEC->SetNpx(25600);

  turnOnTauMuTauMCBL->SetNpx(25600);
  turnOnTauMuTauMCEC->SetNpx(25600);

  ratioTauMuTauBL->SetNpx(25600);
  turnOnTauMuTauBL->SetNpx(25600);
  ratioTauMuTauEC->SetNpx(25600); 
  turnOnTauMuTauEC->SetNpx(25600);

  //==============================
  ratioElecIDBL->Write();
  turnOnElecIDBL->Write();
  ratioElecIDEC->Write();
  turnOnElecIDEC->Write();

  ratioElecIsoBL->Write();
  turnOnElecIsoBL->Write();
  ratioElecIsoEC->Write();
  turnOnElecIsoEC->Write();

  ratioElecRunABL->Write();
  turnOnElecRunABL->Write();
  ratioElecRunAEC->Write();
  turnOnElecRunAEC->Write();
  
  ratioElecRunBBL->Write();
  turnOnElecRunBBL->Write();
  ratioElecRunBEC->Write();
  turnOnElecRunBEC->Write();
  
  ratioElecRunCBL->Write();
  turnOnElecRunCBL->Write();
  ratioElecRunCEC->Write();
  turnOnElecRunCEC->Write();
  
  ratioElecAllBL->Write();
  turnOnElecAllBL->Write();
  ratioElecAllEC->Write();
  turnOnElecAllEC->Write();

  //

  ratioMuIDBL1->Write();
  turnOnMuIDBL1->Write();
  ratioMuIDBL2->Write(); 
  turnOnMuIDBL2->Write();
  ratioMuIDEC->Write();
  turnOnMuIDEC->Write();

  ratioMuIsoBL1->Write();
  ratioMuIsoBL2->Write(); 
  ratioMuIsoEC->Write();
  turnOnMuIsoBL1->Write();
  turnOnMuIsoBL2->Write();
  turnOnMuIsoEC->Write();
  
  turnOnMuRunABL1Pos->Write();
  turnOnMuRunABL2Pos->Write();
  turnOnMuRunAECPos->Write();
  turnOnMuRunABL1Neg->Write();
  turnOnMuRunABL2Neg->Write();
  turnOnMuRunAECNeg->Write();
  
  turnOnMuRunBBL1Pos->Write();
  turnOnMuRunBBL2Pos->Write();  
  turnOnMuRunBECPos->Write(); 
  turnOnMuRunBBL1Neg->Write();
  turnOnMuRunBBL2Neg->Write();
  turnOnMuRunBECNeg->Write();
  
  turnOnMuRunCBL1Pos->Write();
  turnOnMuRunCBL2Pos->Write();
  turnOnMuRunCECPos->Write();
  turnOnMuRunCBL1Neg->Write();
  turnOnMuRunCBL2Neg->Write();
  turnOnMuRunCECNeg->Write();

  ratioMuAllBL1Pos->Write();
  turnOnMuAllBL1Pos->Write();
  ratioMuAllBL2Pos->Write();
  turnOnMuAllBL2Pos->Write();
  ratioMuAllECPos->Write();
  turnOnMuAllECPos->Write();
  ratioMuAllBL1Neg->Write();
  turnOnMuAllBL1Neg->Write();
  ratioMuAllBL2Neg->Write();
  turnOnMuAllBL2Neg->Write();
  ratioMuAllECNeg->Write();
  turnOnMuAllECNeg->Write();

  //

  turnOnTauElecTauMCBL->Write();
  turnOnTauElecTauMCEC->Write();

  ratioTauElecTauBL->Write();
  turnOnTauElecTauBL->Write();
  ratioTauElecTauEC->Write();
  turnOnTauElecTauEC->Write();

  turnOnTauMuTauMCBL->Write();
  turnOnTauMuTauMCEC->Write();

  ratioTauMuTauBL->Write();
  turnOnTauMuTauBL->Write();
  ratioTauMuTauEC->Write(); 
  turnOnTauMuTauEC->Write();

  fout->Write();
  fout->Close();
}
