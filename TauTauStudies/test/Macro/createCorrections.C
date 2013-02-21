#include "TFile.h"
#include "TF1.h"
#include "TSystem.h"

//#include "ratioEfficiencyElec.C"
#include "ratioEfficiencyTest.C"

//Run 2012A
//53X MC (new values from Valentina)
Double_t myFuncRatioElec20BL(Double_t* x, Double_t *par) {  //modified-53x
  gSystem->Load("ratioEfficiencyTest_C.so");  
  ratioEfficiencyTest* fitEffElec20BL = new ratioEfficiencyTest(20.97643939, 1.15196354, 2.27544602, 1.01743868, 2.04391816);
  ratioEfficiencyTest* fitEffElecBLMC = new ratioEfficiencyTest(22.00666445, 0.00036058, 0.00000251, 1.38456083, 1.02640579);  
  
  Float_t xx = x[0];  
  return fitEffElec20BL->turnOn(xx)/fitEffElecBLMC->turnOn(xx);  

}

Double_t myFuncTurnOnElec20BL(Double_t* x, Double_t *par) {  //modified

  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffElec20BL = new ratioEfficiencyTest(20.97643939, 1.15196354, 2.27544602, 1.01743868, 2.04391816);
  
  Float_t xx = x[0];   
  return fitEffElec20BL->turnOn(xx);
}

Double_t myFuncRatioElec20EC(Double_t* x, Double_t *par) { //modified-53x

  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffElec20EC = new ratioEfficiencyTest(20.59874300, 1.25425435, 1.61098921, 1.00146962, 60.35067579);
  ratioEfficiencyTest* fitEffElecECMC = new ratioEfficiencyTest(22.18226941, 1.07762306, 1.23712775, 1.27324238, 1.15312185);
   
  Float_t xx = x[0];   
  return fitEffElec20EC->turnOn(xx)/fitEffElecECMC->turnOn(xx);   
 
}

Double_t myFuncTurnOnElec20EC(Double_t* x, Double_t *par) { //modified
  
  gSystem->Load("ratioEfficiencyTest_C.so");    
  ratioEfficiencyTest* fitEffElec20EC = new ratioEfficiencyTest(20.59874300, 1.25425435, 1.61098921, 1.00146962, 60.35067579);

  Float_t xx = x[0];    
  return fitEffElec20EC->turnOn(xx);
}
//Run2012B

Double_t myFuncRatioEle22RunBBL(Double_t* x, Double_t *par) {  //modified-53x

  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffElec22BL = new ratioEfficiencyTest(22.90752344, 1.32376429, 2.17813319, 1.03674051, 2.15454768);
  ratioEfficiencyTest* fitEffElecBLMC = new ratioEfficiencyTest(22.00666445, 0.00036058, 0.00000251, 1.38456083, 1.02640579);
  
  Float_t xx = x[0];   
  return fitEffElec22BL->turnOn(xx)/fitEffElecBLMC->turnOn(xx);   
 
}

Double_t myFuncTurnOnEle22RunBBL(Double_t* x, Double_t *par) { //modified

  gSystem->Load("ratioEfficiencyTest_C.so");    
  ratioEfficiencyTest* fitEffElec22BL = new ratioEfficiencyTest(22.90752344, 1.32376429, 2.17813319, 1.03674051, 2.15454768);

  Float_t xx = x[0];    
  return fitEffElec22BL->turnOn(xx);
}


Double_t myFuncRatioEle22RunBEC(Double_t* x, Double_t *par) { //modified-53x

  gSystem->Load("ratioEfficiencyTest_C.so");    
  ratioEfficiencyTest* fitEffElec22EC = new ratioEfficiencyTest(22.14553261, 1.19913124, 1.75642067, 1.00826962, 9.04331617);
  ratioEfficiencyTest* fitEffElecECMC = new ratioEfficiencyTest(22.18226941, 1.07762306, 1.23712775, 1.27324238, 1.15312185);

   
  Float_t xx = x[0];    
  return fitEffElec22EC->turnOn(xx)/fitEffElecECMC->turnOn(xx);    
  
}


Double_t myFuncTurnOnEle22RunBEC(Double_t* x, Double_t *par) { //modified

  gSystem->Load("ratioEfficiencyTest_C.so");     
  ratioEfficiencyTest* fitEffElec22EC = new ratioEfficiencyTest(22.14553261, 1.19913124, 1.75642067, 1.00826962, 9.04331617);

  Float_t xx = x[0];     
  return fitEffElec22EC->turnOn(xx);
}

//Run2012C
Double_t myFuncRatioEle22RunCBL(Double_t* x, Double_t *par) {  //modified-53x

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffElec22BL = new ratioEfficiencyTest(23.05556088, 0.96047151, 1.24782044, 1.26042277,1.09675041);
  ratioEfficiencyTest* fitEffElecBLMC = new ratioEfficiencyTest(22.00666445, 0.00036058, 0.00000251, 1.38456083, 1.02640579);

  Float_t xx = x[0];
  return fitEffElec22BL->turnOn(xx)/fitEffElecBLMC->turnOn(xx);

}

Double_t myFuncTurnOnEle22RunCBL(Double_t* x, Double_t *par) { //modified

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffElec22BL = new ratioEfficiencyTest(23.05556088, 0.96047151, 1.24782044, 1.26042277,1.09675041);

  Float_t xx = x[0];
  return fitEffElec22BL->turnOn(xx);
}

Double_t myFuncRatioEle22RunCEC(Double_t* x, Double_t *par) { //modified-53x

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffElec22EC = new ratioEfficiencyTest(21.99911375, 1.15806380, 0.80675262, 1.98765770, 0.97138507);
  ratioEfficiencyTest* fitEffElecECMC = new ratioEfficiencyTest(22.18226941, 1.07762306, 1.23712775, 1.27324238, 1.15312185);


  Float_t xx = x[0];
  return fitEffElec22EC->turnOn(xx)/fitEffElecECMC->turnOn(xx);

}

Double_t myFuncTurnOnEle22RunCEC(Double_t* x, Double_t *par) { //modified

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffElec22EC = new ratioEfficiencyTest(21.99911375, 1.15806380, 0.80675262, 1.98765770, 0.97138507);

  Float_t xx = x[0];
  return fitEffElec22EC->turnOn(xx);
}

//Combined Electron

Double_t myFuncRatioEleAllBL(Double_t* x, Double_t *par) { //modified-53x
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffEle20BL = new ratioEfficiencyTest(20.97643939, 1.15196354, 2.27544602, 1.01743868, 2.04391816);
  ratioEfficiencyTest* fitEffEle22RunBBL = new ratioEfficiencyTest(22.90752344, 1.32376429, 2.17813319, 1.03674051, 2.15454768);
  ratioEfficiencyTest* fitEffEle22RunCBL = new ratioEfficiencyTest(23.05556088, 0.96047151, 1.24782044, 1.26042277,1.09675041);
  
  ratioEfficiencyTest* fitEffElecMCBL = new ratioEfficiencyTest(22.00666445, 0.00036058, 0.00000251, 1.38456083, 1.02640579);

  float weightElec20 = 0.067; //change
  float weightElec22RunB = 0.367; //change
  float weightElec22RunC = 0.566; //change

  float total = weightElec20+weightElec22RunB+weightElec22RunC;
  weightElec20/=total;
  weightElec22RunB/=total;
  weightElec22RunC/=total;

  Float_t xx = x[0];

  return (fitEffEle20BL->turnOn(xx) * weightElec20 +
	  fitEffEle22RunBBL->turnOn(xx) * weightElec22RunB + 
	  fitEffEle22RunCBL->turnOn(xx) * weightElec22RunC 
	  )/fitEffElecMCBL->turnOn(xx);
}

Double_t myFuncTurnOnEleAllBL(Double_t* x, Double_t *par) { //modified-53x

  gSystem->Load("ratioEfficiencyTest_C.so"); 
  ratioEfficiencyTest* fitEffEle20BL = new ratioEfficiencyTest(20.97643939, 1.15196354, 2.27544602, 1.01743868, 2.04391816);
  ratioEfficiencyTest* fitEffEle22RunBBL = new ratioEfficiencyTest(22.90752344, 1.32376429, 2.17813319,1.03674051, 2.15454768);
  ratioEfficiencyTest* fitEffEle22RunCBL = new ratioEfficiencyTest(23.05556088, 0.96047151, 1.24782044,1.26042277,1.09675041);
 
  float weightElec20 = 0.067; //change
  float weightElec22RunB = 0.367; //change
  float weightElec22RunC = 0.566; //change

  float total = weightElec20+weightElec22RunB+weightElec22RunC;
  weightElec20/=total;
  weightElec22RunB/=total;
  weightElec22RunC/=total;

 
  Float_t xx = x[0];

  return (fitEffEle20BL->turnOn(xx) * weightElec20 +
	  fitEffEle22RunBBL->turnOn(xx) * weightElec22RunB +
	  fitEffEle22RunCBL->turnOn(xx) * weightElec22RunC 
	  );
 
}



Double_t myFuncRatioEleAllEC(Double_t* x, Double_t *par) { //modified-53x

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffEle20EC = new ratioEfficiencyTest(20.59874300, 1.25425435, 1.61098921, 1.00146962, 60.35067579);
  ratioEfficiencyTest* fitEffEle22RunBEC = new ratioEfficiencyTest(22.14553261, 1.19913124, 1.75642067, 1.00826962, 9.04331617);
  ratioEfficiencyTest* fitEffEle22RunCEC = new ratioEfficiencyTest(21.99911375, 1.15806380, 0.80675262, 1.98765770, 0.97138507);

  ratioEfficiencyTest* fitEffElecMCEC = new ratioEfficiencyTest(22.18226941, 1.07762306, 1.23712775, 1.27324238, 1.15312185);

  float weightElec20 = 0.067; //change
  float weightElec22RunB = 0.367; //change
  float weightElec22RunC = 0.566; //change

  float total = weightElec20+weightElec22RunB+weightElec22RunC;
  weightElec20/=total;
  weightElec22RunB/=total;
  weightElec22RunC/=total;

  Float_t xx = x[0];

  return (fitEffEle20EC->turnOn(xx) * weightElec20 +
	  fitEffEle22RunBEC->turnOn(xx) * weightElec22RunB +
	  fitEffEle22RunCEC->turnOn(xx) * weightElec22RunC
	  )/fitEffElecMCEC->turnOn(xx);
}

Double_t myFuncTurnOnEleAllEC(Double_t* x, Double_t *par) { //modified-53x

  gSystem->Load("ratioEfficiencyTest_C.so"); 
  ratioEfficiencyTest* fitEffEle20EC = new ratioEfficiencyTest(20.59874300, 1.25425435, 1.61098921, 1.00146962, 60.35067579); 
  ratioEfficiencyTest* fitEffEle22RunBEC = new ratioEfficiencyTest(22.14553261, 1.19913124, 1.75642067, 1.00826962, 9.04331617); 
  ratioEfficiencyTest* fitEffEle22RunCEC = new ratioEfficiencyTest(21.99911375, 1.15806380, 0.80675262, 1.98765770, 0.97138507);

  float weightElec20 = 0.067; //change
  float weightElec22RunB = 0.367; //change
  float weightElec22RunC = 0.566; //change

  float total = weightElec20+weightElec22RunB+weightElec22RunC;
  weightElec20/=total;
  weightElec22RunB/=total;
  weightElec22RunC/=total;
  
  Float_t xx = x[0];

  return (fitEffEle20EC->turnOn(xx) * weightElec20 +
	  fitEffEle22RunBEC->turnOn(xx) * weightElec22RunB +
	  fitEffEle22RunCEC->turnOn(xx) * weightElec22RunC
	  );
 
}


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
Double_t myFuncRatioElecIDBL(Double_t* x, Double_t *par) {  //modified
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
Double_t myFuncTurnOnElecIDBL(Double_t* x, Double_t *par) {  //modified
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
Double_t myFuncRatioElecIDEC(Double_t* x, Double_t *par) { //modified
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
Double_t myFuncTurnOnElecIDEC(Double_t* x, Double_t *par) { //modified
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

Double_t myFuncRatioElecIsoBL(Double_t* x, Double_t *par) { //modified-53x
  
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

Double_t myFuncTurnOnElecIsoBL(Double_t* x, Double_t *par) { //modified-53x

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

Double_t myFuncRatioElecIsoEC(Double_t* x, Double_t *par) { //modified-53x

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

Double_t myFuncTurnOnElecIsoEC(Double_t* x, Double_t *par) {  //modified-53x
 
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
///eta < 0.8
Double_t myFuncRatioMuIDBL1(Double_t* x, Double_t *par) { //modified-53x 
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

Double_t myFuncTurnOnMuIDBL1(Double_t* x, Double_t *par) { //modified-53x 
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
Double_t myFuncRatioMuIDBL2(Double_t* x, Double_t *par) { //modified-53x
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
Double_t myFuncTurnOnMuIDBL2(Double_t* x, Double_t *par) { //modified-53x
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
Double_t myFuncRatioMuIDEC(Double_t* x, Double_t *par) {  //modified-53x
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
Double_t myFuncTurnOnMuIDEC(Double_t* x, Double_t *par) {  //modified-53x
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
///eta < 0.8
Double_t myFuncRatioMuIsoBL1(Double_t* x, Double_t *par) { //modified-53x 
   
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

Double_t myFuncTurnOnMuIsoBL1(Double_t* x, Double_t *par) {  //modified-53x 
      
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
Double_t myFuncRatioMuIsoBL2(Double_t* x, Double_t *par) { //modified-53x
  
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

Double_t myFuncTurnOnMuIsoBL2(Double_t* x, Double_t *par) {  //modified-53x

     
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
Double_t myFuncRatioMuIsoEC(Double_t* x, Double_t *par) {  //modified-53x

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

Double_t myFuncTurnOnMuIsoEC(Double_t* x, Double_t *par) { //modified-53x
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
//Run 2012A (MC changed to 53X)

Double_t myFuncRatioMu18BL(Double_t* x, Double_t *par) { //modified-53x
  gSystem->Load("ratioEfficiencyTest_C.so"); 
  ratioEfficiencyTest* fitEffMuBL = new ratioEfficiencyTest(15.99983195, -0.39072829, 0.28256338, 1.72861719, 0.95769408); 
  ratioEfficiencyTest* fitEffMuBLMC = new ratioEfficiencyTest(16.00073094, 0.00779095, 0.00029834, 2.13782323, 0.95571348);
 
  Float_t xx = x[0]; 
  return fitEffMuBL->turnOn(xx)/fitEffMuBLMC->turnOn(xx); 

}

Double_t myFuncTurnOnMu18BL(Double_t* x, Double_t *par) { //modified
  gSystem->Load("ratioEfficiencyTest_C.so");  
  ratioEfficiencyTest* fitEffMuBL = new ratioEfficiencyTest(15.99983195, -0.39072829, 0.28256338, 1.72861719, 0.95769408);
  Float_t xx = x[0];  
  return fitEffMuBL->turnOn(xx);
}

Double_t myFuncRatioMu18EC(Double_t* x, Double_t *par) { //modified-
  gSystem->Load("ratioEfficiencyTest_C.so");  
  ratioEfficiencyTest* fitEffMuEC = new ratioEfficiencyTest(18.49754887, -0.16941614, 0.26076717, 1.05494469, 1.53819978);
  ratioEfficiencyTest* fitEffMuECMC = new ratioEfficiencyTest(17.03319591, 0.73033173, 1.02903291, 1.46732719, 0.89420534);
  
  Float_t xx = x[0];  
  return fitEffMuEC->turnOn(xx)/fitEffMuECMC->turnOn(xx);
  
}

Double_t myFuncTurnOnMu18EC(Double_t* x, Double_t *par) { //modified
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffMuEC = new ratioEfficiencyTest(18.49754887, -0.16941614, 0.26076717, 1.05494469, 1.53819978); 

  Float_t xx = x[0];   
  return fitEffMuEC->turnOn(xx);
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
//Run2012B

Double_t myFuncRatioMu17RunBBL(Double_t* x, Double_t *par) { //modified-53x
  gSystem->Load("ratioEfficiencyTest_C.so");  
  ratioEfficiencyTest* fitEffMu17BL = new ratioEfficiencyTest(17.21270264, 0.54997112, 1.02874912, 1.29646487, 0.96724273);
  ratioEfficiencyTest* fitEffMuBLMC = new ratioEfficiencyTest(16.00073094, 0.00779095, 0.00029834, 2.13782323, 0.95571348);
  
  Float_t xx = x[0];  
  return fitEffMu17BL->turnOn(xx)/fitEffMuBLMC->turnOn(xx);  
}

Double_t myFuncTurnOnMu17RunBBL(Double_t* x, Double_t *par) {  //modified-53x

  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffMu17BL = new ratioEfficiencyTest(17.21270264, 0.54997112, 1.02874912, 1.29646487, 0.96724273);

  Float_t xx = x[0];
  return fitEffMu17BL->turnOn(xx);
}

Double_t myFuncRatioMu17RunBEC(Double_t* x, Double_t *par) { //modified-53x
  gSystem->Load("ratioEfficiencyTest_C.so");   
  ratioEfficiencyTest* fitEffMu17EC = new ratioEfficiencyTest(15.98037640, 0.12062946, 0.02183977, 2.84751010, 0.83985656);
  ratioEfficiencyTest* fitEffMuECMC = new ratioEfficiencyTest(17.03319591, 0.73033173, 1.02903291, 1.46732719, 0.89420534);
   
  Float_t xx = x[0];   
  return fitEffMu17EC->turnOn(xx)/fitEffMuECMC->turnOn(xx);   
   
}

Double_t myFuncTurnOnMu17RunBEC(Double_t* x, Double_t *par) { //modified-53x 

  gSystem->Load("ratioEfficiencyTest_C.so");    
  ratioEfficiencyTest* fitEffMu17EC = new ratioEfficiencyTest(15.98037640, 0.12062946, 0.02183977, 2.84751010, 0.83985656); 
  Float_t xx = x[0];    
  return fitEffMu17EC->turnOn(xx);
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
//Run2012C (from Valentina)

Double_t myFuncRatioMu17RunCBL(Double_t* x, Double_t *par) { //modified-53x
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffMu17BL = new ratioEfficiencyTest(16.00061526, 0.00737246, 0.00029014, 2.12854792, 0.93371791);
  ratioEfficiencyTest* fitEffMuBLMC = new ratioEfficiencyTest(16.00073094, 0.00779095, 0.00029834, 2.13782323, 0.95571348);
  
  Float_t xx = x[0];
  return fitEffMu17BL->turnOn(xx)/fitEffMuBLMC->turnOn(xx);
}

Double_t myFuncTurnOnMu17RunCBL(Double_t* x, Double_t *par) {  //modified-53x
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffMu17BL = new ratioEfficiencyTest(16.00061526, 0.00737246, 0.00029014, 2.12854792, 0.93371791);
  
  Float_t xx = x[0];
  return fitEffMu17BL->turnOn(xx);
}

Double_t myFuncRatioMu17RunCEC(Double_t* x, Double_t *par) { //modified-53x
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffMu17EC = new ratioEfficiencyTest(16.65093710, 0.48774518, 0.56076820, 1.73768135, 0.86107187);
  ratioEfficiencyTest* fitEffMuECMC = new ratioEfficiencyTest(17.03319591, 0.73033173, 1.02903291, 1.46732719, 0.89420534);
  
  Float_t xx = x[0];
  return fitEffMu17EC->turnOn(xx)/fitEffMuECMC->turnOn(xx);
  
}

Double_t myFuncTurnOnMu17RunCEC(Double_t* x, Double_t *par) { //modified-53x
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* fitEffMu17EC = new ratioEfficiencyTest(16.65093710, 0.48774518, 0.56076820, 1.73768135, 0.86107187);
  Float_t xx = x[0];
  return fitEffMu17EC->turnOn(xx);
}

///////////////////////////////////
Double_t myFuncRatioMuAllBL(Double_t* x, Double_t *par) { //modified-53x

  TF1* ratioMu18BL = new TF1("ratioMu18BL", myFuncRatioMu18BL,0,400,0);
  TF1* ratioMu17RunBBL = new TF1("ratioMu17RunBBL", myFuncRatioMu17RunBBL,0,400,0);
  TF1* ratioMu17RunCBL = new TF1("ratioMu17RunCBL", myFuncRatioMu17RunCBL,0,400,0);

  float weightIsoMu18 = 0.067;  //change
  float weightIsoMu17RunB = 0.367;  //change
  float weightIsoMu17RunC = 0.566;  //change

  float total = weightIsoMu18+weightIsoMu17RunB+weightIsoMu17RunC;
  weightIsoMu18 /= total;
  weightIsoMu17RunB /= total;
  weightIsoMu17RunC /= total;

  Float_t xx = x[0];

  return (ratioMu18BL->Eval(xx) * weightIsoMu18 +
	  ratioMu17RunBBL->Eval(xx) * weightIsoMu17RunB +
	  ratioMu17RunCBL->Eval(xx) * weightIsoMu17RunC );

}

Double_t myFuncTurnOnMuAllBL(Double_t* x, Double_t *par) { //modified-53x

  TF1* turnOnMu18BL = new TF1("turnOnMu18BL", myFuncTurnOnMu18BL,0,400,0);
  TF1* turnOnMu17RunBBL = new TF1("turnOnMu17RunBBL", myFuncTurnOnMu17RunBBL,0,400,0);
  TF1* turnOnMu17RunCBL = new TF1("turnOnMu17RunCBL", myFuncTurnOnMu17RunCBL,0,400,0);

  float weightIsoMu18 = 0.067;  //change
  float weightIsoMu17RunB = 0.367;  //change
  float weightIsoMu17RunC = 0.566;  //change

  float total = weightIsoMu18+weightIsoMu17RunB+weightIsoMu17RunC;
  weightIsoMu18 /= total;
  weightIsoMu17RunB /= total;
  weightIsoMu17RunC /= total;

  Float_t xx = x[0];
  
  return (turnOnMu18BL->Eval(xx) * weightIsoMu18 +
	  turnOnMu17RunBBL->Eval(xx) * weightIsoMu17RunB +
	  turnOnMu17RunCBL->Eval(xx) * weightIsoMu17RunC);
}


Double_t myFuncRatioMuAllEC(Double_t* x, Double_t *par) { //modified-53x

  TF1* ratioMu18EC = new TF1("ratioMu18EC", myFuncRatioMu18EC,0,400,0);
  TF1* ratioMu17RunBEC = new TF1("ratioMu17RunBEC", myFuncRatioMu17RunBEC,0,400,0);
  TF1* ratioMu17RunCEC = new TF1("ratioMu17RunCEC", myFuncRatioMu17RunCEC,0,400,0);
  
  float weightIsoMu18 = 0.067;  //change
  float weightIsoMu17RunB = 0.367;  //change
  float weightIsoMu17RunC = 0.566;  //change

  float total = weightIsoMu18+weightIsoMu17RunB+weightIsoMu17RunC;
  weightIsoMu18 /= total;
  weightIsoMu17RunB /= total;
  weightIsoMu17RunC /= total;

  Float_t xx = x[0];

  return (ratioMu18EC->Eval(xx) * weightIsoMu18 +
	  ratioMu17RunBEC->Eval(xx) * weightIsoMu17RunB +
	  ratioMu17RunCEC->Eval(xx) * weightIsoMu17RunC );

}

Double_t myFuncTurnOnMuAllEC(Double_t* x, Double_t *par) {  //modified-53x

  TF1* turnOnMu18EC = new TF1("turnOnMu18EC", myFuncTurnOnMu18EC,0,400,0);
  TF1* turnOnMu17RunBEC = new TF1("turnOnMu17RunBEC", myFuncTurnOnMu17RunBEC,0,400,0);
  TF1* turnOnMu17RunCEC = new TF1("turnOnMu17RunCEC", myFuncTurnOnMu17RunCEC,0,400,0);

  float weightIsoMu18 = 0.067;  //change
  float weightIsoMu17RunB = 0.367;  //change
  float weightIsoMu17RunC = 0.566;  //change

  float total = weightIsoMu18+weightIsoMu17RunB+weightIsoMu17RunC;
  weightIsoMu18 /= total;
  weightIsoMu17RunB /= total;
  weightIsoMu17RunC /= total;
  
  Float_t xx = x[0];

  return (turnOnMu18EC->Eval(xx) * weightIsoMu18 +
	  turnOnMu17RunBEC->Eval(xx) * weightIsoMu17RunB +
	  turnOnMu17RunCEC->Eval(xx) * weightIsoMu17RunC );
}






/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////

// HLT PFTauLoose20 / MC Summer12 53X / mu+tau

Double_t myFuncTurnOnTauLoose20MuTauMCBL(Double_t* x, Double_t *par) {  //modified-53x
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose20MC = new ratioEfficiencyTest(18.80484409, 0.19082817, 0.19983010, 1.81979820, 0.93270649);
  
  Float_t xx = x[0];

  return ratioEffTauLoose20MC->turnOn(xx);

}

Double_t myFuncTurnOnTauLoose20MuTauMCEC(Double_t* x, Double_t *par) {  //modified-53x
   
  gSystem->Load("ratioEfficiencyTest_C.so"); 
  ratioEfficiencyTest* ratioEffTauLoose20MC = new ratioEfficiencyTest(18.25975478, 1.32745225, 1.70380810, 149.18410074, 0.87377770);
  
  Float_t xx = x[0]; 
  
  return ratioEffTauLoose20MC->turnOn(xx); 
 
} 

/////////////////////////////////////////////////


/////////////////////////////////////////////////
/////////////////////////////////////////////////

// HLT PFTauLoose20 / Run2012A+B+C     / mu+tau

Double_t myFuncRatioTauMuTauAllBL(Double_t* x, Double_t *par) { //modified-53x
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauMuTauAllBL = new ratioEfficiencyTest(18.50940288, 1.62285299, 2.73232995, 1.79135412, 0.91481432);
  ratioEfficiencyTest* ratioEffTauLoose20MuTauMCBL = new ratioEfficiencyTest(18.80484409, 0.19082817, 0.19983010, 1.81979820, 0.93270649);

  Float_t xx = x[0];

  return ratioEffTauMuTauAllBL->turnOn(xx)/ratioEffTauLoose20MuTauMCBL->turnOn(xx);

}

Double_t myFuncTurnOnTauMuTauAllBL(Double_t* x, Double_t *par) { //modified-53x
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauMuTauAllBL = new ratioEfficiencyTest(18.50940288, 1.62285299, 2.73232995, 1.79135412, 0.91481432);
 
  Float_t xx = x[0];

  return ratioEffTauMuTauAllBL->turnOn(xx);

}

Double_t myFuncRatioTauMuTauAllEC(Double_t* x, Double_t *par) { //modified-53x 
   
  gSystem->Load("ratioEfficiencyTest_C.so"); 
  ratioEfficiencyTest* ratioEffTauMuTauAllEC = new ratioEfficiencyTest(18.45678784, 0.68697618, 0.57008697, 3.73470825, 0.84747211);
  ratioEfficiencyTest* ratioEffTauLoose20MuTauMCEC = new ratioEfficiencyTest(18.25975478, 1.32745225, 1.70380810, 149.18410074, 0.87377770);
 
  Float_t xx = x[0]; 
 
  return ratioEffTauMuTauAllEC->turnOn(xx)/ratioEffTauLoose20MuTauMCEC->turnOn(xx); 
 
} 

Double_t myFuncTurnOnTauMuTauAllEC(Double_t* x, Double_t *par) { //modified-53x

  gSystem->Load("ratioEfficiencyTest_C.so");  
  ratioEfficiencyTest* ratioEffTauMuTauAllEC = new ratioEfficiencyTest(18.45678784, 0.68697618, 0.57008697, 3.73470825, 0.84747211);

  Float_t xx = x[0];  
  
  return ratioEffTauMuTauAllEC->turnOn(xx);
}

/////////////////////////////////////////////////////////
// HLT PFTauLoose20 / MC Summer12 53X / elec+tau
//new curves from Joshua
Double_t myFuncTurnOnTauElecTauMCBL(Double_t* x, Double_t *par) { //modified-53x
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  ratioEfficiencyTest* ratioEffTauLoose20MC = new ratioEfficiencyTest(18.40815138, 1.53235636, 3.55989632, 1.74542709, 0.90118450);
  
  Float_t xx = x[0];

  return ratioEffTauLoose20MC->turnOn(xx);

}

Double_t myFuncTurnOnTauElecTauMCEC(Double_t* x, Double_t *par) { //modified-53x 
   
  gSystem->Load("ratioEfficiencyTest_C.so"); 
  ratioEfficiencyTest* ratioEffTauLoose20MC = new ratioEfficiencyTest(18.29028052, 1.56239255, 11.03605631, 155.89290151, 0.85683995);
   
  Float_t xx = x[0]; 
 
  return ratioEffTauLoose20MC->turnOn(xx); 
 
} 

/////////////////////////////////////////////////
// HLT PFTau All / e+tau
//new curves from Joshua
Double_t myFuncRatioTauElecTauAllBL(Double_t* x, Double_t *par) {  //modified-53x
  
  gSystem->Load("ratioEfficiencyTest_C.so");
 
  ratioEfficiencyTest* ratioEffTauLoose20ElecTau = new ratioEfficiencyTest(18.43442868, 2.08967536, 3.27357845, 6.96327309, 0.85564484);
  ratioEfficiencyTest* ratioEffTauLoose20MC = new ratioEfficiencyTest(18.40815138, 1.53235636, 3.55989632, 1.74542709, 0.90118450);
  
  Float_t xx = x[0];

  float ratio = ratioEffTauLoose20MC->turnOn(xx) > 0 ? 
    ratioEffTauLoose20ElecTau->turnOn(xx)/ratioEffTauLoose20MC->turnOn(xx) : 0.;
  
  return ratio;

}

Double_t myFuncTurnOnTauElecTauAllBL(Double_t* x, Double_t *par) { //modified
  
  gSystem->Load("ratioEfficiencyTest_C.so");
  
  ratioEfficiencyTest* ratioEffTauLoose20ElecTau = new ratioEfficiencyTest(18.43442868, 2.08967536, 3.27357845, 6.96327309, 0.85564484);
   
  Float_t xx = x[0]; 
  
  return ratioEffTauLoose20ElecTau->turnOn(xx);

}

Double_t myFuncRatioTauElecTauAllEC(Double_t* x, Double_t *par) {  //modified-53x 
   
  gSystem->Load("ratioEfficiencyTest_C.so"); 
  
  ratioEfficiencyTest* ratioEffTauLoose20ElecTau = new ratioEfficiencyTest(18.16839440, 1.86184564, 4.39116712, 1.01410741, 1.39240481);
  ratioEfficiencyTest* ratioEffTauLoose20MC = new ratioEfficiencyTest(18.29028052, 1.56239255, 11.03605631, 155.89290151, 0.85683995);
   
  Float_t xx = x[0]; 
 
  float ratio = ratioEffTauLoose20MC->turnOn(xx) > 0 ?  
    ratioEffTauLoose20ElecTau->turnOn(xx)/ratioEffTauLoose20MC->turnOn(xx) : 0.; 
   
  return ratio; 
  
} 

Double_t myFuncTurnOnTauElecTauAllEC(Double_t* x, Double_t *par) { //modified 
  
  gSystem->Load("ratioEfficiencyTest_C.so"); 
  ratioEfficiencyTest* ratioEffTauLoose20ElecTau = new ratioEfficiencyTest(18.16839440, 1.86184564, 4.39116712, 1.01410741, 1.39240481);
  
  Float_t xx = x[0];  
  
  return ratioEffTauLoose20ElecTau->turnOn(xx); 
  
} 

/////////////////////////////////////////////////

void makeFile(){

  TFile* fout = new TFile("llrCorrections.root","RECREATE");

  TF1 *ratioElec20BL        = new TF1("ratioElec20BL",           myFuncRatioElec20BL,       24,800,0);
  TF1 *turnOnElec20BL       = new TF1("turnOnElec20BL",          myFuncTurnOnElec20BL,      24,800,0);
  TF1 *ratioElec20EC        = new TF1("ratioElec20EC",           myFuncRatioElec20EC,       24,800,0);
  TF1 *turnOnElec20EC       = new TF1("turnOnElec20EC",          myFuncTurnOnElec20EC,      24,800,0);

  TF1 *ratioElec22RunBBL        = new TF1("ratioElec22RunBBL",           myFuncRatioEle22RunBBL,        24,800,0);
  TF1 *turnOnElec22RunBBL       = new TF1("turnOnElec22RunBBL",          myFuncTurnOnEle22RunBBL,       24,800,0);
  TF1 *ratioElec22RunBEC        = new TF1("ratioElec22RunBEC",           myFuncRatioEle22RunBEC,        24,800,0);
  TF1 *turnOnElec22RunBEC       = new TF1("turnOnElec22RunBEC",          myFuncTurnOnEle22RunBEC,       24,800,0);

  TF1 *ratioElec22RunCBL        = new TF1("ratioElec22RunCBL",           myFuncRatioEle22RunCBL,        24,800,0);
  TF1 *turnOnElec22RunCBL       = new TF1("turnOnElec22RunCBL",          myFuncTurnOnEle22RunCBL,       24,800,0);
  TF1 *ratioElec22RunCEC        = new TF1("ratioElec22RunCEC",           myFuncRatioEle22RunCEC,        24,800,0);
  TF1 *turnOnElec22RunCEC       = new TF1("turnOnElec22RunCEC",          myFuncTurnOnEle22RunCEC,       24,800,0);

  TF1 *ratioElecAllBL       = new TF1("ratioElecAllBL",          myFuncRatioEleAllBL,       24,800,0);
  TF1 *turnOnElecAllBL      = new TF1("turnOnElecAllBL",         myFuncTurnOnEleAllBL,      24,800,0);
  TF1 *ratioElecAllEC       = new TF1("ratioElecAllEC",          myFuncRatioEleAllEC,       24,800,0);
  TF1 *turnOnElecAllEC      = new TF1("turnOnElecAllEC",         myFuncTurnOnEleAllEC,      24,800,0);

  TF1 *ratioElecIDBL        = new TF1("ratioElecIDBL",           myFuncRatioElecIDBL ,      14,800,0);
  TF1 *turnOnElecIDBL       = new TF1("turnOnElecIDBL",          myFuncTurnOnElecIDBL ,     14,800,0);
  TF1 *ratioElecIDEC        = new TF1("ratioElecIDEC",           myFuncRatioElecIDEC ,      14,800,0);
  TF1 *turnOnElecIDEC       = new TF1("turnOnElecIDEC",          myFuncTurnOnElecIDEC ,     14,800,0);

  TF1 *ratioElecIsoBL       = new TF1("ratioElecIsoBL",          myFuncRatioElecIsoBL ,     14,800,0);
  TF1 *turnOnElecIsoBL      = new TF1("turnOnElecIsoBL",         myFuncTurnOnElecIsoBL ,    14,800,0);
  TF1 *ratioElecIsoEC       = new TF1("ratioElecIsoEC",          myFuncRatioElecIsoEC ,     14,800,0);
  TF1 *turnOnElecIsoEC      = new TF1("turnOnElecIsoEC",         myFuncTurnOnElecIsoEC ,    14,800,0);

  TF1 *ratioMu18BL      = new TF1("ratioMu18BL",         myFuncRatioMu18BL,     20,800,0);
  TF1 *turnOnMu18BL     = new TF1("turnOnMu18BL",        myFuncTurnOnMu18BL,    20,800,0);

  TF1 *ratioMu18EC      = new TF1("ratioMu18EC",         myFuncRatioMu18EC,     20,800,0);
  TF1 *turnOnMu18EC     = new TF1("turnOnMu18EC",        myFuncTurnOnMu18EC,    20,800,0);

  TF1 *ratioMu17RunBBL      = new TF1("ratioMu17RunBBL",         myFuncRatioMu17RunBBL,     20,800,0);
  TF1 *turnOnMu17RunBBL     = new TF1("turnOnMu17RunBBL",        myFuncTurnOnMu17RunBBL,    20,800,0);

  TF1 *ratioMu17RunBEC      = new TF1("ratioMu17RunBEC",         myFuncRatioMu17RunBEC,     20,800,0);
  TF1 *turnOnMu17RunBEC     = new TF1("turnOnMu17RunBEC",        myFuncTurnOnMu17RunBEC,    20,800,0);
 
  TF1 *ratioMu17RunCBL      = new TF1("ratioMu17RunCBL",         myFuncRatioMu17RunCBL,     20,800,0);
  TF1 *turnOnMu17RunCBL     = new TF1("turnOnMu17RunCBL",        myFuncTurnOnMu17RunCBL,    20,800,0);

  TF1 *ratioMu17RunCEC      = new TF1("ratioMu17RunCEC",         myFuncRatioMu17RunCEC,     20,800,0);
  TF1 *turnOnMu17RunCEC     = new TF1("turnOnMu17RunCEC",        myFuncTurnOnMu17RunCEC,    20,800,0);

  TF1 *ratioMuAllBL         = new TF1("ratioMuAllBL",            myFuncRatioMuAllBL,        20,800,0);
  TF1 *turnOnMuAllBL        = new TF1("turnOnMuAllBL",           myFuncTurnOnMuAllBL,       20,800,0);

  TF1 *ratioMuAllEC         = new TF1("ratioMuAllEC",            myFuncRatioMuAllEC,        20,800,0);
  TF1 *turnOnMuAllEC        = new TF1("turnOnMuAllEC",           myFuncTurnOnMuAllEC,       20,800,0);
 
  TF1 *ratioMuIDBL1         = new TF1("ratioMuIDBL1",             myFuncRatioMuIDBL1,        14,800,0);
  TF1 *turnOnMuIDBL1        = new TF1("turnOnMuIDBL1",            myFuncTurnOnMuIDBL1,       14,800,0);

  TF1 *ratioMuIDBL2         = new TF1("ratioMuIDBL2",             myFuncRatioMuIDBL2,        14,800,0); 
  TF1 *turnOnMuIDBL2        = new TF1("turnOnMuIDBL2",            myFuncTurnOnMuIDBL2,       14,800,0); 

  TF1 *ratioMuIDEC          = new TF1("ratioMuIDEC",             myFuncRatioMuIDEC ,        14,800,0);
  TF1 *turnOnMuIDEC         = new TF1("turnOnMuIDEC",            myFuncTurnOnMuIDEC ,       14,800,0);

  TF1 *ratioMuIsoBL1        = new TF1("ratioMuIsoBL1",            myFuncRatioMuIsoBL1,       14,800,0);
  TF1 *turnOnMuIsoBL1       = new TF1("turnOnMuIsoBL1",           myFuncTurnOnMuIsoBL1,      14,800,0);

  TF1 *ratioMuIsoBL2        = new TF1("ratioMuIsoBL2",            myFuncRatioMuIsoBL2,       14,800,0); 
  TF1 *turnOnMuIsoBL2       = new TF1("turnOnMuIsoBL2",           myFuncTurnOnMuIsoBL2,      14,800,0);

  TF1 *ratioMuIsoEC         = new TF1("ratioMuIsoEC",            myFuncRatioMuIsoEC ,       14,800,0);
  TF1 *turnOnMuIsoEC        = new TF1("turnOnMuIsoEC",           myFuncTurnOnMuIsoEC ,      14,800,0);

  TF1 *turnOnTauLoose20MuTauMCBL      = new TF1("turnOnTauLoose20MuTauMCBL",   myFuncTurnOnTauLoose20MuTauMCBL,      20,800,0);
  TF1 *turnOnTauLoose20MuTauMCEC      = new TF1("turnOnTauLoose20MuTauMCEC",   myFuncTurnOnTauLoose20MuTauMCEC,	     20,800,0); 

  //TF1 *ratioTauLoose20MuTauRunABL    = new TF1("ratioTauLoose20MuTauRunABL",   myFuncRatioTauLoose20MuTauRunABL,     20,800,0);
  //TF1 *turnOnTauLoose20MuTauRunABL   = new TF1("turnOnTauLoose20MuTauRunABL",  myFuncTurnOnTauLoose20MuTauRunABL,    20,800,0);
  //TF1 *ratioTauLoose20MuTauRunAEC    = new TF1("ratioTauLoose20MuTauRunAEC",   myFuncRatioTauLoose20MuTauRunAEC,     20,800,0); 
  //TF1 *turnOnTauLoose20MuTauRunAEC   = new TF1("turnOnTauLoose20MuTauRunAEC",  myFuncTurnOnTauLoose20MuTauRunAEC,    20,800,0);

  TF1 *turnOnTauElecTauMCBL      = new TF1("turnOnTauElecTauMCBL",   myFuncTurnOnTauElecTauMCBL, 20,800,0);
  TF1 *turnOnTauElecTauMCEC      = new TF1("turnOnTauElecTauMCEC",   myFuncTurnOnTauElecTauMCEC, 20,800,0);

  //TF1 *ratioTauLoose20ElecTauRunA    = new TF1("ratioTauLoose20ElecTauRunA",   myFuncRatioTauLoose20ElecTauRunA ,  20,800,0);
  //TF1 *turnOnTauLoose20ElecTauRunA   = new TF1("turnOnTauLoose20ElecTauRunA",  myFuncTurnOnTauLoose20ElecTauRunA,  20,800,0);
  
  //TF1 *ratioTauLoose20MuTauRunBBL    = new TF1("ratioTauLoose20MuTauRunBBL",   myFuncRatioTauLoose20MuTauRunBBL,     20,800,0);
  //TF1 *turnOnTauLoose20MuTauRunBBL   = new TF1("turnOnTauLoose20MuTauRunBBL",  myFuncTurnOnTauLoose20MuTauRunBBL,    20,800,0);
  //TF1 *ratioTauLoose20MuTauRunBEC    = new TF1("ratioTauLoose20MuTauRunBEC",   myFuncRatioTauLoose20MuTauRunBEC,     20,800,0); 
  //TF1 *turnOnTauLoose20MuTauRunBEC   = new TF1("turnOnTauLoose20MuTauRunBEC",  myFuncTurnOnTauLoose20MuTauRunBEC,    20,800,0);

  //TF1 *ratioTauLoose20ElecTauRunB    = new TF1("ratioTauLoose20ElecTauRunB",   myFuncRatioTauLoose20ElecTauRunB ,  20,800,0);
  //TF1 *turnOnTauLoose20ElecTauRunB   = new TF1("turnOnTauLoose20ElecTauRunB",  myFuncTurnOnTauLoose20ElecTauRunB,  20,800,0);

 
  TF1 *ratioTauElecTauAllBL    = new TF1("ratioTauElecTauAllBL", myFuncRatioTauElecTauAllBL, 20,800,0);
  TF1 *turnOnTauElecTauAllBL   = new TF1("turnOnTauElecTauAllBL", myFuncTurnOnTauElecTauAllBL, 20,800,0);
  TF1 *ratioTauElecTauAllEC    = new TF1("ratioTauElecTauAllEC", myFuncRatioTauElecTauAllEC, 20,800,0); 
  TF1 *turnOnTauElecTauAllEC   = new TF1("turnOnTauElecTauAllEC", myFuncTurnOnTauElecTauAllEC, 20,800,0);

  TF1 *ratioTauMuTauAllBL      = new TF1("ratioTauMuTauAllBL", myFuncRatioTauMuTauAllBL ,20,800,0);
  TF1 *turnOnTauMuTauAllBL     = new TF1("turnOnTauMuTauAllBL",myFuncTurnOnTauMuTauAllBL,20,800,0);
  TF1 *ratioTauMuTauAllEC      = new TF1("ratioTauMuTauAllEC", myFuncRatioTauMuTauAllEC ,20,800,0); 
  TF1 *turnOnTauMuTauAllEC     = new TF1("turnOnTauMuTauAllEC",myFuncTurnOnTauMuTauAllEC,20,800,0);
  

  fout->cd();
  
  

  ratioElecIsoBL->SetNpx(25600);
  turnOnElecIsoBL->SetNpx(25600);
  ratioElecIsoEC->SetNpx(25600);
  turnOnElecIsoEC->SetNpx(25600);

  ratioMuIsoBL1->SetNpx(25600);
  ratioMuIsoBL2->SetNpx(25600);
  ratioMuIsoEC->SetNpx(25600);
  turnOnMuIsoBL1->SetNpx(25600);
  turnOnMuIsoBL2->SetNpx(25600);
  turnOnMuIsoEC->SetNpx(25600);
  
  ratioElec20BL->SetNpx(25600);
  turnOnElec20BL->SetNpx(25600);
  ratioElec20EC->SetNpx(25600);
  turnOnElec20EC->SetNpx(25600);
  
  ratioElec22RunBBL->SetNpx(25600); 
  turnOnElec22RunBBL->SetNpx(25600); 
  ratioElec22RunBEC->SetNpx(25600); 
  turnOnElec22RunBEC->SetNpx(25600);

  ratioElec22RunCBL->SetNpx(25600);
  turnOnElec22RunCBL->SetNpx(25600);
  ratioElec22RunCEC->SetNpx(25600);
  turnOnElec22RunCEC->SetNpx(25600);

  ratioElecAllBL->SetNpx(25600);
  turnOnElecAllBL->SetNpx(25600);
  ratioElecAllEC->SetNpx(25600);
  turnOnElecAllEC->SetNpx(25600);

  
  ratioMu17RunBBL->SetNpx(25600);
  turnOnMu17RunBBL->SetNpx(25600);
  ratioMu17RunBEC->SetNpx(25600);
  turnOnMu17RunBEC->SetNpx(25600);

  ratioMu17RunCBL->SetNpx(25600);
  turnOnMu17RunCBL->SetNpx(25600);
  ratioMu17RunCEC->SetNpx(25600);
  turnOnMu17RunCEC->SetNpx(25600);

  ratioMu18BL->SetNpx(25600); 
  turnOnMu18BL->SetNpx(25600); 
  ratioMu18EC->SetNpx(25600); 
  turnOnMu18EC->SetNpx(25600);

  ratioMuAllBL->SetNpx(25600);
  turnOnMuAllBL->SetNpx(25600);
  ratioMuAllEC->SetNpx(25600);
  turnOnMuAllEC->SetNpx(25600);


  ratioTauElecTauAllBL->SetNpx(25600);
  turnOnTauElecTauAllBL->SetNpx(25600);
  ratioTauElecTauAllEC->SetNpx(25600); 
  turnOnTauElecTauAllEC->SetNpx(25600);

  ratioTauMuTauAllBL->SetNpx(25600);
  turnOnTauMuTauAllBL->SetNpx(25600);
  ratioTauMuTauAllEC->SetNpx(25600); 
  turnOnTauMuTauAllEC->SetNpx(25600);

  turnOnTauLoose20MuTauMCBL->SetNpx(25600);
  turnOnTauLoose20MuTauMCEC->SetNpx(25600);
  //ratioTauLoose20MuTauRunABL->SetNpx(25600);
  //turnOnTauLoose20MuTauRunABL->SetNpx(25600);
  //ratioTauLoose20MuTauRunAEC->SetNpx(25600);
  //turnOnTauLoose20MuTauRunAEC->SetNpx(25600);
  turnOnTauElecTauMCBL->SetNpx(25600);
  turnOnTauElecTauMCEC->SetNpx(25600);
  //ratioTauLoose20ElecTauRunA->SetNpx(25600);
  //turnOnTauLoose20ElecTauRunA->SetNpx(25600);
  //ratioTauLoose20MuTauRunBBL->SetNpx(25600);
  //turnOnTauLoose20MuTauRunBBL->SetNpx(25600);
  //ratioTauLoose20MuTauRunBEC->SetNpx(25600);
  //turnOnTauLoose20MuTauRunBEC->SetNpx(25600);
  //ratioTauLoose20ElecTauRunB->SetNpx(25600);
  //turnOnTauLoose20ElecTauRunB->SetNpx(25600);


  ratioElecIDBL->SetNpx(25600);
  turnOnElecIDBL->SetNpx(25600);
  ratioElecIDEC->SetNpx(25600);
  turnOnElecIDEC->SetNpx(25600);
  ratioMuIDBL1->SetNpx(25600);
  turnOnMuIDBL1->SetNpx(25600);
  ratioMuIDBL2->SetNpx(25600); 
  turnOnMuIDBL2->SetNpx(25600);
  ratioMuIDEC->SetNpx(25600);
  turnOnMuIDEC->SetNpx(25600);

  //==============================

  ratioElecIsoBL->Write();
  turnOnElecIsoBL->Write();
  ratioElecIsoEC->Write();
  turnOnElecIsoEC->Write();
  ratioMuIsoBL1->Write();
  ratioMuIsoBL2->Write(); 
  ratioMuIsoEC->Write();
  turnOnMuIsoBL1->Write();
  turnOnMuIsoBL2->Write();
  turnOnMuIsoEC->Write();
  
  ratioElec20BL->Write();
  turnOnElec20BL->Write();
  ratioElec20EC->Write();
  turnOnElec20EC->Write();
  
  ratioElec22RunBBL->Write(); 
  turnOnElec22RunBBL->Write(); 
  ratioElec22RunBEC->Write(); 
  turnOnElec22RunBEC->Write();

  ratioElec22RunCBL->Write();
  turnOnElec22RunCBL->Write();
  ratioElec22RunCEC->Write();
  turnOnElec22RunCEC->Write();

  ratioElecAllBL->Write();
  turnOnElecAllBL->Write();
  ratioElecAllEC->Write();
  turnOnElecAllEC->Write();
  
  ratioMu17RunBBL->Write();
  turnOnMu17RunBBL->Write();
  ratioMu17RunBEC->Write();
  turnOnMu17RunBEC->Write();

  ratioMu17RunCBL->Write();
  turnOnMu17RunCBL->Write();
  ratioMu17RunCEC->Write();
  turnOnMu17RunCEC->Write();

  ratioMu18BL->Write(); 
  turnOnMu18BL->Write(); 
  ratioMu18EC->Write(); 
  turnOnMu18EC->Write();

  ratioMuAllBL->Write();
  turnOnMuAllBL->Write();
  ratioMuAllEC->Write();
  turnOnMuAllEC->Write();


  ratioTauElecTauAllBL->Write();
  turnOnTauElecTauAllBL->Write();
  ratioTauElecTauAllEC->Write(); 
  turnOnTauElecTauAllEC->Write();

  ratioTauMuTauAllBL->Write();
  turnOnTauMuTauAllBL->Write();
  ratioTauMuTauAllEC->Write(); 
  turnOnTauMuTauAllEC->Write();

  turnOnTauLoose20MuTauMCBL->Write();
  turnOnTauLoose20MuTauMCEC->Write();
  //ratioTauLoose20MuTauRunABL->Write();
  //turnOnTauLoose20MuTauRunABL->Write();
  //ratioTauLoose20MuTauRunAEC->Write();
  //turnOnTauLoose20MuTauRunAEC->Write();
  turnOnTauElecTauMCBL->Write();
  turnOnTauElecTauMCEC->Write();
  //ratioTauLoose20ElecTauRunA->Write();
  //turnOnTauLoose20ElecTauRunA->Write();
  //ratioTauLoose20MuTauRunBBL->Write();
  //turnOnTauLoose20MuTauRunBBL->Write();
  //ratioTauLoose20MuTauRunBEC->Write();
  //turnOnTauLoose20MuTauRunBEC->Write();
  //ratioTauLoose20ElecTauRunB->Write();
  //turnOnTauLoose20ElecTauRunB->Write();


  ratioElecIDBL->Write();
  turnOnElecIDBL->Write();
  ratioElecIDEC->Write();
  turnOnElecIDEC->Write();

  ratioMuIDBL1->Write();
  turnOnMuIDBL1->Write();
  ratioMuIDBL2->Write(); 
  turnOnMuIDBL2->Write();
  ratioMuIDEC->Write();
  turnOnMuIDEC->Write();


  fout->Write();
  fout->Close();


}
