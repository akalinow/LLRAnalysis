import FWCore.ParameterSet.Config as cms

process = cms.Process("ELECTAUANA")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

#from Configuration.PyReleaseValidation.autoCond import autoCond
#process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

runOnMC     = True
runOnEmbed  = False
doSVFitReco = True
usePFMEtMVA = True
useRecoil   = True
useLepTauPAT = True
runUserIsoTau = False
applyTauESCorr= True
useMarkov   = True
runMoriond = True

if runOnMC:
    print "Running on MC"
else:
    print "Running on Data"

if useLepTauPAT:
    print "Running on common LepTau PAT-tuples"
else:
    print "Running on old ElecTau PAT-tuples"

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
    #'file:patTuples_LepTauStream.root'
    #'file:VBFH125.root'
    #'file:data2012D.root'    
    #'root://polgrid4.in2p3.fr//dpm/in2p3.fr/home/cms/trivcat/store/user/mbluj/VBF_HToTauTau_M-125_8TeV-powheg-pythia6/LepTauStream-07Dec2012_VBFH125-LepTau-powheg-PUS10_pat/fbab02682d6b416ae6da687406f89be0/patTuples_LepTauStream_100_1_PYQ.root'
    'file:/data_CMS/cms/htautau/PostMoriond/pat/MC/file_GG125_patTuples_LepTauStream_10_1_Rs2.root'
    #'file:/data_CMS/cms/htautau/PostMoriond/pat/Data/file_Data_2012D_PRV1_HTT_06Mar2013_PAT_v1_p2_patTuples_LepTauStream_78_1_2KS.root'
    )
    )

#process.source.eventsToProcess = cms.untracked.VEventRange(
#    '1:470223'
#    )

process.allEventsFilter = cms.EDFilter(
    "AllEventsFilter"
    )

#######################################################################
#quark/gluon jets
process.load('QuarkGluonTagger.EightTeV.QGTagger_RecoJets_cff')  
process.QGTagger.srcJets = cms.InputTag("selectedPatJets")
process.QGTagger.isPatJet = cms.untracked.bool(True) 
#Switch for when PFJets with CHS are used (only supported for LD):
#process.QGTagger.useCHS  = cms.untracked.bool(True) 
#If an uncorrected jet source is used as input, you can correct the pt on the fly inside the QGTagger:
#process.QGTagger.jec     = cms.untracked.string('ak5PFL1FastL2L3')

###################################################################################
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
    process.pfMEtMVA.srcLeptons = cms.VInputTag( cms.InputTag('UserIsoElectrons'), tausForPFMEtMVA )
                
#process.pfMEtMVA.srcVertices = cms.InputTag("selectedPrimaryVertices")
process.pfMEtMVA.srcVertices = cms.InputTag("offlinePrimaryVertices")

# CV: use new "highqt" MVA training, recommended by Phil
#process.pfMEtMVA.inputFileNames = cms.PSet(
#    DPhi = cms.FileInPath('JetMETCorrections/METPUSubtraction/data/gbrmetphi_53_Dec2012.root'),
#    CovU2 = cms.FileInPath('JetMETCorrections/METPUSubtraction/data/gbru2cov_53_Dec2012_highqt.root'),
#    U = cms.FileInPath('JetMETCorrections/METPUSubtraction/data/gbrmet_53_Dec2012_highqt.root'),
#    CovU1 = cms.FileInPath('JetMETCorrections/METPUSubtraction/data/gbru1cov_53_Dec2012_highqt.root')
#)

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
    electronCollection = cms.InputTag('UserIsoElectrons'),
    photonCollection = '',
    muonCollection = '',
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
    electronCollection = cms.InputTag('UserIsoElectrons'),
    photonCollection = '',
    muonCollection = '',
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
process.patPFMetElectronEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetElectronEnDown.addGenMET = cms.bool(runOnMC)
process.patPFMetTauEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetTauEnDown.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUp.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpJetEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpJetEnDown.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpElectronEnUp.addGenMET = cms.bool(runOnMC)
process.patPFMetNoPileUpElectronEnDown.addGenMET = cms.bool(runOnMC)
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

from RecoJets.JetProducers.PileupJetID_cfi import *
stdalgos = cms.VPSet(full_53x,cutbased,PhilV1)
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

###################################################################################
process.muPtEtaRelIDRelIso = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("muonsForVeto"),
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
    src = cms.InputTag("electronsForVeto"), #loose MVA-Id
    cut = cms.string("pt>10 && abs(eta)<2.4"+
                     " && ( (abs(eta)<0.80 && userFloat('mvaPOGNonTrig')>0.925) || (abs(eta)<1.479 && abs(eta)>0.80 && userFloat('mvaPOGNonTrig')>0.975) || (abs(eta)>1.479 && userFloat('mvaPOGNonTrig')>0.985)  )"+
                     " && gsfTrack.trackerExpectedHitsInner.numberOfHits == 0"),
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
                     " && tauID('byIsolationMVA2raw')>0.8" #Selection as like Phil's
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
if useLepTauPAT:
    process.tauPtEtaIDAgMuAgElecRelIso.src = "tauPtEtaIDAgMuLAgElec"
    process.UserIsoTaus.patTauTag = "tauPtEtaIDAgMuLAgElec"
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
    process.LeptonsForMVAMEt = cms.Sequence(#process.UserIsoMuons*
                                            process.UserIsoElectrons*
                                            process.TausForMVAMEt
                                            )
###################################################################################
process.rescaledMET = cms.EDProducer(
    "MEtRescalerProducer",
    metTag          = cms.InputTag("metRecoilCorrector",  "N"),
    jetTag          = cms.InputTag("selectedPatJets"),
    electronTag     = cms.InputTag("elecPtEtaIDIso"),
    muonTag         = cms.InputTag(""),
    tauTag          = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
    unClusterShift  = cms.double(0.10),
    tauShift        = cms.vdouble(0.03,0.03),
    muonShift       = cms.vdouble(0.01,0.01),
    electronShift   = cms.vdouble(0.01,0.025),
    jetThreshold    = cms.double(10),
    numOfSigmas     = cms.double(1.0),
    verbose         = cms.bool(False)
    )

if usePFMEtMVA:
    if useRecoil :
        process.rescaledMET.metTag = cms.InputTag("metRecoilCorrector",  "N")
    else :
        process.rescaledMET.metTag = cms.InputTag("patPFMetByMVA")
process.rescaledMETRaw = process.rescaledMET.clone( metTag = cms.InputTag("metRecoilCorrector",  "N") )

process.rescaledMETjet = process.rescaledMET.clone(
    unClusterShift = cms.double(0.10),
    tauShift       = cms.vdouble(0.0),
    muonShift      = cms.vdouble(0.0),
    electronShift  = cms.vdouble(0.0),
    )
process.rescaledMETRawjet = process.rescaledMETjet.clone( metTag = cms.InputTag("metRecoilCorrector",  "N") )

process.rescaledMETtau = process.rescaledMET.clone(
    unClusterShift = cms.double(0.0),
    tauShift       = cms.vdouble(0.03,0.03),
    muonShift      = cms.vdouble(0.0),
    electronShift  = cms.vdouble(0.0),
    )
process.rescaledMETRawtau = process.rescaledMETtau.clone( metTag = cms.InputTag("metRecoilCorrector",  "N") )

process.rescaledMETelectron = process.rescaledMET.clone(
    unClusterShift = cms.double(0.0),
    tauShift       = cms.vdouble(0.0),
    muonShift      = cms.vdouble(0.0),
    electronShift  = cms.vdouble(0.01,0.025),
    )
process.rescaledMETRawelectron = process.rescaledMETelectron.clone( metTag = cms.InputTag("metRecoilCorrector",  "N") )


process.rescaledTaus = cms.EDProducer(
    "TauRescalerProducer",
    inputCollection = cms.InputTag("tauPtEtaIDAgMuAgElecIsoPtRel"),
    shift           = cms.vdouble(0.03,0.03),
    numOfSigmas     = cms.double(1.0)
    )
process.rescaledElectrons = cms.EDProducer(
    "ElectronRescalerProducer",
    inputCollection = cms.InputTag("elecPtEtaIDIsoPtRel"),
    shift           = cms.vdouble(0.01,0.025),
    numOfSigmas     = cms.double(1.0),
    )
process.rescaledElectronsRel = cms.EDProducer(
    "ElectronRescalerProducer",
    inputCollection = cms.InputTag("elecPtEtaRelID"),
    shift           = cms.vdouble(0.01,0.025),
    numOfSigmas     = cms.double(1.0),
    )

process.rescaledObjects = cms.Sequence(
    process.rescaledMETjet+
    process.rescaledMETtau+
    process.rescaledMETelectron+
    process.rescaledMETRawjet+
    process.rescaledMETRawtau+
    process.rescaledMETRawelectron+
    process.rescaledTaus+
    process.rescaledElectrons+
    process.rescaledElectronsRel
    )

###################################################################################

process.metRecoilCorrector = cms.EDProducer(
    "MEtRecoilCorrectorProducer",
    genParticleTag      = cms.InputTag("genParticles"),
    jetTag              = cms.InputTag("selectedPatJets"),
    metTag              = cms.InputTag("patMETsPFlow"),
    electronTag         = cms.InputTag("elecPtEtaIDIso"),
    muonTag             = cms.InputTag(""),
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
process.load("TauAnalysis.CandidateTools.elecTauPairProduction_cff")
process.diTau = process.allElecTauPairs.clone()
process.diTau.srcLeg1  = cms.InputTag("elecPtEtaIDIso")
process.diTau.srcLeg2  = cms.InputTag("tauPtEtaIDAgMuAgElecIso")
process.diTau.srcMET   = cms.InputTag("metRecoilCorrector",  "N")
process.diTau.srcPrimaryVertex = cms.InputTag("offlinePrimaryVertices")
process.diTau.dRmin12  = cms.double(0.5)
process.diTau.doSVreco = cms.bool(doSVFitReco)
if useMarkov:
    process.diTau.nSVfit.psKine_MEt_int.algorithm = cms.PSet( 
        pluginName = cms.string("nSVfitAlgorithmByIntegration2"),
        pluginType = cms.string("NSVfitAlgorithmByIntegration2"),
        markovChainOptions = cms.PSet(
            mode = cms.string("Metropolis"),
            initMode = cms.string("Gaus"),
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
        max_or_median = cms.string("max"),
        verbosity = cms.int32(0)
    )
    process.diTau.nSVfit.psKine_MEt_int.config.event.srcPrimaryVertex = cms.InputTag("offlinePrimaryVertices")
# CV: disable old fit mode
delattr(process.diTau.nSVfit, "psKine_MEt_logM_fit")
    
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
        process.diTau.nSVfit.psKine_MEt_int.config.event.srcMEt = cms.InputTag("metRecoilCorrector", "N")
        process.diTau.nSVfit.psKine_MEt_int.config.event.likelihoodFunctions[0].srcMEtCovMatrix = cms.InputTag("pfMEtMVACov")
    else :
        process.diTau.nSVfit.psKine_MEt_int.config.event.srcMEt = cms.InputTag("patPFMetByMVA")
        process.diTau.nSVfit.psKine_MEt_int.config.event.likelihoodFunctions[0].srcMEtCovMatrix = cms.InputTag("pfMEtMVACov")

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

process.diTauRaw = process.allElecTauPairs.clone()
process.diTauRaw.srcLeg1  = cms.InputTag("elecPtEtaIDIso")
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
                                          srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
                                          srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                          srcMET  = cms.InputTag("rescaledMETjet",  "UNNNU")
                                          )
process.selectedDiTauJetUp = process.selectedDiTau.clone(src = cms.InputTag("diTauJetUp") )
process.selectedDiTauJetUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauJetUp"))

process.diTauRawJetUp                = process.diTauJetUp.clone(  srcMET  = cms.InputTag("rescaledMETRawjet",  "UNNNU") )
process.selectedDiTauRawJetUp        = process.selectedDiTauJetUp.clone( src = cms.InputTag("diTauRawJetUp") )
process.selectedDiTauRawJetUpCounter = process.selectedDiTauRawCounter.clone(src =  cms.InputTag("selectedDiTauRawJetUp"))

#######################################################################

process.diTauJetDown =  process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                            srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
                                            srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                            srcMET  = cms.InputTag("rescaledMETjet",  "DNNND")
                                            )
process.selectedDiTauJetDown = process.selectedDiTau.clone(src = cms.InputTag("diTauJetDown") )
process.selectedDiTauJetDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauJetDown"))

process.diTauRawJetDown                = process.diTauJetDown.clone(  srcMET  = cms.InputTag("rescaledMETRawjet",  "DNNND") )
process.selectedDiTauRawJetDown        = process.selectedDiTauJetDown.clone( src = cms.InputTag("diTauRawJetDown") )
process.selectedDiTauRawJetDownCounter = process.selectedDiTauRawCounter.clone(src =  cms.InputTag("selectedDiTauRawJetDown"))

#######################################################################

process.diTauMEtResponseUp =  process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                          srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
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
                                            srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
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
                                          srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
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
                                            srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
                                            srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                            srcMET  = cms.InputTag("metRecoilCorrector",  "ResolutionD")
                                            )
process.selectedDiTauMEtResolutionDown = process.selectedDiTau.clone(src = cms.InputTag("diTauMEtResolutionDown") )
process.selectedDiTauMEtResolutionDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauMEtResolutionDown"))

process.diTauRawMEtResolutionDown                = process.diTauMEtResolutionDown.clone(  srcMET  = cms.InputTag("metRecoilCorrector",  "ResolutionU") )
process.selectedDiTauRawMEtResolutionDown        = process.selectedDiTauMEtResolutionDown.clone( src = cms.InputTag("diTauRawMEtResolutionDown") )
process.selectedDiTauRawMEtResolutionDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauRawMEtResolutionDown"))


#######################################################################


process.diTauElecUp = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                          srcLeg1 = cms.InputTag("rescaledElectrons","U"),
                                          srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                          srcMET  = cms.InputTag("rescaledMETelectron","NUNNN")
                                          )
process.selectedDiTauElecUp = process.selectedDiTau.clone(src = cms.InputTag("diTauElecUp") )
process.selectedDiTauElecUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauElecUp"))

process.diTauRawElecUp                = process.diTauElecUp.clone(  srcMET  = cms.InputTag("rescaledMETRawelectron",  "NUNNN") )
process.selectedDiTauRawElecUp        = process.selectedDiTauElecUp.clone( src = cms.InputTag("diTauRawElecUp") )
process.selectedDiTauRawElecUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauRawElecUp"))

#######################################################################

process.diTauElecDown = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                            srcLeg1 = cms.InputTag("rescaledElectrons","D"),
                                            srcLeg2 = cms.InputTag("tauPtEtaIDAgMuAgElecIso"),
                                            srcMET  = cms.InputTag("rescaledMETelectron","NDNNN")
                                            )
process.selectedDiTauElecDown = process.selectedDiTau.clone(src = cms.InputTag("diTauElecDown") )
process.selectedDiTauElecDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauElecDown"))

process.diTauRawElecDown                = process.diTauElecDown.clone(  srcMET  = cms.InputTag("rescaledMETRawelectron",  "NDNNN") )
process.selectedDiTauRawElecDown        = process.selectedDiTauElecDown.clone( src = cms.InputTag("diTauRawElecDown") )
process.selectedDiTauRawElecDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauRawElecDown"))

#######################################################################


process.diTauTauUp = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                         srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
                                         srcLeg2 = cms.InputTag("rescaledTaus", "U"),
                                         srcMET  = cms.InputTag("rescaledMETtau","NNNUN")
                                         )
process.selectedDiTauTauUp = process.selectedDiTau.clone(src = cms.InputTag("diTauTauUp") )
process.selectedDiTauTauUpCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauTauUp"))

process.diTauRawTauUp                = process.diTauRaw.clone(doSVreco = cms.bool(doSVFitReco),
                                                              srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
                                                              srcLeg2 = cms.InputTag("rescaledTaus", "U"),
                                                              srcMET  = cms.InputTag("rescaledMETRawtau","NNNUN")
                                                              )
process.selectedDiTauRawTauUp        = process.selectedDiTauRaw.clone( src = cms.InputTag("diTauRawTauUp") )
process.selectedDiTauRawTauUpCounter = process.selectedDiTauRawCounter.clone(src =  cms.InputTag("selectedDiTauRawTauUp"))

#######################################################################


process.diTauTauDown = process.diTau.clone(doSVreco = cms.bool(doSVFitReco),
                                           srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
                                           srcLeg2 = cms.InputTag("rescaledTaus", "D"),
                                           srcMET  = cms.InputTag("rescaledMETtau","NNNDN")
                                           )
process.selectedDiTauTauDown = process.selectedDiTau.clone(src = cms.InputTag("diTauTauDown") )
process.selectedDiTauTauDownCounter = process.selectedDiTauCounter.clone(src =  cms.InputTag("selectedDiTauTauDown"))

process.diTauRawTauDown                = process.diTauRaw.clone(doSVreco = cms.bool(doSVFitReco),
                                                                srcLeg1 = cms.InputTag("elecPtEtaIDIso"),
                                                                srcLeg2 = cms.InputTag("rescaledTaus", "U"),
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
    (process.diTauElecUp*process.selectedDiTauElecUp*process.selectedDiTauElecUpCounter +
     process.diTauElecDown*process.selectedDiTauElecDown*process.selectedDiTauElecDownCounter) +
    (process.diTauTauUp*process.selectedDiTauTauUp*process.selectedDiTauTauUpCounter +
     process.diTauTauDown*process.selectedDiTauTauDown*process.selectedDiTauTauDownCounter) +
    (process.diTauRaw*process.selectedDiTauRaw*process.selectedDiTauRawCounter)+
    (process.diTauRawJetUp*process.selectedDiTauRawJetUp*process.selectedDiTauRawJetUpCounter +
     process.diTauRawJetDown*process.selectedDiTauRawJetDown*process.selectedDiTauRawJetDownCounter) +
    (process.diTauRawMEtResolutionUp*process.selectedDiTauRawMEtResolutionUp*process.selectedDiTauRawMEtResolutionUpCounter +
     process.diTauRawMEtResolutionDown*process.selectedDiTauRawMEtResolutionDown*process.selectedDiTauRawMEtResolutionDownCounter) +
    (process.diTauRawMEtResponseUp*process.selectedDiTauRawMEtResponseUp*process.selectedDiTauRawMEtResponseUpCounter +
     process.diTauRawMEtResponseDown*process.selectedDiTauRawMEtResponseDown*process.selectedDiTauRawMEtResponseDownCounter) +
    (process.diTauRawElecUp*process.selectedDiTauRawElecUp*process.selectedDiTauRawElecUpCounter +
     process.diTauRawElecDown*process.selectedDiTauRawElecDown*process.selectedDiTauRawElecDownCounter) +
    (process.diTauRawTauUp*process.selectedDiTauRawTauUp*process.selectedDiTauRawTauUpCounter +
     process.diTauRawTauDown*process.selectedDiTauRawTauDown*process.selectedDiTauRawTauDownCounter)

    )

if usePFMEtMVA:
    process.allDiTau.replace(process.diTau,process.pfMEtMVACov*process.diTau)
#    process.allDiTau.replace(process.diTauJetUp,process.pfMEtMVACovJetUp*process.diTauJetUp)
#    process.allDiTau.replace(process.diTauJetDown,process.pfMEtMVACovJetDown*process.diTauJetDown)
#    process.allDiTau.replace(process.diTauElecUp,process.pfMEtMVACovElecUp*process.diTauElecUp)
#    process.allDiTau.replace(process.diTauElecDown,process.pfMEtMVACovElecDown*process.diTauElecDown)
#    process.allDiTau.replace(process.diTauTauUp,process.pfMEtMVACovTauUp*process.diTauTauUp)
#    process.allDiTau.replace(process.diTauTauDown,process.pfMEtMVACovTauDown*process.diTauTauDown)

#######################################################################
'''
#AN, these are not used anywhere
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
'''
###########################################################################
process.tauPtEtaIDAgMuLAgElec = cms.EDFilter( #apply AntiLep cuts
    "PATTauSelector",
    src = cms.InputTag("tauPtEtaIDAgMuLAgElec"),
    cut = cms.string("tauID('againstMuonLoose2')>0.5 "+
                     " && tauID('againstElectronTightMVA3')>0.5 "
                     ),
    filter = cms.bool(False)
    )
process.tauPtEtaIDAgMuAgElecScaled = cms.EDProducer(
    "TauESCorrector",
    tauTag = cms.InputTag("tauPtEtaIDAgMuAgElec")
    #verbose         = cms.bool(True)
    )
if useLepTauPAT:
    process.tauPtEtaIDAgMuAgElecScaled.tauTag = "tauPtEtaIDAgMuLAgElec"
    
process.tauPtEtaIDAgMuAgElecIso  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    cut = cms.string("pt>20 && abs(eta)<2.3"+
                     " && tauID('byLooseIsolationMVA2')>-0.5"+
                     #" && (tauID('againstElectronTight')>0.5 || tauID('againstElectronMVA')>0.5)"
                     #" && tauID('againstElectronMVA')>0.5"
                     " && tauID('againstElectronTightMVA3')>0.5 && tauID('againstMuonLoose2')>0.5"
                     ),
    filter = cms.bool(False)
    )
process.tauPtEtaIDAgMuAgElecIsoPtRel  = cms.EDFilter(
    "PATTauSelector",
    src = cms.InputTag("tauPtEtaIDAgMuAgElec"),
    cut = cms.string("pt>19 && abs(eta)<2.3"+
                     " && tauID('byLooseIsolationMVA2')>-0.5"+
                     #" && (tauID('againstElectronTight')>0.5 || tauID('againstElectronMVA')>0.5 )"
                     #" && tauID('againstElectronMVA')>0.5"
                     " && tauID('againstElectronTightMVA3')>0.5 && tauID('againstMuonLoose2')>0.5"
                     ),
    filter = cms.bool(False)
    )
if useLepTauPAT:
    process.tauPtEtaIDAgMuAgElecIso.src = "tauPtEtaIDAgMuLAgElec"
    process.tauPtEtaIDAgMuAgElecIsoPtRel.src = "tauPtEtaIDAgMuLAgElec"
if applyTauESCorr:
    process.tauPtEtaIDAgMuAgElecIso.src = cms.InputTag("tauPtEtaIDAgMuAgElecScaled")
    process.tauPtEtaIDAgMuAgElecIsoPtRel.src = cms.InputTag("tauPtEtaIDAgMuAgElecScaled")
if runMoriond:
    process.tauPtEtaIDAgMuLAgElec.cut = cms.string("tauID('againstMuonLoose2')>0.5 || tauID('againstMuonLoose')>0.5")
    process.tauPtEtaIDAgMuAgElecIso.cut = cms.string("pt>20 && abs(eta)<2.3"+
                                                     " && (tauID('byLooseIsolationMVA2')>-0.5 || tauID('byLooseIsolationMVA')>-0.5)"+
                                                     " && (tauID('againstElectronMVA')>0.5 || tauID('againstElectronTightMVA3')>0.5)"
                                                    )
    process.tauPtEtaIDAgMuAgElecIsoPtRel.cut = cms.string("pt>19 && abs(eta)<2.3"+
                                                          " && (tauID('byLooseIsolationMVA2')>-0.5 || tauID('byLooseIsolationMVA')>-0.5)"+
                                                          " && (tauID('againstElectronMVA')>0.5 || tauID('againstElectronTightMVA3')>0.5)"
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

#################################################
#process.elecPtEtaID = cms.EDProducer(
#    "ElectronsUserEmbeddedIso",
#    electronTag = cms.InputTag("elecPtEtaID"),
#    )
#################################################

process.elecPtEtaIDIso  = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("elecPtEtaID"),
    #MBcut = cms.string("userFloat('PFRelIsoDB04v3')<0.5 && pt>24 && abs(eta)<2.1"+
    cut = cms.string("userFloat('PFRelIsoDB04v3')<0.5 && pt>14 && abs(eta)<2.1"+
                     "&& userInt('antiConv')>0.5 && userInt('nHits')<1"),
    filter = cms.bool(False)
    )
process.elecPtEtaIDIsoPtRel  = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("elecPtEtaID"),
    #MBcut = cms.string("userFloat('PFRelIsoDB04v3')<0.5 && pt>23 && abs(eta)<2.1"+
    cut = cms.string("userFloat('PFRelIsoDB04v3')<0.5 && pt>13 && abs(eta)<2.1"+
                     "&& userInt('antiConv')>0.5 && userInt('nHits')<1"),
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

#################################################
#process.elecPtEtaRelIDInt = cms.EDProducer(
#            "ElectronsUserEmbeddedIso",
#            electronTag = cms.InputTag("elecPtEtaRelID"),
#            )
#################################################

process.elecPtEtaRelID = process.elecPtEtaIDIso.clone(
    #src = cms.InputTag("elecPtEtaRelIDInt"),
    src = cms.InputTag("elecPtEtaRelID"),
    #MBcut = cms.string("pt>15")
    cut = cms.string("pt>12 && abs(userFloat('dxyWrtPV'))<0.045")
    )
process.elecPtEtaRelIDElecUp   = process.elecPtEtaRelID.clone(
    src = cms.InputTag("rescaledElectronsRel","U")
    )
process.elecPtEtaRelIDElecDown = process.elecPtEtaRelID.clone(
    src = cms.InputTag("rescaledElectronsRel","D")
    )
##Update 3rdLepVeto cuts##
process.electronsForVeto = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("electronsForVeto"),
    cut = cms.string("userFloat('nHits')==0 && userInt('antiConv')>0.5"),
    filter = cms.bool(False)
    )
#########

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
    diTaus             = cms.InputTag("selectedDiTau"),
    jets               = cms.InputTag("selectedPatJets"),
    newJets            = cms.InputTag(""),
    met                = cms.InputTag("metRecoilCorrector",  "N"),
    rawMet             = cms.InputTag("patMETsPFlow"),
    mvaMet             = cms.InputTag("patPFMetByMVA"),
    metCov             = cms.InputTag("pfMEtMVACov"),
    electrons          = cms.InputTag("elecPtEtaIDIso"),
    electronsRel       = cms.InputTag("elecPtEtaRelID"),
    vertices           = cms.InputTag("selectedPrimaryVertices"),
    triggerResults     = cms.InputTag("patTriggerEvent"),
    genParticles       = cms.InputTag("genParticles"),
    genTaus            = cms.InputTag("tauGenJetsSelectorAllHadrons"),
    isMC               = cms.bool(runOnMC),
    deltaRLegJet       = cms.untracked.double(0.5),
    minCorrPt          = cms.untracked.double(15.),
    minJetID           = cms.untracked.double(0.5), # 1=loose,2=medium,3=tight
    verbose            = cms.untracked.bool( False ),
    doElecIsoMVA       = cms.untracked.bool( False ),
    )

if usePFMEtMVA:
    if useRecoil :
        process.elecTauStreamAnalyzer.met = cms.InputTag("metRecoilCorrector",  "N")
    else :
        process.elecTauStreamAnalyzer.met = cms.InputTag("patPFMetByMVA")

process.elecTauStreamAnalyzerRaw   = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRaw"),
    met    = cms.InputTag("metRecoilCorrector",  "N"),
    )


process.elecTauStreamAnalyzerJetUp     = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauJetUp"),
    met    =  cms.InputTag("rescaledMETjet",  "UNNNU"),
    )
process.elecTauStreamAnalyzerJetDown   = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauJetDown"),
    met    =  cms.InputTag("rescaledMETjet",  "DNNND"),
    )
process.elecTauStreamAnalyzerRawJetUp   = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRawJetUp"),
    met    =  cms.InputTag("rescaledMETRawjet",  "UNNNU"),
    )
process.elecTauStreamAnalyzerRawJetDown = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRawJetDown"),
    met    =  cms.InputTag("rescaledMETRawjet",  "DNNND"),
    )

process.elecTauStreamAnalyzerMEtResponseUp   = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauMEtResponseUp"),
    met    =  cms.InputTag("metRecoilCorrector",  "ResponseU"),
    )
process.elecTauStreamAnalyzerMEtResponseDown = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauMEtResponseDown"),
    met    =  cms.InputTag("metRecoilCorrector",  "ResponseD"),
    )
process.elecTauStreamAnalyzerRawMEtResponseUp   = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRawMEtResponseUp"),
    met    =  cms.InputTag("metRecoilCorrector",  "ResponseU"),
    )
process.elecTauStreamAnalyzerRawMEtResponseDown = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRawMEtResponseDown"),
    met    =  cms.InputTag("metRecoilCorrector",  "ResponseD"),
    )

process.elecTauStreamAnalyzerMEtResolutionUp  = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauMEtResolutionUp"),
    met    =  cms.InputTag("metRecoilCorrector",  "ResolutionU"),
    )
process.elecTauStreamAnalyzerMEtResolutionDown = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauMEtResolutionDown"),
    met    =  cms.InputTag("metRecoilCorrector",  "ResolutionD"),
    )
process.elecTauStreamAnalyzerRawMEtResolutionUp   = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRawMEtResolutionUp"),
    met    =  cms.InputTag("metRecoilCorrector",  "ResolutionU"),
    )
process.elecTauStreamAnalyzerRawMEtResolutionDown = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRawMEtResolutionDown"),
    met    =  cms.InputTag("metRecoilCorrector",  "ResolutionD"),
    )


process.elecTauStreamAnalyzerElecUp    = process.elecTauStreamAnalyzer.clone(
    diTaus       =  cms.InputTag("selectedDiTauElecUp"),
    met          =  cms.InputTag("rescaledMETelectron","NUNNN"),
    electrons    =  cms.InputTag("elecPtEtaIDIsoElecUp"),
    electronsRel =  cms.InputTag("elecPtEtaRelIDElecUp"),
    )
process.elecTauStreamAnalyzerElecDown  = process.elecTauStreamAnalyzer.clone(
    diTaus       =  cms.InputTag("selectedDiTauElecDown"),
    met          =  cms.InputTag("rescaledMETelectron","NDNNN"),
    electrons    =  cms.InputTag("elecPtEtaIDIsoElecDown"),
    electronsRel =  cms.InputTag("elecPtEtaRelIDElecDown"),
    )
process.elecTauStreamAnalyzerRawElecUp    = process.elecTauStreamAnalyzer.clone(
    diTaus   =  cms.InputTag("selectedDiRawTauElecUp"),
    met      =  cms.InputTag("rescaledMETRawelectron","NUNNN"),
    muons    =  cms.InputTag("elecPtEtaIDIsoElecUp"),
    muonsRel =  cms.InputTag("elecPtEtaRelIDElecUp"),
    )
process.elecTauStreamAnalyzerRawElecDown  = process.elecTauStreamAnalyzer.clone(
    diTaus   =  cms.InputTag("selectedDiTauRawElecDown"),
    met      =  cms.InputTag("rescaledMETRawelectron","NDNNN"),
    muons    =  cms.InputTag("elecPtEtaIDIsoElecDown"),
    muonsRel =  cms.InputTag("elecPtEtaRelIDElecDown"),
    )


process.elecTauStreamAnalyzerTauUp     = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauTauUp"),
    met    =  cms.InputTag("rescaledMETtau","NNNUN")
    )
process.elecTauStreamAnalyzerTauDown   = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauTauDown"),
    met    =  cms.InputTag("rescaledMETtau","NNNDN")
    )
process.elecTauStreamAnalyzerRawTauUp   = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRawTauUp"),
    met    =  cms.InputTag("rescaledMETRawtau","NNNUN"),
    )
process.elecTauStreamAnalyzerRawTauDown = process.elecTauStreamAnalyzer.clone(
    diTaus =  cms.InputTag("selectedDiTauRawTauDown"),
    met    =  cms.InputTag("rescaledMETRawtau","NNNDN")
    )


process.allAnalyzers = cms.Sequence(
    process.elecTauStreamAnalyzer+
    process.elecTauStreamAnalyzerJetUp+
    process.elecTauStreamAnalyzerJetDown+
    process.elecTauStreamAnalyzerMEtResponseUp+
    process.elecTauStreamAnalyzerMEtResponseDown+
    process.elecTauStreamAnalyzerMEtResolutionUp+
    process.elecTauStreamAnalyzerMEtResolutionDown+
    process.elecTauStreamAnalyzerElecUp+
    process.elecTauStreamAnalyzerElecDown+
    process.elecTauStreamAnalyzerTauUp+
    process.elecTauStreamAnalyzerTauDown+
    process.elecTauStreamAnalyzerRaw+
    process.elecTauStreamAnalyzerRawJetUp+
    process.elecTauStreamAnalyzerRawJetDown+
    process.elecTauStreamAnalyzerRawMEtResponseUp+
    process.elecTauStreamAnalyzerRawMEtResponseDown+
    process.elecTauStreamAnalyzerRawMEtResolutionUp+
    process.elecTauStreamAnalyzerRawMEtResolutionDown+
    process.elecTauStreamAnalyzerRawElecUp+
    process.elecTauStreamAnalyzerRawElecDown+
    process.elecTauStreamAnalyzerRawTauUp+
    process.elecTauStreamAnalyzerRawTauDown
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
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #process.elecPtEtaID*
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.electronsForVeto *
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.produceType1corrPFMEt*
    process.producePFMEtNoPileUp*
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    process.pfMEtMVACov*
    process.diTau*process.selectedDiTau*process.selectedDiTauCounter*
    process.QuarkGluonTagger* #quark/gluon jets
    process.elecTauStreamAnalyzer
    )
process.seqJetUp = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #process.elecPtEtaID*
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.electronsForVeto *
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    process.rescaledMETjet *
    process.pfMEtMVACov*
    process.diTauJetUp*process.selectedDiTauJetUp*process.selectedDiTauJetUpCounter*
    process.elecTauStreamAnalyzerJetUp
    )
process.seqJetDown = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #process.elecPtEtaID*
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.electronsForVeto *
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    process.rescaledMETjet *
    process.pfMEtMVACov*
    process.diTauJetDown*process.selectedDiTauJetDown*process.selectedDiTauJetDownCounter*
    process.elecTauStreamAnalyzerJetDown
    )

process.seqMEtResolutionUp = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #process.elecPtEtaID*
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.electronsForVeto *
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    process.pfMEtMVACov*
    process.diTauMEtResolutionUp*process.selectedDiTauMEtResolutionUp*process.selectedDiTauMEtResolutionUpCounter*
    process.elecTauStreamAnalyzerMEtResolutionUp
    )
process.seqMEtResolutionDown = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #process.elecPtEtaID*
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.electronsForVeto *
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    process.pfMEtMVACov*
    process.diTauMEtResolutionDown*process.selectedDiTauMEtResolutionDown*process.selectedDiTauMEtResolutionDownCounter*
    process.elecTauStreamAnalyzerMEtResolutionDown
    )
    
process.seqMEtResponseUp = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #process.elecPtEtaID*
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.electronsForVeto *
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    process.pfMEtMVACov*
    process.diTauMEtResponseUp*process.selectedDiTauMEtResponseUp*process.selectedDiTauMEtResponseUpCounter*
    process.elecTauStreamAnalyzerMEtResponseUp
    )
process.seqMEtResponseDown = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #process.elecPtEtaID*
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.electronsForVeto *
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    process.pfMEtMVACov*
    process.diTauMEtResponseDown*process.selectedDiTauMEtResponseDown*process.selectedDiTauMEtResponseDownCounter*
    process.elecTauStreamAnalyzerMEtResponseDown
    )

process.seqElecUp = cms.Sequence(
    process.allEventsFilter*
    #process.elecPtEtaID*
    process.elecPtEtaIDIsoPtRel *
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    (process.rescaledMETelectron+process.rescaledElectrons+process.rescaledElectronsRel)*
    (process.elecPtEtaIDIsoElecUp*process.elecPtEtaIDIsoElecUpCounter) *
    process.elecPtEtaRelIDElecUp *
    process.electronsForVeto *
    process.pfMEtMVACov*
    process.diTauElecUp*process.selectedDiTauElecUp*process.selectedDiTauElecUpCounter*
    process.elecTauStreamAnalyzerElecUp
    )
process.seqElecDown = cms.Sequence(
    process.allEventsFilter*
    #process.elecPtEtaID*
    process.elecPtEtaIDIsoPtRel *
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    (process.rescaledMETelectron+process.rescaledElectrons+process.rescaledElectronsRel)*
    (process.elecPtEtaIDIsoElecDown*process.elecPtEtaIDIsoElecDownCounter) *
    process.elecPtEtaRelIDElecDown *
    process.electronsForVeto *
    process.pfMEtMVACov*
    process.diTauElecDown*process.selectedDiTauElecDown*process.selectedDiTauElecDownCounter*
    process.elecTauStreamAnalyzerElecDown
    )

process.seqTauUp = cms.Sequence(
    process.allEventsFilter*
    #process.elecPtEtaID*
    (process.elecPtEtaIDIso*process.elecPtEtaIDIsoCounter) *
    process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIsoPtRel*
    process.elecPtEtaRelID *
    process.electronsForVeto *
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    (process.rescaledMETtau+process.rescaledTaus)*
    (process.tauPtEtaIDAgMuAgElecIsoTauUp*process.tauPtEtaIDAgMuAgElecIsoTauUpCounter)*
    process.pfMEtMVACov*
    process.diTauTauUp*process.selectedDiTauTauUp*process.selectedDiTauTauUpCounter*
    process.QuarkGluonTagger* #quark/gluon jets    
    process.elecTauStreamAnalyzerTauUp
    )
process.seqTauDown = cms.Sequence(
    process.allEventsFilter*
    #process.elecPtEtaID*
    (process.elecPtEtaIDIso*process.elecPtEtaIDIsoCounter) *
    process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIsoPtRel*
    process.elecPtEtaRelID *
    process.electronsForVeto *
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*
    (process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    (process.rescaledMETtau+process.rescaledTaus)*
    (process.tauPtEtaIDAgMuAgElecIsoTauDown*process.tauPtEtaIDAgMuAgElecIsoTauDownCounter)*
    process.pfMEtMVACov*
    process.diTauTauDown*process.selectedDiTauTauDown*process.selectedDiTauTauDownCounter*
    process.QuarkGluonTagger* #quark/gluon jets    
    process.elecTauStreamAnalyzerTauDown
    )


if not usePFMEtMVA:
    process.seqNominal.remove(process.pfMEtMVACov)
    process.seqJetUp.remove(process.pfMEtMVACov)
    process.seqJetDown.remove(process.pfMEtMVACov)
    process.seqMEtResolutionUp.remove(process.pfMEtMVACov)
    process.seqMEtResolutionDown.remove(process.pfMEtMVACov)
    process.seqMEtResponseUp.remove(process.pfMEtMVACov)
    process.seqMEtResponseDown.remove(process.pfMEtMVACov)
    process.seqElecUp.remove(process.pfMEtMVACov)
    process.seqElecDown.remove(process.pfMEtMVACov)
    process.seqTauUp.remove(process.pfMEtMVACov)
    process.seqTauDown.remove(process.pfMEtMVACov)


process.seqRawNominal = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #process.elecPtEtaID*
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.puJetIdSequence *
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    #process.pfMEtMVACov*
    process.diTauRaw*process.selectedDiTauRaw*process.selectedDiTauRawCounter*
    process.elecTauStreamAnalyzerRaw
    )

process.seqRawJetUp = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #process.elecPtEtaID*
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.puJetIdSequence *
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    process.rescaledMETRawjet *
    #process.pfMEtMVACov*
    process.diTauRawJetUp*process.selectedDiTauRawJetUp*process.selectedDiTauRawJetUpCounter*
    process.elecTauStreamAnalyzerRawJetUp
    )

process.seqRawJetDown = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #process.elecPtEtaID*
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.puJetIdSequence *
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    process.rescaledMETRawjet *
    #process.pfMEtMVACov*
    process.diTauRawJetDown*process.selectedDiTauRawJetDown*process.selectedDiTauRawJetDownCounter*
    process.elecTauStreamAnalyzerRawJetDown
    )

process.seqRawMEtResolutionUp = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #process.elecPtEtaID*
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.puJetIdSequence *
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    #process.pfMEtMVACov*
    process.diTauRawMEtResolutionUp*process.selectedDiTauRawMEtResolutionUp*process.selectedDiTauRawMEtResolutionUpCounter*
    process.elecTauStreamAnalyzerRawMEtResolutionUp
    )

process.seqRawMEtResolutionDown = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #process.elecPtEtaID*
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.puJetIdSequence *
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    #process.pfMEtMVACov*
    process.diTauRawMEtResolutionDown*process.selectedDiTauRawMEtResolutionDown*process.selectedDiTauRawMEtResolutionDownCounter*
    process.elecTauStreamAnalyzerRawMEtResolutionDown
    )

process.seqRawMEtResponseUp = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #process.elecPtEtaID*
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.puJetIdSequence *
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    #process.pfMEtMVACov*
    process.diTauRawMEtResponseUp*process.selectedDiTauRawMEtResponseUp*process.selectedDiTauRawMEtResponseUpCounter*
    process.elecTauStreamAnalyzerRawMEtResponseUp
    )

process.seqRawMEtResponseDown = cms.Sequence(
    process.allEventsFilter*
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    #process.elecPtEtaID*
    (process.elecPtEtaIDIso *process.elecPtEtaIDIsoCounter) *
    process.elecPtEtaRelID *
    process.puJetIdSequence *
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    #process.pfMEtMVACov*
    process.diTauRawMEtResponseDown*process.selectedDiTauRawMEtResponseDown*process.selectedDiTauRawMEtResponseDownCounter*
    process.elecTauStreamAnalyzerRawMEtResponseDown
    )

process.seqRawElecUp = cms.Sequence(
    process.allEventsFilter*
    #process.elecPtEtaID*
    process.elecPtEtaIDIsoPtRel *
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    process.puJetIdSequence *
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    (process.rescaledMETRawelectron+process.rescaledElectrons+process.rescaledElectronsRel)*
    (process.elecPtEtaIDIsoElecUp*process.elecPtEtaIDIsoElecUpCounter) *
    process.elecPtEtaRelIDElecUp *
    #process.pfMEtMVACov*
    process.diTauRawElecUp*process.selectedDiTauRawElecUp*process.selectedDiTauRawElecUpCounter*
    process.elecTauStreamAnalyzerRawElecUp
    )

process.seqRawElecDown = cms.Sequence(
    process.allEventsFilter*
    #process.elecPtEtaID*
    process.elecPtEtaIDIsoPtRel *
    (process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*
     process.tauPtEtaIDAgMuAgElecIso*process.tauPtEtaIDAgMuAgElecIsoCounter)*
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    process.puJetIdSequence *
    (process.rescaledMETRawelectron+process.rescaledElectrons+process.rescaledElectronsRel)*
    (process.elecPtEtaIDIsoElecDown*process.elecPtEtaIDIsoElecDownCounter) *
    process.elecPtEtaRelIDElecDown *
    #process.pfMEtMVACov*
    process.diTauRawElecDown*process.selectedDiTauRawElecDown*process.selectedDiTauRawElecDownCounter*
    process.elecTauStreamAnalyzerRawElecDown
    )

process.seqRawTauUp = cms.Sequence(
    process.allEventsFilter*
    #process.elecPtEtaID*
    (process.elecPtEtaIDIso*process.elecPtEtaIDIsoCounter) *
    process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIsoPtRel*
    process.elecPtEtaRelID *
    process.puJetIdSequence *
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    (process.rescaledMETRawtau+process.rescaledTaus)*
    (process.tauPtEtaIDAgMuAgElecIsoTauUp*process.tauPtEtaIDAgMuAgElecIsoTauUpCounter)*
    #process.pfMEtMVACov*
    process.diTauRawTauUp*process.selectedDiTauRawTauUp*process.selectedDiTauRawTauUpCounter*
    process.elecTauStreamAnalyzerRawTauUp
    )

process.seqRawTauDown = cms.Sequence(
    process.allEventsFilter*
    (process.elecPtEtaIDIso*process.elecPtEtaIDIsoCounter) *
    process.tauPtEtaIDAgMuLAgElec*process.tauPtEtaIDAgMuAgElecScaled*process.tauPtEtaIDAgMuAgElecIsoPtRel*
    #process.elecPtEtaID*
    process.elecPtEtaRelID *
    process.puJetIdSequence *
    process.produceCaloMEtNoHF*
    process.metRecoilCorrector*
    (process.rescaledMETRawtau+process.rescaledTaus)*
    (process.tauPtEtaIDAgMuAgElecIsoTauDown*process.tauPtEtaIDAgMuAgElecIsoTauDownCounter)*
    #process.pfMEtMVACov*
    process.diTauRawTauDown*process.selectedDiTauRawTauDown*process.selectedDiTauRawTauDownCounter*
    process.elecTauStreamAnalyzerRawTauDown
    )
if runMoriond:
    process.seqNominal.remove(process.produceType1corrPFMEt)
    process.seqNominal.remove(process.producePFMEtNoPileUp)
    

#######################################################################

if runOnMC:
    process.pNominal            = cms.Path( process.seqNominal )
    process.pTauUp              = cms.Path( process.seqTauUp)
    process.pTauDown            = cms.Path( process.seqTauDown )
    #process.pElecUp                  = cms.Path( process.seqElecUp)    # TO BE STUDIED LATER
    #process.pElecDown                = cms.Path( process.seqElecDown)  # TO BE STUDIED LATER
    ####
    #process.pJetUp                 = cms.Path( process.seqJetUp   )
    #process.pJetDown               = cms.Path( process.seqJetDown )
    #process.pMEtResolutionUp       = cms.Path( process.seqMEtResolutionUp )
    #process.pMEtResolutionDown     = cms.Path( process.seqMEtResolutionDown )
    #process.pMEtResponseUp         = cms.Path( process.seqMEtResponseUp)
    #process.pMEtResponseDown       = cms.Path( process.seqMEtResponseDown)
    #process.pRawNominal         = cms.Path( process.seqRawNominal )
    #process.pRawJetUp              = cms.Path( process.seqRawJetUp   )
    #process.pRawJetDown            = cms.Path( process.seqRawJetDown )
    #process.pRawMEtResolutionUp    = cms.Path( process.seqRawMEtResolutionUp )
    #process.pRawMEtResolutionDown  = cms.Path( process.seqRawMEtResolutionDown )
    #process.pRawMEtResponseUp      = cms.Path( process.seqRawMEtResponseUp)
    #process.pRawMEtResponseDown    = cms.Path( process.seqRawMEtResponseDown)
    #process.pRawElecUp               = cms.Path( process.seqRawElecUp)
    #process.pRawElecDown             = cms.Path( process.seqRawElecDown)
    #process.pRawTauUp           = cms.Path( process.seqRawTauUp )
    #process.pRawTauDown         = cms.Path( process.seqRawTauDown )

else:
    process.pNominal            = cms.Path( process.seqNominal )
    if runOnEmbed:
        process.pTauUp          = cms.Path( process.seqTauUp)
        process.pTauDown        = cms.Path( process.seqTauDown )
        process.pElecUp         = cms.Path( process.seqElecUp)
        process.pElecDown       = cms.Path( process.seqElecDown)

#######################################################################

process.out = cms.OutputModule(
    "PoolOutputModule",
    outputCommands = cms.untracked.vstring( 'drop *'),
    fileName = cms.untracked.string('patTuplesSkimmed_ElecTauStream.root'),
    )

process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string("treeElecTauStream.root")
    )

# before starting to process 1st event, print event content
##process.printEventContent = cms.EDAnalyzer("EventContentAnalyzer")
##process.filterFirstEvent = cms.EDFilter("EventCountFilter",
##    numEvents = cms.int32(1)
##)
##process.printFirstEventContentPath = cms.Path(process.filterFirstEvent + process.printEventContent)

process.outpath = cms.EndPath()

##
#processDumpFile = open('runElecTauStreamAnalyzer_Moriond2013_NewTauES.dump', 'w')
#print >> processDumpFile, process.dumpPython()
