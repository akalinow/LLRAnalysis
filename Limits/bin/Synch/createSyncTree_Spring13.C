#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TCut.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TLorentzVector.h"
#include "TROOT.h"
#include "TH1F.h"

//Preselection 
//
//Requirement Objects must pass lepton ids and default kinematic selection
//Trigger Selection : 
//mu  Tau     : HLT_IsoMu15_LooseIsoPFTau15
//e   Tau     : HLT_Ele18_MediumIsoPFTau20
//e   Mu      : HLT_Mu8_Ele17_CaloIdT_CaloIsoVL || HLT_Mu17_Ele8_CaloIdT_CaloIsoVL 
//Tau Tau     : No Trigger
//
//Kinematic Selection (Take 2 highest pt leptons passing id+iso)
//mu  Tau     : 17 (muon)     |eta| < 2.1, 20 (hadronic Tau)  |eta| < 2.3
//e   Tau     : 20 (electron) |eta| < 2.1, 20 (hadronic Tau)  |eta| < 2.3
//e   Mu      : 20 (leading)  |eta| < 2.3, 10 (trailing)      |eta| < 2.1 
//Tau Tau     : 45            |eta| < 2.1, 45                 |eta| < 2.1
//
//Lepton Id 
//mu  Tau     : Muon: PF Muon Tight + |dZ| < 0.2 |d0| < 0.045 (these are wrt PV)    Tau:  decay mode + disc agains muon tight + discrimation agains electron loose
//e   Tau     : Electron : WP95(below)+ |dZ| < 0.2 |d0| < 0.045 (these are wrt PV)  Tau:  decay mode + anti-electron mva (+medium agains electrons) + disc against muon loose
//e   Mu      : Electron : Non triggring MVA(Twiki WP)                              Muon: PF Tight
//Tau Tau     : Decay mode + loose Muon + 1 loose electron + 1 MVA rejection
//
//Lepton Iso 
//mu  Tau     : Muon     : Delta Beta < 0.1               Tau  : mva id loose WP ( mva < 0.795)
//e   Tau     : electron : Delta Beta < 0.1               Tau  : mva id loose WP ( mva < 0.795)
//e   Mu      : Electron : Delta Beta < 0.15(EB)<0.1(EE)  Muon : Delta Beta < 0.15(EB)<0.1(EE)  
//Tau Tau     : tight MVA Isolation 
//
//2nd Lepton Veto
//mu Tau     : no 2 pf && (global || tracker) opposite charge muons with pT > 15 GeV and delta beta iso on each < 0.3 and delta R bewteen muons     > 0.15
//e  Tau     : no 2 WP95   opposite charge electrons                with pT > 15 GeV and delta beta iso on each < 0.3 and delta R bewteen electrons > 0.15
//e  Mu      : no Reco muon with 0.3 of the electron
//
//Jet Veto around leptons (please note this occurs AFTER selection)
//mu Tau     : no Jets around selected muon     with Delta R <  0.5 and selected Tau  with Delta R < 0.5
//e  Tau     : no Jets around selected elecron  with Delta R <  0.5 and selected Tau  with Delta R < 0.5
//e  Mu      : no Jets around selected electron with Delta R <  0.5 and selected Muon with Delta R < 0.5
//
//Jet id
//JEC        : https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections
//    Data   : GR_R_42_V23
//    MC     : MC_42_V17
//
//mu Tau     : All Jets passing PF loose id + MVA Id loose WP  + |eta| < 5.0 + pt > 30 GeV
//e  Tau     : All Jets passing PF loose id + MVA Id loose WP  + |eta| < 5.0 + pt > 30 GeV
//e  Mu      : All Jets passing PF loose id + MVA Id loose WP  + |eta| < 5.0 + pt > 30 GeV
//
//B Tag
//mu Tau     : Combined Secondary vertex Medium WP |eta| < 2.4 + pt > 20 GeV
//e  Tau     : Combined Secondary vertex Medium WP |eta| < 2.4 + pt > 20 GeV
//e  Mu      : Combined Secondary vertex Medium WP |eta| < 2.4 + pt > 20 GeV

//Footnotes
//WP95 Electron definition
//    (isEB && sigma_ieta_ieta < 0.01 && dphi < 0.80 && dEta < 0.007 && HoE<0.15)  ||  
//    (isEE && sigma_ieta_ieta )<0.03 && dphi < 0.70 && dEta < 0.010) 
//    && abs(d0wWrtPV)<0.045 && abs(dzWrtPV)<0.2 
//

void synchNtuple(string sample = "GGFH125", string stream = "MuTau", bool incl=false) {

  TFile *lOFile = new TFile(("Output_"+sample+".root").c_str(),"RECREATE");
  TTree *lOTree = new TTree("TauCheck","TauCheck");

   //Bookeeping
   int   lRun         = 0; lOTree->Branch("run"        ,&lRun           ,"lRun/I"     );//Run
   int   lLumi        = 0; lOTree->Branch("lumi"       ,&lLumi          ,"lLumi/I"    );//Lumi
   int   lEvt         = 0; lOTree->Branch("evt"        ,&lEvt           ,"lEvt/I"     );//Evt

   //Event Variables
   int   lNPV         = 0; lOTree->Branch("npv"        ,&lNPV           ,"lNPV/I"     );//NPV
   int   lNPU         = 0; lOTree->Branch("npu"        ,&lNPU           ,"lNPU/I"     );//NPU
   float lRho         = 0; lOTree->Branch("rho"        ,&lRho           ,"lRho/F"     );//Rho
   
   //Event Weights
   float lMCWeight    = 0; lOTree->Branch("mcweight"   ,&lMCWeight      ,"lMCWeight/F");//MC Weight (xs/nevents * additional wieght (ie pt weight for gghiggs))
   float lPUWeight    = 0; lOTree->Branch("puweight"   ,&lPUWeight      ,"lPUWeight/F");//Pielup Weight
   float lEffWeight   = 0; lOTree->Branch("effweight"  ,&lEffWeight     ,"lEffWeight/F");//Effieiency Scale factor (all components multiplied in)
   float lWeight      = 0; lOTree->Branch("weight"     ,&lWeight        ,"lWeight/F"  );//mcweight*puweight*effweight

   float lTrigweight_1 = 0; lOTree->Branch("trigweight_1"     ,&lTrigweight_1        ,"lTrigweight_1/F"  );
   float lIdweight_1 = 0; lOTree->Branch("idweight_1"     ,&lIdweight_1        ,"lIdweight_1/F"  );
   float lIsoweight_1 = 0; lOTree->Branch("isoweight_1"     ,&lIsoweight_1        ,"lIsoweight_1/F"  );
   float lTrigweight_2 = 0; lOTree->Branch("trigweight_2"     ,&lTrigweight_2        ,"lTrigweight_2/F"  ); 
   float lIdweight_2 = 0; lOTree->Branch("idweight_2"     ,&lIdweight_2        ,"lIdweight_2/F"  ); 
   float lIsoweight_2 = 0; lOTree->Branch("isoweight_2"     ,&lIsoweight_2        ,"lIsoweight_2/F"  );

   //SV Fit variables
   float lMSV         = 0; lOTree->Branch("m_sv"       ,&lMSV           ,"lMSV/F"     );//SV Fit using integration method
   float lMSVUp       = 0; lOTree->Branch("m_sv_Up"    ,&lMSVUp         ,"lMSVUp/F"   );//High Energy scale shape
   float lMSVDown     = 0; lOTree->Branch("m_sv_Down"  ,&lMSVDown       ,"lMSVDown/F" );//Low Energy Scale Shape
 
   float lPtSV         = 0; lOTree->Branch("pt_sv"       ,&lPtSV           ,"lPtSV/F"     );//SV Fit using integration method
   float lEtaSV        = 0; lOTree->Branch("eta_sv"       ,&lEtaSV           ,"lEtaSV/F"     );
   float lPhiSV        = 0; lOTree->Branch("phi_sv"       ,&lPhiSV           ,"lPhiSV/F"     );

   ///First lepton :  muon for mu Tau, electron for e Tau, electron for e mu, Leading (in pT) Tau for Tau Tau
   float lPt1         = 0; lOTree->Branch("pt_1"       ,&lPt1           ,"lPt1/F"     ); //pT 
   float lPhi1        = 0; lOTree->Branch("phi_1"      ,&lPhi1          ,"lPhi1/F"    ); //Phi 
   float lEta1        = 0; lOTree->Branch("eta_1"      ,&lEta1          ,"lEta1/F"    ); //Eta 
   float lM1          = 0; lOTree->Branch("m_1"        ,&lM1            ,"lM1/F"      ); //Mass 
   float lIso1        = 0; lOTree->Branch("iso_1"      ,&lIso1          ,"lIso1/F"    ); //Delta Beta iso value 
   float lMVA1        = 0; lOTree->Branch("mva_1"      ,&lMVA1          ,"lMVA1/F"   );//MVA id (when using electron) 0 otherwise
   float lD01         = 0; lOTree->Branch("d0_1"       ,&lD01           ,"lD0/F"      );//d0 with respect to primary vertex
   float lDZ1         = 0; lOTree->Branch("dZ_1"       ,&lDZ1           ,"lDZ/F"      );//dZ with respect to primary vertex
   bool  lPassId1     = 0; lOTree->Branch("passid_1"   ,&lPassId1       ,"lPassId1/B" );//Whether it passes id  (not necessarily iso)
   bool  lPassIso1    = 0; lOTree->Branch("passiso_1"  ,&lPassIso1      ,"lPassIso1/B");//Whether it passes iso (not necessarily id)
   float lMt1         = 0; lOTree->Branch("mt_1"       ,&lMt1           ,"lMt1/F"     );//mT of  first lepton wrt to MVA met

   ///Second lepton :  hadronic Tau for mu Tau had for e Tau, Muon for e mu, Trailing (in pT)  Tau for Tau Tau
   float lPt2         = 0; lOTree->Branch("pt_2"       ,&lPt2           ,"lPt2/F"     );//pT
   float lPhi2        = 0; lOTree->Branch("phi_2"      ,&lPhi2          ,"lPhi2/F"    );//Phi
   float lEta2        = 0; lOTree->Branch("eta_2"      ,&lEta2          ,"lEta2/F"    );//Eta
   float lM2          = 0; lOTree->Branch("m_2"        ,&lM2            ,"lM2/F"      );//Mass (visible mass for hadronic Tau)
   float lIso2        = 0; lOTree->Branch("iso_2"      ,&lIso2          ,"lIso2/F"    );//MVA iso for hadronic Tau, Delta Beta for muon
   float lMVA2        = 0; lOTree->Branch("mva_2"      ,&lMVA2          ,"lMVA2/F"   );//MVA id (for anti electron id)
   bool  lPassId2     = 0; lOTree->Branch("passid_2"   ,&lPassId2       ,"lPassId2/B" );//Whether it passes id  (not necessarily iso)
   bool  lPassIso2    = 0; lOTree->Branch("passiso_2"  ,&lPassIso2      ,"lPassIso2/B");//Whether it passes iso (not necessarily id)
   float lMt2         = 0; lOTree->Branch("mt_2"       ,&lMt2           ,"lMt2/F"     );//mT of 2nd lepton wrt to MVA met
   
   float lbyCombinedIsolationDeltaBetaCorrRaw3Hits         = 0; lOTree->Branch("byCombinedIsolationDeltaBetaCorrRaw3Hits_2"       ,&lbyCombinedIsolationDeltaBetaCorrRaw3Hits           ,"lbyCombinedIsolationDeltaBetaCorrRaw3Hits/F"     );
   float lagainstElectronMVA3raw         = 0; lOTree->Branch("againstElectronMVA3raw_2"       ,&lagainstElectronMVA3raw           ,"lagainstElectronMVA3raw/F"     );
   float lbyIsolationMVA2raw         = 0; lOTree->Branch("byIsolationMVA2raw_2"       ,&lbyIsolationMVA2raw           ,"lbyIsolationMVA2raw/F"     );
   float lagainstMuonLoose2         = 0; lOTree->Branch("againstMuonLoose2_2"       ,&lagainstMuonLoose2           ,"lagainstMuonLoose2/F"     );
   float lagainstMuonMedium2         = 0; lOTree->Branch("againstMuonMedium2_2"       ,&lagainstMuonMedium2           ,"lagainstMuonMedium2/F"     );
   float lagainstMuonTight2         = 0; lOTree->Branch("againstMuonTight2_2"       ,&lagainstMuonTight2           ,"lagainstMuonTight2/F"     );

   //Met related variables
   float lMet         = 0; lOTree->Branch("met"        ,&lMet           ,"lMet/F"      ); //pfmet
   float lMetPhi      = 0; lOTree->Branch("metphi"     ,&lMetPhi        ,"lMetPhi/F"   ); //pfmet Phi
   float lMVAMet      = 0; lOTree->Branch("mvamet"     ,&lMVAMet        ,"lMet/F"      ); //mvamet
   float lMVAMetPhi   = 0; lOTree->Branch("mvametphi"  ,&lMVAMetPhi     ,"lMetPhi/F"   ); //mvamet Phi
   float lPZetaVis    = 0; lOTree->Branch("pzetavis"   ,&lPZetaVis      ,"lPZetaVis/F" ); //pZeta Visible
   float lPZetaMiss   = 0; lOTree->Branch("pzetamiss"  ,&lPZetaMiss     ,"lPZetaMiss/F"); //pZeta Missing
   //MET covariance matrices
   float lMetCov00   = 0; lOTree->Branch("metcov00"   ,&lMetCov00      ,"lMetCov00/F"); //pf met covariance matrix 00 
   float lMetCov01    = 0; lOTree->Branch("metcov01"   ,&lMetCov01      ,"lMetCov01/F"); //pf met covariance matrix 01 
   float lMetCov10    = 0; lOTree->Branch("metcov10"   ,&lMetCov10      ,"lMetCov10/F"); //pf met covariance matrix 10 
   float lMetCov11    = 0; lOTree->Branch("metcov11"   ,&lMetCov11      ,"lMetCov11/F"); //pf met covariance matrix 11 
   //MVAMet covariance matrices
   float lMVACov00    = 0; lOTree->Branch("mvacov00"   ,&lMVACov00      ,"lMVACov00/F"); //mva met covariance matrix 00 
   float lMVACov01    = 0; lOTree->Branch("mvacov01"   ,&lMVACov01      ,"lMVACov01/F"); //mva met covariance matrix 01 
   float lMVACov10    = 0; lOTree->Branch("mvacov10"   ,&lMVACov10      ,"lMVACov10/F"); //mva met covariance matrix 10 
   float lMVACov11    = 0; lOTree->Branch("mvacov11"   ,&lMVACov11      ,"lMVACov11/F"); //mva met covariance matrix 11 

   //First Jet   : leading jet after applying Jet energy corrections (excluding hadronic Tau)
   float lJPt1       = 0; lOTree->Branch("jpt_1"      ,&lJPt1          ,"lJPt1/F"     );//Jet Pt after corrections
   float lJEta1      = 0; lOTree->Branch("jeta_1"     ,&lJEta1         ,"lJEta1/F"    );//Jet Eta
   float lJPhi1      = 0; lOTree->Branch("jphi_1"     ,&lJPhi1         ,"lJPhi1/F"    );//Jet Phi     
   float lJPtRaw1    = 0; lOTree->Branch("jptraw_1"   ,&lJPtRaw1       ,"lJPtRaw1/F"  );//Jet Raw Pt (before corrections)
   float lJPtUnc1    = 0; lOTree->Branch("jptunc_1"   ,&lJPtUnc1       ,"lJPtUnc1/F"  );//Jet Unc (relative to Jet corrected pT)
   float lJMVA1      = 0; lOTree->Branch("jmva_1"     ,&lJMVA1         ,"lJMVA1/F"    );//Jet MVA id value
   bool  lJPass1     = 0; lOTree->Branch("jpass_1"    ,&lJPass1        ,"lJPass1/B"   );//Whether Jet pass PU Id Loose WP

   //Second Jet  : 2nd leading jet (in pt) afer applying Jet energy corrections (excluding Tau)
   float lJPt2       = 0; lOTree->Branch("jpt_2"      ,&lJPt2          ,"lJPt2/F"     );//Jet Pt after corrections
   float lJEta2      = 0; lOTree->Branch("jeta_2"     ,&lJEta2         ,"lJEta2/F"    );//Jet Eta
   float lJPhi2      = 0; lOTree->Branch("jphi_2"     ,&lJPhi2         ,"lJPhi2/F"    );//Jet Phi
   float lJPtRaw2    = 0; lOTree->Branch("jptraw_2"   ,&lJPtRaw2       ,"lJPtRaw2/F"  );//Jet Raw Pt (before corrections)
   float lJPtUnc2    = 0; lOTree->Branch("jptunc_2"   ,&lJPtUnc2       ,"lJPtUnc2/F"  );//Jet Unc (relative to Jet corrected pT)
   float lJMVA2      = 0; lOTree->Branch("jmva_2"     ,&lJMVA2         ,"lJMVA2/F"    );//Jet MVA id value
   bool  lJPass2     = 0; lOTree->Branch("jpass_2"    ,&lJPass2        ,"lJPass2/B"   );//Whether jet passes PU Id Loose WP 
   
   //B Tagged Jet : leading btagged jet (in pt) passing btag wp (pt > 20 + cvs medium)
   float lBTagPt     = 0; lOTree->Branch("bpt"        ,&lBTagPt        ,"lBTagPt/F"   );//Corrected BTag Pt
   float lBTagEta    = 0; lOTree->Branch("beta"       ,&lBTagEta       ,"lBTagEta/F"  );//Btag Eta
   float lBTagPhi    = 0; lOTree->Branch("bphi"       ,&lBTagPhi       ,"lBTagPhi/F"  );//Btag Phi
 
   //Di Jet kinematic variables for VBF selection ==> Two leading pT Jets 
   float lMJJ        = 0; lOTree->Branch("mjj"        ,&lMJJ           ,"lMJJ/F"      );//Mass Di Jet system  
   float lJDEta      = 0; lOTree->Branch("jdeta"      ,&lJDEta         ,"lJDEta/F"    );//|jeta_1-jeta_2| 
   int   lNJetInGap  = 0; lOTree->Branch("njetingap"  ,&lNJetInGap     ,"lNJetInGap/I");//# of Jets between two jets
   float lMVA        = 0; lOTree->Branch("mva"        ,&lMVA           ,"lMVA/F"      );//VBF MVA value
   
   //Variables that go into the VBF MVA
   float lJDPhi      = 0; lOTree->Branch("jdphi"      ,&lJDPhi         ,"lJDPhi/F"    );//Delta Phi between two leading jets
   float lDiJetPt    = 0; lOTree->Branch("dijetpt"    ,&lDiJetPt       ,"lDiJetPt/F"  );//Pt of the di jet system
   float lDiJetPhi   = 0; lOTree->Branch("dijetphi"   ,&lDiJetPhi      ,"lDiJetPhi/F" );//Phi of the di jet system
   float lHDJetPhi   = 0; lOTree->Branch("hdijetphi"  ,&lHDJetPhi      ,"lHDJetPhi/F" );//Phi of the di jet system - Higgs system phi
   float lVisJetEta  = 0; lOTree->Branch("visjeteta"  ,&lVisJetEta     ,"lVisJetEta/F");//TMath::Min(eta_vis - jeta,eta_vis,jeta2);
   float lPtVis      = 0; lOTree->Branch("ptvis"      ,&lPtVis         ,"lPtVis/F"    );//Pt Vis
  
   //number of btags passing btag id ( pt > 20 )
   int   lNBTag      = 0; lOTree->Branch("nbtag"      ,&lNBTag         ,"lNBTag/I");

   //number of jets passing jet id ( pt > 30 )
   int   lNJets      = 0; lOTree->Branch("njets"      ,&lNJets         ,"lNJets/I");
  
   int   lMuFlag     = 0; 
   if(stream.find("MuTau")!=string::npos)
     lOTree->Branch("muFlag"      ,&lMuFlag         ,"muFlag/I");
   else
     lOTree->Branch("elecFlag"    ,&lMuFlag         ,"elecFlag/I");
   int lTriLepVeto = 0;
   lOTree->Branch("triLepVeto"    ,&lTriLepVeto         ,"triLepVeto/I");

   int   lPairIndex   = 0; lOTree->Branch("pairIndex"   ,&lPairIndex      ,"pairIndex/I");
   
   int   lHLTx        = 0; lOTree->Branch("HLTx"        ,&lHLTx ,        "HLTx/I");
   int   lHLTmatch    = 0; lOTree->Branch("HLTmatch"    ,&lHLTmatch ,     "HLTmatch/I");
   float ldiTauCharge = 0; lOTree->Branch("diTauCharge"      ,&ldiTauCharge, "diTauCharge/F");


   float   luPerp    = 0; lOTree->Branch("uPerp"        ,&luPerp,        "uPerp/F");
   float   luParl    = 0; lOTree->Branch("uParl"        ,&luParl,        "uParl/F");
   float   lmetParl    = 0; lOTree->Branch("metParl"        ,&lmetParl,        "metParl/F");
   float   lmetPerp    = 0; lOTree->Branch("metPerp"        ,&lmetPerp,        "metPerp/F");
   float   lmetSigmaParl    = 0; lOTree->Branch("metSigmaParl"        ,&lmetSigmaParl,        "metSigmaParl/F");
   float   lmetSigmaPerp    = 0; lOTree->Branch("metSigmaPerp"        ,&lmetSigmaPerp,        "metSigmaPerp/F");
   float   lmetPullParl    = 0; lOTree->Branch("metPullParl"        ,&lmetPullParl,        "metPullParl/F"); 
   float   lmetPullPerp    = 0; lOTree->Branch("metPullPerp"        ,&lmetPullPerp,        "metPullPerp/F");

   float lembeddedWeight = 0; lOTree->Branch("embeddedWeight" ,&lembeddedWeight,   "embeddedWeight/F");
   /////////////////////////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////////////////////////


   //TFile *file = new TFile(("./batch/nTuple"+sample+"_Open_"+stream+"Stream.root").c_str(),"READ");
   //TFile *file = new TFile(("/data_CMS/cms/anayak/H2TauTauHCP/Moriond/MuTauStream/NtuplesMoriondNewRecoilV1/nTuple"+sample+".root").c_str(),"READ");
   TFile *file = new TFile(("./batch/Sync/nTuple"+sample+".root").c_str(),"READ"); 
   TTree *tree = (TTree*)file->Get("outTreePtOrd");


   int iMuFlag         ; 
   if(stream.find("MuTau")!=string::npos)
     tree->SetBranchAddress("muFlag"        ,&iMuFlag   );
   else
     tree->SetBranchAddress("elecFlag"        ,&iMuFlag   );

   TCut pZ("mt_1<20");
   TCut pZeta("(pzetamiss-1.5*pzetavis) > -20"); 
   TCut OS("diTauCharge==0");
   TCut vbf("jpt_1>30 && jpt_2 > 30 && njetingap<1 && mva>0.50");
   TCut vh("jpt_1>30 && jpt_2 > 30 && mjj>70 && mjj<120 && dijetpt>150 && mva<0.80 && nbtag<1");
   TCut onejet("jpt_1>30 && nbtag<1");
   onejet = onejet && !vbf;
   TCut onejetLow = onejet && TCut("pt_2<40"); 
   TCut onejetHigh = onejet && TCut("pt_2>40");
   TCut bjet("nbtag>0 && njets<2");
   TCut bjetLow = bjet && TCut("pt_2<40");
   TCut bjetHigh = bjet && TCut("pt_2>40");
   TCut zerojet = !vbf && !onejet && !bjet;
   TCut zerojetLow = zerojet && TCut("pt_2<40");
   TCut zerojetHigh = zerojet && TCut("pt_2>40");
   TCut massCat = zerojetLow && TCut("m_sv<80"); 
   TCut massCatUp = zerojetLow && TCut("m_sv_Up<80");
   TCut massCatDown = zerojetLow && TCut("m_sv_Down<80");

   int iPairIndex      ; tree->SetBranchAddress("pairIndex"     ,&iPairIndex);
   //int iPairIndex      ; tree->SetBranchAddress("pairIndexMoriond"     ,&iPairIndex);
   //int   iPairIndex[26]; tree->SetBranchAddress("pairIndex"   ,iPairIndex); 
   //int   iPairIndexOS[6]; tree->SetBranchAddress("pairIndexOS"  ,iPairIndexOS);
   //int   isDiTauOS[6]; tree->SetBranchAddress("isDiTauOS"  ,isDiTauOS);

   float iHLTx          ; tree->SetBranchAddress("HLTx"        ,&iHLTx   );
   float iHLTmatch      ; tree->SetBranchAddress("HLTmatch"    ,&iHLTmatch);

   //Bookeeping
   ULong64_t   iRun         ; tree->SetBranchAddress("run"        ,&iRun   );//Run
   ULong64_t   iLumi        ; tree->SetBranchAddress("lumi"       ,&iLumi    );//Lumi
   ULong64_t   iEvt         ; tree->SetBranchAddress("event"      ,&iEvt      );//Evt

   //Event Variables
   float iNPV         ; tree->SetBranchAddress("numPV"        ,&iNPV       );//NPV
   int   iNPU         ; tree->SetBranchAddress("nPUVertices"  ,&iNPU       );//NPU
   float iRho         ; tree->SetBranchAddress("rhoFastJet"   ,&iRho         );//Rho
   
   //Event Weights
   float iMCWeight1    ; tree->SetBranchAddress("sampleWeight"   ,&iMCWeight1    );//MC Weight (xs/nevents * additional wieght (ie pt weight for gghiggs))
   float iMCWeight2    ; tree->SetBranchAddress("HqTWeight"      ,&iMCWeight2    );//MC Weight (xs/nevents * additional wieght (ie pt weight for gghiggs))
   float iPUWeight    ; tree->SetBranchAddress("puWeight"        ,&iPUWeight    );//Pielup Weight
   //float iPUWeight    ; tree->SetBranchAddress("puWeight2"        ,&iPUWeight    );//Pielup Weight
   //float iEffWeight   ; tree->SetBranchAddress("effweight"  ,&iEffWeight   );//Effieiency Scale factor (all components multiplied in)
   //float iWeight      ; tree->SetBranchAddress("weight"     ,&iWeight      );//mcweight*puweight*effweight

   float iHLTweightTau ; tree->SetBranchAddress("HLTweightTau", &iHLTweightTau);
   float iHLTTau ; tree->SetBranchAddress("HLTTau", &iHLTTau);
   float iSFTau   ; tree->SetBranchAddress("SFTau", &iSFTau);
   float iHLTweightMu, iSFMu, iSFMuID, iSFMuIso;
   float iHLTMu;
   if(stream.find("MuTau")!=string::npos ) {
     tree->SetBranchAddress("HLTweightMu", &iHLTweightMu); 
     tree->SetBranchAddress("HLTMu", &iHLTMu);
     tree->SetBranchAddress("SFMu", &iSFMu);
     tree->SetBranchAddress("SFMuID", &iSFMuID);
     tree->SetBranchAddress("SFMuIso", &iSFMuIso);
   }
   else{ iHLTweightMu  = 0; iSFMu = 0; iSFMuID = 0; iSFMuIso = 0;}
   float iHLTweightElec, iSFElec, iSFElecID, iSFElecIso; 
   float iHLTElec;
   if(stream.find("ElecTau")!=string::npos ) { 
     tree->SetBranchAddress("HLTweightElec", &iHLTweightElec);  
     tree->SetBranchAddress("HLTElec", &iHLTElec);
     tree->SetBranchAddress("SFElec", &iSFElec); 
     tree->SetBranchAddress("SFElecID", &iSFElecID);
     tree->SetBranchAddress("SFElecIso", &iSFElecIso);
   } 
   else{ iHLTweightElec  = 0; iSFElec = 0; iSFElecID = 0; iSFElecIso = 0;}

   //SV Fit variables
   float iMSV         ; tree->SetBranchAddress("diTauNSVfitMass"       ,&iMSV            );//SV Fit using integration method
   float iMSVUp       ; tree->SetBranchAddress("diTauNSVfitMassErrUp"    ,&iMSVUp         );//High Energy scale shape
   float iMSVDown     ; tree->SetBranchAddress("diTauNSVfitMassErrDown"  ,&iMSVDown       );//Low Energy Scale Shape
 
   ///First lepton :  muon for mu Tau, electron for e Tau, electron for e mu, Leading (in pT) Tau for Tau Tau
   float iPt1         ; tree->SetBranchAddress("ptL1"       ,&iPt1            ); //pT 
   float iPhi1        ; tree->SetBranchAddress("phiL1"      ,&iPhi1            ); //Phi 
   float iEta1        ; tree->SetBranchAddress("etaL1"      ,&iEta1           ); //Eta 
   //float iM1          ; tree->SetBranchAddress(""        ,&iM1                ); //Mass 
   float iIso1        ; tree->SetBranchAddress("combRelIsoLeg1DBetav2"      ,&iIso1            ); //Delta Beta iso value 
   float iscEta1;
   if(stream.find("ElecTau")!=string::npos )
     tree->SetBranchAddress("scEtaL1"      ,&iscEta1           ); //supercluster Eta
   
   float iMVA1        ; 
   // if(stream.find("MuTau")!=string::npos )
   tree->SetBranchAddress("isoLeg1MVA"      ,&iMVA1            );//MVA id (when using electron) 0 otherwise
     //else
     //tree->SetBranchAddress("mvaPOGNonTrig"   ,&iMVA1            );//MVA id (when using electron) 0 otherwise
   float inHits       ; tree->SetBranchAddress("nHits"       ,&inHits                );//

   float iD01         ; tree->SetBranchAddress("dxy1"       ,&iD01                );//d0 with respect to primary vertex
   float iDZ1         ; tree->SetBranchAddress("dz1"       ,&iDZ1                );//dZ with respect to primary vertex
   
   int  iPassId1L     ; 
   if(stream.find("MuTau")!=string::npos )
     tree->SetBranchAddress("isPFMuon"   ,  &iPassId1L       );//Whether it passes id  (not necessarily iso)
   else
     tree->SetBranchAddress("isTriggerElectron"   ,  &iPassId1L       );//Whether it passes id  (not necessarily iso)
   int  iPassId1T     ;
   float iPassId1T_F    ;
   if(stream.find("MuTau")!=string::npos )
     tree->SetBranchAddress("isTightMuon"   ,&iPassId1T       );//Whether it passes id  (not necessarily iso)
   else
     tree->SetBranchAddress("mvaPOGNonTrig"   ,&iPassId1T_F       );//Whether it passes id  (not necessarily iso)

   float iMt1         ; tree->SetBranchAddress("MtLeg1MVA"       ,&iMt1              );//mT of  first lepton wrt to MVA met

   ///Second lepton :  hadronic Tau for mu Tau had for e Tau, Muon for e mu, Trailing (in pT)  Tau for Tau Tau
   float iPt2         ; tree->SetBranchAddress("ptL2"       ,&iPt2            );//pT
   float iPhi2        ; tree->SetBranchAddress("phiL2"      ,&iPhi2          );//Phi
   float iEta2        ; tree->SetBranchAddress("etaL2"      ,&iEta2          );//Eta
   float iM2          ; tree->SetBranchAddress("visibleTauMass"        ,&iM2                );//Mass (visible mass for hadronic Tau)
   float iIso2        ; tree->SetBranchAddress("hpsMVA"      ,&iIso2           );//MVA iso for hadronic Tau, Delta Beta for muon
   int iMVA2  = -99, iMVA3 =-99, iMVA4 = -99 ;  
   if(stream.find("ElecTau")!=string::npos) 
     tree->SetBranchAddress("tightestAntiECutWP"      ,&iMVA2           );//MVA id (for anti electron id)
   if(stream.find("ElecTau")!=string::npos)  
     tree->SetBranchAddress("tightestAntiEMVAWP"      ,&iMVA3           );//MVA id (for anti electron id)
   if(stream.find("ElecTau")!=string::npos)   
     tree->SetBranchAddress("tightestAntiEMVA3WP"      ,&iMVA4           );//MVA id (for anti electron id) 

   float ihpsDB3H         ; tree->SetBranchAddress("hpsDB3H"       ,&ihpsDB3H            );
   float ihpsMVA2         ; tree->SetBranchAddress("hpsMVA2"       ,&ihpsMVA2            );
   float iAntiEMVA3raw = -99;
   if(stream.find("ElecTau")!=string::npos)
     tree->SetBranchAddress("AntiEMVA3raw"       ,&iAntiEMVA3raw            );

   int itightestAntiMu2WP         ; tree->SetBranchAddress("tightestAntiMu2WP"       ,&itightestAntiMu2WP    );
   float idiTauNSVfitPt       ; tree->SetBranchAddress("diTauNSVfitPt"       ,&idiTauNSVfitPt          );
   float idiTauSVFitEta       ; tree->SetBranchAddress("diTauSVFitEta"       ,&idiTauSVFitEta          );
   float idiTauSVFitPhi       ; tree->SetBranchAddress("diTauSVFitPhi"       ,&idiTauSVFitPhi          );
     
   int  iPassId2      ; tree->SetBranchAddress("tightestHPSMVAWP"   ,&iPassId2      );//Whether it passes id  (not necessarily iso)
   int iAntiMu        ; tree->SetBranchAddress("tightestAntiMuWP"   ,&iAntiMu      );
   int iAntiMu2       ; tree->SetBranchAddress("tightestAntiMu2WP"   ,&iAntiMu2      );
   
   //bool  lPassIso2    ; tree->SetBranchAddress("passiso_2"  ,&iPassIso2     );//Whether it passes iso (not necessarily id)
   float iMt2         ; tree->SetBranchAddress("MtLeg2MVA"       ,&iMt2             );//mT of 2nd lepton wrt to MVA met
   int  ivetoEvent    ; tree->SetBranchAddress("vetoEventOld"   ,&ivetoEvent      ); //three lepton veto

   //Met related variables
   float iMet         ; tree->SetBranchAddress("MEt"        ,&iMet             ); //pfmet
   float iMetPhi      ; tree->SetBranchAddress("MEtPhi"     ,&iMetPhi          ); //pfmet Phi
   float iMVAMet      ; tree->SetBranchAddress("MEtMVA"     ,&iMVAMet              ); //mvamet
   float iMVAMetPhi   ; tree->SetBranchAddress("MEtMVAPhi"  ,&iMVAMetPhi      ); //mvamet Phi
   float iPZetaVis    ; tree->SetBranchAddress("pZetaVisMVA"   ,&iPZetaVis       ); //pZeta Visible
   float iPZetaMiss   ; tree->SetBranchAddress("pZetaMVA"  ,   &iPZetaMiss    ); //pZeta Missing

   //MET covariance matrices
   float iMetCov00    ; tree->SetBranchAddress("MEtCov00"   ,&iMetCov00     ); //pf met covariance matrix 00 
   float iMetCov01    ; tree->SetBranchAddress("MEtCov01"   ,&iMetCov01      ); //pf met covariance matrix 01 
   float iMetCov10    ; tree->SetBranchAddress("MEtCov10"   ,&iMetCov10     ); //pf met covariance matrix 10 
   float iMetCov11    ; tree->SetBranchAddress("MEtCov11"   ,&iMetCov11    ); //pf met covariance matrix 11 
   //MVAMet covariance matrices
   //float iMVACov00    ; tree->SetBranchAddress("mvacov00"   ,&iMVACov00      ); //mva met covariance matrix 00 
   //float iMVACov01    ; tree->SetBranchAddress("mvacov01"   ,&iMVACov01     ); //mva met covariance matrix 01 
   //float iMVACov10    ; tree->SetBranchAddress("mvacov10"   ,&iMVACov10     ); //mva met covariance matrix 10 
   //float iMVACov11    ; tree->SetBranchAddress("mvacov11"   ,&iMVACov11      ); //mva met covariance matrix 11 

   //First Jet   : leading jet after applying Jet energy corrections (excluding hadronic Tau)
   float iJPt1       ; tree->SetBranchAddress("pt1"      ,&iJPt1            );//Jet Pt after corrections
   float iJEta1      ; tree->SetBranchAddress("eta1"     ,&iJEta1           );//Jet Eta
   float iJPhi1      ; tree->SetBranchAddress("phi1"     ,&iJPhi1          );//Jet Phi     
   //float iJPtRaw1    ; tree->SetBranchAddress("jptraw_1"   ,&iJPtRaw1        );//Jet Raw Pt (before corrections)
   //float iJPtUnc1    ; tree->SetBranchAddress("jptunc_1"   ,&iJPtUnc1        );//Jet Unc (relative to Jet corrected pT)
   float iJMVA1      ; tree->SetBranchAddress("jet1PUMVA"     ,&iJMVA1          );//Jet MVA id value
   float  iJPass1     ; tree->SetBranchAddress("jet1PUWP"      ,&iJPass1         );//Whether Jet pass PU Id Loose WP

   //Second Jet  : 2nd leading jet (in pt) afer applying Jet energy corrections (excluding Tau)
   float iJPt2       ; tree->SetBranchAddress("pt2"      ,&iJPt2              );//Jet Pt after corrections
   float iJEta2      ; tree->SetBranchAddress("eta2"     ,&iJEta2           );//Jet Eta
   float iJPhi2      ; tree->SetBranchAddress("phi2"     ,&iJPhi2             );//Jet Phi
   //float iJPtRaw2    ; tree->SetBranchAddress("jptraw_2"   ,&iJPtRaw2       );//Jet Raw Pt (before corrections)
   //float iJPtUnc2    ; tree->SetBranchAddress("jptunc_2"   ,&iJPtUnc2        );//Jet Unc (relative to Jet corrected pT)
   float iJMVA2      ; tree->SetBranchAddress("jet2PUMVA"     ,&iJMVA2            );//Jet MVA id value
   float  iJPass2     ; tree->SetBranchAddress("jet2PUWP"    ,&iJPass2          );//Whether jet passes PU Id Loose WP 
   
   //B Tagged Jet : leading btagged jet (in pt) passing btag wp (pt > 20 + cvs medium)
   float iBTagPt     ; tree->SetBranchAddress("ptB1"           ,&iBTagPt         );//Corrected BTag Pt
   float iBTagEta    ; tree->SetBranchAddress("etaB1"       ,&iBTagEta        );//Btag Eta
   float iBTagPhi    ; tree->SetBranchAddress("phiB1"       ,&iBTagPhi         );//Btag Phi
 
   //Di Jet kinematic variables for VBF selection ==> Two leading pT Jets 
   float iMJJ        ; tree->SetBranchAddress("Mjj"           ,&iMJJ               );//Mass Di Jet system  
   float iJDEta      ; tree->SetBranchAddress("Deta"          ,&iJDEta            );//|jeta_1-jeta_2| 
   int   iNJetInGap  ; tree->SetBranchAddress("isVetoInJets"  ,&iNJetInGap    );//# of Jets between two jets
   float iptVeto     ; tree->SetBranchAddress("ptVeto"        ,&iptVeto       ); //pt of veto jets
   float iMVA        ; tree->SetBranchAddress("MVAvbf"        ,&iMVA               );//VBF MVA value
   
   //Variables that go into the VBF MVA
   float iJDPhi      ; tree->SetBranchAddress("Dphi"      ,&iJDPhi          );//Delta Phi between two leading jets
   float iDiJetPt    ; tree->SetBranchAddress("diJetPt"    ,&iDiJetPt       );//Pt of the di jet system
   float iDiJetPhi   ; tree->SetBranchAddress("diJetPhi"   ,&iDiJetPhi      );//Phi of the di jet system
   float iHDJetPhi   ; tree->SetBranchAddress("dPhiHjet"  ,&iHDJetPhi      );//Phi of the di jet system - Higgs system phi
   float iVisJetEta  ; tree->SetBranchAddress("c1"  ,      &iVisJetEta    );//TMath::Min(eta_vis - jeta,eta_vis,jeta2);
   float iPtVis      ; tree->SetBranchAddress("c2"        ,&iPtVis           );//Pt Vis
  
   //number of btags passing btag id ( pt > 20 )
   int   iNBTag      ; tree->SetBranchAddress("nJets20BTagged"      ,&iNBTag         );

   //number of jets passing jet id ( pt > 30 )
   int   iNJets      ; tree->SetBranchAddress("nJets30"      ,&iNJets         );
  
   float iDiTauCharge; tree->SetBranchAddress("diTauCharge"      ,&iDiTauCharge         );
   
   float   iuPerp    ; tree->SetBranchAddress("uPerp"        ,&iuPerp); 
   float   iuParl    ; tree->SetBranchAddress("uParl"        ,&iuParl); 
   float   imetParl    ; tree->SetBranchAddress("metParl"        ,&imetParl); 
   float   imetPerp    ; tree->SetBranchAddress("metPerp"        ,&imetPerp); 
   float   imetSigmaParl    ; tree->SetBranchAddress("metSigmaParl"        ,&imetSigmaParl); 
   float   imetSigmaPerp    ; tree->SetBranchAddress("metSigmaPerp"        ,&imetSigmaPerp); 

   float iembeddingWeight = 0; tree->SetBranchAddress("embeddingWeight" ,&iembeddingWeight);
   
   for (Long64_t i0=0; i0<tree->GetEntries();i0++) {
     //if (i0 % 1000 == 0) std::cout << "--- ... Processing event: " << double(i0)/10. << std::endl;

     tree->GetEntry(i0);
     
     //if(iEvt!=15658705 && iEvt!=79045633 && iEvt!=166390474 && iEvt!=217289014 && iEvt!=288718887 && iEvt!=43254671 && iEvt!=4103081 && iEvt!=109554240 && iEvt!=177270849 && iEvt!=20712802 && iEvt!=337831558)continue;

     //std::cout<<"Event     HLTx     HLTmatch    Pt1     Pt2     scEta1     Eta1     Eta2    ElecMVA    VetoFlag    eIso    tauIso     tauAntiE1 tauAntiE2  3rdLepVeto  pairIndex"<<std::endl;
     
     //cout<<iEvt<<"  "<<iHLTx<<"  "<<iHLTmatch<<"  "<<iPt1<<"  "<<iPt2<<"  "<<iscEta1<<"  "<<iEta1<<"  "<<iEta2<<"  "<<iPassId1T_F<<"  "<<iMuFlag<<"  "<<iIso1<<"  "<<ihpsDB3H<<"  "<<iMVA2<<"   "<<iMVA3<<"   "<<ivetoEvent<<"  "<<iPairIndex<<endl;
     //cout<<iEvt<<"  "<<iPt2<<"  "<<iEta2<<"  "<<iPassId2<<endl; 
     
     //cout<<iRun<<"  "<<iLumi<<"  "<<iEvt<<"  "<<iMVAMet<<endl;

     int jPairIndex = iPairIndex; //iPairIndex[5];
     
     if( stream.find("MuTau")!=string::npos   &&   !(iHLTx>0.5 && iHLTmatch>0.5 && iPt1>20 && iPt2>20 && iPassId1L && iPassId1T  && iIso1<0.1 && /*iPassId2>=0*/ ihpsDB3H<1.5 && iAntiMu >= 3 && iMuFlag!=1 && ivetoEvent==0 && jPairIndex<1))
       continue;
     //if( stream.find("ElecTau")!=string::npos &&  !(iHLTx>0.5 && iHLTmatch>0.5 && iPt1>24 && iPt2>20 && iPassId1L && iPassId1T_F>-99 && iIso1<0.1 && iPassId2>=0 && iMuFlag==0 && ivetoEvent==0 && iPairIndex<1 && iMVA2>1 && iDiTauCharge==0))
     if( stream.find("ElecTau")!=string::npos &&  !(iHLTx>0.5 && iHLTmatch>0.5 && iPt1>24 && iPt2>20 && /*iPassId1L &&*/
						     inHits < 0.5 &&
						    ((TMath::Abs(iscEta1)<0.80 && iPassId1T_F>0.925) || (TMath::Abs(iscEta1)<1.479 && TMath::Abs(iscEta1)>0.80 && iPassId1T_F>0.975) || (TMath::Abs(iscEta1)>1.479 && iPassId1T_F>0.985)) && iIso1<0.1 && /*iPassId2>=0*/ ihpsDB3H<1.5 && iAntiMu >= 1 && iMuFlag!=1 && ivetoEvent==0 && jPairIndex<1 && /*iMVA2>1 &&(iMVA3>4 || iMVA3 == 3)*/ iMVA4>2))
       continue;

     if(incl && !(iDiTauCharge == 0 && iMt1 < 20)) continue; //inclusive selection

     //cout<<iRun<<"  "<<iLumi<<"  "<<iEvt<<endl;
     //cout << iMVA2 << endl;

     lPairIndex  = jPairIndex;
     lMuFlag     = iMuFlag;
     lTriLepVeto = ivetoEvent;

     lHLTx       = int(iHLTx);
     lHLTmatch   = int(iHLTmatch);

     //Bookeeping
     lRun         = iRun ;
     lLumi        = iLumi;
     lEvt         = iEvt;

     //Event Variables
     lNPV         = int(iNPV);
     lNPU         = iNPU;
     lRho         = iRho;
   
     //Event Weights
     lMCWeight    = iMCWeight1*iMCWeight2 ;
     lPUWeight    = iPUWeight;
     if(sample.find("Emb") != string::npos){
       lEffWeight   = stream.find("MuTau")!=string::npos ? iHLTTau*iHLTMu*iSFMuID : iHLTTau*iHLTElec*iSFElecID;
     }
     else
       lEffWeight   = stream.find("MuTau")!=string::npos ? iHLTweightTau*iHLTweightMu*iSFTau*iSFMu : iHLTweightTau*iHLTweightElec*iSFTau*iSFElec;
     lWeight      = lMCWeight*lPUWeight*lEffWeight;
     lTrigweight_1 = stream.find("MuTau")!=string::npos ? iHLTweightMu : iHLTweightElec;
     lTrigweight_2 = iHLTweightTau;
     lIdweight_1  = stream.find("MuTau")!=string::npos ? iSFMuID : iSFElecID;
     lIsoweight_1  = stream.find("MuTau")!=string::npos ? iSFMuIso : iSFElecIso;
     lIdweight_2  = iSFTau;
     lIsoweight_2 = iSFTau;
     /*
     if(lEffWeight > 0.94 && lEffWeight < 0.96){
       cout<<iEvt<<" * "<<iPt1<<" * "<<iPt2<<" * "<<iEta1<<" * "<<iEta2<<" * "<<iHLTweightElec<<" * "<<iSFElecID<<" * "<<iSFElecIso<<" * "<<iHLTweightTau<<endl;
     }
     */
     //SV Fit variables
     lMSV         = iMSV;
     lMSVUp       = iMSVUp;
     lMSVDown     = iMSVDown;
 
     lPtSV        = idiTauNSVfitPt;
     lEtaSV       = idiTauSVFitEta;
     lPhiSV       = idiTauSVFitPhi;
     
     ///First lepton :  muon for mu Tau, electron for e Tau, electron for e mu, Leading (in pT) Tau for Tau Tau
     lPt1         = iPt1;
     lPhi1        = iPhi1;
     lEta1        = iEta1;
     lM1          = -99;
     lIso1        = iIso1;
     lMVA1        = iPassId1T_F;//iMVA1;
     lD01         = iD01;
     lDZ1         = iDZ1;
     lPassId1     = stream.find("MuTau")!=string::npos ? (iPassId1L && iPassId1T) : (iPassId1L && ((TMath::Abs(iEta1)<0.80 && iPassId1T_F>0.925) || (TMath::Abs(iEta1)<1.479 && TMath::Abs(iEta1)>0.80 && iPassId1T_F>0.975) || (TMath::Abs(iEta1)>1.479 && iPassId1T_F>0.985)));
     lPassIso1    = iIso1<0.1;
     lMt1         = iMt1;

     ///Second lepton :  hadronic Tau for mu Tau had for e Tau, Muon for e mu, Trailing (in pT)  Tau for Tau Tau
     lPt2         = iPt2;
     lPhi2        = iPhi2;
     lEta2        = iEta2;
     lM2          = iM2;
     lIso2        = iIso2;
     lMVA2        = iMVA2;
     lPassId2     = iPassId2>=0;
     lPassIso2    = iPassId2>=0;
     lMt2         = iMt2;
   
     //Tau variables
     lbyCombinedIsolationDeltaBetaCorrRaw3Hits = ihpsDB3H;
     lagainstElectronMVA3raw = iAntiEMVA3raw;
     lbyIsolationMVA2raw = ihpsMVA2;
     lagainstMuonLoose2 = (itightestAntiMu2WP > 0) ? 1.0 : 0.;
     lagainstMuonMedium2 = (itightestAntiMu2WP > 1) ? 1.0 : 0.;
     lagainstMuonTight2 = (itightestAntiMu2WP > 2) ? 1.0 : 0.;

     ldiTauCharge = iDiTauCharge;
     //Met related variables
     lMet         = iMet;
     lMetPhi      = iMetPhi;
     lMVAMet      = iMVAMet;
     lMVAMetPhi   = iMVAMetPhi;
     lPZetaVis    = iPZetaVis;
     lPZetaMiss   = iPZetaMiss-iPZetaVis;
     //MET covariance matrices
     lMetCov00   = iMetCov00;
     lMetCov01   = iMetCov01;
     lMetCov10   = iMetCov10;
     lMetCov11   = iMetCov11;
     //MVAMet covariance matrices
     lMVACov00   = iMetCov00;
     lMVACov01   = iMetCov01;
     lMVACov10   = iMetCov10;
     lMVACov11   = iMetCov11;


     //First Jet   : leading jet after applying Jet energy corrections (excluding hadronic Tau)
     lJPt1       = iJPt1;
     lJEta1      = iJEta1;
     lJPhi1      = iJPhi1;
     lJPtRaw1    = -99;
     lJPtUnc1    = -99;
     lJMVA1      = iJMVA1;
     lJPass1     = iJPass1>0.5;

     //Second Jet  : 2nd leading jet (in pt) afer applying Jet energy corrections (excluding Tau)
     lJPt2       = iJPt2;
     lJEta2      = iJEta2;
     lJPhi2      = iJPhi2;
     lJPtRaw2    = -99;
     lJPtUnc2    = -99;
     lJMVA2      = iJMVA2;
     lJPass2     = iJPass2>0.5;
   
     //B Tagged Jet : leading btagged jet (in pt) passing btag wp (pt > 20 + cvs medium)
     lBTagPt     = iBTagPt;
     lBTagEta    = iBTagEta;
     lBTagPhi    = iBTagPhi;
 
     //Di Jet kinematic variables for VBF selection ==> Two leading pT Jets 
     lMJJ        = iMJJ;
     lJDEta      = iJDEta;
     lNJetInGap  = (iptVeto > 30) ? iNJetInGap : 0;
     lMVA        = iMVA;
   
     //Variables that go o the VBF MVA
     lJDPhi      = iJDPhi;
     lDiJetPt    = iDiJetPt;
     lDiJetPhi   = iDiJetPhi;
     lHDJetPhi   = iHDJetPhi;
     lVisJetEta  = iVisJetEta;
     lPtVis      = iPtVis;
  
     //number of btags passing btag id ( pt > 20 )
     lNBTag      = iNBTag;

     //number of jets passing jet id ( pt > 30 )
     lNJets      = iNJets;
  

     luPerp    = iuPerp;
     luParl    = iuParl;
     lmetParl    = imetParl;
     lmetPerp    = imetPerp;
     lmetSigmaParl    = imetSigmaParl;
     lmetSigmaPerp    = imetSigmaPerp;
     lmetPullParl    = (imetSigmaParl > 0) ? imetParl/imetSigmaParl : -999.; 
     lmetPullPerp    = (imetSigmaPerp > 0) ? imetPerp/imetSigmaPerp : -999.; 

     lembeddedWeight = iembeddingWeight;
     lOTree->Fill();
   }  
   /*
   //get yields 
   TH1F* hm = new TH1F("hm","", 20, 0., 400.); 
   cout<<" 0jet low "<<endl;
   hm->Reset(); 
   lOTree->Draw("m_sv>>hm", (zerojetLow&&pZ&&OS)); 
   cout<<"entries "<<hm->GetEntries()<<" Integral "<<hm->Integral()<<endl; 
   cout<<" 0jet high "<<endl; 
   hm->Reset();  
   lOTree->Draw("m_sv>>hm", (zerojetHigh&&pZ&&OS));  
   cout<<"entries "<<hm->GetEntries()<<" Integral "<<hm->Integral()<<endl;
   cout<<" 1jet low "<<endl;  
   hm->Reset();   
   lOTree->Draw("m_sv>>hm", (onejetLow&&pZ&&OS));   
   cout<<"entries "<<hm->GetEntries()<<" Integral "<<hm->Integral()<<endl;
   cout<<" 1jet high "<<endl;  
   hm->Reset();   
   lOTree->Draw("m_sv>>hm", (onejetHigh&&pZ&&OS));   
   cout<<"entries "<<hm->GetEntries()<<" Integral "<<hm->Integral()<<endl;
   cout<<" VBF "<<endl;  
   hm->Reset();   
   lOTree->Draw("m_sv>>hm", (vbf&&pZ&&OS));   
   cout<<"entries "<<hm->GetEntries()<<" Integral "<<hm->Integral()<<endl;
   cout<<" bTag low "<<endl;   
   hm->Reset();    
   lOTree->Draw("m_sv>>hm", (bjetLow&&pZeta&&OS));    
   cout<<"entries "<<hm->GetEntries()<<" Integral "<<hm->Integral()<<endl; 
   cout<<" bTag High "<<endl;    
   hm->Reset();     
   lOTree->Draw("m_sv>>hm", (bjetHigh&&pZeta&&OS));     
   cout<<"entries "<<hm->GetEntries()<<" Integral "<<hm->Integral()<<endl;
   cout<<" mass cat "<<endl;    
   hm->Reset();     
   lOTree->Draw("m_sv>>hm", (massCat&&pZ&&OS));     
   cout<<"entries "<<hm->GetEntries()<<" Integral "<<hm->Integral()<<endl;
   cout<<" mass cat Up"<<endl;     
   hm->Reset();      
   lOTree->Draw("m_sv>>hm", (massCatUp&&pZ&&OS));      
   cout<<"entries "<<hm->GetEntries()<<" Integral "<<hm->Integral()<<endl;
   cout<<" mass cat Down"<<endl;     
   hm->Reset();      
   lOTree->Draw("m_sv>>hm", (massCatDown&&pZ&&OS));      
   cout<<"entries "<<hm->GetEntries()<<" Integral "<<hm->Integral()<<endl;
   */
   lOFile->cd();
   lOTree->Write();
   lOFile->Close();

   file->Close();

}

void synchNtupleAll()
{
  synchNtuple("VBF_HToTauTau_M-125_Open_MuTauStream", "MuTau");
  //synchNtuple("GluGluToHToTauTau_M-125_Open_MuTauStream", "MuTau");
  //synchNtuple("WH_ZH_TTH_HToTauTau_M-125_Open_MuTauStream", "MuTau");

  //synchNtuple("_Data_2012ABCD_MuTau", "MuTau");
  //synchNtuple("DYJets-MuTau-50-madgraph-PUS10_run_Open_MuTauStream", "MuTau");
  //synchNtuple("_DYJ_JetToTau_Open_MuTauStream", "MuTau");
  //synchNtuple("_DYJ_MuToTau_Open_MuTauStream", "MuTau");
  //synchNtuple("_DYJ_TauTau_Open_MuTauStream", "MuTau");
  //synchNtuple("_Embedded_2012ABCD_MuTau", "MuTau");
  //synchNtuple("Others-MuTau-madgraph-PUS10_run_Open_MuTauStream", "MuTau");
  //synchNtuple("WJetsAllBins-MuTau-madgraph-PUS10_run_Open_MuTauStream", "MuTau");
  //synchNtuple("WJets-MuTau-madgraph-PUS10_run_Open_MuTauStream", "MuTau");
  //synchNtuple("TTJets-MuTau-madgraph-PUS10_run_Open_MuTauStream", "MuTau");
}

