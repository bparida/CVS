#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <istream>
#include <sstream>

#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "THStack.h"
#include "TString.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TPaveLabel.h"
#include "TPaveText.h"
#include "TLatex.h"
#include "TMath.h"

#include "TF1.h"

#include "ntpleUtils.h"
#include "ConfigParser.h"
#include "ReadInputFile.h"
#include "DataMCPlotTool.h"

void banner4Plot (const bool & isLabel){

  TPaveText* pt ;
  if (!isLabel) pt = new TPaveText(.20,0.72,.34,.91,"NDC");
  else pt = new TPaveText(.22,0.72,.28,.91,"NDC");
  pt->AddText("CA R = 0.8");
  //  pt->AddText("250 < p_{T} < 350 GeV");
  pt->AddText("p_{T} > 200 GeV");
  pt->AddText("|#eta|<2.4");
  pt->AddText("40 < m_{j} < 130 GeV");                                                                                                                                                       

  pt->SetFillColor(0);
  pt->SetTextSize(0.035);
  pt->SetFillStyle(0);
  pt->SetLineColor(0);
  pt->SetLineWidth(0);
  pt->SetMargin(0);
  pt->SetShadowColor(0);
  pt->Draw();

}


// To lunch the sequence for the Higgs : 

// 1) Ele : ls cfg/DataMCComparison_InputCfgFile/ | grep _el_ | grep HWW | grep cfg | grep -v ~ | awk '{print "./bin/DataMCComparisonPlot.exe cfg/DataMCComparison_InputCfgFile/"$1}' | /bin/sh
// 2) Mu  : ls cfg/DataMCComparison_InputCfgFile/ | grep _mu_ | grep HWW | grep cfg | grep -v ~ | awk '{print "./bin/DataMCComparisonPlot.exe cfg/DataMCComparison_InputCfgFile/"$1}' | /bin/sh


// To lunch the sequence for the RSG Graviton : 

// 1) Ele : ls cfg/DataMCComparison_InputCfgFile/ | grep _el_ | grep RSG | grep cfg | grep -v ~ | awk '{print "./bin/DataMCComparisonPlot.exe cfg/DataMCComparison_InputCfgFile/"$1}' | /bin/sh
// 2) Mu  : ls cfg/DataMCComparison_InputCfgFile/ | grep _mu_ | grep RSG | grep cfg | grep -v ~ | awk '{print "./bin/DataMCComparisonPlot.exe cfg/DataMCComparison_InputCfgFile/"$1}' | /bin/sh

int main (int argc, char **argv){

  if(argc<2){ std::cout<<" Not correct number of input parameter --> Need Just one cfg file exit "<<std::endl; return -1; }

  // Load TTree Lybrary                                                                                                                                                     

  gSystem->Load("libTree.so");

  // Set Root style from global enviroment path                                                                                                                                  

  std::string ROOTStyle =  getenv ("ROOTStyle");

  gROOT->ProcessLine((".x "+ROOTStyle+"/rootLogon.C").c_str());
  gROOT->ProcessLine((".x "+ROOTStyle+"/rootPalette.C").c_str());
  gROOT->ProcessLine((".x "+ROOTStyle+"/rootColors.C").c_str());
  gROOT->ProcessLine((".x "+ROOTStyle+"/setTDRStyle.C").c_str());

  gStyle->SetPadLeftMargin(0.25);
  gStyle->SetPalette(1);
  gStyle->SetErrorX(0.5);

  // parse config file parameter
  
  parseConfigFile(argv[1]);

  std::string InputDirectory     = gConfigParser -> readStringOption("Input::InputDirectory");
  std::string InputSampleList    = gConfigParser -> readStringOption("Input::InputSampleList");
  std::string InputVariableList  = gConfigParser -> readStringOption("Input::InputVariableList");

  std::string InputVariableListBlinded ;
  try{  InputVariableListBlinded = gConfigParser -> readStringOption("Input::InputVariableListBlinded");}
  catch(const char* exceptionString){ InputVariableListBlinded = "NULL" ;
                                      std::cerr<<" InputVariableListBlinded Set by default to --> NULL "<<std::endl;
  }                                     

  std::string InputCutList       = gConfigParser -> readStringOption("Input::InputCutList");

  std::cout<<"      "<<std::endl;
  std::cout<<" InputDirectory: "<<InputDirectory<<std::endl;
  std::cout<<"      "<<std::endl;
  std::cout<<" InputSampleList: "<<InputSampleList<<std::endl;
  std::cout<<"      "<<std::endl;
  std::cout<<" InputVariableList: "<<InputVariableList<<std::endl;
  std::cout<<"      "<<std::endl;
  std::cout<<" InputVariableListBlinded: "<<InputVariableListBlinded<<std::endl;
  std::cout<<"      "<<std::endl;
  std::cout<<" InputCutList: "<<InputCutList<<std::endl;
  std::cout<<"      "<<std::endl;


  std::string TreeName ;
  try{ TreeName  = gConfigParser -> readStringOption("Input::TreeName");}
  catch(char const* exceptionString){ TreeName = "WJet"; 
                                      std::cerr<<" TreeName Set by default to --> WJet "<<std::endl;
  }

  std::cout<<" TreeName: "<<TreeName<<std::endl;
  std::cout<<"      "<<std::endl;

  std::string LeptonType ;
  try{ LeptonType  = gConfigParser -> readStringOption("Input::LeptonType");}
  catch(char const* exceptionString){ LeptonType = "none"; 
                                      std::cerr<<" LeptonType Set by default to --> none "<<std::endl;
  }

  std::cout<<" TreeName: "<<TreeName<<std::endl;
  std::cout<<"      "<<std::endl;
  
  std::string SignalggHName ; 
  try{  SignalggHName = gConfigParser -> readStringOption("Input::SignalggHName");}
  catch(char const* exceptionString) { SignalggHName = "NULL";
                                       std::cerr<<" No Signal ggH Name --> NULL  "<<std::endl;
  }

  std::cout<<" SignalggHName: "<<SignalggHName<<std::endl;
  std::cout<<"      "<<std::endl;

  std::string SignalqqHName ;
  try{  SignalqqHName = gConfigParser -> readStringOption("Input::SignalqqHName");}
  catch(char const* exceptionString) { SignalqqHName = "NULL";
                                       std::cerr<<" No Signal qqH Name --> NULL "<<std::endl;
  }

  std::cout<<" SignalqqHName: "<<SignalqqHName<<std::endl;
  std::cout<<"      "<<std::endl;

  std::string SignalRSGPythiaName ;
  try{  SignalRSGPythiaName = gConfigParser -> readStringOption("Input::SignalPythiaName");}
  catch(char const* exceptionString){ SignalRSGPythiaName = "NULL";
                                      std::cerr<<" No Signal RSG Pythia Name --> NULL "<<std::endl;
  }

  std::cout<<" SignalRSGPythiaName: "<<SignalRSGPythiaName<<std::endl;
  std::cout<<"      "<<std::endl;

  std::string SignalRSGHerwigName ;
  try{  SignalRSGHerwigName = gConfigParser -> readStringOption("Input::SignalHerwigName");}
  catch(char const* exceptionString) { SignalRSGHerwigName = "NULL";
                                       std::cerr<<" No Signal RSG Herwig Name --> NULL "<<std::endl;
  }

  std::cout<<" SignalRSGHerwigName: "<<SignalRSGHerwigName<<std::endl;
  std::cout<<"      "<<std::endl;

  std::string SignalGravitonName ;
  try{  SignalGravitonName = gConfigParser -> readStringOption("Input::SignalGraviton");}
  catch(char const* exceptionString) { SignalGravitonName = "NULL";
                                       std::cerr<<" No Signal Grvaiton  Name --> NULL "<<std::endl;
  }

  std::cout<<" SignalGravitonName: "<<SignalGravitonName<<std::endl;
  std::cout<<"      "<<std::endl;


  bool   WithoutData  = gConfigParser -> readBoolOption("Input::WithoutData");

  std::string BackgroundWeight   = gConfigParser -> readStringOption("Option::BackgroundWeight");

  std::cout<<" BackgroundWeight: "<<BackgroundWeight<<std::endl;
  std::cout<<"      "<<std::endl;

  std::string BackgroundWeight_mcatnlo;
  try{ BackgroundWeight_mcatnlo  = gConfigParser -> readStringOption("Option::BackgroundWeightMCatNLO");}
  catch(char const* exceptionString) { BackgroundWeight_mcatnlo="1";
                                       std::cerr<<" BackgroundWeight_mcatnlo set to --> 1 Default "<<std::endl;
  }

  std::cout<<" BackgroundWeight_mcatnlo: "<<BackgroundWeight_mcatnlo<<std::endl;
  std::cout<<"      "<<std::endl;

  std::string SignalggHWeight;
  try{ SignalggHWeight  = gConfigParser -> readStringOption("Option::SignalggHWeight");}
  catch(char const* exceptionString) { SignalggHWeight="1";
                                       std::cerr<<" Weight ggH set to --> 1 Default "<<std::endl;
  }

  std::cout<<" SignalggHWeight: "<<SignalggHWeight<<std::endl;
  std::cout<<"      "<<std::endl;

  std::string SignalqqHWeight;
  try{ SignalqqHWeight  = gConfigParser -> readStringOption("Option::SignalqqHWeight");}
  catch(char const* exceptionString) { SignalqqHWeight="1";
                                       std::cerr<<" Weight qqH set to --> 1 Default "<<std::endl;
  }

  std::cout<<" SignalqqHWeight: "<<SignalqqHWeight<<std::endl;
  std::cout<<"      "<<std::endl;

  std::string SignalRSGPythiaWeight;
  try{ SignalRSGPythiaWeight  = gConfigParser -> readStringOption("Option::SignalPythiaWeight");}
  catch(char const* exceptionString) { SignalRSGPythiaWeight="1";
                                       std::cerr<<" Weight Pyhtia set to --> 1 Default "<<std::endl;
  }

  std::cout<<" SignalRSGPythiaWeight: "<<SignalRSGPythiaWeight<<std::endl;
  std::cout<<"      "<<std::endl;

  std::string SignalRSGHerwigWeight;
  try{ SignalRSGHerwigWeight  = gConfigParser -> readStringOption("Option::SignalHerwigWeight");}
  catch(char const* exceptionString) { SignalRSGHerwigWeight="1";
                                       std::cerr<<" Weight Herwig set to --> 1 Default "<<std::endl;
  }

  std::cout<<" SignalRSGHerwigWeight: "<<SignalRSGHerwigWeight<<std::endl;
  std::cout<<"      "<<std::endl;

  std::string SignalGravitonWeight;
  try{ SignalGravitonWeight  = gConfigParser -> readStringOption("Option::SignalGravitonWeight");}
  catch(char const* exceptionString) { SignalGravitonWeight="1";
                                       std::cerr<<" Weight Herwig set to --> 1 Default "<<std::endl;
  }

  std::cout<<" SignalGravitonWeight: "<<SignalGravitonWeight<<std::endl;
  std::cout<<"      "<<std::endl;

 
  double Lumi         = gConfigParser -> readDoubleOption("Option::Lumi");

  std::cout<<" Lumi: "<<Lumi<<std::endl;
  std::cout<<"      "<<std::endl;


  double SignalScaleFactor;
  try{ SignalScaleFactor  = gConfigParser -> readDoubleOption("Option::SignalScaleFactor");}
  catch(char const* exceptionString) { SignalScaleFactor=100;
                                       std::cerr<<" Signal Scale Factor  --> 100 Default "<<std::endl;
  }

  std::cout<<" Signal Scale Factor : "<<SignalScaleFactor<<std::endl;
  std::cout<<"      "<<std::endl;

  bool NormalizeSignalToData ;
  try{ NormalizeSignalToData  = gConfigParser -> readBoolOption("Option::NormalizeSignalToData");}
  catch(char const* exceptionString) { NormalizeSignalToData = false ;
                                       std::cerr<<" Normalize to Data  --> False Default "<<std::endl;
  }

  std::cout<<" Normalize Signal to Data Flag : "<<NormalizeSignalToData<<std::endl;
  std::cout<<"      "<<std::endl;

  bool NormalizeBackgroundToData = false  ;
  try{ NormalizeBackgroundToData  = gConfigParser -> readBoolOption("Option::NormalizeBackgroundToData");}
  catch(char const* exceptionString) { NormalizeBackgroundToData = false ;
                                       std::cerr<<" Normalize Bkg to Data  --> False Default "<<std::endl;
  }

  std::cout<<" Normalize Bkg to Data Flag : "<<NormalizeBackgroundToData<<std::endl;
  std::cout<<"      "<<std::endl;

  std::string OutputRootDirectory   = gConfigParser -> readStringOption("Output::OutputRootDirectory");

  std::cout<<" OutputRootDirectory: "<<OutputRootDirectory<<std::endl;
  std::cout<<"      "<<std::endl;

  std::string OutputRootFile        = gConfigParser -> readStringOption("Output::OutputRootFile");

  std::cout<<" OutputRootFile: "<<OutputRootFile<<std::endl;
  std::cout<<"      "<<std::endl;

  
  std::string command = "if [ ! -e "+OutputRootDirectory+" ] ; then mkdir "+OutputRootDirectory+" ; fi";
  std::cout<<" command = "<<command<<std::endl;
  std::cout<<"           "<<std::endl;

  system(command.c_str());

  
  std::string OutputPlotDirectory = OutputRootDirectory+"/"+OutputRootFile ;
  OutputPlotDirectory.replace(OutputPlotDirectory.end()-5,OutputPlotDirectory.end(),"_plot");
  std::cout<<" OutputPlotDirectory = "<<OutputPlotDirectory<<std::endl;
  std::cout<<"           "<<std::endl;
  
  
  command = "if [ ! -e "+OutputPlotDirectory+" ] ; then mkdir "+OutputPlotDirectory+" ; fi";
  std::cout<<" command = "<<command<<std::endl;
  std::cout<<"           "<<std::endl;

  system(command.c_str());

  
  command = "if [ ! -f "+OutputPlotDirectory+" ] ; then rm "+OutputPlotDirectory+"/* ; fi";
  std::cout<<" command = "<<command<<std::endl;
  std::cout<<"           "<<std::endl;
  
  system(command.c_str());
  

  TFile *outputFile = new TFile((OutputRootDirectory+"/"+OutputRootFile).c_str(),"RECREATE");


  // read sample input file list to Plot

  std::vector <std::string> NameSample;
  std::vector <std::string> NameReducedSample;
  std::vector <int> ColorSample;
  std::vector <double> SampleCrossSection;
  std::vector <int> NumEntriesBefore;


  if(ReadInputSampleFile(InputSampleList,NameSample,NameReducedSample,ColorSample,SampleCrossSection,NumEntriesBefore) <= 0){ 
    std::cerr<<" Empty Input Sample File or not Exisisting --> Exit "<<std::endl; return -1;}


  // read input variables to Plot

  std::vector <std::string> Variables;
  std::vector <double> VariablesMinValue;
  std::vector <double> VariablesMaxValue;
  std::vector <int> VariablesNbin;
  std::vector <std::string> VariablesTitle;

 
  if(ReadInputVariableFile(InputVariableList,Variables,VariablesNbin,VariablesMinValue,VariablesMaxValue,VariablesTitle) <= 0){ 
    std::cerr<<" Empty Variable List File or not Exisisting --> Exit "<<std::endl; return -1;}

  std::vector <double> VariablesBlindedMinValue(Variables.size(),-999.);
  std::vector <double> VariablesBlindedMaxValue(Variables.size(),-999.);

 
  if(InputVariableListBlinded == "NULL" || ReadInputVariableBlindedFile(InputVariableListBlinded,Variables,VariablesNbin,VariablesMinValue,VariablesMaxValue,
                                                                         VariablesBlindedMinValue, VariablesBlindedMaxValue, VariablesTitle) <=0)
    std::cerr<<" Empty Variable Blinded List File or not Exisisting --> Exit "<<std::endl;

 

  std::vector <std::string> CutList;

  if(ReadInputCutFile(InputCutList,CutList) <= 0){ 
    std::cerr<<" Empty Cut List File or not Exisisting --> Exit "<<std::endl; return -1;}

  // take input File and related tree from sampleList and do the plots

  std::vector <TTree*> TreeVect;
  std::vector <TFile*> FileVect;

  TH1F* histos[CutList.size()][Variables.size()][NameSample.size()];
  TString hname ;

  bool isttbar_controlplots = false ;
  
  for (size_t iCut=0; iCut<CutList.size(); iCut++){
    
    std::cout<<std::endl;
    std::cout<<" Cut String "<<CutList.at(iCut)<<std::endl;
    std::cout<<std::endl;

    for (size_t iVar=0; iVar<Variables.size(); iVar++){

     std::cout<<std::endl;
     std::cout<<" Variable "<<Variables.at(iVar)<<std::endl;
     std::cout<<std::endl;

     for (size_t iSample=0; iSample<NameSample.size(); iSample++){

       TString NameFile = Form("%s/%s.root",InputDirectory.c_str(),NameSample.at(iSample).c_str());
       std::cout<<" Input File : "<< NameFile.Data()<<std::endl;

       if( iVar == 0 && iCut == 0) FileVect.push_back ( new TFile (NameFile.Data(),"READ") );  
       if( iVar == 0 && iCut == 0) TreeVect.push_back( (TTree*) FileVect.at(iSample)->Get(TreeName.c_str()));
        
       hname.Form ("%s_%s_%d",NameSample.at(iSample).c_str(),Variables.at(iVar).c_str(),int(iCut));
       hname.ReplaceAll("[","_");
       hname.ReplaceAll("]","_");
       hname.ReplaceAll("(","_");
       hname.ReplaceAll(")","_");
 
       histos[iCut][iVar][iSample] = new TH1F (hname.Data(),"",VariablesNbin.at(iVar),VariablesMinValue.at(iVar),VariablesMaxValue.at(iVar));
       histos[iCut][iVar][iSample]->Sumw2();
       
       if( NameReducedSample.at(iSample) == "DATA" ){ 
         TreeVect.at(iSample)-> Draw((Variables.at(iVar)+" >> "+hname.Data()).c_str(), (CutList.at(iCut)).c_str() ,"goff");
	 std::cout<<" Data Entries "<<histos[iCut][iVar][iSample]->GetEntries()<< " weigthed events "<<histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
       }
	 
       else if(NameReducedSample.at(iSample) == SignalggHName && SignalggHName!="NULL"){
             TreeVect.at(iSample)->Draw((Variables.at(iVar)+" >> "+hname.Data()).c_str(),("("+SignalggHWeight+")*( "+CutList.at(iCut)+")").c_str() ,"goff");
             std::cout<<" Signal ggH Entries "<<histos[iCut][iVar][iSample]->GetEntries()<< " weigthed events "<<histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
       }
	 
       else if(NameReducedSample.at(iSample) == SignalqqHName && SignalqqHName!="NULL") {
	 TreeVect.at(iSample)->Draw((Variables.at(iVar)+" >> "+hname.Data()).c_str(),("("+SignalqqHWeight+")*( "+CutList.at(iCut)+")").c_str() ,"goff");
        std::cout<<" Signal qqH Entries "<<histos[iCut][iVar][iSample]->GetEntries()<< " weigthed events "<<histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
       }
	 
       else if(NameReducedSample.at(iSample) == SignalRSGPythiaName && SignalRSGPythiaName!="NULL") {
        TreeVect.at(iSample)->Draw((Variables.at(iVar)+" >> "+hname.Data()).c_str(),("("+SignalRSGPythiaWeight+")*( "+CutList.at(iCut)+")").c_str() ,"goff");
        std::cout<<" Signal RSGPythia Entries "<<histos[iCut][iVar][iSample]->GetEntries()<< " weigthed events "<<histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
       }
	 
       else if(NameReducedSample.at(iSample) == SignalRSGHerwigName && SignalRSGHerwigName!="NULL") {
	TreeVect.at(iSample)->Draw((Variables.at(iVar)+" >> "+hname.Data()).c_str(),("("+SignalRSGHerwigWeight+")*( "+CutList.at(iCut)+")").c_str() ,"goff");
        std::cout<<" Signal RSG Herwig Entries "<<histos[iCut][iVar][iSample]->GetEntries()<< " weigthed events "<<histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
       }
	 
       else if(NameReducedSample.at(iSample) == SignalGravitonName && SignalGravitonName!="NULL") {
	 TreeVect.at(iSample)->Draw((Variables.at(iVar)+" >> "+hname.Data()).c_str(),("("+SignalGravitonWeight+")*( "+CutList.at(iCut)+")").c_str() ,"goff");
         std::cout<<" Signal Graviton Entries "<<histos[iCut][iVar][iSample]->GetEntries()<< " weighted events "<<histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
       }

       else if(NameReducedSample.at(iSample) == "tt_bar_mcatnlo" ){

         isttbar_controlplots = true ;

         TreeVect.at(iSample)->Draw((Variables.at(iVar)+" >> "+hname.Data()).c_str(),("("+BackgroundWeight_mcatnlo+") * ("+CutList.at(iCut)+")").c_str() ,"goff");

	 std::cout<<" Bkg mc@nlo "<<NameSample.at(iSample)<<" Entries "<<histos[iCut][iVar][iSample]->GetEntries()<<" weighted events "<<
                                 histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl; 
       }
       else {

         TreeVect.at(iSample)->Draw((Variables.at(iVar)+" >> "+hname.Data()).c_str(),("("+BackgroundWeight+") * ("+CutList.at(iCut)+")").c_str() ,"goff");
	 std::cout<<" Bkg "<<NameSample.at(iSample)<<" Entries "<<histos[iCut][iVar][iSample]->GetEntries()<<" weighted events "<<
                             histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl; 
       }
       histos[iCut][iVar][iSample]->SetFillColor(ColorSample.at(iSample));
       histos[iCut][iVar][iSample]->SetLineColor(ColorSample.at(iSample));
       histos[iCut][iVar][iSample]->SetLineWidth(2);
       histos[iCut][iVar][iSample]->GetXaxis()->SetTitle(VariablesTitle.at(iVar).c_str());
       histos[iCut][iVar][iSample]->GetXaxis()->SetTitleSize(0.04);
       histos[iCut][iVar][iSample]->GetYaxis()->SetTitle("Events");
       histos[iCut][iVar][iSample]->GetYaxis()->SetTitleSize(0.04);
    } 

   }
  }
  
  std::cout<<std::endl;
  std::cout<<std::endl;

  // Normalization to the lumi of MC samples 
  double norm = 1.;
  double normalizeToData = 1.;
  double scaleFactorWjet_Pythia = 1.;
  double scaleFactorWjet_Herwig = 1.;

  if( argc==3 ) { scaleFactorWjet_Pythia =  std::atof(argv[2]); scaleFactorWjet_Herwig =  std::atof(argv[2]); }
  else if( argc==4 ) {scaleFactorWjet_Pythia =  std::atof(argv[2]); scaleFactorWjet_Herwig =  std::atof(argv[3]); }

  
  for (size_t iCut=0; iCut<CutList.size(); iCut++){

    //    std::cout<<std::endl;
    //    std::cout<<" Normalization Factor for Cut "<<CutList.at(iCut)<<std::endl;
    //    std::cout<<std::endl;

    for (size_t iVar=0; iVar<Variables.size(); iVar++){

      int iSampleData = -1 ; 
      
      for (size_t iSample=0; iSample<NameSample.size(); iSample++){
	if(NameReducedSample.at(iSample) == "DATA") iSampleData = iSample ;
      }
      
      normalizeToData = 1. ;

      for (size_t iSample=0; iSample<NameSample.size(); iSample++){
      
       if(NameReducedSample.at(iSample) == "DATA") continue;
 
       norm =  Lumi*SampleCrossSection.at(iSample) / NumEntriesBefore.at(iSample);

       //       std::cout<< " Sample to be scaled "<<NameReducedSample.at(iSample)<<" Lumi "<<Lumi<<" xs : "<<SampleCrossSection.at(iSample)<<" numEntries "<<NumEntriesBefore.at(iSample)<<
       //	           " Normalization Factor "<<norm<<std::endl;

       if(NameReducedSample.at(iSample) ==  "W+Jets") norm = norm *scaleFactorWjet_Pythia;
       if(NameReducedSample.at(iSample) ==  "W+Jets_herwig") norm = norm *scaleFactorWjet_Herwig;

       if ( NameReducedSample.at(iSample)==SignalggHName && SignalggHName!="NULL") {
         if(!NormalizeSignalToData)  histos[iCut][iVar][iSample]->Scale(1.*norm);
         else histos[iCut][iVar][iSample]->Scale(1.*histos[iCut][iVar][iSampleData]->Integral(0, VariablesNbin.at(iVar)+1)/histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1)); 
      	 //	 std::cout<<" Scaled entries for "<<NameReducedSample.at(iSample)<<" : "<<histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;

       }

       else if ( NameReducedSample.at(iSample)==SignalqqHName && SignalqqHName!="NULL") {
         if(!NormalizeSignalToData)  histos[iCut][iVar][iSample]->Scale(1.*norm);
         else histos[iCut][iVar][iSample]->Scale(1.*histos[iCut][iVar][iSampleData]->Integral(0, VariablesNbin.at(iVar)+1)/histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1));
	 //	 std::cout<<" Scaled entries for "<<NameReducedSample.at(iSample)<<" : "<<histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
       }

       else if ( NameReducedSample.at(iSample)==SignalRSGPythiaName && SignalRSGPythiaName!="NULL"){
         if(!NormalizeSignalToData)  histos[iCut][iVar][iSample]->Scale(1.*norm);
         else histos[iCut][iVar][iSample]->Scale(1.*histos[iCut][iVar][iSampleData]->Integral(0, VariablesNbin.at(iVar)+1)/histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1));
	 //	 std::cout<<" Scaled entries for "<<NameReducedSample.at(iSample)<<" : "<<histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
       }

       else if ( NameReducedSample.at(iSample)==SignalRSGHerwigName && SignalRSGHerwigName!="NULL"){
         if(!NormalizeSignalToData)  histos[iCut][iVar][iSample]->Scale(1.*norm);
         else histos[iCut][iVar][iSample]->Scale(1.*histos[iCut][iVar][iSampleData]->Integral(0, VariablesNbin.at(iVar)+1)/histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1));
	 //	 std::cout<<" Scaled entries for "<<NameReducedSample.at(iSample)<<" : "<<histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
       }

       else if ( NameReducedSample.at(iSample)==SignalGravitonName && SignalGravitonName!="NULL"){
         if(!NormalizeSignalToData)  histos[iCut][iVar][iSample]->Scale(1.*norm);
         else histos[iCut][iVar][iSample]->Scale(1.*histos[iCut][iVar][iSampleData]->Integral(0, VariablesNbin.at(iVar)+1)/histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1));
	 //	 std::cout<<" Scaled entries for "<<NameReducedSample.at(iSample)<<" : "<<histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
       }

       else{ if(!NormalizeBackgroundToData) histos[iCut][iVar][iSample]->Scale(1.*norm);
	     else { histos[iCut][iVar][iSample]->Scale(1.*norm); normalizeToData += histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1) ;
	       //	             std::cout<<" Scaled entries for "<<NameReducedSample.at(iSample)<<" : "<<histos[iCut][iVar][iSample]->Integral(0, VariablesNbin.at(iVar)+1)<<" Entries "<<histos[iCut][iVar][iSample]->GetEntries()<<"   "<<normalizeToData<<std::endl;
                  }
           }
      }
      for (size_t iSample=0; iSample<NameReducedSample.size(); iSample++){
	if(NormalizeBackgroundToData && NameReducedSample.at(iSample)!="DATA" && NameReducedSample.at(iSample)!=SignalGravitonName && NameReducedSample.at(iSample)!=SignalggHName && 
           NameReducedSample.at(iSample)!=SignalqqHName && NameReducedSample.at(iSample)!=SignalRSGHerwigName && NameReducedSample.at(iSample)!=SignalRSGPythiaName ) 
	  histos[iCut][iVar][iSample]->Scale(1.*histos[iCut][iVar][iSampleData]->Integral(0, VariablesNbin.at(iVar)+1)/normalizeToData);
      }

    }
  }


  outputFile->cd();
  
  TCanvas* c[CutList.size()][Variables.size()];
  TCanvas* cLog[CutList.size()][Variables.size()];
  TCanvas* cNoRatio[CutList.size()][Variables.size()];
  TCanvas* cLogNoRatio[CutList.size()][Variables.size()];

  TLegend* leg[CutList.size()][Variables.size()];
  TLegend* legNoRatio[CutList.size()][Variables.size()];

  TH1F*    histo_top[CutList.size()][Variables.size()];
  TH1F*    histo_diboson[CutList.size()][Variables.size()];
  TH1F*    histo_WJets[CutList.size()][Variables.size()];
  TH1F*    histo_WJets_herwig[CutList.size()][Variables.size()];
  TH1F*    histo_ttbar[CutList.size()][Variables.size()];
  TH1F*    histo_ttbar_herwig[CutList.size()][Variables.size()];
  TH1F*    histoSum[CutList.size()][Variables.size()];
  TH1F*    histoSum_herwig[CutList.size()][Variables.size()];
  TH1F*    RatioDataMC[CutList.size()][Variables.size()];
  TH1F*    RatioDataMC_herwig[CutList.size()][Variables.size()];

  TH1F*    RatioDataMC_error[CutList.size()][Variables.size()];

  THStack* hs[CutList.size()][Variables.size()];
  THStack* hs_herwig[CutList.size()][Variables.size()];

  TF1* RatioLine = new TF1("RatioLine","1");
  
  for (size_t iCut=0; iCut<CutList.size(); iCut++){

     int iSampleData = -1;
     int iSampleggH = -1;
     int iSampleqqH = -1;
     int iSampleRSGPythia = -1;
     int iSampleRSGHerwig = -1;
     int iSampleGraviton  = -1;

     for (size_t iVar=0; iVar<Variables.size(); iVar++){

          std::map<int,double> SystematicErrorMap ;
          std::map<int,double> SystematicErrorMap_herwig ;

          TString CanvasName = Form("%s_%zu",Variables.at(iVar).c_str(),iCut);         
	  c[iCut][iVar] = new TCanvas (CanvasName.Data() ,"",500,525) ;

	  TString CanvasNameNoRatio = Form("%s_%zu_NoRatio",Variables.at(iVar).c_str(),iCut);         
	  cNoRatio[iCut][iVar] = new TCanvas (CanvasNameNoRatio.Data() ,"",500,525) ;

          TString CanvasNameLog = Form("%s_%zu_Log",Variables.at(iVar).c_str(),iCut);
          cLog[iCut][iVar] = new TCanvas (CanvasNameLog.Data() ,"",500,525) ;

          TString CanvasNameLogNoRatio = Form("%s_%zu_Log_NoRatio",Variables.at(iVar).c_str(),iCut);
          cLogNoRatio[iCut][iVar] = new TCanvas (CanvasNameLogNoRatio.Data() ,"",500,525) ;

	  leg[iCut][iVar] = new TLegend (0.46, 0.51, 0.85, 0.91);
	  //leg[iCut][iVar] = new TLegend (0.55, 0.63, 0.88, 0.92);
  	  leg[iCut][iVar]->SetFillColor(0);
  	  leg[iCut][iVar]->SetTextSize(0.03);
  	  leg[iCut][iVar]->SetFillStyle(0);
	  leg[iCut][iVar]->SetNColumns(2);
          leg[iCut][iVar]->SetBorderSize(0);

	  //legNoRatio[iCut][iVar] = new TLegend (0.49, 0.51, 0.85, 0.91);
	  legNoRatio[iCut][iVar] = new TLegend (0.42, 0.61, 0.85, 0.91);
  	  legNoRatio[iCut][iVar]->SetFillColor(0);
  	  legNoRatio[iCut][iVar]->SetFillStyle(0);
  	  legNoRatio[iCut][iVar]->SetTextSize(0.03);
	  legNoRatio[iCut][iVar]->SetNColumns(2);
          legNoRatio[iCut][iVar]->SetBorderSize(0);

	  TLegend* legRatio = new TLegend (0.3, 0.28, 0.7, 0.56);

          TString histoName = Form("%s_sTop_%d",Variables.at(iVar).c_str(),int(iCut));

	  histo_top[iCut][iVar] = new TH1F ( histoName.Data(),"",VariablesNbin.at(iVar),
                                             VariablesMinValue.at(iVar),VariablesMaxValue.at(iVar)) ;

          histoName = Form("%s_diboson_%d",Variables.at(iVar).c_str(),int(iCut));

	  histo_diboson[iCut][iVar] = new TH1F ( histoName.Data(),"",VariablesNbin.at(iVar),
                                             VariablesMinValue.at(iVar),VariablesMaxValue.at(iVar)) ;

          histoName = Form("%s_WJets_%d",Variables.at(iVar).c_str(),int(iCut));

	  histo_WJets[iCut][iVar] = new TH1F ( histoName.Data(),"",VariablesNbin.at(iVar),
                                             VariablesMinValue.at(iVar),VariablesMaxValue.at(iVar)) ;

          histoName = Form("%s_WJets_herwig_%d",Variables.at(iVar).c_str(),int(iCut));

	  histo_WJets_herwig[iCut][iVar] = new TH1F ( histoName.Data(),"",VariablesNbin.at(iVar),
                                                      VariablesMinValue.at(iVar),VariablesMaxValue.at(iVar)) ;

          histoName = Form("%s_ttbar_%d",Variables.at(iVar).c_str(),int(iCut));

	  histo_ttbar[iCut][iVar] = new TH1F ( histoName.Data(),"",VariablesNbin.at(iVar),
                                             VariablesMinValue.at(iVar),VariablesMaxValue.at(iVar)) ;

          histoName = Form("%s_ttbar_herwig_%d",Variables.at(iVar).c_str(),int(iCut));

	  histo_ttbar_herwig[iCut][iVar] = new TH1F ( histoName.Data(),"",VariablesNbin.at(iVar),
                                                      VariablesMinValue.at(iVar),VariablesMaxValue.at(iVar)) ;

          histoName = Form("%s_stack_%d",Variables.at(iVar).c_str(),int(iCut));

	  hs[iCut][iVar] = new THStack (histoName.Data(),"") ;

          histoName = Form("%s_stack_herwig%d",Variables.at(iVar).c_str(),int(iCut));

	  hs_herwig[iCut][iVar] = new THStack (histoName.Data(),"") ;

          histoName = Form("%s_sum_%d",Variables.at(iVar).c_str(),int(iCut));

	  histoSum[iCut][iVar] = new TH1F ( histoName.Data(),"",
					    VariablesNbin.at(iVar),VariablesMinValue.at(iVar),VariablesMaxValue.at(iVar)) ;

          histoName = Form("%s_sum_herwig_%d",Variables.at(iVar).c_str(),int(iCut));

	  histoSum_herwig[iCut][iVar] = new TH1F ( histoName.Data(),"",
					           VariablesNbin.at(iVar),VariablesMinValue.at(iVar),VariablesMaxValue.at(iVar)) ;

          
          TPad* upperPad ; TPad* lowerPad ; TPad* upperPadLog ; TPad* lowerPadLog ;  
          TPad* upperPadNoRatio ; TPad *upperPadLogNoRatio ; 

          if(!WithoutData){

              upperPad = new TPad("upperPad", "upperPad", .005, .260, .995, .980);
	      lowerPad = new TPad("lowerPad", "lowerPad", .005, .005, .995, .335);
              upperPad->SetLeftMargin(0.12);
              upperPad->SetRightMargin(0.1);
              lowerPad->SetBottomMargin(0.26);
              lowerPad->SetLeftMargin(0.12);
              lowerPad->SetRightMargin(0.1);

              upperPadLog = new TPad("upperPadLog", "upperPadLog", .005, .260, .995, .980);
              upperPadLog->SetLogy();
	      lowerPadLog = new TPad("lowerPadLog", "lowerPadLog", .005, .005, .995, .335);
              lowerPadLog->SetBottomMargin(0.26);
              upperPadLog->SetLeftMargin(0.12);
              upperPadLog->SetRightMargin(0.1);
              lowerPadLog->SetBottomMargin(0.27);
              lowerPadLog->SetLeftMargin(0.12);
              lowerPadLog->SetRightMargin(0.1);

              upperPadNoRatio = new TPad("upperPadLogNoRatio", "upperPadLogNoRatio", .005, .025, .995, .950);
	      upperPadLogNoRatio = new TPad("lowerPadLogNoRatio", "lowerPadLogNoRatio", .005, .025, .995, .950);
              upperPadNoRatio->SetLeftMargin(0.1);
              upperPadNoRatio->SetRightMargin(0.1);
              upperPadLogNoRatio->SetLeftMargin(0.1);
              upperPadLogNoRatio->SetRightMargin(0.1);
              upperPadLogNoRatio->SetLogy();
          }
          else{
	        upperPad = new TPad("upperPad", "upperPad", .005, .025, .995, .950); 
                upperPadLog = new TPad("upperPadLog", "upperPadLog", .005, .025, .995, .950);
                upperPadLog->SetLogy();
	  }
	  
	  
	  for (size_t iSample = 0; iSample<NameSample.size(); iSample++){
	  
	    if( NameReducedSample.at(iSample) == "DATA"){

	      histos[iCut][iVar][iSample]->SetLineColor(kBlack);
	      histos[iCut][iVar][iSample]->SetLineStyle(1);
              histos[iCut][iVar][iSample]->GetXaxis()->SetTitle((VariablesTitle.at(iVar)).c_str());
              histos[iCut][iVar][iSample]->GetXaxis()->SetTitleSize(0.04);
	      histos[iCut][iVar][iSample]->GetYaxis()->SetTitle("Events");
              histos[iCut][iVar][iSample]->GetYaxis()->SetTitleSize(0.04);
	      iSampleData = iSample;                                                                       
	      if(!WithoutData){ leg[iCut][iVar]->AddEntry( histos[iCut][iVar][iSample], "Data", "ple" );
                                legNoRatio[iCut][iVar]->AddEntry( histos[iCut][iVar][iSample], "Data", "ple" ); 
              }
	    }

	    
	    else if ( NameReducedSample.at(iSample)==SignalggHName && SignalggHName!="NULL") iSampleggH = iSample;

	    else if ( NameReducedSample.at(iSample)==SignalqqHName && SignalqqHName!="NULL") iSampleqqH = iSample;
	    
            else if ( NameReducedSample.at(iSample)==SignalRSGPythiaName && SignalRSGPythiaName!="NULL") iSampleRSGPythia = iSample;
	    
	    else if ( NameReducedSample.at(iSample)==SignalRSGHerwigName && SignalRSGHerwigName!="NULL") iSampleRSGHerwig = iSample;
	    
            else if ( NameReducedSample.at(iSample)==SignalGravitonName && SignalGravitonName!="NULL") iSampleGraviton = iSample;
	  
	    else if (( NameReducedSample.at(iSample)=="STop") ){ 
 
		histo_top[iCut][iVar]->SetFillColor(ColorSample.at(iSample));
		histo_top[iCut][iVar]->SetLineColor(kBlack);
		histo_top[iCut][iVar]->SetLineWidth(2);
		histo_top[iCut][iVar]->Add(histos[iCut][iVar][iSample]); 
		histoSum[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
		histoSum_herwig[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
	    }
	    else if ( NameReducedSample.at(iSample)=="W+Jets"){
  
		histo_WJets[iCut][iVar]->SetFillColor(ColorSample.at(iSample));
		histo_WJets[iCut][iVar]->SetLineColor(kBlack);
		histo_WJets[iCut][iVar]->SetLineWidth(2);
		histo_WJets[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
	        histoSum[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
	    }
	  else if ( NameReducedSample.at(iSample)=="W+Jets_herwig"){
  
		histo_WJets_herwig[iCut][iVar]->SetFillColor(ColorSample.at(iSample));
		histo_WJets_herwig[iCut][iVar]->SetLineColor(kBlack);
		histo_WJets_herwig[iCut][iVar]->SetLineWidth(2);
		histo_WJets_herwig[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
		histoSum_herwig[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
	    }
	    else if ( NameReducedSample.at(iSample)=="tt_bar" && !isttbar_controlplots){  

		histo_ttbar[iCut][iVar]->SetFillColor(ColorSample.at(iSample));
		histo_ttbar[iCut][iVar]->SetLineColor(kBlack);
		histo_ttbar[iCut][iVar]->SetLineWidth(2);
		histo_ttbar[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
		histoSum[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
		histoSum_herwig[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
	    } 

	    else if ( NameReducedSample.at(iSample)=="tt_bar" && isttbar_controlplots){  

		histo_ttbar[iCut][iVar]->SetFillColor(ColorSample.at(iSample));
		histo_ttbar[iCut][iVar]->SetLineColor(ColorSample.at(iSample));
		histo_ttbar[iCut][iVar]->SetLineWidth(2);
		histo_ttbar[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
		histoSum[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
	    } 

	    else if ( NameReducedSample.at(iSample)=="tt_bar_mcatnlo" && isttbar_controlplots){
		histo_ttbar_herwig[iCut][iVar]->SetFillColor(ColorSample.at(iSample));
		histo_ttbar_herwig[iCut][iVar]->SetLineColor(ColorSample.at(iSample));
		histo_ttbar_herwig[iCut][iVar]->SetLineWidth(2);
		histo_ttbar_herwig[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
		histoSum_herwig[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
	    }
	    else if (( NameReducedSample.at(iSample)=="WW") || ( NameReducedSample.at(iSample)=="WZ") || ( NameReducedSample.at(iSample)=="ZZ") ){  
		histo_diboson[iCut][iVar]->SetFillColor(ColorSample.at(iSample));
		histo_diboson[iCut][iVar]->SetLineColor(kBlack);
		histo_diboson[iCut][iVar]->SetLineWidth(2);
		histo_diboson[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
		histoSum[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
		histoSum_herwig[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
	    }
	    else{  histos[iCut][iVar][iSample]->SetFillColor(ColorSample.at(iSample));
                   histos[iCut][iVar][iSample]->SetLineColor(kBlack);
		   hs[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
		   leg[iCut][iVar]->AddEntry( histos[iCut][iVar][iSample], (NameReducedSample.at(iSample)).c_str(), "f" );
		   legNoRatio[iCut][iVar]->AddEntry( histos[iCut][iVar][iSample], (NameReducedSample.at(iSample)).c_str(), "f" );
		   histoSum[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
		   histoSum_herwig[iCut][iVar]->Add(histos[iCut][iVar][iSample]);
	    }

	   }

          if(histo_WJets[iCut][iVar]->GetEntries()!=0 && histo_WJets_herwig[iCut][iVar]->GetEntries() !=0 && !isttbar_controlplots){
            histo_WJets[iCut][iVar]->SetLineColor(kRed);
            histo_WJets_herwig[iCut][iVar]->SetLineColor(kBlue);
	  }
								  
          
	  if(histo_top[iCut][iVar]->GetEntries()!=0)     leg[iCut][iVar]->AddEntry( histo_top[iCut][iVar], "Single Top", "f" );
	  if(histo_diboson[iCut][iVar]->GetEntries()!=0) leg[iCut][iVar]->AddEntry( histo_diboson[iCut][iVar], "WW/WZ/ZZ", "f" );

	  if(histo_WJets[iCut][iVar]->GetEntries()!=0 && histo_WJets_herwig[iCut][iVar]->GetEntries()==0) 
                leg[iCut][iVar]->AddEntry( histo_WJets[iCut][iVar], "W+jets Pythia", "f" );
	  else if(histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && histo_WJets[iCut][iVar]->GetEntries()==0)  
                leg[iCut][iVar]->AddEntry( histo_WJets_herwig[iCut][iVar], "W+jets Herwig", "f" );
	  else if(histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && histo_WJets[iCut][iVar]->GetEntries()!=0){
                leg[iCut][iVar]->AddEntry( histo_WJets[iCut][iVar], "W+jets Pythia", "l" );
                leg[iCut][iVar]->AddEntry( histo_WJets_herwig[iCut][iVar], "W+jets Herwig", "l" );
	  }
  
         if(histo_ttbar[iCut][iVar]->GetEntries()!=0 && histo_ttbar_herwig[iCut][iVar]->GetEntries()==0) 
                leg[iCut][iVar]->AddEntry( histo_ttbar[iCut][iVar], "t#bar{t} powheg", "f" );
	  else if(histo_ttbar_herwig[iCut][iVar]->GetEntries()!=0 && histo_ttbar[iCut][iVar]->GetEntries()==0)  
                leg[iCut][iVar]->AddEntry( histo_ttbar_herwig[iCut][iVar], "t#bar{t} mc@nlo", "f" );
	  else if(histo_ttbar_herwig[iCut][iVar]->GetEntries()!=0 && histo_ttbar[iCut][iVar]->GetEntries()!=0){
                leg[iCut][iVar]->AddEntry( histo_ttbar[iCut][iVar], "t#bar{t} powheg", "l" );
                leg[iCut][iVar]->AddEntry( histo_ttbar_herwig[iCut][iVar], "t#bar{t} mc@nlo", "l" );
	  }
  
	  if(histo_top[iCut][iVar]->GetEntries()!=0)     legNoRatio[iCut][iVar]->AddEntry( histo_top[iCut][iVar], "Single Top", "f" );
	  if(histo_diboson[iCut][iVar]->GetEntries()!=0) legNoRatio[iCut][iVar]->AddEntry( histo_diboson[iCut][iVar], "WW/WZ/ZZ", "f" );

	  if(histo_WJets_herwig[iCut][iVar]->GetEntries()==0 && histo_WJets[iCut][iVar]->GetEntries()!=0 )  
             legNoRatio[iCut][iVar]->AddEntry( histo_WJets[iCut][iVar], "W+jets Pythia", "f" );
	  else if(histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && histo_WJets[iCut][iVar]->GetEntries()==0)  
             legNoRatio[iCut][iVar]->AddEntry( histo_WJets_herwig[iCut][iVar], "W+jets Herwig", "f" );
	  else if(histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && histo_WJets[iCut][iVar]->GetEntries()!=0){
                legNoRatio[iCut][iVar]->AddEntry( histo_WJets[iCut][iVar], "W+jets Pythia", "l" );
                legNoRatio[iCut][iVar]->AddEntry( histo_WJets_herwig[iCut][iVar], "W+jets Herwig", "l" );
	  }

	  if(histo_ttbar_herwig[iCut][iVar]->GetEntries()==0 && histo_ttbar[iCut][iVar]->GetEntries()!=0 )  
             legNoRatio[iCut][iVar]->AddEntry( histo_ttbar[iCut][iVar], "t#bar{t} powheg", "f" );
	  else if(histo_ttbar_herwig[iCut][iVar]->GetEntries()!=0 && histo_ttbar[iCut][iVar]->GetEntries()==0)  
             legNoRatio[iCut][iVar]->AddEntry( histo_ttbar_herwig[iCut][iVar], "t#bar{t} mc@nlo", "f" );
	  else if(histo_ttbar_herwig[iCut][iVar]->GetEntries()!=0 && histo_ttbar[iCut][iVar]->GetEntries()!=0){
                legNoRatio[iCut][iVar]->AddEntry( histo_ttbar[iCut][iVar], "t#bar{t} powheg", "l" );
                legNoRatio[iCut][iVar]->AddEntry( histo_ttbar_herwig[iCut][iVar], "t#bar{t} mc@nlo", "l" );
	  }
 
	  if(histo_top[iCut][iVar]->GetEntries()!=0)     { hs[iCut][iVar]->Add(histo_top[iCut][iVar]);   hs_herwig[iCut][iVar]->Add(histo_top[iCut][iVar]); }

          if(!isttbar_controlplots){

	   if(histo_ttbar[iCut][iVar]->GetEntries()!=0)        { hs[iCut][iVar]->Add(histo_ttbar[iCut][iVar]); hs_herwig[iCut][iVar]->Add(histo_ttbar[iCut][iVar]);}

	   if(histo_diboson[iCut][iVar]->GetEntries()!=0)      { hs[iCut][iVar]->Add(histo_diboson[iCut][iVar]);  hs_herwig[iCut][iVar]->Add(histo_diboson[iCut][iVar]); }

	   if(histo_WJets[iCut][iVar]->GetEntries()!=0)          hs[iCut][iVar]->Add(histo_WJets[iCut][iVar]);
	   if(histo_WJets_herwig[iCut][iVar]->GetEntries()!=0)   hs_herwig[iCut][iVar]->Add(histo_WJets_herwig[iCut][iVar]);
	  }
          else{

	   if(histo_WJets[iCut][iVar]->GetEntries()!=0)        { hs[iCut][iVar]->Add(histo_WJets[iCut][iVar]); hs_herwig[iCut][iVar]->Add(histo_WJets[iCut][iVar]);}

	   if(histo_diboson[iCut][iVar]->GetEntries()!=0)      { hs[iCut][iVar]->Add(histo_diboson[iCut][iVar]);  hs_herwig[iCut][iVar]->Add(histo_diboson[iCut][iVar]); }

  	   if(histo_ttbar[iCut][iVar]->GetEntries()!=0)        { hs[iCut][iVar]->Add(histo_ttbar[iCut][iVar]); }
	   if(histo_ttbar_herwig[iCut][iVar]->GetEntries()!=0) { hs_herwig[iCut][iVar]->Add(histo_ttbar_herwig[iCut][iVar]); }

	  }
          // Set the systenatic for each sample 
	  
          TH1F* MCSysStat = (TH1F*) histo_top[iCut][iVar]->Clone("MCSysStat");
          MCSysStat->SetLineColor(kBlack);
          MCSysStat->SetLineWidth(1);
          MCSysStat->SetFillColor(kBlack);
          MCSysStat->SetFillStyle(3013);
          
	  if(!isttbar_controlplots){
           SystematicErrorMap[0] = 0.0 ;   SystematicErrorMap_herwig[0] = 0.0 ;
	   SystematicErrorMap[1] = 0.3 ;   SystematicErrorMap_herwig[1] = 0.3 ;
           SystematicErrorMap[2] = 0.07 ;  SystematicErrorMap_herwig[2] = 0.07 ;
           SystematicErrorMap[3] = 0.0 ;   SystematicErrorMap_herwig[3] = 0.0 ;
           SystematicErrorMap[4] = 0.30 ;  SystematicErrorMap_herwig[4] = 0.30 ;
          }
          else{
           SystematicErrorMap[0] = 0.07 ;  SystematicErrorMap_herwig[0] = 0.07 ;
	   SystematicErrorMap[1] = 0.3 ;   SystematicErrorMap_herwig[1] = 0.3 ;
           SystematicErrorMap[2] = 0.3 ;   SystematicErrorMap_herwig[2] = 0.30 ;
           SystematicErrorMap[3] = 0.07 ;  SystematicErrorMap_herwig[3] = 0.07 ;
           SystematicErrorMap[4] = 0.0 ;   SystematicErrorMap_herwig[4] = 0.0 ;
	  }

	  std::vector<double> SysError ;
	  std::vector<double> SysError_herwig ;

	  SetTotalSystematicVector(SysError,hs[iCut][iVar],SystematicErrorMap);	  
	  SetTotalSystematicVector(SysError_herwig,hs_herwig[iCut][iVar],SystematicErrorMap_herwig);	  

          THStack* StackNoRatio           = (THStack*) hs[iCut][iVar]->Clone("StackNoRatio");
          THStack* StackNoRatio_herwig    = (THStack*) hs_herwig[iCut][iVar]->Clone("StackNoRatio_herwig");
          THStack* LogStack               = (THStack*) hs[iCut][iVar]->Clone("LogStack");
          THStack* LogStack_herwig        = (THStack*) hs_herwig[iCut][iVar]->Clone("LogStack_herwig");
          THStack* LogStackNoRatio           = (THStack*) hs[iCut][iVar]->Clone("StackNoRatio");
          THStack* LogStackNoRatio_herwig    = (THStack*) hs_herwig[iCut][iVar]->Clone("StackNoRatio_herwig");


	  c[iCut][iVar] ->cd();
	  upperPad->Draw();     
	  if(!WithoutData) lowerPad->Draw();

	  if(!WithoutData) {cNoRatio[iCut][iVar] ->cd(); upperPadNoRatio->Draw(); }

	  
	  if(WithoutData) { 

            upperPad->cd(); 

	    if(histo_WJets_herwig[iCut][iVar]->GetEntries()==0 && histo_WJets[iCut][iVar]->GetEntries()!=0 && !isttbar_controlplots) 
                 DrawStackError(hs[iCut][iVar],VariablesTitle.at(iVar),SystematicErrorMap,false,true); 
            else if (histo_WJets[iCut][iVar]->GetEntries()==0 && histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && !isttbar_controlplots)
                 DrawStackError(hs_herwig[iCut][iVar],VariablesTitle.at(iVar),SystematicErrorMap_herwig,false,true); 
	    else if (histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && histo_WJets[iCut][iVar]->GetEntries()!=0 && !isttbar_controlplots)
	         DrawDoubleStackError(hs[iCut][iVar],hs_herwig[iCut][iVar],VariablesTitle.at(iVar),SystematicErrorMap,SystematicErrorMap_herwig,false,true);                 
	    else if (histo_ttbar_herwig[iCut][iVar]->GetEntries()!=0 && histo_ttbar[iCut][iVar]->GetEntries()!=0 && isttbar_controlplots)
	         DrawDoubleStackError(hs[iCut][iVar],hs_herwig[iCut][iVar],VariablesTitle.at(iVar),SystematicErrorMap,SystematicErrorMap_herwig,false,true,isttbar_controlplots);           
          }
                         
	  else{ 
	       upperPad->cd();  

               if(histo_WJets_herwig[iCut][iVar]->GetEntries()==0 && histo_WJets[iCut][iVar]->GetEntries()!=0 && !isttbar_controlplots) 
                  DrawStackError(hs[iCut][iVar],VariablesTitle.at(iVar),histos[iCut][iVar][iSampleData],SystematicErrorMap,false,false);
               else if (histo_WJets[iCut][iVar]->GetEntries()==0 && histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && !isttbar_controlplots)
                  DrawStackError(hs_herwig[iCut][iVar],VariablesTitle.at(iVar),histos[iCut][iVar][iSampleData],SystematicErrorMap_herwig,false,false);
               else if (histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && histo_WJets[iCut][iVar]->GetEntries()!=0 && !isttbar_controlplots)  
 		  DrawDoubleStackError(hs[iCut][iVar],hs_herwig[iCut][iVar],VariablesTitle.at(iVar),histos[iCut][iVar][iSampleData],SystematicErrorMap,SystematicErrorMap_herwig,false,false);
		 else if (histo_ttbar_herwig[iCut][iVar]->GetEntries()!=0 && histo_ttbar[iCut][iVar]->GetEntries()!=0 && isttbar_controlplots)
	          DrawDoubleStackError(hs[iCut][iVar],hs_herwig[iCut][iVar],VariablesTitle.at(iVar),SystematicErrorMap,SystematicErrorMap_herwig,false,false,isttbar_controlplots);          

  	       upperPadNoRatio->cd();

	       if(histo_WJets_herwig[iCut][iVar]->GetEntries()==0 && histo_WJets[iCut][iVar]->GetEntries()!=0 && !isttbar_controlplots) 
                  DrawStackError(StackNoRatio,VariablesTitle.at(iVar),histos[iCut][iVar][iSampleData],SystematicErrorMap,false,true);
               else if (histo_WJets[iCut][iVar]->GetEntries()==0 && histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && !isttbar_controlplots)
                  DrawStackError(StackNoRatio_herwig,VariablesTitle.at(iVar),histos[iCut][iVar][iSampleData],SystematicErrorMap,false,true);
               else if(histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && histo_WJets[iCut][iVar]->GetEntries()!=0 && !isttbar_controlplots)
 	 	  DrawDoubleStackError(StackNoRatio,StackNoRatio_herwig,VariablesTitle.at(iVar),histos[iCut][iVar][iSampleData],SystematicErrorMap,SystematicErrorMap_herwig,false,true);
    	       else if (histo_ttbar_herwig[iCut][iVar]->GetEntries()!=0 && histo_ttbar[iCut][iVar]->GetEntries()!=0 && isttbar_controlplots)
	          DrawDoubleStackError(StackNoRatio,StackNoRatio_herwig,VariablesTitle.at(iVar),SystematicErrorMap,SystematicErrorMap_herwig,false,true,isttbar_controlplots);            

            if((VariablesBlindedMinValue.at(iVar) != -999. && VariablesBlindedMaxValue.at(iVar) != -999.) && VariablesBlindedMinValue.at(iVar) != VariablesBlindedMaxValue.at(iVar)){
	     
                  for(int iBin = histos[iCut][iVar][iSampleData]->FindBin(VariablesBlindedMinValue.at(iVar)) ; 
                      iBin< histos[iCut][iVar][iSampleData]->FindBin(VariablesBlindedMaxValue.at(iVar)) ; iBin++) histos[iCut][iVar][iSampleData]->SetBinContent(iBin,-1.);

		  upperPad->cd(); histos[iCut][iVar][iSampleData]->DrawCopy("E same"); histos[iCut][iVar][iSampleData]->DrawCopy("E axissame");
		  upperPadNoRatio->cd(); histos[iCut][iVar][iSampleData]->DrawCopy("E same");  histos[iCut][iVar][iSampleData]->DrawCopy("E axissame");
                
            }
	    else{
                  upperPad->cd(); histos[iCut][iVar][iSampleData]->DrawCopy("E same"); histos[iCut][iVar][iSampleData]->DrawCopy("E axissame");  
                  upperPadNoRatio->cd(); histos[iCut][iVar][iSampleData]->DrawCopy("E same"); histos[iCut][iVar][iSampleData]->DrawCopy("E axissame");  }
          }

	  upperPad->cd(); leg[iCut][iVar]->AddEntry(MCSysStat,"MC Stat + Sys","f");
	  upperPadNoRatio->cd(); legNoRatio[iCut][iVar]->AddEntry(MCSysStat,"MC Stat + Sys","f"); 
	  
	  if(SignalggHName!="NULL" && iSampleggH!=-1){ 
              
              	    TString Name ;
	            if(!NormalizeSignalToData)  Name = Form("%s x %d",NameReducedSample.at(iSampleggH).c_str(),int(SignalScaleFactor));
	            else                        Name = Form("%s",NameReducedSample.at(iSampleggH).c_str());

                    Name = Form("ggH x %d, 600GeV",int(SignalScaleFactor));

                    histos[iCut][iVar][iSampleggH]->SetLineWidth(3);
                    histos[iCut][iVar][iSampleggH]->SetFillStyle(0);

                    leg[iCut][iVar]->AddEntry( histos[iCut][iVar][iSampleggH], Name.Data(), "l" );
                    legNoRatio[iCut][iVar]->AddEntry( histos[iCut][iVar][iSampleggH], Name.Data(), "l" );
                                     
                    if(!NormalizeSignalToData) histos[iCut][iVar][iSampleggH]->Scale(SignalScaleFactor*1.);

		    upperPad->cd(); histos[iCut][iVar][iSampleggH]->DrawCopy("hist same");  histos[iCut][iVar][iSampleggH]->DrawCopy("hist axissame");
		    if(!WithoutData) { upperPadNoRatio->cd(); histos[iCut][iVar][iSampleggH]->DrawCopy("hist same");histos[iCut][iVar][iSampleggH]->DrawCopy("hist axissame"); }
          }
       
          if(SignalqqHName!="NULL" && iSampleqqH!=-1){ 

              	    TString Name ;
	            if(!NormalizeSignalToData)  Name = Form("%s x %d",NameReducedSample.at(iSampleqqH).c_str(),int(SignalScaleFactor));
	            else                        Name = Form("%s",NameReducedSample.at(iSampleqqH).c_str());


                    Name = Form("qqH x %d, 600GeV",int(SignalScaleFactor));

                    histos[iCut][iVar][iSampleqqH]->SetLineWidth(3);
                    histos[iCut][iVar][iSampleqqH]->SetFillStyle(0);

               	    leg[iCut][iVar]->AddEntry( histos[iCut][iVar][iSampleqqH], Name.Data(), "l" );
               	    legNoRatio[iCut][iVar]->AddEntry( histos[iCut][iVar][iSampleqqH], Name.Data(), "l" );
 
          	    if(!NormalizeSignalToData) histos[iCut][iVar][iSampleqqH]->Scale(SignalScaleFactor*1.);

		    upperPad->cd(); histos[iCut][iVar][iSampleqqH]->DrawCopy("hist same"); histos[iCut][iVar][iSampleqqH]->DrawCopy("hist axissame");
		    if(!WithoutData) { upperPadNoRatio->cd(); histos[iCut][iVar][iSampleqqH]->DrawCopy("hist same"); histos[iCut][iVar][iSampleqqH]->DrawCopy("hist axissame"); }

          }

	  if(SignalRSGPythiaName!="NULL" && iSampleRSGPythia!=-1){ 

              	    TString Name ;
	            if(!NormalizeSignalToData)  Name = Form("%s x %d",NameReducedSample.at(iSampleRSGPythia).c_str(),int(SignalScaleFactor));
	            else                        Name = Form("%s",NameReducedSample.at(iSampleRSGPythia).c_str());

               	    leg[iCut][iVar]->AddEntry( histos[iCut][iVar][iSampleRSGPythia], Name.Data(), "l" );
               	    legNoRatio[iCut][iVar]->AddEntry( histos[iCut][iVar][iSampleRSGPythia], Name.Data(), "l" );

                    histos[iCut][iVar][iSampleRSGPythia]->SetLineWidth(3);
                    histos[iCut][iVar][iSampleRSGPythia]->SetFillStyle(0);
                                     
                    if(!NormalizeSignalToData) histos[iCut][iVar][iSampleRSGPythia]->Scale(SignalScaleFactor*1.);

		    upperPad->cd(); histos[iCut][iVar][iSampleRSGPythia]->DrawCopy("hist same"); histos[iCut][iVar][iSampleRSGPythia]->DrawCopy("hist axissame");
		    if(!WithoutData) { upperPadNoRatio->cd(); histos[iCut][iVar][iSampleRSGPythia]->DrawCopy("hist same"); histos[iCut][iVar][iSampleRSGPythia]->DrawCopy("hist axissame"); }
          }
       
	  if(SignalRSGHerwigName!="NULL" && iSampleRSGHerwig!=-1){ 

              	    TString Name ;
	            if(!NormalizeSignalToData)  Name = Form("%s x %d",NameReducedSample.at(iSampleRSGHerwig).c_str(),int(SignalScaleFactor));
	            else                        Name = Form("%s",NameReducedSample.at(iSampleRSGHerwig).c_str());
 
               	    leg[iCut][iVar]->AddEntry( histos[iCut][iVar][iSampleRSGHerwig], Name.Data(), "l" );
               	    legNoRatio[iCut][iVar]->AddEntry( histos[iCut][iVar][iSampleRSGHerwig], Name.Data(), "l" );
                                     
                    histos[iCut][iVar][iSampleRSGHerwig]->SetLineWidth(3);
                    histos[iCut][iVar][iSampleRSGHerwig]->SetFillStyle(0);

                    if(!NormalizeSignalToData) histos[iCut][iVar][iSampleRSGHerwig]->Scale(SignalScaleFactor*1.);

		    upperPad->cd(); histos[iCut][iVar][iSampleRSGHerwig]->DrawCopy("hist same"); histos[iCut][iVar][iSampleRSGHerwig]->DrawCopy("hist axissame");
		    if(!WithoutData) { upperPadNoRatio->cd(); histos[iCut][iVar][iSampleRSGHerwig]->DrawCopy("hist same"); histos[iCut][iVar][iSampleRSGHerwig]->DrawCopy("hist axissame"); }
          }

	  if(SignalGravitonName!="NULL" && iSampleGraviton!=-1){ 

             	    TString Name ;
	            if(!NormalizeSignalToData)  Name = Form("%s x %d",NameReducedSample.at(iSampleGraviton).c_str(),int(SignalScaleFactor));
	            else                        Name = Form("%s",NameReducedSample.at(iSampleGraviton).c_str());
 
              	    leg[iCut][iVar]->AddEntry( histos[iCut][iVar][iSampleGraviton], Name.Data(), "l" );
              	    legNoRatio[iCut][iVar]->AddEntry( histos[iCut][iVar][iSampleGraviton], Name.Data(), "l" );
                                     
                    histos[iCut][iVar][iSampleGraviton]->SetLineWidth(3);
                    histos[iCut][iVar][iSampleGraviton]->SetFillStyle(0);

                    if(!NormalizeSignalToData) histos[iCut][iVar][iSampleGraviton]->Scale(SignalScaleFactor*1.);

		    upperPad->cd(); histos[iCut][iVar][iSampleGraviton]->Draw("hist same"); histos[iCut][iVar][iSampleGraviton]->Draw("hist axissame");
		    if(!WithoutData) { upperPadNoRatio->cd(); histos[iCut][iVar][iSampleGraviton]->Draw("hist axissame"); }

          }
          
	  
	  if(!WithoutData) { upperPad->cd(); leg[iCut][iVar]->Draw("same"); LatexCMS(Lumi,LeptonType,false); banner4Plot(false);
                 	     upperPadNoRatio->cd(); legNoRatio[iCut][iVar]->Draw("same"); LatexCMS(Lumi,LeptonType,true); banner4Plot(true); }

          else { upperPad->cd(); leg[iCut][iVar]->Draw("same"); LatexCMS(Lumi,LeptonType,true); banner4Plot(true); }

	  if(!WithoutData) {
         
                            lowerPad->cd();
	                    lowerPad->SetGridx();
	                    lowerPad->SetGridy();
	  
                            RatioDataMC[iCut][iVar] = (TH1F*) histos[iCut][iVar][iSampleData]->Clone(("RatioDataMC-"+Variables.at(iVar)+"-"+CutList.at(iCut)).c_str()) ;
                            RatioDataMC[iCut][iVar]->Divide(histoSum[iCut][iVar]);

                            RatioDataMC_herwig[iCut][iVar] = (TH1F*) histos[iCut][iVar][iSampleData]->Clone(("RatioDataMC-"+Variables.at(iVar)+"-"+CutList.at(iCut)).c_str()) ;
                            RatioDataMC_herwig[iCut][iVar]->Divide(histoSum_herwig[iCut][iVar]);

                            RatioDataMC_error[iCut][iVar] = (TH1F*) histos[iCut][iVar][iSampleData]->Clone(("RatioDataMC-"+Variables.at(iVar)+"-"+CutList.at(iCut)).c_str()) ;
                            RatioDataMC_error[iCut][iVar]->Divide(histoSum_herwig[iCut][iVar]);
                          
                            RatioDataMC[iCut][iVar]->SetMinimum(0.001);
                            RatioDataMC[iCut][iVar]->SetMaximum(2.);
			
			    RatioDataMC[iCut][iVar]->GetYaxis()->SetTitle("Data / Sim");
			    RatioDataMC[iCut][iVar]->GetYaxis()->SetTitleSize(0.105);
			    RatioDataMC[iCut][iVar]->GetYaxis()->SetLabelSize(0.100);
			    RatioDataMC[iCut][iVar]->GetYaxis()->SetTitleOffset(0.48);

      	                    RatioDataMC[iCut][iVar]->GetXaxis()->SetTitle(VariablesTitle.at(iVar).c_str());	
                            RatioDataMC[iCut][iVar]->GetXaxis()->SetLabelSize(0.105);
                            RatioDataMC[iCut][iVar]->GetXaxis()->SetTitleSize(0.15);
			    RatioDataMC[iCut][iVar]->GetXaxis()->SetTitleOffset(0.75);

                            RatioDataMC_herwig[iCut][iVar]->SetMinimum(0.001);
                            RatioDataMC_herwig[iCut][iVar]->SetMaximum(2.);
			
			    RatioDataMC_herwig[iCut][iVar]->GetYaxis()->SetTitle("Data / Sim");
			    RatioDataMC_herwig[iCut][iVar]->GetYaxis()->SetTitleSize(0.105);
			    RatioDataMC_herwig[iCut][iVar]->GetYaxis()->SetLabelSize(0.100);
			    RatioDataMC_herwig[iCut][iVar]->GetYaxis()->SetTitleOffset(0.48);

      	                    RatioDataMC_herwig[iCut][iVar]->GetXaxis()->SetTitle(VariablesTitle.at(iVar).c_str());	
                            RatioDataMC_herwig[iCut][iVar]->GetXaxis()->SetLabelSize(0.105);
                            RatioDataMC_herwig[iCut][iVar]->GetXaxis()->SetTitleSize(0.15);
			    RatioDataMC_herwig[iCut][iVar]->GetXaxis()->SetTitleOffset(0.75);

                            RatioDataMC_error[iCut][iVar]->SetMinimum(0.001);
                            RatioDataMC_error[iCut][iVar]->SetMaximum(2.);
			
			    RatioDataMC_error[iCut][iVar]->GetYaxis()->SetTitle("Data / Sim");
			    RatioDataMC_error[iCut][iVar]->GetYaxis()->SetTitleSize(0.105);
			    RatioDataMC_error[iCut][iVar]->GetYaxis()->SetLabelSize(0.100);
			    RatioDataMC_error[iCut][iVar]->GetYaxis()->SetTitleOffset(0.48);

      	                    RatioDataMC_error[iCut][iVar]->GetXaxis()->SetTitle(VariablesTitle.at(iVar).c_str());	
                            RatioDataMC_error[iCut][iVar]->GetXaxis()->SetLabelSize(0.105);
                            RatioDataMC_error[iCut][iVar]->GetXaxis()->SetTitleSize(0.15);
			    RatioDataMC_error[iCut][iVar]->GetXaxis()->SetTitleOffset(0.75);

                            // Propagate Sys Error to the ratio
                            if( (histo_WJets_herwig[iCut][iVar]->GetEntries()==0 && histo_WJets[iCut][iVar]->GetEntries()!=0) &&  !isttbar_controlplots){
			     for( int iBin = 0; iBin< RatioDataMC[iCut][iVar]->GetNbinsX() ; iBin++){
			      RatioDataMC[iCut][iVar]->SetBinError(iBin+1,sqrt(RatioDataMC[iCut][iVar]->GetBinError(iBin+1)*RatioDataMC[iCut][iVar]->GetBinError(iBin+1)+
			      SysError.at(iBin) * (TMath::Power(histos[iCut][iVar][iSampleData]->GetBinContent(iBin+1),2))/(TMath::Power(histoSum[iCut][iVar]->GetBinContent(iBin+1),4)))) ;

			     }
                             RatioDataMC[iCut][iVar]->GetYaxis()->SetNdivisions(504);
                             RatioDataMC[iCut][iVar]->Draw("PE");         
                            
                             RatioLine->SetRange((RatioDataMC[iCut][iVar]->GetBinCenter(1)-RatioDataMC[iCut][iVar]->GetBinWidth(1)/2),
			     (RatioDataMC[iCut][iVar]->GetBinCenter(RatioDataMC[iCut][iVar]->GetNbinsX()+1)-RatioDataMC[iCut][iVar]->GetBinWidth(RatioDataMC[iCut][iVar]->GetNbinsX()+1)/2));
			     RatioLine->SetLineWidth(2);
                             RatioLine->Draw("same");
	                     RatioDataMC[iCut][iVar]->Draw("PEsame");
			    }

                            else if((histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && histo_WJets[iCut][iVar]->GetEntries()==0) && !isttbar_controlplots){

			     for( int iBin = 0; iBin< RatioDataMC_herwig[iCut][iVar]->GetNbinsX() ; iBin++){
 			     RatioDataMC_herwig[iCut][iVar]->SetBinError(iBin+1,sqrt(RatioDataMC_herwig[iCut][iVar]->GetBinError(iBin+1)*RatioDataMC_herwig[iCut][iVar]->GetBinError(iBin+1)+
			     SysError_herwig.at(iBin) * (TMath::Power(histos[iCut][iVar][iSampleData]->GetBinContent(iBin+1),2))/(TMath::Power(histoSum_herwig[iCut][iVar]->GetBinContent(iBin+1),4)))) ;
			    }
                            RatioDataMC_herwig[iCut][iVar]->GetYaxis()->SetNdivisions(504);
	                    RatioDataMC_herwig[iCut][iVar]->Draw("PE");

                            RatioLine->SetRange((RatioDataMC_herwig[iCut][iVar]->GetBinCenter(1)-RatioDataMC_herwig[iCut][iVar]->GetBinWidth(1)/2),
			    (RatioDataMC_herwig[iCut][iVar]->GetBinCenter(RatioDataMC_herwig[iCut][iVar]->GetNbinsX()+1)-RatioDataMC_herwig[iCut][iVar]->GetBinWidth(RatioDataMC_herwig[iCut][iVar]->GetNbinsX()+1)/2));
			    RatioLine->SetLineWidth(2);
			    RatioLine->SetLineColor(2);
                            RatioLine->Draw("same");
	                    RatioDataMC_herwig[iCut][iVar]->Draw("PEsame");

			    }
                   
                            else if ((histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && histo_WJets[iCut][iVar]->GetEntries()!=0) && !isttbar_controlplots){

			     for( int iBin = 0; iBin< RatioDataMC[iCut][iVar]->GetNbinsX() ; iBin++){
			      RatioDataMC[iCut][iVar]->SetBinError(iBin+1,sqrt(RatioDataMC[iCut][iVar]->GetBinError(iBin+1)*RatioDataMC[iCut][iVar]->GetBinError(iBin+1)+
			      SysError.at(iBin) * (TMath::Power(histos[iCut][iVar][iSampleData]->GetBinContent(iBin+1),2))/(TMath::Power(histoSum[iCut][iVar]->GetBinContent(iBin+1),4)))) ;
	                      RatioDataMC_error[iCut][iVar]->SetBinContent(iBin+1,1) ;
			      RatioDataMC_error[iCut][iVar]->SetBinError(iBin+1,sqrt(RatioDataMC[iCut][iVar]->GetBinError(iBin+1)*RatioDataMC[iCut][iVar]->GetBinError(iBin+1)+
			      SysError.at(iBin) * (TMath::Power(histos[iCut][iVar][iSampleData]->GetBinContent(iBin+1),2))/(TMath::Power(histoSum[iCut][iVar]->GetBinContent(iBin+1),4)))) ;

			     }

                             RatioDataMC_error[iCut][iVar]->SetBinContent(0,1) ;
                             RatioDataMC_error[iCut][iVar]->SetBinContent(RatioDataMC[iCut][iVar]->GetNbinsX()+1,1) ;
                             RatioDataMC_error[iCut][iVar]->SetBinError(0,RatioDataMC[iCut][iVar]->GetBinError(1)) ;
                             RatioDataMC_error[iCut][iVar]->SetBinError(RatioDataMC[iCut][iVar]->GetNbinsX()+1,RatioDataMC[iCut][iVar]->GetBinError((RatioDataMC[iCut][iVar]->GetNbinsX()))) ;

                             RatioDataMC[iCut][iVar]->GetYaxis()->SetNdivisions(504);

                             RatioDataMC_error[iCut][iVar]->GetYaxis()->SetNdivisions(504);
                             RatioDataMC_error[iCut][iVar]->SetMarkerColor(12);
                             RatioDataMC_error[iCut][iVar]->SetLineColor(1);
                             RatioDataMC_error[iCut][iVar]->SetFillColor(12);         
                             RatioDataMC_error[iCut][iVar]->SetLineWidth(1);         
                             RatioDataMC_error[iCut][iVar]->SetMarkerSize(0);         
                             RatioDataMC_error[iCut][iVar]->SetFillStyle(3002);         
                             RatioDataMC[iCut][iVar]->Draw("hist");         
                             RatioDataMC_error[iCut][iVar]->SetBarWidth(1.2); 
	                     RatioDataMC_error[iCut][iVar]->Draw("b same");

                             RatioDataMC[iCut][iVar]->SetMarkerColor(2);
                             RatioDataMC[iCut][iVar]->SetFillColor(0);         
                             RatioDataMC[iCut][iVar]->SetLineWidth(2);         
                             RatioDataMC[iCut][iVar]->SetLineColor(2);         

                             RatioLine->SetRange((RatioDataMC[iCut][iVar]->GetBinCenter(1)-RatioDataMC[iCut][iVar]->GetBinWidth(1)/2),
			     (RatioDataMC[iCut][iVar]->GetBinCenter(RatioDataMC[iCut][iVar]->GetNbinsX()+1)-RatioDataMC[iCut][iVar]->GetBinWidth(RatioDataMC[iCut][iVar]->GetNbinsX()+1)/2));
			     RatioLine->SetLineWidth(2);
			     RatioLine->SetLineColor(1);
                             RatioLine->Draw("same");

                             RatioDataMC[iCut][iVar]->Draw("hist same");         
                            
			    

       		           for( int iBin = 0; iBin< RatioDataMC_herwig[iCut][iVar]->GetNbinsX() ; iBin++){
 			     RatioDataMC_herwig[iCut][iVar]->SetBinError(iBin+1,sqrt(RatioDataMC_herwig[iCut][iVar]->GetBinError(iBin+1)*RatioDataMC_herwig[iCut][iVar]->GetBinError(iBin+1)+
			     SysError_herwig.at(iBin) * (TMath::Power(histos[iCut][iVar][iSampleData]->GetBinContent(iBin+1),2))/(TMath::Power(histoSum_herwig[iCut][iVar]->GetBinContent(iBin+1),4)))) ;

			    }
                            RatioDataMC_herwig[iCut][iVar]->GetYaxis()->SetNdivisions(504);
                            RatioDataMC_herwig[iCut][iVar]->SetMarkerColor(4);
                            RatioDataMC_herwig[iCut][iVar]->SetLineColor(4);         
                            RatioDataMC_herwig[iCut][iVar]->SetFillColor(0);         
                            RatioDataMC_herwig[iCut][iVar]->SetLineWidth(2);         
	                    RatioDataMC_herwig[iCut][iVar]->Draw("hist same");
                           
			    /*legRatio->SetFillColor(0);
			    legRatio->SetFillStyle(3001);
			    legRatio->SetNColumns(2);
                            legRatio->AddEntry(RatioDataMC[iCut][iVar],"Data/MC Pythia","l");
                            legRatio->AddEntry(RatioDataMC_herwig[iCut][iVar],"Data/MC Herwig","l");
                            legRatio->Draw("same");*/
	                    lowerPad->Update();
			    }
			    /////////////////////////////////////////////////////////////

                   
                            else if ((histo_ttbar_herwig[iCut][iVar]->GetEntries()!=0 && histo_ttbar[iCut][iVar]->GetEntries()!=0) && isttbar_controlplots){

			     for( int iBin = 0; iBin< RatioDataMC[iCut][iVar]->GetNbinsX() ; iBin++){
			      RatioDataMC[iCut][iVar]->SetBinError(iBin+1,sqrt(RatioDataMC[iCut][iVar]->GetBinError(iBin+1)*RatioDataMC[iCut][iVar]->GetBinError(iBin+1)+
			      SysError.at(iBin) * (TMath::Power(histos[iCut][iVar][iSampleData]->GetBinContent(iBin+1),2))/(TMath::Power(histoSum[iCut][iVar]->GetBinContent(iBin+1),4)))) ;
	                      RatioDataMC_error[iCut][iVar]->SetBinContent(iBin+1,1) ;
			      RatioDataMC_error[iCut][iVar]->SetBinError(iBin+1,sqrt(RatioDataMC[iCut][iVar]->GetBinError(iBin+1)*RatioDataMC[iCut][iVar]->GetBinError(iBin+1)+
			      SysError.at(iBin) * (TMath::Power(histos[iCut][iVar][iSampleData]->GetBinContent(iBin+1),2))/(TMath::Power(histoSum[iCut][iVar]->GetBinContent(iBin+1),4)))) ;

			     }

                             RatioDataMC_error[iCut][iVar]->SetBinContent(0,1) ;
                             RatioDataMC_error[iCut][iVar]->SetBinContent(RatioDataMC[iCut][iVar]->GetNbinsX()+1,1) ;
                             RatioDataMC_error[iCut][iVar]->SetBinError(0,RatioDataMC[iCut][iVar]->GetBinError(1)) ;
                             RatioDataMC_error[iCut][iVar]->SetBinError(RatioDataMC[iCut][iVar]->GetNbinsX()+1,RatioDataMC[iCut][iVar]->GetBinError((RatioDataMC[iCut][iVar]->GetNbinsX()))) ;

                             RatioDataMC[iCut][iVar]->GetYaxis()->SetNdivisions(504);

                             RatioDataMC_error[iCut][iVar]->GetYaxis()->SetNdivisions(504);
                             RatioDataMC_error[iCut][iVar]->SetMarkerColor(12);
                             RatioDataMC_error[iCut][iVar]->SetLineColor(1);
                             RatioDataMC_error[iCut][iVar]->SetFillColor(12);         
                             RatioDataMC_error[iCut][iVar]->SetLineWidth(1);         
                             RatioDataMC_error[iCut][iVar]->SetMarkerSize(0);         
                             RatioDataMC_error[iCut][iVar]->SetFillStyle(3002);         
                             RatioDataMC[iCut][iVar]->Draw("hist");         
                             RatioDataMC_error[iCut][iVar]->SetBarWidth(1.2); 
	                     RatioDataMC_error[iCut][iVar]->Draw("b same");

                             RatioDataMC[iCut][iVar]->SetMarkerColor(2);
                             RatioDataMC[iCut][iVar]->SetFillColor(0);         
                             RatioDataMC[iCut][iVar]->SetLineWidth(2);         
                             RatioDataMC[iCut][iVar]->SetLineColor(210);

                             RatioLine->SetRange((RatioDataMC[iCut][iVar]->GetBinCenter(1)-RatioDataMC[iCut][iVar]->GetBinWidth(1)/2),
			     (RatioDataMC[iCut][iVar]->GetBinCenter(RatioDataMC[iCut][iVar]->GetNbinsX()+1)-RatioDataMC[iCut][iVar]->GetBinWidth(RatioDataMC[iCut][iVar]->GetNbinsX()+1)/2));
			     RatioLine->SetLineWidth(2);
			     RatioLine->SetLineColor(1);
                             RatioLine->Draw("same");

                             RatioDataMC[iCut][iVar]->Draw("hist same");         
                            
			    

       		           for( int iBin = 0; iBin< RatioDataMC_herwig[iCut][iVar]->GetNbinsX() ; iBin++){
 			     RatioDataMC_herwig[iCut][iVar]->SetBinError(iBin+1,sqrt(RatioDataMC_herwig[iCut][iVar]->GetBinError(iBin+1)*RatioDataMC_herwig[iCut][iVar]->GetBinError(iBin+1)+
			     SysError_herwig.at(iBin) * (TMath::Power(histos[iCut][iVar][iSampleData]->GetBinContent(iBin+1),2))/(TMath::Power(histoSum_herwig[iCut][iVar]->GetBinContent(iBin+1),4)))) ;

			    }
                            RatioDataMC_herwig[iCut][iVar]->GetYaxis()->SetNdivisions(504);
                            RatioDataMC_herwig[iCut][iVar]->SetMarkerColor(4);
                            RatioDataMC_herwig[iCut][iVar]->SetLineColor(53);
                            RatioDataMC_herwig[iCut][iVar]->SetFillColor(0);         
                            RatioDataMC_herwig[iCut][iVar]->SetLineWidth(2);         
	                    RatioDataMC_herwig[iCut][iVar]->Draw("hist same");
                           
			    /*legRatio->SetFillColor(0);
			    legRatio->SetFillStyle(3001);
			    legRatio->SetNColumns(2);
                            legRatio->AddEntry(RatioDataMC[iCut][iVar],"Data/MC Pythia","l");
                            legRatio->AddEntry(RatioDataMC_herwig[iCut][iVar],"Data/MC Herwig","l");
                            legRatio->Draw("same");
	                    lowerPad->Update();*/
			    }


	  }

	  upperPad->Update();     	  
	  c[iCut][iVar]->Update();
	  c[iCut][iVar]->Write();
          
	  std::string canvasname = CanvasName.Data() ;
	  std::replace(canvasname.begin(),canvasname.end(),'[','_');
	  std::replace(canvasname.begin(),canvasname.end(),']','_');
	  std::replace(canvasname.begin(),canvasname.end(),'(','_');
	  std::replace(canvasname.begin(),canvasname.end(),')','_');

	  c[iCut][iVar]->Print( (OutputPlotDirectory+"/"+canvasname+".pdf").c_str(),"pdf");
	  c[iCut][iVar]->Print( (OutputPlotDirectory+"/"+canvasname+".png").c_str(),"png");
	  //	  c[iCut][iVar]->Print( (OutputPlotDirectory+"/"+canvasname+".root").c_str(),"root");
	  //	  c[iCut][iVar]->Print( (OutputPlotDirectory+"/"+canvasname+".cxx").c_str(),"cxx");
	  
	  c[iCut][iVar]->Close();
	  
	  if(!WithoutData) {
            
            upperPadNoRatio->Update();
            cNoRatio[iCut][iVar]->Update();
            cNoRatio[iCut][iVar]->Write();

	    std::string canvasnameNoRatio = CanvasNameNoRatio.Data() ;
	    std::replace(canvasnameNoRatio.begin(),canvasnameNoRatio.end(),'[','_');
	    std::replace(canvasnameNoRatio.begin(),canvasnameNoRatio.end(),']','_');
	    std::replace(canvasnameNoRatio.begin(),canvasnameNoRatio.end(),'(','_');
 	    std::replace(canvasnameNoRatio.begin(),canvasnameNoRatio.end(),')','_');
	            
	    cNoRatio[iCut][iVar]->Print( (OutputPlotDirectory+"/"+canvasnameNoRatio+".pdf").c_str(),"pdf");
	    cNoRatio[iCut][iVar]->Print( (OutputPlotDirectory+"/"+canvasnameNoRatio+".png").c_str(),"png");
	    //	    cNoRatio[iCut][iVar]->Print( (OutputPlotDirectory+"/"+canvasnameNoRatio+".root").c_str(),"root");
	    //	    cNoRatio[iCut][iVar]->Print( (OutputPlotDirectory+"/"+canvasnameNoRatio+".cxx").c_str(),"cxx");
	    
	    cNoRatio[iCut][iVar]->Close();
	  }
      
	  if(!WithoutData){  lowerPadLog->cd();
                             lowerPadLog->SetGridx();
	                     lowerPadLog->SetGridy();

                             if(histo_WJets[iCut][iVar]->GetEntries()!=0 && histo_WJets_herwig[iCut][iVar]->GetEntries()==0 && !isttbar_controlplots){

                              RatioDataMC[iCut][iVar]->Draw("PE");
                              RatioLine->Draw("same");
                              RatioDataMC[iCut][iVar]->Draw("PEsame");
			     }
                             else if (histo_WJets[iCut][iVar]->GetEntries()==0 && histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 &&  !isttbar_controlplots){

                              RatioDataMC_herwig[iCut][iVar]->Draw("PE");
                              RatioLine->Draw("same");
                              RatioDataMC_herwig[iCut][iVar]->Draw("PEsame");

			     }

                             else if(histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && histo_WJets[iCut][iVar]->GetEntries()!=0 &&  !isttbar_controlplots){

                              RatioDataMC_error[iCut][iVar]->SetBarWidth(1.2); 
                              RatioDataMC_error[iCut][iVar]->Draw("b");
                              RatioLine->Draw("same");
                              RatioDataMC[iCut][iVar]->Draw("hist same");

                              RatioDataMC_herwig[iCut][iVar]->Draw("hist same");
                              /*legRatio->Clear();
			      legRatio->SetFillColor(0);
			      legRatio->SetFillStyle(3001);
			      legRatio->SetNColumns(2);
                              legRatio->AddEntry(RatioDataMC[iCut][iVar],"Data/MC Pythia","l");
                              legRatio->AddEntry(RatioDataMC_herwig[iCut][iVar],"Data/MC Herwig","l");
                              legRatio->Draw("same");*/

			     }

                             else if(histo_ttbar_herwig[iCut][iVar]->GetEntries()!=0 && histo_ttbar[iCut][iVar]->GetEntries()!=0 &&  isttbar_controlplots){

                              RatioDataMC_error[iCut][iVar]->SetBarWidth(1.2); 
                              RatioDataMC_error[iCut][iVar]->Draw("b");
                              RatioLine->Draw("same");
                              RatioDataMC[iCut][iVar]->Draw("hist same");
                              RatioDataMC_herwig[iCut][iVar]->Draw("hist same");
                              /*legRatio->Clear();
			      legRatio->SetFillColor(0);
			      legRatio->SetFillStyle(3001);
			      legRatio->SetNColumns(2);
                              legRatio->AddEntry(RatioDataMC[iCut][iVar],"Data/MC Pythia","l");
                              legRatio->AddEntry(RatioDataMC_herwig[iCut][iVar],"Data/MC Herwig","l");
                              legRatio->Draw("same");*/

			     }

          }

	  cLog[iCut][iVar] ->cd();
	  upperPadLog->Draw();     

 	  if(!WithoutData) lowerPadLog->Draw();

	  if(!WithoutData) {cLogNoRatio[iCut][iVar] ->cd();
	                    upperPadLogNoRatio->Draw();}
          	  
	  if(WithoutData){ 

            upperPadLog->cd();

	    if(histo_WJets_herwig[iCut][iVar]->GetEntries()==0 && histo_WJets[iCut][iVar]->GetEntries()!=0 && !isttbar_controlplots) 
               DrawStackError(LogStack,VariablesTitle.at(iVar),SystematicErrorMap,true,false);

            else if (histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && histo_WJets[iCut][iVar]->GetEntries()==0 && !isttbar_controlplots)
               DrawStackError(LogStack_herwig,VariablesTitle.at(iVar),SystematicErrorMap_herwig,true,false);

            else if (histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && histo_WJets[iCut][iVar]->GetEntries()!=0 && !isttbar_controlplots)
 	       DrawDoubleStackError(LogStack,LogStack_herwig,VariablesTitle.at(iVar),SystematicErrorMap,SystematicErrorMap_herwig,true,false);

    	    else if (histo_ttbar_herwig[iCut][iVar]->GetEntries()!=0 && histo_ttbar[iCut][iVar]->GetEntries()!=0 && isttbar_controlplots)
	       DrawDoubleStackError(LogStack,LogStack_herwig,VariablesTitle.at(iVar),SystematicErrorMap,SystematicErrorMap_herwig,true,false,isttbar_controlplots);                 

          }
	  else{ 

	    upperPadLog->cd(); 

            if(histo_WJets_herwig[iCut][iVar]->GetEntries()==0 && histo_WJets[iCut][iVar]->GetEntries()!=0 && !isttbar_controlplots) 
              DrawStackError(LogStack,VariablesTitle.at(iVar),histos[iCut][iVar][iSampleData],SystematicErrorMap,true,false);
            else if (histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && histo_WJets[iCut][iVar]->GetEntries()==0 && !isttbar_controlplots)
              DrawStackError(LogStack_herwig,VariablesTitle.at(iVar),histos[iCut][iVar][iSampleData],SystematicErrorMap_herwig,true,false);
            else if(histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && histo_WJets[iCut][iVar]->GetEntries()!=0 && !isttbar_controlplots)
	      DrawDoubleStackError(LogStack,LogStack_herwig,VariablesTitle.at(iVar),SystematicErrorMap,SystematicErrorMap_herwig,true,false);
    	    else if (histo_ttbar_herwig[iCut][iVar]->GetEntries()!=0 && histo_ttbar[iCut][iVar]->GetEntries()!=0 && isttbar_controlplots)
	      DrawDoubleStackError(LogStack,LogStack_herwig,VariablesTitle.at(iVar),SystematicErrorMap,SystematicErrorMap_herwig,true,false,isttbar_controlplots);          
	    
 	    upperPadLogNoRatio->cd(); 

            if(histo_WJets_herwig[iCut][iVar]->GetEntries()==0 && histo_WJets[iCut][iVar]->GetEntries()!=0 && !isttbar_controlplots)
               DrawStackError(LogStackNoRatio,VariablesTitle.at(iVar),histos[iCut][iVar][iSampleData],SystematicErrorMap,true,true);
	    else if (histo_WJets[iCut][iVar]->GetEntries()==0 && histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && !isttbar_controlplots)
               DrawStackError(LogStackNoRatio_herwig,VariablesTitle.at(iVar),histos[iCut][iVar][iSampleData],SystematicErrorMap_herwig,true,true);
            else if(histo_WJets[iCut][iVar]->GetEntries()!=0 && histo_WJets_herwig[iCut][iVar]->GetEntries()!=0 && !isttbar_controlplots)
	      DrawDoubleStackError(LogStackNoRatio,LogStackNoRatio_herwig,VariablesTitle.at(iVar),SystematicErrorMap,SystematicErrorMap_herwig,true,true);
            else if (histo_ttbar_herwig[iCut][iVar]->GetEntries()!=0 && histo_ttbar[iCut][iVar]->GetEntries()!=0 && isttbar_controlplots)
	      DrawDoubleStackError(LogStackNoRatio,LogStackNoRatio_herwig,VariablesTitle.at(iVar),SystematicErrorMap,SystematicErrorMap_herwig,true,true,isttbar_controlplots);                 

            if((VariablesBlindedMinValue.at(iVar) != -999. && VariablesBlindedMaxValue.at(iVar) != -999.) && VariablesBlindedMinValue.at(iVar) != VariablesBlindedMaxValue.at(iVar)){
	     
                  for(int iBin = histos[iCut][iVar][iSampleData]->FindBin(VariablesBlindedMinValue.at(iVar)) ; 
                      iBin< histos[iCut][iVar][iSampleData]->FindBin(VariablesBlindedMaxValue.at(iVar)) ; iBin++) histos[iCut][iVar][iSampleData]->SetBinContent(iBin,-1.);

		  upperPadLog->cd(); histos[iCut][iVar][iSampleData]->DrawCopy("E same"); histos[iCut][iVar][iSampleData]->DrawCopy("E axissame");
		  upperPadLogNoRatio->cd(); histos[iCut][iVar][iSampleData]->DrawCopy("E same"); histos[iCut][iVar][iSampleData]->DrawCopy("E axissame");
                
            }
            else{ upperPadLog->cd(); histos[iCut][iVar][iSampleData]->DrawCopy("E same"); histos[iCut][iVar][iSampleData]->DrawCopy("E axissame"); 
                  upperPadLogNoRatio->cd(); histos[iCut][iVar][iSampleData]->DrawCopy("E same"); histos[iCut][iVar][iSampleData]->DrawCopy("E axissame"); }
          }
	 
	  if(SignalggHName!="NULL" && iSampleggH!=-1){ upperPadLog->cd() ; histos[iCut][iVar][iSampleggH]->DrawCopy("hist same"); histos[iCut][iVar][iSampleggH]->DrawCopy("hist axissame");
	                      if(!WithoutData) { upperPadLogNoRatio->cd(); histos[iCut][iVar][iSampleggH]->DrawCopy("hist same"); histos[iCut][iVar][iSampleggH]->DrawCopy("hist axissame"); }
	  }
	  if(SignalqqHName!="NULL" && iSampleqqH!=-1){ upperPadLog->cd(); histos[iCut][iVar][iSampleqqH]->DrawCopy("hist same"); histos[iCut][iVar][iSampleqqH]->DrawCopy("hist axissame");
	                     if(!WithoutData) { upperPadLogNoRatio->cd(); histos[iCut][iVar][iSampleqqH]->DrawCopy("hist same"); histos[iCut][iVar][iSampleqqH]->DrawCopy("hist axissame"); }
	  }
	  if(SignalRSGPythiaName!="NULL" && iSampleRSGPythia!=-1){ upperPadLog->cd(); histos[iCut][iVar][iSampleRSGPythia]->DrawCopy("hist same"); 
                                                                   histos[iCut][iVar][iSampleRSGPythia]->DrawCopy("hist axissame");
	     if(!WithoutData) { upperPadLogNoRatio->cd(); histos[iCut][iVar][iSampleRSGPythia]->DrawCopy("hist same"); histos[iCut][iVar][iSampleRSGPythia]->DrawCopy("hist axissame"); }
	  }
	  if(SignalRSGHerwigName!="NULL" && iSampleRSGHerwig!=-1){ upperPadLog->cd(); histos[iCut][iVar][iSampleRSGHerwig]->DrawCopy("hist same"); 
                                                                   histos[iCut][iVar][iSampleRSGHerwig]->DrawCopy("hist axissame");
	     if(!WithoutData) { upperPadLogNoRatio->cd(); histos[iCut][iVar][iSampleRSGHerwig]->DrawCopy("hist same"); histos[iCut][iVar][iSampleRSGHerwig]->DrawCopy("hist axissame"); }
	  }
	  if(SignalGravitonName!="NULL" && iSampleGraviton!=-1){ upperPadLog->cd(); histos[iCut][iVar][iSampleGraviton]->DrawCopy("hist same"); 
                                                                 histos[iCut][iVar][iSampleGraviton]->DrawCopy("hist axissame");
	     if(!WithoutData) { upperPadLogNoRatio->cd(); histos[iCut][iVar][iSampleGraviton]->DrawCopy("hist same"); histos[iCut][iVar][iSampleGraviton]->DrawCopy("hist axissame"); }
          }
	  
	  if(!WithoutData) { upperPadLog->cd(); leg[iCut][iVar]->Draw("same"); LatexCMS(Lumi,LeptonType,false); banner4Plot(false);
                             upperPadLogNoRatio->cd(); legNoRatio[iCut][iVar]->Draw("same"); LatexCMS(Lumi,LeptonType,true); banner4Plot(true);}
          else { upperPadLog->cd(); leg[iCut][iVar]->Draw("same"); LatexCMS(Lumi,LeptonType,true); banner4Plot(true); } 

          upperPadLog->Update(); 
          cLog[iCut][iVar]->Update(); 
	  cLog[iCut][iVar]->Write();
         
          std::string canvasnameLog = CanvasNameLog.Data() ;
	  std::replace(canvasnameLog.begin(),canvasnameLog.end(),'[','_');
	  std::replace(canvasnameLog.begin(),canvasnameLog.end(),']','_');
          std::replace(canvasnameLog.begin(),canvasnameLog.end(),'(','_');
          std::replace(canvasnameLog.begin(),canvasnameLog.end(),')','_');
	           
	  cLog[iCut][iVar]->Print( (OutputPlotDirectory+"/"+canvasnameLog+".pdf").c_str(),"pdf");
	  cLog[iCut][iVar]->Print( (OutputPlotDirectory+"/"+canvasnameLog+".png").c_str(),"png");
	  //	  cLog[iCut][iVar]->Print( (OutputPlotDirectory+"/"+canvasnameLog+".root").c_str(),"root");
	  //	  cLog[iCut][iVar]->Print( (OutputPlotDirectory+"/"+canvasnameLog+".cxx").c_str(),"cxx");
	  
          cLog[iCut][iVar]->Close();
	  if(!WithoutData) {

            upperPadLogNoRatio->Update();
            cLogNoRatio[iCut][iVar]->Update();
            cLogNoRatio[iCut][iVar]->Write();
            std::string canvasnameLogNoRatio = CanvasNameLogNoRatio.Data() ;
   	    std::replace(canvasnameLogNoRatio.begin(),canvasnameLogNoRatio.end(),'[','_');
	    std::replace(canvasnameLogNoRatio.begin(),canvasnameLogNoRatio.end(),']','_');
	    std::replace(canvasnameLogNoRatio.begin(),canvasnameLogNoRatio.end(),'(','_');
 	    std::replace(canvasnameLogNoRatio.begin(),canvasnameLogNoRatio.end(),')','_');
          
	    cLogNoRatio[iCut][iVar]->Print( (OutputPlotDirectory+"/"+canvasnameLogNoRatio+".pdf").c_str(),"pdf");
	    cLogNoRatio[iCut][iVar]->Print( (OutputPlotDirectory+"/"+canvasnameLogNoRatio+".png").c_str(),"png");
	    //	    cLogNoRatio[iCut][iVar]->Print( (OutputPlotDirectory+"/"+canvasnameLogNoRatio+".root").c_str(),"root");
	    //	    cLogNoRatio[iCut][iVar]->Print( (OutputPlotDirectory+"/"+canvasnameLogNoRatio+".cxx").c_str(),"cxx");
	    
            cLogNoRatio[iCut][iVar]->Close();
	  }

          if(!StackNoRatio)       delete StackNoRatio ; 
          if(!LogStack)           delete LogStack ;
          if(!LogStackNoRatio)    delete LogStackNoRatio ; 
          if(!upperPad)           delete upperPad ;
          if(!lowerPad)           delete lowerPad ;
          if(!upperPadLog)        delete upperPadLog ;
          if(!lowerPadLog)        delete lowerPadLog ;
          if(!upperPadNoRatio)    delete upperPadNoRatio ;
          if(!upperPadLogNoRatio) delete upperPadLogNoRatio ;
          if(!MCSysStat)          delete MCSysStat ;
          if(!legRatio)           delete legRatio;}
     
     
	  
    //Print Signal/Background ratios for every cut
    int iVar = 0 ;

    std::cout<<std::endl;
    std::cout<<" ##################### "<<std::endl;
    std::cout<< " Event Scaled To Lumi "<<std::endl;
    std::cout<<" ##################### "<<std::endl;
   
    std::cout<<std::endl;
    std::cout<<" Signal to Background ratios: Cut_"<<int(iCut)<<"  String: "<<CutList.at(iCut)<<std::endl;
    std::cout<<" Data Events:            "<<histos[iCut][iVar][iSampleData]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
    std::cout<<" WJets Pythia  Events:           "<<histo_WJets[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
    std::cout<<" WJets Herwig  Events:           "<<histo_WJets_herwig[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
    std::cout<<" TTbar Powegh Events:           "<<histo_ttbar[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
    std::cout<<" TTbar mc@nlo Events:           "<<histo_ttbar_herwig[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
    std::cout<<" VV Events:              "<<histo_diboson[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
    std::cout<<" STop Events:            "<<histo_top[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
    std::cout<<" All Backgrounds Events Pythia: "<<histo_WJets[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)
      +histo_ttbar[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)
      +histo_top[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)
      +histo_diboson[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;

    if(histo_ttbar_herwig[iCut][iVar]->GetEntries()!=0){
     std::cout<<" All Backgrounds Events Herwig: "<<histo_WJets_herwig[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)
      +histo_ttbar_herwig[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)
      +histo_top[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)
       +histo_diboson[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;}
    else{
     std::cout<<" All Backgrounds Events Herwig: "<<histo_WJets_herwig[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)
      +histo_ttbar[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)
      +histo_top[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)
      +histo_diboson[iCut][iVar]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;
    }
    if(SignalggHName!="NULL" && iSampleggH!=-1){
      if(!NormalizeSignalToData) histos[iCut][iVar][iSampleggH]->Scale(1./SignalScaleFactor);
      else histos[iCut][iVar][iSampleggH]->Scale(1./histos[iCut][iVar][iSampleData]->Integral());     
      std::cout<<" Signal ggH:             "<<histos[iCut][iVar][iSampleggH]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;

    }

    if(SignalqqHName!="NULL" && iSampleqqH!=-1){
      if(!NormalizeSignalToData) histos[iCut][iVar][iSampleqqH]->Scale(1./SignalScaleFactor);
      else histos[iCut][iVar][iSampleqqH]->Scale(1./histos[iCut][iVar][iSampleData]->Integral());
      std::cout<<" Signal qqH:             "<<histos[iCut][iVar][iSampleqqH]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;

    }

    if(SignalRSGPythiaName!="NULL" && iSampleRSGPythia!=-1){
      if(!NormalizeSignalToData) histos[iCut][iVar][iSampleRSGPythia]->Scale(1./SignalScaleFactor);
      else histos[iCut][iVar][iSampleRSGPythia]->Scale(1./histos[iCut][iVar][iSampleData]->Integral());
      std::cout<<" Signal RSGPythia:       "<<histos[iCut][iVar][iSampleRSGPythia]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;

    }

    if(SignalRSGHerwigName!="NULL" && iSampleRSGHerwig!=-1){
      if(!NormalizeSignalToData) histos[iCut][iVar][iSampleRSGHerwig]->Scale(1./SignalScaleFactor);
      else histos[iCut][iVar][iSampleRSGHerwig]->Scale(1./histos[iCut][iVar][iSampleData]->Integral());
      std::cout<<" Signal RSGHerwig:       "<<histos[iCut][iVar][iSampleRSGHerwig]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;

    }

    if(SignalGravitonName!="NULL" && iSampleGraviton!=-1){
      if(!NormalizeSignalToData) histos[iCut][iVar][iSampleGraviton]->Scale(1./SignalScaleFactor);
      else histos[iCut][iVar][iSampleGraviton]->Scale(1./histos[iCut][iVar][iSampleData]->Integral());
      std::cout<<" Signal Graviton:        "<<histos[iCut][iVar][iSampleGraviton]->Integral(0, VariablesNbin.at(iVar)+1)<<std::endl;

    }
     
 }

 outputFile->Close();
  
 return 0 ;
}
