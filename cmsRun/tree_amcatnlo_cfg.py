### *****************************************************************************************
### Usage:
###
### cmsRun topplusdmanaEDMntuples_cfg.py maxEvts=N sample="mySample/sample.root" version="71" outputLabel="myoutput"
###
### Default values for the options are set:
### maxEvts     = -1
### sample      = 'file:/scratch/decosa/ttDM/testSample/tlbsm_53x_v3_mc_10_1_qPV.root'
### outputLabel = 'analysisTTDM.root'
### *****************************************************************************************
import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as opts



options = opts.VarParsing ('analysis')

options.register('maxEvts',
#                 -1,# default value: process all events
                 5,		
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.int,
                 'Number of events to process')

options.register('sample',
                 [
#	'file:/tmp/oiorio/B2GEDMNtuple_1.root'
#       'file:../../edm_mc/B2GEDMNtuple.root'
#       'file:/afs/cern.ch/work/n/nfalterm/public/B2GEDMNtuple.root'
#       'root://xrootd.ba.infn.it///store/user/decosa/ttDM/CMSSW_7_4_X/TT_TuneCUETP8M1_13TeV-powheg-pythia8/TT_TuneCUETP8M1_13TeV/150926_070344/0000/B2GEDMNtuple_1.root'
#	'root://se01.indiacms.res.in//store/user/smitra/25ns/EDMTuple_74Xv8/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/DYJets_EDMTuple_74Xv8/151109_200859/0000/DYJets_EDMTuple_1.root'	
#	'root://se01.indiacms.res.in//store/user/smitra/25ns/TopMass/EDMTuple_80X/ST_t-channel_top_4f_leptonDecays_13TeV-powheg-pythia8_TuneCUETP8M1/TChannel_Powheg_EDMTuple/160714_102438/0000/TChannel_aMCatNLO_EDMTuple_9.root'
        #'file:/store/user/smitra/25ns/TopMass/2017/EDMTuple_80X/ST_t-channel_top_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/TChannel_Powheg_EDMTuple_Summer16/170309_045551/0000/TChannel_Powheg_EDMTuple_1.root'
#        'root://se01.indiacms.res.in//store/user/smitra/25ns/TopMass/2017/EDMTuple_80X/ST_t-channel_top_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/TChannel_Powheg_EDMTuple_Summer16/170309_045551/0000/TChannel_Powheg_EDMTuple_1.root'
#	'file:/storage/c/smitra/B2GEDMNtuple_Signal.root'
	'root://se01.indiacms.res.in//store/user/smitra/25ns/TopMass/2017/EDMTuple_80X/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/TTbar_EDMTuple_Summer16/170310_092846/0000/TTbar_EDMTuple_1.root'	
        ],
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'Sample to analyze')

options.register('outputLabel',
                # 'treesTest_NewSmall_EleTrig.root',
                #'TChannel_Powheg.root',
                "TTbar.root",
                # 'tree.root',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'Output label')

options.register('isData',
                 False,
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.bool,
                 'Is data?')

options.register('useLHE',
                 True,
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.bool,
                 'Keep LHEProducts')

options.register('lhes',
#                 'source',
                 'externalLHEProducer',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
		 'name from generator')

options.register('syst',
                 ['noSyst'],
#                 ['noSyst','jer__up','jer__down','jes__up','jes__down','unclusteredMet__up','unclusteredMet__down'],
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'systematic trees')


options.register('globalTag',
#                 '76X_mcRun2_asymptotic_v12',
                 #'76X_dataRun2_v15',
 		'80X_mcRun2_asymptotic_2016_miniAODv2',	
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'global tag to be used')

options.parseArguments()

if(options.isData):options.useLHE = False

process = cms.Process("ttDManalysisTrees")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.categories.append('HLTrigReport')
### Output Report
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
### Number of maximum events to process
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvts) )
### Source file
process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
        options.sample
        )
)

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
print "# using GT ", options.globalTag
process.GlobalTag.globaltag = options.globalTag

#from Configuration.AlCa.GlobalTag import GlobalTag as customiseGlobalTag
#process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'auto:run2_mc_50nsGRun')
#process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'
#process.GlobalTag.pfnPrefix = cms.untracked.string('frontier://FrontierProd/')

#for pset in process.GlobalTag.toGet.value():
#    pset.connect = pset.connect.value().replace('frontier://FrontierProd/', 'frontier://FrontierProd/')
#    #   Fix for multi-run processing:
#    process.GlobalTag.RefreshEachRun = cms.untracked.bool( False )
#    process.GlobalTag.ReconnectEachRun = cms.untracked.bool( False )

    

### Rootplizer

process.TFileService = cms.Service("TFileService", fileName = cms.string(options.outputLabel))
process.load("TreeMaker.ST_RunII_EA.topplusdmedmRootTreeMaker_cff")
#process.DMTreesDumper.lhes =cms.InputTag("externalLHEProducer")
process.DMTreesDumper.lhes =cms.InputTag(options.lhes)
process.DMTreesDumper.channelInfo.useLHE =(options.useLHE)
process.DMTreesDumper.systematics =(options.syst)
process.DMTreesDumper.changeJECs = cms.untracked.bool(False)# JEC via GT
process.DMTreesDumper.useMETNoHF = cms.untracked.bool(False)
#process.DMTreesDumper.addPV = cms.untracked.bool(True)
process.DMTreesDumper.channelInfo.useLHEWeights =cms.untracked.bool(True)
process.DMTreesDumper.channelInfo.addLHAPDFWeights =cms.untracked.bool(True)
process.DMTreesDumper.channelInfo.topPtreweight=cms.untracked.bool(True)
process.DMTreesDumper.isData = cms.untracked.bool(False)#This adds the L2L3Residuals
process.DMTreesDumper.doPU= cms.bool(True);
process.DMTreesDumper.dataPUFile=cms.string("DistrSummer16_25ns");

process.analysisPath = cms.Path(
    process.DMTreesDumper
    )
