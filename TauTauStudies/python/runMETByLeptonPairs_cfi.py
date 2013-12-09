import FWCore.ParameterSet.Config as cms
from PhysicsTools.PatAlgos.tools.helpers import cloneProcessingSnippet

def getDiTauMassByLeptonPair(process, muonColl, electronColl, tauColl, runOnMC=True, useMarkov=True, useRecoil=True, doSVFitReco=True, postfix="") :

    ##print "<getDiTauMassByLeptonPair>:"

    isMuTau   = False
    isElecTau = False
    isTauTau  = False
    if muonColl != "":
        isMuTau = True
        ##print " isMuTau"
    elif electronColl != "":
        isElecTau = True
        ##print " isElecTau"
    else:
        isTauTau = True
        ##print " isTauTau"
    
    process.load("JetMETCorrections.METPUSubtraction.mvaPFMET_cff")
    process.pfMEtMVA.minNumLeptons = cms.int32(2)
    if runOnMC:
        process.calibratedAK5PFJetsForPFMEtMVA.correctors = cms.vstring("ak5PFL1FastL2L3")
    else:
        process.calibratedAK5PFJetsForPFMEtMVA.correctors = cms.vstring("ak5PFL1FastL2L3Residual")
    ##process.pfMEtMVA.verbosity = cms.int32(1)
        
    process.metRecoilCorrector = cms.EDProducer("MEtRecoilCorrectorProducer",
        genParticleTag      = cms.InputTag("genParticles"),
        jetTag              = cms.InputTag("selectedPatJets"),
        metTag              = cms.InputTag("patMETsPFlow"),
        electronTag         = cms.InputTag(electronColl),
        muonTag             = cms.InputTag(muonColl),
        tauTag              = cms.InputTag(tauColl),
        inputFileNamezmm42X = cms.FileInPath("LLRAnalysis/Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_zmm53XRR_2012_njet.root"),
        inputFileNamedatamm = cms.FileInPath("LLRAnalysis/Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_datamm53XRR_2012_njet.root"),
        inputFileNamewjets  = cms.FileInPath("LLRAnalysis/Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_wjets53X_20pv_njet.root"),
        inputFileNamezjets  = cms.FileInPath("LLRAnalysis/Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_zmm53XRR_2012_njet.root"),
        inputFileNamehiggs  = cms.FileInPath("LLRAnalysis/Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_higgs53X_20pv_njet.root"),
        numOfSigmas         = cms.double(1.0),
        minJetPt            = cms.double(30.0),
        verbose             = cms.bool(False),
        isMC                = cms.bool(runOnMC),
        idxTau              = cms.int32(-1),
    )

    process.rescaledMET = cms.EDProducer("MEtRescalerProducer",
        metTag         = cms.InputTag("metRecoilCorrector",  "N"),
        jetTag         = cms.InputTag("selectedPatJets"),
        electronTag    = cms.InputTag(electronColl),
        muonTag        = cms.InputTag(muonColl),
        tauTag         = cms.InputTag(tauColl),
        unClusterShift = cms.double(0.10),
        tauShift       = cms.vdouble(0.03,0.03),
        muonShift      = cms.vdouble(0.01,0.01),
        electronShift  = cms.vdouble(0.01,0.025),
        jetThreshold   = cms.double(10),
        numOfSigmas    = cms.double(1.0),
        verbose = cms.bool(False)
    )

    process.load("PhysicsTools.PatAlgos.producersLayer1.metProducer_cfi")
    process.patPFMetByMVA = process.patMETs.clone(
        metSource = cms.InputTag('pfMEtMVA'),
        addMuonCorrections = cms.bool(False),
        genMETSource = cms.InputTag('genMetTrue'),
        addGenMET = cms.bool(runOnMC)
    )
    
    process.pfMEtMVACov = cms.EDProducer("PFMEtSignCovMatrixUnembedder",
        src = cms.InputTag("patPFMetByMVA")
    )

    if isMuTau:
        process.load("TauAnalysis.CandidateTools.muTauPairProduction_cff")
        process.diTau = process.allMuTauPairs.clone()
        process.diTau.srcLeg1 = cms.InputTag(muonColl)
        process.diTau.srcLeg2 = cms.InputTag(tauColl)
    elif isElecTau:    
        process.load("TauAnalysis.CandidateTools.elecTauPairProduction_cff")
        process.diTau = process.allElecTauPairs.clone()
        process.diTau.srcLeg1 = cms.InputTag(electronColl)
        process.diTau.srcLeg2 = cms.InputTag(tauColl)
    elif isTauTau:
        process.load("TauAnalysis.CandidateTools.diTauPairProduction_cff")
        process.diTau = process.allDiTauPairs.clone()
        process.diTau.srcLeg1 = cms.InputTag(tauColl)
        process.diTau.srcLeg2 = cms.InputTag(tauColl)
   
    process.diTau.srcMET      = cms.InputTag("metRecoilCorrector",  "N")
    process.diTau.doPFMEtSign = cms.bool(False)
    process.diTau.srcPrimaryVertex = cms.InputTag("offlinePrimaryVertices")
    process.diTau.dRmin12     = cms.double(0.5)
    process.diTau.doSVreco    = cms.bool(False)  #False, to run Standalone SVFit
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
    delattr(process.diTau.nSVfit, "psKine_MEt_logM_fit")
    if not runOnMC:
        process.diTau.srcGenParticles = ""

    ###############################################################
    diTauInputList = []
    runMETByPairsSequence = cms.Sequence()
    for idxLeg1 in range(10):
        for idxLeg2 in range(10):

            srcLeg1 = None
            if isMuTau:
                moduleNameLeg1 = "%sLeg1comb%i%i%s" % (muonColl, idxLeg1, idxLeg2, postfix)
                moduleLeg1 = cms.EDProducer("SinglePatMuonPicker",
                    src = cms.InputTag(muonColl),
                    itemNumber = cms.uint32(idxLeg1),
                    verbose = cms.untracked.bool(False)
                )
                setattr(process, moduleNameLeg1, moduleLeg1)
                runMETByPairsSequence += moduleLeg1
                srcLeg1 = moduleNameLeg1
            elif isElecTau:   
                moduleNameLeg1 = "%sLeg1comb%i%i%s" % (electronColl, idxLeg1, idxLeg2, postfix)
                moduleLeg1 = cms.EDProducer("SinglePatElectronPicker",
                    src = cms.InputTag(electronColl),
                    itemNumber = cms.uint32(idxLeg1),
                    verbose = cms.untracked.bool(False)
                )
                setattr(process, moduleNameLeg1, moduleLeg1)
                runMETByPairsSequence += moduleLeg1
                srcLeg1 = moduleNameLeg1
            elif isTauTau:
                if idxLeg2 <= idxLeg1:
                    continue
                moduleNameLeg1 = "%sLeg1comb%i%i%s" % (tauColl, idxLeg1, idxLeg2, postfix)
                moduleLeg1 = cms.EDProducer("SinglePatTauPicker",
                    src = cms.InputTag(tauColl),
                    itemNumber = cms.uint32(idxLeg1),
                    verbose = cms.untracked.bool(False)
                )
                setattr(process, moduleNameLeg1, moduleLeg1)
                runMETByPairsSequence += moduleLeg1
                srcLeg1 = moduleNameLeg1

            moduleNameLeg2 = "%sLeg2comb%i%i%s" % (tauColl, idxLeg1, idxLeg2, postfix)
            moduleLeg2 = cms.EDProducer("SinglePatTauPicker",
                src = cms.InputTag(tauColl),
                itemNumber = cms.uint32(idxLeg2),
                verbose = cms.untracked.bool(False)
            )
            setattr(process, moduleNameLeg2, moduleLeg2)
            runMETByPairsSequence += moduleLeg2
            srcLeg2 = moduleNameLeg2

            moduleNameMVAMET = "pfMEtMVAcomb%i%i%s" % (idxLeg1, idxLeg2 ,postfix)
            moduleMVAMET = process.pfMEtMVA.clone(
                srcLeptons = cms.VInputTag(cms.InputTag(srcLeg1), cms.InputTag(srcLeg2))
            )
            setattr(process, moduleNameMVAMET, moduleMVAMET)
            runMETByPairsSequence += moduleMVAMET
            moduleNamePatMetMVA = "patPFMetByMVAcomb%i%i%s" % (idxLeg1, idxLeg2, postfix)
            modulePatMetMVA = process.patPFMetByMVA.clone(metSource = cms.InputTag(moduleNameMVAMET))
            setattr(process, moduleNamePatMetMVA, modulePatMetMVA)
            runMETByPairsSequence += modulePatMetMVA
            moduleNameMetCov = "pfMEtMVACovComb%i%i%s" % (idxLeg1, idxLeg2, postfix)
            moduleMetCov = process.pfMEtMVACov.clone(src = cms.InputTag(moduleNamePatMetMVA))
            setattr(process, moduleNameMetCov, moduleMetCov)
            runMETByPairsSequence += moduleMetCov
            moduleNameDiTau = "diTauComb%i%i%s" % (idxLeg1, idxLeg2, postfix)
            moduleDiTau = process.diTau.clone(
                srcLeg1 = cms.InputTag(srcLeg1),
                srcLeg2 = cms.InputTag(srcLeg2),
                srcMET = cms.InputTag(moduleNamePatMetMVA)
            )
            metCollection = cms.InputTag(moduleNamePatMetMVA)
            if useRecoil :
                moduleNameRecoilMET = "metRecoilCorrectorComb%i%i%s" % (idxLeg1, idxLeg2, postfix)
                moduleRecoilMET = process.metRecoilCorrector.clone()
                moduleRecoilMET.metTag = cms.InputTag(moduleNamePatMetMVA)
                if isMuTau:
                    moduleRecoilMET.electronTag = cms.InputTag("")
                    moduleRecoilMET.muonTag = cms.InputTag(srcLeg1)
                elif isElecTau:
                    moduleRecoilMET.electronTag = cms.InputTag(srcLeg1)
                    moduleRecoilMET.muonTag = cms.InputTag("")
                else:
                    moduleRecoilMET.electronTag = cms.InputTag("")
                    moduleRecoilMET.muonTag = cms.InputTag("")
                moduleRecoilMET.tauTag = cms.InputTag(srcLeg2)
                moduleRecoilMET.idxLeg2 = cms.int32(-1)
                setattr(process, moduleNameRecoilMET, moduleRecoilMET)
                runMETByPairsSequence += moduleRecoilMET

                moduleDiTau.srcMET = cms.InputTag(moduleNameRecoilMET, "N")
                moduleDiTau.nSVfit.psKine_MEt_int.config.event.srcMEt = cms.InputTag(moduleNameRecoilMET, "N")
                moduleDiTau.nSVfit.psKine_MEt_int.config.event.likelihoodFunctions[0].srcMEtCovMatrix = cms.InputTag(moduleNameMetCov)
                metCollection = cms.InputTag(moduleNameRecoilMET, "N")                                
            else :
                moduleDiTau.nSVfit.psKine_MEt_int.config.event.srcMEt = cms.InputTag(moduleNamePatMetMVA)
                moduleDiTau.nSVfit.psKine_MEt_int.config.event.likelihoodFunctions[0].srcMEtCovMatrix = cms.InputTag(moduleNameMetCov) 
                
            setattr(process, moduleNameDiTau, moduleDiTau)
            runMETByPairsSequence += moduleDiTau
            
            diTauInputList.append(cms.PSet(
                srcDiTau = cms.InputTag(moduleNameDiTau),
                srcMET = metCollection
            ))            
            
    ################################################################3
    #moduleEvtContentName = "printEventContent%s" % postfix
    #moduleEventContent = cms.EDAnalyzer("EventContentAnalyzer")
    #setattr(process, moduleEvtContentName, moduleEventContent)
    #runMETByPairsSequence += moduleEventContent
    MergedDiTausName  = "MergedDiTaus%s" % postfix        
    if isMuTau:
        MergedDiTaus = cms.EDProducer("PATMuTauPairToMEtAssociationProducer",
            inputs = cms.VPSet(diTauInputList)
        )
    elif isElecTau:
        MergedDiTaus = cms.EDProducer("PATElecTauPairToMEtAssociationProducer",
            inputs = cms.VPSet(diTauInputList)
        )
    elif isTauTau:
        MergedDiTaus = cms.EDProducer("PATDiTauPairToMEtAssociationProducer",
            inputs = cms.VPSet(diTauInputList)
        )
    setattr(process, MergedDiTausName, MergedDiTaus) 
    runMETByPairsSequence += MergedDiTaus

    runMETByPairsSequenceName = "runMETByPairsSequence%s" % postfix
    setattr(process, runMETByPairsSequenceName, runMETByPairsSequence)
    #print runMETByPairsSequence
    return runMETByPairsSequence
