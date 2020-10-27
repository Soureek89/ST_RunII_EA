import FWCore.ParameterSet.Config as cms
import copy

leptonssize = cms.untracked.int32(10)
jetssize = cms.untracked.int32(20)

mulabel = cms.string("muons")
elelabel = cms.string("electrons")
jetlabel = cms.string("jetsAK4CHS")
jetak8label = cms.string("jetsAK8CHS")
subjetak8label = cms.string("subjetsAK8CHS")
metlabel = cms.string("metFull")
metnohflabel = cms.string("metnohf")
jetnohflabel = cms.string("jetsAK4NoHF")

#Systematics:
# default, can be overwritten in the *_cfg.py
systsToSave = ["noSyst"]
#systsToSave = ["noSyst","jes__up","jes__down","jer__up","jer__down"]#,"unclusteredMet__up","unclusteredMet__down"]

metFilters = ["Flag_CSCTightHaloFilter","Flag_goodVertices", "Flag_eeBadScFilter"]
#metFilters = ["Flag_CSCTightHaloFilter","Flag_goodVertices"]

#Triggers
leptonTriggers = [
    "HLT_IsoMu22_v",
    "HLT_IsoMu24_v",
    "HLT_IsoTkMu22_v",
    "HLT_IsoTkMu24_v",
    "HLT_Ele32_eta2p1_WPTight_Gsf"
]


hadronTriggers = [] #<--Check those triggers!
#hadronTriggers = ["HLT_PFMET170_NoiseCleaned_v1","HLT_PFMET120_NoiseCleaned_BTagCSV07_v1","HLT_PFHT350_PFMET120_NoiseCleaned_v1","HLT_AK8PFJet360TrimMod_Mass30_v1"]
cutOnTriggers = False

#What to use for jets/other variables
saveBase = cms.untracked.bool(False)
j= "jetsAK4CHS"
jpref= "jetAK4CHS"
#jnohf= "jetsAK4NoHF"
#jprefnohf= "jetAK4NoHF"

sj = "subjetsAK8CHS"
sjpref = "subjetAK8CHS"

#sj = "subjetsCmsTopTag"
#sjpref = "subjetsCmsTopTag"
subjetak8label = cms.string(sj)

#Initializing the analyzer
DMTreesDumper = cms.EDAnalyzer(
    'DMAnalysisTreeMaker',
    lhes = cms.InputTag('source'),
    genprod = cms.InputTag('generator'),
    genParticles= cms.InputTag('filteredPrunedGenParticles'),
    muLabel = mulabel,
    eleLabel = elelabel,
    jetsLabel = jetlabel,
    jetsnohfLabel = jetnohflabel,
    boostedTopsLabel = jetak8label,
    boostedTopsSubjetsLabel = subjetak8label,
    metLabel = metlabel,
    metnohfLabel = metnohflabel,
    physicsObjects = cms.VPSet(
        cms.PSet(
            label = metlabel,
            prefix = cms.string("metFull"),
            maxInstances =  cms.untracked.int32(1),
            saveBaseVariables = cms.untracked.bool(True),
            variablesF = cms.VInputTag(
                cms.InputTag("metFull","metFullPt"),
                cms.InputTag("metFull","metFullPhi"),
                cms.InputTag("metFull","metFullPx"),
                cms.InputTag("metFull","metFullPy"),
                cms.InputTag("metFull","metFulluncorPhi"),
                cms.InputTag("metFull","metFulluncorPt"),
                cms.InputTag("metFull","metFulluncorSumEt"),
            ),
            variablesI = cms.VInputTag(),
            singleI = cms.VInputTag(),
            singleF = cms.VInputTag(),
            toSave = cms.vstring(),
            )
        ),
    
    #trigger part:
    useTriggers = cms.untracked.bool(True),
    cutOnTriggers = cms.untracked.bool(cutOnTriggers),
    triggerBits = cms.InputTag("TriggerUserData","triggerBitTree"),
    triggerNames = cms.InputTag("TriggerUserData","triggerNameTree"),
    triggerPrescales = cms.InputTag("TriggerUserData","triggerPrescaleTree"),
    #met filters
    metBits = cms.InputTag("METUserData","triggerBitTree"),
    metNames = cms.InputTag("METUserData","triggerNameTree"),
    #lumi,run,number
    lumiBlock = cms.InputTag("eventInfo","evtInfoLumiBlock"),
    runNumber = cms.InputTag("eventInfo","evtInfoRunNumber"),
    eventNumber = cms.InputTag("eventInfo","evtInfoEventNumber"),
    #HBHE
#    HBHEFilter = cms.InputTag("HBHENoiseFilterResultProducer","HBHENoiseFilterResult"),
#	HBHEFilter = cms.InputTag("HBHENoiseFilterResultProducer","HBHENoiseFilterResultRun2Loose"),
#	HBHEIsoFilter = cms.InputTag("HBHENoiseFilterResultProducer","HBHEIsoNoiseFilterResult"),
    #vertex
    vertexZ =  cms.InputTag("vertexInfo","z"),
    vertexChi2 =  cms.InputTag("vertexInfo","chi"),
    vertexNdof =  cms.InputTag("vertexInfo","ndof"),
    vertexRho =  cms.InputTag("vertexInfo","rho"),
    #resolved top part:
    #doResolvedTopHad=cms.untracked.bool(True),
    #doResolvedTopSemiLep=cms.untracked.bool(True),
    #cuts for the jet scan
    jetScanCuts=cms.vdouble(30), #Note: the order is important, as the jet collection with the first cut is used for the definition of mt2w.
    
    #Systematics trees to produce. Include:
    #jes__up,jes__down,jer__up,jer__down,unclusteredMet__up,unclusteredMet__down    
    systematics = cms.vstring(systsToSave), #cms.vstring("jes__up","jes__down","jer__up","jer__down","unclusteredMet__up","unclusteredMet__down"),
    channelInfo = cms.PSet(
        channel = cms.string("ttDM"),#Name of the channel, to use in the trees
        crossSection = cms.double(1),#Cross section in pb
        originalEvents = cms.double(1),#Number of events in the MC
        hadronicTriggers = cms.vstring(hadronTriggers),
        leptonicTriggers = cms.vstring(leptonTriggers),
        metFilters = cms.vstring(metFilters),
        #useLHE = cms.untracked.bool(False),#Whether one uses the weights from the LHE in order to get scale uncertainties
        useLHE = cms.untracked.bool(True),
        topPtreweight= cms.untracked.bool(False),
        useLHEWeights = cms.untracked.bool(False),#Whether one uses the weights from the LHE in order to get scale uncertainties
        addLHAPDFWeights = cms.untracked.bool(False), #Whether to add the PDF for uncertainty evaluation (time consuming)
        maxWeights = cms.untracked.int32(9),
        maxPdf = cms.untracked.int32(102),
       )
    )

#Now taking the other input objects:
# DMTreesDumper.physicsObjects.append(  
#     cms.PSet(
#         label = metnohflabel,
#         prefix = cms.string("metNoHF"),
#         maxInstances =  cms.untracked.int32(1),
#         saveBaseVariables = cms.untracked.bool(True),
#         variablesF = cms.VInputTag(
#             cms.InputTag("metNoHF","metNoHFPt"),
#             cms.InputTag("metNoHF","metNoHFPhi"),
#             cms.InputTag("metNoHF","metNoHFPx"),
#             cms.InputTag("metNoHF","metNoHFPy"),
#             cms.InputTag("metNoHF","metNoHFuncorPhi"),
#             cms.InputTag("metNoHF","metNoHFuncorPt"),
#             cms.InputTag("metNoHF","metNoHFuncorSumEt"),
#                 ),
#         variablesI = cms.VInputTag(),
#         singleI = cms.VInputTag(),
#         singleF = cms.VInputTag(),
#         toSave = cms.vstring(),
#         )
#     )


DMTreesDumper.physicsObjects.append(  
    cms.PSet(
        label = mulabel,
        prefix = cms.string("mu"),
        maxInstances = leptonssize,
        saveBaseVariables = saveBase,
        variablesF = cms.VInputTag(
            cms.InputTag("muons","muE"),
            cms.InputTag("muons","muPt"),
            cms.InputTag("muons","muEta"),
            cms.InputTag("muons","muPhi"),
            cms.InputTag("muons","muCharge"),
            cms.InputTag("muons","muIsLooseMuon"),
            cms.InputTag("muons","muIsSoftMuon"),
            cms.InputTag("muons","muIsTightMuon"),
            cms.InputTag("muons","muDxy"),
#           cms.InputTag("muons","muDBerr"),
            cms.InputTag("muons","muDz"),
#           cms.InputTag("muons","muDzerr"),
            cms.InputTag("muons","muGenMuonCharge"),
            cms.InputTag("muons","muGenMuonEta"),
            cms.InputTag("muons","muGenMuonPt"),
            cms.InputTag("muons","muGenMuonE"),
            cms.InputTag("muons","muGenMuonPhi"),
#           cms.InputTag("muons","muGenMuonY"),
            cms.InputTag("muons","muGlbTrkNormChi2"),
            #cms.InputTag("muons","muHLTmuonDeltaR"),
            #cms.InputTag("muons","muHLTmuonE"),
            #cms.InputTag("muons","muHLTmuonEta"),
            #cms.InputTag("muons","muHLTmuonPt"),
            #cms.InputTag("muons","muHLTmuonPhi"),
            cms.InputTag("muons","muInTrkNormChi2"),
            cms.InputTag("muons","muIsGlobalMuon"),
            cms.InputTag("muons","muIsPFMuon"),
            cms.InputTag("muons","muIsTrackerMuon"),
            cms.InputTag("muons","muIso04"),
            cms.InputTag("muons","muNumberMatchedStations"),
            cms.InputTag("muons","muNumberOfPixelLayers"),
            cms.InputTag("muons","muNumberOfValidTrackerHits"),
            cms.InputTag("muons","muNumberTrackerLayers"),
            cms.InputTag("muons","muNumberValidMuonHits"),
            cms.InputTag("muons","muNumberValidPixelHits"),
            cms.InputTag("muons","muSumChargedHadronPt"),
            cms.InputTag("muons","muSumNeutralHadronPt"),
            cms.InputTag("muons","muSumPUPt"),
            cms.InputTag("muons","muSumPhotonPt"),
#           cms.InputTag("muons","muY"),            

            ),
        variablesI = cms.VInputTag(),
        singleI = cms.VInputTag(),
        singleF = cms.VInputTag(),
        toSave = cms.vstring("muE","muPt","muEta","muPhi","muIso04","muCharge","muIsTightMuon","muIsLooseMuon","allExtra"),
        )
    )

DMTreesDumper.physicsObjects.append( 
    cms.PSet(
        label = elelabel,
        prefix = cms.string("el"),
        maxInstances = leptonssize,
        saveBaseVariables = saveBase,
        variablesF = cms.VInputTag(
            cms.InputTag("electrons","elE"),
            cms.InputTag("electrons","elPt"),
#           cms.InputTag("electrons","elMass"),
            cms.InputTag("electrons","elEta"),
            cms.InputTag("electrons","elPhi"),
            cms.InputTag("electrons","elCharge"),
            cms.InputTag("electrons","elSCEta"),
            cms.InputTag("electrons","elHoE"),
            cms.InputTag("electrons","elIso03"),
#           cms.InputTag("electrons","elY"),
            cms.InputTag("electrons","eldEtaIn"),
            cms.InputTag("electrons","eldPhiIn"),
            cms.InputTag("electrons","elmissHits"),
            cms.InputTag("electrons","elfull5x5siee"),
            cms.InputTag("electrons","elooEmooP"),
            cms.InputTag("electrons","elhasMatchedConVeto"),
            cms.InputTag("electrons","elvidVeto"),
            cms.InputTag("electrons","elvidTight"),
            cms.InputTag("electrons","elvidMedium"),
            cms.InputTag("electrons","elDz"),
            cms.InputTag("electrons","elDxy"),
            cms.InputTag("electrons","elvidHEEP"),
        ),
        variablesI = cms.VInputTag( ),
        singleI = cms.VInputTag(),
        singleF = cms.VInputTag(),
		toSave = cms.vstring("elE","elPt","elEta","elPhi","elIso03","elvidTight","elCharge","elvidMedium","elvidLoose","elvidVeto","elDxy","elDz","elvidHEEP","elSCEta","allExtra"),
        )
    )                                     

DMTreesDumper.physicsObjects.append( 
    cms.PSet(
        label = jetlabel,
        prefix = cms.string(jpref),
        maxInstances = jetssize,
        saveBaseVariables = saveBase,
        variablesF = cms.VInputTag(
            cms.InputTag(j,jpref+"E"),
            cms.InputTag(j,jpref+"Pt"),
#           cms.InputTag(j,jpref+"Mass"),
            cms.InputTag(j,jpref+"Eta"),
            cms.InputTag(j,jpref+"Phi"),
#           cms.InputTag(j,jpref+"QGL"),
            cms.InputTag(j,jpref+"PartonFlavour"),
            cms.InputTag(j,jpref+"Phi"),
            cms.InputTag(j,jpref+"CSVv2"),
            cms.InputTag(j,jpref+"CMVAv2"),
#           cms.InputTag(j,jpref+"CSVV1"),
            cms.InputTag(j,jpref+"Charge"),
#           cms.InputTag(j,jpref+"ChargeMuEnergy"),
#           cms.InputTag(j,jpref+"ChargedHadronMultiplicity"),
#           cms.InputTag(j,jpref+"ElectronEnergy"),
            cms.InputTag(j,jpref+"GenJetCharge"),
            cms.InputTag(j,jpref+"GenJetE"),
            cms.InputTag(j,jpref+"GenJetEta"),
            cms.InputTag(j,jpref+"GenJetPhi"),
            cms.InputTag(j,jpref+"GenJetPt"),
#           cms.InputTag(j,jpref+"GenJetY"),
            cms.InputTag(j,jpref+"GenPartonCharge"),
            cms.InputTag(j,jpref+"GenPartonE"),
            cms.InputTag(j,jpref+"GenPartonEta"),
            cms.InputTag(j,jpref+"GenPartonPhi"),
            cms.InputTag(j,jpref+"GenPartonPt"),
#           cms.InputTag(j,jpref+"GenPartonY"),
#           cms.InputTag(j,jpref+"HFEMEnergy"),
#           cms.InputTag(j,jpref+"HFEMMultiplicity"),
#           cms.InputTag(j,jpref+"HFHadronEnergy"),
#           cms.InputTag(j,jpref+"HFHadronMultiplicity"),
            cms.InputTag(j,jpref+"HadronFlavour"),
#           cms.InputTag(j,jpref+"SmearedE"),
#           cms.InputTag(j,jpref+"SmearedPt"),
#           cms.InputTag(j,jpref+"SmearedPEta"),
#           cms.InputTag(j,jpref+"SmearedPhi"),
#           cms.InputTag(j,jpref+"Y"),
            cms.InputTag(j,jpref+"MuonEnergy"),

            cms.InputTag(j,jpref+"neutralHadronEnergyFrac"),
            cms.InputTag(j,jpref+"neutralEmEnergyFrac"),
            cms.InputTag(j,jpref+"chargedEmEnergyFrac"),
            cms.InputTag(j,jpref+"chargedHadronEnergyFrac"),
#           cms.InputTag(j,jpref+"NumConstituents"),
#           cms.InputTag(j,jpref+"electronMultiplicity"),
#           cms.InputTag(j,jpref+"muonMultiplicity"),
#           cms.InputTag(j,jpref+"neutralHadronMultiplicity"),
            cms.InputTag(j,jpref+"neutralMultiplicity"),
#           cms.InputTag(j,jpref+"photonMultiplicity"),
#           cms.InputTag(j,jpref+"numberOfDaughters"),
#           cms.InputTag(j,jpref+"chargedHadronEnergy"),
            cms.InputTag(j,jpref+"chargedMultiplicity"),
#           cms.InputTag(j,jpref+"chargedEmEnergy"),
#           cms.InputTag(j,jpref+"neutralEmEnergy"),
#           cms.InputTag(j,jpref+"neutralHadronEnergy"),
            cms.InputTag(j,jpref+"jecFactor0"),
            cms.InputTag(j,jpref+"jetArea"),
#           cms.InputTag(j,jpref+"pileupJetIdRMS"),#
#           cms.InputTag(j,jpref+"pileupJetIdbeta"),
#           cms.InputTag(j,jpref+"pileupJetIdbetaClassic"),
#           cms.InputTag(j,jpref+"pileupJetIdbetaStar"),
#           cms.InputTag(j,jpref+"pileupJetIdbetaStarClassic"),
#           cms.InputTag(j,jpref+"pileupJetIddR2Mean"),
#           cms.InputTag(j,jpref+"pileupJetIddRMean"),
#           cms.InputTag(j,jpref+"pileupJetIddZ"),
#           cms.InputTag(j,jpref+"pileupJetIdfrac01"),
#           cms.InputTag(j,jpref+"pileupJetIdfrac02"),
#           cms.InputTag(j,jpref+"pileupJetIdfrac03"),
#           cms.InputTag(j,jpref+"pileupJetIdfrac04"),
#           cms.InputTag(j,jpref+"pileupJetIdjetR"),
#           cms.InputTag(j,jpref+"pileupJetIdjetRchg"),
#           cms.InputTag(j,jpref+"pileupJetIdmajW"),
#           cms.InputTag(j,jpref+"pileupJetIdminW"),
#           cms.InputTag(j,jpref+"pileupJetIdnCharged"),
#           cms.InputTag(j,jpref+"pileupJetIdnNeutrals"),
#           cms.InputTag(j,jpref+"pileupJetIdnParticles"),
#           cms.InputTag(j,jpref+"pileupJetIdptD"),
#           cms.InputTag(j,jpref+"pileupJetIdpull")
        ),
        variablesI = cms.VInputTag(),
        singleI = cms.VInputTag(),
        singleF = cms.VInputTag(),
        #toSave = cms.vstring(jpref+"Eta",jpref+"Phi","allExtra"),
        toSave = cms.vstring(jpref+"E",jpref+"Pt",jpref+"Eta",jpref+"Phi",jpref+"GenJetPt",jpref+"GenJetEta",jpref+"CMVAv2",jpref+"CSVv2",jpref+"HadronFlavour",jpref+"PartonFlavour",jpref+"jecFactor0","allExtra"),
        ),
    )

# DMTreesDumper.physicsObjects.append( 
#     cms.PSet(
#         label = jetnohflabel,
#         prefix = cms.string(jprefnohf),
#         maxInstances = jetssize,
#         saveBaseVariables = saveBase,
#         variablesF = cms.VInputTag(
#             cms.InputTag(jnohf,jprefnohf+"E"),
#             cms.InputTag(jnohf,jprefnohf+"Pt"),
#             cms.InputTag(jnohf,jprefnohf+"Eta"),
#             cms.InputTag(jnohf,jprefnohf+"Phi"),
#             cms.InputTag(jnohf,jprefnohf+"jecFactor0"),
#             cms.InputTag(jnohf,jprefnohf+"jetArea")            
#             ),
#         variablesI = cms.VInputTag(),
#         singleI = cms.VInputTag(),
#         singleF = cms.VInputTag(),
#         #toSave = cms.vstring(jpref+"Eta",jpref+"Phi","allExtra"),
#         toSave = cms.vstring(jprefnohf+"E",jprefnohf+"Pt",jprefnohf+"Eta",jprefnohf+"Phi"),
#         ),
#     )

'''
DMTreesDumper.physicsObjects.append( 
    cms.PSet(
        label = jetak8label,
        prefix = cms.string("jetAK8"),
        maxInstances = cms.untracked.int32(4),
        variablesF = cms.VInputTag(
            cms.InputTag("jetsAK8","jetAK8E"),
            cms.InputTag("jetsAK8","jetAK8Pt"),
            cms.InputTag("jetsAK8","jetAK8Mass"),
            cms.InputTag("jetsAK8","jetAK8Eta"),
            cms.InputTag("jetsAK8","jetAK8Phi"),
            cms.InputTag("jetsAK8","jetAK8Mass"),
            cms.InputTag("jetsAK8","jetAK8minmass"),
            cms.InputTag("jetsAK8","jetAK8nSubJets"),
            cms.InputTag("jetsAK8","jetAK8prunedMass"),
            cms.InputTag("jetsAK8","jetAK8tau1"),
            cms.InputTag("jetsAK8","jetAK8tau2"),
            cms.InputTag("jetsAK8","jetAK8tau3"),
            cms.InputTag("jetsAK8","jetAK8topMass"),
            cms.InputTag("jetsAK8","jetAK8trimmedMass"),
            cms.InputTag("jetsAK8","jetAK8wMass"),
            ),
        variablesI = cms.VInputTag(),
        singleI = cms.VInputTag(),
        singleF = cms.VInputTag(),
        toSave = cms.vstring("jetAK8E","jetAK8Pt","jetAK8Eta","jetAK8Phi","jetAK8Mass","jetAK8minmass", "jetAK8nSubJets", "jetAK8prunedMass", "jetAK8tau1", "jetAK8tau2", "jetAK8tau3", "jetAK8topMass", "jetAK8trimmedMass", "jetAK8wMass", jpref+"PartonFlavour", "allExtra"),
        )
    )
'''


'''
DMTreesDumper.physicsObjects.append( 
    cms.PSet(
        label = subjetak8label,
        prefix = cms.string(sjpref),
        maxInstances = cms.untracked.int32(10),
        variablesF = cms.VInputTag(
            cms.InputTag(sj,sjpref+"E"),
            cms.InputTag(sj,sjpref+"Pt"),
            cms.InputTag(sj,sjpref+"Mass"),
            cms.InputTag(sj,sjpref+"Eta"),
            cms.InputTag(sj,sjpref+"Phi"),
            cms.InputTag(sj,sjpref+"CSV"),
            ),                         
        variablesI = cms.VInputTag(),
        singleI = cms.VInputTag(),
        singleF = cms.VInputTag(),
        toSave = cms.vstring(sjpref+"E",sjpref+"Pt",sjpref+"Eta",sjpref+"Phi", "allExtra"),
        )
    )
'''

