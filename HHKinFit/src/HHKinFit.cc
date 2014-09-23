/*
 * HHKinFit.cpp
 *
 *  Created on: Jun 17, 2014
 *      Author: vormwald
 */

#include "LLRAnalysis/HHKinFit/interface/HHKinFit.h"
#include "LLRAnalysis/HHKinFit/interface/PSMath.h"
#include "LLRAnalysis/HHKinFit/interface/PSTools.h"

#include "TString.h"
#include "TPad.h"
#include "TPolyMarker.h"
#include "TMarker.h"
#include "TMatrixD.h"
#include "TLatex.h"
#include "TH2F.h"
#include <iostream>

HHKinFit::HHKinFit(HHEventRecord* recrecord)
    : m_recrecord(recrecord), 
      m_fitrecord (new HHEventRecord(*recrecord, "Fit")),      
      m_advancedBalance(kTRUE),
      m_logLevel(0),
      m_chi2(-1), 
      m_chi2_b1(-1), 
      m_chi2_b2(-1), 
      m_chi2_balance(-1),
      m_convergence(0), 
      m_fittedmH(-1),
      m_printlevel(1), 
      m_graphicslevel(1),
      m_maxloops(100)
{
  m_particlelist = m_recrecord->GetParticleList();
}

void
HHKinFit::FitNew()
{

  //  ----------  for PSfit -----
  const Int_t np = 2;
  Double_t a[np];
  Double_t astart[np];
  Double_t alimit[np][2];

  // calculate htau from tauvis; recombine leaves measured entries in event record untouched
  m_recrecord->Recombine();
  //recrecord->GetEntry(V4EventRecord::tauvis1)->PrintEPxPyPzM(V4EventRecord::tauvis1);
  //recrecord->GetEntry(V4EventRecord::tau1)->PrintEPxPyPzM(V4EventRecord::tau1);
  //recrecord->GetEntry(V4EventRecord::tauvis2)->PrintEPxPyPzM(V4EventRecord::tauvis2);
  //recrecord->GetEntry(V4EventRecord::tau2)->PrintEPxPyPzM(V4EventRecord::tau2);
  //recrecord->GetEntry(V4EventRecord::htau)->PrintEPxPyPzM(V4EventRecord::htau);

  Double_t mhtauHypo =  m_particlelist->GetParticleInfo(HHPID::h1)->M();
  Double_t mhtauReco  = m_recrecord->GetEntry(HHEventRecord::htau)->M();
  Double_t mtau  = m_particlelist->GetParticleInfo(HHPID::tau)->M();

  Double_t Etaumean =  mhtauHypo / mhtauReco
                       * sqrt(   m_recrecord->GetEntry(HHEventRecord::tauvis1)->E()
                               * m_recrecord->GetEntry(HHEventRecord::tauvis2)->E() );

  m_fitrecord->UpdateEntry(HHEventRecord::tau1)->SetEEtaPhiM(
                                    Etaumean,
                                    m_recrecord->GetEntry(HHEventRecord::tauvis1)->Eta(),
                                    m_recrecord->GetEntry(HHEventRecord::tauvis1)->Phi(),
                                    mtau);
  m_fitrecord->UpdateEntry(HHEventRecord::tau2)->SetEEtaPhiM(
                                    Etaumean,
                                    m_recrecord->GetEntry(HHEventRecord::tauvis2)->Eta(),
                                    m_recrecord->GetEntry(HHEventRecord::tauvis2)->Phi(),
                                    mtau);
  m_fitrecord->UpdateMothers(HHEventRecord::b1);
  m_fitrecord->UpdateMothers(HHEventRecord::tau1);

  
  
  
  
  
  // fill initial fit parameters
  astart[0] = m_fitrecord->GetEntry(HHEventRecord::b1)->E();         // energy of first b-jet
  astart[1] = m_fitrecord->GetEntry(HHEventRecord::tau1)->E();       // energy of first tau

  // fit range
  alimit[0][0] = astart[0] - m_fitrecord->GetEntry(HHEventRecord::b1)->dE() * 5.; // b-jets: 5 sigma
  if (alimit[0][0]<0) alimit[0][0]=0;
  alimit[0][1] = astart[0] + m_fitrecord->GetEntry(HHEventRecord::b1)->dE() * 5.;
  alimit[1][0] = m_fitrecord->GetEntry(HHEventRecord::tauvis1)->E(); // tau: minimum is muon energy
  alimit[1][1] = Etaumean * 4.;                  //      maximum is 4 times mean


  
  //first grid scan
  Int_t steps1 = 10;
  Int_t steps2 = 10;
  Double_t stepsize1=(alimit[0][1]-alimit[0][0])/(steps1*1.0);
  Double_t stepsize2=(alimit[1][1]-alimit[1][0])/(steps2*1.0);
  Double_t minE1=0;
  Double_t minE2=0;
  Double_t chi2_min=999999;
  
  for (Int_t i=0; i<steps1; i++) { // FIT loop
  for (Int_t j=0; j<steps2; j++) { // FIT loop
    Double_t Eb1test=alimit[0][0]+i*stepsize1;
    Double_t Etau1test=alimit[1][0]+j*stepsize2;
    
    m_fitrecord->UpdateEntry(HHEventRecord::b1)->SetEkeepM(Eb1test); // update 4-vectors with fit parameters
    m_fitrecord->UpdateEntry(HHEventRecord::tau1)->SetEkeepM(Etau1test);
    ConstrainE2(HHEventRecord::hb, HHEventRecord::b1, HHEventRecord::b2);
    ConstrainE2(HHEventRecord::htau, HHEventRecord::tau1, HHEventRecord::tau2);

    Double_t Etau2 = m_fitrecord->GetEntry(HHEventRecord::tau2)->E();
    if (Etau2<m_recrecord->GetEntry(HHEventRecord::tauvis2)->E()) continue;
        
    m_chi2_b1 = Chi2V4(HHEventRecord::b1);
    m_chi2_b2 = Chi2V4(HHEventRecord::b2);
    m_chi2_balance = Chi2Balance();
    m_chi2 = m_chi2_b1 + m_chi2_b2 + m_chi2_balance; // chi2 calculation

    if (m_chi2 < chi2_min){
      chi2_min=m_chi2;
      minE1=Eb1test;
      minE2=Etau1test;
    }
    
    if (m_logLevel>1){
      std::cout << "chi2 b1: " << m_chi2_b1 << std::endl;
      std::cout << "chi2 b2: " << m_chi2_b2 << std::endl;
      std::cout << "chi2 missing: " << m_chi2_balance << std::endl;
      std::cout << "------------------" << std::endl;
      std::cout << "chi2 : " << m_chi2 << std::endl;
      std::cout << "------------------" << std::endl;
      std::cout << "------------------" << std::endl;
    }
  }
  }
  
  
    
  //second grid scan
  steps1 = 20;
  steps2 = 20;
  Double_t limit1_up   = ((minE1+2*stepsize1)>alimit[0][1])?alimit[0][1]:(minE1+1*stepsize1);
  Double_t limit1_down = ((minE1-2*stepsize1)<alimit[0][0])?alimit[0][0]:(minE1-1*stepsize1);
  Double_t limit2_up   = ((minE2+2*stepsize2)>alimit[1][1])?alimit[1][1]:(minE2+1*stepsize2);
  Double_t limit2_down = ((minE2-2*stepsize2)<alimit[1][0])?alimit[1][0]:(minE2-1*stepsize2);
  stepsize1=(limit1_up-limit1_down)/(steps1*1.0);
  stepsize2=(limit2_up-limit2_down)/(steps2*1.0);
  
  for (Int_t i=0; i<steps1; i++) { // FIT loop
  for (Int_t j=0; j<steps2; j++) { // FIT loop
    Double_t Eb1test=limit1_down+i*stepsize1;
    Double_t Etau1test=limit2_down+j*stepsize2;
    
    m_fitrecord->UpdateEntry(HHEventRecord::b1)->SetEkeepM(Eb1test); // update 4-vectors with fit parameters
    m_fitrecord->UpdateEntry(HHEventRecord::tau1)->SetEkeepM(Etau1test);
    ConstrainE2(HHEventRecord::hb, HHEventRecord::b1, HHEventRecord::b2);
    ConstrainE2(HHEventRecord::htau, HHEventRecord::tau1, HHEventRecord::tau2);

    Double_t Etau2 = m_fitrecord->GetEntry(HHEventRecord::tau2)->E();
    if (Etau2<m_recrecord->GetEntry(HHEventRecord::tauvis2)->E()) continue;
        
    m_chi2_b1 = Chi2V4(HHEventRecord::b1);
    m_chi2_b2 = Chi2V4(HHEventRecord::b2);
    m_chi2_balance = Chi2Balance();
    m_chi2 = m_chi2_b1 + m_chi2_b2 + m_chi2_balance; // chi2 calculation

    if (m_chi2 < chi2_min){
      chi2_min=m_chi2;
      minE1=Eb1test;
      minE2=Etau1test;
    }
    
    if (m_logLevel>1){
      std::cout << "chi2 b1: " << m_chi2_b1 << std::endl;
      std::cout << "chi2 b2: " << m_chi2_b2 << std::endl;
      std::cout << "chi2 missing: " << m_chi2_balance << std::endl;
      std::cout << "------------------" << std::endl;
      std::cout << "chi2 : " << m_chi2 << std::endl;
      std::cout << "------------------" << std::endl;
      std::cout << "------------------" << std::endl;
    }
  }
  }
  
  
  //calculate final best SetNextPoint
    m_fitrecord->UpdateEntry(HHEventRecord::b1)->SetEkeepM(minE1); // update 4-vectors with fit parameters
    m_fitrecord->UpdateEntry(HHEventRecord::tau1)->SetEkeepM(minE2);
    ConstrainE2(HHEventRecord::hb, HHEventRecord::b1, HHEventRecord::b2);
    ConstrainE2(HHEventRecord::htau, HHEventRecord::tau1, HHEventRecord::tau2);
    m_chi2_b1 = Chi2V4(HHEventRecord::b1);
    m_chi2_b2 = Chi2V4(HHEventRecord::b2);
    m_chi2_balance = Chi2Balance();
    m_chi2 = m_chi2_b1 + m_chi2_b2 + m_chi2_balance; // chi2 calculation
  
  
  
 
  // ------ end of FIT loop
  m_fittedmH = m_fitrecord->GetEntry(HHEventRecord::H)->M();

  if (m_logLevel>0){
    std::cout << "chi2 b1:      " << m_chi2_b1 << std::endl;
    std::cout << "chi2 b2:      " << m_chi2_b2 << std::endl;
    std::cout << "chi2 balance: " << m_chi2_balance << std::endl;
    std::cout << "CHI2 TOTAL:   " << m_chi2 << std::endl;
  }
}






















void
HHKinFit::Fit()
{

//  V4Vector* ptmissrec = PTmissCalc(fitrecord);
  HHV4Vector* ptmissrec = m_recrecord->GetEntry(HHEventRecord::MET);

  //  ----------  for PSfit -----
  const Int_t np = 2;
  Double_t a[np];
  Double_t astart[np];
  Double_t alimit[np][2];
  Double_t aprec[np];
  Double_t daN[np];
  Double_t h[np];
  Double_t chi2iter[1], aMemory[np], g[np], H[np * np], Hinv[np * np];

  Int_t iter = 0;             //  number of iterations
  Int_t method = 1;           //  initial fit method, see PSfit()
  Int_t mode = 1;             //  mode =1 for start of a new fit by PSfit()
  Int_t icallNewton = -1;     //  init start of Newton Method
  Int_t iloop = 0;            // counter for falls to fit function

  // calculate htau from tauvis; recombine leaves measured entries in event record untouched
  m_recrecord->Recombine();

//  recrecord->GetEntry(V4EventRecord::tauvis1)->PrintEPxPyPzM(V4EventRecord::tauvis1);
//  recrecord->GetEntry(V4EventRecord::tau1)->PrintEPxPyPzM(V4EventRecord::tau1);
//  recrecord->GetEntry(V4EventRecord::tauvis2)->PrintEPxPyPzM(V4EventRecord::tauvis2);
//  recrecord->GetEntry(V4EventRecord::tau2)->PrintEPxPyPzM(V4EventRecord::tau2);
//  recrecord->GetEntry(V4EventRecord::htau)->PrintEPxPyPzM(V4EventRecord::htau);

  Double_t mhtauHypo =  m_particlelist->GetParticleInfo(HHPID::h1)->M();
  Double_t mhtauReco  = m_recrecord->GetEntry(HHEventRecord::htau)->M();
  Double_t mtau  = m_particlelist->GetParticleInfo(HHPID::tau)->M();

  Double_t Etaumean =  mhtauHypo / mhtauReco
                       * sqrt(   m_recrecord->GetEntry(HHEventRecord::tauvis1)->E()
                               * m_recrecord->GetEntry(HHEventRecord::tauvis2)->E() );

  m_fitrecord->UpdateEntry(HHEventRecord::tau1)->SetEEtaPhiM(
                                    Etaumean,
                                    m_recrecord->GetEntry(HHEventRecord::tauvis1)->Eta(),
                                    m_recrecord->GetEntry(HHEventRecord::tauvis1)->Phi(),
                                    mtau);
  m_fitrecord->UpdateEntry(HHEventRecord::tau2)->SetEEtaPhiM(
                                    Etaumean,
                                    m_recrecord->GetEntry(HHEventRecord::tauvis2)->Eta(),
                                    m_recrecord->GetEntry(HHEventRecord::tauvis2)->Phi(),
                                    mtau);
  m_fitrecord->UpdateMothers(HHEventRecord::b1);
  m_fitrecord->UpdateMothers(HHEventRecord::tau1);

  // fill initial fit parameters
  astart[0] = m_fitrecord->GetEntry(HHEventRecord::b1)->E();         // energy of first b-jet
  astart[1] = m_fitrecord->GetEntry(HHEventRecord::tau1)->E();       // energy of first tau
  aprec[0] = 0.01;   //0.1                 // precision for fit
  aprec[1] = 0.01;   //0.1                 // precision for fit

  // fill initial step width
  h[0] = 0.5*m_fitrecord->GetEntry(HHEventRecord::b1)->dE();        // step width = bjet uncertainty
  h[1] = 0.05*astart[1];   //   0.10*astart[1]                  // step width = 5% of value

  daN[0] = 1.0;   //0.0                 // initial search direction in Eb-Etau diagonal
  daN[1] = 1.0;   //1.0

  // fit range
  alimit[0][0] = astart[0] - m_fitrecord->GetEntry(HHEventRecord::b1)->dE() * 5.; // b-jets: 5 sigma
  if (alimit[0][0]<0) alimit[0][0]=0;
  alimit[0][1] = astart[0] + m_fitrecord->GetEntry(HHEventRecord::b1)->dE() * 5.;
  alimit[1][0] = m_fitrecord->GetEntry(HHEventRecord::tauvis1)->E(); // tau: minimum is muon energy
  alimit[1][1] = Etaumean * 4.;                  //      maximum is 4 times mean

  // tau: check initial values against fit range
  if (astart[1] - h[1] < alimit[1][0]) {
    astart[1] = alimit[1][0] + h[1];
  }
  else if (astart[1] + h[1] > alimit[1][1]) {
    astart[1] = alimit[1][1] - h[1];
  }

  for (Int_t ip = 0; ip < np; ip++) {
    a[ip] = astart[ip];
  }

  static const Int_t nloopmax = 100;
  static Double_t Xa[nloopmax], Ya[nloopmax];
  static Double_t Xa1[nloopmax], Ya1[nloopmax];
  static Double_t HPx[nloopmax], HPy[nloopmax];
  static Double_t HPx1[nloopmax], HPy1[nloopmax];

  for (Int_t iloop = 0; iloop < m_maxloops * 10 && iter < m_maxloops; iloop++) { // FIT loop
    //    chi2        = testfunction(a,np) ;
    //    std::cout << "V4KinFit  in loop iloop " << iloop << std::endl;

//    fitrecord->Print("before",0);
//    std::cout << "new values:" << "(" << alimit[0][0] << "-" << alimit[0][1] << ") " << a[0] << "--- (" << alimit[1][0] << "-" << alimit[1][1] << ") " << a[1] << std::endl;
    m_fitrecord->UpdateEntry(HHEventRecord::b1)->SetEkeepM(a[0]); // update 4-vectors with fit parameters
    m_fitrecord->UpdateEntry(HHEventRecord::tau1)->SetEkeepM(a[1]);
    ConstrainE2(HHEventRecord::hb, HHEventRecord::b1, HHEventRecord::b2);
    ConstrainE2(HHEventRecord::htau, HHEventRecord::tau1, HHEventRecord::tau2);

    m_chi2_b1 = Chi2V4(HHEventRecord::b1);
    m_chi2_b2 = Chi2V4(HHEventRecord::b2);
    m_chi2_balance = Chi2Balance();
    m_chi2 = m_chi2_b1 + m_chi2_b2 + m_chi2_balance; // chi2 calculation

    if (m_logLevel>1){
      std::cout << "chi2 b1: " << m_chi2_b1 << std::endl;
      std::cout << "chi2 b2: " << m_chi2_b2 << std::endl;
      std::cout << "chi2 missing: " << m_chi2_balance << std::endl;
      std::cout << "------------------" << std::endl;
      std::cout << "chi2 : " << m_chi2 << std::endl;
      std::cout << "------------------" << std::endl;
      std::cout << "------------------" << std::endl;
    }

//    chi2 = Chi2V4(recrecord, fitrecord, V4EventRecord::b1)
//        + Chi2V4(recrecord, fitrecord, V4EventRecord::b2)
//        + Chi2PTmiss(*ptmissrec, fitrecord);  // chi2 calculation

    if (iloop >= 0 && iloop < nloopmax) {
      Xa1[iloop] = m_fitrecord->GetEntry(HHEventRecord::b1)->E();
      Ya1[iloop] = m_fitrecord->GetEntry(HHEventRecord::tau1)->E();
    }
    if (iter >= 0 && iter < nloopmax) {
      Xa[iter] = m_fitrecord->GetEntry(HHEventRecord::b1)->E();
      Ya[iter] = m_fitrecord->GetEntry(HHEventRecord::tau1)->E();
    }
    if (iloop >= 0 && iloop < nloopmax) {
      HPx1[iloop] = m_fitrecord->GetEntry(HHEventRecord::H)->Px();
      HPy1[iloop] = m_fitrecord->GetEntry(HHEventRecord::H)->Py();
    }
    if (iter >= 0 && iter < nloopmax) {
      HPx[iter] = m_fitrecord->GetEntry(HHEventRecord::H)->Px();
      HPy[iter] = m_fitrecord->GetEntry(HHEventRecord::H)->Py();
    }

    //    if (iloop>=0 && iloop<nloopmax) {Xloop[iloop] = a[0] ;  Yloop[iloop] = a[1] ; }
    //    if (iter>=0) { nIterations = Iterations->SetNextPoint( V4fit[V4EventRecord::b1].E(), V4fit[V4EventRecord::tau1].E() ) ;   }
    //    if (iter>=0) { nIterations = Iterations->SetNextPoint( a[0], a[1] ) ;   }

    PSMath::PSfitShow(iloop, m_convergence, iter, method, mode, m_printlevel,
                      m_graphicslevel, np, a, astart, alimit, aprec, daN, h,
                      m_chi2, g, H);

    if (m_convergence != 0) {
      break;
    }
    m_convergence = PSMath::PSfit(iloop, iter, method, mode, m_printlevel,
                                  m_graphicslevel, np, a, astart, alimit, aprec,
                                  daN, h, aMemory, m_chi2, chi2iter, g, H,
                                  Hinv);
  }
  // ------ end of FIT loop
  m_fittedmH = m_fitrecord->GetEntry(HHEventRecord::H)->M();

  if (m_logLevel>0){
    std::cout << "chi2 b1:      " << m_chi2_b1 << std::endl;
    std::cout << "chi2 b2:      " << m_chi2_b2 << std::endl;
    std::cout << "chi2 balance: " << m_chi2_balance << std::endl;
    std::cout << "CHI2 TOTAL:   " << m_chi2 << std::endl;
  }
}

void
HHKinFit::ConstrainE2(Int_t iv4, Int_t iv41, Int_t iv42)
{
  m_fitrecord->UpdateMothers(iv41);
  Double_t M1, M2, M, Mc, E1, E2, b2, beta2;
  M = m_fitrecord->GetEntry(iv4)->M();
  Mc = m_particlelist->GetParticleInfo( m_fitrecord->GetEntry(iv4)->ID() )->M();
  E1 = m_fitrecord->GetEntry(iv41)->E();
  M1 = m_fitrecord->GetEntry(iv41)->M();
  E2 = m_fitrecord->GetEntry(iv42)->E();
  M2 = m_fitrecord->GetEntry(iv42)->M();

  if (M2 == 0.) { // massless case
    m_fitrecord->UpdateEntry(iv42)->SetEkeepM(E2 * (Mc / M) * (Mc / M)); // only changes absolute value and keeps eta, phi, m untouched
                                                                         // such that invariant mass of 1 and 2 gives correct mass Mc
    m_fitrecord->UpdateMothers(iv42);
    return;
  }

  Int_t ID2jet = -1;
  Int_t ID2 = m_fitrecord->GetEntry(iv42)->ID();

  if (ID2 == HHPID::q || ID2 == HHPID::c
      || ID2 == HHPID::b || ID2 == HHPID::gluon)
    ID2jet = 1;

  beta2 = sqrt(E2 * E2 - M2 * M2) / E2;
  if (ID2jet < 0) { // is not a jet
    b2 = (M2 / E2) * (M2 / E2); // isn't the no jet case identical to the jet case? 1/gamma**2 = m**2/E**2 = 1-beta**2
  }
  else { // is a jet
    b2 = 1 - beta2 * beta2;
  }

  Double_t d = (M * M - M1 * M1 - M2 * M2) / (2. * E1 * E2);
  Double_t E2lin = (Mc * Mc - M1 * M1) / (2. * E1 * d);
  Double_t E2N = E1 * d / b2;
  Double_t E2new = E2N * (-1. + sqrt(1. + 2. * E2lin / E2N));

  if (ID2jet < 0) { // is not a jet
    m_fitrecord->UpdateEntry(iv42)->SetEkeepM(E2new);
  }
  else { // is a jet
    HHV4Vector* entry = m_fitrecord->GetEntry(iv42);
    m_fitrecord->UpdateEntry(iv42)->SetEEtaPhiM(E2new, entry->Eta(), entry->Phi(), M2 * E2new / E2);
  }

  m_fitrecord->UpdateMothers(iv42);
}

Double_t
HHKinFit::Chi2V4(Int_t iv4)
{
  Double_t chi2 = 0;
  Double_t chi2_E = 0;
  Double_t chi2_Eta = 0;
  Double_t chi2_Phi = 0;
  HHV4Vector* fitobject = m_fitrecord->GetEntry(iv4);
  HHV4Vector* recobject = m_recrecord->GetEntry(iv4);

  //  std::cout << "V4chi2 dE " ; V4fit[iv4].Print();
  if (fitobject->dE() > 0.) {
    chi2_E = ((recobject->E() - fitobject->E()) / fitobject->dE())
        * ((recobject->E() - fitobject->E()) / fitobject->dE());
  }

  if (fitobject->dEta() > 0.) {
    chi2_Eta = ((recobject->Eta() - fitobject->Eta()) / fitobject->dEta())
        * ((recobject->Eta() - fitobject->Eta()) / fitobject->dEta());
  }
  if (fitobject->dPhi() > 0.) {
    chi2_Phi = ((recobject->Phi() - fitobject->Phi()) / fitobject->dPhi())
        * ((recobject->Phi() - fitobject->Phi()) / fitobject->dPhi());
  }

  chi2 = chi2_E + chi2_Eta + chi2_Phi;
  if (m_logLevel>1){
    PSTools::coutf(5, TString("E"));
    PSTools::coutf(9, 2, recobject->E());
    PSTools::coutf(9, 2, fitobject->E());
    PSTools::coutf(9, 2, recobject->E()-fitobject->E());
    PSTools::coutf(3, TString("|"));
    PSTools::coutf(9, 2, fitobject->dE());
    PSTools::coutf(3, TString("->"));
    PSTools::coutf(9, 2, chi2_E);
    std::cout << std::endl;

    PSTools::coutf(5, TString("Eta"));
    PSTools::coutf(9, 2, recobject->Eta());
    PSTools::coutf(9, 2, fitobject->Eta());
    PSTools::coutf(9, 2, recobject->Eta()-fitobject->Eta());
    PSTools::coutf(3, TString("|"));
    PSTools::coutf(9, 2, fitobject->dEta());
    PSTools::coutf(3, TString("->"));
    PSTools::coutf(9, 2, chi2_Eta);
    std::cout << std::endl;

    PSTools::coutf(5, TString("Phi"));
    PSTools::coutf(9, 2, recobject->Phi());
    PSTools::coutf(9, 2, fitobject->Phi());
    PSTools::coutf(9, 2, recobject->Phi()-fitobject->Phi());
    PSTools::coutf(3, TString("|"));
    PSTools::coutf(9, 2, fitobject->dPhi());
    PSTools::coutf(3, TString("->"));
    PSTools::coutf(9, 2, chi2_Phi);
    std::cout << std::endl;
  }

  return chi2;
}

Double_t
HHKinFit::Chi2Balance()
{

  //fit objets
  Double_t px_H_fit   = m_fitrecord->GetEntry(HHEventRecord::H)->Px();
  Double_t py_H_fit   = m_fitrecord->GetEntry(HHEventRecord::H)->Py();


  //reco objects
  Double_t px_MET = m_recrecord->GetEntry(HHEventRecord::MET)->Px();
  Double_t py_MET = m_recrecord->GetEntry(HHEventRecord::MET)->Py();

  Double_t px_b1 = m_recrecord->GetEntry(HHEventRecord::b1)->Px();
  Double_t py_b1 = m_recrecord->GetEntry(HHEventRecord::b1)->Py();

  Double_t px_b2 = m_recrecord->GetEntry(HHEventRecord::b2)->Px();
  Double_t py_b2 = m_recrecord->GetEntry(HHEventRecord::b2)->Py();

  Double_t px_tauvis1 = m_recrecord->GetEntry(HHEventRecord::tauvis1)->Px();
  Double_t py_tauvis1 = m_recrecord->GetEntry(HHEventRecord::tauvis1)->Py();

  Double_t px_tauvis2 = m_recrecord->GetEntry(HHEventRecord::tauvis2)->Px();
  Double_t py_tauvis2 = m_recrecord->GetEntry(HHEventRecord::tauvis2)->Py();

  Double_t px_H_reco = px_MET + px_b1 + px_b2 + px_tauvis1 + px_tauvis2;
  Double_t py_H_reco = py_MET + py_b1 + py_b2 + py_tauvis1 + py_tauvis2;
  Double_t pt_H_reco = sqrt(pow(px_H_reco,2) + pow(py_H_reco,2));

  Double_t res[2];
  res[0] = px_H_fit - px_H_reco;    // residuum in Pt_H
  res[1] = py_H_fit - py_H_reco;    // residuum in Pt_H

  TMatrixD Cov_MET     = m_recrecord->GetEntry(HHEventRecord::MET)->GetCov();
  TMatrixD Cov_b1      = m_recrecord->GetEntry(HHEventRecord::b1)->GetCov();
  TMatrixD Cov_b2      = m_recrecord->GetEntry(HHEventRecord::b2)->GetCov();
  TMatrixD Cov_tauvis1 = m_recrecord->GetEntry(HHEventRecord::tauvis1)->GetCov();
  TMatrixD Cov_tauvis2 = m_recrecord->GetEntry(HHEventRecord::tauvis2)->GetCov();

  Double_t Vxx = (Cov_MET - Cov_b1 - Cov_b2 - Cov_tauvis1 - Cov_tauvis2)(0,0);
  Double_t Vyy = (Cov_MET - Cov_b1 - Cov_b2 - Cov_tauvis1 - Cov_tauvis2)(1,1);
  Double_t Vxy = (Cov_MET - Cov_b1 - Cov_b2 - Cov_tauvis1 - Cov_tauvis2)(0,1);

  Double_t det, Vinvxx, Vinvyy, Vinvxy;
  det = Vxx * Vyy - Vxy * Vxy;
  Vinvxx = Vyy / det;
  Vinvyy = Vxx / det;
  Vinvxy = -Vxy / det;

  Double_t Vinv[2 * 2];
  Vinv[0] = Vinvxx;
  Vinv[1] = Vinvxy;
  Vinv[2] = Vinv[1];
  Vinv[3] = Vinvyy;

  Double_t chi2=-1;
  if (m_advancedBalance){
    chi2 = res[0] * (Vinv[0] * res[0] + Vinv[1] * res[1]) // chi2 = res_transponiert * Vinv * res
         + res[1] * (Vinv[2] * res[0] + Vinv[3] * res[1]);
  }
  else{
    chi2 =  pow(( pt_H_reco - m_fitrecord->GetEntry(HHEventRecord::H)->Pt()), 2)
          / pow((m_recrecord->GetEntry(HHEventRecord::MET)->dE()), 2);
  }

  if (m_logLevel>2){
    std::cout << "res[0] " << res[0] << "  res[1] " << res[1] << std::endl;
    std::cout << "V[0] " << Vxx << "  V[1] " << Vxy << std::endl;
    std::cout << "V[2] " << Vxy << "  V[3] " << Vyy << std::endl;
    std::cout << "Vinv[0] " << Vinv[0] << "  Vinv[1] " << Vinv[1] << std::endl;
    std::cout << "Vinv[2] " << Vinv[2] << "  Vinv[3] " << Vinv[3] << std::endl;
  }

  if (m_logLevel>1){
    PSTools::coutf(5, TString("pmx"));
    PSTools::coutf(9, 2, px_H_fit);
    PSTools::coutf(9, 2, px_H_reco);
    PSTools::coutf(9, 2, res[0]);
    PSTools::coutf(3, TString("|"));
    PSTools::coutf(9, 2, sqrt(Vxx));
    std::cout << std::endl;

    PSTools::coutf(5, TString("pmy"));
    PSTools::coutf(9, 2, py_H_fit);
    PSTools::coutf(9, 2, py_H_reco);
    PSTools::coutf(9, 2, res[1]);
    PSTools::coutf(3, TString("|"));
    PSTools::coutf(9, 2, sqrt(Vyy));
    PSTools::coutf(3, TString("->"));
    PSTools::coutf(9, 2, chi2);
    std::cout << std::endl;
    std::cout << "--------------" << std::endl;
  }

  return chi2;
}

void
HHKinFit::SetPrintLevel(Int_t level){
  m_printlevel = level;
}

void
HHKinFit::SetGraphicsLevel(Int_t level){
  m_graphicslevel=level;
}

void
HHKinFit::SetMaxLoops(Int_t loops){
  m_maxloops = loops;
}

Double_t
HHKinFit::GetChi2()
{
  return m_chi2;
}

Double_t
HHKinFit::GetChi2_b1()
{
  return m_chi2_b1;
}

Double_t
HHKinFit::GetChi2_b2()
{
  return m_chi2_b2;
}

Double_t
HHKinFit::GetChi2_balance()
{
  return m_chi2_balance;
}

Double_t
HHKinFit::GetFittedMH()
{
  return m_fittedmH;
}

Double_t
HHKinFit::GetConvergence()
{
  return m_convergence;
}

HHEventRecord*
HHKinFit::GetFitrecord(){
  return m_fitrecord;
}

void
HHKinFit::SetAdvancedBalance(Bool_t flag)
{
  m_advancedBalance = flag;
}

void
HHKinFit::SetLogLevel(Int_t level)
{
  m_logLevel = level;
}


double
HHKinFit::GetPullE(Int_t iv4){
  HHV4Vector* fv = m_fitrecord->GetEntry(iv4);
  HHV4Vector* rv = m_recrecord->GetEntry(iv4);
  return (fv->E()-rv->E())/rv->dE();
}


double
HHKinFit::GetPullBalance(){
  Double_t px_MET = m_recrecord->GetEntry(HHEventRecord::MET)->Px();
  Double_t py_MET = m_recrecord->GetEntry(HHEventRecord::MET)->Py();

  Double_t px_b1 = m_recrecord->GetEntry(HHEventRecord::b1)->Px();
  Double_t py_b1 = m_recrecord->GetEntry(HHEventRecord::b1)->Py();

  Double_t px_b2 = m_recrecord->GetEntry(HHEventRecord::b2)->Px();
  Double_t py_b2 = m_recrecord->GetEntry(HHEventRecord::b2)->Py();

  Double_t px_tauvis1 = m_recrecord->GetEntry(HHEventRecord::tauvis1)->Px();
  Double_t py_tauvis1 = m_recrecord->GetEntry(HHEventRecord::tauvis1)->Py();

  Double_t px_tauvis2 = m_recrecord->GetEntry(HHEventRecord::tauvis2)->Px();
  Double_t py_tauvis2 = m_recrecord->GetEntry(HHEventRecord::tauvis2)->Py();

  Double_t px_H_reco = px_MET + px_b1 + px_b2 + px_tauvis1 + px_tauvis2;
  Double_t py_H_reco = py_MET + py_b1 + py_b2 + py_tauvis1 + py_tauvis2;
  Double_t pt_H_reco = sqrt(pow(px_H_reco,2) + pow(py_H_reco,2));


  Double_t pull=-99;
  if (m_advancedBalance){
//    pull = res[0] * (Vinv[0] * res[0] + Vinv[1] * res[1]) // chi2 = res_transponiert * Vinv * res
//         + res[1] * (Vinv[2] * res[0] + Vinv[3] * res[1]);
//    pull = sqrt(pull); // is this correct??
    if (m_chi2_balance>=0) return sqrt(m_chi2_balance);
  }
  else{
    pull =  (pt_H_reco - m_fitrecord->GetEntry(HHEventRecord::H)->Pt()) / (m_recrecord->GetEntry(HHEventRecord::MET)->dE());
  }
  return pull;
}
