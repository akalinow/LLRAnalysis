import FWCore.ParameterSet.Config as cms

process = cms.Process("ELECTAUANA")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

runOnMC     = True
doSVFitReco = True

if runOnMC:
    print "Running on MC"
else:
    print "Running on Data"


if runOnMC:
    process.GlobalTag.globaltag = cms.string('START42_V13::All')
else:
    process.GlobalTag.globaltag = cms.string('GR_R_42_V19::All')
    
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 500
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/GluGluToHToTauTau_M-120_7TeV-powheg-pythia6/ElecTauStream-A/7844d4f37a96a2c29702b4cab23898d9/patTuples_ElecTauStream_1_1_L9Q.root'
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/GluGluToHToTauTau_M-120_7TeV-powheg-pythia6/ElecTauStream-13Oct2011/4858ffc00b76ea327a878ab2c9d1d4f3/patTuples_ElecTauStream_1_1_MHb.root'

    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/GluGluToHToTauTau_M-120_7TeV-powheg-pythia6/ElecTauStream-21Oct2011/72336b8f61b87f1536c6b95a5cfbfe6e/patTuples_ElecTauStream_1_1_qHA.root'
    'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/TauPlusX/ElecTauStream-21Oct2011-05AugReReco/ea0534a53ae8342d77d517a552111b33/patTuples_ElecTauStream_18_1_L7T.root'

    #'rfio:rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/TauPlusX/ElecTauStream-v6/add82882179501750b106d9900e51989/patTuples_ElecTauStream_45_1_UZL.root',
    #'file:patTuples_ElecTauStream.root'
    )
    )

process.allEventsFilter = cms.EDFilter(
    "AllEventsFilter"
    )

###################################################################################
process.rescaledMET = cms.EDProducer(
    "MEtRescalerProducer",
    metTag = cms.InputTag("patMETsPFlow"),
    jetTag = cms.InputTag("selectedPatJets"),
    electronTag = cms.InputTag("elecPtEtaIDIso"),
    muonTag = cms.InputTag(""),
    tauTag = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
    unClusterShift = cms.double(0.10),
    tauShift = cms.double(0.03),
    muonShift = cms.double(0.01),
    electronShift = cms.double(0.02),
    jetThreshold  = cms.double(10),
    verbose = cms.bool(False)
    )

process.rescaledMETjet = process.rescaledMET.clone(
    unClusterShift = cms.double(0.10),
    tauShift = cms.double(0.0),
    muonShift = cms.double(0.0),
    electronShift = cms.double(0.0),
    )
process.rescaledMETtau = process.rescaledMET.clone(
    unClusterShift = cms.double(0.0),
    tauShift = cms.double(0.03),
    muonShift = cms.double(0.0),
    electronShift = cms.double(0.0),
    )
process.rescaledMETelectron = process.rescaledMET.clone(
    unClusterShift = cms.double(0.0),
    tauShift = cms.double(0.0),
    muonShift = cms.double(0.0),
    electronShift = cms.double(0.02),
    )

process.rescaledTaus = cms.EDProducer(
    "TauRescalerProducer",
    inputCollection = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
    shift = cms.vdouble(0.03,0.03)
    )
process.rescaledElectrons = cms.EDProducer(
    "ElectronRescalerProducer",
    inputCollection = cms.InputTag("elecPtEtaIDIso"),
    shift = cms.vdouble(0.01,0.025)
    )
process.rescaledElectronsRel = cms.EDProducer(
    "ElectronRescalerProducer",
    inputCollection = cms.InputTag("elecPtEtaRelID"),
    shift = cms.vdouble(0.01,0.025)
    )

process.rescaledObjects = cms.Sequence(
    process.rescaledMETjet+
    process.rescaledMETtau+
    process.rescaledMETelectron+
    process.rescaledTaus+
    process.rescaledElectrons+
    process.rescaledElectronsRel
    )
###################################################################################

process.load("LLRAnalysis.Utilities.diTausReconstruction_cff")
process.diTau = process.allElecTauPairs.clone()
process.diTau.srcLeg1  = cms.InputTag("elecPtEtaIDIso")
process.diTau.srcLeg2  = cms.InputTag("tauPtEtaIDAgMuAgElecIso")
process.diTau.srcMET   = cms.InputTag("patMETsPFlow")
process.diTau.dRmin12  = cms.double(0.5)
process.diTau.doSVreco = cms.bool(doSVFitReco)

if not runOnMC:
    process.diTau.srcGenParticles = ""
        
process.selectedDiTau = cms.EDFilter(
    "ElecTauPairSelector",
    src = cms.InputTag("diTau"),
    cut = cms.string("dR12>0.5")
    )
process.selectedDiTauCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("selectedDiTau"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )

#######################################################################

process.diTauJetUp =  process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                          srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
                                          srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                          srcMET  = cms.InputTag("rescaledMETjet",  "UNNNU")
                                          )
process.selectedDiTauJetUp = process.selectedDiTau.clone(src = cms.InputTag("diTauJetUp") )
process.selectedDiTauJetUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauJetUp"))

process.diTauJetDown =  process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                            srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
                                            srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                            srcMET  = cms.InputTag("rescaledMETjet",  "DNNND")
                                            )
process.selectedDiTauJetDown = process.selectedDiTau.clone(src = cms.InputTag("diTauJetDown") )
process.selectedDiTauJetDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauJetDown"))


process.diTauElecUp = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                          srcLeg1 = cms.InputTag("rescaledElectrons","U"),
                                          srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                          srcMET  = cms.InputTag("rescaledMETelectron","NUNNN")
                                          )
process.selectedDiTauElecUp = process.selectedDiTau.clone(src = cms.InputTag("diTauElecUp") )
process.selectedDiTauElecUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauElecUp"))

process.diTauElecDown = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                            srcLeg1 = cms.InputTag("rescaledElectrons","D"),
                                            srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                            srcMET  = cms.InputTag("rescaledMETelectron","NDNNN")
                                            )
process.selectedDiTauElecDown = process.selectedDiTau.clone(src = cms.InputTag("diTauElecDown") )
process.selectedDiTauElecDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauElecDown"))


process.diTauTauUp = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                         srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
                                         srcLeg2 = cms.InputTag("rescaledTaus", "U"),
                                         srcMET  = cms.InputTag("rescaledMETtau","NNNUN")
                                         )
process.selectedDiTauTauUp = process.selectedDiTau.clone(src = cms.InputTag("diTauTauUp") )
process.selectedDiTauTauUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauTauUp"))

process.diTauTauDown = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                           srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
                                           srcLeg2 = cms.InputTag("rescaledTaus", "D"),
                                           srcMET  = cms.InputTag("rescaledMETtau","NNNDN")
                                           )
process.selectedDiTauTauDown = process.selectedDiTau.clone(src = cms.InputTag("diTauTauDown") )
process.selectedDiTauTauDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauTauDown"))


process.allDiTau = cms.Sequence(
    (process.diTau*process.selectedDiTau*process.selectedDiTauCounter)+
    (process.diTauJetUp*process.selectedDiTauJetUp*process.selectedDiTauJetUpCounter +
     process.diTauJetDown*process.selectedDiTauJetDown*process.selectedDiTauJetDownCounter) +
    (process.diTauElecUp*process.selectedDiTauElecUp*process.selectedDiTauElecUpCounter +
     process.diTauElecDown*process.selectedDiTauElecDown*process.selectedDiTauElecDownCounter) +
    (process.diTauTauUp*process.selectedDiTauTauUp*process.selectedDiTauTauUpCounter +
     process.diTauTauDown*process.selectedDiTauTauDown*process.selectedDiTauTauDownCounter)
    )
#######################################################################

MVA = "((pt<=20 && abs(superClusterPosition.Eta)>=0.0 && abs(superClusterPosition.Eta)<1.0 && userFloat('mva')>0.133) ||" + \
      " (pt<=20 && abs(superClusterPosition.Eta)>=1.0 && abs(superClusterPosition.Eta)<1.5 && userFloat('mva')>0.465) ||" + \
      " (pt<=20 && abs(superClusterPosition.Eta)>=1.5 && abs(superClusterPosition.Eta)<2.5 && userFloat('mva')>0.518) ||" + \
      " (pt>20  && abs(superClusterPosition.Eta)>=0.0 && abs(superClusterPosition.Eta)<1.0 && userFloat('mva')>0.942) ||" + \
      " (pt>20  && abs(superClusterPosition.Eta)>=1.0 && abs(superClusterPosition.Eta)<1.5 && userFloat('mva')>0.947) ||" + \
      " (pt>20  && abs(superClusterPosition.Eta)>=1.5 && abs(superClusterPosition.Eta)<2.5 && userFloat('mva')>0.878) )"

simpleCutsWP95 = "(userFloat('nHits')<=1"+ \
                 " && (" + \
                 " (isEB && userFloat('sihih')<0.010 && userFloat('dPhi')<0.80 && "+ \
                 "          userFloat('dEta') <0.007 && userFloat('HoE') <0.15)"   + \
                 " || "  + \
                 " (isEE && userFloat('sihih')<0.030 && userFloat('dPhi')<0.70 && "+ \
                 "          userFloat('dEta') <0.010 && userFloat('HoE') <0.07)"   + \
                 "     )"+ \
                 ")"
simpleCutsWP80 = "(userFloat('nHits')==0 && userInt('antiConv')>0.5 "+ \
                 " && ("   + \
                 " (pt>=20 && ("    + \
                 "               (isEB && userFloat('sihih')<0.010 && userFloat('dPhi')<0.06 && "  + \
                 "                        userFloat('dEta')< 0.004 && userFloat('HoE') <0.04)"     + \
                 "               ||"+ \
                 "               (isEE && userFloat('sihih')<0.030 && userFloat('dPhi')<0.030 && " + \
                 "                        userFloat('dEta') <0.007 && userFloat('HoE') <0.025) )) "+ \
                 "     || "+ \
                 " (pt<20 && (fbrem>0.15 || (abs(superClusterPosition.Eta)<1. && eSuperClusterOverP>0.95) ) && ( "+ \
                 "               (isEB && userFloat('sihih')<0.010 && userFloat('dPhi')<0.030 && " + \
                 "                        userFloat('dEta') <0.004 && userFloat('HoE') <0.025) "   + \
                 "               ||"+ \
                 "               (isEE && userFloat('sihih')<0.030 && userFloat('dPhi')<0.020 &&"  + \
                 "                        userFloat('dEta') <0.005 && userFloat('HoE') <0.025) ))" + \
                 "    )"   + \
                 ")"


process.tauPtEtaIDAgMuAgElecIso  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    cut = cms.string("tauID('byLooseCombinedIsolationDeltaBetaCorr')>0.5 && pt>20 && abs(eta)<2.3"),
    filter = cms.bool(False)
    )
process.tauPtEtaIDAgMuAgElecIsoCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )
process.tauPtEtaIDAgMuAgElecIsoTauUp  =  process.tauPtEtaIDAgMuAgElecIso.clone(
    src = cms.InputTag("rescaledTaus", "U")
    )
process.tauPtEtaIDAgMuAgElecIsoTauUpCounter = process.tauPtEtaIDAgMuAgElecIsoCounter.clone(
    src = cms.InputTag("tauPtEtaIDAgMuAgElecIsoTauUp"),
    )
process.tauPtEtaIDAgMuAgElecIsoTauDown  =  process.tauPtEtaIDAgMuAgElecIso.clone(
    src = cms.InputTag("rescaledTaus", "D")
    )
process.tauPtEtaIDAgMuAgElecIsoTauDownCounter = process.tauPtEtaIDAgMuAgElecIsoCounter.clone(
    src = cms.InputTag("tauPtEtaIDAgMuAgElecIsoTauDown"),
    )


process.elecPtEtaIDIso  = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("elecPtEtaID"),
    cut = cms.string("userFloat('PFRelIsoDB04v3')<0.10 && pt>15 && abs(eta)<2.5 && ("+MVA+" || "+simpleCutsWP80+")"),
    filter = cms.bool(False)
    )
process.elecPtEtaIDIsoCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("elecPtEtaIDIso"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )
process.elecPtEtaIDIsoElecUp = process.elecPtEtaIDIso.clone(
    src = cms.InputTag("rescaledElectrons","U")
    )
process.elecPtEtaIDIsoElecUpCounter = process.elecPtEtaIDIsoCounter.clone(
    src = cms.InputTag("elecPtEtaIDIsoElecUp"),
    )
process.elecPtEtaIDIsoElecDown = process.elecPtEtaIDIso.clone(
    src = cms.InputTag("rescaledElectrons","D")
    )
process.elecPtEtaIDIsoElecDownCounter = process.elecPtEtaIDIsoCounter.clone(
    src = cms.InputTag("elecPtEtaIDIsoElecDown"),
    )
process.elecPtEtaRelID = process.elecPtEtaIDIso.clone(
    src = cms.InputTag("elecPtEtaRelID"),
    cut = cms.string("pt>15")
    )
process.elecPtEtaRelIDElecUp   = process.elecPtEtaRelID.clone(
    src = cms.InputTag("rescaledElectronsRel","U")
    )
process.elecPtEtaRelIDElecDown = process.elecPtEtaRelID.clone(
    src = cms.InputTag("rescaledElectronsRel","D")
    )

process.filterSequence = cms.Sequence(
    (process.tauPtEtaIDAgMuAgElecIso       * process.tauPtEtaIDAgMuAgElecIsoCounter) +
    (process.tauPtEtaIDAgMuAgElecIsoTauUp  * process.tauPtEtaIDAgMuAgElecIsoTauUpCounter) +
    (process.tauPtEtaIDAgMuAgElecIsoTauDown* process.tauPtEtaIDAgMuAgElecIsoTauDownCounter) +
    (process.elecPtEtaIDIso                * process.elecPtEtaIDIsoCounter) +
    (process.elecPtEtaIDIsoElecUp          * process.elecPtEtaIDIsoElecUpCounter) +
    (process.elecPtEtaIDIsoElecDown        * process.elecPtEtaIDIsoElecDownCounter) +
    (process.elecPtEtaRelID+process.elecPtEtaRelIDElecUp+process.elecPtEtaRelIDElecDown)
    )

#######################################################################


process.elecTauStreamAnalyzer = cms.EDAnalyzer(
    "ElecTauStreamAnalyzer",
    diTaus         = cms.InputTag("selectedDiTau"),
    jets           = cms.InputTag("selectedPatJets"),
    newJets        = cms.InputTag(""),
    met            = cms.InputTag("patMETsPFlow"),
    rawMet         = cms.InputTag("patMETsPFlow"),
    electrons      = cms.InputTag("elecPtEtaID"),
    electronsRel   = cms.InputTag("elecPtEtaRelID"),
    vertices       = cms.InputTag("selectedPrimaryVertices"),
    triggerResults = cms.InputTag("patTriggerEvent"),
    isMC           = cms.bool(runOnMC),
    deltaRLegJet   = cms.untracked.double(0.5),
    minCorrPt      = cms.untracked.double(15.),
    minJetID       = cms.untracked.double(0.5), # 1=loose,2=medium,3=tight
    verbose        = cms.untracked.bool( False ),
    )
process.elecTauStreamAnalyzerJetUp     = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauJetUp"),
    met    =  cms.InputTag("rescaledMETjet",  "UNNNU"),
    )
process.elecTauStreamAnalyzerJetDown   = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauJetDown"),
    met    =  cms.InputTag("rescaledMETjet",  "DNNND"),
    )
process.elecTauStreamAnalyzerElecUp    = process.elecTauStreamAnalyzer.clone(
    diTaus       =  cms.InputTag("selectedDiTauElecUp"),
    met          =  cms.InputTag("rescaledMETelectron","NUNNN"),
    electrons    =  cms.InputTag("elecPtEtaIDIsoElecUp"),
    electronsRel =  cms.InputTag("elecPtEtaRelIDElecUp"),
    )
process.elecTauStreamAnalyzerElecDown  = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauElecDown"),
    met    =  cms.InputTag("rescaledMETelectron","NDNNN"),
    electrons    =  cms.InputTag("elecPtEtaIDIsoElecDown"),
    electronsRel =  cms.InputTag("elecPtEtaRelIDElecDown"),
    )
process.elecTauStreamAnalyzerTauUp     = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauTauUp"),
    met    =  cms.InputTag("rescaledMETtau","NNNUN")
    )
process.elecTauStreamAnalyzerTauDown   = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauTauDown"),
    met    =  cms.InputTag("rescaledMETtau","NNNDN")
    )

process.allAnalyzers = cms.Sequence(
    process.elecTauStreamAnalyzer+
    process.elecTauStreamAnalyzerJetUp+
    process.elecTauStreamAnalyzerJetDown+
    process.elecTauStreamAnalyzerElecUp+
    process.elecTauStreamAnalyzerElecDown+
    process.elecTauStreamAnalyzerTauUp+
    process.elecTauStreamAnalyzerTauDown
    )
#######################################################################

process.analysis = cms.Sequence(
    process.allEventsFilter*
    process.filterSequence*
    process.rescaledObjects*
    process.allDiTau*
    process.allAnalyzers
    )

process.pNominal = cms.Path(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.rescaledObjects*
    process.diTau*process.selectedDiTau*process.selectedDiTauCounter*
    process.elecTauStreamAnalyzer
    )
process.pJetUp = cms.Path(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.rescaledObjects*
    process.diTauJetUp*process.selectedDiTauJetUp*process.selectedDiTauJetUpCounter*
    process.elecTauStreamAnalyzerJetUp
    )
process.pJetDown = cms.Path(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.rescaledObjects*
    process.diTauJetDown*process.selectedDiTauJetDown*process.selectedDiTauJetDownCounter*
    process.elecTauStreamAnalyzerJetDown
    )
process.pElecUp = cms.Path(
    process.allEventsFilter*
    process.elecPtEtaIDIso *
    process.elecPtEtaRelID *
    (process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    process.rescaledObjects*
    (process.elecPtEtaIDIsoElecUp*process.elecPtEtaIDIsoElecUpCounter) *
    process.elecPtEtaRelIDElecUp *
    process.diTauElecUp*process.selectedDiTauElecUp*process.selectedDiTauElecUpCounter*
    process.elecTauStreamAnalyzerElecUp
    )
process.pElecDown = cms.Path(
    process.allEventsFilter*
    process.elecPtEtaIDIso *
    process.elecPtEtaRelID *
    (process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    process.rescaledObjects*
    (process.elecPtEtaIDIsoElecDown*process.elecPtEtaIDIsoElecDownCounter) *
    process.elecPtEtaRelIDElecDown *
    process.diTauElecDown*process.selectedDiTauElecDown*process.selectedDiTauElecDownCounter*
    process.elecTauStreamAnalyzerElecDown
    )
process.pTauUp = cms.Path(
    process.allEventsFilter*
    (process.elecPtEtaIDIso*process.elecPtEtaIDIsoCounter) *
    process.tauPtEtaIDAgMuAgElecIso*
    process.elecPtEtaRelID *
    process.rescaledObjects*
    (process.tauPtEtaIDAgMuAgElecIsoTauUp*process.tauPtEtaIDAgMuAgElecIsoTauUpCounter)*
    process.diTauTauUp*process.selectedDiTauTauUp*process.selectedDiTauTauUpCounter*
    process.elecTauStreamAnalyzerTauUp
    )
process.pTauDown = cms.Path(
    process.allEventsFilter*
    (process.elecPtEtaIDIso*process.elecPtEtaIDIsoCounter) *
    process.tauPtEtaIDAgMuAgElecIso*
    process.elecPtEtaRelID *
    process.rescaledObjects*
    (process.tauPtEtaIDAgMuAgElecIsoTauDown*process.tauPtEtaIDAgMuAgElecIsoTauDownCounter)*
    process.diTauTauDown*process.selectedDiTauTauDown*process.selectedDiTauTauDownCounter*
    process.elecTauStreamAnalyzerTauDown
    )


process.out = cms.OutputModule(
    "PoolOutputModule",
    outputCommands = cms.untracked.vstring( 'drop *'),
    fileName = cms.untracked.string('patTuplesSkimmed_ElecTauStream.root'),
    )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("treeElecTauStream.root")
                                   )

process.outpath = cms.EndPath()
