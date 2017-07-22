from CRABClient.UserUtilities import config, getLumiListInValidFiles
from WMCore.DataStructs.LumiList import LumiList

config = config()

#config.General.requestName = 'TTbar_Recovery'
#config.General.requestName = 'TChannel_Recovery'
#config.General.requestName = 'TbarChannel_Recovery'
#config.General.requestName = 'WJets_Recovery'
#config.General.requestName = 'Data_RunG_Recovery_v2'
config.General.requestName = 'DYJets_Recovery'

config.General.transferLogs = True
# you want to use same Pset as in previous task, in order to publish in same dataset
config.JobType.psetName = 'tree_amcatnlo_cfg.py'
config.JobType.allowUndistributedCMSSW = True
config.JobType.pluginName = 'Analysis'

#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TChannel_Powheg_Summer16_80X.root","isData=False","useLHE=True", "lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TbarChannel_Powheg_Summer16_80X.root","isData=False","useLHE=True", "lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_TTbar_Summer16_80X.root","isData=False","useLHE=True","lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_WJets_aMCatNLO_Inclusive_Summer16_80X.root","isData=False","useLHE=True", "lhes=externalLHEProducer"]
config.JobType.pyCfgParams =["maxEvents=-1","outputLabel=Trees_DYJets_aMCatNLO_Inclusive_Summer16_80X.root","isData=False","useLHE=True", "lhes=externalLHEProducer"]
#config.JobType.pyCfgParams =["maxEvts=-1","outputLabel=Trees_Data_RunG_v1_ReReco_80X.root","useLHE=False","isData=True"]

#config.JobType.psetName = 'tree_data.py'

config.JobType.inputFiles = ['Summer16JEC/Summer16_23Sep2016HV4_DATA_L1FastJet_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_L2Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_L3Absolute_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_L2L3Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_UncertaintySources_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016V4_MC_L1FastJet_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016V4_MC_L2Relative_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016V4_MC_L3Absolute_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016V4_MC_UncertaintySources_AK4PFchs.txt','pileUpDistrSummer16_25ns.root','DataPileupHistogram_69p2mbMinBias.root','DataPileupHistogram_69p2mbMinBias_down.root','DataPileupHistogram_69p2mbMinBias_up.root','cMVAv2_Moriond17_B_H.csv','btagging_cmva.root']

#config.JobType.inputFiles = ['Summer16JEC/Summer16_23Sep2016BCDV4_DATA_L1FastJet_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016BCDV4_DATA_L2Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016BCDV4_DATA_L3Absolute_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016BCDV4_DATA_L2L3Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016BCDV4_DATA_UncertaintySources_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016EFV4_DATA_L1FastJet_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016EFV4_DATA_L2Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016EFV4_DATA_L3Absolute_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016EFV4_DATA_L2L3Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016EFV4_DATA_UncertaintySources_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016GV4_DATA_L1FastJet_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016GV4_DATA_L2Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016GV4_DATA_L3Absolute_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016GV4_DATA_L2L3Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016GV4_DATA_UncertaintySources_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_L1FastJet_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_L2Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_L3Absolute_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_L2L3Residual_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016HV4_DATA_UncertaintySources_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016V4_MC_L1FastJet_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016V4_MC_L2Relative_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016V4_MC_L3Absolute_AK4PFchs.txt','Summer16JEC/Summer16_23Sep2016V4_MC_UncertaintySources_AK4PFchs.txt','pileUpDistrSummer16_25ns.root','DataPileupHistogram_69p2mbMinBias.root','DataPileupHistogram_69p2mbMinBias_down.root','DataPileupHistogram_69p2mbMinBias_up.root','cMVAv2_Moriond17_B_H.csv','btagging_cmva.root']

# and of course same input dataset
#config.Data.inputDataset = '/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/smitra-TTbar_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER' 
#config.Data.inputDataset = '/ST_t-channel_top_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/smitra-TChannel_Powheg_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'
#config.Data.inputDataset = '/ST_t-channel_antitop_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/smitra-TbarChannel_Powheg_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'
#config.Data.inputDataset = '/WJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/smitra-WJets_aMCatNLO_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'
config.Data.inputDataset = '/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/smitra-DYJets_aMCatNLO_EDMTuple_Summer16-de822f6bc13a69d387b17bc05e2b65fc/USER'
#config.Data.inputDataset = '/SingleMuon/smitra-SingleMu_2016G_23SepReReco_v1-56c7d11e670cf8cfec530dfb67866b49/USER'


config.Data.inputDBS = 'phys03'  # but this will work for a dataset in phys03 as well
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1 

#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TChannel_Powheg/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TTbar/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/TbarChannel_Powheg/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/WJets_Inclusive_aMCatNLO/'
config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/DYJets_aMCatNLO/'
#config.Data.outLFNDirBase = '/store/user/smitra/25ns/TopMass/2017/Trees_80X/Data_RunG_ReReco/'

# now the list of lumis that you successfully processed in Task-A
# it can be done in two ways. Uncomment and edit the appropriate one:
#1. (recommended) when Task-A output was a dataset published in DBS
#taskALumis = getLumiListInValidFiles(dataset=<TaskA-output-dataset-name>, dbsurl='phys03')
# or 2. when output from Task-A was not put in DBS

#taskALumis = LumiList(filename="crab_Data_RunG_v1_ReReco/results/processedLumis.json")

# now the current list of golden lumis for the data range you are interested, can be different from the one used in Task-A
#officialLumiMask = LumiList(filename='crab_Data_RunG_v1_ReReco/results/lumisToProcess.json') 

# this is the main trick. Mask out also the lumis which you processed already
#newLumiMask = officialLumiMask - taskALumis 

# write the new lumiMask file, now you can use it as input to CRAB
#newLumiMask.writeJSON('lumi_mask_DataG_v2.json')
# and there we, process from input dataset all the lumi listed in the current officialLumiMask file, skipping the ones you already have.
#config.Data.lumiMask = 'lumi_mask_DataG_v2.json' 
config.Data.lumiMask = 'DYJets_recovery.json'
#config.Data.outputDatasetTag =  #  add to your existing dataset
config.Site.storageSite = 'T2_IN_TIFR'
