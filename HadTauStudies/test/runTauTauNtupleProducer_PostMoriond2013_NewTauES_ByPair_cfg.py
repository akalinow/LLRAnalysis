import FWCore.ParameterSet.Config as cms

process = cms.Process("TAUTAUANA")

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

#from Configuration.PyReleaseValidation.autoCond import autoCond
#process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

runOnMC        = False
runOnEmbed     = True
embedType      = "PfEmbed" #"PfEmbed" or "RhEmbed"
reRunPatJets   = True
applyTauESCorr = True 
doSVFitReco    = True
usePFMEtMVA    = True
useRecoil      = True
useMarkov      = False

# CV: flags for cutting low mass tail from MSSM Higgs -> tautau samples
#    (cross-sections provided by LHC Higgs XS working group refer to nominal Higgs mass)
applyHiggsMassCut = False
nomHiggsMass = 80.

# CV: flags that allow to run separate jobs
#     for nominal tau energy scale and +1 sigma, -1 sigma shifts
#     in order not to exceed computing time limits of lxbatch queues
runNominal     = True
runTauEnUp     = False
runTauEnDown   = False

sampleName = None

#--------------------------------------------------------------------------------
# define "hooks" for replacing configuration parameters
# in case running jobs on the CERN batch system/grid
#
#__runOnMC = $runOnMC
#__runOnEmbed = $runOnEmbed
#__embedType = $embedType
#__applyHiggsMassCut = $applyHiggsMassCut
#__nomHiggsMass = $nomHiggsMass
#__runNominal = $runNominal
#__runTauEnUp = $runTauEnUp
#__runTauEnDown = $runTauEnDown
#__sampleName = $sampleName
#
#--------------------------------------------------------------------------------

if runOnMC:
    print "Running on MC"
else:
    print "Running on Data"

if (not runOnMC) and (not runOnEmbed) and applyTauESCorr:
    print "Running on Data, Tau ESCorr should be switched off"
    applyTauESCorr=False 

if useMarkov:
    print "Use SVFit with Markov chain integration"
else:
    print "Use SVFit with VEGAS integration"
    
if runOnMC:
    process.GlobalTag.globaltag = cms.string('START53_V23::All')
else:
    process.GlobalTag.globaltag = cms.string('FT_53_V21_AN4::All')
    
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        ##'/store/user/veelken/CMSSW_5_3_x/PATTuples/AHtoTauTau/2013Dec10/HiggsSUSYGluGlu130/patTuple_HadTauStream_1_2_NrD.root'
        ##'file:/data1/veelken/tmp/patTuple_HadTauStream.root'
        ##'file:/data1/veelken/CMSSW_5_3_x/PATTuples/patTuple_HadTauStream_selEvents_simHiggsSUSYGluGlu130_tautau_selEventFromRiccardo.root'
        ##'file:/afs/cern.ch/user/v/veelken/scratch0/CMSSW_5_3_14/src/TauAnalysis/Skimming/test/selEvents_HiggsSUSYGluGlu80_fromCecile.root'
        'file:/afs/cern.ch/user/v/veelken/scratch0/CMSSW_5_3_14/src/TauAnalysis/Skimming/test/selEvents_DYJets_Embedded_genMassEq0.root' 
        ##'file:/tmp/veelken/patTuple_HadTauStream_231_1_reu.root'                        
    ),
    ##eventsToProcess = cms.untracked.VEventRange(
    ##    '1:1435:918133'
    ##)
)

process.allEventsFilter = cms.EDFilter("AllEventsFilter")

process.genFilterSequence = cms.Sequence()
if applyHiggsMassCut:
    process.genHiggsMassFilter = cms.EDFilter("GenParticleSelector",
        src = cms.InputTag('genParticles'),                                      
        cut = cms.string("(pdgId = 25 | pdgId = 35 | pdgId = 36) & mass > %f & mass < %f" % (0.7*nomHiggsMass, 1.3*nomHiggsMass)),
        stableOnly = cms.bool(False),                                      
        filter = cms.bool(True)
    )
    process.genFilterSequence += process.genHiggsMassFilter

#--------------------------------------------------------------------------------
# produce genMET

process.produceGenMEt = cms.Sequence()

if runOnMC:
    process.load("RecoMET.Configuration.GenMETParticles_cff")
    process.produceGenMEt += process.genParticlesForMETAllVisible

    process.load("RecoMET.METProducers.genMetTrue_cfi")
    process.genMetFromGenParticles = process.genMetTrue.clone(
        src = cms.InputTag('genParticlesForMETAllVisible'),
        alias = cms.string('genMetFromGenParticles')
    )
    process.produceGenMEt += process.genMetFromGenParticles
#--------------------------------------------------------------------------------

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
    rho = cms.InputTag("kt6PFJets", "rho"),
    jec = cms.string("AK5PF"),
    applyJec = cms.bool(True),
    inputIsCorrected = cms.bool(False),
    residualsFromTxt = cms.bool(False),
    residualsTxt = cms.FileInPath("RecoJets/JetProducers/data/dummy.txt"),
)
process.puJetIdSequence = cms.Sequence(process.puJetMva)

#----------------------------------------------------------------------------------

#process.puJetIdAndMvaMet = cms.Sequence(process.puJetIdSequence *
#                                        (process.pfMEtMVAsequence*process.patPFMetByMVA))

#######################################################################
# ReRun patJets
process.runPatJets = cms.Sequence()

if reRunPatJets:
    #process.load("PhysicsTools.PatAlgos.producersLayer1.jetProducer_cff")
    process.load("PhysicsTools.PatAlgos.patSequences_cff")
    from PhysicsTools.PatAlgos.tools.jetTools import *

    switchJetCollection(process,cms.InputTag('ak5PFJets'),
                        doJTA        = True,
                        doBTagging   = True,
                        jetCorrLabel = ('AK5PF', [ 'L2Relative', 'L3Absolute',]),
                        doType1MET   = False,
                        genJetCollection=cms.InputTag("ak5GenJets"),
                        doJetID      = True,
                        jetIdLabel   = 'ak5',
                        outputModules = []
    )

    JEClevels = cms.vstring([ 'L2Relative', 'L3Absolute' ])
    if runOnMC:
        JEClevels = [ 'L1FastJet', 'L2Relative', 'L3Absolute' ]
    else:
        JEClevels = [ 'L1FastJet', 'L2Relative', 'L3Absolute', 'L2L3Residual' ]
        process.makePatJets.remove(process.patJetPartonMatch)
        process.makePatJets.remove(process.patJetGenJetMatch)
        process.makePatJets.remove(process.patJetFlavourId)
        process.patJets.addGenJetMatch = False
        process.patJets.addGenPartonMatch = False
        process.patJets.addPartonJetMatch = False
        process.patJets.getJetMCFlavour = False
        
    process.patJetCorrFactors.levels = JEClevels
    process.patJetCorrFactors.rho = cms.InputTag('kt6PFJets', 'rho')
    process.patJetCorrFactors.useRho = True
    
    process.runPatJets = cms.Sequence(process.makePatJets*process.selectedPatJets)

#######################################################################

# CV: add b-jet energy regression scale-factors

process.load("LLRAnalysis.HadTauStudies2b2tau.patJetBJetEnRegEmbedder_cfi")
##process.selectedPatJetsBJetEnReg.inputFileName_jetPtLt100 = cms.string("/afs/cern.ch/user/o/ojalvo/public/bJetRegression/Example/factoryJetRegNewGenJetsAll_BDT_LT100.weights.xml")
##process.selectedPatJetsBJetEnReg.inputFileName_jetPtGt100 = cms.string("/afs/cern.ch/user/o/ojalvo/public/bJetRegression/Example/factoryJetRegNewGenJetsAll_BDT_GT100.weights.xml")
##process.selectedPatJetsBJetEnReg.inputFileType = cms.string("XML")
process.selectedPatJetsBJetEnReg.inputFileName_jetPtLt100 = cms.FileInPath("LLRAnalysis/HadTauStudies2b2tau/data_nocrab/gbrBjetEnReg_allJetPt.root")
process.selectedPatJetsBJetEnReg.gbrForestName_jetPtLt100 = cms.string("gbrBjetEnReg") 
process.selectedPatJetsBJetEnReg.inputFileName_jetPtGt100 = cms.FileInPath("LLRAnalysis/HadTauStudies2b2tau/data_nocrab/gbrBjetEnReg_allJetPt.root")
process.selectedPatJetsBJetEnReg.gbrForestName_jetPtGt100 = cms.string("gbrBjetEnReg")
process.selectedPatJetsBJetEnReg.verbosity = cms.int32(0)
process.runPatJets += process.selectedPatJetsBJetEnReg

###################################################################################

#--------------------------------------------------------------------------------
# CV: add extra working-points to pat::Tau collection

uncorrectedTaus = "tauPtEtaID"

process.uncorrectedTausAddedWPs = cms.EDProducer("PATTauWPEmbedder",
    src = cms.InputTag(uncorrectedTaus),                                                
    inputFileName = cms.FileInPath('LLRAnalysis/HadTauStudies/data/wpDiscriminationByIsolationMVA3_oldDMwLT_21Jan2015.root'),
    mvaOutput_normalization = cms.string("mvaOutput_normalization_oldDMwLT"),
    discriminator = cms.string("byIsolationMVA3oldDMwLTraw"),
    WPs = cms.PSet(
        byVLooseIsolationMVA3oldDMwLTconstEff = cms.PSet(
            cut = cms.string("oldDMwLTEff90"),
            variable = cms.string("pt")
        ),
        byLooseIsolationMVA3oldDMwLTconstEff = cms.PSet(
            cut = cms.string("oldDMwLTEff80"),
            variable = cms.string("pt")
        ),
        byMediumIsolationMVA3oldDMwLTconstEff = cms.PSet(
            cut = cms.string("oldDMwLTEff70"),
            variable = cms.string("pt")
        ),
        byTightIsolationMVA3oldDMwLTconstEff = cms.PSet(
            cut = cms.string("oldDMwLTEff60"),
            variable = cms.string("pt")
        ),
        byVTightIsolationMVA3oldDMwLTconstEff = cms.PSet(
            cut = cms.string("oldDMwLTEff50"),
            variable = cms.string("pt")
        ),
        byVVTightIsolationMVA3oldDMwLTconstEff = cms.PSet(
            cut = cms.string("oldDMwLTEff40"),
            variable = cms.string("pt")
        ),                                                     
        byVLooseIsolationMVA3oldDMwLTconstFR = cms.PSet(
            cut = cms.string("oldDMwLTFR050"),
            variable = cms.string("pt")
        ),
        byLooseIsolationMVA3oldDMwLTconstFR = cms.PSet(
            cut = cms.string("oldDMwLTFR020"),
            variable = cms.string("pt")
        ),
        byMediumIsolationMVA3oldDMwLTconstFR = cms.PSet(
            cut = cms.string("oldDMwLTFR010"),
            variable = cms.string("pt")
        ),
        byTightIsolationMVA3oldDMwLTconstFR = cms.PSet(
            cut = cms.string("oldDMwLTFR005"),
            variable = cms.string("pt")
        ),
        byVTightIsolationMVA3oldDMwLTconstFR = cms.PSet(
            cut = cms.string("oldDMwLTFR002"),
            variable = cms.string("pt")
        ),
        byVVTightIsolationMVA3oldDMwLTconstFR = cms.PSet(
            cut = cms.string("oldDMwLTFR001"),
            variable = cms.string("pt")
        ),
        byVLooseIsolationMVA3oldDMwLTplatFR = cms.PSet(
            cut = cms.string("oldDMwLTMinEff90minFR0200"),
            variable = cms.string("pt")
        ),
        byLooseIsolationMVA3oldDMwLTplatFR = cms.PSet(
            cut = cms.string("oldDMwLTMinEff80minFR0100"),
            variable = cms.string("pt")
        ),
        byMediumIsolationMVA3oldDMwLTplatFR = cms.PSet(
            cut = cms.string("oldDMwLTMinEff70minFR0050"),
            variable = cms.string("pt")
        ),
        byTightIsolationMVA3oldDMwLTplatFR = cms.PSet(
            cut = cms.string("oldDMwLTMinEff60minFR0020"),
            variable = cms.string("pt")
        ),
        byVTightIsolationMVA3oldDMwLTplatFR = cms.PSet(
            cut = cms.string("oldDMwLTMinEff50minFR0010"),
            variable = cms.string("pt")
        ),
        byVVTightIsolationMVA3oldDMwLTplatFR = cms.PSet(
            cut = cms.string("oldDMwLTMinEff40minFR0005"),
            variable = cms.string("pt")
        )
    ),
    verbosity = cms.int32(0)
)

uncorrectedTaus = "uncorrectedTausAddedWPs"
#--------------------------------------------------------------------------------    

#--------------------------------------------------------------------------------
# CV: rerun generator level matching for pat::Taus in Embedded samples
process.applyTauES = cms.Sequence()

##uncorrectedTaus = "tauPtEtaID"
if runOnEmbed:
    from PhysicsTools.PatAlgos.mcMatchLayer0.tauMatch_cfi import tauMatch, tauGenJetMatch
    process.tauMatchEmbeddedRECO = tauMatch.clone(
        src = cms.InputTag(uncorrectedTaus),
        matched = cms.InputTag("genParticles::EmbeddedRECO")
    )
    process.applyTauES += process.tauMatchEmbeddedRECO

    from PhysicsTools.JetMCAlgos.TauGenJets_cfi import tauGenJets
    process.tauGenJetsEmbeddedRECO = tauGenJets.clone(
        GenParticles = cms.InputTag("genParticles::EmbeddedRECO")
    )
    process.applyTauES += process.tauGenJetsEmbeddedRECO
    from PhysicsTools.JetMCAlgos.TauGenJetsDecayModeSelectorAllHadrons_cfi import tauGenJetsSelectorAllHadrons
    process.tauGenJetsSelectorAllHadronsEmbeddedRECO = tauGenJetsSelectorAllHadrons.clone(
        src = cms.InputTag("tauGenJetsEmbeddedRECO")
    )
    process.applyTauES += process.tauGenJetsSelectorAllHadronsEmbeddedRECO
    process.tauGenJetMatchEmbeddedRECO = tauGenJetMatch.clone(
        src = cms.InputTag(uncorrectedTaus),
        matched = cms.InputTag("tauGenJetsSelectorAllHadronsEmbeddedRECO")
    )
    process.applyTauES += process.tauGenJetMatchEmbeddedRECO
    
    process.tauPtEtaIDGenMatched = cms.EDProducer("PATTauGenMatchEmbedder",
        src = cms.InputTag(uncorrectedTaus),
        srcGenParticleMatch = cms.InputTag("tauMatchEmbeddedRECO"),
        srcGenJetMatch = cms.InputTag("tauGenJetMatchEmbeddedRECO")
    )
    process.applyTauES += process.tauPtEtaIDGenMatched

    uncorrectedTaus = "tauPtEtaIDGenMatched"
#--------------------------------------------------------------------------------    

process.tauPtEtaIDScaled = cms.EDProducer("TauESCorrector",
    tauTag = cms.InputTag(uncorrectedTaus),
    ##verbose = cms.bool(True)                
)
process.applyTauES += process.tauPtEtaIDScaled

correctedTaus = uncorrectedTaus
if applyTauESCorr:
    correctedTaus = "tauPtEtaIDScaled"

###################################################################################

process.tauPtEtaIDIso = cms.EDFilter("PATTauSelector",
    src = cms.InputTag(correctedTaus),
    cut = cms.string(
        "pt > 40 & abs(eta) < 2.3" +
        " & tauID('decayModeFindingOldDMs') > 0.5" +                                  
        " & (tauID('byLooseCombinedIsolationDeltaBetaCorr3Hits') > 0.5 | tauID('byCombinedIsolationDeltaBetaCorrRaw3Hits') < 4.0 | tauID('byLooseIsolationMVA3oldDMwLT') > 0.5 | tauID('byVLooseIsolationMVA3oldDMwLT') > 0.5)"
    ),
    filter = cms.bool(False)
)

process.tauPtEtaIDIsoFilter = cms.EDFilter("CandViewCountFilter",
    src = cms.InputTag("tauPtEtaIDIso"),
    minNumber = cms.uint32(2),
    maxNumber = cms.uint32(999)
)

process.rescaledTaus = cms.EDProducer("TauRescalerProducer",
    inputCollection = cms.InputTag(correctedTaus),
    shift = cms.vdouble(0.03, 0.03),
    numOfSigmas = cms.double(1.0),
    #verbose = cms.bool(True)
)

process.tauPtEtaIDIsoTauUp = process.tauPtEtaIDIso.clone(
    src = cms.InputTag("rescaledTaus", "U")
)
process.tauPtEtaIDIsoTauUpFilter = process.tauPtEtaIDIsoFilter.clone(
    src = cms.InputTag("tauPtEtaIDIsoTauUp")
)

process.tauPtEtaIDIsoTauDown = process.tauPtEtaIDIso.clone(
    src = cms.InputTag("rescaledTaus", "D")
)
process.tauPtEtaIDIsoTauDownFilter = process.tauPtEtaIDIsoFilter.clone(
    src = cms.InputTag("tauPtEtaIDIsoTauDown")
)

#######################################################################
from LLRAnalysis.TauTauStudies.runMETByLeptonPairs_cfi import *
getDiTauMassByLeptonPair(process, "", "", "tauPtEtaIDIso", runOnMC, useMarkov, useRecoil, doSVFitReco)

process.selectedDiTau = cms.EDFilter("PATDiTauPairSelector",
    src = cms.InputTag("MergedDiTaus:diTaus"),
    cut = cms.string("dR12 > 0.5")
)
process.selectedDiTauFilter = cms.EDFilter("CandViewCountFilter",
    src = cms.InputTag("selectedDiTau"),
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
)

#######################################################################
getDiTauMassByLeptonPair(process, "", "", "tauPtEtaIDIsoTauUp", runOnMC, useMarkov, useRecoil, doSVFitReco, "TauUp")

process.selectedDiTauTauUp = process.selectedDiTau.clone(src = cms.InputTag("MergedDiTausTauUp:diTaus") )
process.selectedDiTauTauUpFilter = process.selectedDiTauFilter.clone(src =  cms.InputTag("selectedDiTauTauUp"))

#######################################################################
getDiTauMassByLeptonPair(process, "", "", "tauPtEtaIDIsoTauDown", runOnMC, useMarkov, useRecoil, doSVFitReco, "TauDown")

process.selectedDiTauTauDown = process.selectedDiTau.clone(src = cms.InputTag("MergedDiTausTauDown:diTaus") )
process.selectedDiTauTauDownFilter = process.selectedDiTauFilter.clone(src =  cms.InputTag("selectedDiTauTauDown"))

########################################################################

##Update 3rdLepVeto cuts##
##Loose MVA ID used for e-mu and 3d lepton veto selection
MVALoose = "((pt <= 20 & abs(superCluster.eta) >= 0.0   & abs(superCluster.eta) < 0.8   & userFloat('mvaPOGNonTrig') > 0.925) |" + \
           " (pt <= 20 & abs(superCluster.eta) >= 0.8   & abs(superCluster.eta) < 1.479 & userFloat('mvaPOGNonTrig') > 0.915) |" + \
           " (pt <= 20 & abs(superCluster.eta) >= 1.479 & abs(superCluster.eta) < 2.5   & userFloat('mvaPOGNonTrig') > 0.965) |" + \
           " (pt >  20 & abs(superCluster.eta) >= 0.0   & abs(superCluster.eta) < 0.8   & userFloat('mvaPOGNonTrig') > 0.905) |" + \
           " (pt >  20 & abs(superCluster.eta) >= 0.8   & abs(superCluster.eta) < 1.479 & userFloat('mvaPOGNonTrig') > 0.955) |" + \
           " (pt >  20 & abs(superCluster.eta) >= 1.479 & abs(superCluster.eta) < 2.5   & userFloat('mvaPOGNonTrig') > 0.975))"
##Loose MVA ID: new e-Id, same FR as old one
MVALooseNew = "((pt <= 20 & abs(superCluster.eta) >= 0.0   & abs(superCluster.eta) < 0.8   & userFloat('mvaPOGTrigNoIP') > -0.5375) |" + \
              " (pt <= 20 & abs(superCluster.eta) >= 0.8   & abs(superCluster.eta) < 1.479 & userFloat('mvaPOGTrigNoIP') > -0.375)  |" + \
              " (pt <= 20 & abs(superCluster.eta) >= 1.479 & abs(superCluster.eta) < 2.5   & userFloat('mvaPOGTrigNoIP') > -0.025)  |" + \
              " (pt >  20 & abs(superCluster.eta) >= 0.0   & abs(superCluster.eta) < 0.8   & userFloat('mvaPOGTrigNoIP') >  0.325)  |" + \
              " (pt >  20 & abs(superCluster.eta) >= 0.8   & abs(superCluster.eta) < 1.479 & userFloat('mvaPOGTrigNoIP') >  0.775)  |" + \
              " (pt >  20 & abs(superCluster.eta) >= 1.479 & abs(superCluster.eta) < 2.5   & userFloat('mvaPOGTrigNoIP') >  0.775))"

process.electronsForVeto = cms.EDFilter("PATElectronSelector",
    src = cms.InputTag("electronsForVeto"),
    cut = cms.string(
        "userFloat('nHits') = 0 & userInt('antiConv') > 0.5"
        ##+" && "+MVALoose),#use MVAloose for back compability, can be changed in future
    ),
    filter = cms.bool(False)
)
#########
process.filterSequence = cms.Sequence(
    process.uncorrectedTausAddedWPs * process.applyTauES *
    process.tauPtEtaIDIso * process.tauPtEtaIDIsoFilter * 
    process.rescaledTaus * process.tauPtEtaIDIsoTauUp * process.tauPtEtaIDIsoTauDown
)

#######################################################################

process.vertexMultiplicityReweightSequence = cms.Sequence()
if runOnMC:
    from LLRAnalysis.HadTauStudies.vertexMultiplicityReweight_cfi import vertexMultiplicityReweight
    process.vertexMultiplicityReweight3d2012RunABCDruns190456to208686 = vertexMultiplicityReweight.clone(
        ##inputFileName = cms.FileInPath("LLRAnalysis/HadTauStudies/data/expPUpoissonMean_runs190456to208686_Mu17_Mu8.root"),
        ##inputFileName = cms.FileInPath("LLRAnalysis/HadTauStudies/data/Data_Pileup_2012_ReRecoPixel-600bins.root"),
        inputFileName = cms.FileInPath("LLRAnalysis/HadTauStudies/data/expPUpoissonMean_runs190456to208686_DoubleMediumIsoPFTau35_TrkSTAR_eta2p1-600bins.root"),
        type = cms.string("gen3d"),
        mcPeriod = cms.string("Summer12_S10")
    )
    process.vertexMultiplicityReweightSequence += process.vertexMultiplicityReweight3d2012RunABCDruns190456to208686
    process.vertexMultiplicityReweight1d2012RunABCDruns190456to208686 = vertexMultiplicityReweight.clone(
        ##inputFileName = cms.FileInPath("LLRAnalysis/HadTauStudies/data/expPUpoissonDist_runs190456to208686_Mu17_Mu8.root"),
        ##inputFileName = cms.FileInPath("LLRAnalysis/HadTauStudies/data/Data_Pileup_2012_ReRecoPixel-60bins.root"),
        inputFileName = cms.FileInPath("LLRAnalysis/HadTauStudies/data/expPUpoissonMean_runs190456to208686_DoubleMediumIsoPFTau35_TrkSTAR_eta2p1-60bins.root"),
        type = cms.string("gen"),
        mcPeriod = cms.string("Summer12_S10")
    )
    process.vertexMultiplicityReweightSequence += process.vertexMultiplicityReweight1d2012RunABCDruns190456to208686
    if runOnEmbed:
        process.vertexMultiplicityReweight3d2012RunABCDruns190456to208686.src = cms.InputTag('addPileupInfo::HLT')
        process.vertexMultiplicityReweight1d2012RunABCDruns190456to208686.src = cms.InputTag('addPileupInfo::HLT')
        
#######################################################################

def drange(start, stop, step):
    r = float(start)
    while r < stop:
    	yield r
     	r += step

mssmHiggsPtReweights = []

mssm_models = [ "mhmax" , "mhmod+", "mhmod-", "low-mH", 'tau-phobic', 'light_stop', 'light_stau' ]
if applyHiggsMassCut:
    for mssm_model in mssm_models:
        inputFileName = None
        mA_values = None
        mu_values = None
        moduleName = None
        instanceLabel = None
        if mssm_model in [ "mhmax" , "mhmod+", "mhmod-" ]:
            if mssm_model == "mhmax":
                inputFileName = "mssmHiggsPtReweightGluGlu_mhmax.root"
                moduleName = "mhmaxHiggsPtReweightGluGlu"
            elif mssm_model == "mhmod+":
                inputFileName = "mssmHiggsPtReweightGluGlu_mhmod+.root"
                moduleName = "mhmodPlusHiggsPtReweightGluGlu"                
            elif mssm_model == "mhmod-":
                inputFileName = "mssmHiggsPtReweightGluGlu_mhmod-.root"
                moduleName = "mhmodMinusHiggsPtReweightGluGlu"
            mA_values = [ 80, 90, 100, 110, 120, 130, 140, 160, 180, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000 ]
            mu_values = [ 200, ]
            instanceLabel = "$higgsTypemA$mA$central_or_shift"            
        elif mssm_model in [ "low-mH" ]:
            inputFileName = "mssmHiggsPtReweightGluGlu_low-mH.root"
            moduleName = "lowmHHiggsPtReweightGluGlu"
            mA_values = [ 110, ]
            mu_values = [ mu_value for mu_value in drange(300, 3501, 200) ]
            instanceLabel = "$higgsTypemu$mu$central_or_shift"
        elif mssm_model in [ "tau-phobic" ]:
            inputFileName = "mssmHiggsPtReweightGluGlu_tau-phobic.root"
            moduleName = "tauphobicHiggsPtReweightGluGlu"
            mA_values = [ 80, 90, 100, 110, 120, 130, 140, 160, 180, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000 ]
            mu_values = [ 2000, ]
            instanceLabel = "$higgsTypemA$mA$central_or_shift"
        elif mssm_model in [ "light_stop"]:
            inputFileName = "mssmHiggsPtReweightGluGlu_light_stop.root"
            moduleName = "lightstopHiggsPtReweightGluGlu"
            mA_values = [ 80, 90, 100, 110, 120, 130, 140, 160, 180, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000 ]
            mu_values = [ 350, ]
            instanceLabel = "$higgsTypemA$mA$central_or_shift"
        elif mssm_model in [ "light_stau" ]:
            inputFileName = "mssmHiggsPtReweightGluGlu_light_stau.root"
            moduleName = "lightstauHiggsPtReweightGluGlu"
            mA_values = [ 80, 90, 100, 110, 120, 130, 140, 160, 180, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000 ]
            mu_values = [ 500, ]
            instanceLabel = "$higgsTypemA$mA$central_or_shift"    
        if not (inputFileName and mA_values and mu_values and moduleName and instanceLabel):
            raise ValueError("Invalid MSSM model = %s !!" % mssm_model)        
        mssmHiggsPtReweightProducerGluGlu = cms.EDProducer("MSSMHiggsPtReweightProducerGluGlu",
            srcGenParticles = cms.InputTag('genParticles'),
            inputFileName = cms.FileInPath("LLRAnalysis/HadTauStudies/data/%s" % inputFileName),
            lutName = cms.string("$higgsType_mA$mA_mu$mu/mssmHiggsPtReweight_$higgsType_mA$mA_mu$mu_$central_or_shift"),                                               
            mA_values = cms.vdouble(mA_values),
            mu_values = cms.vdouble(mu_values),
            higgsTypes = cms.vstring("A", "H", "h"),
            shifts = cms.vstring("HqTscaleUp", "HqTscaleDown", "HIGLUscaleUp", "HIGLUscaleDown", "tanBetaLow", "tanBetaHigh"),
            instanceLabel = cms.string(instanceLabel)
        )
        setattr(process, moduleName, mssmHiggsPtReweightProducerGluGlu)
        process.vertexMultiplicityReweightSequence += mssmHiggsPtReweightProducerGluGlu
        for mA_value in mA_values:
            for mu_value in mu_values:
                for central_or_shift in [ "central", "HqTscaleUp", "HqTscaleDown", "HIGLUscaleUp", "HIGLUscaleDown", "tanBetaLow", "tanBetaHigh" ]:
                    if (mA_value > (0.95*nomHiggsMass) and mA_value < (1.05*nomHiggsMass)) or mssm_model == "low-mH":
                        branchName = "mssmHiggsPtReweightGluGlu_%s_%s_mA%1.0f_mu%1.0f_%s" % (mssm_model, "A", mA_value, mu_value, central_or_shift)
                        instanceLabel_expanded = instanceLabel
                        instanceLabel_expanded = instanceLabel_expanded.replace("$higgsType", "A")
                        instanceLabel_expanded = instanceLabel_expanded.replace("$mA", "%1.0f" % mA_value)
                        instanceLabel_expanded = instanceLabel_expanded.replace("$mu", "%1.0f" % mu_value)
                        instanceLabel_expanded = instanceLabel_expanded.replace("$central_or_shift", "%s" % central_or_shift)
                        mssmHiggsPtReweights.append([ branchName, "%s:%s" % (moduleName, instanceLabel_expanded) ])
                        
#######################################################################
process.tauTauNtupleProducer = cms.EDAnalyzer("TauTauNtupleProducer",
    srcDiTau = cms.InputTag('selectedDiTau'),
    bestDiTauPreselection = cms.vstring("decayModeFindingOldDMs"),
    bestDiTauRanking = cms.vstring("byCombinedIsolationDeltaBetaCorrRaw3Hits"), # CV: agreed with Cecile that we will keep using 'byCombinedIsolationDeltaBetaCorrRaw3Hits' (2014/11/28)
    invertRanking = cms.bool(True), # CV: set to False (True) if ranking taus by MVA output (isolation Pt-sum)                                          
    ##bestDiTauRanking = cms.vstring("byIsolationMVA3oldDMwLTraw"),                                      
    ##invertRanking = cms.bool(False), # CV: set to False (True) if ranking taus by MVA output (isolation Pt-sum)
    tauIdDiscriminators = cms.PSet(
        decayModeFinding = cms.string("decayModeFindingOldDMs"),                                        
        LooseDB3HIso = cms.string("byLooseCombinedIsolationDeltaBetaCorr3Hits"),
        MediumDB3HIso = cms.string("byMediumCombinedIsolationDeltaBetaCorr3Hits"),
        TightDB3HIso = cms.string("byTightCombinedIsolationDeltaBetaCorr3Hits"),
        RawDB3HIso = cms.string("byCombinedIsolationDeltaBetaCorrRaw3Hits"),
        VLooseMVAwoLT = cms.string("byVLooseIsolationMVA3oldDMwoLT"),                                          
        LooseMVAwoLT = cms.string("byLooseIsolationMVA3oldDMwoLT"),
        MediumMVAwoLT = cms.string("byMediumIsolationMVA3oldDMwoLT"),
        TightMVAwoLT = cms.string("byTightIsolationMVA3oldDMwoLT"),
        VTightMVAwoLT = cms.string("byVTightIsolationMVA3oldDMwoLT"),
        VVTightMVAwoLT = cms.string("byVVTightIsolationMVA3oldDMwoLT"),
        RawMVAwoLT = cms.string("byIsolationMVA3oldDMwoLTraw"),
        VLooseMVAwLT = cms.string("byVLooseIsolationMVA3oldDMwLT"),                                          
        LooseMVAwLT = cms.string("byLooseIsolationMVA3oldDMwLT"),
        MediumMVAwLT = cms.string("byMediumIsolationMVA3oldDMwLT"),
        TightMVAwLT = cms.string("byTightIsolationMVA3oldDMwLT"),
        VTightMVAwLT = cms.string("byVTightIsolationMVA3oldDMwLT"),
        VVTightMVAwLT = cms.string("byVVTightIsolationMVA3oldDMwLT"),
        RawMVAwLT = cms.string("byIsolationMVA3oldDMwLTraw"),
        #--------------------------------------------------------------------------------
        # CV: add extra working-points to pat::Tau collection
        VLooseMVAwLTconstEff = cms.string("byVLooseIsolationMVA3oldDMwLTconstEff"),
        LooseMVAwLTconstEff = cms.string("byLooseIsolationMVA3oldDMwLTconstEff"),
        MediumMVAwLTconstEff = cms.string("byMediumIsolationMVA3oldDMwLTconstEff"),
        TightMVAwLTconstEff = cms.string("byTightIsolationMVA3oldDMwLTconstEff"),
        VTightMVAwLTconstEff = cms.string("byVTightIsolationMVA3oldDMwLTconstEff"),
        VVTightMVAwLTconstEff = cms.string("byVVTightIsolationMVA3oldDMwLTconstEff"),
        VLooseMVAwLTconstFR = cms.string("byVLooseIsolationMVA3oldDMwLTconstFR"),
        LooseMVAwLTconstFR = cms.string("byLooseIsolationMVA3oldDMwLTconstFR"),
        MediumMVAwLTconstFR = cms.string("byMediumIsolationMVA3oldDMwLTconstFR"),
        TightMVAwLTconstFR = cms.string("byTightIsolationMVA3oldDMwLTconstFR"),
        VTightMVAwLTconstFR = cms.string("byVTightIsolationMVA3oldDMwLTconstFR"),
        VVTightMVAwLTconstFR = cms.string("byVVTightIsolationMVA3oldDMwLTconstFR"),
        VLooseMVAwLTplatFR = cms.string("byVLooseIsolationMVA3oldDMwLTplatFR"),
        LooseMVAwLTplatFR = cms.string("byLooseIsolationMVA3oldDMwLTplatFR"),
        MediumMVAwLTplatFR = cms.string("byMediumIsolationMVA3oldDMwLTplatFR"),
        TightMVAwLTplatFR = cms.string("byTightIsolationMVA3oldDMwLTplatFR"),
        VTightMVAwLTplatFR = cms.string("byVTightIsolationMVA3oldDMwLTplatFR"),
        VVTightMVAwLTplatFR = cms.string("byVVTightIsolationMVA3oldDMwLTplatFR"),
        #--------------------------------------------------------------------------------
        againstElectronLoose = cms.string("againstElectronLoose"),                                                  
        againstElectronVLooseMVA3 = cms.string("againstElectronVLooseMVA5"), # CV: keep branchname used for old discriminator 
        againstElectronLooseMVA3 = cms.string("againstElectronLooseMVA5"),
        againstElectronMediumMVA3 = cms.string("againstElectronMediumMVA5"),
        againstElectronTightMVA3 = cms.string("againstElectronTightMVA5"),
        againstElectronVTightMVA3 = cms.string("againstElectronVTightMVA5"),                                                  
        againstElectronMVA3raw = cms.string("againstElectronMVA5raw"),
        againstElectronMVA3category = cms.string("againstElectronMVA5category"),                                                 
        againstMuonLoose2 = cms.string("againstMuonLoose3"), # CV: keep branchname used for old discriminator 
        againstMuonTight2 = cms.string("againstMuonTight3"),
        againstMuonLooseMVA = cms.string("againstMuonLooseMVA"),                                          
        againstMuonMediumMVA = cms.string("againstMuonMediumMVA"),                                          
        againstMuonTightMVA = cms.string("againstMuonTightMVA"),                                          
        againstMuonMVAraw = cms.string("againstMuonMVAraw"),                                          
    ),
    srcTriggerResults = cms.InputTag('patTriggerEvent'),                                              
    hltPaths_diTau = cms.vstring(),
    hltPaths_diTauJet = cms.vstring(),
    hltPaths_singleJet = cms.vstring(),
    srcTriggerObjects = cms.InputTag('patTrigger'),                                          
    hltTauFilters_diTau = cms.vstring(),
    hltTauFilters_diTauJet = cms.vstring(),
    hltJetFilters_diTauJet = cms.vstring(),
    hltJetFilters_singleJet = cms.vstring(),
    srcL1Taus = cms.InputTag('l1extraParticles', 'Tau'),
    srcL1Jets = cms.InputTag('l1extraParticles', 'Central'),
    srcElectrons = cms.InputTag('electronsForVeto'),
    srcMuons = cms.InputTag('muonsForVeto'),
    srcJets = cms.InputTag('selectedPatJetsBJetEnReg'),
    srcPileupJetId = cms.InputTag('puJetMva', 'fullId'),
    wpPileupJetId = cms.string("Loose"),
    srcPileupJetIdMVA = cms.InputTag('puJetMva', 'fullDiscriminant'),
    bJetDiscriminator = cms.string("combinedSecondaryVertexBJetTags"),
    jetCorrLabel = cms.string(""),   # CV: for jet energy corrections                      
    jetCorrPayload = cms.string(""), # CV: for jet energy uncertainties
    jecUncertaintyFile = cms.FileInPath("LLRAnalysis/HadTauStudies/data/Summer13_V1_DATA_UncertaintySources_AK5PF.txt"),                                          
    jecUncertaintyTag = cms.string("SubTotalMC"),
    minJetPt = cms.double(30.),
    maxJetAbsEta = cms.double(4.7),
    minBJetPt = cms.double(20.),
    maxBJetAbsEta = cms.double(2.4),
    ##wpBJetDiscriminator = cms.double(0.679), # CSV tagger Medium working-point (cf. https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagPerformanceOP)
    wpBJetDiscriminator = cms.double(0.244), # CSV tagger Loose working-point (cf. https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagPerformanceOP)
    srcRawPFMEt = cms.InputTag('patMETsPFlow'),
    srcGenMEt = cms.InputTag(''),                                           
    srcVertices = cms.InputTag('selectedPrimaryVertices'),
    srcRho = cms.InputTag('kt6PFJets', 'rho'),                       
    evtWeights =  cms.PSet(),
    isMC = cms.bool(runOnMC),
    srcGenPileUpSummary = cms.InputTag('addPileupInfo'),
    srcLHE = cms.InputTag('source'),
    srcGenParticles = cms.InputTag('genParticles'),
    srcGenParticlesForTopPtReweighting = cms.InputTag('genParticles'),                                
    isEmbedded = cms.bool(runOnEmbed),                                          
    srcEmbeddingWeight = cms.InputTag(''),
    verbosity = cms.int32(0)                                              
)

if runOnMC:
    process.tauTauNtupleProducer.hltPaths_diTau = cms.vstring(
        "HLT_DoubleMediumIsoPFTau30_Trk5_eta2p1_v2",
        "HLT_DoubleMediumIsoPFTau35_Trk5_eta2p1_v6"
    )
    process.tauTauNtupleProducer.hltPaths_diTauJet = cms.vstring(
        "HLT_DoubleMediumIsoPFTau30_Trk5_eta2p1_Jet30_v2"
    )
    process.tauTauNtupleProducer.hltPaths_singleJet = cms.vstring(
         "HLT_PFJet320_v5"
    )
    process.tauTauNtupleProducer.hltTauFilters_diTau = cms.vstring(
        "hltDoublePFTau35TrackPt5MediumIsolationProng4Dz02",
        "hltDoublePFTau35TrackPt1MediumIsolationProng4Dz02"
    )
    process.tauTauNtupleProducer.hltTauFilters_diTauJet = cms.vstring(
        "hltDoublePFTau30TrackPt5MediumIsolationProng4Dz02",
        "hltDoublePFTau30TrackPt1MediumIsolationProng4Dz02"
    )
    process.tauTauNtupleProducer.hltJetFilters_diTauJet = cms.vstring(
        "hltTripleL2Jets30eta3"
    )
    process.tauTauNtupleProducer.hltJetFilters_singleJet = cms.vstring(
        "hlt1PFJet320"
    )
    process.tauTauNtupleProducer.jetCorrLabel = cms.string("ak5PFL1FastL2L3")
    process.tauTauNtupleProducer.srcGenMEt = cms.InputTag('genMetFromGenParticles')
    process.tauTauNtupleProducer.evtWeights.vertexWeight = cms.InputTag('vertexMultiplicityReweight1d2012RunABCDruns190456to208686')
    process.tauTauNtupleProducer.evtWeights.vertexWeight3d = cms.InputTag('vertexMultiplicityReweight3d2012RunABCDruns190456to208686')
else:    
    process.tauTauNtupleProducer.hltPaths_diTau = cms.vstring(
        "HLT_DoubleMediumIsoPFTau35_Trk5_eta2p1_v2",
        "HLT_DoubleMediumIsoPFTau35_Trk5_eta2p1_v3",
        "HLT_DoubleMediumIsoPFTau35_Trk5_eta2p1_v4",
        "HLT_DoubleMediumIsoPFTau35_Trk5_eta2p1_v6",
        "HLT_DoubleMediumIsoPFTau35_Trk1_eta2p1_v1",
        "HLT_DoubleMediumIsoPFTau35_Trk1_eta2p1_v3",
        "HLT_DoubleMediumIsoPFTau35_Trk1_eta2p1_v4"
    )
    process.tauTauNtupleProducer.hltPaths_diTauJet = cms.vstring(
        "HLT_DoubleMediumIsoPFTau30_Trk5_eta2p1_Jet30_v2",
        "HLT_DoubleMediumIsoPFTau30_Trk1_eta2p1_Jet30_v1",
        "HLT_DoubleMediumIsoPFTau30_Trk1_eta2p1_Jet30_v4",
        "HLT_DoubleMediumIsoPFTau30_Trk1_eta2p1_Jet30_v5"
    )
    process.tauTauNtupleProducer.hltPaths_singleJet = cms.vstring(
        "HLT_PFJet320_v3",
        "HLT_PFJet320_v4",
        "HLT_PFJet320_v5",
        "HLT_PFJet320_v6",
        "HLT_PFJet320_v8",
        "HLT_PFJet320_v9"
    )
    process.tauTauNtupleProducer.hltTauFilters_diTau = cms.vstring(
        "hltDoublePFTau35TrackPt5MediumIsolationProng4Dz02",
        "hltDoublePFTau35TrackPt1MediumIsolationProng4Dz02"
    )
    process.tauTauNtupleProducer.hltTauFilters_diTauJet = cms.vstring(
        "hltDoublePFTau30TrackPt5MediumIsolationProng4Dz02",
        "hltDoublePFTau30TrackPt1MediumIsolationProng4Dz02"
    )
    process.tauTauNtupleProducer.hltJetFilters_diTauJet = cms.vstring(
        "hltTripleL2Jets30eta3"
    )
    process.tauTauNtupleProducer.hltJetFilters_singleJet = cms.vstring(
        "hlt1PFJet320"
    )
    process.tauTauNtupleProducer.jetCorrLabel = cms.string("ak5PFL1FastL2L3Residual")
    
if runOnEmbed:
    process.tauTauNtupleProducer.srcGenPileUpSummary = cms.InputTag('addPileupInfo::HLT')
    if embedType == "PfEmbed":
        process.tauTauNtupleProducer.srcEmbeddingWeight = cms.InputTag('generator', 'minVisPtFilter', 'EmbeddedRECO')
    elif embedType == "RhEmbed":
        process.tauTauNtupleProducer.srcEmbeddingWeight = cms.InputTag('generator', 'minVisPtFilter', 'EmbeddedRECO')
        process.tauTauNtupleProducer.evtWeights.tauSpin = cms.InputTag('TauSpinnerReco', 'TauSpinnerWT', 'EmbeddedSPIN')
        process.tauTauNtupleProducer.evtWeights.zmumusel = cms.InputTag('ZmumuEvtSelEffCorrWeightProducer', 'weight', 'EmbeddedRECO')
        process.tauTauNtupleProducer.evtWeights.muradcorr = cms.InputTag('muonRadiationCorrWeightProducer', 'weight', 'EmbeddedRECO')
        if runOnMC:
            process.tauTauNtupleProducer.evtWeights.genDiTauMassVsGenDiTauPt = cms.InputTag('embeddingKineReweightGENembedding', 'genDiTauMassVsGenDiTauPt', 'EmbeddedRECO')
            process.tauTauNtupleProducer.evtWeights.genTau2EtaVsGenTau1Eta = cms.InputTag('embeddingKineReweightGENembedding', 'genTau2EtaVsGenTau1Eta', 'EmbeddedRECO')
            process.tauTauNtupleProducer.evtWeights.genTau2PtVsGenTau1Pt = cms.InputTag('embeddingKineReweightGENembedding', 'genTau2PtVsGenTau1Pt', 'EmbeddedRECO')
        else:
            process.tauTauNtupleProducer.evtWeights.genDiTauMassVsGenDiTauPt = cms.InputTag('embeddingKineReweightRECembedding', 'genDiTauMassVsGenDiTauPt', 'EmbeddedRECO')
            process.tauTauNtupleProducer.evtWeights.genTau2EtaVsGenTau1Eta = cms.InputTag('embeddingKineReweightRECembedding', 'genTau2EtaVsGenTau1Eta', 'EmbeddedRECO')
            process.tauTauNtupleProducer.evtWeights.genTau2PtVsGenTau1Pt = cms.InputTag('embeddingKineReweightRECembedding', 'genTau2PtVsGenTau1Pt', 'EmbeddedRECO')
    if runOnMC:
        process.tauTauNtupleProducer.srcGenParticlesForTopPtReweighting = cms.InputTag('genParticles::SIM')  
    
if usePFMEtMVA:
    if useRecoil:
        process.tauTauNtupleProducer.met = cms.InputTag("metRecoilCorrector00", "N")
    else:
        process.tauTauNtupleProducer.met = cms.InputTag("patPFMetByMVA00")

for mssmHiggsPtReweight in mssmHiggsPtReweights:
    setattr(process.tauTauNtupleProducer.evtWeights, mssmHiggsPtReweight[0], cms.InputTag(mssmHiggsPtReweight[1]))

process.tauTauNtupleProducerTauUp = process.tauTauNtupleProducer.clone(
    srcDiTau = cms.InputTag("selectedDiTauTauUp"),
    #met = cms.InputTag("rescaledMETtau","NNNUN"),
)
process.tauTauNtupleProducerTauDown = process.tauTauNtupleProducer.clone(
    srcDiTau = cms.InputTag("selectedDiTauTauDown"),
    #met = cms.InputTag("rescaledMETtau","NNNDN")
)
#######################################################################
# weights for embedding
process.kineWeightsForEmbed = cms.Sequence()
if runOnEmbed:
    if "RhEmbed" in embedType:
        process.load("TauAnalysis.MCEmbeddingTools.embeddingKineReweight_cff")
        if not runOnMC:
            process.kineWeightsForEmbed += process.embeddingKineReweightSequenceRECembedding
        else:
            process.kineWeightsForEmbed += process.embeddingKineReweightSequenceGENembedding
        process.embeddingKineReweightRECembedding.inputFileName = cms.FileInPath('TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_tautau_recEmbedded.root')
        process.embeddingKineReweightGENembedding.inputFileName = cms.FileInPath("TauAnalysis/MCEmbeddingTools/data/embeddingKineReweight_tautau_genEmbedded.root")
                
#######################################################################

process.seqNominal = cms.Sequence(
    process.allEventsFilter *
    process.genFilterSequence *
    process.runPatJets *
    process.uncorrectedTausAddedWPs * process.applyTauES *
    process.tauPtEtaIDIso * process.tauPtEtaIDIsoFilter * 
    process.electronsForVeto *
    #(process.pfMEtMVAsequence*process.patPFMetByMVA)*    
    #(process.LeptonsForMVAMEt*process.puJetIdAndMvaMet)*
    process.produceGenMEt * 
    process.puJetIdSequence *
    #process.produceType1corrPFMEt*
    #process.producePFMEtNoPileUp*
    #process.metRecoilCorrector*
    #process.pfMEtMVACov*
    #process.diTau*process.selectedDiTau*process.selectedDiTauCounter*
    process.calibratedAK5PFJetsForPFMEtMVA *
    process.runMETByPairsSequence *
    process.selectedDiTau * process.selectedDiTauFilter *
    process.kineWeightsForEmbed * #IN
    process.vertexMultiplicityReweightSequence * 
    process.tauTauNtupleProducer
)

process.seqTauUp = cms.Sequence(
    process.allEventsFilter *
    process.genFilterSequence *
    process.runPatJets *
    process.uncorrectedTausAddedWPs * process.applyTauES *
    process.tauPtEtaIDIso * process.tauPtEtaIDIsoFilter * 
    process.rescaledTaus * process.tauPtEtaIDIsoTauUp * process.tauPtEtaIDIsoTauUpFilter * 
    process.electronsForVeto *
    process.produceGenMEt * 
    process.puJetIdSequence *
    process.calibratedAK5PFJetsForPFMEtMVA *
    process.runMETByPairsSequenceTauUp *
    process.selectedDiTauTauUp * process.selectedDiTauTauUpFilter *
    process.kineWeightsForEmbed * #IN
    process.vertexMultiplicityReweightSequence * 
    process.tauTauNtupleProducerTauUp
)

process.seqTauDown = cms.Sequence(
    process.allEventsFilter *
    process.genFilterSequence *
    process.runPatJets *
    process.uncorrectedTausAddedWPs * process.applyTauES *
    process.tauPtEtaIDIso * process.tauPtEtaIDIsoFilter * 
    process.rescaledTaus * process.tauPtEtaIDIsoTauDown * process.tauPtEtaIDIsoTauDownFilter * 
    process.electronsForVeto *
    process.produceGenMEt * 
    process.puJetIdSequence *
    process.calibratedAK5PFJetsForPFMEtMVA *
    process.runMETByPairsSequenceTauDown *
    process.selectedDiTauTauDown * process.selectedDiTauTauDownFilter *
    process.kineWeightsForEmbed * #IN
    process.vertexMultiplicityReweightSequence *
    process.tauTauNtupleProducerTauDown
)

# run TauSpinner to simulate tau polarization effects
# for special DYJets sample that was produced with tau polarization disabled
if sampleName and sampleName == "DYJets_noTauPolarization":
    process.load("TauSpinnerInterface.TauSpinnerInterface.TauSpinner_cfi")    
    process.seqNominal.replace(process.tauTauNtupleProducer, process.TauSpinnerReco*process.tauTauNtupleProducer)
    process.tauTauNtupleProducer.evtWeights.tauSpin = cms.InputTag('TauSpinnerReco', 'TauSpinnerWT')
    process.seqTauUp.replace(process.tauTauNtupleProducerTauUp, process.TauSpinnerReco*process.tauTauNtupleProducerTauUp)
    process.tauTauNtupleProducerTauUp.evtWeights.tauSpin = cms.InputTag('TauSpinnerReco', 'TauSpinnerWT')
    process.seqTauDown.replace(process.tauTauNtupleProducerTauDown, process.TauSpinnerReco*process.tauTauNtupleProducerTauDown)        
    process.tauTauNtupleProducerTauDown.evtWeights.tauSpin = cms.InputTag('TauSpinnerReco', 'TauSpinnerWT')

#######################################################################

if runNominal:
    process.pNominal = cms.Path( process.seqNominal )
if runTauEnUp and (runOnMC or runOnEmbed):
    process.pTauUp = cms.Path( process.seqTauUp)
if runTauEnDown and (runOnMC or runOnEmbed):
    process.pTauDown = cms.Path( process.seqTauDown )

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

##process.out = cms.OutputModule(
##    "PoolOutputModule",
##    outputCommands = cms.untracked.vstring( 'drop *',
##                                            'keep *_metRecoilCorrector_*_*'),
##    fileName = cms.untracked.string('patTuplesSkimmed_TauTauStream.root'),
##    )

process.TFileService = cms.Service("TFileService",
    fileName = cms.string("ntuple_HadTauStream.root")
)

# before starting to process 1st event, print event content
#process.printEventContent = cms.EDAnalyzer("EventContentAnalyzer")
##process.filterFirstEvent = cms.EDFilter("EventCountFilter",
##    numEvents = cms.int32(1)
##)
##process.printFirstEventContentPath = cms.Path(process.filterFirstEvent + process.printEventContent)

process.outpath = cms.EndPath()

processDumpFile = open('runTauTauNtupleProducer.dump', 'w')
print >> processDumpFile, process.dumpPython()
