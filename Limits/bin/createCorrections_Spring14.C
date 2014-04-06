#include "TFile.h"
#include "TF1.h"
#include "TSystem.h"

//#include "ratioEfficiencyElec.C"
#include "ratioEfficiencyTest.C"

// Weights of differents periods
#define weightRunA  0.04185 //0.067   L=810.99
#define weightRunB  0.22924 //0.367   L=4442.288743
#define weightRunC  0.35354 //0.566   L=6851.050214
#define weightRunD  0.37537 //0       L=7274
// total L(ABC)= 12104.329
// total L(D)  = 7274
// total ABCD  = 19378.329

///////////////////
//   ELECTRONS   //
///////////////////
//
// Electron ID, ISO
//
Double_t myFuncTurnOnEleIdIso(Double_t *x, Double_t *par) 
{

  Double_t pt = x[0];
  Int_t iEta  = (Int_t)par[0];
  Int_t run   = (Int_t)par[1];
  Int_t ratio = (Int_t)par[2];
  Int_t idiso = (Int_t)par[3]; // 0 : id ; 1 : iso

  const int nEta=2; // [0;1.479[  [1.479;2.1[
  const int nRun=6; // ABCD, MC-ABCD, ABC, MC-ABC, D, MC-D
  const int nPt =2; // ]-inf;30[ [30;+inf[

  // aborting cases //
  if(run<0 || run>=nRun) { cout << "Choose run>=0 and run<" << nRun << endl; return 0; }
  if(ratio<0 || ratio>1) { cout << "Choose ratio=0 or 1"            << endl; return 0; }
  if(idiso<0 || idiso>1) { cout << "Choose idiso=0 or 1"            << endl; return 0; }
  if(iEta<0  || iEta>=nEta)  { cout << "Choose iEta>=0 and <"       << nEta   << endl; return 0; }
  
  // define results
  //
  // ID
  //
//   Double_t resID[nRun][nEta][nPt]
//     = { { { 0.7463 , 0.8506} , { 0.3444 , 0.5580} } ,  // ABCD
// 	{ { 0.8200 , 0.8961} , { 0.4178 , 0.6026} } ,  // MC ABCD
// 	{ { 0.7532 , 0.8587} , {0.3631  , 0.5611} } ,  // ABC
// 	{ { 0.8249 , 0.8975} , {0.4267  , 0.6074} } ,  // MC ABC
// 	{ { 0.7299 , 0.8510} , {0.3294  , 0.5491} } ,  // D
// 	{ { 0.7903 , 0.8961} , {0.4075  , 0.5984} } }; // MC D
// 	//                  //                   //                         //
// 	//      [0;1.479[           [1.479;2.1[
//NEW ReReco
  Double_t resID[nRun][nEta][nPt]
    = { { { 0.737978 , 0.849993} , { 0.331858 , 0.534207} } ,  // ABCD
	{ { 0.82005 , 0.896069} , { 0.41771 , 0.602545} } ,  // MC ABCD
	{ { 0.7532 , 0.8587} , {0.3631  , 0.5611} } ,  // ABC
	{ { 0.8249 , 0.8975} , {0.4267  , 0.6074} } ,  // MC ABC
	{ { 0.7299 , 0.8510} , {0.3294  , 0.5491} } ,  // D
	{ { 0.7903 , 0.8961} , {0.4075  , 0.5984} } }; // MC D
	//                  //                   //                         //
	//      [0;1.479[           [1.479;2.1[
  // ISO
  //
//   Double_t resISO[nRun][nEta][nPt]
//     = { { { 0.7365 , 0.8904} , { 0.8177 , 0.9190} } ,  // ABCD
// 	{ { 0.7779 , 0.9068} , { 0.8530 , 0.9237} } ,  // MC ABCD
// 	{ { 0.7496 , 0.8956} , {0.8244  , 0.9187} } ,  // ABC
// 	{ { 0.7806 , 0.9084} , {0.8533  , 0.9240} } ,  // MC ABC
// 	{ { 0.7302 , 0.8823} , {0.8304  , 0.9174} } ,  // D
// 	{ { 0.7735 , 0.9041} , {0.8511  , 0.9233} } }; // MC D
// 	//                  //                   //                         //
// 	//      [0;1.479[           [1.479;2.1[
//NEW ReReco
  Double_t resISO[nRun][nEta][nPt]
    = { { { 0.732434 , 0.888954} , { 0.807757 , 0.915259} } ,  // ABCD
	{ { 0.777811 , 0.906771} , { 0.852909 , 0.924465} } ,  // MC ABCD
	{ { 0.7496 , 0.8956} , {0.8244  , 0.9187} } ,  // ABC
	{ { 0.7806 , 0.9084} , {0.8533  , 0.9240} } ,  // MC ABC
	{ { 0.7302 , 0.8823} , {0.8304  , 0.9174} } ,  // D
	{ { 0.7735 , 0.9041} , {0.8511  , 0.9233} } }; // MC D
	//                  //                   //                         //
	//      [0;1.479[           [1.479;2.1[	
  // choose eta index
//   int iEta;
//   if(abs(eta) < 0.8)      iEta = 0; // [0;0.8[
//   else if( iEta = 1; // [0.8;1.2[
  
  // choose pt index
  int iPt;
  if(pt<30.) iPt=0;
  else       iPt=1;

  // Return relevant result //
  if(ratio==0) {
    if(idiso==0) return resID[run][iEta][iPt];
    else         return resISO[run][iEta][iPt];
  }
  else if(run==0 || run==2 || run==4) { // numerator is always data
    if(idiso==0) return resID[run+1][iEta][iPt] !=0 ? resID[run][iEta][iPt] / resID[run+1][iEta][iPt] : 0 ;
    else         return resISO[run+1][iEta][iPt]!=0 ? resISO[run][iEta][iPt]/ resISO[run+1][iEta][iPt]: 0 ;
  }
  else return 0;
}
//
// Electron Trigger
//
Double_t myFuncTurnOnEle(Double_t *x, Double_t *par) { 

  Double_t pt  = x[0];
  Int_t iEta  = (Int_t)par[0];
  Int_t run   = (Int_t)par[1];
  Int_t ratio = (Int_t)par[2];

  const int nEta=2; // EB / EE (1.479)
  const int nRun=9; // A, B, C, D, MC-old, ABCD, MC-new, SoftEleD, MC-new (to be replaced by SoftEleMC)

  // aborting cases //
  if(run<0   || run>=nRun) { cout << "Choose run>=0 and run<" << nRun << endl; return 0; }
  if(ratio<0 || ratio>1)   { cout << "Choose ratio=0 or 1"    << endl; return 0; }
  if(iEta<0  || iEta>=nEta){ cout << "Choose iEta>=0 and <"   << nEta << endl; return 0; }

  ratioEfficiencyTest* fitEffEle[nRun][nEta];

  // 2012A
  fitEffEle[0][0] = new ratioEfficiencyTest(20.4669, 1.20429, 1.84954, 1.38645, 0.891122);
  fitEffEle[0][1] = new ratioEfficiencyTest(21.4136, 1.93922, 2.43562, 1.00186, 51.947);

  // 2012B
  fitEffEle[1][0] = new ratioEfficiencyTest(22.8618, 0.844755, 1.07941, 1.27956, 1.07722);
  fitEffEle[1][1] = new ratioEfficiencyTest(22.1045, 1.08481, 0.780119, 1.91846, 0.962174);

  // 2012C
  fitEffEle[2][0] = new ratioEfficiencyTest(22.8598, 0.855666, 1.02951, 1.32713, 1.05486);
  fitEffEle[2][1] = new ratioEfficiencyTest(21.7643, 1.45024, 0.785753, 3.14722, 0.926788 );

  // 2012D 
  fitEffEle[3][0] = new ratioEfficiencyTest(23.0825, 0.930633, 1.17931, 1.19807, 1.19376 );
  fitEffEle[3][1] = new ratioEfficiencyTest(21.8373, 1.26926, 0.801134, 2.27808, 0.938937 );
					   
  // MC-old
  fitEffEle[4][0] = new ratioEfficiencyTest(21.4136, 0.000422, 2.47314e-06, 1.42487, 1.00104);
  fitEffEle[4][1] = new ratioEfficiencyTest(20.9985, 0.002918, 3.43131e-05, 1.41479, 1.06506);
					   
  // ABCD           
//   fitEffEle[5][0] = new ratioEfficiencyTest(22.9041, 1.04728, 1.38544, 1.22576, 1.13019);
//   fitEffEle[5][1] = new ratioEfficiencyTest(21.9941, 1.43419, 1.01152, 2.28622, 0.939872);
  fitEffEle[5][0] = new ratioEfficiencyTest(22.9704, 	1.0258, 	1.26889, 	1.31024, 	1.06409);
  fitEffEle[5][1] = new ratioEfficiencyTest(21.9816, 	1.40993, 	0.978597, 	2.33144, 	0.937552);
					   
  // MC-new
  fitEffEle[6][0] = new ratioEfficiencyTest(21.7243, 0.619015, 0.739301, 1.34903, 1.02594);
  fitEffEle[6][1] = new ratioEfficiencyTest(22.1217, 1.34054, 1.8885, 1.01855, 4.7241);
					   
  // SoftEleD
  // REBECCA //
  //fitEffEle[7][0] = new ratioEfficiencyTest(13.6024, 0.818415, 0.395957, 2.34391, 0.822224);
  //fitEffEle[7][1] = new ratioEfficiencyTest(14.1269, 1.37584,  0.408418, 118.504, 0.800704);
  // THIBAUT //
  fitEffEle[7][0] = new ratioEfficiencyTest(13.1463, 0.0512085, 0.00895914, 1.33272, 1.03694);
  fitEffEle[7][1] = new ratioEfficiencyTest(12.2183, 0.432813,  0.0510815,  4.62911, 0.842382);
					   
  // MC-new
  fitEffEle[8][0] = new ratioEfficiencyTest(21.7243, 0.619015, 0.739301, 1.34903, 1.02594);
  fitEffEle[8][1] = new ratioEfficiencyTest(22.1217, 1.34054, 1.8885, 1.01855, 4.7241);

					   
  // choose iEta //
//   int iEta;
//   if(eta < -1.2)      iEta = 0;
//   else if(eta < -0.8) iEta = 1;
//   else if(eta < 0)    iEta = 2;
//   else if(eta < 0.8)  iEta = 3;
//   else if(eta < 1.2)  iEta = 4;
//   else                iEta = 5;


  // return relevant result //
  if(ratio==0) return fitEffEle[run][iEta]->turnOn(pt) ;

  else {
    if(run>=0 && run<=3) 
      return fitEffEle[4][iEta]->turnOn(pt)!=0 ? fitEffEle[run][iEta]->turnOn(pt) / fitEffEle[4][iEta]->turnOn(pt) : 0;

    else if(run==5)
      return fitEffEle[6][iEta]->turnOn(pt)!=0 ? fitEffEle[run][iEta]->turnOn(pt) / fitEffEle[6][iEta]->turnOn(pt) : 0;

    else if(run==7)
      return fitEffEle[8][iEta]->turnOn(pt)!=0 ? fitEffEle[run][iEta]->turnOn(pt) / fitEffEle[8][iEta]->turnOn(pt) : 0;

    else return 0;
  }
}

///////////////
//   MUONS   //
///////////////
//
// Muons ID, ISO
//
Double_t myFuncTurnOnMuIdIso(Double_t *x, Double_t *par) {

  Double_t pt = x[0];
  Int_t iEta  = (Int_t)par[0];
  Int_t run   = (Int_t)par[1];
  Int_t ratio = (Int_t)par[2];
  Int_t idiso = (Int_t)par[3]; // 0 : id ; 1 : iso

  const int nEta=3; // [0;0.8[  [0.8;1.2[  [1.2;2.1[
  const int nRun=6; // ABCD, MC-ABCD, ABC, MC-ABC, D, MC-D
  const int nPt =2; // ]-inf;30[ [30;+inf[

  // aborting cases //
  if(run<0 || run>=nRun)    { cout << "Choose run>=0 and run<" << nRun << endl; return 0; }
  if(ratio<0 || ratio>1)    { cout << "Choose ratio=0 or 1"            << endl; return 0; }
  if(idiso<0 || idiso>1)    { cout << "Choose idiso=0 or 1"            << endl; return 0; }
  if(iEta<0  || iEta>=nEta) { cout << "Choose iEta>=0 and <"   << nEta << endl; return 0; }
  
  // define results
  //
  // ID
  //
//   Double_t resID[nRun][nEta][nPt]
//     = { { { 0.9505 , 0.9493} , { 0.9412 , 0.9391} , { 0.9415 , 0.9396} } ,  // ABCD
// 	{ { 0.9647 , 0.9631} , { 0.9586 , 0.9578} , { 0.9511 , 0.9490} } ,  // MC ABCD
// 	{ { 0.9524 , 0.9508} , { 0.9430 , 0.9406} , { 0.9430 , 0.9508} } ,  // ABC
// 	{ { 0.9649 , 0.9632} , { 0.9586 , 0.9581} , { 0.9512 , 0.9491} } ,  // MC ABC
// 	{ { 0.9485 , 0.9470} , { 0.9382 , 0.9370} , { 0.9392 , 0.9379} } ,  // D
// 	{ { 0.9644 , 0.9629} , { 0.9585 , 0.9576} , { 0.9510 , 0.9489} } }; // MC D
// 	//                  //                   //                         //
// 	//      [0;0.8[             [0.8;1.2[            [1.2;2.1[
//NEW ReReco
  Double_t resID[nRun][nEta][nPt]
    = { { { 0.947122 , 0.94881} , { 0.942154 , 0.943422} , { 0.938653 , 0.938045} } ,  // ABCD
	{ { 0.964696 , 0.963092} , { 0.958555 , 0.957613} , { 0.951103 , 0.949022} } ,  // MC ABCD
	{ { 0.9524 , 0.9508} , { 0.9430 , 0.9406} , { 0.9430 , 0.9508} } ,  // ABC
	{ { 0.9649 , 0.9632} , { 0.9586 , 0.9581} , { 0.9512 , 0.9491} } ,  // MC ABC
	{ { 0.9485 , 0.9470} , { 0.9382 , 0.9370} , { 0.9392 , 0.9379} } ,  // D
	{ { 0.9644 , 0.9629} , { 0.9585 , 0.9576} , { 0.9510 , 0.9489} } }; // MC D
	//                  //                   //                         //
	//      [0;0.8[             [0.8;1.2[            [1.2;2.1[
  // ISO
  //
//   Double_t resISO[nRun][nEta][nPt]
//     = { { { 0.7591 , 0.9016} , { 0.8044 , 0.9218} , { 0.8491 , 0.9393} } ,  // ABCD
// 	{ { 0.7838 , 0.9133} , { 0.8201 , 0.9289} , { 0.8515 , 0.9382} } ,  // MC ABCD
// 	{ { 0.7642 , 0.9041} , { 0.8080 , 0.9239} , { 0.8497 , 0.9397} } ,  // ABC
// 	{ { 0.7866 , 0.9151} , { 0.8222 , 0.9361} , { 0.8531 , 0.9389} } ,  // MC ABC
// 	{ { 0.7493 , 0.8973} , { 0.7986 , 0.9187} , { 0.8486 , 0.9387} } ,  // D
// 	{ { 0.7794 , 0.9337} , { 0.8168 , 0.9270} , { 0.8491 , 0.9370} } }; // MC D
// 	//                  //                   //                         //
// 	//      [0;0.8[             [0.8;1.2[            [1.2;2.1[
//NEW ReReco
  Double_t resISO[nRun][nEta][nPt]
    = { { { 0.74406 , 0.902759} , { 0.806595 , 0.923013} , { 0.844972 , 0.937799} } ,  // ABCD
	{ { 0.783752 , 0.913384} , { 0.820126 , 0.928884} , { 0.851507 , 0.938155} } ,  // MC ABCD
	{ { 0.7642 , 0.9041} , { 0.8080 , 0.9239} , { 0.8497 , 0.9397} } ,  // ABC
	{ { 0.7866 , 0.9151} , { 0.8222 , 0.9361} , { 0.8531 , 0.9389} } ,  // MC ABC
	{ { 0.7493 , 0.8973} , { 0.7986 , 0.9187} , { 0.8486 , 0.9387} } ,  // D
	{ { 0.7794 , 0.9337} , { 0.8168 , 0.9270} , { 0.8491 , 0.9370} } }; // MC D
	//                  //                   //                         //
	//      [0;0.8[             [0.8;1.2[            [1.2;2.1[	

  // choose eta index
//   int iEta;
//   if(abs(eta) < 0.8)      iEta = 0; // [0;0.8[
//   else if(abs(eta) < 1.2) iEta = 1; // [0.8;1.2[
//   else                    iEta = 2; // [1.2;2.1[
  
  // choose pt index
  int iPt;
  if(pt<30.) iPt=0;
  else       iPt=1;

  // Return relevant result //
  if(ratio==0) {
    if(idiso==0) return resID[run][iEta][iPt];
    else         return resISO[run][iEta][iPt];
  }
  else if(run==0 || run==2 || run==4) { // numerator is always data
    if(idiso==0) return resID[run+1][iEta][iPt]!=0 ? resID[run][iEta][iPt]/resID[run+1][iEta][iPt] : 0 ;
    else         return resISO[run+1][iEta][iPt]!=0 ? resISO[run][iEta][iPt]/resISO[run+1][iEta][iPt] : 0 ;
  }
  else return 0;
}
//
// MUON TRIGGER
//
Double_t myFuncTurnOnMu(Double_t *x, Double_t *par) { 

  Double_t pt  = x[0];
  //Double_t eta = par[0];
  Int_t iEta  = (Int_t)par[0];
  Int_t run   = (Int_t)par[1];
  Int_t ratio = (Int_t)par[2];

  const int nEta=6; // ]-inf,-1.2[ [-1.2,-0.8[ [-0.8,0[ [0,0.8[ [0.8,1.2[ [1.2,+inf[
  const int nRun=9; // A, B, C, D, MC-old, ABCD, MC-new, softmuD, softmuMC

  // aborting cases //
  if(run<0   || run>=nRun)   { cout << "Choose run>=0 and run<"     << nRun << endl; return 0; }
  if(ratio<0 || ratio>1)     { cout << "Choose ratio=0 or 1"                << endl; return 0; }
  if(iEta<0  || iEta>=nEta)  { cout << "Choose iEta>=0 and <"       << nEta << endl; return 0; }

  ratioEfficiencyTest* fitEffMu[nRun][nEta];

  // 2012A
  fitEffMu[0][0] = new ratioEfficiencyTest(16.9993, 8.82202e-05, 7.91529e-08, 1.40792, 0.928102); 
  fitEffMu[0][1] = new ratioEfficiencyTest(16.9824, 0.0694986,   0.0186614,   1.66577, 0.908218); 
  fitEffMu[0][2] = new ratioEfficiencyTest(17.2736, 0.13896,     0.198452,    1.13119, 1.21897);
  fitEffMu[0][3] = new ratioEfficiencyTest(17.9605, 0.500059,    0.865294,    1.04633, 1.69027);
  fitEffMu[0][4] = new ratioEfficiencyTest(18.094,  0.607997,    0.89385,     1.36337, 0.92399); 
  fitEffMu[0][5] = new ratioEfficiencyTest(16.9805, 9.18396e-05, 2.81836e-08, 1.83783, 0.858988); 

  // 2012B
  fitEffMu[1][0] = new ratioEfficiencyTest(16.0015, 5.59745e-07, 1.3395e-07,  1.37357, 0.891284);
  fitEffMu[1][1] = new ratioEfficiencyTest(18.015,  0.0512973,   0.0603545,   1.36001, 0.907481);
  fitEffMu[1][2] = new ratioEfficiencyTest(16.4569, 0.214484,    0.302707,    1.42363, 0.982643);
  fitEffMu[1][3] = new ratioEfficiencyTest(15.9829, 0.0435624,   0.0196399,   1.71605, 0.967839);
  fitEffMu[1][4] = new ratioEfficiencyTest(17.4688, 0.0494554,   0.0628053,   1.34067, 0.904989);
  fitEffMu[1][5] = new ratioEfficiencyTest(16.0029, 4.01862e-05, 6.62491e-08, 1.42189, 0.880251);

  // 2012C
  fitEffMu[2][0] = new ratioEfficiencyTest(15.9974, 7.20337e-05,  7.72238e-08, 1.5461,  0.87064);
  fitEffMu[2][1] = new ratioEfficiencyTest(17.446,  0.760355,     1.58032,     1.0623,  1.10472);
  fitEffMu[2][2] = new ratioEfficiencyTest(15.9788, 0.044455,     0.0215911,   1.71024, 0.965673);
  fitEffMu[2][3] = new ratioEfficiencyTest(15.9762, 0.0552286,    0.0231409,   1.78576, 0.96848);
  fitEffMu[2][4] = new ratioEfficiencyTest(17.462,  0.804351,     1.62323,     1.22776, 0.900085);
  fitEffMu[2][5] = new ratioEfficiencyTest(16.0051, -4.10239e-05, 1.15509e-08, 1.82463, 0.865417);

  // 2012D ( computed for abs(eta) )

//   fitEffMu[3][0] = new ratioEfficiencyTest(15.9994, 7.37077e-05, 7.21076e-08, 1.58178, 0.861339);
//   fitEffMu[3][1] = new ratioEfficiencyTest(16.7041, 0.383545,    0.467605,    1.59941, 0.882451);
//   fitEffMu[3][2] = new ratioEfficiencyTest(15.9852, 0.0428581,   0.0160247,   1.69952, 0.971443);
//   fitEffMu[3][3] = new ratioEfficiencyTest(15.9852, 0.0428581,   0.0160247,   1.69952, 0.971443);
//   fitEffMu[3][4] = new ratioEfficiencyTest(16.7041, 0.383545,    0.467605,    1.59941, 0.882451);
//   fitEffMu[3][5] = new ratioEfficiencyTest(15.9994, 7.37077e-05, 7.21076e-08, 1.58178, 0.861339);

  // update Rebecca (not abs eta)
  fitEffMu[3][0] = new ratioEfficiencyTest(16.6377, 	-1.23117, 	2.14735, 	1.17365, 	0.957028 );
  fitEffMu[3][1] = new ratioEfficiencyTest(17.1986, 	0.649411, 	1.0234, 	1.45531, 	0.888363 );
  fitEffMu[3][2] = new ratioEfficiencyTest(15.9819, 	0.0376022, 	0.0180587, 	1.58457, 	0.97677 );
  fitEffMu[3][3] = new ratioEfficiencyTest(15.9894, 	0.0362229, 	0.0132101, 	1.65758, 	0.975996 );
  fitEffMu[3][4] = new ratioEfficiencyTest(16.5276, 	0.28491, 	0.342352, 	1.49551, 	0.894305 );
  fitEffMu[3][5] = new ratioEfficiencyTest(17.184, 	-1.44981, 	3.08139, 	1.01062, 	2.25017 );
					   
  // MC-old
  fitEffMu[4][0] = new ratioEfficiencyTest(15.997,  8.73042e-05, 5.36172e-08, 1.67934, 0.871415);
  fitEffMu[4][1] = new ratioEfficiencyTest(17.3339, 0.768105,    1.31172,     1.35161, 0.942887);
  fitEffMu[4][2] = new ratioEfficiencyTest(15.959,  0.0229759,   0.00597735,  1.76124, 0.980734);
  fitEffMu[4][3] = new ratioEfficiencyTest(15.9618, 0.0587497,   0.0189749,   1.94016, 0.978294);
  fitEffMu[4][4] = new ratioEfficiencyTest(16.7859, 0.443337,    0.571078,    1.62214, 0.919211);
  fitEffMu[4][5] = new ratioEfficiencyTest(15.9974, 8.50572e-05, 5.53033e-08, 1.64714, 0.888026);
					   
  // ABCD

//   fitEffMu[5][0] = new ratioEfficiencyTest(15.9825, 7.90724e-05, 5.49275e-08, 1.6403,  0.858285);
//   fitEffMu[5][1] = new ratioEfficiencyTest(17.3283, 0.707103,    1.2047,      1.3732,  0.900519);
//   fitEffMu[5][2] = new ratioEfficiencyTest(15.9828, 0.0412999,   0.0177441,   1.66934, 0.970097);
//   fitEffMu[5][3] = new ratioEfficiencyTest(15.9802, 0.0548775,   0.020313,    1.79791, 0.968398);
//   fitEffMu[5][4] = new ratioEfficiencyTest(16.8396, 0.458636,    0.633185,    1.5706,  0.8848);
//   fitEffMu[5][5] = new ratioEfficiencyTest(15.9987, 8.94398e-05, 5.18549e-08, 1.8342,  0.854625);

  // Update Rebecca
  fitEffMu[5][0] = new ratioEfficiencyTest(15.9977, 	7.64004e-05, 	6.4951e-08, 	1.57403, 	0.865325);
  fitEffMu[5][1] = new ratioEfficiencyTest(17.3974, 	0.804001, 	1.47145, 	1.24295, 	0.928198);
  fitEffMu[5][2] = new ratioEfficiencyTest(16.4307, 	0.226312, 	0.265553, 	1.55756, 	0.974462);
  fitEffMu[5][3] = new ratioEfficiencyTest(17.313, 	0.662731, 	1.3412, 	1.05778, 	1.26624);
  fitEffMu[5][4] = new ratioEfficiencyTest(16.9966, 	0.550532, 	0.807863, 	1.55402, 	0.885134);
  fitEffMu[5][5] = new ratioEfficiencyTest(15.9962, 	0.000106195, 	4.95058e-08, 	1.9991, 	0.851294);

  // MC-new
  fitEffMu[6][0] = new ratioEfficiencyTest(16.0051, 2.45144e-05, 4.3335e-09,  1.66134, 0.87045);
  fitEffMu[6][1] = new ratioEfficiencyTest(17.3135, 0.747636,    1.21803,     1.40611, 0.934983);
  fitEffMu[6][2] = new ratioEfficiencyTest(15.9556, 0.0236127,   0.00589832,  1.75409, 0.981338);
  fitEffMu[6][3] = new ratioEfficiencyTest(15.9289, 0.0271317,   0.00448573,  1.92101, 0.978625);
  fitEffMu[6][4] = new ratioEfficiencyTest(16.5678, 0.328333,    0.354533,    1.67085, 0.916992);
  fitEffMu[6][5] = new ratioEfficiencyTest(15.997,  7.90069e-05, 4.40036e-08, 1.66272, 0.884502);
					   
  // Soft Muon run D
  fitEffMu[7][0] = new ratioEfficiencyTest(8.18027, 1.22848,   2.1079,    1.33584, 0.887698);
  fitEffMu[7][1] = new ratioEfficiencyTest(7.39947, 0.499191,  0.287668,  2.34153, 0.875522);
  fitEffMu[7][2] = new ratioEfficiencyTest(7.36018, 0.435426,  0.144283,  2.64701, 0.961927);
  fitEffMu[7][3] = new ratioEfficiencyTest(7.49457, 0.375078,  0.134229,  2.46046, 0.96723);
  fitEffMu[7][4] = new ratioEfficiencyTest(7.42679, 0.0323626, 0.0118257, 1.59101, 0.901039);
  fitEffMu[7][5] = new ratioEfficiencyTest(8.24475, 2.07543,   4.64884,   1.1,     0.942451);
					   
  // Soft Muon MC
  fitEffMu[8][0] = new ratioEfficiencyTest(7.49948, 0.000192152, 5.71411e-05, 1.1,     1.25147);
  fitEffMu[8][1] = new ratioEfficiencyTest(7.50108, 0.00378983,  0.00376623,  1.10002, 1.30277);
  fitEffMu[8][2] = new ratioEfficiencyTest(6.48875, 0.936487,    0.282555,    3.77694, 0.981963);
  fitEffMu[8][3] = new ratioEfficiencyTest(8.55999, 1.68979,     3.19392,     1.10001, 1.20707);
  fitEffMu[8][4] = new ratioEfficiencyTest(7.42458, 0.159432,    0.200064,    1.12226, 1.30744);
  fitEffMu[8][5] = new ratioEfficiencyTest(7.48564, 0.000828055, 0.00031403,  1.1,     1.354);
					   
  // choose iEta //
//   int iEta;
//   if(eta < -1.2)      iEta = 0;
//   else if(eta < -0.8) iEta = 1;
//   else if(eta < 0)    iEta = 2;
//   else if(eta < 0.8)  iEta = 3;
//   else if(eta < 1.2)  iEta = 4;
//   else                iEta = 5;


  // return relevant result //
  if(ratio==0) return fitEffMu[run][iEta]->turnOn(pt) ;

  else {
    if(run>=0 && run<=3) 
      return fitEffMu[4][iEta]->turnOn(pt)!=0 ? fitEffMu[run][iEta]->turnOn(pt) / fitEffMu[4][iEta]->turnOn(pt) : 0;

    else if(run==5)
      return fitEffMu[6][iEta]->turnOn(pt)!=0 ? fitEffMu[run][iEta]->turnOn(pt) / fitEffMu[6][iEta]->turnOn(pt) : 0;

    else if(run==7)
      return fitEffMu[8][iEta]->turnOn(pt)!=0 ? fitEffMu[run][iEta]->turnOn(pt) / fitEffMu[8][iEta]->turnOn(pt) : 0;

    else return 0;
  }
}


//////////////
//   TAUS   //
//////////////
//
// TAU TRIGGER
//
// HLT PFTauLoose20 / MC Summer12 53X / elec+tau
//
Double_t myFuncTurnOnTau(Double_t *x, Double_t *par) { 

  Double_t pt  = x[0];
  Int_t iEta  = (Int_t)par[0];
  Int_t run   = (Int_t)par[1];
  Int_t ratio = (Int_t)par[2];
  Int_t emu   = (Int_t)par[3];

  const int nEta=2; // EB / EE (1.5)
  const int nRun=8; // ABC, MC-ABC, D, MC-D, ABCD antiEMed, MC-ABCD antiEMed, ABCD antiETight, MC-ABCD antiETight
  const int nemu=2; // e+tau / mu+tau

  // aborting cases //
  if(run<0   || run>=nRun) { cout << "Choose run>=0 and run<" << nRun << endl; return 0; }
  if(ratio<0 || ratio>1)   { cout << "Choose ratio=0 or 1"    << endl; return 0; }
  if(iEta<0  || iEta>=nEta){ cout << "Choose iEta>=0 and <"   << nEta << endl; return 0; }
  if(emu<0   || emu>=nemu) { cout << "Choose emu >=0 and <"   << nemu << endl; return 0; }

  ratioEfficiencyTest* fitEffTau[nemu][nRun][nEta];

  // UPDATED JOSE 19Jun2013 //

  // e+tau channel

  // 2012 ABC 
  fitEffTau[0][0][0] = new ratioEfficiencyTest(18.950621,    1.628276,    1.900225,    45.753269,   0.900336);
  fitEffTau[0][0][1] = new ratioEfficiencyTest(18.666002,    0.263605,    0.111819,    5.365893,    0.847341);
  // MC ABC 
  fitEffTau[0][1][0] = new ratioEfficiencyTest(18.527735,    0.278572,    0.117046,    5.580345,    0.914990);
  fitEffTau[0][1][1] = new ratioEfficiencyTest(18.622831,    0.631734,    0.432745,    133.583847,  0.865263);

  // 2012 D
  fitEffTau[0][2][0] = new ratioEfficiencyTest(18.840761,    1.783221,    2.001567,    15.510686,    0.904269);
  fitEffTau[0][2][1] = new ratioEfficiencyTest(18.735365,    0.809614,    1.409530,    1.173482,    1.017551);
  // MC D
  fitEffTau[0][3][0] = new ratioEfficiencyTest(18.530840,    0.281408,    0.119690,    5.554314,    0.915107);
  fitEffTau[0][3][1] = new ratioEfficiencyTest(18.653790,    1.410397,    3.777847,    169.134155,    0.864934);

//   // ABCD      antiEMed
//   fitEffTau[0][4][0] = new ratioEfficiencyTest(18.920740,    1.705521,    1.996879,    35.638721,   0.901402);
//   fitEffTau[0][4][1] = new ratioEfficiencyTest(18.712473,    0.263371,    0.146785,    3.342299,    0.853802);
//   // MC ABCD   antiEMed
//   fitEffTau[0][5][0] = new ratioEfficiencyTest(18.531258,    0.283163,    0.120956,    5.559196,    0.915048);
//   fitEffTau[0][5][1] = new ratioEfficiencyTest(18.641788,    0.742923,    0.598460,    134.920670,  0.865315);
  
  //Old Corrections Prior To Mar 14
//Update 28Jun2013 by Jose
//   // ABCD      antiEMed
//   fitEffTau[0][4][0] = new ratioEfficiencyTest(18.538229,    0.651562,    0.324869,    13.099048,    0.902365);
//   fitEffTau[0][4][1] = new ratioEfficiencyTest(18.756548,    0.230732,    0.142859,    3.358497,    0.851919);
//   // MC ABCD   antiEMed
//   fitEffTau[0][5][0] = new ratioEfficiencyTest(18.525766,    0.275904,    0.126185,    4.957594,    0.915910);
//   fitEffTau[0][5][1] = new ratioEfficiencyTest(18.552006,    0.632002,    0.426891,    133.934952,    0.866543);
  
  //New Corrections For MVA Tau ID Mar 14 - Medium Anti E
  //http://anayak.web.cern.ch/anayak/HToTauTau/TriggerTest/FitResults/SyncSelection/NewTauID_NewTES/FitResults.txt
  //etau channel
  // ABCD      antiEMed
  fitEffTau[0][4][0] = new ratioEfficiencyTest(1.83211e+01, -1.89051e+00, 3.71081e+00, 1.06628e+00, 1.28559e+00);//data barrel
  fitEffTau[0][4][1] = new ratioEfficiencyTest(1.80812e+01, 1.39482e+00, 1.14305e+00, 1.08989e+01, 8.97087e-01);//data endcap
  // MC ABCD   antiEMed
  fitEffTau[0][5][0] = new ratioEfficiencyTest(1.83709e+01, 1.37806e-01, 1.64478e-01, 1.44798e+00, 9.92673e-01);//MC barrel
  fitEffTau[0][5][1] = new ratioEfficiencyTest(1.83074e+01, 1.43406e+00, 1.40743e+00, 1.41501e+02, 9.19457e-01);//MC endcap

  // ABCD      antiETight
  fitEffTau[0][6][0] = new ratioEfficiencyTest(18.895025,    1.695306,    1.922852,    34.020744,   0.903446);
  fitEffTau[0][6][1] = new ratioEfficiencyTest(18.711233,    0.255624,    0.131574,    3.648101,    0.857139);
  // MC ABCD   antiETight
  fitEffTau[0][7][0] = new ratioEfficiencyTest(18.520018,    0.294271,    0.127877,    5.275917,    0.918806);
  fitEffTau[0][7][1] = new ratioEfficiencyTest(18.657221,    0.770777,    0.648889,    138.380600,  0.870723);

  /* Outdated : above are updated for Summer13 (Jose 19Jun2013)

  // 2012 ABC (took by Ivo from Josh code)
  fitEffTau[0][0][0] = new ratioEfficiencyTest(18.43442868,2.08967536,3.27357845,6.96327309,0.85564484);
  fitEffTau[0][0][1] = new ratioEfficiencyTest(18.16839440,1.86184564,4.39116712,1.01410741,1.39240481);
  // MC ABC (took by Ivo from Josh code)
  fitEffTau[0][1][0] = new ratioEfficiencyTest(18.40815138, 1.53235636, 3.55989632, 1.74542709, 0.90118450);
  fitEffTau[0][1][1] = new ratioEfficiencyTest(18.29028052, 1.56239255, 11.03605631, 155.89290151, 0.85683995);

  // 2012 D
  fitEffTau[0][2][0] = new ratioEfficiencyTest(18.73, 0.374578, 0.136068, 5.789410, 0.8638 );
  fitEffTau[0][2][1] = new ratioEfficiencyTest(19.32, 0.146243, 0.123579, 3.126114, 0.8313 );
  // MC D
  fitEffTau[0][3][0] = new ratioEfficiencyTest(19.22, 0.204905, 0.175676, 2.644803, 0.8974);
  fitEffTau[0][3][1] = new ratioEfficiencyTest(18.62, 0.037935, 0.002134, 95.090919, 0.8515 );

  // ABCD
  fitEffTau[0][4][0] = new ratioEfficiencyTest(18.686211, 1.993524, 3.202713, 3.612693, 0.871640 );
  fitEffTau[0][4][1] = new ratioEfficiencyTest(18.472954, 1.606388, 3.468975, 55.629620,0.828977 );
  // MC ABCD
  fitEffTau[0][5][0] = new ratioEfficiencyTest(18.431118, 1.572877, 3.301699, 4.760769, 0.899620 );
  fitEffTau[0][5][1] = new ratioEfficiencyTest(18.257217, 1.632443, 9.283116, 40.219585,0.858643 );
  */
					   
  // mu+tau channel

  // 2012 ABC
  fitEffTau[1][0][0] = new ratioEfficiencyTest(18.517653,    0.359486,    0.146362,    4.051668,    0.933128);
  fitEffTau[1][0][1] = new ratioEfficiencyTest(18.349554,    0.329283,    0.127311,    4.701736,    0.876362);
  // MC ABC
  fitEffTau[1][1][0] = new ratioEfficiencyTest(18.540964,    1.328441,    2.915922,    1.001261,    12.719448);
  fitEffTau[1][1][1] = new ratioEfficiencyTest(18.328148,    1.844240,    3.687733,    134.335556,   0.889851);

  // 2012 D
  fitEffTau[1][2][0] = new ratioEfficiencyTest(18.434069,    0.530757,    0.201776,    7.190775,    0.932454);
  fitEffTau[1][2][1] = new ratioEfficiencyTest(18.671316,    1.572636,    3.074206,    1.000632,    41.692646);
  // MC D
  fitEffTau[1][3][0] = new ratioEfficiencyTest(18.539244,    1.326609,    2.909963,    1.001269,    12.692931);
  fitEffTau[1][3][1] = new ratioEfficiencyTest(18.326162,    1.849068,    3.737118,    140.684357,    0.889961);

//   // ABCD
//   fitEffTau[1][4][0] = new ratioEfficiencyTest(18.444056,    0.363077,    0.126673,    4.798026,    0.932726);
//   fitEffTau[1][4][1] = new ratioEfficiencyTest(18.556707,    0.243482,    0.121221,    2.636403,    0.894483);
//   // MC ABCD
//   fitEffTau[1][5][0] = new ratioEfficiencyTest(18.540892,    1.323321,    2.901140,    1.001297,    12.472203);
//   fitEffTau[1][5][1] = new ratioEfficiencyTest(18.151899,    0.446117,    0.159085,    120.380318,  0.890775);


  //New Corrections For MVA Tau ID Mar 14
  //mutau channel
  // ABCD
  fitEffTau[1][4][0] = new ratioEfficiencyTest(1.83211e+01, -1.89051e+00, 3.71081e+00, 1.06628e+00, 1.28559e+00);//data barrel
  fitEffTau[1][4][1] = new ratioEfficiencyTest(1.80812e+01, 1.39482e+00, 1.14305e+00, 1.08989e+01, 8.97087e-01);//data endcap
  // MC ABCD   antiEMed
  fitEffTau[1][5][0] = new ratioEfficiencyTest(1.83709e+01, 1.37806e-01, 1.64478e-01, 1.44798e+00, 9.92673e-01);//MC barrel
  fitEffTau[1][5][1] = new ratioEfficiencyTest(1.83074e+01, 1.43406e+00, 1.40743e+00, 1.41501e+02, 9.19457e-01);//MC endcap

  //Old Corrections Prior To Mar 14
//   //28Jun2013 by Jose
//   // ABCD
//   fitEffTau[1][4][0] = new ratioEfficiencyTest(18.604910,    0.276042,    0.137039,    2.698437,    0.940721);
//   fitEffTau[1][4][1] = new ratioEfficiencyTest(18.701715,    0.216523,    0.148111,    2.245081,    0.895320);
//   // MC ABCD
//   fitEffTau[1][5][0] = new ratioEfficiencyTest(18.537441,    1.385790,    3.102076,    1.002486,    6.275127);
//   fitEffTau[1][5][1] = new ratioEfficiencyTest(18.393366,    1.526254,    2.021678,    124.741631,    0.894280);


  //Outdated : above are updated for Summer13 (Jose 19Jun2013)
  //ABCD
  fitEffTau[1][6][0] = new ratioEfficiencyTest(18.52036251, 1.47760312, 2.53574445, 1.71202550, 0.93019930 );
  fitEffTau[1][6][1] = new ratioEfficiencyTest(18.41225333, 0.76598912, 0.60544260, 5.38350881, 0.85870108 );
  // MC ABCD
  fitEffTau[1][7][0] = new ratioEfficiencyTest(18.88740627, 0.10718873, 0.12277723, 1.60581265, 0.95041892 );
  fitEffTau[1][7][1] = new ratioEfficiencyTest(18.30439676, 1.44360240,	3.79358997, 1.07560564, 0.93103925 );

  /*  // OUTDATED //
  // 2012 ABC
  fitEffTau[1][0][0] = new ratioEfficiencyTest(18.50940288, 1.62285299, 2.73232995, 1.79135412, 0.91481432);
  fitEffTau[1][0][1] = new ratioEfficiencyTest(18.45678784, 0.68697618, 0.57008697, 3.73470825, 0.84747211);
  // MC ABC
  fitEffTau[1][1][0] = new ratioEfficiencyTest(18.80484409, 0.19082817, 0.19983010, 1.81979820, 0.93270649);
  fitEffTau[1][1][1] = new ratioEfficiencyTest(18.25975478, 1.32745225, 1.70380810, 149.18410074, 0.87377770);

  // 2012 D
  fitEffTau[1][2][0] = new ratioEfficiencyTest(19.09, 0.236111, 0.140104, 2.361672, 0.9137 );
  fitEffTau[1][2][1] = new ratioEfficiencyTest(19.49, 0.003359, 0.005832, 1.000378, 85.3401 );
  // MC D
  fitEffTau[1][3][0] = new ratioEfficiencyTest(18.84, 0.962342, 2.103198, 1.014981, 1.8846 );
  fitEffTau[1][3][1] = new ratioEfficiencyTest(19.01, 0.492647, 0.449299, 137.190323, 0.8850 );
   */				   
  // choose iEta //
//   int iEta;
//   if(eta < -1.2)      iEta = 0;
//   else if(eta < -0.8) iEta = 1;
//   else if(eta < 0)    iEta = 2;
//   else if(eta < 0.8)  iEta = 3;
//   else if(eta < 1.2)  iEta = 4;
//   else                iEta = 5;

  // return relevant result //
  if(ratio==0)
    {
      //Additional corrections for high pT taus in muTau & eTau (Arun, Mar14)
      TF1 *TriggerWeightBarrel = new TF1("AddTriggerWeightMuTauBarrel"  ,"1 - 9.01280e-04*(x - 140) + 4.81592e-07*(x - 140)*(x-140)",0.,800.);
      TF1 *TriggerWeightEndcaps = new TF1("AddTriggerWeightMuTauEndcaps","1 - 1.81148e-03*(x - 60) + 5.44335e-07*(x - 60)*(x-60)",0.,800.);
	  
      Double_t MCValBarrel_pt = TriggerWeightBarrel->Eval(pt);
      Double_t MCValBarrel_800 = TriggerWeightBarrel->Eval(800.);
	  
      Double_t MCValEndcaps_pt = TriggerWeightEndcaps->Eval(pt);
      Double_t MCValEndcaps_400 = TriggerWeightEndcaps->Eval(400.);

      Double_t DataValBarrel_pt = 0.3+0.7*TriggerWeightBarrel->Eval(pt);
      Double_t DataValBarrel_800 = 0.3+0.7*TriggerWeightBarrel->Eval(800.);
	  
      Double_t DataValEndcaps_pt = 0.3+0.7*TriggerWeightEndcaps->Eval(pt);
      Double_t DataValEndcaps_400 = 0.3+0.7*TriggerWeightEndcaps->Eval(400.);
	  
      delete TriggerWeightEndcaps;
      delete TriggerWeightBarrel;

      if(run==0 || run==2 || run==4)
	{
	  if(run==4 && iEta==0)//MC barrel
	    {
	      if(pt>140. && pt<=800.) return fitEffTau[emu][run][iEta]->turnOn(pt)*DataValBarrel_pt;
	      else if(pt>800.) return fitEffTau[emu][run][iEta]->turnOn(pt)*DataValBarrel_800;
	      else return fitEffTau[emu][run][iEta]->turnOn(pt);
	    }
	  else if(run==4 && iEta==1)//MC endcaps
	    {
	      if(pt>60. && pt<=400.) return fitEffTau[emu][run][iEta]->turnOn(pt)*DataValEndcaps_pt;
	      else if(pt>400.) return fitEffTau[emu][run][iEta]->turnOn(pt)*DataValEndcaps_400;
	      else return fitEffTau[emu][run][iEta]->turnOn(pt);
	    }
	  else return fitEffTau[emu][run][iEta]->turnOn(pt) ;
	}
      else
	{
	  if(run==5 && iEta==0)//MC barrel
	    {
	      if(pt>140. && pt<=800.) return fitEffTau[emu][run][iEta]->turnOn(pt)*MCValBarrel_pt;
	      else if(pt>800.) return fitEffTau[emu][run][iEta]->turnOn(pt)*MCValBarrel_800;
	      else return fitEffTau[emu][run][iEta]->turnOn(pt);
	    }
	  else if(run==5 && iEta==1)//MC endcaps
	    {
	      if(pt>60. && pt<=400.) return fitEffTau[emu][run][iEta]->turnOn(pt)*MCValEndcaps_pt;
	      else if(pt>400.) return fitEffTau[emu][run][iEta]->turnOn(pt)*MCValEndcaps_400;
	      else return fitEffTau[emu][run][iEta]->turnOn(pt);
	    }
	  else return fitEffTau[emu][run][iEta]->turnOn(pt) ;
	}
    }

  else {
    if(run==0 || run==2 || run==4)
      return fitEffTau[emu][run+1][iEta]->turnOn(pt)!=0 ? fitEffTau[emu][run][iEta]->turnOn(pt) / fitEffTau[emu][run+1][iEta]->turnOn(pt) : 0;

    else return 0;
  }


}

/////////////////////////////////////////////////
/////////////////////////////////////////////////

void makeFile(){

  TFile* fout = new TFile("/data_CMS/cms/htautau/PostMoriond/tools/llrCorrections_Winter13_v8_MVAIso.root","RECREATE");
//   TFile* fout = new TFile("/data_CMS/cms/htautau/PostMoriond/tools/llrCorrections_Summer13_v6.root","RECREATE");

  //fout->cd();

  // ELECTRON //
  //
  // Trigger
  cout << "Electron trigger" << endl;
  //
  const int nEtaEle=2;
  const int nRunEle=9;
  TString nom_run_ele[nRunEle]={"A","B","C","D","MCold","ABCD","MCnew","SoftD","SoftMC"};
  TString nom_eta_ele[nEtaEle]={"EB","EE"};
  TF1 *turnOnEle[nEtaEle][nRunEle];

  for(int iR=0 ; iR<nRunEle ; iR++) {
    for(int iE=0 ; iE<nEtaEle ; iE++) {
      turnOnEle[iE][iR] = new TF1("turnOnEle_"+nom_run_ele[iR]+"_"+nom_eta_ele[iE], myFuncTurnOnEle, 0,800,3);
      turnOnEle[iE][iR]->SetParameters(iE,iR,0);
      turnOnEle[iE][iR]->SetNpx(25600);
      turnOnEle[iE][iR]->Write();
    }
  }
  //
  // Id, Iso
  cout << "Electron id/iso" << endl;
  //
  const int nEtaEleI=2; // EB / EE (1.479)
  const int nRunEleI=6; // 
  const int nIdIso=2;   // id / iso
  TString nom_run_eleI[nRunEleI]={"ABCD","MC-ABCD","ABC","MC-ABC","D","MC-D"};
  TString nom_eta_eleI[nEtaEleI]={"EB","EE"};
  TString nom_idiso_ele[nIdIso]={"id","iso"};
  TF1 *turnOnEleIdIso[nEtaEle][nRunEle][nIdIso];

  for(int iR=0 ; iR<nRunEleI ; iR++) {
    for(int iE=0 ; iE<nEtaEleI ; iE++) {
      for(int idiso=0 ; idiso<nIdIso ; idiso++) {
	turnOnEleIdIso[iE][iR][idiso] = new TF1("turnOnEle_"+nom_run_eleI[iR]+"_"+nom_eta_eleI[iE]+"_"+nom_idiso_ele[idiso], 
					   myFuncTurnOnEleIdIso, 0,800,4);
	turnOnEleIdIso[iE][iR][idiso]->SetParameters(iE,iR,0,idiso);
	turnOnEleIdIso[iE][iR][idiso]->SetNpx(25600);
	turnOnEleIdIso[iE][iR][idiso]->Write();
      }
    }
  }
  

  // MUON //
  //
  // Trigger
  cout << "Muon trigger" << endl;
  //
  const int nEtaMuT=6;    // ]-inf,-1.2[ [-1.2,-0.8[ [-0.8,0[ [0,0.8[ [0.8,1.2[ [1.2,+inf[
  const int nRunMuT=9; // A, B, C, D, MC-old, ABCD, MC-new, softmuD, softmuMC
  TString nom_run_mu[nRunMuT]={"A","B","C","D","MCold","ABCD","MCnew","SoftD","SoftMC"};
  TString nom_eta_mu[nEtaMuT]={"0","1","2","3","4","5"};
  TF1 *turnOnMu[nEtaMuT][nRunMuT];

  for(int iR=0 ; iR<nRunMuT ; iR++) {
    for(int iE=0 ; iE<nEtaMuT ; iE++) {
      turnOnMu[iE][iR] = new TF1("turnOnMu_"+nom_run_mu[iR]+"_"+nom_eta_mu[iE], myFuncTurnOnMu, 0,800,3);
      turnOnMu[iE][iR]->SetParameters(iE,iR,0);
      turnOnMu[iE][iR]->SetNpx(25600);
      turnOnMu[iE][iR]->Write();
    }
  }
  //
  // Id, Iso
  cout << "Muon id/iso" << endl;
  //
  const int nEtaMuI=3; // [0,0.8[ [0.8,1.2[ [1.2,+inf[
  const int nRunMuI=6; // ABCD, MC-ABCD, ABC, MC-ABC, D, MC-D
  //const int nIdIso =2; // id, iso
  TString nom_run_muI[nRunMuI]={"ABCD","MC-ABCD","ABC","MC-ABC","D","MC-D"};
  TString nom_eta_muI[nEtaMuI]={"0","1","2"};
  TString nom_idiso_muI[nIdIso]={"id","iso"};
  TF1 *turnOnMuIdIso[nEtaMuI][nRunMuI][nIdIso];
  
  for(int iR=0 ; iR<nRunMuI ; iR++) {
    for(int iE=0 ; iE<nEtaMuI ; iE++) {
      for(int idiso=0 ; idiso<nIdIso ; idiso++) {
	turnOnMuIdIso[iE][iR][idiso] = new TF1("turnOnMuIdIso_"+nom_run_muI[iR]+"_"+nom_eta_muI[iE]+"_"+nom_idiso_muI[idiso], 
					       myFuncTurnOnMuIdIso, 0,800,4);
	turnOnMuIdIso[iE][iR][idiso]->SetParameters(iE,iR,0,idiso);
	turnOnMuIdIso[iE][iR][idiso]->SetNpx(25600);
	turnOnMuIdIso[iE][iR][idiso]->Write();
      }
    }
  }
  

  // TAU //
  //
  // Trigger
  cout << "Tau trigger" << endl;
  //
  const int nemu=2; // e+tau / mu+tau
  const int nEtaTauT=2; // EB / EE
  const int nRunTauT=8; // ABC, MC-ABC, D, MC-D, ABCD antiEMed, MC-ABCD antiEMed, ABCD antiETight, MC-ABCD antiETight
  TString nom_emu_tau[nemu]={"etau","mutau"};
  TString nom_run_tau[nRunTauT]={"ABC","MC-ABC","D","MC-D","ABCD","MC-ABCD","ABCD-Tight","MC-ABCD-Tight"};
  TString nom_eta_tau[nEtaTauT]={"EB","EE"};

  TF1 *turnOnTau[nemu][nEtaTauT][nRunTauT];

  for(int emu=0 ; emu<nemu ; emu++) {
    cout <<" channel = "<<emu<<endl;
    for(int iR=0 ; iR<nRunTauT ; iR++) {
      cout << "Run = "<<iR<<endl;
      for(int iE=0 ; iE<nEtaTauT ; iE++) {
	cout << "EB/EC = " <<iE<<endl;
	turnOnTau[emu][iE][iR] = new TF1("turnOnTau_"+nom_emu_tau[emu]+"_"+nom_run_tau[iR]+"_"+nom_eta_tau[iE], 
					 myFuncTurnOnTau, 0,800,4);
	turnOnTau[emu][iE][iR]->SetParameters(iE,iR,0,emu);
	turnOnTau[emu][iE][iR]->SetNpx(25600);
	turnOnTau[emu][iE][iR]->Write();
      }
    }
  }
  //

//   //Additional corrections for TAU in MuTau (Arun, Mar14)
//   TF1 *TriggerWeightBarrel = new TF1("AddTriggerWeightMuTauBarrel"  ,"1 - 9.01280e-04*(x - 140) + 4.81592e-07*(x - 140)*(x-140)",0.,800.);
//   TriggerWeightBarrel->SetNpx(25600);
//   TF1 *TriggerWeightEndcaps = new TF1("AddTriggerWeightMuTauEndcaps","1 - 1.81148e-03*(x - 140) + 5.44335e-07*(x - 140)*(x-140)",0.,800.);
//   TriggerWeightEndcaps->SetNpx(25600);

//   TriggerWeightBarrel->Write();
//   TriggerWeightEndcaps->Write();

//   //Apply this additional corrections to MC turn ons for mutau
//   TString NameTurnOn(turnOnTau[1][0][4]->GetName());//mutau, barrel, ABCD
//   cout<<turnOnTau[1][0][4]->GetExpFormula()<<endl;
//   cout<<"NameTurnOn = "<<NameTurnOn<<endl;
//   TString NameCorrection(TriggerWeightBarrel->GetName());
//   cout<<TriggerWeightBarrel->GetExpFormula()<<endl;
//   TString NewFunction = NameTurnOn+"*"+NameCorrection;
  
//   TF1* test = new TF1("test",NewFunction.Data(),0.,800.);
//   test->Write();
//   test->Print();

  fout->Write();
  fout->Close();
}
