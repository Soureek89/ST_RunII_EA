{
  TFile f("treesTest_NewSmall.root","OPEN");
  TTree * t = new TTree;

  t=(TTree*)(f.Get("DMTreesDumper/ttDM__noSyst"));
  //Legenda for weights:
  //Event_LHEWeight nominal LHE weight
  //Event_LHEWeightN = LHE weight for
  //N=1-8 ==> fact-renorm scales combinations
  //e.g. 5 == Up+Up; 9==Down+Down
  //N=9-110 == NNPDF weights
  float scalesTotal[8], scalesSF[8];
  float PDFsTotal[102], PDFsSF[102];
  
  //Defining histogram for sum of weights:

  //Nominal: 
  TH1D nominal("nominal","nominal LHE weights integral", 100,-10,10);
  TH1D nominalTotal("nominalTotal","nominal LHE weights integral", 100,-10,10);
  //Scales: before and after selection
  TH1D scalesTotalH[8],scalesH[8];
  //PDF: before and after selection
  TH1D PDFsTotalH[102],PDFsH[102];
  //Observable:
  TH1D myObservableScales[8],myObservablePDFs[102],myObservable;

  //Substitute here with actual selection and observable of interest:
  TString Selection = "(muons_size==1 && met_Pt > 0 && Event_nJetsCut30==2)";//Dummy selection
  TString obs = "met_Pt[0]", obsName = "met";
  myObservable=TH1D(obsName,obsName, 100, 0,2000);
  

  t->Project("nominalTotal","Event_LHEWeight/abs(Event_LHEWeight)");
  t->Project("nominal","Event_LHEWeight/abs(Event_LHEWeight)",Selection);

  t->Project(obsName,obs,"Event_LHEWeight/abs(Event_LHEWeight)*"+Selection);

  cout << " nominal int "<< myObservable.Integral()<<endl;

  //Part 1: scales: this gets the histogram
  //1.1: Get the effect on the observable and on the total cross section
  for(int s = 1; s <= 8; ++s){
    stringstream s_n,s_nm;
    s_nm<< (s-1);    
    s_n<< s;
    TString scaleName("scales"+s_nm.str());
    TString obsScaleName(obsName+scaleName);
    TString weight("Event_LHEWeight"+s_n.str()+"/abs(Event_LHEWeight)");
    TString weightFill("Event_LHEWeight"+s_n.str()+"/abs(Event_LHEWeight)");
 
    cout << weight<<endl;
    scalesTotalH[s-1]=TH1D(scaleName+"Total",scaleName+"Total", 1000, -10,10); //This gives you the total cross-section variation
    scalesH[s-1]=TH1D(scaleName,scaleName, 1000, -10,10);
    myObservableScales[s-1]=TH1D(obsScaleName,obsScaleName, 100, 0,2000);

    t->Project(scaleName+"Total",weight);
    t->Project(scaleName,weight,Selection);
    t->Project(obsScaleName,obs,weightFill+"*"+Selection);
    cout<< " name "<< myObservableScales[s-1].GetName() <<" int "<<myObservableScales[s-1].Integral()<<endl;

  }
  //1.2: Get the effect on the cut acceptance only
  for(int s = 1; s <= 8; ++s){
    double obsVariation = myObservableScales[s-1].Integral() / myObservable.Integral();
   
    double crossSecVariation = scalesTotalH[s-1].GetMean()/nominalTotal.GetMean();
    double scaleVariation = scalesH[s-1].GetMean()/nominal.GetMean();

    cout << "cross check if the following two are equals: obs variation is"<< obsVariation<< " scale variation is "<< scaleVariation <<endl;
    cout << "sel Integral: "<<myObservable.Integral()<<endl;
    cout << "var Integral: "<<myObservableScales[s-1].Integral()<<endl;
    cout << "cross section : "<< nominalTotal.GetMean()<<endl;
    cout << "cross section vared: "<< scalesTotalH[s-1].GetMean()<<endl;
    //Rescale the histogram to take into account the varied cross section:
    myObservableScales[s-1].Scale(1.0/crossSecVariation);
    cout << "rescaled integral is: " << myObservableScales[s-1].Integral()<<endl;
  }

  //Application:
  //Apply all 8 systematics
  //Take maximum variation


  //PDF Part: analogous for the PDFs
  for(int s = 9; s <= 110; ++s){
    stringstream s_n,s_nm;
    s_nm<< (s-9);    
    s_n<< s;
    TString PDFName("PDF"+s_nm.str());
    TString obsPDFName(obsName+PDFName);
    TString weight("Event_LHEWeight"+s_n.str()+"/abs(Event_LHEWeight)");
    TString weightFill("Event_LHEWeight"+s_n.str()+"/abs(Event_LHEWeight)");
 
    //    cout << weight<<endl;
    PDFsTotalH[s-9]=TH1D(PDFName+"Total",PDFName+"Total", 1000, -10,10); //This gives you the total cross-section variation
    PDFsH[s-9]=TH1D(PDFName,PDFName, 1000, -10,10);
    myObservablePDFs[s-9]=TH1D(obsPDFName,obsPDFName, 100, 0,2000);

    t->Project(PDFName+"Total",weight);
    t->Project(PDFName,weight,Selection);
    t->Project(obsPDFName,obs,weightFill+"*"+Selection);
    //    cout<< " name "<< myObservablePDFs[s-9].GetName() <<" int "<<myObservablePDFs[s-9].Integral()<<endl;
    
  }
  for(int s = 9; s <= 110; ++s){
    double obsVariation = myObservablePDFs[s-9].Integral() / myObservable.Integral();
    double crossSecVariation = PDFsTotalH[s-9].GetMean()/nominalTotal.GetMean();
    double PDFVariation = PDFsH[s-9].GetMean()/nominal.GetMean();
    //Rescale the histogram to take into account the varied cross section:
    cout << " pdf # "<< s-9 <<endl;
    cout << "integral is: " << myObservablePDFs[s-9].Integral()<<endl;
    myObservablePDFs[s-9].Scale(1.0/crossSecVariation);
    cout << "rescaled integral is: " << myObservablePDFs[s-9].Integral()<<endl;
    }

  //Apply all 102 systematics
  //Take RMS of the distribution of the results

}
