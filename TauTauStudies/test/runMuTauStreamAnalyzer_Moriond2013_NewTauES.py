import FWCore.ParameterSet.Config as cms

process = cms.Process("MUTAUANA")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
#process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

#from Configuration.PyReleaseValidation.autoCond import autoCond
#process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

runOnMC     = True
doSVFitReco = True
usePFMEtMVA = True
useRecoil   = True

if runOnMC:
    print "Running on MC"
else:
    print "Running on Data"
        
if runOnMC:
    process.GlobalTag.globaltag = cms.string('START53_V18::All')
else:
    process.GlobalTag.globaltag = cms.string('GR_P_V41_AN1::All')
    
    
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola/MuTauStream-04May2012-Reload_DYJets-MuTau-50-madgraph-PUS6_skim/f2017a8682c2724bef5e6ba529285334/patTuples_MuTauStream_9_1_CS7.root'
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/TauPlusX/MuTauStream-04May2012-Reload-05AugReReco/d7ab9a49aa7555b45f2fd6a9510b15e8/patTuples_MuTauStream_9_2_bFX.root'
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/DoubleMu/MuTauStream-04May2012-Reload-RunBPromptReco-v1-Embedded/f5416ddeffc52a24fc875c17bf3889c0/patTuples_MuTauStream_8_3_C7R.root'
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/VBF_HToTauTau_M-130_7TeV-powheg-pythia6-tauola/MuTauStream-04May2012-Reload_VBFH130-MuTau-powheg-PUS6_skim/f2017a8682c2724bef5e6ba529285334/patTuples_MuTauStream_1_1_QFt.root',
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/VBF_HToTauTau_M-130_7TeV-powheg-pythia6-tauola/MuTauStream-04May2012-Reload_VBFH130-MuTau-powheg-PUS6_skim/f2017a8682c2724bef5e6ba529285334/patTuples_MuTauStream_2_1_QDr.root',
    #'rfio:/dpm/in2p3.fr/home/cms/trivcat/store/user/bianchi/VBF_HToTauTau_M-130_7TeV-powheg-pythia6-tauola/MuTauStream-04May2012-Reload_VBFH130-MuTau-powheg-PUS6_skim/f2017a8682c2724bef5e6ba529285334/patTuples_MuTauStream_3_1_IXG.root',
    #'file:./patTuples_MuTauStream.root'
    'file:/afs/cern.ch/user/m/mbluj/public/forChristian/patTuples_LepTauStream.root'
    #'root://polgrid4.in2p3.fr//dpm/in2p3.fr/home/cms/trivcat/store/user/mbluj/VBF_HToTauTau_M-125_8TeV-powheg-pythia6/LepTauStream-07Dec2012_VBFH125-LepTau-powheg-PUS10_pat/fbab02682d6b416ae6da687406f89be0/patTuples_LepTauStream_100_1_PYQ.root'
    #'root://polgrid4.in2p3.fr//dpm/in2p3.fr/home/cms/trivcat/store/user/ndaci/VBF_HToTauTau_M-125_8TeV-powheg-pythia6/HTT_PAT_MuTau_Oct02_MC_VBF_HToTauTau_M-125/1580e1ad29bd5f6cf6006f65f956c13c/patTuples_MuTauStream_9_8_FTF.root'
    )
    )
#process.source.skipEvents = cms.untracked.uint32(90)

#process.source.eventsToProcess = cms.untracked.VEventRange(
#    '1:69216'
#    )

process.allEventsFilter = cms.EDFilter(
    "AllEventsFilter"
    )

###################################################################################

#----------------------------------------------------------------------------------
# produce MVA MET

process.load("JetMETCorrections.METPUSubtraction.mvaPFMET_cff")

if runOnMC:
    process.calibratedAK5PFJetsForPFMEtMVA.correctors = cms.vstring("ak5PFL1FastL2L3")
else:
    process.calibratedAK5PFJetsForPFMEtMVA.correctors = cms.vstring("ak5PFL1FastL2L3Residual")
    
process.pfMEtMVA.srcLeptons = cms.VInputTag( cms.InputTag('muPtEtaRelIDRelIso'), cms.InputTag('elecPtEtaRelIDRelIso'), cms.InputTag('tauPtEtaIDAgMuAgElecRelIso') )
# use the following for the most isolated tau
#process.pfMEtMVA.srcLeptons = cms.VInputTag( cms.InputTag('muPtEtaRelIDRelIso'), cms.InputTag('elecPtEtaRelIDRelIso'), cms.InputTag('UserIsoTaus') )

#process.pfMEtMVA.srcVertices = cms.InputTag("selectedPrimaryVertices")
process.pfMEtMVA.srcVertices = cms.InputTag("offlinePrimaryVertices")

process.load("PhysicsTools.PatAlgos.producersLayer1.metProducer_cfi")
process.patPFMetByMVA = process.patMETs.clone(
    metSource = cms.InputTag('pfMEtMVA'),
    addMuonCorrections = cms.bool(False),
    genMETSource = cms.InputTag('genMetTrue'),
    addGenMET = cms.bool(False)
)
#----------------------------------------------------------------------------------

#----------------------------------------------------------------------------------
# produce No-PU MET

process.load("JetMETCorrections.METPUSubtraction.noPileUpPFMET_cff")

doSmearJets = None
if runOnMC:
    doSmearJets = True
else:
    doSmearJets = False

from PhysicsTools.PatUtils.tools.runNoPileUpMEtUncertainties import runNoPileUpMEtUncertainties
runNoPileUpMEtUncertainties(
    process,
    electronCollection = cms.InputTag('elecPtEtaRelIDRelIso'),
    photonCollection = '',
    muonCollection = cms.InputTag('muPtEtaRelIDRelIso'),
    tauCollection = cms.InputTag('tauPtEtaIDAgMuAgElecRelIso'),
    doApplyChargedHadronSubtraction = False,
    doSmearJets = doSmearJets,
    jecUncertaintyTag = "SubTotalMC",
    addToPatDefaultSequence = False
)

if runOnMC:
    process.calibratedAK5PFJetsForNoPileUpPFMEt.correctors = cms.vstring("ak5PFL1FastL2L3")
else:
    process.calibratedAK5PFJetsForNoPileUpPFMEt.correctors = cms.vstring("ak5PFL1FastL2L3Residual")

process.producePFMEtNoPileUp = cms.Sequence(process.pfNoPileUpMEtUncertaintySequence)
#----------------------------------------------------------------------------------

#----------------------------------------------------------------------------------
# produce Type-1 corrected PFMET

process.load("PhysicsTools.PatUtils.patPFMETCorrections_cff")

doSmearJets = None
if runOnMC:
    doSmearJets = True
else:
    doSmearJets = False

process.load("JetMETCorrections.Type1MET.pfMETsysShiftCorrections_cfi")
sysShiftCorrParameter = None
if runOnMC:
    sysShiftCorrParameter = process.pfMEtSysShiftCorrParameters_2012runABCvsNvtx_mc
else:
    sysShiftCorrParameter = process.pfMEtSysShiftCorrParameters_2012runABCvsNvtx_data

from PhysicsTools.PatUtils.tools.runType1PFMEtUncertainties import runType1PFMEtUncertainties
runType1PFMEtUncertainties(
    process,
    electronCollection = cms.InputTag('elecPtEtaRelIDRelIso'),
    photonCollection = '',
    muonCollection = cms.InputTag('muPtEtaRelIDRelIso'),
    tauCollection = cms.InputTag('tauPtEtaIDAgMuAgElecRelIso'),
    jetCollection = cms.InputTag('selectedPatJets'),        
    doSmearJets = doSmearJets,
    jecUncertaintyTag = "SubTotalMC",
    makeType1corrPFMEt = True,
    makeType1p2corrPFMEt = False,
    doApplyType0corr = True,
    sysShiftCorrParameter = sysShiftCorrParameter,
    doApplySysShiftCorr = True,
    addToPatDefaultSequence = False
)

if runOnMC:
    process.patPFJetMETtype1p2Corr.jetCorrLabel = cms.string("L3Absolute")
else:
    # CV: apply data/MC residual correction to "unclustered energy"
    process.calibratedPFCandidates = cms.EDProducer("PFCandResidualCorrProducer",
        src = cms.InputTag('particleFlow'),
        residualCorrLabel = cms.string("ak5PFResidual"),
        residualCorrEtaMax = cms.double(9.9),
        extraCorrFactor = cms.double(1.05)
    )                                                        
    process.pfCandidateToVertexAssociation.PFCandidateCollection = cms.InputTag('calibratedPFCandidates')
    process.patPFJetMETtype1p2Corr.type2ResidualCorrLabel = cms.string("ak5PFResidual")
    process.patPFJetMETtype1p2Corr.type2ResidualCorrEtaMax = cms.double(9.9)
    process.pfCandMETresidualCorr = process.pfCandMETcorr.clone(                
        residualCorrLabel = cms.string("ak5PFResidual"),
        residualCorrEtaMax = cms.double(9.9),
        residualCorrOffset = cms.double(1.),
        extraCorrFactor = cms.double(1.05)
    )
    process.producePatPFMETCorrections.replace(process.pfCandMETcorr, process.pfCandMETcorr + process.pfCandMETresidualCorr)
    process.patType1CorrectedPFMet.srcType1Corrections.append(cms.InputTag('pfCandMETresidualCorr'))

process.produceType1corrPFMEt = cms.Sequence(process.pfType1MEtUncertaintySequence)    
#----------------------------------------------------------------------------------

#----------------------------------------------------------------------------------
# produce CaloMEtNoHF (MC corrected by data/MC difference in CaloMET response)

# CV: TO BE IMPLEMENTED <- FIXME
#----------------------------------------------------------------------------------

#----------------------------------------------------------------------------------
# produce PU Jet Ids

from RecoJets.JetProducers.PileupJetID_cfi import pileupJetIdProducer, stdalgos_5x, stdalgos_4x, stdalgos, cutbased, chsalgos_5x, chsalgos_4x, chsalgos
process.puJetId = pileupJetIdProducer.clone(
    produceJetIds = cms.bool(True),
    jetids = cms.InputTag(""),
    runMvas = cms.bool(False),
    jets = cms.InputTag("ak5PFJets"),
    vertexes = cms.InputTag("offlinePrimaryVertices"),
    algos = cms.VPSet(cutbased)
)
process.puJetMva = process.puJetId.clone(
    produceJetIds = cms.bool(False),
    runMvas = cms.bool(True),
    algos = stdalgos
)
process.puJetIdSequence = cms.Sequence(process.puJetId + process.puJetMva)
#----------------------------------------------------------------------------------

process.puJetIdAndMvaMet = cms.Sequence(process.puJetIdSequence *
                                        (process.pfMEtMVAsequence*process.patPFMetByMVA))

###################################################################################
process.muPtEtaRelIDRelIso = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("muonsForVeto"),#muPtEtaRelID?//muons for veto?
    cut = cms.string("pt>10 && abs(eta)<2.4"+
                     " && isTrackerMuon"),
    filter = cms.bool(False)
    )

process.elecPtEtaRelIDRelIso = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("electronsForVeto"),
    cut = cms.string("pt>10 && abs(eta)<2.4"+
                     " && ( (abs(eta)<0.80 && userFloat('mvaPOGNonTrig')>0.925) || (abs(eta)<1.479 && abs(eta)>0.80 && userFloat('mvaPOGNonTrig')>0.975) || (abs(eta)>1.479 && userFloat('mvaPOGNonTrig')>0.985)  )"+
                     " && gsfTrack.trackerExpectedHitsInner.numberOfHits == 0"),
    filter = cms.bool(False)
    )

process.tauPtEtaIDAgMuAgElecRelIso  = cms.EDFilter( #Selection as like Phil's
    "PATTauSelector",
    #src = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    src = cms.InputTag("tauPtEtaIDAgMuAgElecScaled"),
    cut = cms.string("pt>19 && abs(eta)<2.3"+
                     " && tauID('byIsolationMVAraw')>0.7"),
    filter = cms.bool(False)
    )
process.UserIsoTaus = cms.EDProducer( #Most isolated tau
    "UserIsolatedPatTau",
    #patTauTag = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    patTauTag = cms.InputTag("tauPtEtaIDAgMuAgElecScaled"),
    isMC = cms.bool(runOnMC),
    verbose = cms.untracked.bool(False),
    useIsoMVA = cms.untracked.bool(True)
    )

process.LeptonsForMVAMEt = cms.Sequence(process.muPtEtaRelIDRelIso*
                                        process.elecPtEtaRelIDRelIso*
                                        process.tauPtEtaIDAgMuAgElecRelIso
                                        #*process.UserIsoTaus # uncomment to use most isolated tau
                                        )

###################################################################################

process.rescaledMET = cms.EDProducer(
    "MEtRescalerProducer",
    metTag         = cms.InputTag("metRecoilCorrector",  "N"),
    jetTag         = cms.InputTag("selectedPatJets"),
    electronTag    = cms.InputTag(""),
    muonTag        = cms.InputTag("muPtEtaIDIso"),
    tauTag         = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
    unClusterShift = cms.double(0.10),
    tauShift       = cms.vdouble(0.03,0.03),
    muonShift      = cms.vdouble(0.01,0.01),
    electronShift  = cms.vdouble(0.01,0.025),
    jetThreshold   = cms.double(10),
    numOfSigmas    = cms.double(1.0),
    verbose = cms.bool(False)
    )

if usePFMEtMVA:
    if useRecoil :
        process.rescaledMET.metTag = cms.InputTag("metRecoilCorrector",  "N")
    else :
        process.rescaledMET.metTag = cms.InputTag("patPFMetByMVA")
process.rescaledMETRaw = process.rescaledMET.clone( metTag = cms.InputTag("metRecoilCorrector",  "N") )


process.rescaledMETjet = process.rescaledMET.clone(
    unClusterShift = cms.double(0.10),
    tauShift       = cms.vdouble(0.0,0.0),
    muonShift      = cms.vdouble(0.0,0.0),
    electronShift  = cms.vdouble(0.0,0.0),
    )
process.rescaledMETRawjet = process.rescaledMETjet.clone( metTag = cms.InputTag("metRecoilCorrector",  "N") )

process.rescaledMETtau = process.rescaledMET.clone(
    unClusterShift = cms.double(0.0),
    tauShift       = cms.vdouble(0.03,0.03),
    muonShift      = cms.vdouble(0.0,0.0),
    electronShift  = cms.vdouble(0.0,0.0),
    )
process.rescaledMETRawtau = process.rescaledMETtau.clone( metTag = cms.InputTag("metRecoilCorrector",  "N") )

process.rescaledMETmuon = process.rescaledMET.clone(
    unClusterShift = cms.double(0.0),
    tauShift       = cms.vdouble(0.0,0.0),
    muonShift      = cms.vdouble(0.01,0.01),
    electronShift  = cms.vdouble(0.0,0.0),
    )
process.rescaledMETRawmuon = process.rescaledMETmuon.clone( metTag = cms.InputTag("metRecoilCorrector",  "N") )

process.rescaledTaus = cms.EDProducer(
    "TauRescalerProducer",
    inputCollection = cms.InputTag("tauPtEtaIDAgMuAgElecIsoPtRel"),
    shift           = cms.vdouble(0.03,0.03),
    numOfSigmas     = cms.double(1.0),
    #verbose         = cms.bool(True)
    )
process.rescaledMuons = cms.EDProducer(
    "MuonRescalerProducer",
    inputCollection = cms.InputTag("muPtEtaIDIsoPtRel"),
    shift           = cms.vdouble(0.01,0.01),
    numOfSigmas     = cms.double(1.0),
    #verbose         = cms.bool(True)
    )
process.rescaledMuonsRel = cms.EDProducer(
    "MuonRescalerProducer",
    inputCollection = cms.InputTag("muPtEtaRelID"),
    shift           = cms.vdouble(0.01,0.01),
    numOfSigmas     = cms.double(1.0),
    #verbose         = cms.bool(True)
    )

process.rescaledObjects = cms.Sequence(
    process.rescaledMETjet+
    process.rescaledMETtau+
    process.rescaledMETmuon+
    process.rescaledMETRawjet+
    process.rescaledMETRawtau+
    process.rescaledMETRawmuon+
    process.rescaledTaus+
    process.rescaledMuons+
    process.rescaledMuonsRel
    )

###################################################################################

process.metRecoilCorrector = cms.EDProducer(
    "MEtRecoilCorrectorProducer",
    genParticleTag      = cms.InputTag("genParticles"),
    jetTag              = cms.InputTag("selectedPatJets"),
    metTag              = cms.InputTag("patMETsPFlow"),
    electronTag         = cms.InputTag(""),
    muonTag             = cms.InputTag("muPtEtaIDIso"),
    tauTag              = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
    inputFileNamezmm42X = cms.FileInPath("LLRAnalysis/Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_zmm53X_2012_njet.root"),
    inputFileNamedatamm = cms.FileInPath("LLRAnalysis/Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_datamm53X_2012_njet.root"),
    inputFileNamewjets  = cms.FileInPath("LLRAnalysis/Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_wjets53X_20pv_njet.root"),
    inputFileNamezjets  = cms.FileInPath("LLRAnalysis/Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_zmm53X_2012_njet.root"),
    inputFileNamehiggs  = cms.FileInPath("LLRAnalysis/Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_higgs53X_20pv_njet.root"),
    numOfSigmas         = cms.double(1.0),
    minJetPt            = cms.double(30.0),
    verbose             = cms.bool(True),
    isMC                = cms.bool(runOnMC),
    )

if usePFMEtMVA and useRecoil :
    process.metRecoilCorrector.metTag = cms.InputTag("patPFMetByMVA")
        
###################################################################################

process.load("LLRAnalysis.Utilities.diTausReconstruction_cff")
process.diTau = process.allMuTauPairs.clone()
process.diTau.srcLeg1  = cms.InputTag("muPtEtaIDIso")
process.diTau.srcLeg2  = cms.InputTag("tauPtEtaIDAgMuAgElecIso")
process.diTau.srcMET   = cms.InputTag("metRecoilCorrector",  "N")
process.diTau.dRmin12  = cms.double(0.5)
process.diTau.doSVreco = cms.bool(doSVFitReco)
if usePFMEtMVA:
    if useRecoil :
        process.diTau.srcMET = cms.InputTag("metRecoilCorrector",  "N")
    else :
        process.diTau.srcMET = cms.InputTag("patPFMetByMVA")


if not runOnMC:
    process.diTau.srcGenParticles = ""
        
process.pfMEtMVACov = cms.EDProducer(
    "PFMEtSignCovMatrixUnembedder",
    src = cms.InputTag("patPFMetByMVA")
    )

if usePFMEtMVA:
    if useRecoil :
        process.diTau.nSVfit.psKine_MEt_logM_fit.config.event.srcMEt = cms.InputTag("metRecoilCorrector", "N")
        process.diTau.nSVfit.psKine_MEt_logM_fit.config.event.likelihoodFunctions[0].srcMEtCovMatrix = cms.InputTag("pfMEtMVACov")
        process.diTau.nSVfit.psKine_MEt_logM_int.config.event.srcMEt = cms.InputTag("metRecoilCorrector", "N")
        process.diTau.nSVfit.psKine_MEt_logM_int.config.event.likelihoodFunctions[0].srcMEtCovMatrix = cms.InputTag("pfMEtMVACov")
    else :
        process.diTau.nSVfit.psKine_MEt_logM_fit.config.event.srcMEt = cms.InputTag("patPFMetByMVA")
        process.diTau.nSVfit.psKine_MEt_logM_fit.config.event.likelihoodFunctions[0].srcMEtCovMatrix = cms.InputTag("pfMEtMVACov")
        process.diTau.nSVfit.psKine_MEt_logM_int.config.event.srcMEt = cms.InputTag("patPFMetByMVA")
        process.diTau.nSVfit.psKine_MEt_logM_int.config.event.likelihoodFunctions[0].srcMEtCovMatrix = cms.InputTag("pfMEtMVACov")

process.selectedDiTau = cms.EDFilter(
    "MuTauPairSelector",
    src = cms.InputTag("diTau"),
    cut = cms.string("dR12>0.5")
    )
process.selectedDiTauCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("selectedDiTau"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )



process.diTauRaw = process.allMuTauPairs.clone()
process.diTauRaw.srcLeg1  = cms.InputTag("muPtEtaIDIso")
process.diTauRaw.srcLeg2  = cms.InputTag("tauPtEtaIDAgMuAgElecIso")
process.diTauRaw.srcMET   = cms.InputTag("metRecoilCorrector",  "N")
process.diTauRaw.dRmin12  = cms.double(0.5)
process.diTauRaw.doSVreco = cms.bool(doSVFitReco)

if not runOnMC:
    process.diTauRaw.srcGenParticles = ""
        
process.diTauRaw.nSVfit.psKine_MEt_logM_fit.config.event.srcMEt = cms.InputTag("metRecoilCorrector",  "N")
process.diTauRaw.nSVfit.psKine_MEt_logM_int.config.event.srcMEt = cms.InputTag("metRecoilCorrector",  "N")

process.selectedDiTauRaw =  process.selectedDiTau.clone(   src = cms.InputTag("diTauRaw") )
process.selectedDiTauRawCounter =  process.selectedDiTauCounter.clone(   src = cms.InputTag("selectedDiTauRaw") )


#######################################################################
#######################################################################


process.diTauJetUp =  process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                          srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                          srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                          srcMET  = cms.InputTag("rescaledMETjet",  "UNNNU")
                                          )
process.pfMEtMVACovJetUp   = process.pfMEtMVACov.clone(src = cms.InputTag("rescaledMETjet",  "UNNNU"))
process.selectedDiTauJetUp = process.selectedDiTau.clone(src = cms.InputTag("diTauJetUp") )
process.selectedDiTauJetUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauJetUp"))

process.diTauRawJetUp                = process.diTauRaw.clone(doSVreco = cms.bool(doSVFitReco),
                                                              srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                                              srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                                              srcMET  = cms.InputTag("rescaledMETRawjet",  "UNNNU")
                                                              )
process.selectedDiTauRawJetUp        = process.selectedDiTauRaw.clone( src = cms.InputTag("diTauRawJetUp") )
process.selectedDiTauRawJetUpCounter = process.selectedDiTauRawCounter.clone(src =  cms.InputTag("selectedDiTauRawJetUp"))


#######################################################################


process.diTauJetDown =  process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                            srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                            srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                            srcMET  = cms.InputTag("rescaledMETjet",  "DNNND")
                                            )
process.pfMEtMVACovJetDown   = process.pfMEtMVACov.clone(src = cms.InputTag("rescaledMETjet",  "DNNND"))
process.selectedDiTauJetDown = process.selectedDiTau.clone(src = cms.InputTag("diTauJetDown") )
process.selectedDiTauJetDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauJetDown"))

process.diTauRawJetDown                = process.diTauRaw.clone(doSVreco = cms.bool(doSVFitReco),
                                                                srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                                                srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                                                srcMET  = cms.InputTag("rescaledMETRawjet",  "DNNND")
                                                                )
process.selectedDiTauRawJetDown        = process.selectedDiTauRaw.clone( src = cms.InputTag("diTauRawJetDown") )
process.selectedDiTauRawJetDownCounter = process.selectedDiTauRawCounter.clone(src =  cms.InputTag("selectedDiTauRawJetDown"))

#######################################################################

process.diTauMEtResponseUp =  process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                                  srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                                  srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                                  srcMET  = cms.InputTag("metRecoilCorrector",  "ResponseU")
                                                  )
process.selectedDiTauMEtResponseUp = process.selectedDiTau.clone(src = cms.InputTag("diTauMEtResponseUp") )
process.selectedDiTauMEtResponseUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauMEtResponseUp"))

process.diTauRawMEtResponseUp                = process.diTauMEtResponseUp.clone(  srcMET  = cms.InputTag("metRecoilCorrector",  "ResponseU") )
process.selectedDiTauRawMEtResponseUp        = process.selectedDiTauMEtResponseUp.clone( src = cms.InputTag("diTauRawMEtResponseUp") )
process.selectedDiTauRawMEtResponseUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauRawMEtResponseUp"))

#######################################################################


process.diTauMEtResponseDown =  process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                            srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                            srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                            srcMET  = cms.InputTag("metRecoilCorrector",  "ResponseD")
                                            )
process.selectedDiTauMEtResponseDown = process.selectedDiTau.clone(src = cms.InputTag("diTauMEtResponseDown") )
process.selectedDiTauMEtResponseDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauMEtResponseDown"))

process.diTauRawMEtResponseDown                = process.diTauMEtResponseDown.clone(  srcMET  = cms.InputTag("metRecoilCorrector",  "ResponseU") )
process.selectedDiTauRawMEtResponseDown        = process.selectedDiTauMEtResponseDown.clone( src = cms.InputTag("diTauRawMEtResponseDown") )
process.selectedDiTauRawMEtResponseDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauRawMEtResponseDown"))

#######################################################################


process.diTauMEtResolutionUp =  process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                          srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                          srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                          srcMET  = cms.InputTag("metRecoilCorrector",  "ResolutionU")
                                          )
process.selectedDiTauMEtResolutionUp = process.selectedDiTau.clone(src = cms.InputTag("diTauMEtResolutionUp") )
process.selectedDiTauMEtResolutionUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauMEtResolutionUp"))

process.diTauRawMEtResolutionUp                = process.diTauMEtResolutionUp.clone(  srcMET  = cms.InputTag("metRecoilCorrector",  "ResolutionU") )
process.selectedDiTauRawMEtResolutionUp        = process.selectedDiTauMEtResolutionUp.clone( src = cms.InputTag("diTauRawMEtResolutionUp") )
process.selectedDiTauRawMEtResolutionUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauRawMEtResolutionUp"))

#######################################################################

process.diTauMEtResolutionDown =  process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                            srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                            srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                            srcMET  = cms.InputTag("metRecoilCorrector",  "ResolutionD")
                                            )
process.selectedDiTauMEtResolutionDown = process.selectedDiTau.clone(src = cms.InputTag("diTauMEtResolutionDown") )
process.selectedDiTauMEtResolutionDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauMEtResolutionDown"))

process.diTauRawMEtResolutionDown                = process.diTauMEtResolutionDown.clone(  srcMET  = cms.InputTag("metRecoilCorrector",  "ResolutionU") )
process.selectedDiTauRawMEtResolutionDown        = process.selectedDiTauMEtResolutionDown.clone( src = cms.InputTag("diTauRawMEtResolutionDown") )
process.selectedDiTauRawMEtResolutionDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauRawMEtResolutionDown"))


#######################################################################

process.diTauMuUp = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                        srcLeg1 = cms.InputTag("rescaledMuons","U"),
                                        srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                        srcMET  = cms.InputTag("rescaledMETmuon","NNUNN")
                                        )
process.pfMEtMVACovMuUp   = process.pfMEtMVACov.clone(src = cms.InputTag("rescaledMETmuon",  "NNUNN"))
process.selectedDiTauMuUp = process.selectedDiTau.clone(src = cms.InputTag("diTauMuUp") )
process.selectedDiTauMuUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauMuUp"))

process.diTauRawMuUp                = process.diTauMuUp.clone(  srcMET  = cms.InputTag("rescaledMETRawmuon",  "NNUNN") )
process.selectedDiTauRawMuUp        = process.selectedDiTauMuUp.clone( src = cms.InputTag("diTauRawMuUp") )
process.selectedDiTauRawMuUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauRawMuUp"))

#######################################################################

process.diTauMuDown = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                          srcLeg1 = cms.InputTag("rescaledMuons","D"),
                                          srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                          srcMET  = cms.InputTag("rescaledMETmuon","NNDNN")
                                          )
process.pfMEtMVACovMuDown   = process.pfMEtMVACov.clone(src = cms.InputTag("rescaledMETmuon",  "NNDNN"))
process.selectedDiTauMuDown = process.selectedDiTau.clone(src = cms.InputTag("diTauMuDown") )
process.selectedDiTauMuDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauMuDown"))

process.diTauRawMuDown                = process.diTauMuDown.clone(  srcMET  = cms.InputTag("rescaledMETRawmuon",  "NNDNN") )
process.selectedDiTauRawMuDown        = process.selectedDiTauMuDown.clone( src = cms.InputTag("diTauRawMuDown") )
process.selectedDiTauRawMuDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauRawMuDown"))

#######################################################################

process.diTauTauUp = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                         srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                         srcLeg2 = cms.InputTag("rescaledTaus", "U"),
                                         srcMET  = cms.InputTag("rescaledMETtau","NNNUN")
                                         )
process.pfMEtMVACovTauUp   = process.pfMEtMVACov.clone(src = cms.InputTag("rescaledMETtau",  "NNNUN"))
process.selectedDiTauTauUp = process.selectedDiTau.clone(src = cms.InputTag("diTauTauUp") )
process.selectedDiTauTauUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauTauUp"))

process.diTauRawTauUp                = process.diTauRaw.clone(doSVreco = cms.bool(doSVFitReco),
                                                              srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                                              srcLeg2 = cms.InputTag("rescaledTaus", "U"),
                                                              srcMET  = cms.InputTag("rescaledMETRawtau","NNNUN")
                                                              )
process.selectedDiTauRawTauUp        = process.selectedDiTauRaw.clone( src = cms.InputTag("diTauRawTauUp") )
process.selectedDiTauRawTauUpCounter = process.selectedDiTauRawCounter.clone(src =  cms.InputTag("selectedDiTauRawTauUp"))

#######################################################################

process.diTauTauDown = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                           srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                           srcLeg2 = cms.InputTag("rescaledTaus", "D"),
                                           srcMET  = cms.InputTag("rescaledMETtau","NNNDN")
                                           )
process.pfMEtMVACovTauDown   = process.pfMEtMVACov.clone(src = cms.InputTag("rescaledMETtau",  "NNNDN"))
process.selectedDiTauTauDown = process.selectedDiTau.clone(src = cms.InputTag("diTauTauDown") )
process.selectedDiTauTauDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauTauDown"))

process.diTauRawTauDown                =  process.diTauRaw.clone(doSVreco = cms.bool(doSVFitReco),
                                                                 srcLeg1 = cms.InputTag("muPtEtaIDIso"),
                                                                 srcLeg2 = cms.InputTag("rescaledTaus", "D"),
                                                                 srcMET  = cms.InputTag("rescaledMETRawtau","NNNDN")
                                                                 )
process.selectedDiTauRawTauDown        = process.selectedDiTauRaw.clone( src = cms.InputTag("diTauRawTauDown") )
process.selectedDiTauRawTauDownCounter = process.selectedDiTauRawCounter.clone(src =  cms.InputTag("selectedDiTauRawTauDown"))

#######################################################################

process.allDiTau = cms.Sequence(
    (process.diTau*process.selectedDiTau*process.selectedDiTauCounter)+
    (process.diTauJetUp*process.selectedDiTauJetUp*process.selectedDiTauJetUpCounter +
     process.diTauJetDown*process.selectedDiTauJetDown*process.selectedDiTauJetDownCounter) +
    (process.diTauMEtResolutionUp*process.selectedDiTauMEtResolutionUp*process.selectedDiTauMEtResolutionUpCounter +
     process.diTauMEtResolutionDown*process.selectedDiTauMEtResolutionDown*process.selectedDiTauMEtResolutionDownCounter) +
    (process.diTauMEtResponseUp*process.selectedDiTauMEtResponseUp*process.selectedDiTauMEtResponseUpCounter +
     process.diTauMEtResponseDown*process.selectedDiTauMEtResponseDown*process.selectedDiTauMEtResponseDownCounter) +
    (process.diTauMuUp*process.selectedDiTauMuUp*process.selectedDiTauMuUpCounter +
     process.diTauMuDown*process.selectedDiTauMuDown*process.selectedDiTauMuDownCounter) +
    (process.diTauTauUp*process.selectedDiTauTauUp*process.selectedDiTauTauUpCounter +
     process.diTauTauDown*process.selectedDiTauTauDown*process.selectedDiTauTauDownCounter)+
     (process.diTauRaw*process.selectedDiTauRaw*process.selectedDiTauRawCounter)+
    (process.diTauRawJetUp*process.selectedDiTauRawJetUp*process.selectedDiTauRawJetUpCounter +
     process.diTauRawJetDown*process.selectedDiTauRawJetDown*process.selectedDiTauRawJetDownCounter) +
    (process.diTauRawMEtResolutionUp*process.selectedDiTauRawMEtResolutionUp*process.selectedDiTauRawMEtResolutionUpCounter +
     process.diTauRawMEtResolutionDown*process.selectedDiTauRawMEtResolutionDown*process.selectedDiTauRawMEtResolutionDownCounter) +
    (process.diTauRawMEtResponseUp*process.selectedDiTauRawMEtResponseUp*process.selectedDiTauRawMEtResponseUpCounter +
     process.diTauRawMEtResponseDown*process.selectedDiTauRawMEtResponseDown*process.selectedDiTauRawMEtResponseDownCounter) +
    (process.diTauRawMuUp*process.selectedDiTauRawMuUp*process.selectedDiTauRawMuUpCounter +
     process.diTauRawMuDown*process.selectedDiTauRawMuDown*process.selectedDiTauRawMuDownCounter) +
    (process.diTauRawTauUp*process.selectedDiTauRawTauUp*process.selectedDiTauRawTauUpCounter +
     process.diTauRawTauDown*process.selectedDiTauRawTauDown*process.selectedDiTauRawTauDownCounter)
    )

if usePFMEtMVA:
    process.allDiTau.replace(process.diTau,process.pfMEtMVACov*process.diTau)
    process.allDiTau.replace(process.diTauJetUp,process.pfMEtMVACovJetUp*process.diTauJetUp)
    process.allDiTau.replace(process.diTauJetDown,process.pfMEtMVACovJetDown*process.diTauJetDown)
    process.allDiTau.replace(process.diTauMuUp,process.pfMEtMVACovMuUp*process.diTauMuUp)
    process.allDiTau.replace(process.diTauMuDown,process.pfMEtMVACovMuDown*process.diTauMuDown)
    process.allDiTau.replace(process.diTauTauUp,process.pfMEtMVACovTauUp*process.diTauTauUp)
    process.allDiTau.replace(process.diTauTauDown,process.pfMEtMVACovTauDown*process.diTauTauDown)

#######################################################################

process.tauPtEtaIDAgMuAgElecScaled = cms.EDProducer(
    "TauESCorrector",
    tauTag = cms.InputTag("tauPtEtaIDAgMuAgElec")
    #verbose         = cms.bool(True)
    )

process.tauPtEtaIDAgMuAgElecIso  = cms.EDFilter(
    "PATTauSelector",
    #src = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    src = cms.InputTag("tauPtEtaIDAgMuAgElecScaled"),
    cut = cms.string("pt>20 && abs(eta)<2.3"+
                     " && tauID('byLooseIsolationMVA')>-0.5"
                     ),
    filter = cms.bool(False)
    )
process.tauPtEtaIDAgMuAgElecIsoPtRel  = cms.EDFilter(
    "PATTauSelector",
    #src = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    src = cms.InputTag("tauPtEtaIDAgMuAgElecScaled"),
    cut = cms.string("pt>19 && abs(eta)<2.3"+
                     " && tauID('byLooseIsolationMVA')>-0.5"
                     ),
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


process.muPtEtaIDIso  = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("muPtEtaID"),
    cut = cms.string("userFloat('PFRelIsoDB04v2')<0.50 && pt>20 && abs(eta)<2.1"),
    filter = cms.bool(False)
    )
process.muPtEtaIDIsoPtRel  = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("muPtEtaID"),
    cut = cms.string("userFloat('PFRelIsoDB04v2')<0.50 && pt>19 && abs(eta)<2.1"),
    filter = cms.bool(False)
    )

process.muPtEtaIDIsoCounter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("muPtEtaIDIso"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    )
process.muPtEtaIDIsoMuUp = process.muPtEtaIDIso.clone(
    src = cms.InputTag("rescaledMuons","U")
    )
process.muPtEtaIDIsoMuUpCounter = process.muPtEtaIDIsoCounter.clone(
    src = cms.InputTag("muPtEtaIDIsoMuUp"),
    )
process.muPtEtaIDIsoMuDown = process.muPtEtaIDIso.clone(
    src = cms.InputTag("rescaledMuons","D")
    )
process.muPtEtaIDIsoMuDownCounter = process.muPtEtaIDIsoCounter.clone(
    src = cms.InputTag("muPtEtaIDIsoMuDown"),
    )
process.muPtEtaRelID = process.muPtEtaIDIso.clone(
    src = cms.InputTag("muPtEtaRelID"),
    cut = cms.string("pt>15")
    )
process.muPtEtaRelIDMuUp   = process.muPtEtaRelID.clone(
    src = cms.InputTag("rescaledMuonsRel","U")
    )
process.muPtEtaRelIDMuDown = process.muPtEtaRelID.clone(
    src = cms.InputTag("rescaledMuonsRel","D")
    )

process.filterSequence = cms.Sequence(
    (process.tauPtEtaIDAgMuAgElecIso       * process.tauPtEtaIDAgMuAgElecIsoCounter) +
    (process.tauPtEtaIDAgMuAgElecIsoTauUp  * process.tauPtEtaIDAgMuAgElecIsoTauUpCounter) +
    (process.tauPtEtaIDAgMuAgElecIsoTauDown* process.tauPtEtaIDAgMuAgElecIsoTauDownCounter) +
    (process.muPtEtaIDIso      * process.muPtEtaIDIsoCounter) +
    (process.muPtEtaIDIsoMuUp  * process.muPtEtaIDIsoMuUpCounter) +
    (process.muPtEtaIDIsoMuDown* process.muPtEtaIDIsoMuDownCounter) +
    (process.muPtEtaRelID+process.muPtEtaRelIDMuUp+process.muPtEtaRelIDMuDown)
    )

#######################################################################


process.muTauStreamAnalyzer = cms.EDAnalyzer(
    "MuTauStreamAnalyzer",
    diTaus         = cms.InputTag("selectedDiTau"),
    jets           = cms.InputTag("selectedPatJets"),
    newJets        = cms.InputTag(""),
    met            = cms.InputTag("metRecoilCorrector",  "N"),
    rawMet         = cms.InputTag("patMETsPFlow"),
    mvaMet         = cms.InputTag("patPFMetByMVA"),
    metCov         = cms.InputTag("pfMEtMVACov"),
    muons          = cms.InputTag("muPtEtaIDIso"),
    muonsRel       = cms.InputTag("muPtEtaRelID"),
    vertices       = cms.InputTag("selectedPrimaryVertices"),
    triggerResults = cms.InputTag("patTriggerEvent"),
    genParticles   = cms.InputTag("genParticles"),
    genTaus        = cms.InputTag("tauGenJetsSelectorAllHadrons"),
    isMC           = cms.bool(runOnMC),
    deltaRLegJet   = cms.untracked.double(0.5),
    minCorrPt      = cms.untracked.double(15.),
    minJetID       = cms.untracked.double(0.5), # 1=loose,2=medium,3=tight
    verbose        = cms.untracked.bool( False ),
    doMuIsoMVA     = cms.bool( False ),
    inputFileName0 = cms.FileInPath("Muon/MuonAnalysisTools/data/MuonIsoMVA_sixie-BarrelPt5To10_V0_BDTG.weights.xml"),
    inputFileName1 = cms.FileInPath("Muon/MuonAnalysisTools/data/MuonIsoMVA_sixie-EndcapPt5To10_V0_BDTG.weights.xml"),
    inputFileName2 = cms.FileInPath("Muon/MuonAnalysisTools/data/MuonIsoMVA_sixie-BarrelPt10ToInf_V0_BDTG.weights.xml"),
    inputFileName3 = cms.FileInPath("Muon/MuonAnalysisTools/data/MuonIsoMVA_sixie-EndcapPt10ToInf_V0_BDTG.weights.xml"),                 
    inputFileName4 = cms.FileInPath("Muon/MuonAnalysisTools/data/MuonIsoMVA_sixie-Tracker_V0_BDTG.weights.xml"),
    inputFileName5 = cms.FileInPath("Muon/MuonAnalysisTools/data/MuonIsoMVA_sixie-Global_V0_BDTG.weights.xml"),
    )

if usePFMEtMVA:
    if useRecoil :
        process.muTauStreamAnalyzer.met = cms.InputTag("metRecoilCorrector",  "N")
    else :
        process.muTauStreamAnalyzer.met = cms.InputTag("patPFMetByMVA")

process.muTauStreamAnalyzerRaw   = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRaw"),
    met    = cms.InputTag("metRecoilCorrector",  "N"),
    )

process.muTauStreamAnalyzerJetUp   = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauJetUp"),
    met    =  cms.InputTag("rescaledMETjet",  "UNNNU"),
    )
process.muTauStreamAnalyzerJetDown = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauJetDown"),
    met    =  cms.InputTag("rescaledMETjet",  "DNNND"),
    )
process.muTauStreamAnalyzerRawJetUp   = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRawJetUp"),
    met    =  cms.InputTag("rescaledMETRawjet",  "UNNNU"),
    )
process.muTauStreamAnalyzerRawJetDown = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRawJetDown"),
    met    =  cms.InputTag("rescaledMETRawjet",  "DNNND"),
    )


process.muTauStreamAnalyzerMEtResolutionUp   = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauMEtResolutionUp"),
    met    =  cms.InputTag("metRecoilCorrector",  "ResolutionU"),
    )
process.muTauStreamAnalyzerMEtResolutionDown = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauMEtResolutionDown"),
    met    =  cms.InputTag("metRecoilCorrector",  "ResolutionD"),
    )

process.muTauStreamAnalyzerRawMEtResolutionUp   = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRawMEtResolutionUp"),
    met    =  cms.InputTag("metRecoilCorrector",  "ResolutionU"),
    )
process.muTauStreamAnalyzerRawMEtResolutionDown = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRawMEtResolutionDown"),
    met    =  cms.InputTag("metRecoilCorrector",  "ResolutionD"),
    )

process.muTauStreamAnalyzerMEtResponseUp   = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauMEtResponseUp"),
    met    =  cms.InputTag("metRecoilCorrector",  "ResponseU"),
    )
process.muTauStreamAnalyzerMEtResponseDown = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauMEtResponseDown"),
    met    =  cms.InputTag("metRecoilCorrector",  "ResponseD"),
    )

process.muTauStreamAnalyzerRawMEtResponseUp   = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRawMEtResponseUp"),
    met    =  cms.InputTag("metRecoilCorrector",  "ResponseU"),
    )
process.muTauStreamAnalyzerRawMEtResponseDown = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRawMEtResponseDown"),
    met    =  cms.InputTag("metRecoilCorrector",  "ResponseD"),
    )

process.muTauStreamAnalyzerMuUp    = process.muTauStreamAnalyzer.clone(
    diTaus   =  cms.InputTag("selectedDiTauMuUp"),
    met      =  cms.InputTag("rescaledMETmuon","NNUNN"),
    muons    =  cms.InputTag("muPtEtaIDIsoMuUp"),
    muonsRel =  cms.InputTag("muPtEtaRelIDMuUp"),
    )
process.muTauStreamAnalyzerMuDown  = process.muTauStreamAnalyzer.clone(
    diTaus   =  cms.InputTag("selectedDiTauMuDown"),
    met      =  cms.InputTag("rescaledMETmuon","NNDNN"),
    muons    =  cms.InputTag("muPtEtaIDIsoMuDown"),
    muonsRel =  cms.InputTag("muPtEtaRelIDMuDown"),
    )

process.muTauStreamAnalyzerRawMuUp    = process.muTauStreamAnalyzer.clone(
    diTaus   =  cms.InputTag("selectedDiRawTauMuUp"),
    met      =  cms.InputTag("rescaledMETRawmuon","NNUNN"),
    muons    =  cms.InputTag("muPtEtaIDIsoMuUp"),
    muonsRel =  cms.InputTag("muPtEtaRelIDMuUp"),
    )
process.muTauStreamAnalyzerRawMuDown  = process.muTauStreamAnalyzer.clone(
    diTaus   =  cms.InputTag("selectedDiTauRawMuDown"),
    met      =  cms.InputTag("rescaledMETRawmuon","NNDNN"),
    muons    =  cms.InputTag("muPtEtaIDIsoMuDown"),
    muonsRel =  cms.InputTag("muPtEtaRelIDMuDown"),
    )

process.muTauStreamAnalyzerTauUp   = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauTauUp"),
    met    =  cms.InputTag("rescaledMETtau","NNNUN"),
    )
process.muTauStreamAnalyzerTauDown = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauTauDown"),
    met    =  cms.InputTag("rescaledMETtau","NNNDN")
    )
process.muTauStreamAnalyzerRawTauUp   = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRawTauUp"),
    met    =  cms.InputTag("rescaledMETRawtau","NNNUN"),
    )
process.muTauStreamAnalyzerRawTauDown = process.muTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRawTauDown"),
    met    =  cms.InputTag("rescaledMETRawtau","NNNDN")
    )

process.allAnalyzers = cms.Sequence(
    process.muTauStreamAnalyzer+
    process.muTauStreamAnalyzerJetUp+
    process.muTauStreamAnalyzerJetDown+
    process.muTauStreamAnalyzerMEtResponseUp+
    process.muTauStreamAnalyzerMEtResponseDown+
    process.muTauStreamAnalyzerMEtResolutionUp+
    process.muTauStreamAnalyzerMEtResolutionDown+
    process.muTauStreamAnalyzerMuUp+
    process.muTauStreamAnalyzerMuDown+
    process.muTauStreamAnalyzerTauUp+
    process.muTauStreamAnalyzerTauDown+
    process.muTauStreamAnalyzerRaw+
    process.muTauStreamAnalyzerRawJetUp+
    process.muTauStreamAnalyzerRawJetDown+
    process.muTauStreamAnalyzerRawMEtResponseUp+
    process.muTauStreamAnalyzerRawMEtResponseDown+
    process.muTauStreamAnalyzerRawMEtResolutionUp+
    process.muTauStreamAnalyzerRawMEtResolutionDown+
    process.muTauStreamAnalyzerRawMuUp+
    process.muTauStreamAnalyzerRawMuDown+
    process.muTauStreamAnalyzerRawTauUp+
    process.muTauStreamAnalyzerRawTauDown
    )
#######################################################################

process.analysis = cms.Sequence(
    process.allEventsFilter*
    process.filterSequence*
    process.rescaledObjects*
    process.allDiTau*
    process.allAnalyzers
    )

#######################################################################

process.tauPtEtaIDAgMuAgElecScaled
process.seqNominal = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*    
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.produceType1corrPFMEt*
    process.producePFMEtNoPileUp*
    process.metRecoilCorrector*
    process.pfMEtMVACov*
    process.diTau*process.selectedDiTau*process.selectedDiTauCounter*
    process.muTauStreamAnalyzer
    )

process.seqJetUp = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.metRecoilCorrector*
    process.rescaledMETjet *
    process.pfMEtMVACov*
    process.diTauJetUp*process.selectedDiTauJetUp*process.selectedDiTauJetUpCounter*
    process.muTauStreamAnalyzerJetUp
    )

process.seqJetDown = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.metRecoilCorrector*
    process.rescaledMETjet *
    process.pfMEtMVACov*
    process.diTauJetDown*process.selectedDiTauJetDown*process.selectedDiTauJetDownCounter*
    process.muTauStreamAnalyzerJetDown
    )

process.seqMEtResolutionUp = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.metRecoilCorrector*
    process.pfMEtMVACov*
    process.diTauMEtResolutionUp*process.selectedDiTauMEtResolutionUp*process.selectedDiTauMEtResolutionUpCounter*
    process.muTauStreamAnalyzerMEtResolutionUp
    )
process.seqMEtResolutionDown = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.metRecoilCorrector*
    process.pfMEtMVACov*
    process.diTauMEtResolutionDown*process.selectedDiTauMEtResolutionDown*process.selectedDiTauMEtResolutionDownCounter*
    process.muTauStreamAnalyzerMEtResolutionDown
    )

process.seqMEtResponseUp = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.metRecoilCorrector*
    process.pfMEtMVACov*
    process.diTauMEtResponseUp*process.selectedDiTauMEtResponseUp*process.selectedDiTauMEtResponseUpCounter*
    process.muTauStreamAnalyzerMEtResponseUp
    )
process.seqMEtResponseDown = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.metRecoilCorrector*
    process.pfMEtMVACov*
    process.diTauMEtResponseDown*process.selectedDiTauMEtResponseDown*process.selectedDiTauMEtResponseDownCounter*
    process.muTauStreamAnalyzerMEtResponseDown
    )

process.seqMuUp = cms.Sequence(
    process.allEventsFilter*
    process.muPtEtaIDIsoPtRel *
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.metRecoilCorrector*
    (process.rescaledMETmuon+process.rescaledMuons+process.rescaledMuonsRel)*
    (process.muPtEtaIDIsoMuUp*process.muPtEtaIDIsoMuUpCounter) *
    process.muPtEtaRelIDMuUp *
    process.pfMEtMVACov*
    process.diTauMuUp*process.selectedDiTauMuUp*process.selectedDiTauMuUpCounter*
    process.muTauStreamAnalyzerMuUp
    )
process.seqMuDown = cms.Sequence(
    process.allEventsFilter*
    process.muPtEtaIDIsoPtRel *
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.metRecoilCorrector*
    (process.rescaledMETmuon+process.rescaledMuons+process.rescaledMuonsRel)*
    (process.muPtEtaIDIsoMuUp*process.muPtEtaIDIsoMuUpCounter) *
    (process.muPtEtaIDIsoMuDown*process.muPtEtaIDIsoMuDownCounter) *
    process.muPtEtaRelIDMuDown *
    process.pfMEtMVACov*
    process.diTauMuDown*process.selectedDiTauMuDown*process.selectedDiTauMuDownCounter*
    process.muTauStreamAnalyzerMuDown
    )

process.seqTauUp = cms.Sequence(
    process.allEventsFilter*
    (process.muPtEtaIDIso*process.muPtEtaIDIsoCounter) *
    process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIsoPtRel*
    process.muPtEtaRelID *
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.metRecoilCorrector*
    (process.rescaledMETtau+process.rescaledTaus)*
    (process.tauPtEtaIDAgMuAgElecIsoTauUp*process.tauPtEtaIDAgMuAgElecIsoTauUpCounter)*
    process.pfMEtMVACov*
    process.diTauTauUp*process.selectedDiTauTauUp*process.selectedDiTauTauUpCounter*
    process.muTauStreamAnalyzerTauUp
    )
process.seqTauDown = cms.Sequence(
    process.allEventsFilter*
    (process.muPtEtaIDIso*process.muPtEtaIDIsoCounter) *
    process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIsoPtRel*
    process.muPtEtaRelID *
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.metRecoilCorrector*
    (process.rescaledMETtau+process.rescaledTaus)*
    (process.tauPtEtaIDAgMuAgElecIsoTauDown*process.tauPtEtaIDAgMuAgElecIsoTauDownCounter)*
    process.pfMEtMVACov*
    process.diTauTauDown*process.selectedDiTauTauDown*process.selectedDiTauTauDownCounter*
    process.muTauStreamAnalyzerTauDown
    )


if not usePFMEtMVA:
    process.seqNominal.remove(process.pfMEtMVACov)
    process.seqJetUp.remove(process.pfMEtMVACov)
    process.seqJetDown.remove(process.pfMEtMVACov)
    process.seqMEtResolutionUp.remove(process.pfMEtMVACov)
    process.seqMEtResolutionDown.remove(process.pfMEtMVACov)
    process.seqMEtResponseUp.remove(process.pfMEtMVACov)
    process.seqMEtResponseDown.remove(process.pfMEtMVACov)
    process.seqMuUp.remove(process.pfMEtMVACov)
    process.seqMuDown.remove(process.pfMEtMVACov)
    process.seqTauUp.remove(process.pfMEtMVACov)
    process.seqTauDown.remove(process.pfMEtMVACov)


process.seqRawNominal = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    process.puJetIdSequence *
    process.metRecoilCorrector*
    #process.pfMEtMVACov*
    process.diTauRaw*process.selectedDiTauRaw*process.selectedDiTauRawCounter*
    process.muTauStreamAnalyzerRaw
    )

process.seqRawJetUp = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    process.puJetIdSequence *
    process.metRecoilCorrector*
    process.rescaledMETRawjet *
    #process.pfMEtMVACov*
    process.diTauRawJetUp*process.selectedDiTauRawJetUp*process.selectedDiTauRawJetUpCounter*
    process.muTauStreamAnalyzerRawJetUp
    )

process.seqRawJetDown = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    process.puJetIdSequence *
    process.metRecoilCorrector*
    process.rescaledMETRawjet *
    #process.pfMEtMVACov*
    process.diTauRawJetDown*process.selectedDiTauRawJetDown*process.selectedDiTauRawJetDownCounter*
    process.muTauStreamAnalyzerRawJetDown
    )

process.seqRawMEtResolutionUp = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    process.puJetIdSequence *
    process.metRecoilCorrector*
    #process.pfMEtMVACov*
    process.diTauRawMEtResolutionUp*process.selectedDiTauRawMEtResolutionUp*process.selectedDiTauRawMEtResolutionUpCounter*
    process.muTauStreamAnalyzerRawMEtResolutionUp
    )

process.seqRawMEtResolutionDown = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    process.puJetIdSequence *
    process.metRecoilCorrector*
    #process.pfMEtMVACov*
    process.diTauRawMEtResolutionDown*process.selectedDiTauRawMEtResolutionDown*process.selectedDiTauRawMEtResolutionDownCounter*
    process.muTauStreamAnalyzerRawMEtResolutionDown
    )

process.seqRawMEtResponseUp = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    process.puJetIdSequence *
    process.metRecoilCorrector*
    #process.pfMEtMVACov*
    process.diTauRawMEtResponseUp*process.selectedDiTauRawMEtResponseUp*process.selectedDiTauRawMEtResponseUpCounter*
    process.muTauStreamAnalyzerRawMEtResponseUp
    )

process.seqRawMEtResponseDown = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    process.puJetIdSequence *
    process.metRecoilCorrector*
    #process.pfMEtMVACov*
    process.diTauRawMEtResponseDown*process.selectedDiTauRawMEtResponseDown*process.selectedDiTauRawMEtResponseDownCounter*
    process.muTauStreamAnalyzerRawMEtResponseDown
    )

process.seqRawMuUp = cms.Sequence(
    process.allEventsFilter*
    process.muPtEtaIDIsoPtRel *
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    process.puJetIdSequence *
    process.metRecoilCorrector*
    (process.rescaledMETRawmuon+process.rescaledMuons+process.rescaledMuonsRel)*
    (process.muPtEtaIDIsoMuUp*process.muPtEtaIDIsoMuUpCounter) *
    process.muPtEtaRelIDMuUp *
    #process.pfMEtMVACov*
    process.diTauRawMuUp*process.selectedDiTauRawMuUp*process.selectedDiTauRawMuUpCounter*
    process.muTauStreamAnalyzerRawMuUp
    )

process.seqRawMuDown = cms.Sequence(
    process.allEventsFilter*
    process.muPtEtaIDIsoPtRel *
    (process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    process.puJetIdSequence *
    process.metRecoilCorrector*
    (process.rescaledMETRawmuon+process.rescaledMuons+process.rescaledMuonsRel)*
    (process.muPtEtaIDIsoMuDown*process.muPtEtaIDIsoMuDownCounter) *
    process.muPtEtaRelIDMuDown *
    #process.pfMEtMVACov*
    process.diTauRawMuDown*process.selectedDiTauRawMuDown*process.selectedDiTauRawMuDownCounter*
    process.muTauStreamAnalyzerRawMuDown
    )

process.seqRawTauUp = cms.Sequence(
    process.allEventsFilter*
    (process.muPtEtaIDIso*process.muPtEtaIDIsoCounter) *
    process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIsoPtRel*
    process.muPtEtaRelID *
    process.puJetIdSequence *
    process.metRecoilCorrector*
    (process.rescaledMETRawtau+process.rescaledTaus)*
    (process.tauPtEtaIDAgMuAgElecIsoTauUp*process.tauPtEtaIDAgMuAgElecIsoTauUpCounter)*
    #process.pfMEtMVACov*
    process.diTauRawTauUp*process.selectedDiTauRawTauUp*process.selectedDiTauRawTauUpCounter*
    process.muTauStreamAnalyzerRawTauUp
    )

process.seqRawTauDown = cms.Sequence(
    process.allEventsFilter*
    (process.muPtEtaIDIso*process.muPtEtaIDIsoCounter) *
    process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIsoPtRel*
    process.muPtEtaRelID *
    process.puJetIdSequence *
    process.metRecoilCorrector*
    (process.rescaledMETRawtau+process.rescaledTaus)*
    (process.tauPtEtaIDAgMuAgElecIsoTauDown*process.tauPtEtaIDAgMuAgElecIsoTauDownCounter)*
    #process.pfMEtMVACov*
    process.diTauRawTauDown*process.selectedDiTauRawTauDown*process.selectedDiTauRawTauDownCounter*
    process.muTauStreamAnalyzerRawTauDown
    )

#######################################################################

if runOnMC:
    process.pNominal            = cms.Path( process.seqNominal )
    #process.pJetUp                 = cms.Path( process.seqJetUp   )
    #process.pJetDown               = cms.Path( process.seqJetDown )
    #process.pMEtResolutionUp       = cms.Path( process.seqMEtResolutionUp )
    #process.pMEtResolutionDown     = cms.Path( process.seqMEtResolutionDown )
    #process.pMEtResponseUp         = cms.Path( process.seqMEtResponseUp)
    #process.pMEtResponseDown       = cms.Path( process.seqMEtResponseDown)
    #process.pMuUp                  = cms.Path( process.seqMuUp)
    #process.pMuDown                = cms.Path( process.seqMuDown)
    process.pTauUp              = cms.Path( process.seqTauUp)
    process.pTauDown            = cms.Path( process.seqTauDown )
    #process.pRawNominal         = cms.Path( process.seqRawNominal )
    #process.pRawJetUp              = cms.Path( process.seqRawJetUp   )
    #process.pRawJetDown            = cms.Path( process.seqRawJetDown )
    #process.pRawMEtResolutionUp    = cms.Path( process.seqRawMEtResolutionUp )
    #process.pRawMEtResolutionDown  = cms.Path( process.seqRawMEtResolutionDown )
    #process.pRawMEtResponseUp      = cms.Path( process.seqRawMEtResponseUp)
    #process.pRawMEtResponseDown    = cms.Path( process.seqRawMEtResponseDown)
    #process.pRawMuUp               = cms.Path( process.seqRawMuUp)
    #process.pRawMuDown             = cms.Path( process.seqRawMuDown)
    #process.pRawTauUp           = cms.Path( process.seqRawTauUp )
    #process.pRawTauDown         = cms.Path( process.seqRawTauDown )

else:
    process.pNominal            = cms.Path( process.seqNominal )
    #process.pTauUp              = cms.Path( process.seqTauUp)
    #process.pTauDown            = cms.Path( process.seqTauDown )
    #process.pRawNominal         = cms.Path( process.seqRawNominal )
    #process.pRawTauUp           = cms.Path( process.seqRawTauUp )
    #process.pRawTauDown         = cms.Path( process.seqRawTauDown )


#from PhysicsTools.PatAlgos.tools.helpers import massSearchReplaceAnyInputTag
#massSearchReplaceAnyInputTag(process.pNominalRaw,
#                             "patPFMetByMVA",
#                             "patMETsPFlow",
#                             verbose=True)
#massSearchReplaceAnyInputTag(process.pNominalRaw,
#                             "pfMEtMVACov",
#                             "",
#                             verbose=True)

#######################################################################

process.out = cms.OutputModule(
    "PoolOutputModule",
    outputCommands = cms.untracked.vstring( 'drop *',
                                            'keep *_metRecoilCorrector_*_*'),
    fileName = cms.untracked.string('patTuplesSkimmed_MuTauStream.root'),
    )

process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string("treeMuTauStream.root")
    )


## To work on Artur's skim
#from PhysicsTools.PatAlgos.tools.helpers import massSearchReplaceAnyInputTag
#massSearchReplaceAnyInputTag(process.pNominal,
#                             "muPtEtaID",
#                             "selectedPatMuonsTriggerMatch",
#                             verbose=False)
#massSearchReplaceAnyInputTag(process.pNominal,
#                             "muPtEtaRelID",
#                             "selectedPatMuonsTriggerMatch",
#                             verbose=False)
#massSearchReplaceAnyInputTag(process.pNominal,
#                             "selectedPrimaryVertices",
#                             "offlinePrimaryVertices",
#                             verbose=False)
#massSearchReplaceAnyInputTag(process.pNominal,
#                            "tauPtEtaIDAgMuAgElec",
#                             "selectedPatTausTriggerMatch",
#                             verbose=False)
#massSearchReplaceAnyInputTag(process.pNominal,
#                             "genParticles",
#                             "prunedGenParticles",
#                             verbose=False)
#massSearchReplaceAnyInputTag(process.pNominal,
#                             "tauGenJetsSelectorAllHadrons",
#                             "genTauDecaysToHadrons",
#                             verbose=False)


process.outpath = cms.EndPath()

processDumpFile = open('runMuTauStreamAnalyzerFullAnalysis_Recoil.dump', 'w')
print >> processDumpFile, process.dumpPython()
