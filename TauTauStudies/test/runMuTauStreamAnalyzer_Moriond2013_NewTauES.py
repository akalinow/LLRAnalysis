import FWCore.ParameterSet.Config as cms

process = cms.Process("MUTAUANA")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

#from Configuration.PyReleaseValidation.autoCond import autoCond
#process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

runOnMC     = True
doSVFitReco = True
usePFMEtMVA = True
useRecoil   = True
runUserIsoTau = False
applyTauESCorr= True
useMarkov   = True
runMoriond = True

if runOnMC:
    print "Running on MC"
else:
    print "Running on Data"

if useMarkov:
    print "Use SVFit with Markov chain integration"
else:
    print "Use SVFit with VEGAS integration"
    
if runOnMC:
    process.GlobalTag.globaltag = cms.string('START53_V15::All')
else:
    process.GlobalTag.globaltag = cms.string('GR_P_V42_AN3::All')
    
    
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
        ##'file:patTuples_LepTauStream.root'
        'file:/afs/cern.ch/work/a/anayak/public/HTauTau/Spring2013/patTuples_LepTauStream.root'
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

tausForPFMEtMVA = None
if runUserIsoTau:
    tausForPFMEtMVA = cms.InputTag('UserIsoTaus') 
else :
    tausForPFMEtMVA = cms.InputTag('tauPtEtaIDAgMuAgElecRelIso')
if runMoriond:
    process.pfMEtMVA.srcLeptons = cms.VInputTag( cms.InputTag('muPtEtaRelIDRelIso'), cms.InputTag('elecPtEtaRelIDRelIso'), cms.InputTag('tauPtEtaIDAgMuAgElecRelIso') )
else:    
    process.pfMEtMVA.srcLeptons = cms.VInputTag( cms.InputTag('UserIsoMuons'), tausForPFMEtMVA )
    
#process.pfMEtMVA.srcVertices = cms.InputTag("selectedPrimaryVertices")
process.pfMEtMVA.srcVertices = cms.InputTag("offlinePrimaryVertices")

# CV: use new "highqt" MVA training, recommended by Phil
process.pfMEtMVA.inputFileNames = cms.PSet(
    DPhi = cms.FileInPath('JetMETCorrections/METPUSubtraction/data/gbrmetphi_53_Dec2012.root'),
    CovU2 = cms.FileInPath('JetMETCorrections/METPUSubtraction/data/gbru2cov_53_Dec2012_highqt.root'),
    U = cms.FileInPath('JetMETCorrections/METPUSubtraction/data/gbrmet_53_Dec2012_highqt.root'),
    CovU1 = cms.FileInPath('JetMETCorrections/METPUSubtraction/data/gbru1cov_53_Dec2012_highqt.root')
)

process.load("PhysicsTools.PatAlgos.producersLayer1.metProducer_cfi")
process.patPFMetByMVA = process.patMETs.clone(
    metSource = cms.InputTag('pfMEtMVA'),
    addMuonCorrections = cms.bool(False),
    genMETSource = cms.InputTag('genMetTrue'),
    addGenMET = cms.bool(runOnMC)
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
    electronCollection = '',
    photonCollection = '',
    muonCollection = cms.InputTag('UserIsoMuons'),
    tauCollection = tausForPFMEtMVA,
    jetCollection = cms.InputTag('selectedPatJets'),     
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

process.load("JetMETCorrections.Type1MET.pfMETsysShiftCorrections_cfi")
sysShiftCorrParameter = None
if runOnMC:
    sysShiftCorrParameter = process.pfMEtSysShiftCorrParameters_2012runABCvsNvtx_mc
else:
    sysShiftCorrParameter = process.pfMEtSysShiftCorrParameters_2012runABCvsNvtx_data

from PhysicsTools.PatUtils.tools.runType1PFMEtUncertainties import runType1PFMEtUncertainties
runType1PFMEtUncertainties(
    process,
    electronCollection = '',
    photonCollection = '',
    muonCollection = cms.InputTag('UserIsoMuons'),
    tauCollection = tausForPFMEtMVA,
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

# CV: collections of MET and jets used as input for SysShiftMETcorrInputProducer
#     are not fully consistent, but we anyway use parametrization of MET x/y shift as function of Nvtx
process.pfMEtSysShiftCorr.srcMEt = cms.InputTag('patMETs')
process.pfMEtSysShiftCorr.srcJets = cms.InputTag('selectedPatJets')

##from PhysicsTools.PatAlgos.tools.helpers import massSearchReplaceAnyInputTag
##massSearchReplaceAnyInputTag(process.producePatPFMETCorrections, cms.InputTag('patPFMet'), cms.InputTag('patMETs'))
process.patPFMet.addGenMET = cms.bool(runOnMC)
process.patPFMetJetEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetJetEnDown.addGenMET = cms.bool(runOnMC)
process.patPFMetMuonEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetMuonEnDown.addGenMET = cms.bool(runOnMC)
process.patPFMetTauEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetTauEnDown.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUp.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpJetEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpJetEnDown.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpMuonEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpMuonEnDown.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpTauEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpTauEnDown.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpUnclusteredEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpUnclusteredEnDown.addGenMET = cms.bool(runOnMC)
##process.producePatPFMETCorrections.remove(process.patPFMet)

process.produceType1corrPFMEt = cms.Sequence()
if runOnMC:
    process.patPFJetMETtype1p2Corr.jetCorrLabel = cms.string("L3Absolute")
    process.produceType1corrPFMEt += process.pfType1MEtUncertaintySequence
else:
    # CV: apply data/MC residual correction to "unclustered energy"
    process.calibratedPFCandidates = cms.EDProducer("PFCandResidualCorrProducer",
        src = cms.InputTag('particleFlow'),
        residualCorrLabel = cms.string("ak5PFResidual"),
        residualCorrEtaMax = cms.double(9.9),
        extraCorrFactor = cms.double(1.05)
    )
    process.produceType1corrPFMEt += process.calibratedPFCandidates
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
    process.produceType1corrPFMEt += process.pfType1MEtUncertaintySequence
#----------------------------------------------------------------------------------

#----------------------------------------------------------------------------------
# produce CaloMEtNoHF (MC corrected by data/MC difference in CaloMET response)

process.produceCaloMEtNoHF = cms.Sequence()

process.load("LLRAnalysis/TauTauStudies/sumCaloTowersInEtaSlices_cfi")
if runOnMC:
    process.metNoHFresidualCorrected.residualCorrLabel = cms.string("ak5CaloResidual")
    process.metNoHFresidualCorrected.extraCorrFactor = cms.double(1.05)
    process.metNoHFresidualCorrected.isMC = cms.bool(True)
    process.produceCaloMEtNoHF += process.metNoHFresidualCorrected
    process.metNoHFresidualCorrectedUp = process.metNoHFresidualCorrected.clone(
        extraCorrFactor = cms.double(1.10)
    )
    process.produceCaloMEtNoHF += process.metNoHFresidualCorrectedUp
    process.metNoHFresidualCorrectedDown = process.metNoHFresidualCorrected.clone(
        extraCorrFactor = cms.double(1.0)
    )
    process.produceCaloMEtNoHF += process.metNoHFresidualCorrectedDown
else:
    process.metNoHFresidualCorrected.residualCorrLabel = cms.string("")
    process.metNoHFresidualCorrected.extraCorrFactor = cms.double(1.0)
    process.metNoHFresidualCorrected.isMC = cms.bool(False)
    process.produceCaloMEtNoHF += process.metNoHFresidualCorrected
#----------------------------------------------------------------------------------

#----------------------------------------------------------------------------------
# produce PU Jet Ids

from RecoJets.JetProducers.PileupJetID_cfi import stdalgos_5x, stdalgos_4x, stdalgos, cutbased, chsalgos_5x, chsalgos_4x, chsalgos
process.puJetMva = cms.EDProducer('PileupJetIdProducer',
                                  produceJetIds = cms.bool(True),
                                  jetids = cms.InputTag(""),
                                  runMvas = cms.bool(True),
                                  jets = cms.InputTag("ak5PFJets"),
                                  vertexes = cms.InputTag("offlinePrimaryVertices"),
                                  algos = cms.VPSet(stdalgos),
                                  rho     = cms.InputTag("kt6PFJets","rho"),
                                  jec     = cms.string("AK5PF"),
                                  applyJec = cms.bool(True),
                                  inputIsCorrected = cms.bool(False),
                                  residualsFromTxt = cms.bool(False),
                                  residualsTxt     = cms.FileInPath("RecoJets/JetProducers/data/dummy.txt"),
                                  )

process.puJetIdSequence = cms.Sequence(process.puJetMva)

#----------------------------------------------------------------------------------

process.puJetIdAndMvaMet = cms.Sequence(process.puJetIdSequence *
                                        (process.pfMEtMVAsequence*process.patPFMetByMVA))

############################Leptons for MVA MET ################################################
process.muPtEtaRelIDRelIso = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("muonsForVeto"),#muPtEtaRelID?//muons for veto?
    cut = cms.string("pt>10 && abs(eta)<2.4"+
                     " && isTrackerMuon"),
    filter = cms.bool(False)
    )

process.UserIsoMuons = cms.EDProducer( #Most isolated muon
    "UserIsolatedPatMuon",
    MuonTag = cms.InputTag("muPtEtaID"),
    isMC = cms.bool(runOnMC),
    verbose = cms.untracked.bool(False),
    userIso = cms.untracked.string("PFRelIsoDB04v2"),
    pt = cms.double(20),
    eta = cms.double(2.1)
    )

process.elecPtEtaRelIDRelIso = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("electronsForVeto"),
    cut = cms.string("pt>10 && abs(eta)<2.4"+
                     " && ( (abs(eta)<0.80 && userFloat('mvaPOGNonTrig')>0.925) || (abs(eta)<1.479 && abs(eta)>0.80 && userFloat('mvaPOGNonTrig')>0.975) || (abs(eta)>1.479 && userFloat('mvaPOGNonTrig')>0.985)  )"+
                     " && userFloat('nHits')==0"),
    filter = cms.bool(False)
    )

process.UserIsoElectrons = cms.EDProducer( #Most isolated electron
    "UserIsolatedPatElectron",
    ElectronTag = cms.InputTag("elecPtEtaID"),
    isMC = cms.bool(runOnMC),
    verbose = cms.untracked.bool(False),
    userIso = cms.untracked.string("PFRelIsoDB04v3"),
    pt = cms.double(24),
    eta = cms.double(2.1)
    )

process.tauPtEtaIDAgMuAgElecRelIso  = cms.EDFilter( 
    "PATTauSelector",
    src = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    cut = cms.string("pt>19 && abs(eta)<2.3"+
                     " && tauID('byIsolationMVA2raw')>0.7" #Selection as like Phil's
                     #" && tauID('byLooseIsolationMVA2')>0.5"
                     ),
    filter = cms.bool(False)
    )
process.UserIsoTaus = cms.EDProducer( #Most isolated tau
    "UserIsolatedPatTau",
    patTauTag = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    isMC = cms.bool(runOnMC),
    verbose = cms.untracked.bool(False),
    useIsoMVA = cms.untracked.bool(True),
    pt = cms.double(19),
    eta = cms.double(2.3)
    )
if applyTauESCorr:
    process.tauPtEtaIDAgMuAgElecRelIso.src = cms.InputTag("tauPtEtaIDAgMuAgElecScaled")
    process.UserIsoTaus.patTauTag = cms.InputTag("tauPtEtaIDAgMuAgElecScaled")
    
if runUserIsoTau :
    process.TausForMVAMEt = cms.Sequence(process.UserIsoTaus)
else :
    process.TausForMVAMEt = cms.Sequence(process.tauPtEtaIDAgMuAgElecRelIso)
if runMoriond:
    process.LeptonsForMVAMEt = cms.Sequence(process.muPtEtaRelIDRelIso*
                                            process.elecPtEtaRelIDRelIso*
                                            process.TausForMVAMEt
                                        )
else: 
    process.LeptonsForMVAMEt = cms.Sequence(process.UserIsoMuons*
                                            #process.UserIsoElectrons*
                                            process.TausForMVAMEt
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
    verbose             = cms.bool(False),
    isMC                = cms.bool(runOnMC),
    )

if usePFMEtMVA and useRecoil :
    process.metRecoilCorrector.metTag = cms.InputTag("patPFMetByMVA")
        
###################################################################################

#process.load("LLRAnalysis.Utilities.diTausReconstruction_cff")
process.load("TauAnalysis.CandidateTools.muTauPairProduction_cff")
process.diTau = process.allMuTauPairs.clone()
process.diTau.srcLeg1  = cms.InputTag("muPtEtaIDIso")
process.diTau.srcLeg2  = cms.InputTag("tauPtEtaIDAgMuAgElecIso")
process.diTau.srcMET   = cms.InputTag("metRecoilCorrector",  "N")
process.diTau.srcPrimaryVertex = cms.InputTag("offlinePrimaryVertices")
process.diTau.dRmin12  = cms.double(0.5)
process.diTau.doSVreco = cms.bool(doSVFitReco)
if useMarkov:
    process.diTau.nSVfit.psKine_MEt_int.algorithm = cms.PSet( #Markov chain integration
    pluginName = cms.string(
        "nSVfitAlgorithmByIntegration2"
    ),
    pluginType = cms.string(
        "NSVfitAlgorithmByIntegration2"
    ),
    markovChainOptions = cms.PSet(
        mode = cms.string(
            "Metropolis"
        ),
        initMode = cms.string(
            "Gaus"
        ),
        numIterBurnin = cms.uint32(10000),
        numIterSampling = cms.uint32(100000),
        numIterSimAnnealingPhase1 = cms.uint32(2000),
        numIterSimAnnealingPhase2 = cms.uint32(6000),
        T0 = cms.double(15.),
        alpha = cms.double(0.999),
        numChains = cms.uint32(1),
        numBatches = cms.uint32(1),
        L = cms.uint32(1),
        epsilon0 = cms.double(1.e-2),
        nu = cms.double(0.71)
    ),
    max_or_median = cms.string(
        "max"
    ),
    verbosity = cms.int32(0)
)#end of Markov chain configuration
    
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
        process.diTau.nSVfit.psKine_MEt_int.config.event.srcMEt = cms.InputTag("metRecoilCorrector", "N")
        process.diTau.nSVfit.psKine_MEt_int.config.event.likelihoodFunctions[0].srcMEtCovMatrix = cms.InputTag("pfMEtMVACov")
    else :
        process.diTau.nSVfit.psKine_MEt_logM_fit.config.event.srcMEt = cms.InputTag("patPFMetByMVA")
        process.diTau.nSVfit.psKine_MEt_logM_fit.config.event.likelihoodFunctions[0].srcMEtCovMatrix = cms.InputTag("pfMEtMVACov")
        process.diTau.nSVfit.psKine_MEt_int.config.event.srcMEt = cms.InputTag("patPFMetByMVA")
        process.diTau.nSVfit.psKine_MEt_int.config.event.likelihoodFunctions[0].srcMEtCovMatrix = cms.InputTag("pfMEtMVACov")

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
process.diTauRaw.srcPrimaryVertex = cms.InputTag("offlinePrimaryVertices")
process.diTauRaw.dRmin12  = cms.double(0.5)
process.diTauRaw.doSVreco = cms.bool(doSVFitReco)
process.diTauRaw.nSVfit.psKine_MEt_int.algorithm = process.diTau.nSVfit.psKine_MEt_int.algorithm

if not runOnMC:
    process.diTauRaw.srcGenParticles = ""
        
process.diTauRaw.nSVfit.psKine_MEt_logM_fit.config.event.srcMEt = cms.InputTag("metRecoilCorrector",  "N")
process.diTauRaw.nSVfit.psKine_MEt_int.config.event.srcMEt = cms.InputTag("metRecoilCorrector",  "N")

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
process.tauPtEtaIDAgMuAgElec = cms.EDFilter( #apply AntiMuTight2
    "PATTauSelector",
    src = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    cut = cms.string("tauID('againstMuonTight2')>0.5 "+
                     " && tauID('againstElectronLoose')>0.5 "
                     ),
    filter = cms.bool(False)
    )

process.tauPtEtaIDAgMuAgElecScaled = cms.EDProducer(
    "TauESCorrector",
    tauTag = cms.InputTag("tauPtEtaIDAgMuAgElec")
    #verbose         = cms.bool(True)
    )

process.tauPtEtaIDAgMuAgElecIso  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    cut = cms.string("pt>20 && abs(eta)<2.3"+
                     " && tauID('byLooseIsolationMVA2')>-0.5"
                     ),
    filter = cms.bool(False)
    )
process.tauPtEtaIDAgMuAgElecIsoPtRel  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    cut = cms.string("pt>19 && abs(eta)<2.3"+
                     " && tauID('byLooseIsolationMVA2')>-0.5"
                     ),
    filter = cms.bool(False)
    )
if applyTauESCorr:
    process.tauPtEtaIDAgMuAgElecIso.src = cms.InputTag("tauPtEtaIDAgMuAgElecScaled")
    process.tauPtEtaIDAgMuAgElecIsoPtRel.src = cms.InputTag("tauPtEtaIDAgMuAgElecScaled")
if runMoriond:
    process.tauPtEtaIDAgMuAgElec.cut = cms.string("(tauID('againstMuonTight2')>0.5 || tauID('againstMuonTight')>0.5)"+
                                                 " && tauID('againstElectronLoose')>0.5 ") 
    process.tauPtEtaIDAgMuAgElecIso.cut = cms.string("pt>20 && abs(eta)<2.3"+
                                                     " && (tauID('byLooseIsolationMVA2')>-0.5 || tauID('byLooseIsolationMVA')>-0.5)"
                                                     )
    process.tauPtEtaIDAgMuAgElecIsoPtRel.cut = cms.string("pt>19 && abs(eta)<2.3"+
                                                          " && (tauID('byLooseIsolationMVA2')>-0.5 || tauID('byLooseIsolationMVA')>-0.5)"
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
    doIsoMVAOrdering = cms.untracked.bool(runUserIsoTau),
    doMuIsoMVA     = cms.untracked.bool( False ),
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
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    (process.muPtEtaIDIso *process.muPtEtaIDIsoCounter) *
    process.muPtEtaRelID *
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*    
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.produceType1corrPFMEt*
    process.producePFMEtNoPileUp*
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    process.pfMEtMVACov*
    process.diTau*process.selectedDiTau*process.selectedDiTauCounter*
    process.muTauStreamAnalyzer
    )

process.seqJetUp = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
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
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
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
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
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
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
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
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
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
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
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
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
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
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
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
    process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
    process.tauPtEtaIDAgMuAgElecIsoPtRel*
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
    process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
    process.tauPtEtaIDAgMuAgElecIsoPtRel*
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
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
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
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
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
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
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
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
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
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
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
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
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
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
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
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
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
    (process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
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
    process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIsoPtRel*
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
    process.tauPtEtaIDAgMuAgElec*process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIsoPtRel*
    process.muPtEtaRelID *
    process.puJetIdSequence *
    process.metRecoilCorrector*
    (process.rescaledMETRawtau+process.rescaledTaus)*
    (process.tauPtEtaIDAgMuAgElecIsoTauDown*process.tauPtEtaIDAgMuAgElecIsoTauDownCounter)*
    #process.pfMEtMVACov*
    process.diTauRawTauDown*process.selectedDiTauRawTauDown*process.selectedDiTauRawTauDownCounter*
    process.muTauStreamAnalyzerRawTauDown
    )
if runMoriond:
    process.seqNominal.remove(process.produceType1corrPFMEt)
    process.seqNominal.remove(process.producePFMEtNoPileUp)
            
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

# before starting to process 1st event, print event content
#process.printEventContent = cms.EDAnalyzer("EventContentAnalyzer")
##process.filterFirstEvent = cms.EDFilter("EventCountFilter",
##    numEvents = cms.int32(1)
##)
##process.printFirstEventContentPath = cms.Path(process.filterFirstEvent + process.printEventContent)

process.outpath = cms.EndPath()

##
#processDumpFile = open('runMuTauStreamAnalyzer_Moriond2013_NewTauES.dump', 'w')
#print >> processDumpFile, process.dumpPython()
